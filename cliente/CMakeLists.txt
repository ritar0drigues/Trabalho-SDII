cmake_minimum_required(VERSION 3.10)
project(cliente)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Adiciona o diretório src
add_executable(cliente src/main.cpp)

# Configura threads (necessário para cpp-httplib)
find_package(Threads REQUIRED)
target_link_libraries(cliente PRIVATE Threads::Threads)

# cpp-httplib é header-only, então precisamos apenas incluir
include_directories(${PROJECT_SOURCE_DIR}/include)

# Adiciona flags de compilação
if(UNIX)
    target_link_libraries(cliente PRIVATE pthread)
endif()
