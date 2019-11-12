.PHONY: all clean

TARGET = lexer.o
all: $(TARGET)


INCLUDES = -I../inc
CXXFLAGS = -std=c++11 \
	-Wall -Wextra -Wpedantic \
	$(INCLUDES)

clean:
	rm -rf $(TARGET)
