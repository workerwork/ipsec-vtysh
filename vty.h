/* Virtual terminal [aka TeletYpe] interface routine
   Copyright (C) 1997 Kunihiro Ishiguro

This file is part of GNU Zebra.

GNU Zebra is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Zebra is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Zebra; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#ifndef _ZEBRA_VTY_H
#define _ZEBRA_VTY_H

#define VTY_BUFSIZ 512

#include "vector.h"

/* VTY struct. */
struct vty 
{
  /* Is this vty connect to file or not */
  enum {VTY_SHELL, VTY_FILE} type;

  /* Node status of this vty */
  int node;

  /* Command input buffer */
  char *buf;
};

/* Small macro to determine newline is newline only or linefeed needed. */
#define VTY_NEWLINE  "\n"

struct vty *vty_new (void);
void vty_destroy(struct vty *vty);

int vty_out (struct vty *, const char *, ...);

#endif /* _ZEBRA_VTY_H */
