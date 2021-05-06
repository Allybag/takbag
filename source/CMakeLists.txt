add_subdirectory(tak)
add_subdirectory(engine)
add_subdirectory(tcp)
add_subdirectory(playtak)
add_subdirectory(log)

if (NOT LOW_MEMORY)
    add_subdirectory(ptn)
endif()
