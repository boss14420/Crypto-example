cmake_minimum_required(VERSION 2.8.4)
project(sha3)

ADD_CUSTOM_TARGET(debug
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Switch CMAKE_BUILD_TYPE to Debug"
  )

ADD_CUSTOM_TARGET(release
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Switch CMAKE_BUILD_TYPE to Release"
  )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -std=c++11")

set(SOURCE_FILES main.cc sha3.hpp)

add_executable(sha3-224 ${SOURCE_FILES})
set_target_properties(sha3-224 PROPERTIES COMPILE_DEFINITIONS "HASH_SIZE=224")
add_executable(sha3-256 ${SOURCE_FILES})
set_target_properties(sha3-256 PROPERTIES COMPILE_DEFINITIONS "HASH_SIZE=256")
add_executable(sha3-384 ${SOURCE_FILES})
set_target_properties(sha3-384 PROPERTIES COMPILE_DEFINITIONS "HASH_SIZE=384")
add_executable(sha3-512 ${SOURCE_FILES})
set_target_properties(sha3-512 PROPERTIES COMPILE_DEFINITIONS "HASH_SIZE=512")

add_executable(hash-by-block sha3.hpp hash_by_block.cc)
