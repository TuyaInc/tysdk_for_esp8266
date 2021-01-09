/*  Customer ID=7011; Build=0x2b6f6; Copyright (C) 2007--2009 Tensilica, Inc. */
/* Copyright (C) 1993, 1997, 2000 Free Software Foundation, Inc.
   This file is part of the GNU IO Library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2, or (at
   your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA.

   As a special exception, if you link this library with files
   compiled with a GNU compiler to produce an executable, this does
   not cause the resulting executable to be covered by the GNU General
   Public License.  This exception does not however invalidate any
   other reasons why the executable file might be covered by the GNU
   General Public License.  */

#ifndef _POSIX_SOURCE
# define _POSIX_SOURCE
#endif

#include <errno.h>
#ifndef __set_errno
# define __set_errno(Val) errno = (Val)
#endif

#ifdef _IO_MTSAFE_IO
# if defined __GLIBC__ && __GLIBC__ >= 2
#  if __GLIBC_MINOR__ > 0
#   include <bits/libc-lock.h>
#  else
#   include <libc-lock.h>
#  endif
# else
/*# include <comthread.h>*/
# endif
#endif

#include "iolibio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _IO_seek_set 0
#define _IO_seek_cur 1
#define _IO_seek_end 2

/* THE JUMPTABLE FUNCTIONS.

 * The _IO_FILE type is used to implement the FILE type in GNU libc,
 * as well as the streambuf class in GNU iostreams for C++.
 * These are all the same, just used differently.
 * An _IO_FILE (or FILE) object is allows followed by a pointer to
 * a jump table (of pointers to functions).  The pointer is accessed
 * with the _IO_JUMPS macro.  The jump table has a eccentric format,
 * so as to be compatible with the layout of a C++ virtual function table.
 * (as implemented by g++).  When a pointer to a streambuf object is
 * coerced to an (_IO_FILE*), then _IO_JUMPS on the result just
 * happens to point to the virtual function table of the streambuf.
 * Thus the _IO_JUMPS function table used for C stdio/libio does
 * double duty as the virtual function table for C++ streambuf.
 *
 * The entries in the _IO_JUMPS function table (and hence also the
 * virtual functions of a streambuf) are described below.
 * The first parameter of each function entry is the _IO_FILE/streambuf
 * object being acted on (i.e. the 'this' parameter).
 */

#define _IO_JUMPS(THIS) ((struct _IO_FILE_plus *) (THIS))->vtable
#ifdef _G_USING_THUNKS
# define JUMP_FIELD(TYPE, NAME) TYPE NAME
# define JUMP0(FUNC, THIS) _IO_JUMPS(THIS)->FUNC (THIS)
# define JUMP1(FUNC, THIS, X1) _IO_JUMPS(THIS)->FUNC (THIS, X1)
# define JUMP2(FUNC, THIS, X1, X2) _IO_JUMPS(THIS)->FUNC (THIS, X1, X2)
# define JUMP3(FUNC, THIS, X1,X2,X3) _IO_JUMPS(THIS)->FUNC (THIS, X1,X2, X3)
# define JUMP_INIT(NAME, VALUE) VALUE
# define JUMP_INIT_DUMMY JUMP_INIT(dummy, 0), JUMP_INIT (dummy2, 0)
#else
/* These macros will change when we re-implement vtables to use "thunks"! */
# define JUMP_FIELD(TYPE, NAME) struct { short delta1, delta2; TYPE pfn; } NAME
# define JUMP0(FUNC, THIS) _IO_JUMPS(THIS)->FUNC.pfn (THIS)
# define JUMP1(FUNC, THIS, X1) _IO_JUMPS(THIS)->FUNC.pfn (THIS, X1)
# define JUMP2(FUNC, THIS, X1, X2) _IO_JUMPS(THIS)->FUNC.pfn (THIS, X1, X2)
# define JUMP3(FUNC, THIS, X1,X2,X3) _IO_JUMPS(THIS)->FUNC.pfn (THIS, X1,X2,X3)
# define JUMP_INIT(NAME, VALUE) {0, 0, VALUE}
# define JUMP_INIT_DUMMY JUMP_INIT(dummy, 0)
#endif

