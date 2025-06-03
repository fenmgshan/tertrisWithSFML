cmake_minimum_required(VERSION 3.25)
project(tertrisWithSFML)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "src/*.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(FetchContent)

set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "Build SFML Examples" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "Build SFML Documentation" FORCE)
set(SFML_BUILD_TESTS OFF CACHE BOOL "Build SFML Tests" FORCE)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.5.1
    GIT_SHALLOW ON
    SYSTEM
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SFML)


add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/src/Aleo-Regular.ttf
    ${CMAKE_BINARY_DIR}/bin/
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics  
    sfml-window    
    sfml-system    
)
