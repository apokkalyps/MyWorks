//
// Created by Utilisateur on 04/04/2023.
//

#include <iostream>

#include "MF/LightWindows.hpp"
#include "MF/SystemErrors.hpp"
#include "MF/Windows.hpp"
#include "Volumes_internal.hpp"

std::vector<std::wstring> enumerateVolumeGuids() {
    std::vector<std::wstring> result;

    std::wstring volumeGuidName;
    volumeGuidName.reserve(MAX_PATH);

    //  Enumerate all volumes in the system.
    MF::Windows::FindVolumeCloser findHandle =
        FindFirstVolumeW(&volumeGuidName[0], volumeGuidName.capacity());
    MF::SystemErrors::Win32::throwCurrentSystemErrorIf(findHandle.isInvalid());
    result.push_back(volumeGuidName.c_str());

    while (true) {
        auto res = FindNextVolumeW(findHandle.get(), &volumeGuidName[0], volumeGuidName.capacity());
        if (res == FALSE) {
            auto errorCode = MF::SystemErrors::Win32::getCurrentErrorCode();
            if (errorCode == ERROR_NO_MORE_FILES) {
                break;
            }

            throw MF::SystemErrors::Win32::getSystemErrorForErrorCode(errorCode);
        }
        result.push_back(volumeGuidName.c_str());
    }
    return result;
}