/* The 'finish' function does any final cleaning up of an _IO_FILE object.
   It does not delete (free) it, but does everything else to finalize it/
   It matches the streambuf::~streambuf virtual destructor.  */
typedef void (*_IO_finish_t) __PMT ((_IO_FILE *, int)); /* finalize */
#define _IO_FINISH(FP) JUMP1 (__finish, FP, 0)

/* The 'overflow' hook flushes the buffer.
   The second argument is a character, or EOF.
   It matches the streambuf::overflow virtual function. */
typedef int (*_IO_overflow_t) __PMT ((_IO_FILE *, int));
#define _IO_OVERFLOW(FP, CH) JUMP1 (__overflow, FP, CH)

/* The 'underflow' hook tries to fills the get buffer.
   It returns the next character (as an unsigned char) or EOF.  The next
   character remains in the get buffer, and the get position is not changed.
   It matches the streambuf::underflow virtual function. */
typedef int (*_IO_underflow_t) __PMT ((_IO_FILE *));
#define _IO_UNDERFLOW(FP) JUMP0 (__underflow, FP)

/* The 'uflow' hook returns the next character in the input stream
   (cast to unsigned char), and increments the read position;
   EOF is returned on failure.
   It matches the streambuf::uflow virtual function, which is not in the
   cfront implementation, but was added to C++ by the ANSI/ISO committee. */
#define _IO_UFLOW(FP) JUMP0 (__uflow, FP)

/* The 'pbackfail' hook handles backing up.
   It matches the streambuf::pbackfail virtual function. */
typedef int (*_IO_pbackfail_t) __PMT ((_IO_FILE *, int));
#define _IO_PBACKFAIL(FP, CH) JUMP1 (__pbackfail, FP, CH)

/* The 'xsputn' hook writes upto N characters from buffer DATA.
   Returns the number of character actually written.
   It matches the streambuf::xsputn virtual function. */
typedef _IO_size_t (*_IO_xsputn_t) __PMT ((_IO_FILE *FP, const void *DATA,
					 _IO_size_t N));
#define _IO_XSPUTN(FP, DATA, N) JUMP2 (__xsputn, FP, DATA, N)

/* The 'xsgetn' hook reads upto N characters into buffer DATA.
   Returns the number of character actually read.
   It matches the streambuf::xsgetn virtual function. */
typedef _IO_size_t (*_IO_xsgetn_t) __PMT ((_IO_FILE *FP, void *DATA,
					 _IO_size_t N));
#define _IO_XSGETN(FP, DATA, N) JUMP2 (__xsgetn, FP, DATA, N)

/* The 'seekoff' hook moves the stream position to a new position
   relative to the start of the file (if DIR==0), the current position
   (MODE==1), or the end of the file (MODE==2).
   It matches the streambuf::seekoff virtual function.
   It is also used for the ANSI fseek function. */
#if defined(_G_IO_IO_FILE_VERSION) && _G_IO_IO_FILE_VERSION == 0x20001
typedef _IO_off64_t (*_IO_seekoff_t) __PMT ((_IO_FILE *FP, _IO_off64_t OFF,
					  int DIR, int MODE));
#else
typedef _IO_off_t (*_IO_seekoff_t) __PMT ((_IO_FILE *FP, _IO_off_t OFF,
					  int DIR, int MODE));
#endif
#define _IO_SEEKOFF(FP, OFF, DIR, MODE) JUMP3 (__seekoff, FP, OFF, DIR, MODE)

/* The 'seekpos' hook also moves the stream position,
   but to an absolute position given by a off_t (seekpos).
   It matches the streambuf::seekpos virtual function.
   It is also used for the ANSI fgetpos and fsetpos functions.  */
/* The _IO_seek_cur and _IO_seek_end options are not allowed. */
#if defined(_G_IO_IO_FILE_VERSION) && _G_IO_IO_FILE_VERSION == 0x20001
typedef _IO_off64_t (*_IO_seekpos_t) __PMT ((_IO_FILE *, _IO_off64_t, int));
#else
typedef _IO_off_t (*_IO_seekpos_t) __PMT ((_IO_FILE *, _IO_off_t, int));
#endif
#define _IO_SEEKPOS(FP, POS, FLAGS) JUMP2 (__seekpos, FP, POS, FLAGS)

