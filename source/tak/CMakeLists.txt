include_directories(../../external)
include_directories(..)

add_library(game Game.cpp Position.cpp Square.cpp)
add_executable(tak tak.cpp)
target_link_libraries(tak game)
target_link_libraries(tak ptn)
target_link_libraries(tak engine)
target_link_libraries(tak playtak)
