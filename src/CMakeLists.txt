cmake_minimum_required(VERSION 2.6)
project(rfid)

add_definitions("-Wall -Wshadow -ggdb")
set(sources
    main.c
    serial_port.c
    )

add_executable(rfid
    ${sources}
    )

target_link_libraries(rfid
    pthread
    )