/* The 'setbuf' hook gives a buffer to the file.
   It matches the streambuf::setbuf virtual function. */
typedef _IO_FILE* (*_IO_setbuf_t) __PMT ((_IO_FILE *, char *, _IO_ssize_t));
#define _IO_SETBUF(FP, BUFFER, LENGTH) JUMP2 (__setbuf, FP, BUFFER, LENGTH)

/* The 'sync' hook attempts to synchronize the internal data structures
   of the file with the external state.
   It matches the streambuf::sync virtual function. */
typedef int (*_IO_sync_t) __PMT ((_IO_FILE *));
#define _IO_SYNC(FP) JUMP0 (__sync, FP)

/* The 'doallocate' hook is used to tell the file to allocate a buffer.
   It matches the streambuf::doallocate virtual function, which is not
   in the ANSI/ISO C++ standard, but is part traditional implementations. */
typedef int (*_IO_doallocate_t) __PMT ((_IO_FILE *));
#define _IO_DOALLOCATE(FP) JUMP0 (__doallocate, FP)

/* The following four hooks (sysread, syswrite, sysclose, sysseek, and
   sysstat) are low-level hooks specific to this implementation.
   There is no correspondence in the ANSI/ISO C++ standard library.
   The hooks basically correspond to the Unix system functions
   (read, write, close, lseek, and stat) except that a _IO_FILE*
   parameter is used instead of a integer file descriptor;  the default
   implementation used for normal files just calls those functions.
   The advantage of overriding these functions instead of the higher-level
   ones (underflow, overflow etc) is that you can leave all the buffering
   higher-level functions.  */

/* The 'sysread' hook is used to read data from the external file into
   an existing buffer.  It generalizes the Unix read(2) function.
   It matches the streambuf::sys_read virtual function, which is
   specific to this implementation. */
typedef _IO_ssize_t (*_IO_read_t) __PMT ((_IO_FILE *, void *, _IO_ssize_t));
#define _IO_SYSREAD(FP, DATA, LEN) JUMP2 (__read, FP, DATA, LEN)

/* The 'syswrite' hook is used to write data from an existing buffer
   to an external file.  It generalizes the Unix write(2) function.
   It matches the streambuf::sys_write virtual function, which is
   specific to this implementation. */
typedef _IO_ssize_t (*_IO_write_t) __PMT ((_IO_FILE *,const void *,_IO_ssize_t));
#define _IO_SYSWRITE(FP, DATA, LEN) JUMP2 (__write, FP, DATA, LEN)

/* The 'sysseek' hook is used to re-position an external file.
   It generalizes the Unix lseek(2) function.
   It matches the streambuf::sys_seek virtual function, which is
   specific to this implementation. */
#if defined(_G_IO_IO_FILE_VERSION) && _G_IO_IO_FILE_VERSION == 0x20001
typedef _IO_off64_t (*_IO_seek_t) __PMT ((_IO_FILE *, _IO_off64_t, int));
#else
typedef _IO_off_t (*_IO_seek_t) __PMT ((_IO_FILE *, _IO_off_t, int));
#endif
#define _IO_SYSSEEK(FP, OFFSET, MODE) JUMP2 (__seek, FP, OFFSET, MODE)

/* The 'sysclose' hook is used to finalize (close, finish up) an
   external file.  It generalizes the Unix close(2) function.
   It matches the streambuf::sys_close virtual function, which is
   specific to this implementation. */
typedef int (*_IO_close_t) __PMT ((_IO_FILE *)); /* finalize */
#define _IO_SYSCLOSE(FP) JUMP0 (__close, FP)

/* The 'sysstat' hook is used to get information about an external file
   into a struct stat buffer.  It generalizes the Unix fstat(2) call.
   It matches the streambuf::sys_stat virtual function, which is
   specific to this implementation. */
typedef int (*_IO_stat_t) __PMT ((_IO_FILE *, void *));
#define _IO_SYSSTAT(FP, BUF) JUMP1 (__stat, FP, BUF)

