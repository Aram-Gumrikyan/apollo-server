cmake_minimum_required(VERSION 3.25)
project(apollo_server)

find_package(OpenSSL REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(apollo_server main.cpp src/InetSocket.cpp src/InetSocket.h src/UserInput.cpp src/UserInput.h utils/State.cpp utils/State.h cryptography/rsa.h cryptography/aes.h)
target_link_libraries(apollo_server OpenSSL::Crypto)