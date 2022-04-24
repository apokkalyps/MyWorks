//
// Created by Utilisateur on 27/03/2022.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_FILESYSTEM_TESTS_COMMONS_HPP
#define MFRANCESCHI_CPPLIBRARIES_FILESYSTEM_TESTS_COMMONS_HPP

#include "MF/Filesystem.hpp"
#include "tests_data.hpp"

/*
#if defined(UNICODE)
#    define MIDDLE_SIZE_RAW L"aom_v.scx"
#    define NOT_EXISTING_RAW L"not_existing._tut"
#    define SMALL_UTF16LE_RAW L"Small_utf16le.txt"
#    define TEMP_RAW L"I_AM_TEMP"
#else
#    define MIDDLE_SIZE_RAW "aom_v.scx"
#    define NOT_EXISTING_RAW "not_existing._tut"
#    define SMALL_UTF16LE_RAW "Small_utf16le.txt"
#    define TEMP_RAW "I_AM_TEMP"
#endif
*/

// First settings : file names, (Win) memory leaks check.
#if 1

using namespace MF::Filesystem;

const SFilename_t FILENAME_MIDDLE_SIZE = MAKE_FILE_NAME MF_FILESYSTEM_TESTS_FILE_MEDIUM;
const SFilename_t FILENAME_NOT_EXISTING = MAKE_FILE_NAME MF_FILESYSTEM_TESTS_NON_EXISTING;
const SFilename_t FILENAME_SMALL_UTF16LE = MAKE_FILE_NAME MF_FILESYSTEM_TESTS_FILE_SMALL_UTF16LE;
const SFilename_t FILENAME_TEMP = MAKE_FILE_NAME MF_FILESYSTEM_TESTS_TEMP;

#endif

// Structure that holds file information.
struct file_info_data {
    Filesize_t size;
    SFilename_t name;
    char firstByte, lastByte;
    bool exists;
    Encoding_t encoding;
    size_t offset = 0;
};

static const file_info_data fid_middle_size{287815, FILENAME_MIDDLE_SIZE,   'l', '\xEF',
                                            true,   Encoding_e::ENC_DEFAULT};
static const file_info_data fid_not_existing{0,     FILENAME_NOT_EXISTING, 0, 0,
                                             false, Encoding_e::ENC_ERROR};
static const file_info_data fid_smallfile_utf16le{38,   FILENAME_SMALL_UTF16LE,  '\xFF', '\x00',
                                                  true, Encoding_e::ENC_UTF16LE, 2};

#endif // MFRANCESCHI_CPPLIBRARIES_FILESYSTEM_TESTS_COMMONS_HPP