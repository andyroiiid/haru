add_subdirectory(dependencies EXCLUDE_FROM_ALL)

add_library(engine
        include/haru/MoveOnly.h

        src/main.cpp)

target_include_directories(engine PUBLIC include)

target_compile_definitions(engine PUBLIC GLFW_INCLUDE_NONE)

target_link_libraries(engine PUBLIC glfw glad glm)