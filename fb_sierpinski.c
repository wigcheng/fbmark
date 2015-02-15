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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

typedef struct {
  int x;
  int y;
} pixel_t;

int main(int argc, char **argv)
{
  int fd;
  struct fb_var_screeninfo info;
  size_t len;
  unsigned char *buffer, *data;
  pixel_t p[3], v;
  unsigned int angle, r, iters, frames, time, i, n, d[3];
  float seconds, fps[16];
  struct timeval last, now;

  fd = open("/dev/fb0", O_RDWR);
  ioctl(fd, FBIOGET_VSCREENINFO, &info);
  len = info.xres * info.yres * info.bits_per_pixel / 8;
  buffer = mmap(NULL, len, PROT_WRITE, MAP_SHARED, fd, 0);
  data = malloc(len);
  angle = 0;
  r = info.yres / 2 - 32;
  iters = 1024;
  frames = time = 0;
  gettimeofday(&last, NULL);

  while (time < sizeof(fps) / sizeof(float)) {
    for (n = 0; n < 3; n++) {
      p[n].x = info.xres / 2 + r * cos((n * 120 + angle) * M_PI / 180);
      p[n].y = info.yres / 2 + r * sin((n * 120 + angle) * M_PI / 180);
    }
    v.x = p[0].x;
    v.y = p[0].y;
    for (i = 0; i < iters; i++) {
      n = rand() % 3;
      v.x = (v.x + p[n].x) / 2. + .5;
      v.y = (v.y + p[n].y) / 2. + .5;
      for (n = 0; n < 3; n++) {
        d[n] = (v.x - p[n].x) * (v.x - p[n].x) + (v.y - p[n].y) * (v.y - p[n].y);
        data[v.y * info.xres * 3 + v.x * 3 + 2 - n] = (1 - d[n] / (3. * r * r)) * 255;
      }
    }

    memcpy(buffer, data, len);
    memset(data, 0, len);

    frames++;
    gettimeofday(&now, NULL);
    seconds = (now.tv_sec - last.tv_sec) + (now.tv_usec - last.tv_usec) * 1e-6;
    if (seconds >= 1) {
      fps[time] = frames / seconds;
      if (fps[time] < 4)
        break;
      last = now;
      frames = 0;
      iters *= 2;
      time++;
    }

    angle += 1;
  }

  memset(buffer, 0, len);

  printf("\nSierpinski frame buffer test bench\n\n");
  iters = 1024;
  for (i = 0; i < time; i++) {
    printf("Benchmarking %8d iterations: %8.2f Frames/second\n", iters, fps[i]);
    iters *= 2;
  }
  printf("\n");

  free(data);
  munmap(buffer, len);
  close(fd);

  return 0;
}
