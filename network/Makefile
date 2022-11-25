######################################
#
######################################
#source file
SOURCE  := $(wildcard *.c) $(wildcard *.cpp)
OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))
ASSEMBLY:= $(patsubst %.c,%.S,$(patsubst %.cpp,%.S,$(SOURCE)))

#target you can change test to what you want

TARGET  := $(patsubst %.c,%,$(patsubst %.cpp,%,$(SOURCE)))

# x86 or x64
SOURCE_X86 := $(wildcard *-sender.c)
OBJS_X86   := $(patsubst %.c,%.o,$(SOURCE_X86))
ASSEMBLY_X86:= $(patsubst %.c,%.S,$(SOURCE_X86))
TARGET_X86  := $(patsubst %.c,%,$(SOURCE_X86))

#compile and lib parameter

CC      := gcc
LIBS    := m
LDFLAGS :=
DEFINES :=
INCLUDE := -I.
CFLAGS  := -g -Wall -O0 $(DEFINES) $(INCLUDE)
CXXFLAGS:= $(CFLAGS) -DHAVE_CONFIG_H
ARCH    := $(shell uname -m)


#i think you should do anything here

.PHONY : everything objs clean veryclean rebuild

everything : $(TARGET)

all : $(TARGET)

objs : $(OBJS)

rebuild: veryclean everything

clean :
	rm -rf build

preprocess :
	if ! [ -d build ]; then mkdir build; fi

$(ASSEMBLY) : preprocess
	$(CC) $(CXXFLAGS) -o build/$@ $(patsubst %.S,%.c,$@) -S $(LDFLAGS) -l $(LIBS)

$(TARGET) : $(ASSEMBLY)
	$(CC) $(CXXFLAGS) -o build/$@ build/$@.S $(LDFLAGS) -l $(LIBS)

$(ASSEMBLY_X86) : preprocess
ifeq ($(ARCH),x86_64)
	$(CC) $(CXXFLAGS) -o build/$@ $(patsubst %.S,%.c,$@) -S $(LDFLAGS) -l $(LIBS)
else
	echo "This is not x86_64, but $(ARCH). So skip it."
endif

$(TARGET_X86) : $(ASSEMBLY_X86)
ifeq ($(ARCH),x86_64)
	$(CC) $(CXXFLAGS) -o build/$@ build/$@.S $(LDFLAGS) -l $(LIBS)
else
	echo "This is not x86_64, but $(ARCH). So skip it."
endif
