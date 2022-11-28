cmake_minimum_required(VERSION 3.0)

project(ced)


find_package(FLTK REQUIRED)
find_library(GCRYPT_LIBRARY REQUIRED NAMES gcrypt)
include_directories (${FLTK_INCLUDE_DIR})


add_executable(ced ced_main.cpp )
#add_executable(ced aes_test.cpp )
target_link_libraries(ced ${FLTK_LIBRARIES} ${GCRYPT_LIBRARY})


install(TARGETS ced RUNTIME DESTINATION bin)
