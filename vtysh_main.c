/* Virtual terminal interface shell.
 * Copyright (C) 2000 Kunihiro Ishiguro
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

#include "vtysh.h"
#include "command.h"
#include "vtysh_config.h"
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <libgen.h>

/* Help information display. */
static void usage (char *progname, int status)
{
	printf ("Usage : %s [OPTION...]\n"
		"\t-b, --boot          Execute boot startup configuration\n"
		"\t-e, --eval          Execute argument as command\n"
		"\t-c, --config        Load the config file,default["CONFIG_DIR"/"CONFIG_FILE"]\n"
		"\t-v, --version       Show the version\n"
		"\t-h, --help          Display this help and exit\n", basename(progname));
	exit (status);
}

/* VTY shell options, we use GNU getopt library. */
struct option longopts[] = 
{
	{ "boot",	no_argument,		NULL, 'b'},
	{ "eval",	required_argument,	NULL, 'e'},
	{ "config",	required_argument,	NULL, 'c'},
	{ "version",	no_argument,		NULL, 'v'},
	{ "help",	no_argument,		NULL, 'h'},
	{ 0 }
};

/* Initialization of signal handles. */
#include <readline/readline.h>
#include <readline/history.h>

static void my_sig(int sig)
{
	// ctrl+ c 
	rl_replace_line("", 0);
	rl_crlf();
	rl_forced_update_display();
}

static void signal_init ()
{
  signal (SIGINT, my_sig );
  signal (SIGTSTP, SIG_IGN);
  signal (SIGPIPE, SIG_IGN);

}

static void in_show_welcome()
{
	char build[1024];
	FILE *fp;

	sprintf(build, "Build On %s %s", __DATE__, __TIME__);
	fp = fopen("/BUILD", "r");
	if(fp)
	{
		fgets(build, sizeof(build), fp);
		fclose(fp);
	}
	vty_out (vty, "%s\n", build);
}

/* VTY shell main routine. */
int main (int argc, char **argv, char **env)
{
	char *line;
	int opt;
	int eval_flag = 0;
	int boot_flag = 0;
	char *eval_line = NULL;
	char *config_file = CONFIG_DIR "/" CONFIG_FILE;

	if(getenv("VTYSH_CONFIG"))
		config_file = getenv("VTYSH_CONFIG");
	while (1) 
	{
		opt = getopt_long (argc, argv, "be:c:hv", longopts, 0);
		if (opt == EOF)
			break;
		switch (opt) 
		{
			case 0:
				break;
			case 'b':
				boot_flag = 1;
				break;
			case 'e':
				eval_flag = 1;
				eval_line = optarg;
				break;
			case 'h':
				usage (argv[0], 0);
				break;
			case 'c':
				config_file = optarg;
				break;
			case 'v':
				printf("Ver:%s %s\n", __DATE__, __TIME__);
				exit(0);
			default:
				usage (argv[0], 1);
				break;
		}
	}

	/* Signal and others. */
	signal_init ();

	/* Init config. */
	config_init();

	/* Init the cmd */
	cmd_init();

	/* Init the vtysh */
	vtysh_init_vty ();

	/* Install command and node view */
	cmd_parse_init();

	//TODO load the dynamic so

	/* sort the node */
	cmd_sort_node();

	/* If eval mode */
	if (eval_flag)
	{
		vtysh_execute("enable");
		vtysh_execute("config terminal");
		exit(vtysh_execute(eval_line));
	}

	/* Boot startup configuration file. */
	if (boot_flag)
		exit(vtysh_boot_config (config_file));
	
	in_show_welcome();
	host.config = config_file;
	vtysh_load_config(config_file);

	/* Main command loop. */
	while ((line = vtysh_readline()) != NULL)
		vtysh_execute (line);
	printf ("\n");

	exit (0);
}
