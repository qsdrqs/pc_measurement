######################################
#
######################################
#source file
SOURCE  := $(wildcard *.c) $(wildcard *.cpp)
OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))

#target you can change test to what you want

TARGET  := $(patsubst %.c,%,$(patsubst %.cpp,%,$(SOURCE)))

#compile and lib parameter

CC      := gcc
LIBS    := pthread
LDFLAGS :=
DEFINES :=
INCLUDE := -I.
CFLAGS  := -g -Wall -O0 $(DEFINES) $(INCLUDE)
CXXFLAGS:= $(CFLAGS) -DHAVE_CONFIG_H


#i think you should do anything here

.PHONY : everything objs clean veryclean rebuild

everything : $(TARGET)

all : $(TARGET)

objs : $(OBJS)

rebuild: veryclean everything

clean :
	rm -rf build

$(TARGET) :
	if ! [ -d build ]; then mkdir build; fi
	$(CC) $(CXXFLAGS) -o build/$@ $@.c $(LDFLAGS) -l $(LIBS)
