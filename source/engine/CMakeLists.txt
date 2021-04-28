include_directories(../../external)
include_directories(..)
add_library(engine Engine.cpp)
target_link_libraries(engine log)
