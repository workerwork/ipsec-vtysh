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

#include <stdio.h>
#include <string.h>
#include <sys/un.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <unistd.h>
#include "command.h"
#include "memory.h"
#include "vtysh.h"

#include <readline/readline.h>
#include <readline/history.h>

/* Struct VTY. */
struct vty *vty;

/* add by dongfeng */
#include <termios.h>
extern char ch1[10];
extern char ch2[10];
extern char ch3[100];
char ch1[10] = "";
char ch2[10] = "";
char ch3[100] = "";

void hide_input_echo()
{
	struct termios savetty;
	int ret = 0;
	tcgetattr(fileno(stdout),&savetty);
	savetty.c_lflag &= ~ECHO;
	ret = tcsetattr(fileno(stdout),TCSANOW,&savetty);
	return;
}

void cancel_input_echo()
{
	struct termios savetty;
	int ret = 0;
	tcgetattr(fileno(stdout),&savetty);
	savetty.c_lflag |= ECHO;
	ret = tcsetattr(fileno(stdout),TCSANOW,&savetty);
	return;
}
/* end of add */


/* Command execution over the vty interface. */
static int vtysh_execute_func (char *line, int pager)
{
    int ret;
    vector vline;
    struct cmd_element *cmd;

    /* Split readline string up into the vector */
    vline = cmd_make_strvec (line);

    if (vline == NULL)
        return -1;

    ret = cmd_execute_command (vline, vty, &cmd);

    cmd_free_strvec (vline);
	/* mod by dongfeng */
    switch (ret)
    {
        case CMD_WARNING:
            //printf ("Warning...\n");
            break;
        case CMD_ERR_AMBIGUOUS:
			//printf ("%% Ambiguous command.\n");
            printf ("\x1b[31m%% Ambiguous command.\x1b[0m\n");
            break;
        case CMD_ERR_NO_MATCH:
			//printf ("%% Unknown command.\n");
            printf ("\x1b[31m%% Unknown command.\x1b[0m\n");
            break;
        case CMD_ERR_INCOMPLETE:
			//printf ("%% Command incomplete.\n");
            printf ("\x1b[31m%% Command incomplete.\x1b[0m\n");
            break;
        case CMD_SUCCESS_DAEMON:
            {
                if (cmd->func)
                    (*cmd->func) (cmd, vty, 0, NULL);
            }
    }
	/* end of mod */
    return ret;
}

static int in_is_valid_password(char *passwd)
{
        char salt[6];
        char *crypt(const char *key, const char *salt);

	memset(salt, 0, sizeof(salt));
	memcpy(salt, host.enable_encrypt, 2);
	return strcmp(host.enable_encrypt, crypt (passwd, salt));
}

int vtysh_execute (char *line)
{
    if(vty->node == AUTH_ENABLE_NODE)
    {
        if(in_is_valid_password(line) == 0)
        {
            vty->node = ENABLE_NODE;
            host.trytimes = 0;
            return -1;
        }
        if(host.trytimes ++ > 1)
        {
            vty_out (vty, "Authentication failed!\n");
            exit (1);
        }
        vty_out(vty, "Invalid password!\n");
        return -1;
    }
    return vtysh_execute_func (line, 1);
}

/* Configration make from file. */
static int vtysh_config_from_file (struct vty *vty, char *filename)
{
    int ret;
    vector vline;
    struct cmd_element *cmd;
    FILE *fp;

    fp = fopen(filename, "r");
    if(fp == NULL)
        return -1;

    while (fgets (vty->buf, VTY_BUFSIZ, fp))
    {
        if (vty->buf[0] == '#')
            continue;
        if (vty->buf[0] == '!')
            vty->node = CONFIG_NODE;

        vline = cmd_make_strvec (vty->buf);

        /* In case of comment line */
        if (vline == NULL)
            continue;

        /* Execute configuration command : this is strict match */
        ret = cmd_execute_command_strict (vline, vty, &cmd);

        /* Try again with setting node to CONFIG_NODE */
        if (ret != CMD_SUCCESS && ret != CMD_SUCCESS_DAEMON && ret != CMD_WARNING)
        {
            vtysh_execute ("end");
            vtysh_execute ("configure terminal");
            vty->node = CONFIG_NODE;
            ret = cmd_execute_command_strict (vline, vty, &cmd);
        }	  

        cmd_free_strvec (vline);

        switch (ret)
        {
            case CMD_WARNING:
                //printf ("Warning...\n");
                break;
            case CMD_ERR_AMBIGUOUS:
                printf ("%% Ambiguous command.\n");
                break;
            case CMD_ERR_NO_MATCH:
                printf ("%% Unknown command: %s", vty->buf);
                break;
            case CMD_ERR_INCOMPLETE:
                printf ("%% Command incomplete.\n");
                break;
            case CMD_SUCCESS_DAEMON:
                if (cmd->func)
                    (*cmd->func) (cmd, vty, 0, NULL);
                break;
        }
    }
    fclose(fp);
    return CMD_SUCCESS;
}

int vtysh_load_config(char *filename)
{
    struct vty *myvty = vty_new();
    int nRet;

    myvty->type = VTY_FILE;
    myvty->node = CONFIG_NODE;
    nRet = vtysh_config_from_file(myvty, filename);
    vty_destroy(myvty);
    return nRet;
}

