cmake_minimum_required(VERSION 3.23)
project(haru)

if (POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif ()

macro(set_msvc_runtime_library target)
    set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endmacro()

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(engine)

add_subdirectory(game)