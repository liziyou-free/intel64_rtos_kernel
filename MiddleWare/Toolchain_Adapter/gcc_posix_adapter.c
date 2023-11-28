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


/* Forward prototypes.  */
int _system     (const char *){return 0;}
int _rename     (const char *, const char *){return 0;}
int _isatty     (int){return 1;}
clock_t _times      (struct tms *){return 1;}
int _gettimeofday   (struct timeval *, void *){return 1;}
void    _raise      (void){};
int _unlink     (const char *){return 1;}
int _link       (const char *, const char *){return 1;}
int _stat       (const char *, struct stat *){return 1;}
int _fstat      (int, struct stat *){return 1;}
void *  _sbrk       (ptrdiff_t){return NULL;}
pid_t   _getpid     (void){return 1;}
int _kill       (int, int){return 1;}
void _exit      (int){ return;}
int _close      (int){return 1;}
int _swiclose   (int){return 1;}
int _open       (const char *, int, ...){return 1;}
int _swiopen    (const char *, int){return 1;}
int _write      (int, const void *, size_t){return 1;}
int _swiwrite   (int, const void *, size_t){return 1;}
_off_t  _lseek      (int, _off_t, int){return 1;}
_off_t  _swilseek   (int, _off_t, int){return 1;}
int _read       (int, void *, size_t){return 1;}
int _swiread    (int, void *, size_t){return 1;}
void    initialise_monitor_handles (void){}

