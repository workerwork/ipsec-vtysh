#include "command.h"
#include "vtysh_config.h"
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>

static char *zencrypt(char *pass)
{
	char *crypt(const char *key, const char *salt);
	char salt[6];

	memset(salt, 0, sizeof(salt));
	memcpy(salt, pass, 2);
	
	return crypt(pass, salt);
}

DEFUN (shell,
	   config_shell_cmd,
	   "shell",
	   "turn on shell command\n")
{
		vty->node = SHELL_NODE;

		return CMD_SUCCESS;
}


DEFUN (ipsec,
	   config_ipsec_cmd,
	   "ipsec",
	   "turn on ipsec command\n")
{
		vty->node = IPSEC_NODE;
		
		return CMD_SUCCESS;		
}

//add at here
//
//

/* Enable command */
DEFUN (enable,
       config_enable_cmd,
       "enable",
       "turn on privileged mode command\n")
{
    /* If enable password is NULL, change to ENABLE_NODE */
    if ((host.enable == NULL && host.enable_encrypt == NULL) || vty->type == VTY_FILE)
        vty->node = ENABLE_NODE;
		//vty->node = AUTH_ENABLE_NODE;
    else
        vty->node = AUTH_ENABLE_NODE;

    return CMD_SUCCESS;
}

/* Disable command */
DEFUN (disable, 
       config_disable_cmd,
       "disable",
       "turn off privileged mode command\n")
{
    if (vty->node == ENABLE_NODE)
        vty->node = VIEW_NODE;
    
	return CMD_SUCCESS;
}

/* Down vty node level. */
DEFUN (config_exit,
       config_exit_cmd,
       "exit",
       "exit current mode and down to previous mode\n")
{
    switch (vty->node)
    {
        case VIEW_NODE:
	        exit (0);
	        break;
        case ENABLE_NODE:
            vty->node = VIEW_NODE;
	        break;
        case CONFIG_NODE:
            vty->node = ENABLE_NODE;
            break;
		case SHELL_NODE:
			vty->node = CONFIG_NODE;
			break;
		case IPSEC_NODE:
			vty->node = CONFIG_NODE;
			break;
		case IPSEC_S_NODE:
		case IPSEC_C_NODE:
			vty->node = IPSEC_NODE;
			break;
		case CONN_S_NODE:
			vty->node = IPSEC_S_NODE;
			break;
		case CONN_C_NODE:
			vty->node = IPSEC_C_NODE;
			break;
        default:
            break;
    }

    return CMD_SUCCESS;
}

DEFUN (config_quit,
	config_quit_cmd,
	"quit",
	"end the current session\n")
{
	exit(0);
	return CMD_SUCCESS;
}

/* Show version. */
DEFUN (show_version,
       show_version_cmd,
       "show version",
	   SHOW_STR
       "displays the version information\n")
{
	char info[1024];

	FILE *fp;
	if(access(VTY_DIR"/COPYRIGHT", R_OK) == 0)
	{
		system("cat "VTY_DIR"/COPYRIGHT");
	}
	else
	{
		sprintf(info, "Copyright(c) 2006-2018 beijing baicells Tech. Co., Ltd.");
		vty_out (vty, "%s\n", info);
	}

	fp = fopen(VTY_DIR"/BUILD", "r");
	if(fp)
	{
		fgets(info, sizeof(info), fp);
		fclose(fp);
		vty_out (vty, "%s", info);
	}
	else
	{
		sprintf(info, "Build on %s %s", __DATE__, __TIME__);
		vty_out (vty, "%s\n", info);
	}
	//vty_out (vty, "%s\n", info);
	
	return CMD_SUCCESS;
}

/* Configration from terminal */
DEFUN (config_terminal,
       config_terminal_cmd,
       "configure terminal",
       "configuration from vty interface\n"
       "configuration terminal\n")
{
    vty->node = CONFIG_NODE;
    
	return CMD_SUCCESS;
}

/* Help display function for all node. */
DEFUN (config_list,
       config_list_cmd,
       "list",
       "print command list\n")
{
    int i;
    struct cmd_node *cnode = vector_slot (cmdvec, vty->node);
    struct cmd_element *cmd;

    for (i = 0; i < vector_max (cnode->cmd_vector); i++)
        if ((cmd = vector_slot (cnode->cmd_vector, i)) != NULL)
            vty_out (vty, "  %s%s", cmd->string, VTY_NEWLINE);
  
	return CMD_SUCCESS;
}

