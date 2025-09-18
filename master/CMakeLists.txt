cmake_minimum_required(VERSION 3.10)
project(master)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Adiciona o diretório src
add_executable(master src/main.cpp)

# Configura threads
find_package(Threads REQUIRED)
target_link_libraries(master PRIVATE Threads::Threads)

# cpp-httplib é header-only
include_directories(${PROJECT_SOURCE_DIR}/include)

# Adiciona flags de compilação
if(UNIX)
    target_link_libraries(master PRIVATE pthread)
endif()
