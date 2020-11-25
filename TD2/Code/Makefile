include Make.inc
#include Make_msys2.inc
#include Make_osx.inc

CXXFLAGS = -std=c++11 -fPIC
ifdef DEBUG
CXXFLAGS += -g -O0 -Wall -fbounds-check -pedantic -D_GLIBCXX_DEBUG
else
CXXFLAGS += -O3 -march=native -Wall
endif

ALL=matvec.exe Mandelbrot.exe 

default:	help

clean:
	@rm -fr *.o *.exe *~ *.tga

%.exe: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIB)

%: %.exe
	mpiexec $(MPIOPT) -np 8 ./$^

help:
	@echo "Available targets : "
	@echo "    matvec_Row.exe     : compile matrix vector product by row executable"
	@echo "    Mandelbrot.exe     : compile Mandelbrot executable"
	@echo "    Mandelbrot_M-E.exe : compile Mandelbrot with M-E mode executable"
	@echo "    matvec_Row         : execute matrix vector product by row with nbp = 8"
	@echo "    Mandelbrot         : execute Mandelbrot with nbp = 8"
	@echo "    Mandelbrot_M-E     : execute Mandelbrot with M-E mode with nbp = 8"
	@echo "Add DEBUG=yes to compile in debug"
	@echo "Configuration :"
	@echo "    CXX      :    $(CXX)"
	@echo "    CXXFLAGS :    $(CXXFLAGS)"
