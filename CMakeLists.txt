cmake_minimum_required(VERSION 3.10)
project(Traffic)

set(CMAKE_CXX_STANDARD 17)

# Adiciona os arquivos .cpp do bayonet manualmente
file(GLOB BAYONET_SOURCES
    external/bayonet/src/*.cpp
)

# Seu executável principal
add_executable(traffic
    main.cpp
    ${BAYONET_SOURCES}
)

# Inclui os headers do bayonet
target_include_directories(traffic PRIVATE external/bayonet/include)

