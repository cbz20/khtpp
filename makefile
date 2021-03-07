PATH_EIGEN = ../libraries/Eigen
OBJECTS = aux_svg.o aux_sys.o Coefficients.o BNAlgebra.o Cob.o Chain.o Complex.o Tangles.o Tests.o
LIBS = -I$(PATH_EIGEN)

GIT_VERSION := "$(shell git describe --dirty --always --tags)"
# If git is unavailable, use the version below.
ifneq ($(.SHELLSTATUS),0)
	GIT_VERSION = 0.1
endif

DOXYGEN_VERSION := "$(shell doxygen -v)"

CXX = clang++
%.o : CPPFLAGS = -std=c++17 -O3 -fPIE -Wall $(LIBS) -MD -DVERSION=\"$(GIT_VERSION)\"

vpath %.h sources/headers
vpath %.h sources/specialization
vpath %.cpp sources/modules
vpath %.cpp sources

kht++ : main.o $(OBJECTS) main.cpp computations.h
	$(CXX) -o kht++ $(CPPFLAGS) -flto main.o $(OBJECTS)

.PHONY: docs

docs :
	cp conf.py conf.py.tmp
	sed -i 's/MY_GIT_VERSION/'$(GIT_VERSION)'/g' conf.py
	sed -i 's/MY_DOXYGEN_VERSION/'$(DOXYGEN_VERSION)'/g' conf.py
	./docs/m.css/documentation/doxygen.py conf.py
	mv conf.py.tmp conf.py

.PHONY: clean

clean :
	rm -f *.o
	rm -f *.d

# see https://nathandumont.com/blog/automatically-detect-changes-in-header-files-in-a
-include $(OBJECTS:.o=.d)
