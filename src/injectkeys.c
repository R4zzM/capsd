#include <linux/input.h>
#include <linux/uinput.h>
#include <linux/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

static int inject_event(fd) 
{
  int ret;
  int i;
  struct input_event keydown;
  struct input_event keyup;
  struct input_event syn;

  memset(&keydown, 0, sizeof(keydown));
  memset(&keyup, 0, sizeof(keyup));
  memset(&syn, 0, sizeof(syn));

  keydown.type = EV_KEY;
  keydown.code = KEY_D;
  keydown.value = 1;

  keyup.type = EV_KEY;
  keyup.code = KEY_D;
  keyup.value = 0;

  syn.type = EV_SYN;
  syn.code = 0;
  syn.value = 0;

  for(i = 0; i < 5; i++) {
    ret = write(fd, &keydown, sizeof(keydown));
    if(ret != sizeof(keydown)) 
      goto err;

    ret = write(fd, &keyup, sizeof(keyup));
    if(ret != sizeof(keyup)) 
      goto err;

    ret = write(fd, &syn, sizeof(syn));
    if(ret != sizeof(syn)) {
      goto err;
    }
  }

  return 0;

err:
  return ret;
}

static int create_device(fd) 
{
  int ret;
  struct uinput_user_dev uidev;

  // open device
  memset(&uidev, 0, sizeof(uidev));
  
  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "Smartcaps");
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor = 0x045e;
  uidev.id.product = 0x00dd;
  uidev.id.version = 0x0111;

  ret = write(fd, &uidev, sizeof(uidev));
  if(ret != sizeof(uidev))
    goto err;

  ret = ioctl(fd, UI_DEV_CREATE);
  if(ret == -1)
    goto err;
  
  return 0;

err:
  return ret;
}

static int configure_device(fd) 
{
  int ret; 

  ret = ioctl(fd, UI_SET_EVBIT, EV_KEY);
  if(ret == -1) {
    goto err;
  }

  ret = ioctl(fd, UI_SET_EVBIT, EV_SYN);
  if(ret == -1) {
    goto err;
  }
 
  ret = ioctl(fd, UI_SET_KEYBIT, KEY_D);
  if (ret == -1) {
    goto err;
  }

  return 0;

err:
  return ret;
}

static int open_device() 
{
  int fd;
  fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  return fd;
}

static void destroy_device(int fd) 
{
  ioctl(fd, UI_DEV_DESTROY);
}

int main(int argc, char **argv)
{
  int ret = 0;
  int fd;

  fd = open_device();
  if(fd < 0) {
    printf("Could not open device. ret = %d", fd);
    goto err; 
  }

  ret = configure_device(fd);
  if(ret) {
    printf("Could not configure device. ret = %d", ret);
    goto err_close_uinput;
  }

  ret = create_device(fd);
  if(ret) {
    printf("Could not create device. ret = %d", ret);
    goto err_close_uinput;
  }

  sleep(2);
  ret = inject_event(fd);
  if(ret) {
    printf("Could not inject event. ret = %d", ret);
    goto err_destroy_device;
  }

  destroy_device(fd);
  close(fd);

  return 0;

err_destroy_device:
  destroy_device(fd);
err_close_uinput:
  close(fd);
err:
  return ret; 
}