#if _G_IO_IO_FILE_VERSION == 0x20001
/* The 'showmany' hook can be used to get an image how much input is
   available.  In many cases the answer will be 0 which means unknown
   but some cases one can provide real information.  */
typedef int (*_IO_showmanyc_t) __PMT ((_IO_FILE *));
#define _IO_SHOWMANYC(FP) JUMP0 (__showmanyc, FP)

/* The 'imbue' hook is used to get information about the currently
   installed locales.  */
typedef void (*_IO_imbue_t) __PMT ((_IO_FILE *, void *));
#define _IO_IMBUE(FP, LOCALE) JUMP1 (__imbue, FP, LOCALE)
#endif


#define _IO_CHAR_TYPE char /* unsigned char ? */
#define _IO_INT_TYPE int

struct _IO_jump_t
{
    JUMP_FIELD(_G_size_t, __dummy);
#ifdef _G_USING_THUNKS
    JUMP_FIELD(_G_size_t, __dummy2);
#endif
    JUMP_FIELD(_IO_finish_t, __finish);
    JUMP_FIELD(_IO_overflow_t, __overflow);
    JUMP_FIELD(_IO_underflow_t, __underflow);
    JUMP_FIELD(_IO_underflow_t, __uflow);
    JUMP_FIELD(_IO_pbackfail_t, __pbackfail);
    /* showmany */
    JUMP_FIELD(_IO_xsputn_t, __xsputn);
    JUMP_FIELD(_IO_xsgetn_t, __xsgetn);
    JUMP_FIELD(_IO_seekoff_t, __seekoff);
    JUMP_FIELD(_IO_seekpos_t, __seekpos);
    JUMP_FIELD(_IO_setbuf_t, __setbuf);
    JUMP_FIELD(_IO_sync_t, __sync);
    JUMP_FIELD(_IO_doallocate_t, __doallocate);
    JUMP_FIELD(_IO_read_t, __read);
    JUMP_FIELD(_IO_write_t, __write);
    JUMP_FIELD(_IO_seek_t, __seek);
    JUMP_FIELD(_IO_close_t, __close);
    JUMP_FIELD(_IO_stat_t, __stat);
#if _G_IO_IO_FILE_VERSION == 0x20001
    JUMP_FIELD(_IO_showmanyc_t, __showmanyc);
    JUMP_FIELD(_IO_imbue_t, __imbue);
#endif
#if 0
    get_column;
    set_column;
#endif
};

/* We always allocate an extra word following an _IO_FILE.
   This contains a pointer to the function jump table used.
   This is for compatibility with C++ streambuf; the word can
   be used to smash to a pointer to a virtual function table. */

struct _IO_FILE_plus
{
  _IO_FILE file;
  const struct _IO_jump_t *vtable;
};

/* Generic functions */

#if defined(_G_IO_IO_FILE_VERSION) && _G_IO_IO_FILE_VERSION == 0x20001
extern _IO_off64_t _IO_seekoff __P ((_IO_FILE *, _IO_off64_t, int, int));
extern _IO_off64_t _IO_seekpos __P ((_IO_FILE *, _IO_off64_t, int));
#else
extern _IO_off_t _IO_seekoff __P ((_IO_FILE *, _IO_off_t, int, int));
extern _IO_off_t _IO_seekpos __P ((_IO_FILE *, _IO_off_t, int));
#endif

extern void _IO_switch_to_main_get_area __P ((_IO_FILE *));
extern void _IO_switch_to_backup_area __P ((_IO_FILE *));
extern int _IO_switch_to_get_mode __P ((_IO_FILE *));
extern void _IO_init __P ((_IO_FILE *, int));
extern int _IO_sputbackc __P ((_IO_FILE *, int));
extern int _IO_sungetc __P ((_IO_FILE *));
extern void _IO_un_link __P ((_IO_FILE *));
extern void _IO_link_in __P ((_IO_FILE *));
extern void _IO_doallocbuf __P ((_IO_FILE *));
extern void _IO_unsave_markers __P ((_IO_FILE *));
extern void _IO_setb __P ((_IO_FILE *, char *, char *, int));
extern unsigned _IO_adjust_column __P ((unsigned, const char *, int));
#define _IO_sputn(__fp, __s, __n) _IO_XSPUTN (__fp, __s, __n)

