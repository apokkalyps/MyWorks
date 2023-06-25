//
// Created by MartinF on 15/10/2022.
//

#if MF_WINDOWS

#    include <cassert>
#    include <iostream>
#    include <mutex>

#    include "MF/Commons.hpp"
#    include "MF/SystemErrors.hpp"
#    include "MF/Volumes.hpp"
#    include "Volumes_internal.hpp"

using namespace MF::Volumes;

namespace MF
{
    namespace Volumes
    {
        template <typename T>
        struct ProviderWithSimpleConstructor : MF::Commons::NoCopy, MF::Commons::NoMove {
           public:
            ProviderWithSimpleConstructor() = default;
            ~ProviderWithSimpleConstructor() override = default;

            template <typename... ConstrParams>
            T& get(const ConstrParams&... constrParams) {
                std::call_once(
                    onceFlag,
                    [this](const ConstrParams&... constrParams1) {
                        store = std::make_unique<T>(constrParams1...);
                    },
                    constrParams...);
                return *store;
            }

           private:
            std::once_flag onceFlag{};
            std::unique_ptr<T> store;
        };

        struct VolumeInfo_Windows : Volume {
            VolumeInfo_Windows(
                const std::wstring& volumeGuid, const std::vector<std::wstring>& paths)
                : volumeGuid(volumeGuid), paths(paths), rootPath(this->paths[0]) {
            }

           private:
            const std::wstring volumeGuid;
            const std::vector<std::wstring> paths;
            const std::wstring& rootPath;

           public:
            bool isMounted() override {
                return true;
            }

            OptionalPtr<Filesize_t> getBytesPerSector() override {
                return Optionals::ofLvalue(diskSpaceInfo.get(rootPath).getBytesPerSector());
            }

            OptionalPtr<std::uint16_t> getSectorsPerAllocationUnit() override {
                return Optionals::ofLvalue(
                    diskSpaceInfo.get(rootPath).getSectorsPerAllocationUnit());
            }

            OptionalPtr<Filesize_t> getTotalSize() override {
                return Optionals::ofLvalue(diskSpaceInfo.get(rootPath).getTotalSize());
            }

            OptionalPtr<Filesize_t> getFreeSize() override {
                return Optionals::ofLvalue(diskSpaceInfo.get(rootPath).getFreeSize());
            }

            OptionalPtr<Filesize_t> getUsedSize() override {
                return Optionals::ofLvalue(diskSpaceInfo.get(rootPath).getUsedSize());
            }

            OptionalBool isRemovableDrive() override {
                return OptionalBool::of(driveType.get(rootPath).isRemovableDrive());
            }

            OptionalBool isNotRemovableDrive() override {
                return OptionalBool::of(driveType.get(rootPath).isNotRemovableDrive());
            }

            OptionalBool isCdRomDrive() override {
                return OptionalBool::of(driveType.get(rootPath).isCdRomDrive());
            }

            OptionalBool isRemoteDrive() override {
                return OptionalBool::of(driveType.get(rootPath).isRemoteDrive());
            }

            OptionalBool isRamDisk() override {
                return OptionalBool::of(driveType.get(rootPath).isRamDisk());
            }

            Filename_t getName() override {
                return volumeInformation.get(rootPath).getName();
            }

            WideFilename_t getNameWide() override {
                return volumeInformation.get(rootPath).getNameWide();
            }

            Filename_t getFileSystemName() override {
                return volumeInformation.get(rootPath).getFileSystemName();
            }

            WideFilename_t getFileSystemNameWide() override {
                return volumeInformation.get(rootPath).getFileSystemNameWide();
            }

            OptionalBool isReadOnly() override {
                return OptionalBool::of(volumeInformation.get(rootPath).isReadOnly());
            }

            OptionalBool hasUnicodeSupportForFileNames() override {
                return OptionalBool::of(
                    volumeInformation.get(rootPath).hasUnicodeSupportForFileNames());
            }

            OptionalBool hasFileBasedCompressionSupport() override {
                return OptionalBool::of(volumeInformation.get(rootPath).hasCompressionSupport());
            }

            OptionalBool hasCaseSensitiveFileNamesSupport() override {
                return OptionalBool::of(
                    volumeInformation.get(rootPath).hasCaseSensitiveFileNamesSupport());
            }

            Filename_t getSystemIdentifier() override {
                return MF::Strings::Conversions::wideCharToUtf8(volumeGuid);
            }

            WideFilename_t getSystemIdentifierWide() override {
                return volumeGuid;
            }

