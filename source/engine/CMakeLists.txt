include_directories(../../external)
include_directories(..)
add_library(engine Engine.cpp TranspositionTable.cpp)
target_link_libraries(engine log)
