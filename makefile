PATH_EIGEN = ../libraries/Eigen
OBJECTS = aux_svg.o aux_sys.o Coefficients.o BNAlgebra.o Cob.o Chain.o Complex.o Tangles.o Tests.o
LIBS = -I$(PATH_EIGEN)

GIT_LATEST_COMMIT := "$(shell git log --format="%H" -n 1)"
GIT_VERSION := "$(shell git describe --dirty --always --tags)"
# If git is unavailable, use the version below.
ifneq ($(.SHELLSTATUS),0)
	GIT_VERSION = 0.1
	GIT_LATEST_COMMIT = main
endif

DOXYGEN_VERSION := "$(shell doxygen -v)"

CXX = clang++ # tested with clang 10.0.0-4ubuntu1
#CXX = g++ # tested with g++ 9.3.0-17ubuntu1~20.04
%.o : CPPFLAGS = -std=c++17 -O3 -fPIE -Wall $(LIBS) -MD -DNDEBUG -DVERSION=\"$(GIT_VERSION)\"
# for debugging:
#%.o : CPPFLAGS = -std=c++17 -ggdb -fPIE -Wall $(LIBS) -MD -DVERSION=\"$(GIT_VERSION)\"

vpath %.h sources/headers
vpath %.h sources/specialization
vpath %.cpp sources/modules
vpath %.cpp sources

kht++ : main.o $(OBJECTS) main.cpp computations.h
	$(CXX) -o kht++ $(CPPFLAGS) -flto main.o $(OBJECTS)
# force recompilation, so versioning is always up to date
	rm main.o

.PHONY: docs

docs :
	cp conf.py conf.py.tmp
	sed -i 's/MY_GIT_VERSION/'$(GIT_VERSION)'/g' conf.py
	sed -i 's/MY_GIT_LATEST_COMMIT/'$(GIT_LATEST_COMMIT)'/g' conf.py
	sed -i 's/MY_DOXYGEN_VERSION/'$(DOXYGEN_VERSION)'/g' conf.py
	./docs/m.css/documentation/doxygen.py conf.py
	(cat sources/Brief.md && echo "\n### Acknowledgements" && cat sources/Acknowledgements.md) > README.md
	mv conf.py.tmp conf.py

.PHONY: clean

clean :
	rm -f *.o
	rm -f *.d

# see https://nathandumont.com/blog/automatically-detect-changes-in-header-files-in-a
-include $(OBJECTS:.o=.d)
