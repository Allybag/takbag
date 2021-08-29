include_directories(../../external)
include_directories(..)
add_library(engine Engine.cpp TranspositionTable.cpp OpeningBook.cpp)
target_link_libraries(engine log)