/* Marker-related function. */

extern void _IO_init_marker __P ((struct _IO_marker *, _IO_FILE *));
extern void _IO_remove_marker __P ((struct _IO_marker *));
extern int _IO_marker_difference __P ((struct _IO_marker *,
				       struct _IO_marker *));
extern int _IO_marker_delta __P ((struct _IO_marker *));
extern int _IO_seekmark __P ((_IO_FILE *, struct _IO_marker *, int));

/* Default jumptable functions. */

extern int _IO_default_underflow __P ((_IO_FILE *));
extern int _IO_default_uflow __P ((_IO_FILE *));
extern int _IO_default_doallocate __P ((_IO_FILE *));
extern void _IO_default_finish __P ((_IO_FILE *, int));
extern int _IO_default_pbackfail __P ((_IO_FILE *, int));
extern _IO_FILE* _IO_default_setbuf __P ((_IO_FILE *, char *, _IO_ssize_t));
extern _IO_size_t _IO_default_xsputn __P ((_IO_FILE *, const void *,
					   _IO_size_t));
extern _IO_size_t _IO_default_xsgetn __P ((_IO_FILE *, void *, _IO_size_t));
#if defined(_G_IO_IO_FILE_VERSION) && _G_IO_IO_FILE_VERSION == 0x20001
extern _IO_off64_t _IO_default_seekoff __P ((_IO_FILE *,
					      _IO_off64_t, int, int));
extern _IO_off64_t _IO_default_seekpos __P ((_IO_FILE *,
					      _IO_off64_t, int));
#else
extern _IO_off_t _IO_default_seekoff __P ((_IO_FILE *, _IO_off_t, int, int));
extern _IO_off_t _IO_default_seekpos __P ((_IO_FILE *, _IO_off_t, int));
#endif
extern _IO_ssize_t _IO_default_write __P ((_IO_FILE *, const void *,
					   _IO_ssize_t));
extern _IO_ssize_t _IO_default_read __P ((_IO_FILE *, void *, _IO_ssize_t));
extern int _IO_default_stat __P ((_IO_FILE *, void *));
#if defined(_G_IO_IO_FILE_VERSION) && _G_IO_IO_FILE_VERSION == 0x20001
extern _IO_off64_t _IO_default_seek __P ((_IO_FILE *, _IO_off64_t, int));
#else
extern _IO_off_t _IO_default_seek __P ((_IO_FILE *, _IO_off_t, int));
#endif
extern int _IO_default_sync __P ((_IO_FILE *));
#define _IO_default_close ((_IO_close_t) _IO_default_sync)

extern struct _IO_jump_t _IO_file_jumps;
extern struct _IO_jump_t _IO_streambuf_jumps;
extern struct _IO_jump_t _IO_proc_jumps;
extern struct _IO_jump_t _IO_str_jumps;
extern int _IO_do_write __P ((_IO_FILE *, const char *, _IO_size_t));
extern int _IO_flush_all __P ((void));
extern void _IO_cleanup __P ((void));
extern void _IO_flush_all_linebuffered __P ((void));

#define _IO_do_flush(_f) \
  _IO_do_write(_f, (_f)->_IO_write_base, \
	       (_f)->_IO_write_ptr-(_f)->_IO_write_base)
#define _IO_in_put_mode(_fp) ((_fp)->_flags & _IO_CURRENTLY_PUTTING)
#define _IO_mask_flags(fp, f, mask) \
       ((fp)->_flags = ((fp)->_flags & ~(mask)) | ((f) & (mask)))
#define _IO_setg(fp, eb, g, eg)  ((fp)->_IO_read_base = (eb),\
	(fp)->_IO_read_ptr = (g), (fp)->_IO_read_end = (eg))
#define _IO_setp(__fp, __p, __ep) \
       ((__fp)->_IO_write_base = (__fp)->_IO_write_ptr = __p, (__fp)->_IO_write_end = (__ep))
#define _IO_have_backup(fp) ((fp)->_IO_save_base != NULL)
#define _IO_in_backup(fp) ((fp)->_flags & _IO_IN_BACKUP)
#define _IO_have_markers(fp) ((fp)->_markers != NULL)
#define _IO_blen(fp) ((fp)->_IO_buf_end - (fp)->_IO_buf_base)

