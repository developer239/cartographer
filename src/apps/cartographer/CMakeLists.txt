set(APP_NAME Cartographer)
set(APP_FOLDER_NAME cartographer)

set(SOURCES
        src/strategies/GameLoopStrategy.h
        src/main.cpp
        )

add_executable(${APP_NAME} ${SOURCES})

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/src/apps/${APP_FOLDER_NAME})

target_link_libraries(${APP_NAME} Core)

target_link_libraries(${APP_NAME} ${OpenCV_LIBS})
