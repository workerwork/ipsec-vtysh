/* Configuration generator.
   Copyright (C) 2000 Kunihiro Ishiguro

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

#define _GNU_SOURCE
 
#include "command.h"
#include "linklist.h"
#include "memory.h"

#include "vtysh.h"
#include <stdarg.h>
#include "vtysh_config.h"

static vector configvec;

struct list *config_top;

static int line_cmp (char *c1, char *c2)
{
  return strcmp (c1, c2);
}
static void line_del (char *line)
{
  XFREE (MTYPE_VTYSH_CONFIG_LINE, line);
}
static struct config *config_new ()
{
  struct config *config;
  config = XCALLOC (MTYPE_VTYSH_CONFIG, sizeof (struct config));
  return config;
}
static int config_cmp (struct config *c1, struct config *c2)
{
  return strcmp (c1->name, c2->name);
}
static void config_del (struct config* config)
{
  list_delete (config->line);
  if (config->name)
    XFREE (MTYPE_VTYSH_CONFIG_LINE, config->name);
  XFREE (MTYPE_VTYSH_CONFIG, config);
}

struct config *config_get (int index, char *line)
{
  struct config *config;
  struct config *config_loop;
  struct list *master;
  struct listnode *nn;

  config = config_loop = NULL;

  master = vector_lookup_ensure (configvec, index);

  if (! master)
    {
      master = list_new ();
      master->del = (void (*) (void *))config_del;
      master->cmp = (int (*)(void *, void *)) config_cmp;
      vector_set_index (configvec, index, master);
    }
  
  LIST_LOOP (master, config_loop, nn)
    {
      if (strcmp (config_loop->name, line) == 0)
	config = config_loop;
    }

  if (! config)
    {
      config = config_new ();
      config->line = list_new ();
      config->line->del = (void (*) (void *))line_del;
      config->line->cmp = (int (*)(void *, void *)) line_cmp;
      config->name = XSTRDUP (MTYPE_VTYSH_CONFIG_LINE, line);
      config->index = index;
      listnode_add (master, config);
    }
  return config;
}

void config_add_line (struct list *config, char *line, ...)
{
	char *info = NULL;
	va_list ap;

	va_start(ap, line);
	if(vasprintf(&info, line, ap) > 0)
	{
		listnode_add (config, XSTRDUP (MTYPE_VTYSH_CONFIG_LINE, info));
		free(info);
	}
	va_end(ap);
}

void config_del_line (struct list *config, char *line)
{
	struct listnode *nn;
	char *pnt;

	LIST_LOOP (config, pnt, nn)
	{
		if (strcmp (pnt, line) == 0)
		{
			listnode_delete (config, pnt);
			line_del(pnt);
			return;
		}
	}
}

void config_del_line_byleft(struct list *config, char *line)
{
	struct listnode *nn;
	char *pnt;

	LIST_LOOP (config, pnt, nn)
	{
		if (strncmp(pnt, line, strlen(line)) == 0)
		{
			listnode_delete (config, pnt);
			line_del(pnt);
			return;
		}
	}
}

char * config_get_line_byleft(struct list *config, char *line)
{
	struct listnode *nn;
	char *pnt;

	LIST_LOOP (config, pnt, nn)
	{
		if (strncmp(pnt, line, strlen(line)) == 0)
		{
			return pnt;
		}
	}

	return NULL;
}

/* Display configuration to file pointer.  */
void config_dump (FILE *fp)
{
	struct listnode *nn;
	struct listnode *nm;
	struct config *config;
	struct list *master;
	char *line;
	int i;

	char timestr[200];
	time_t t;
	struct tm *tmp;

	t = time(NULL);
	tmp = localtime(&t);
	if (tmp == NULL) 
	{
		vty_out(vty, "Get localtime error\n");
		return;
	}
	memset(timestr, 0, sizeof(timestr));
	strftime(timestr, sizeof(timestr), "%c", tmp);
	fprintf(fp,"#Writed on %s\n", timestr);

	LIST_LOOP (config_top, line, nn)
	{
		fprintf (fp, "%s\n", line);
		fflush (fp);
	}
	fprintf (fp, "!\n");
	fflush (fp);

	for (i = 0; i < vector_max (configvec); i++)
	{
		if ((master = vector_slot (configvec, i)) != NULL)
		{
			LIST_LOOP (master, config, nn)
			{
				if(config->line->head)
				{
					fprintf (fp, "%s\n", config->name);
					fflush (fp);

					LIST_LOOP (config->line, line, nm)
					{
						fprintf  (fp, " %s\n", line);
						fflush (fp);
					}
					fprintf (fp, "!\n");
					fflush (fp);
				}
			}
		}
	}
}

void config_init ()
{
  config_top = list_new ();
  config_top->del = (void (*) (void *))line_del;
  config_top->cmp = line_cmp;
  configvec = vector_init (1);
}
