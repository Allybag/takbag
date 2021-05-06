add_library(playtak PlaytakClient.cpp)
target_link_libraries(playtak tcp)
target_link_libraries(playtak pthread)
