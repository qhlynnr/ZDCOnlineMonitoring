RUNTOOLSDIR=/afs/cern.ch/user/x/xirong/runtools

CXX = g++
CXXFLAGS = -O2 -Wall -std=c++17 \
          $(shell root-config --cflags) \
          -Iinclude -I$(RUNTOOLSDIR)

LDFLAGS = $(shell root-config --libs --glibs)

TARGET = plotZDCEmuLoop_Parallel

SRC = plotZDCEmuLoop_Parallel.C
OBJ = $(SRC:.C=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.C
	$(CXX) $(CXXFLAGS) -Iinclude -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)