project(cryptoleq LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

set(OPT_FLAGS "-march=native -mtune=native -flto=full -O3 -fomit-frame-pointer")
set(COMPILE_FLAGS "-fno-strict-aliasing")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILE_FLAGS}")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OPT_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${OPT_FLAGS}")

add_library(sqc STATIC
        utils.cpp
        contxt.cpp
        tokzer.cpp
        nodes.cpp
        parser.cpp
        evltor.cpp
        emltor.cpp
        cloner.cpp
        macros.cpp
        errex.cpp
        memory.cpp
        pragma.cpp
        stat.cpp
        ../processor/compiler.cpp
        ../processor/processor.cpp
        ../processor/memcell.cpp
        ../mmcalc/mmcalc.cpp
        ../mmcalc/prho.cpp
        ../mmcalc/prime.cpp
        ../unumber/unumberg.cpp
        ../unumber/ma_invert_m.cpp
        ../unumber/cunmber_4096_m.c)

add_executable(ceal ceal.cpp)
target_link_libraries(ceal PRIVATE sqc)

add_executable(emlrun emlrun.cpp)
target_link_libraries(emlrun PRIVATE sqc)