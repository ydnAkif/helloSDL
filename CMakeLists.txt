cmake_minimum_required(VERSION 3.30)

project(helloSDL)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)

set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_SOURCE_DIR ${PROJECT_DIR}/src)
set(PROJECT_INCLUDE_DIR ${PROJECT_DIR}/include)

set(PROJECT_ASSETS_DIR ${PROJECT_DIR}/assets)

set(PROJECT_SOURCES
        ${PROJECT_SOURCE_DIR}/main.cpp
)

set(PROJECT_HEADERS
        #${PROJECT_INCLUDE_DIR}/helloSDL.h
)

set(PROJECT_RESOURCES
        ${PROJECT_ASSETS_DIR}/sld.png
)

include_directories(
        ${PROJECT_INCLUDE_DIR}
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
)

if (EXISTS ${PROJECT_ASSETS_DIR})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_ASSETS_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    )
endif ()