/* Write current configuration into file. */
DEFUN (config_write_file, 
       config_write_file_cmd,
       "write file",  
       "write running configuration to memory, network, or terminal\n"
       "write to configuration file\n")
{
	FILE *fp;

	if (host.config == NULL)
	{
		vty_out (vty, "Can't save to configuration file, using vtysh.%s", VTY_NEWLINE);
		
		return CMD_WARNING;
	}
	fp = fopen(host.config, "w");
	if(fp == NULL)
	{
	    vty_out (vty, "Can't open configuration file %s.%s", host.config, VTY_NEWLINE);
      	
		return CMD_WARNING;
    }
  
	config_dump(fp);
	fflush(fp);
	vty_out (vty, "Configuration saved SUCCESS %s", VTY_NEWLINE);

	if(host.chpasswd)
	{
		char szCmd[1024];
		char password[1024];
		
		strcpy(password, host.password);
		sprintf(szCmd, "echo \"%s:%s\" | chpasswd  ",  RUN_USER, password);
		system(szCmd);
		host.chpasswd = 0;
	}

	return CMD_SUCCESS;
}

ALIAS (config_write_file, 
       config_write_cmd,
       "write",  
       "write running configuration to memory, network, or terminal\n"
       "write configureation to the file(same as write file)\n");

ALIAS (config_write_file, 
       config_write_memory_cmd,
       "write memory",  
       "write running configuration to memory, network, or terminal\n"
       "write configuration to the file (same as write file)\n");



/* Write current configuration into the terminal. */
DEFUN (config_write_terminal,
       config_write_terminal_cmd,
       "write terminal",
       "write running configuration to memory, network, or terminal\n"
       "write to terminal\n")
{
    config_dump(stderr);
    
	return CMD_SUCCESS;
}

/* Write current configuration into the terminal. */
ALIAS (config_write_terminal,
       show_running_config_cmd,
       "show running-config",
	   SHOW_STR
       "running configuration\n");

/* Write startup configuration into the terminal. */
DEFUN (show_startup_config,
       show_startup_config_cmd,
       "show startup-config",
	   SHOW_STR
       "contentes of startup configuration\n")
{
    char buf[BUFSIZ];
    FILE *confp;

    confp = fopen (host.config, "r");
    if (confp == NULL)
    {
        vty_out (vty, "Can't open configuration file [%s]%s", host.config, VTY_NEWLINE);
        
		return CMD_WARNING;
    }

    while (fgets (buf, BUFSIZ, confp))
    {
        char *cp = buf;

        while (*cp != '\r' && *cp != '\n' && *cp != '\0')
	        cp++;
        *cp = '\0';

        vty_out (vty, "%s%s", buf, VTY_NEWLINE);
    }

    fclose (confp);

    return CMD_SUCCESS;
}

/* Hostname configuration */
DEFUN (config_hostname, 
       hostname_cmd,
       "hostname WORD",
       "set system's network name\n"
       "this system's network name\n")
{
	if (!isalpha((int) *argv[0]))
	{
	    vty_out (vty, "Please specify string starting with alphabet%s", VTY_NEWLINE);
	  
		return CMD_WARNING;
	}

	config_del_line_byleft(config_top, "hostname");
	config_add_line(config_top, "hostname %s", argv[0]);
	ENSURE_CONFIG(vty);

	if (host.name)
	    XFREE (0, host.name);
	host.name = strdup (argv[0]);

	sethostname(host.name, strlen(host.name));
	
	return CMD_SUCCESS;
}

DEFUN (config_no_hostname, 
       no_hostname_cmd,
       "no hostname [HOSTNAME]",
       "negate a command or set its defaults"
       "reset system's network name\n"
       "host name of this router\n")
{
    if (host.name)
        XFREE (0, host.name);
    host.name = NULL;
    config_del_line_byleft(config_top, "hostname ");
  
	return CMD_SUCCESS;
}

/* Hostname configuration */
DEFUN (config_boot_image, 
       config_boot_image_cmd,
       "boot WORD",
       "use the new image to boot device\n"
       "the new image name\n")
{
	char *myargv[10];
	char filename[1024];

	sprintf(filename, "%s/%s", CONFIG_DIR, argv[0]);
	if(access(filename, R_OK) != 0)
	{
		vty_out(vty, "the image file '%s' dose not exits\n", argv[0]);
		
		return CMD_ERR_NOTHING_TODO;
	}
	myargv[0] = filename;
	
	return cmd_execute_system_command("switchasm", 1, myargv);
}

