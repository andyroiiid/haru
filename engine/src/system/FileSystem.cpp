//
// Created by andyroiiid on 11/20/2022.
//

#include "haru/system/FileSystem.h"

#include <physfs.h>

#include "haru/core/Debug.h"

static const char *GetLastError() {
    return PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
}

FileSystem::FileSystem() {
    DebugCheckCritical(PHYSFS_init(__argv[0]), "Failed to init Physfs: %s", GetLastError());
    DebugLog("Physfs %d.%d.%d", PHYSFS_VER_MAJOR, PHYSFS_VER_MINOR, PHYSFS_VER_PATCH);
}

FileSystem::~FileSystem() {
    DebugCheckCritical(PHYSFS_deinit(), "Failed to destroy Physfs: %s", GetLastError());
}

FileSystem &FileSystem::Mount(const std::string &newDir, const std::string &mountPoint, bool appendToPath) {
    if (!PHYSFS_mount(newDir.c_str(), mountPoint.c_str(), appendToPath)) {
        DebugLog("Physfs failed to mount %s at %s: %s", newDir.c_str(), mountPoint.c_str(), GetLastError());
    } else {
        DebugLog("Physfs mount %s at %s", newDir.c_str(), mountPoint.c_str());
    }
    return *this;
}

std::string FileSystem::ReadFile(const std::string &filename) {
    PHYSFS_File *file = PHYSFS_openRead(filename.c_str());
    if (!file) {
        DebugLog("Physfs failed to open file %s: %s", filename.c_str(), GetLastError());
        return {};
    }
    PHYSFS_sint64 length = PHYSFS_fileLength(file);
    if (length == -1) {
        DebugLog("Physfs failed to get file size %s: %s", filename.c_str(), GetLastError());
        return {};
    }
    std::string bytes(length, '\0');
    if (PHYSFS_readBytes(file, bytes.data(), length) == -1) {
        DebugLog("Physfs failed to read file %s: %s", filename.c_str(), GetLastError());
    }
    return bytes;
}
