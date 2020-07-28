.PHONY: all clean

TARGET = spccc
all: $(TARGET)


LLVM_CONFIG = llvm-config
INCLUDES = -I../inc \
	$(shell $(LLVM_CONFIG) --cxxflags)
CXXFLAGS = -std=c++14 \
	-Wall -Wextra -Wpedantic \
	-Wno-unused-parameter \
	$(INCLUDES)
LDFLAGS = -lc++
LDLIBS = $(shell $(LLVM_CONFIG) --ldflags --libs --system-libs)
OBJS = spccc.o \
	ast.o \
	binop.o \
	codegen.o \
	lexer.o \
	parser.o \
	type.o \
	typechecker.o

$(TARGET): $(OBJS)

ast.o: ../inc/ast.hpp

codegen.o: ../inc/ast.hpp ../inc/binop.hpp ../inc/codegen.hpp

lexer.o: ../inc/lexer.hpp

parser.o: ../inc/ast.hpp ../inc/lexer.hpp ../inc/parser.hpp

spccc.o: ../inc/ast.hpp ../inc/lexer.hpp ../inc/parser.hpp

clean:
	rm -rf $(TARGET) $(OBJS)
