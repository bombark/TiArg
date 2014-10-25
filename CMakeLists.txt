# The name of our project is "HELLO". CMakeLists files in this project can 
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and 
# to the root binary directory of the project as ${HELLO_BINARY_DIR}. 
cmake_minimum_required (VERSION 2.6) 
project (TiArg) 



SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x -O2")

add_library (tiarg src/tiarg.cpp)
#add_executable (tiarg.complete src-bin/tiarg-complete.cpp) 


#target_link_libraries (tiarg.complete tiobj tiarg) 


install(TARGETS tiarg DESTINATION lib)
install(FILES   src/tiarg.hpp DESTINATION include)

