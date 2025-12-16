cmake_minimum_required(VERSION 3.29)
project(Blockchain)

set(CMAKE_CXX_STANDARD 20)

# 1. Create a library for your C backend
# This wraps your backend.c logic so it can be shared.
add_library(c_backend
        c_backend/backend.c
        c_backend/backend.h)

# 2. Create the Web Server executable
add_executable(web_server
        web_server.cpp
        httplib.h)

# 3. Link the Web Server to your C backend
# This allows web_server.cpp to call functions like initialize_system()
target_link_libraries(web_server PRIVATE c_backend)