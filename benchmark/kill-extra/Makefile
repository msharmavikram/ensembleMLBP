CXX := aarch64-linux-gnu-g++
CXXFLAGS := --static
LDFLAGS := -L m5
LDLIBS := -l m5

progs := $(patsubst %.cpp,%,$(wildcard *.cpp))

all: $(progs)

.PHONY: clean

clean:
	rm $(progs)
