include_directories(../source)
include_directories(../external)

add_executable(testPosition testPosition.cpp)
target_link_libraries(testPosition game)
target_link_libraries(testPosition ptn)

add_executable(testMoveGenerator testMoveGenerator.cpp)
target_link_libraries(testMoveGenerator game)
target_link_libraries(testMoveGenerator ptn)
