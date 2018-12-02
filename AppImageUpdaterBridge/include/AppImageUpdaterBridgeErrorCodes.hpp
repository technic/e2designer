#ifndef APPIMAGE_UPDATER_BRIDGE_ERROR_CODES_HPP_INCLUDED
#define APPIMAGE_UPDATER_BRIDGE_ERROR_CODES_HPP_INCLUDED

namespace AppImageUpdaterBridge
{
enum : short {
    NO_ERROR = 0,
    APPIMAGE_NOT_READABLE = 1, //  50 < and > 0 , AppImage Update Information Error.
    NO_READ_PERMISSION,
    APPIMAGE_NOT_FOUND,
    CANNOT_OPEN_APPIMAGE,
    EMPTY_UPDATE_INFORMATION,
    INVALID_APPIMAGE_TYPE,
    INVALID_MAGIC_BYTES,
    INVALID_UPDATE_INFORMATION,
    NOT_ENOUGH_MEMORY,
    SECTION_HEADER_NOT_FOUND,
    UNSUPPORTED_ELF_FORMAT,
    UNSUPPORTED_TRANSPORT,
    UNKNOWN_NETWORK_ERROR = 50, // >= 50 , Zsync Control File Parser Error.
    IO_READ_ERROR,
    ERROR_RESPONSE_CODE,
    GITHUB_API_RATE_LIMIT_REACHED,
    NO_MARKER_FOUND_IN_CONTROL_FILE,
    INVALID_ZSYNC_HEADERS_NUMBER,
    INVALID_ZSYNC_MAKE_VERSION,
    INVALID_ZSYNC_TARGET_FILENAME,
    INVALID_ZSYNC_MTIME,
    INVALID_ZSYNC_BLOCKSIZE,
    INVALID_TARGET_FILE_LENGTH,
    INVALID_HASH_LENGTH_LINE,
    INVALID_HASH_LENGTHS,
    INVALID_TARGET_FILE_URL,
    INVALID_TARGET_FILE_SHA1,
    HASH_TABLE_NOT_ALLOCATED = 100, // >= 100 , Zsync Writer error.
    INVALID_TARGET_FILE_CHECKSUM_BLOCKS,
    CANNOT_OPEN_TARGET_FILE_CHECKSUM_BLOCKS,
    CANNOT_CONSTRUCT_HASH_TABLE,
    QBUFFER_IO_READ_ERROR,
    SOURCE_FILE_NOT_FOUND,
    NO_PERMISSION_TO_READ_SOURCE_FILE,
    CANNOT_OPEN_SOURCE_FILE,
    NO_PERMISSION_TO_READ_WRITE_TARGET_FILE,
    CANNOT_OPEN_TARGET_FILE,
    TARGET_FILE_SHA1_HASH_MISMATCH
};
}
#endif // APPIMAGE_UPDATER_BRIDGE_ERROR_CODES_HPP_INCLUDED
