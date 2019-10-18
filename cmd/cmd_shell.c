#include "command.h"
#include "vtysh_config.h"


DEFUN (config_bash,
	   config_bash_cmd,
	   "bash",
	   "turn on bash command\n")
{
		//vty_out (vty, "%s\n", "this is a test\n");
		return system("bash");
		//return CMD_SUCCESS;
}

DEFUN (config_sh,
	   config_sh_cmd,
	   "sh",
	   "turn on sh command\n")
{
		return system("sh");
}

DEFUN (config_tmux,
	   config_tmux_cmd,
	   "tmux",
	   "turn on tmux command\n")
{
		//char *myargv[10];
		//cmd_execute_system_command("tmux", 1, myargv);
		return system("tmux new -A -s gotty");

		//return CMD_SUCCESS;
}

DEFUN (set_gotty_tmux,
	   set_gotty_tmux_cmd,
	   "web tmux set PORT USER:PASSWD",
	   "web client command\n"
	   "web tmux client command\n"
	   "turn on web tmux client command\n"
	   "set the port of web tmux client\n"
	   "set the user and passwd of web tmux client\n")
{
	/* 
	   char *myargv[10];
	   myargv[0] = "-w";
	   myargv[1] = "-p";
	   myargv[2] = argv[0];
	   //myargv[2] = "tmux new -A -s gotty &";
	   myargv[3] = "tmux";
	   myargv[4] = "new";
	   myargv[5] = "-A";
	   myargv[6] = "-s";
	   myargv[7] = "gotty";
	   myargv[8] = "&";

	   cmd_execute_system_command("./gotty", 9, myargv);
	 */
	  char cmd[100];
	  sprintf(cmd, "./gotty -w -p %s -c %s tmux new -A -s gotty &", argv[0], argv[1]);
      system(cmd);
	  //system("./gotty -w -p %s tmux new -A -s gotty &>/dev/null &", myargv);

      return CMD_SUCCESS; 
}

DEFUN (unset_gotty_tmux,
	   unset_gotty_tmux_cmd,
	   "web tmux unset PORT",
	   "web client command\n"
	   "web tmux client command\n"
	   "turn off web tmux client command\n"
	   "set the port of web tmux client\n")
{
		char cmd[100];
		sprintf(cmd, "kill -9 $(ps -ef | grep gotty | awk '/-p %s/{print $2}')", argv[0]);
		system(cmd);
		
		return CMD_SUCCESS;
}

DEFUN (unsetall_gotty_tmux,
	   unsetall_gotty_tmux_cmd,
	   "web tmux unset all",
	   "web client command\n"
	   "web tmux client command\n"
	   "turn off web tmux client command\n"
	   "turn off all web tmux client command\n")
{
	   return system("kill -9 $(ps -ef | grep 'gotty -w -p' | grep 'tmux new -A -s gotty' | grep -v 'sh -c' | awk '{print $2}')");
}

DEFUN (show_gotty_tmux,
	   show_gotty_tmux_cmd,
	   "web tmux show",
	   "web client command\n"
	   "web tmux client command\n"
	   "show all web tmux client command\n")
{
	   return system("ps -ef | grep 'gotty -w -p' | grep 'tmux new -A -s gotty' | grep -v 'sh -c' | awk '{print $11}'");
}

DEFUN (set_gotty_vtysh,
	   set_gotty_vtysh_cmd,
	   "web vtysh set PORT USER:PASSWD",
	   "web client command\n"
	   "web vtysh client command\n"
	   "turn on web vtysh client command\n"
	   "set the port of web vtysh client\n"
	   "set the user and passwd of web vtysh client\n")
{
	  char cmd[100];
	  sprintf(cmd, "./gotty -w -p %s -c %s ./vtysh &", argv[0],argv[1]);
      system(cmd);

      return CMD_SUCCESS; 
}

DEFUN (unset_gotty_vtysh,
	   unset_gotty_vtysh_cmd,
	   "web vtysh unset PORT",
	   "web client command\n"
	   "web vtysh client command\n"
	   "turn off web vtysh client command\n"
	   "set the port of web vtysh client\n")
{
		char cmd[100];
		sprintf(cmd, "kill -9 $(ps -ef | grep gotty | awk '/-p %s/{print $2}')", argv[0]);
		system(cmd);
		
		return CMD_SUCCESS;
}

DEFUN (unsetall_gotty_vtysh,
	   unsetall_gotty_vtysh_cmd,
	   "web vtysh unset all",
	   "web client command\n"
	   "web vtysh client command\n"
	   "turn off web vtysh client command\n"
	   "turn off all web vtysh client command\n")
{
	   return system("kill -9 $(ps -ef | grep 'gotty -w -p' | grep 'vtysh' | grep -v 'sh -c' | awk '{print $2}')");
}


DEFUN (show_gotty_vtysh,
	   show_gotty_vtysh_cmd,
	   "web vtysh show",
	   "web client command\n"
	   "web vtysh client command\n"
	   "show all web vtysh client command\n")
{
	   return system("ps -ef | grep 'gotty -w -p' | grep 'vtysh' | grep -v 'sh -c' | awk '{print $11}'");
}

int cmd_shell_init()
{
    cmd_install_element (SHELL_NODE, &config_bash_cmd);
	cmd_install_element (SHELL_NODE, &config_sh_cmd);
	cmd_install_element (SHELL_NODE, &config_tmux_cmd);
	cmd_install_element (SHELL_NODE, &set_gotty_tmux_cmd);
	cmd_install_element (SHELL_NODE, &unset_gotty_tmux_cmd);
	cmd_install_element (SHELL_NODE, &unsetall_gotty_tmux_cmd);
    cmd_install_element (SHELL_NODE, &show_gotty_tmux_cmd);
	cmd_install_element (SHELL_NODE, &set_gotty_vtysh_cmd);
	cmd_install_element (SHELL_NODE, &unset_gotty_vtysh_cmd);
	cmd_install_element (SHELL_NODE, &unsetall_gotty_vtysh_cmd);
	cmd_install_element (SHELL_NODE, &show_gotty_vtysh_cmd);

	return 0;
}

