.PHONY: all clean

TARGET = spccc
all: $(TARGET)


INCLUDES = -I../inc
CXXFLAGS = -std=c++14 \
	-Wall -Wextra -Wpedantic \
	$(INCLUDES)
LDFLAGS = -lc++
OBJS = spccc.o \
	ast.o \
	lexer.o \
	parser.o \
	type.o

$(TARGET): $(OBJS)

ast.o: ../inc/ast.hpp

lexer.o: ../inc/lexer.hpp

parser.o: ../inc/ast.hpp ../inc/lexer.hpp ../inc/parser.hpp

spccc.o: ../inc/ast.hpp ../inc/lexer.hpp ../inc/parser.hpp

clean:
	rm -rf $(TARGET) $(OBJS)
