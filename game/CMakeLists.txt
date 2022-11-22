add_executable(game game.cpp)

target_link_libraries(game PRIVATE engine)

set_msvc_runtime_library(game)