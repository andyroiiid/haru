set(BUILD_SHARED_LIBS OFF)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw EXCLUDE_FROM_ALL)
set_msvc_runtime_library(glfw)

add_subdirectory(glm EXCLUDE_FROM_ALL)

add_subdirectory(glad EXCLUDE_FROM_ALL)
set_msvc_runtime_library(glad)

add_subdirectory(stb EXCLUDE_FROM_ALL)

add_subdirectory(physx-static EXCLUDE_FROM_ALL)

set(PHYSFS_ARCHIVE_GRP FALSE CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_WAD FALSE CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_HOG FALSE CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_MVL FALSE CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_SLB FALSE CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_ISO9660 FALSE CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_VDF FALSE CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_SHARED FALSE CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_TEST FALSE CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_DOCS FALSE CACHE BOOL "" FORCE)
add_subdirectory(physfs EXCLUDE_FROM_ALL)
set_msvc_runtime_library(physfs-static)

add_subdirectory(simdjson EXCLUDE_FROM_ALL)
set_msvc_runtime_library(simdjson)

add_subdirectory(tracy EXCLUDE_FROM_ALL)
set_msvc_runtime_library(TracyClient)
