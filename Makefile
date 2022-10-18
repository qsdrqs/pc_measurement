TARGET := $(patsubst %,%.build,$(wildcard *))

$(TARGET):
	cd $(patsubst %.build,%,$@) && make
