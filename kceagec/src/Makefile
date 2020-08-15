KCFLAGS =
LDFLAGS = -no-pie
OBJS = kceagec

.PHONY: all clean
.SUFFIXES: .kcea .o

all: $(OBJS)

clean:
	$(RM) $(OBJS)

%.o: %.kcea $(KCC)
	$(KCC) $(KCFLAGS) -o $@ $<

kceagec: kceagec.o helper.o