int vtysh_boot_config(char *filename)
{
    struct vty *myvty = vty_new();
    int nRet;

    myvty->type = VTY_SHELL;
    myvty->node = CONFIG_NODE;
    nRet = vtysh_config_from_file(myvty, filename);
    vty_destroy(myvty);
    return nRet;

}

/* We don't care about the point of the cursor when '?' is typed. */
static int vtysh_rl_describe ()
{
    int ret;
    int i;
    vector vline;
    vector describe;
    int width;
    struct desc *desc;

    vline = cmd_make_strvec (rl_line_buffer);

    /* In case of '> ?'. */
    if (vline == NULL)
    {
        vline = vector_init (1);
        vector_set (vline, '\0');
    }
    else 
        if (rl_end && isspace ((int) rl_line_buffer[rl_end - 1]))
            vector_set (vline, '\0');

    describe = cmd_describe_command (vline, vty, &ret);

    printf ("\n");

    /* Ambiguous and no match error. */
    switch (ret)
    {
        case CMD_ERR_AMBIGUOUS:
            cmd_free_strvec (vline);
            printf ("%% Ambiguous command.\n");
            rl_on_new_line ();
            return 0;
            break;
        case CMD_ERR_NO_MATCH:
            cmd_free_strvec (vline);
            printf ("%% There is no matched command.\n");
            rl_on_new_line ();
            return 0;
            break;
    }  

    /* Get width of command string. */
    width = 0;
    for (i = 0; i < vector_max (describe); i++)
        if ((desc = vector_slot (describe, i)) != NULL)
        {
            int len;

            if (desc->cmd[0] == '\0')
                continue;

            len = strlen (desc->cmd);
            if (desc->cmd[0] == '.')
                len--;

            if (width < len)
                width = len;
        }

    for (i = 0; i < vector_max (describe); i++)
        if ((desc = vector_slot (describe, i)) != NULL)
        {
            if (desc->cmd[0] == '\0')
                continue;

            if (! desc->str)
                printf ("  %-s\n",
                        desc->cmd[0] == '.' ? desc->cmd + 1 : desc->cmd);
            else
                printf ("  %-*s  %s\n",
                        width,
                        desc->cmd[0] == '.' ? desc->cmd + 1 : desc->cmd,
                        desc->str);
        }

    cmd_free_strvec (vline);
    vector_free (describe);

    rl_on_new_line();

    return 0;
}

/* result of cmd_complete_command() call will be stored here
   and used in new_completion() in order to put the space in
   correct places only */
static int complete_status;

static char * command_generator (char *text, int state)
{
    vector vline;
    static char **matched = NULL;
    static int index = 0;

    /* First call. */
    if (! state)
    {
        index = 0;

        if (vty->node == AUTH_NODE || vty->node == AUTH_ENABLE_NODE)
            return NULL;

        vline = cmd_make_strvec (rl_line_buffer);
        if (vline == NULL)
            return NULL;

        if (rl_end && isspace ((int) rl_line_buffer[rl_end - 1]))
            vector_set (vline, '\0');

        matched = cmd_complete_command (vline, vty, &complete_status);
    }

    if (matched && matched[index])
        return matched[index++];

    return NULL;
}

static char **new_completion (char *text, int start, int end)
{
    char **matches;

    matches = completion_matches (text, command_generator);

    if (matches)
    {
        rl_point = rl_end;
        /*mod by dongfeng*/
		/*if (complete_status == CMD_COMPLETE_FULL_MATCH)
            rl_pending_input = ' ';
		*/
		if ((complete_status == CMD_COMPLETE_MATCH) || (complete_status == CMD_COMPLETE_LIST_MATCH))
		{
			rl_completion_append_character = '\0';
		}
		if ((complete_status == CMD_COMPLETE_MATCH) || (complete_status == CMD_COMPLETE_LIST_MATCH))
		{
			printf("\x1b[0m");	
		}
		else if (complete_status == 2)
		{
			printf("\x1b[31m");
		}
		/*end of mod*/
    }

    return matches;
}

/* To disable readline's filename completion */
static int vtysh_completion_entry_function (int ignore, int invoking_key)
{
    return 0;
}

static void vtysh_readline_init ()
{
    /* readline related settings. */
    rl_bind_key ('?', vtysh_rl_describe);
    rl_completion_entry_function = vtysh_completion_entry_function;
    rl_attempted_completion_function = (CPPFunction *)new_completion;
    /* do not append space after completion. It will be appended
       in new_completion() function explicitly */
    rl_completion_append_character = '\0';

}

static char * vtysh_prompt ()
{
    struct utsname names;
    static char buf[100];
    const char*hostname;
    extern struct host host;

    hostname = host.name;

    if (!hostname)
    {
        uname (&names);
        hostname = names.nodename;
        host.name = strdup (hostname);
    }

    snprintf (buf, sizeof buf, cmd_prompt (vty->node), hostname, ch1, ch2, ch3);

    return buf;
}

void vtysh_init_vty ()
{
    /* Make vty structure. */
    vty = vty_new ();
    vty->type = VTY_SHELL;
    vty->node = VIEW_NODE;

    vtysh_readline_init ();
}

/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
char * vtysh_readline()
{
    static char *line_read = NULL;
    if (line_read)
    {
        free (line_read);
        line_read = NULL;
    }
    line_read = readline (vtysh_prompt ());

    if (line_read && *line_read)
        add_history (line_read);
    return (line_read);
}
