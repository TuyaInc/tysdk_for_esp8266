/*  Customer ID=7011; Build=0x2b6f6; Copyright (C) 2007--2009 Tensilica, Inc. */
/* This is part of libio/iostream, providing -*- C++ -*- input/output.
Copyright (C) 1993 Free Software Foundation

This file is part of the GNU IO Library.  This library is free
software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option)
any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this library; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

As a special exception, if you link this library with files
compiled with a GNU compiler to produce an executable, this does not cause
the resulting executable to be covered by the GNU General Public License.
This exception does not however invalidate any other reasons why
the executable file might be covered by the GNU General Public License. */

/* Written by Per Bothner (bothner@cygnus.com). */

#ifndef _PROCBUF_H
#define _PROCBUF_H
#ifdef __GNUG__
#pragma interface
#endif

#include <streambuf.h>

extern "C++" {
class procbuf : public filebuf {
    /* Following fields must match those in struct _IO_proc_file */
    _IO_pid_t _pid;
    procbuf *_next;
  public:
    procbuf() : filebuf() { }
    procbuf(const char *command, int mode);
    procbuf* open(const char *command, int mode);
    procbuf *close() { return (procbuf*)filebuf::close(); }
    virtual int sys_close();
    ~procbuf();
};
} // extern "C++"

#endif /* !_PROCBUF_H */