/* VTY enable password set. */
DEFUN (config_enable_password, enable_password_cmd,
       "enable password (8|) WORD",
       "modify enable password parameters\n"
       "assign the privileged level password\n"
       "specifies a HIDDEN password will follow\n"
       "the HIDDEN 'enable' password string\n")
{
	/* Argument check. */
	if (argc == 0)
	{
		vty_out (vty, "Please specify password.%s", VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/* Crypt type is specified. */
	if (argc == 2)
	{
		if (*argv[0] == '8')
		{
			if (host.enable)
				XFREE (0, host.enable);
			host.enable = NULL;

			if (host.enable_encrypt)
				XFREE (0, host.enable_encrypt);
			host.enable_encrypt = XSTRDUP (0, argv[1]);

			config_del_line_byleft(config_top, "enable password");
			config_add_line(config_top, "enable password 8 %s", host.enable_encrypt);
			
			return CMD_SUCCESS;
		}
		else
		{
			vty_out (vty, "Unknown encryption type.%s", VTY_NEWLINE);
			
			return CMD_WARNING;
		}
	}

	if (!isalnum ((int) *argv[0]))
	{
		vty_out (vty, "Please specify string starting with alphanumeric%s", VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	if (strlen(argv[0]) < 2)
	{
		vty_out (vty, "Please specify a long password%s", VTY_NEWLINE);
		
		return CMD_WARNING;
	}


	if (host.enable)
		XFREE (0, host.enable);
	host.enable = NULL;

	if (host.enable_encrypt)
		XFREE (0, host.enable_encrypt);
	host.enable_encrypt = NULL;

	/* Plain password input. */
	config_del_line_byleft(config_top, "enable password");
	host.enable_encrypt = XSTRDUP (0, zencrypt (argv[0]));
	config_add_line(config_top, "enable password 8 %s", host.enable_encrypt);
	
	return CMD_SUCCESS;
}

ALIAS (config_enable_password,
       enable_password_text_cmd,
       "enable password LINE",
       "modify enable password parameters\n"
       "assign the privileged level password\n"
       "the UNENCRYPTED (cleartext) 'enable' password\n");

/* VTY enable password delete. */
DEFUN (no_config_enable_password, no_enable_password_cmd,
       "no enable password",
       "negate a command or set its defaults"
       "modify enable password parameters\n"
       "assign the privileged level password\n")
{
	config_del_line_byleft(config_top, "enable password");

	if (host.enable)
		XFREE (0, host.enable);
	host.enable = NULL;

	if (host.enable_encrypt)
		XFREE (0, host.enable_encrypt);
	host.enable_encrypt = NULL;
  	
  	return CMD_SUCCESS;
}

DEFUN (config_password, password_cmd,
       "password (8|) WORD",
       "modify password parameters\n"
       "specifies a HIDDEN password will follow\n"
       "the HIDDEN password string\n")
{
	/* Argument check. */
	if (argc == 0)
	{
		vty_out (vty, "Please specify password.%s", VTY_NEWLINE);
		
		return CMD_WARNING;
	}

	/* Crypt type is specified. */
	if (argc == 2)
	{
		if (*argv[0] == '8')
		{
			if (host.password)
				XFREE (0, host.password);
			host.password = NULL;

			if (host.password_encrypt)
				XFREE (0, host.password_encrypt);
			host.password_encrypt = XSTRDUP (0, argv[1]);

			config_del_line_byleft(config_top, "password");
			config_add_line(config_top, "password 8 %s", host.password_encrypt);
			
			return CMD_SUCCESS;
		}
		else
		{
			vty_out (vty, "Unknown encryption type.%s", VTY_NEWLINE);
			
			return CMD_WARNING;
		}
	}

	if (!isalnum ((int) *argv[0]))
	{
		vty_out (vty, "Please specify string starting with alphanumeric%s", VTY_NEWLINE);
		
		return CMD_WARNING;
	}
	if(strchr(argv[0], '"') || strchr(argv[0], '`') || strchr(argv[0], '<') 
		|| strchr(argv[0], '>') || strchr(argv[0], '|') )
	{
		vty_out(vty, "Password can't include special char\n");
		
		return CMD_WARNING;
	}
	if (strlen(argv[0]) < 2)
	{
		vty_out (vty, "Please specify a long password%s", VTY_NEWLINE);
		
		return CMD_WARNING;
	}
	if (host.password)
		XFREE (0, host.password);
	host.password = NULL;

	if (host.password_encrypt)
		XFREE (0, host.password_encrypt);
	host.password_encrypt = NULL;

	/* Plain password input. */
	config_del_line_byleft(config_top, "password ");
	host.password = XSTRDUP(0, argv[0]);
	host.password_encrypt = XSTRDUP (0, zencrypt (argv[0]));
	config_add_line(config_top, "password 8 %s", host.password_encrypt);
	
	ENSURE_CONFIG(vty);
	host.chpasswd = 1;
	
	return CMD_SUCCESS;
}

ALIAS (config_password,
       password_text_cmd,
       "password LINE",
       "modify password parameters\n"
       "the UNENCRYPTED (cleartext) password\n");



int cmd_common_init()
{
    /* Each node's basic commands. */
	cmd_install_element (VIEW_NODE, &show_version_cmd);
    cmd_install_element (VIEW_NODE, &config_enable_cmd);
    cmd_install_element (VIEW_NODE, &config_exit_cmd);
	cmd_install_element (VIEW_NODE, &config_quit_cmd);
    cmd_install_element (VIEW_NODE, &config_list_cmd);

	cmd_install_element (ENABLE_NODE, &show_version_cmd);
    cmd_install_element (ENABLE_NODE, &config_exit_cmd);
    cmd_install_element (ENABLE_NODE, &config_quit_cmd);
	cmd_install_element (ENABLE_NODE, &config_list_cmd);
    cmd_install_element (ENABLE_NODE, &config_disable_cmd);
    cmd_install_element (ENABLE_NODE, &config_terminal_cmd);
    cmd_install_element (ENABLE_NODE, &config_write_terminal_cmd);
    cmd_install_element (ENABLE_NODE, &config_write_file_cmd);
    cmd_install_element (ENABLE_NODE, &config_write_memory_cmd);
    cmd_install_element (ENABLE_NODE, &config_write_cmd);
    cmd_install_element (ENABLE_NODE, &show_running_config_cmd);
    cmd_install_element (ENABLE_NODE, &show_startup_config_cmd);
    cmd_install_element (ENABLE_NODE, &config_boot_image_cmd);

    cmd_install_element (CONFIG_NODE, &show_version_cmd);
	cmd_install_element (CONFIG_NODE, &config_exit_cmd);
	cmd_install_element (CONFIG_NODE, &config_quit_cmd);
	cmd_install_element (CONFIG_NODE, &config_list_cmd);
    cmd_install_element (CONFIG_NODE, &config_write_terminal_cmd);
    cmd_install_element (CONFIG_NODE, &config_write_file_cmd);
    cmd_install_element (CONFIG_NODE, &config_write_memory_cmd);
    cmd_install_element (CONFIG_NODE, &config_write_cmd);
	cmd_install_element (CONFIG_NODE, &hostname_cmd);
	cmd_install_element (CONFIG_NODE, &no_hostname_cmd);
	cmd_install_element (CONFIG_NODE, &password_cmd);
	cmd_install_element (CONFIG_NODE, &password_text_cmd);
	cmd_install_element (CONFIG_NODE, &enable_password_cmd);
	cmd_install_element (CONFIG_NODE, &enable_password_text_cmd);
	cmd_install_element (CONFIG_NODE, &no_enable_password_cmd);
	cmd_install_element (CONFIG_NODE, &show_running_config_cmd);
    cmd_install_element (CONFIG_NODE, &show_startup_config_cmd);

	cmd_install_element (CONFIG_NODE, &config_shell_cmd);
	cmd_install_element (SHELL_NODE, &config_exit_cmd);
	cmd_install_element (SHELL_NODE, &config_quit_cmd);

	cmd_install_element (CONFIG_NODE, &config_ipsec_cmd);
	cmd_install_element (IPSEC_NODE, &config_exit_cmd);
	cmd_install_element (IPSEC_NODE, &config_quit_cmd);
	cmd_install_element (IPSEC_S_NODE, &config_exit_cmd);
	cmd_install_element (IPSEC_S_NODE, &config_quit_cmd);
	cmd_install_element (IPSEC_C_NODE, &config_exit_cmd);
	cmd_install_element (IPSEC_C_NODE, &config_quit_cmd);
	cmd_install_element (CONN_S_NODE, &config_exit_cmd);
	cmd_install_element (CONN_S_NODE, &config_quit_cmd);
	cmd_install_element (CONN_C_NODE, &config_exit_cmd);
	cmd_install_element (CONN_C_NODE, &config_quit_cmd);

	return 0;
}

