include_directories(../source)
include_directories(../external)

add_executable(testPosition testPosition.cpp)
target_link_libraries(testPosition game)

add_executable(testMoveGenerator testMoveGenerator.cpp)
target_link_libraries(testMoveGenerator game)
target_link_libraries(testMoveGenerator log)
target_link_libraries(testMoveGenerator engine)

add_executable(bench bench.cpp)
target_link_libraries(bench game)
target_link_libraries(bench log)
target_link_libraries(bench engine)

add_executable(testOpeningBook testOpeningBook.cpp)
target_link_libraries(testOpeningBook game)
target_link_libraries(testOpeningBook engine)

add_executable(testEngine testEngine.cpp)
target_link_libraries(testEngine game)
target_link_libraries(testEngine engine)

add_executable(testTranspositionTable testTranspositionTable.cpp)
target_link_libraries(testTranspositionTable game)
target_link_libraries(testTranspositionTable engine)

if (NOT LOW_MEMORY)
    target_link_libraries(testMoveGenerator ptn)
    target_link_libraries(testEngine ptn)
    target_link_libraries(testOpeningBook ptn)
    target_link_libraries(testTranspositionTable ptn)
    target_link_libraries(bench ptn)
    target_link_libraries(testPosition ptn)
endif()