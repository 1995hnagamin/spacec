#include <unistd.h>

struct octet_seq {
  char const *data;
  int cap;
};

int write_oseq(struct octet_seq os) {
  write(1, os.data, os.cap);
  return 0;
}
