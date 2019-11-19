.PHONY: all clean

TARGET = spccc
all: $(TARGET)


INCLUDES = -I../inc
CXXFLAGS = -std=c++11 \
	-Wall -Wextra -Wpedantic \
	$(INCLUDES)
OBJS = spccc.o \
	lexer.o

$(TARGET): $(OBJS)

clean:
	rm -rf $(TARGET) $(OBJS)
