TARGET := $(patsubst %,%.build,$(wildcard *))

all: $(TARGET)

$(TARGET):
	if [[ -d $(patsubst %.build,%,$@) ]]; then cd $(patsubst %.build,%,$@) && make; fi
