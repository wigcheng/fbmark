/*
  fbmark              Linux Framebuffer Benchmark
  Copyright (C) 2014  Nicolas Caramelli

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
  int fd;
  struct fb_var_screeninfo info;
  size_t len;
  unsigned char *buffer, *data, r, g ,b;
  unsigned int w, h, i, j, x, y, start, end, count;
  struct timeval tv;

  fd = open("/dev/fb0", O_RDWR);
  ioctl(fd, FBIOGET_VSCREENINFO, &info);
  len = info.xres * info.yres * info.bits_per_pixel / 8;
  buffer = mmap(NULL, len, PROT_WRITE, MAP_SHARED, fd, 0);
  w = 320;
  h = 240;
  count = 0;
  gettimeofday(&tv, NULL);
  start = tv.tv_sec * 1000 + tv.tv_usec / 1000;

  do {
    r = rand() % 256;
    g = rand() % 256;
    b = rand() % 256;
    x = rand() % (info.xres - w);
    y = rand() % (info.yres - h);
    data = buffer + y * info.xres * 3 + x * 3;

    for (j = 0; j < h; j++) {
      for (i = 0; i < w; i++) {
        data[3*i + 2] = r;
        data[3*i + 1] = g;
        data[3*i    ] = b;
      }
      data += info.xres * 3;
    }

    count++;
    gettimeofday(&tv, NULL);
    end = tv.tv_sec * 1000 + tv.tv_usec / 1000;
  } while (end < (start + 5000));

  memset(buffer, 0, len);

  printf("\nRectangle frame buffer test bench\n\n");
  printf("Benchmarking %dx%d size: %.2f MPixels/second\n\n", w, h, count * w * h / ((end - start) * 1000.));

  munmap(buffer, len);
  close(fd);

  return 0;
}
