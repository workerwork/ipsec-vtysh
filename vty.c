/*
 * Virtual terminal [aka TeletYpe] interface routine.
 * Copyright (C) 1997, 98 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.  
 */


#include "vty.h"
#include "command.h"
#include <stdlib.h>

/* VTY standard output function. */
int
vty_out (struct vty *vty, const char *format, ...)
{
  va_list args;
  
  va_start (args, format);
  vprintf (format, args);
  va_end (args);

  return 0;
}

/* Allocate new vty struct. */
struct vty *
vty_new ()
{
  struct vty *new = XCALLOC (MTYPE_VTY, sizeof (struct vty));

  new->buf = XCALLOC (MTYPE_VTY, VTY_BUFSIZ);

  return new;
}

void vty_destroy(struct vty *vty)
{
  if (vty->buf)
    XFREE (MTYPE_VTY, vty->buf);
  XFREE (MTYPE_VTY, vty);
}