/* Jumptable functions for files. */

extern int _IO_file_doallocate __P ((_IO_FILE *));
extern _IO_FILE* _IO_file_setbuf __P ((_IO_FILE *, char *, _IO_ssize_t));
#if defined(_G_IO_IO_FILE_VERSION) && _G_IO_IO_FILE_VERSION == 0x20001
extern _IO_off64_t _IO_file_seekoff __P ((_IO_FILE *, _IO_off64_t, int, int));
extern _IO_off64_t _IO_file_seek __P ((_IO_FILE *, _IO_off64_t, int));
#else
extern _IO_off_t _IO_file_seekoff __P ((_IO_FILE *, _IO_off_t, int, int));
extern _IO_off_t _IO_file_seek __P ((_IO_FILE *, _IO_off_t, int));
#endif
extern _IO_size_t _IO_file_xsputn __P ((_IO_FILE *, const void *, _IO_size_t));
extern int _IO_file_stat __P ((_IO_FILE *, void *));
extern int _IO_file_close __P ((_IO_FILE *));
extern int _IO_file_underflow __P ((_IO_FILE *));
extern int _IO_file_overflow __P ((_IO_FILE *, int));
#define _IO_file_is_open(__fp) ((__fp)->_fileno >= 0)
extern void _IO_file_init __P ((_IO_FILE *));
extern _IO_FILE* _IO_file_attach __P ((_IO_FILE *, int));
extern _IO_FILE* _IO_file_open __P ((_IO_FILE *, const char *, int, int,
				     int, int));
#if _G_IO_IO_FILE_VERSION == 0x20001
extern _IO_FILE* _IO_file_fopen __P ((_IO_FILE *, const char *, const char *,
				      int));
#else
extern _IO_FILE* _IO_file_fopen __P ((_IO_FILE *, const char *, const char *));
#endif
extern _IO_ssize_t _IO_file_write __P ((_IO_FILE *, const void *,
					_IO_ssize_t));
extern _IO_ssize_t _IO_file_read __P ((_IO_FILE *, void *, _IO_ssize_t));
extern int _IO_file_sync __P ((_IO_FILE *));
extern int _IO_file_close_it __P ((_IO_FILE *));
extern void _IO_file_finish __P ((_IO_FILE *, int));

/* Jumptable functions for proc_files. */
extern _IO_FILE* _IO_proc_open __P ((_IO_FILE *, const char *, const char *));
extern int _IO_proc_close __P ((_IO_FILE *));

/* Jumptable functions for strfiles. */
extern int _IO_str_underflow __P ((_IO_FILE *));
extern int _IO_str_overflow __P ((_IO_FILE *, int));
extern int _IO_str_pbackfail __P ((_IO_FILE *, int));
#if defined(_G_IO_IO_FILE_VERSION) && _G_IO_IO_FILE_VERSION == 0x20001
extern _IO_off64_t _IO_str_seekoff __P ((_IO_FILE *, _IO_off64_t, int, int));
#else
extern _IO_off_t _IO_str_seekoff __P ((_IO_FILE *, _IO_off_t, int, int));
#endif
extern void _IO_str_finish __P ((_IO_FILE *, int));

/* Other strfile functions */
extern void _IO_str_init_static __P ((_IO_FILE *, char *, int, char *));
extern void _IO_str_init_readonly __P ((_IO_FILE *, const char *, int));
extern _IO_ssize_t _IO_str_count __P ((_IO_FILE *));

extern int _IO_vasprintf __P ((char **result_ptr, __const char *format,
			       _IO_va_list args));
extern int _IO_vdprintf __P ((int d, __const char *format, _IO_va_list arg));
extern int _IO_vsnprintf __P ((char *string, _IO_size_t maxlen,
			       __const char *format, _IO_va_list args));


extern _IO_size_t _IO_getline __P ((_IO_FILE *,char *, _IO_size_t, int, int));
extern _IO_size_t _IO_getline_info __P ((_IO_FILE *,char *, _IO_size_t,
					 int, int, int *));
