KCFLAGS =
OBJS = kceagec

.PHONY: all clean
.SUFFIXES: .kcea .o

all: $(OBJS)

clean:
	$(RM) $(OBJS)

.kcea.o:
	$(KCC) $(KCFLAGS) -o $@ $^
