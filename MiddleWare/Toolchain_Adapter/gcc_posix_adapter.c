/*
 * gcc_posix_adapter.c
 *
 *  Created on: 2023年11月28日
 *      Author: root
 */


/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */

#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../../x64_driver/x64_serial.h"



/***************************************************************/

extern int malloc_region_start[];
extern int malloc_region_size[];

/*
 * read  -- read bytes from the serial port. Ignore fd, since
 *          we only have stdin.
 */
int
_read(fd, buf, nbytes)
     int fd;
     char *buf;
     int nbytes;
{
  int i = 0;

  for (i = 0; i < nbytes; i++) {
    //*(buf + i) = inbyte();
      *(buf + i) = x86_serial_receive(0x3f8);
    if ((*(buf + i) == '\n') || (*(buf + i) == '\r')) {
      (*(buf + i)) = 0;
      break;
    }
  }
  return (i);
}

/*
 * write -- write bytes to the serial port. Ignore fd, since
 *          stdout and stderr are the same. Since we have no filesystem,
 *          open will only return an error.
 */
int
_write(fd, buf, nbytes)
     int fd;
     char *buf;
     int nbytes;
{
  int i;

  for (i = 0; i < nbytes; i++) {
    if (*(buf + i) == '\n') {
        x86_serial_send(0x3f8, '\r');
    }
    x86_serial_send (0x3f8, *(buf + i));
  }
  return (nbytes);
}

/*
 * open -- open a file descriptor. We don't have a filesystem, so
 *         we return an error.
 */
int
_open(buf, flags, mode)
     char *buf;
     int flags;
     int mode;
{
  errno = EIO;
  return (-1);
}

/*
 * close -- close a file descriptor. We don't need
 *          to do anything, but pretend we did.
 */
int
_close(fd)
     int fd;
{
  return (0);
}

/*
 * sbrk -- changes heap size size. Get nbytes more
 *         RAM. We just increment a pointer in what's
 *         left of memory on the board.
 */
caddr_t
_sbrk(nbytes)
     int nbytes;
{
  static caddr_t  heap_ptr = NULL;
  static uint64_t heap_top = 0;
  caddr_t        base;

  if (heap_ptr == NULL) {
    heap_ptr = (caddr_t)&malloc_region_start[0];
    heap_top = (uint64_t)malloc_region_start + \
                                 (uint64_t)malloc_region_size;
  }

  if ((heap_top - ((uint64_t)heap_ptr + nbytes)) >= 0) {
    base = heap_ptr;
    heap_ptr += nbytes;
    return (base);
  } else {
    errno = ENOMEM;
    return ((caddr_t)-1);
  }
}

/*
 * isatty -- returns 1 if connected to a terminal device,
 *           returns 0 if not. Since we're hooked up to a
 *           serial port, we'll say yes and return a 1.
 */
int
_isatty(fd)
     int fd;
{
  return (1);
}

/*
 * lseek -- move read/write pointer. Since a serial port
 *          is non-seekable, we return an error.
 */
off_t
_lseek(fd,  offset, whence)
     int fd;
     off_t offset;
     int whence;
{
  errno = ESPIPE;
  return ((off_t)-1);
}

/*
 * fstat -- get status of a file. Since we have no file
 *          system, we just return an error.
 */
int
_fstat(fd, buf)
     int fd;
     struct stat *buf;
{
  errno = EIO;
  return (-1);
}

/*
 * getpid -- only one process, so just return 1.
 */
#define __MYPID 1
int
_getpid()
{
  return __MYPID;
}


void _exit (int __status) {
    (void)__status;
}


/*
 * kill -- go out via exit...
 */
int
_kill(pid, sig)
     int pid;
     int sig;
{
  if(pid == __MYPID)
    _exit(sig);
  return 0;
}

///*
// * print -- do a raw print of a string
// */
//int
//_print(ptr)
//char *ptr;
//{
//  while (*ptr) {
//    outbyte (*ptr++);
//  }
//}
//
///*
// * putnum -- print a 32 bit number in hex
// */
//int
//_putnum (num)
//unsigned int num;
//{
//  char  buffer[9];
//  int   count;
//  char  *bufptr = buffer;
//  int   digit;
//
//  for (count = 7 ; count >= 0 ; count--) {
//    digit = (num >> (count * 4)) & 0xf;
//
//    if (digit <= 9)
//      *bufptr++ = (char) ('0' + digit);
//    else
//      *bufptr++ = (char) ('a' - 10 + digit);
//  }
//
//  *bufptr = (char) 0;
//  printf (buffer);
//  return 0;
//}