extern _IO_ssize_t _IO_getdelim __P ((char **, _IO_size_t *, int, _IO_FILE *));
extern double _IO_strtod __P ((const char *, char **));
extern char *_IO_dtoa __P ((double __d, int __mode, int __ndigits,
			    int *__decpt, int *__sign, char **__rve));
extern int _IO_outfloat __P ((double __value, _IO_FILE *__sb, int __type,
			      int __width, int __precision, int __flags,
			      int __sign_mode, int __fill));

extern _IO_FILE *_IO_list_all;
extern void (*_IO_cleanup_registration_needed) __PMT ((void));

#ifndef EOF
# define EOF (-1)
#endif
#ifndef NULL
# if defined __GNUG__ && \
    (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8))
#  define NULL (__null)
# else
#  if !defined(__cplusplus)
#   define NULL ((void*)0)
#  else
#   define NULL (0)
#  endif
# endif
#endif

#if _G_HAVE_MMAP

# include <unistd.h>
# include <fcntl.h>
# include <sys/mman.h>
# include <sys/param.h>

# if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#  define MAP_ANONYMOUS MAP_ANON
# endif

# if !defined(MAP_ANONYMOUS) || !defined(EXEC_PAGESIZE)
#  undef _G_HAVE_MMAP
#  define _G_HAVE_MMAP 0
# endif

#endif /* _G_HAVE_MMAP */

#if _G_HAVE_MMAP

# ifdef _LIBC
/* When using this code in the GNU libc we must not pollute the name space.  */
#  define mmap __mmap
#  define munmap __munmap
# endif

# define ROUND_TO_PAGE(_S) \
       (((_S) + EXEC_PAGESIZE - 1) & ~(EXEC_PAGESIZE - 1))

# define FREE_BUF(_B, _S) \
       munmap ((_B), ROUND_TO_PAGE (_S))
# define ALLOC_BUF(_B, _S, _R) \
       do {								      \
	  (_B) = (char *) mmap (0, ROUND_TO_PAGE (_S),			      \
				PROT_READ | PROT_WRITE,			      \
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);	      \
	  if ((_B) == (char *) -1)					      \
	    return (_R);						      \
       } while (0)

#else /* _G_HAVE_MMAP */

# define FREE_BUF(_B, _S) \
       free(_B)
# define ALLOC_BUF(_B, _S, _R) \
       do {								      \
	  (_B) = (char*)malloc(_S);					      \
	  if ((_B) == NULL)						      \
	    return (_R);						      \
       } while (0)

#endif /* _G_HAVE_MMAP */

#ifndef OS_FSTAT
# define OS_FSTAT fstat
#endif
struct stat;
extern _IO_ssize_t _IO_read __P ((int, void *, _IO_size_t));
extern _IO_ssize_t _IO_write __P ((int, const void *, _IO_size_t));
extern _IO_off_t _IO_lseek __P ((int, _IO_off_t, int));
extern int _IO_close __P ((int));
extern int _IO_fstat __P ((int, struct stat *));
extern int _IO_vscanf __P ((const char *, _IO_va_list));

/* Operations on _IO_off_t.
   Normally, these are trivial, but we provide hooks for configurations
   where an _IO_off_t is a struct.
   Note that _IO_off_t must be an integral type. */

/* _IO_pos_BAD is an _IO_off_t value indicating error, unknown, or EOF. */
#ifndef _IO_pos_BAD
# if defined(_G_IO_IO_FILE_VERSION) && _G_IO_IO_FILE_VERSION == 0x20001
#  define _IO_pos_BAD ((_IO_off64_t) -1)
# else
#  define _IO_pos_BAD ((_IO_off_t) -1)
# endif
#endif
/* _IO_pos_as_off converts an _IO_fpos_t value to an _IO_off_t value. */
#ifndef _IO_pos_as_off
# if defined(_G_IO_IO_FILE_VERSION) && _G_IO_IO_FILE_VERSION == 0x20001
#  define _IO_pos_as_off(__pos) ((_IO_off64_t) (__pos))
# else
#  define _IO_pos_as_off(__pos) ((_IO_off_t) (__pos))
# endif
#endif
/* _IO_pos_adjust adjust an _IO_fpos_t by some number of bytes. */
#ifndef _IO_pos_adjust
# define _IO_pos_adjust(__pos, __delta) ((__pos) += (__delta))
#endif
/* _IO_pos_0 is an _IO_off_t value indicating beginning of file. */
#ifndef _IO_pos_0
# if defined(_G_IO_IO_FILE_VERSION) && _G_IO_IO_FILE_VERSION == 0x20001
#  define _IO_pos_0 ((_IO_off64_t) 0)
# else
#  define _IO_pos_0 ((_IO_off_t) 0)
# endif
#endif

