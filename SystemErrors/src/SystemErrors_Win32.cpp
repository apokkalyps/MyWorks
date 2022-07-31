//
// Created by MartinF on 31/07/2022.
//

//
// Created by MartinF on 03/07/2022.
//

#if MF_WINDOWS

#    include "MF/LightWindows.hpp"
#    include "MF/SystemErrors.hpp"
#    include "SystemErrors_WindowsCommons.hpp"

namespace MF
{
    namespace SystemErrors
    {
        namespace Win32
        {
            static_assert(std::is_same_v<DWORD, unsigned long>, "DWORD is not unsigned long!");

            ErrorCode_t getCurrentErrorCode() {
                return GetLastError();
            }

            void setCurrentErrorCode(ErrorCode_t value) {
                SetLastError(value);
            }

            std::system_error getSystemErrorForErrorCode(ErrorCode_t errorCode) {
                return WindowsCommons::getSystemErrorForErrorCode(errorCode);
            }
        } // namespace Win32
    } // namespace SystemErrors
} // namespace MF
#endif
