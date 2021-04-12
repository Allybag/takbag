include_directories(../source)
include_directories(../external)

add_executable(testPosition testPosition.cpp)
target_link_libraries(testPosition game)
target_link_libraries(testPosition ptn)