#ifdef __cplusplus
}
#endif

#ifdef _IO_MTSAFE_IO
/* check following! */
# define FILEBUF_LITERAL(CHAIN, FLAGS, FD) \
       { _IO_MAGIC+_IO_LINKED+_IO_IS_FILEBUF+FLAGS, \
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CHAIN, FD, \
	   0, 0, 0, 0, { 0 }, &_IO_stdfile_##FD##_lock }
#else
/* check following! */
# define FILEBUF_LITERAL(CHAIN, FLAGS, FD) \
       { _IO_MAGIC+_IO_LINKED+_IO_IS_FILEBUF+FLAGS, \
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CHAIN, FD }
#endif

/* VTABLE_LABEL defines NAME as of the CLASS class.
   CNLENGTH is strlen(#CLASS).  */
#ifdef __GNUC__
# if (!defined(__GXX_ABI_VERSION) || __GXX_ABI_VERSION < 100)
#  if _G_VTABLE_LABEL_HAS_LENGTH
#   define VTABLE_LABEL(NAME, CLASS, CNLENGTH) \
  extern char NAME[] asm (_G_VTABLE_LABEL_PREFIX #CNLENGTH #CLASS);
#  else
#   define VTABLE_LABEL(NAME, CLASS, CNLENGTH) \
  extern char NAME[] asm (_G_VTABLE_LABEL_PREFIX #CLASS);
#  endif
# else
#  define VTABLE_LABEL(NAME, CLASS, CNLENGTH) \
  extern char NAME[] asm ("_ZTV" #CNLENGTH #CLASS);
# endif /* (!defined(__GXX_ABI_VERSION) || __GXX_ABI_VERSION < 100) */
#endif /* __GNUC__ */

#if !defined(builtinbuf_vtable) && defined(__cplusplus)
# ifdef __GNUC__
VTABLE_LABEL(builtinbuf_vtable, builtinbuf, 10)
# else
#  if _G_VTABLE_LABEL_HAS_LENGTH
#   define builtinbuf_vtable _G_VTABLE_LABEL_PREFIX_ID##10builtinbuf
#  else
#   define builtinbuf_vtable _G_VTABLE_LABEL_PREFIX_ID##builtinbuf
#  endif
# endif
#endif /* !defined(builtinbuf_vtable) && defined(__cplusplus) */

#if defined(__STDC__) || defined(__cplusplus)
# define _IO_va_start(args, last) va_start(args, last)
#else
# define _IO_va_start(args, last) va_start(args)
#endif

extern struct _IO_fake_stdiobuf _IO_stdin_buf, _IO_stdout_buf, _IO_stderr_buf;

#if 1
# define COERCE_FILE(FILE) /* Nothing */
#else
/* This is part of the kludge for binary compatibility with old stdio. */
# define COERCE_FILE(FILE) \
  (((FILE)->_IO_file_flags & _IO_MAGIC_MASK) == _OLD_MAGIC_MASK \
    && (FILE) = *(FILE**)&((int*)fp)[1])
#endif

#ifdef EINVAL
# define MAYBE_SET_EINVAL __set_errno (EINVAL)
#else
# define MAYBE_SET_EINVAL /* nothing */
#endif

#ifdef IO_DEBUG
# define CHECK_FILE(FILE, RET) \
	if ((FILE) == NULL) { MAYBE_SET_EINVAL; return RET; } \
	else { COERCE_FILE(FILE); \
	       if (((FILE)->_IO_file_flags & _IO_MAGIC_MASK) != _IO_MAGIC) \
	  { MAYBE_SET_EINVAL; return RET; }}
#else
# define CHECK_FILE(FILE, RET) COERCE_FILE (FILE)
#endif
