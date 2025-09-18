cmake_minimum_required(VERSION 3.10)
project(cliente)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontrar pacotes necessários
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# GTK+3
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Adicionar diretórios de include
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${GTK3_INCLUDE_DIRS})

# Adicionar diretórios de bibliotecas
link_directories(${GTK3_LIBRARY_DIRS})

# Adicionar flags de compilação
add_definitions(${GTK3_CFLAGS_OTHER})

# Criar executável
add_executable(cliente src/main.cpp)

# Linkar bibliotecas
target_link_libraries(cliente 
    PRIVATE 
    ${GTK3_LIBRARIES}
    Threads::Threads
)

# Adiciona flags específicas para Unix
if(UNIX)
    target_link_libraries(cliente PRIVATE pthread)
endif()