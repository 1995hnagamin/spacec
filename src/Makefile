.PHONY: all clean

TARGET = spccc
all: $(TARGET)


INCLUDES = -I../inc
CXXFLAGS = -std=c++14 \
	-Wall -Wextra -Wpedantic \
	$(INCLUDES)
LDFLAGS = -lc++
OBJS = spccc.o \
	lexer.o

$(TARGET): $(OBJS)

clean:
	rm -rf $(TARGET) $(OBJS)
