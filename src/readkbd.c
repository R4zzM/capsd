#include <stdio.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
  struct input_event ev;  
  int fd;
  int nbytes;
  
  const char *file = "/dev/input/event5";
  fd = fopen(file, O_RDONLY);
  if(fd == -1) {
    printf("Could not open %s\n", file);
    goto err;
  }
  
  while(1) {
    nbytes = read(fd, &ev, sizeof(ev));
    if(nbytes != sizeof(ev)) {
      // Not sure if this is possible.
      printf("Crap. Got %d bytes but expected %u.\n", nbytes, sizeof(ev));
      goto err_close_file;
    } else {
      printf("ev.type = %u, ev.code = %u, ev.value = %u\n", ev.type, ev.code, 
          ev.value);
    }
  }
  
  close(fd);

  return 0;

err_close_file:
  close(fd);
err:
  return -1;
}
