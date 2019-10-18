/*
 * Zebra configuration command interface routine
 * Copyright (C) 1997, 98 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
 * 
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _ZEBRA_COMMAND_H
#define _ZEBRA_COMMAND_H

#include "vector.h"
#include "vty.h"

/* Host configuration variable */
struct host
{
  /* Host name of this router. */
  char *name;

  /* password */
  char *password;
  char *password_encrypt;

  /* Enable password */
  char *enable;
  char *enable_encrypt;
  
  char *config;
  int trytimes;

  int chpasswd;
};

/* There are some command levels which called from command node. */
enum node_type 
{
	AUTH_NODE,			/* Authentication mode of vty interface. */
	VIEW_NODE,			/* View node. Default mode of vty interface. */
	AUTH_ENABLE_NODE,	/* Authentication mode for change enable. */
	ENABLE_NODE,		/* Enable node. */
	CONFIG_NODE,		/* Config node. Default mode of config file. */
    SHELL_NODE,
	IPSEC_NODE,
	IPSEC_S_NODE,
	IPSEC_C_NODE,
	CONN_S_NODE,
	CONN_C_NODE,

	NAC_NODE,			/* for nac */
	USER_NODE,			/* for user */
	VTY_NODE			/* Vty node. */
};

/* Node which has some commands and prompt string and configuration
   function pointer . */
struct cmd_node 
{
  /* Node index. */
  enum node_type node;		

  /* Prompt character at vty interface. */
  char *prompt;			

  /* Is this node's configuration goes to vtysh ? */
  int vtysh;
  
  /* Node's configuration write function */
  int (*func) (struct vty *);

  /* Vector of this node's command list. */
  vector cmd_vector;	
};

/* Structure of command element. */
struct cmd_element 
{
  char *string;			/* Command specification by string. */
  int (*func) (struct cmd_element *, struct vty *, int, char **);
  char *doc;			/* Documentation of this command. */
  int daemon;           /* Daemon to which this command belong. */
  vector strvec;		/* Pointing out each description vector. */
  int cmdsize;			/* Command index count. */
  char *config;			/* Configuration string */
  vector subconfig;		/* Sub configuration string */
};

/* Command description structure. */
struct desc
{
  char *cmd;			/* Command string. */
  char *str;			/* Command's description. */
};

/* Return value of the commands. */
#define CMD_SUCCESS              0
#define CMD_WARNING              1
#define CMD_ERR_NO_MATCH         2
#define CMD_ERR_AMBIGUOUS        3
#define CMD_ERR_INCOMPLETE       4
#define CMD_ERR_EXEED_ARGC_MAX   5
#define CMD_ERR_NOTHING_TODO     6
#define CMD_COMPLETE_FULL_MATCH  7
#define CMD_COMPLETE_MATCH       8
#define CMD_COMPLETE_LIST_MATCH  9
#define CMD_SUCCESS_DAEMON      10

/* Argc max counts. */
#define CMD_ARGC_MAX   25


/* DEFUN for vty command interafce. Little bit hacky ;-). */
#define DEFUN(funcname, cmdname, cmdstr, helpstr) \
  static int funcname (struct cmd_element *, struct vty *, int, char **); \
  static struct cmd_element cmdname = \
  { \
    cmdstr, \
    funcname, \
    helpstr \
  }; \
  static int funcname \
  (struct cmd_element *self, struct vty *vty, int argc, char **argv)

/* DEFUN + DEFSH */
#define DEFUNSH(daemon, funcname, cmdname, cmdstr, helpstr) \
  static int funcname (struct cmd_element *, struct vty *, int, char **); \
  static struct cmd_element cmdname = \
  { \
    cmdstr, \
    funcname, \
    helpstr, \
    daemon \
  }; \
  static int funcname \
  (struct cmd_element *self, struct vty *vty, int argc, char **argv)

/* ALIAS macro which define existing command's alias. */
#define ALIAS(funcname, cmdname, cmdstr, helpstr) \
  static struct cmd_element cmdname = \
  { \
    cmdstr, \
    funcname, \
    helpstr \
  };

/* Some macroes */
#define CMD_OPTION(S)   ((S[0]) == '[')
#define CMD_VARIABLE(S) (((S[0]) >= 'A' && (S[0]) <= 'Z') || ((S[0]) == '<'))
#define CMD_VARARG(S)   ((S[0]) == '.')
#define CMD_RANGE(S)	((S[0] == '<'))

#define CMD_IPV4(S)	   ((strcmp ((S), "A.B.C.D") == 0))
#define CMD_IPV4_PREFIX(S) ((strcmp ((S), "A.B.C.D/M") == 0))
#define CMD_IPV6(S)        ((strcmp ((S), "X:X::X:X") == 0))
#define CMD_IPV6_PREFIX(S) ((strcmp ((S), "X:X::X:X/M") == 0))

/* Common descriptions. */
#define SHOW_STR "show running system information\n"
#define IP_STR "ip information set\n"
#define NO_STR "negate a command or set its defaults\n"
#define CLEAR_STR "reset functions\n"
#define SERVICE_ADD_STR "enable service daemon\n"
#define SERVICE_DEL_STR "disable service daemon\n"
//#define IPSEC_STR "ipsec set command\n"
//#define STRONGSWAN_STR "strongswan set command\n"

#ifndef VTY_DIR
#define VTY_DIR ".vtysh"
#endif

#ifndef CONFIG_DIR
#define CONFIG_DIR	".vtysh/config"
#endif

#ifndef CONFIG_FILE
#define	CONFIG_FILE	"asmcli.conf"
#endif

#ifndef RUN_USER
#define RUN_USER	"root"
#endif

char **cmd_complete_command (vector vline, struct vty *vty, int *status);
char *cmd_prompt (enum node_type node);
int cmd_execute_command (vector vline, struct vty *vty, struct cmd_element **cmd);
int cmd_execute_command_strict (vector vline, struct vty *vty, struct cmd_element **cmd);
int cmd_execute_system_command (char *command, int argc, char **argv);
vector cmd_describe_command (vector vline, struct vty *vty, int *status);
vector cmd_make_strvec (char *string);
void cmd_free_strvec (vector v);
void cmd_init ();
void cmd_install_element (enum node_type ntype, struct cmd_element *cmd);
void cmd_install_node (struct cmd_node *node, int (*func) (struct vty *));
void cmd_sort_node ();
void cmd_parse_init();

extern struct cmd_node view_node;
extern struct cmd_node enable_node;
extern struct cmd_node config_node;
extern struct cmd_node shell_node;
extern struct cmd_node ipsec_node;
extern struct cmd_node ipsec_s_node;
extern struct cmd_node ipsec_c_node;
extern struct cmd_node conn_s_node;
extern struct cmd_node conn_c_node;
extern struct host host;
extern vector cmdvec;

#endif /* _ZEBRA_COMMAND_H */
