//---------- Implementation of module <File> (file File.cpp) 

//--------------------------------------------------------------- Includes

#include <codecvt>
#include <fcntl.h>
#include "File.hpp"
#include <locale>
#include <map>
#include <mutex>
#include <sys/stat.h>

#if defined(_WIN32)
#   pragma warning( disable: 26444) // Warning that occurs when using imbue.
#   include <io.h>
#   include "Toolbox.hpp"
#   include <Windows.h>
#   include "WindowsAPIHelper.hpp"
#else
#   include <dirent.h>
#   include <sys/mman.h>
#   include <unistd.h>
#endif

using std::string; 
using std::locale; 
using std::ifstream; 
using std::ios_base;

constexpr static size_t NBR_BITS_TO_READ_ENCODING = 3;

namespace File
{
/////////////////////////////////////////////////////////////////  PRIVATE

//------------------------------------------------------------------ Types

//-------------------------------------------------------------- Constants

	const static locale LOCALE_UTF8("");
	const static locale LOCALE_UTF16LE(
		locale(""),
		new std::codecvt_utf8_utf16<wchar_t, 0x10ffffUL, std::little_endian>()
	); // I can call "new" because the locale's destructors deletes the facet.

	static std::map<const char*, ReadFileData>openedFiles;
	static std::mutex openedFilesMutex;

//------------------------------------------------------- Static variables

//------------------------------------------------------ Private functions

    // Returns true on success.
	static inline bool open_file(File::filename_t filename, int& fd) { 
	#ifdef _WIN32
			#ifdef UNICODE
				#define WIN_OPEN_FCT _wsopen_s
			#else
				#define WIN_OPEN_FCT _sopen_s
			#endif
			return !WIN_OPEN_FCT(&fd, filename, _O_RDONLY | O_BINARY, _SH_DENYWR, _S_IREAD);
			#undef WIN_OPEN_FCT
		#else
			fd = open(filename, O_RDONLY);
			return fd != -1;
		#endif
	}

	/* LOW-LEVEL FILE HANDLING */
#ifdef _WIN32 // Windows
	static inline HANDLE OpenHandleWindows(filename_t filename) {
		return CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	}
	static inline bool IsADirFromAttributesWindows(DWORD dword) {
	    return dword & FILE_ATTRIBUTE_DIRECTORY;
	}
#else
	#define _read read /* POSIX form */
	#define _close close /* POSIX form */
#endif

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions

	bool Delete(filename_t filename, bool fileOnly)
	{
#ifdef _WIN32 // Win32
		if (fileOnly)
			return _WindowsDeleteFile(filename);
		else
			return _WindowsDeleteFile(filename) ? true : _WindowsDeleteDirectory(filename);
#else // POSIX
		if (fileOnly)
			return !unlink(filename);
		else
			return !remove(filename);
#endif
	}

	bool Exists(filename_t filename)
	{
#ifdef _WIN32 // Win32
	    return _WindowsFileExists(filename);
#else // POSIX
		struct stat t{};
		return !stat(filename, &t);
#endif
	}

	bool IsDir(filename_t filename)
	{
#ifdef _WIN32 // Win32
		return _WindowsDirectoryExists(filename);
#else // POSIX
		struct stat s{};
		return !stat(filename, &s) & S_ISDIR(s.st_mode);
#endif
	}

	bool CanReadFile(filename_t filename, int charsToRead)
	{
	    // TODO fix
		int file;
		bool forReturn = false;

		if (!open_file(filename, file))
			forReturn = true;
		else
		{
		    if (charsToRead >= 0) {
                char* content = new char[charsToRead];
                int chars_just_read = _read(file, content, charsToRead);
                forReturn = chars_just_read != charsToRead;
                delete[] content;
		    }
			_close(file);
		}

		return forReturn;
	}

	bool Open(ifstream& ifs, filename_t filename,
		encoding_t encoding)
	{
		ifs.close();
		if (encoding == encoding_t::ENC_ERROR)
			encoding = File::Encoding(filename);

		if (encoding == encoding_t::ENC_UTF8)
		{
			ifs.open(filename);
			ifs.imbue(LOCALE_UTF8);
			ifs.seekg(3);
			return true;
		}
		else if (encoding == encoding_t::ENC_UTF16LE)
		{
			ifs.open(filename, ios_base::binary);
			ifs.imbue(LOCALE_UTF16LE);
			ifs.seekg(2, ios_base::beg);
			return true;
		}
		else if (encoding == encoding_t::ENC_DEFAULT)
		{
			ifs.open(filename, ios_base::binary);
			return true;
		}
		else // Encoding is unknown
			return false;
	}

