PROJECT(wizipedia)

FIND_LIBRARY( /toolchain/lib/libbz2.a bz2 )
ADD_EXECUTABLE(wizipedia render.cpp util.cpp gui.cpp input.cpp index.cpp main.cpp)

# add_library(/toolchain/lib/libbz2.a  -lshared)

TARGET_LINK_LIBRARIES(wizipedia ${/toolchain/lib/libbz2.a})