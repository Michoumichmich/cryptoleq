
COMPILER=g++
COMPILERC=gcc
ARCR=ar crs #
AR=arXXX
LD=ld
RANLIB=ranlibXXX
STRIP=strip

OPT=-Wall -O2 -std=c++17 -fno-strict-aliasing -march=native -mtune=native -flto
OPTC=-Wall -O2

INC=
EEXT=
OEXT=.o
LEXT=.a
DEFEXT=
LDF=
OOUT=-o 
EOUT=-o 

ifdef GCOV
OPT= -Wall -g -std=c++14 -fprofile-arcs -ftest-coverage -fno-elide-constructors
endif