	file_size_t Size(filename_t filename)
	{
#ifdef _WIN32 // Win32
		return _WindowsGetFileSize(filename);
#else // POSIX
		struct stat t{};
		if (stat(filename, &t)) return 0;
		return file_size_t(t.st_size);
#endif
	}

	encoding_t Encoding(filename_t filename)
	{
	    // TODO fix
		int file;
		encoding_t forReturn;

		if (!open_file(filename, file))
			forReturn = encoding_t::ENC_ERROR;
		else
		{
			char bits[NBR_BITS_TO_READ_ENCODING];
			int ret_read = _read(file, bits, NBR_BITS_TO_READ_ENCODING);
			
			if (ret_read != NBR_BITS_TO_READ_ENCODING)
				forReturn = encoding_t::ENC_ERROR;
			else if (bits[0] == '\xff' && bits[1] == '\xfe')
				forReturn = encoding_t::ENC_UTF16LE;
			else if (bits[0] == '\xef' && bits[1] == '\xbb' && bits[2] == '\xbf')
				forReturn = encoding_t::ENC_UTF8;
			else
				forReturn = encoding_t::ENC_DEFAULT;

			_close(file);
		}
		return forReturn;
	}

	bool CreateFolder(filename_t filename)
	{
#ifdef _WIN32
		return _WindowsCreateDirectory(filename);
#else
		return !mkdir(filename, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
	}

	const char* Read(filename_t filename)
	{
#ifdef _WIN32
		return _WindowsOpenFile(filename)->contents;
#else
		if (!open_file(filename, rfd.fd))
			return nullptr;
		rfd.memoryPointer = (const char*)mmap(nullptr, rfd.size, PROT_READ, MAP_PRIVATE, rfd.fd, 0);
		if (rfd.memoryPointer == (void*)-1)
		{
			_close(rfd.fd);
			return nullptr;
		}
#endif
	}

	bool Read_Close(const char* content)
	{
		openedFilesMutex.lock();
		bool found = false;

		auto iterToContent = openedFiles.find(content);
		if (iterToContent != openedFiles.end())
		{
			found = true;
			// File is found, release its data.
			ReadFileData& rfd = iterToContent->second;
#ifdef _WIN32
			UnmapViewOfFile(content);
			CloseHandle(rfd.mappingHandle);
			CloseHandle(rfd.fileHandle);
#else
			munmap((void*)rfd.memoryPointer, rfd.size);
			_close(rfd.fd);
#endif
			openedFiles.erase(iterToContent);
		}

		openedFilesMutex.unlock();
		return found;
	}

	std::ostream& operator<< (std::ostream& os, const encoding_t& enc)
	{
		switch (enc)
		{
		case encoding_t::ENC_UTF16LE:
			os << "UTF-16LE";
			break;
		case encoding_t::ENC_UTF8:
			os << "UTF-8";
			break;
		case encoding_t::ENC_ERROR:
			os << "<encoding-error>";
			break;
		case encoding_t::ENC_DEFAULT:
			os << "<encoding-unknown>";
			break;
		}
		return os;
	}

	str_filename_t GetCWD()
	{
#ifdef _WIN32
        return _WindowsGetCurrentWorkingDirectory();
#else
        File::filename_t result_of_syscall;
        result_of_syscall = getcwd(nullptr, 0);
        str_filename_t to_return = result_of_syscall;
		free((void *) result_of_syscall);
		return to_return;
#endif
	}


    std::vector<File::str_filename_t> FilesInDirectory(filename_t folder) {
        std::vector<File::str_filename_t> result;
#if defined(_WIN32)
        _WindowsGetDirectoryContents(folder, result);
#else
        File::str_filename_t tempFilename;
        static File::filename_t CURRENT_FOLDER = MAKE_FILE_NAME ".";
        static File::filename_t PARENT_FOLDER = MAKE_FILE_NAME "..";
        DIR * d;
        dirent * dir_entry;
                d = opendir(folder);
        if (d) {
            while ((dir_entry = readdir(d)) != nullptr) {
                tempFilename = dir_entry->d_name;
                if (tempFilename == CURRENT_FOLDER || tempFilename == PARENT_FOLDER) {
                    continue;
                } else if (File::IsDir((std::string(folder) + tempFilename).c_str())) {
                    tempFilename.append(FILE_SEPARATOR);
                }
                result.emplace_back(tempFilename);
            }
            closedir(d);
        }
#endif
        return result;
    }
}
