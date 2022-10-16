/* Martin Franceschi - MyWorks package - File module.
 *
 * This module gives various information about a given file.
 * You can check if it exists, check its size and encoding.
 * You can also open it by applying automatically a locale
 * (UTF-8 and UTF-16LE only).
 * You are in charge of providing the right C-string filename (wide or not).
 */

//---------- Interface of module <File> (file File.h)
#if !defined(FILE_H)
#    define FILE_H

//--------------------------------------------------------------- Includes
#    include <fstream>
#    include <memory>
#    include <string>
#    include <vector>

namespace MF
{
    namespace Filesystem
    {
        // Represents a file encoding.
        // It is an implementation detail that may change. Please use "..._t" instead.
        enum class Encoding_e {
            ENC_UTF16LE, // Normal UTF-16LE
            ENC_UTF8, // Normal UTF-8
            ENC_DEFAULT // If no encoding is false, we assume the default locale.
        };

        // Represents a file encoding.
        using Encoding_t = Encoding_e;

        // Type used to deal with file sizes.
        using Filesize_t = unsigned long;

        using Filename_t = std::string;
        using WideFilename_t = std::wstring;

        extern const Filename_t FILE_SEPARATOR;
        extern const WideFilename_t FILE_SEPARATOR_WIDE;
        extern const Filename_t LINE_END;
        extern const WideFilename_t LINE_END_WIDE;

        //////////////////////////////////////////////////////////////////  PUBLIC
        //------------------------------------------------------- Public functions

        void deleteFile(const Filename_t &filename);
        void deleteFile(const WideFilename_t &filename);

        void deleteDirectory(const Filename_t &filename);
        void deleteDirectory(const WideFilename_t &filename);

        bool isFile(const Filename_t &filename);
        bool isFile(const WideFilename_t &filename);

        bool isDir(const Filename_t &filename);
        bool isDir(const WideFilename_t &filename);

        Filesize_t getFileSize(const Filename_t &filename);
        Filesize_t getFileSize(const WideFilename_t &filename);

        Encoding_t getFileEncoding(const Filename_t &filename);
        Encoding_t getFileEncoding(const WideFilename_t &filename);

        /**
         * Opens the file with the correct locale (regarding the encoding) and skipping the
         * corresponding Byte Order Marks.
         */
        std::unique_ptr<std::ifstream> openFile(const Filename_t &filename);
        std::unique_ptr<std::ifstream> openFile(const Filename_t &filename, Encoding_t encoding);
        std::unique_ptr<std::wifstream> openFile(const WideFilename_t &filename);
        std::unique_ptr<std::wifstream> openFile(
            const WideFilename_t &filename, Encoding_t encoding);

        void createDirectory(const Filename_t &filename);
        void createDirectory(const WideFilename_t &filename);

        Filename_t getCWD();
        WideFilename_t getCWDWide();

        /**
         * Generates the complete list of files and directories that are direct children of the
         * given folder. Names are returned relative to the "folder". Directories have an ending
         * PATH_SEPARATOR.
         * > listFilesInDirectory("myFolder/") -> ("file.txt", "image.png", "subfolder/")
         * @param folder Name or path to the folder. It must end with a PATH_SEPARATOR character.
         * @return List of files and directories names, or empty vector if anything failed.
         */
        std::vector<Filename_t> listFilesInDirectory(const Filename_t &folder);
        std::vector<WideFilename_t> listFilesInDirectory(const WideFilename_t &folder);

        /// Data structure used to store information about files opened with openFile.
        class WholeFileData {
           public:
            const char *&getContent() {
                return content;
            }

            Filesize_t getSize() const {
                return filesize;
            }

            virtual ~WholeFileData() = default; // For polymorphic reasons.
           protected:
            WholeFileData() = delete;
            WholeFileData(const char *content, Filesize_t size) : content(content), filesize(size) {
            }

           private:
            const char *content;
            Filesize_t filesize;
        };

        /**
         * Stores the entire contents of file "filename" in a read-only C-string.
         * Also, that C-string does not end with '\0'.
         * It is advised to use an "InCharArrayStream".
         * The structure must remain a pointer (in reality, it is an instance of a subclass of
         * WholeFileData). The purpose of this function is to offer the fastest way to read an
         * entire file.
         * @param filename Name of the file to open.
         * @return "nullptr" if anything failed or the file is empty, or a new structure.
         */
        std::unique_ptr<const WholeFileData> readWholeFile(const Filename_t &filename);
        std::unique_ptr<const WholeFileData> readWholeFile(const WideFilename_t &filename);
    } // namespace Filesystem
} // namespace MF
#endif // FILE_H
