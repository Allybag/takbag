add_library(tcp TcpClient.cpp)
add_library(playtak PlaytakClient.cpp)
target_link_libraries(playtak tcp)