            std::vector<Filename_t> getMountPoints() override {
                std::vector<Filename_t> mountPoints;
                mountPoints.reserve(paths.size());
                for (const std::wstring& path : paths) {
                    mountPoints.push_back(MF::Strings::Conversions::wideCharToUtf8(path));
                }
                return mountPoints;
            }

            std::vector<WideFilename_t> getMountPointsWide() override {
                return paths;
            }

           private:
            ProviderWithSimpleConstructor<GetDiskSpaceInfo_Windows> diskSpaceInfo;
            ProviderWithSimpleConstructor<GetDriveType_Windows> driveType;
            ProviderWithSimpleConstructor<GetVolumeInformation_Windows> volumeInformation;
        };

        struct NotMountedVolumeInfo_Windows : Volume {
            NotMountedVolumeInfo_Windows(const std::wstring& volumeGuid) : volumeGuid(volumeGuid) {
            }

           private:
            [[noreturn]] void throwNotMounted() {
                throw NotMountedException(volumeGuid);
            }

            const std::wstring volumeGuid;

           public:
            bool isMounted() override {
                return false;
            }

            OptionalPtr<Filesize_t> getBytesPerSector() override {
                return Optionals::empty<Filesize_t>();
            }

            OptionalPtr<std::uint16_t> getSectorsPerAllocationUnit() override {
                return Optionals::empty<std::uint16_t>();
            }

            OptionalPtr<Filesize_t> getTotalSize() override {
                return Optionals::empty<Filesize_t>();
            }

            OptionalPtr<Filesize_t> getFreeSize() override {
                return Optionals::empty<Filesize_t>();
            }

            OptionalPtr<Filesize_t> getUsedSize() override {
                return Optionals::empty<Filesize_t>();
            }

            OptionalBool isRemovableDrive() override {
                return OptionalBool::empty();
            }

            OptionalBool isNotRemovableDrive() override {
                return OptionalBool::empty();
            }

            OptionalBool isCdRomDrive() override {
                return OptionalBool::empty();
            }

            OptionalBool isRemoteDrive() override {
                return OptionalBool::empty();
            }

            OptionalBool isRamDisk() override {
                return OptionalBool::empty();
            }

            Filename_t getName() override {
                throwNotMounted();
            }

            Filename_t getFileSystemName() override {
                throwNotMounted();
            }

            WideFilename_t getNameWide() override {
                throwNotMounted();
            }

            WideFilename_t getFileSystemNameWide() override {
                throwNotMounted();
            }

            OptionalBool isReadOnly() override {
                return OptionalBool::empty();
            }

            OptionalBool hasUnicodeSupportForFileNames() override {
                return OptionalBool::empty();
            }

            OptionalBool hasFileBasedCompressionSupport() override {
                return OptionalBool::empty();
            }

            OptionalBool hasCaseSensitiveFileNamesSupport() override {
                return OptionalBool::empty();
            }

            Filename_t getSystemIdentifier() override {
                return MF::Strings::Conversions::wideCharToUtf8(volumeGuid);
            }

            WideFilename_t getSystemIdentifierWide() override {
                return volumeGuid;
            }

            std::vector<Filename_t> getMountPoints() override {
                return {};
            }

            std::vector<WideFilename_t> getMountPointsWide() override {
                return {};
            }
        };

        std::vector<std::unique_ptr<Volume>> listAll() {
            const std::vector<std::wstring> volumeGuids = enumerateVolumeGuids();
            std::vector<std::unique_ptr<Volume>> volumes;
            volumes.reserve(volumeGuids.size());
            for (const std::wstring& guid : volumeGuids) {
                const auto paths = getPathsForVolumeGuid(guid);
                if (paths.empty()) {
                    volumes.push_back(std::make_unique<NotMountedVolumeInfo_Windows>(guid));
                } else {
                    volumes.push_back(std::make_unique<VolumeInfo_Windows>(guid, paths));
                }
            }
            return volumes;
        }

        std::unique_ptr<Volume> getMaybeForMountPoint(const Filename_t& path) {
            const WideFilename_t wPath = Strings::Conversions::utf8ToWideChar(path);
            WideFilename_t guid;
            guid.resize(50);
            const BOOL volumeGuidFound =
                GetVolumeNameForVolumeMountPointW(wPath.data(), &(guid[0]), 50);
            SystemErrors::Win32::throwCurrentSystemErrorIf(volumeGuidFound == FALSE);

            const auto paths = getPathsForVolumeGuid(guid);
            assert(!paths.empty());
            return std::make_unique<VolumeInfo_Windows>(guid, paths);
        }
    } // namespace Volumes
} // namespace MF
#endif
