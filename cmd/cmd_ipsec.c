#include "command.h"
#include "vtysh_config.h"

#define IPSEC_STR "ipsec info\n"
#define STRONGSWAN_STR "strongswan set command\n"
#define CONN_STR "ipsec connection info\n"
#define IPSEC_FUNC "./test.sh"
#define IPSEC_MODE_STR1 "ipsec-server"
#define IPSEC_MODE_STR2 "ipsec-client"
#define CONN_NAME_CHAR_MAX 15
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char ch1[10];
char ch2[10];
char ch3[100];

DEFUN (start_ipsec,
       start_ipsec_cmd,
       "ipsec start",
       IPSEC_STR
       "ipsec start command\n")
{	
    char *myargv[10];
    myargv[0] = "start";
       
    return cmd_execute_system_command("ipsec", 1, myargv);
}


DEFUN (stop_ipsec,
       stop_ipsec_cmd,
       "ipsec stop",
       IPSEC_STR
       "ipsec stop command\n")
{
    char *myargv[10];
    myargv[0] = "stop";

    return cmd_execute_system_command("ipsec", 1, myargv);
}

DEFUN (restart_ipsec,
       restart_ipsec_cmd,
       "ipsec restart",
       IPSEC_STR
       "ipsec restart command\n")
{
    char *myargv[10];
    myargv[0] = "restart";

    return cmd_execute_system_command("ipsec", 1, myargv);
}

DEFUN (status_ipsec,
       status_ipsec_cmd,
       "ipsec status",
       IPSEC_STR
       "ipsec status command\n")
{
    char *myargv[10];
    myargv[0] = "status";

    return cmd_execute_system_command("ipsec", 1, myargv);
}

DEFUN (statusall_ipsec,
       statusall_ipsec_cmd,
       "ipsec statusall",
       IPSEC_STR
       "ipsec statusall command\n")
{
    char *myargv[10];
    myargv[0] = "statusall";

    return cmd_execute_system_command("ipsec", 1, myargv);
}

DEFUN (start_strongswan,
       start_strongswan_cmd,
       "strongswan start",
       STRONGSWAN_STR
       "strongswan start command\n")
{
    char *myargv[10];
    myargv[0] = "start";
    myargv[1] = "strongswan.service";

    return cmd_execute_system_command("systemctl", 2, myargv);
}

DEFUN (stop_strongswan,
       stop_strongswan_cmd,
       "strongswan stop",
       STRONGSWAN_STR
       "strongswan stop command\n")
{
    char *myargv[10];
    myargv[0] = "stop";
    myargv[1] = "strongswan.service";

    return cmd_execute_system_command("systemctl", 2, myargv);
}

DEFUN (restart_strongswan,
       restart_strongswan_cmd,
       "strongswan restart",
       STRONGSWAN_STR
       "strongswan restart command\n")
{
    char *myargv[10];
    myargv[0] = "restart";
    myargv[1] = "strongswan.service";

    return cmd_execute_system_command("systemctl", 2, myargv);
}

DEFUN (status_strongswan,
       status_strongswan_cmd,
       "strongswan status",
       STRONGSWAN_STR
       "strongswan status command\n")
{
    char *myargv[10];
    myargv[0] = "status";
    myargv[1] = "strongswan.service";

    return cmd_execute_system_command("systemctl", 2, myargv);
}

DEFUN (up_ipsec,
       up_ipsec_cmd,
       "ipsec up WORD",
       IPSEC_STR
       "ipsec up command\n"
       "the connection name\n")
{
    char *myargv[10];
    myargv[0] = "up";
    myargv[1] = argv[0];

    return cmd_execute_system_command("ipsec", 2, myargv);
}

DEFUN (down_ipsec,
       down_ipsec_cmd,
       "ipsec down",
       IPSEC_STR
       "ipsec down command\n"
       "the connection name\n")
{
    char *myargv[10];
    myargv[0] = "down";
    myargv[1] = argv[0];

    return cmd_execute_system_command("ipsec", 2, myargv);
}

DEFUN (update_ipsec,
       update_ipsec_cmd,
       "ipsec update",
       IPSEC_STR
       "ipsec update command\n")
{
    char *myargv[10];
    myargv[0] = "update";

    return cmd_execute_system_command("ipsec", 1, myargv);
}

DEFUN (reload_ipsec,
       reload_ipsec_cmd,
       "ipsec reload",
       IPSEC_STR
       "ipsec reload command\n")
{
    char *myargv[10];
    myargv[0] = "reload";

    return cmd_execute_system_command("ipsec", 1, myargv);
}

DEFUN (ipsec_m,
       ipsec_m_cmd,
       "workmode (server|client)",
       "ipsec workmode (server|client)\n"
       "work with server mode\n"
       "work with client mode\n")
{
    if (NULL != strstr("server", argv[0]))
    {
        strcpy(ch1, IPSEC_MODE_STR1);
        vty->node = IPSEC_S_NODE;
    }
    else if (NULL != strstr("client", argv[0]))	
    {
        strcpy(ch1, IPSEC_MODE_STR2);
        vty->node = IPSEC_C_NODE;
    }
        
    return CMD_SUCCESS;
}

DEFUN (add_conn,
       add_conn_cmd,
       "connection WORD",
       CONN_STR
       "connection name\n")
{
    if (strlen(argv[0]) > CONN_NAME_CHAR_MAX)
    {
        char str[100] = "";
        sprintf(str, "%s%d%s", "Error name: too many characters(>", CONN_NAME_CHAR_MAX, ")!");
        vty_out(vty, "%s%% %s%s\n",ANSI_COLOR_RED, str, ANSI_COLOR_RESET);
    }
    else
    {
        strcpy(ch2, ANSI_COLOR_RED);
        strcpy(ch3, argv[0]);
        
        //vty->node = CONN_S_NODE;
	
		char *myargv[10];
		if (0 == strcmp(ch1, IPSEC_MODE_STR1))
		{
			myargv[0] = "--create_conn_s";
			vty->node = CONN_S_NODE;
		}
		else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
		{
			myargv[0] = "--create_conn_c";
			vty->node = CONN_C_NODE;
		}
		myargv[1] = argv[0];
		
		return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
	}

    return CMD_WARNING;
}

DEFUN (del_conn,
       del_conn_cmd,
       "connection del (WORD|all)",
       CONN_STR
       "delete connection\n"
       "connection name\n"
       "all connection\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--del_conn_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--del_conn_c";
    }
    myargv[1] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (show_conn,
       show_conn_cmd,
       "connection show (configure|status) (WORD|all)",
       CONN_STR
       "show connection\n"
	   "configure info\n"
	   "status info\n"
       "connection name\n"
       "all connection\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_conn_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_conn_c";
    }
	myargv[1] = argv[0];
    myargv[2] = argv[1];
	
    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);	
}

DEFUN (add_local_addr,
       add_local_addr_cmd,
       "local addr add A.B.C.D",
       "local connection info\n"
       "connection address info\n"
       "add connection address\n"
       "ip address\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--add_local_addr_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--add_local_addr_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];
	
    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (del_local_addr,
       del_local_addr_cmd,
       "local addr del (A.B.C.D|all)",
       "local connection info\n"
       "connection address info\n"
       "delete connection address\n"
       "ip address\n"
       "all ip address\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--del_local_addr_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--del_local_addr_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (show_local_addr,
       show_local_addr_cmd,
       "local addr show",
       "local connection info\n"
       "connection address info\n"
       "show connection address\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_local_addr_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_local_addr_c";
    }
    myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (add_remote_addr,
       add_remote_addr_cmd,
       "remote addr add A.B.C.D",
       "remote connection info\n"
       "connection address info\n"
       "add connection address\n"
       "ip address\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--add_remote_addr_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--add_remote_addr_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];
	
    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (del_remote_addr,
       del_remote_addr_cmd,
       "remote addr del (A.B.C.D|all)",
       "remote connection info\n"
       "connection address info\n"
       "delete connection address\n"
       "ip address\n"
       "all ip address\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--del_remote_addr_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--del_remote_addr_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (show_remote_addr,
       show_remote_addr_cmd,
       "remote addr show",
       "remote connection info\n"
       "connection address info\n"
       "show connection address\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_remote_addr_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_remote_addr_c";
    }
    myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (set_local_id,
       set_local_id_cmd,
       "local id set WORD",
       "local connection info\n"
       "id info\n"
       "set connection id\n"
       "id name\n")
{   
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--set_local_id_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--set_local_id_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (unset_local_id,
       unset_local_id_cmd,
       "local id unset",
       "local connection info\n"
       "id info\n"
       "unset connection id\n")
{   
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--unset_local_id_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--unset_local_id_c";
    }
    myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (show_local_id,
       show_local_id_cmd,
       "local id show",
       "local connection info\n"
       "id info\n"
       "show connection id\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_local_id_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_local_id_c";
    }
	myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (set_remote_id,
       set_remote_id_cmd,
       "remote id set WORD",
       "remote connection info\n"
       "id info\n"
       "set connection id\n"
       "id name\n")
{   
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--set_remote_id_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--set_remote_id_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (unset_remote_id,
       unset_remote_id_cmd,
       "remote id unset",
       "remote connection info\n"
       "id info\n"
       "unset connection id\n")
{   
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--unset_remote_id_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--unset_remote_id_c";
    }
    myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (show_remote_id,
       show_remote_id_cmd,
       "remote id show",
       "remote connection info\n"
       "id info\n"
       "show connection id\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_remote_id_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_remote_id_c";
    }
	myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (add_local_subnet,
       add_local_subnet_cmd,
       "local subnet add A.B.C.D/X",
       "local connection info\n"
       "subnet info\n"
       "add connection subnet\n"
	   "subnet addr\n"
	   "all subnet addr\n")
{
    char *myargv[10];	
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--add_local_subnet_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--add_local_subnet_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];
	
    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (del_local_subnet,
       del_local_subnet_cmd,
       "local subnet del (A.B.C.D/X|all)",
       "local connection info\n"
       "subnet info\n"
       "del connection subnet\n"
	   "subnet addr\n"
	   "all subnet addr\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--del_local_subnet_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--del_local_subnet_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (show_local_subnet,
       show_local_subnet_cmd,
       "local subnet show",
       "local connection info\n"
       "subnet info\n"
       "show connection subnet\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_local_subnet_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_local_subnet_c";
    }
    myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (add_remote_subnet,
       add_remote_subnet_cmd,
       "remote subnet add A.B.C.D/X",
       "remote connection info\n"
       "subnet info\n"
       "add connection subnet\n"
	   "subnet addr\n")
{
    char *myargv[10];	
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--add_remote_subnet_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--add_remote_subnet_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];
	
    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (del_remote_subnet,
       del_remote_subnet_cmd,
       "remote subnet del A.B.C.D/X",
       "remote connection info\n"
       "subnet info\n"
       "del connection subnet\n"
	   "subnet addr\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--del_remote_subnet_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--del_remote_subnet_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (show_remote_subnet,
       show_remote_subnet_cmd,
       "remote subnet show",
       "remote connection info\n"
       "subnet info\n"
       "show connection subnet\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_remote_subnet_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_remote_subnet_c";
    }
    myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (set_local_sourceip,
       set_local_sourceip_cmd,
       "local sourceip set A.B.C.D/X",
       "local connection info\n"
       "sourceip info\n"
       "set connection sourceip\n"
       "sourceip addr\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--set_local_sourceip_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--set_local_sourceip_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (unset_local_sourceip,
       unset_local_sourceip_cmd,
       "local sourceip unset",
       "local connection info\n"
       "sourceip info\n"
       "unset connection sourceip\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--unset_local_sourceip_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--unset_local_sourceip_c";
    }
    myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (show_local_sourceip,
       show_local_sourceip_cmd,
       "local sourceip show",
       "local connection info\n"
       "sourceip info\n"
       "show connection sourceip\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_local_sourceip_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_local_sourceip_c";
    }
	myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (set_remote_sourceip,
       set_remote_sourceip_cmd,
       "remote sourceip set A.B.C.D/X",
       "remote connection info\n"
       "sourceip info\n"
       "set connection sourceip\n"
       "sourceip addr\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--set_remote_sourceip_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--set_remote_sourceip_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (unset_remote_sourceip,
       unset_remote_sourceip_cmd,
       "remote sourceip unset",
       "remote connection info\n"
       "sourceip info\n"
       "unset connection sourceip\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--unset_remote_sourceip_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--unset_remote_sourceip_c";
    }
    myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (show_remote_sourceip,
       show_remote_sourceip_cmd,
       "remote sourceip show",
       "remote connection info\n"
       "sourceip info\n"
       "show connection sourceip\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_remote_sourceip_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_remote_sourceip_c";
    }
	myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (set_local_auth,
       set_local_auth_cmd,
       "local auth set AUTH",
       "local connection info\n"
       "auth info\n"
       "set connection auth\n"
	   "auth algorithm\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--set_local_auth_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--set_local_auth_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (unset_local_auth,
       unset_local_auth_cmd,
       "local auth unset AUTH",
       "local connection info\n"
       "auth info\n"
       "unset connection auth\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--unset_local_auth_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--unset_local_auth_c";
    }
    myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (show_local_auth,
       show_local_auth_cmd,
       "local auth show",
       "local connection info\n"
       "auth info\n"
       "show connection auth\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_local_auth_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_local_auth_c";
    }
	myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (set_remote_auth,
       set_remote_auth_cmd,
       "remote auth set AUTH",
       "remote connection info\n"
       "auth info\n"
       "set connection auth\n"
	   "auth algorithm\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--set_remote_auth_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--set_remote_auth_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (unset_remote_auth,
       unset_remote_auth_cmd,
       "remote auth unset AUTH",
       "remote connection info\n"
       "auth info\n"
       "unset connection auth\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--unset_remote_auth_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--unset_remote_auth_c";
    }
    myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (show_remote_auth,
       show_remote_auth_cmd,
       "remote auth show",
       "remote connection info\n"
       "auth info\n"
       "show connection auth\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_remote_auth_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_remote_auth_c";
    }
	myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (set_auto,
       set_auto_cmd,
       "auto set (add|start|route|ignore)",
       "auto info\n"
       "set auto\n"
       "load a connection without starting it\n"
       "load a connection and bring it up immediately\n"
	   "load a connection and install kernel traps\n"
	   "ignore the connection\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--set_auto_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--set_auto_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (unset_auto,
       unset_auto_cmd,
       "auto unset",
       "auto info\n"
       "unset auto\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--unset_auto_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--unset_auto_c";
    }
    myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (show_auto,
       show_auto_cmd,
       "auto show",
       "auto info\n"
       "show auto info\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_auto_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_auto_c";
    }
	myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (set_dpdaction,
       set_dpdaction_cmd,
       "dpdaction set (clear|restart|hold|none)",
       "dpdaction info\n"
       "set dpdaction\n"
       "the connection is closed with no further actions taken\n"
       "immediately trigger an attempt to re-negotiate the connection\n"
	   "install a trap policy to catch matching traffic and try to re-negotiate the connection on demand\n"
	   "disable the active sending of DPD messages\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--set_dpdaction_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--set_dpdaction_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (unset_dpdaction,
       unset_dpdaction_cmd,
       "dpdaction unset",
       "dpdaction info\n"
       "unset dpdaction\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--unset_dpdaction_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--unset_dpdaction_c";
    }
    myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (show_dpdaction,
       show_dpdaction_cmd,
       "dpdaction show",
       "dpdaction info\n"
       "show dpdaction info\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_dpdaction_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_dpdaction_c";
    }
	myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (set_dpddelay,
       set_dpddelay_cmd,
       "dpddelay set TIME",
       "dpddelay info\n"
       "set dpddelay\n"
	   "period time interval(s)\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--set_dpddelay_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--set_dpddelay_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (unset_dpddelay,
       unset_dpddelay_cmd,
       "dpddelay unset",
       "dpddelay info\n"
       "unset dpddelay\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--unset_dpddelay_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--unset_dpddelay_c";
    }
    myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (show_dpddelay,
       show_dpddelay_cmd,
       "dpddelay show",
       "dpddelay info\n"
       "show dpddelay info\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_dpddelay_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_dpddelay_c";
    }
	myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (set_dpdtimeout,
       set_dpdtimeout_cmd,
       "dpdtimeout set TIME",
       "dpdtimeout info\n"
       "set dpdtimeout\n"
	   "timeout interval(s)\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--set_dpdtimeout_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--set_dpdtimeout_c";
    }
    myargv[1] = ch3;
    myargv[2] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 3, myargv);
}

DEFUN (unset_dpdtimeout,
       unset_dpdtimeout_cmd,
       "dpdtimeout unset",
       "dpdtimeout info\n"
       "unset dpdtimeout\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
		myargv[0] = "--unset_dpdtimeout_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
		myargv[0] = "--unset_dpdtimeout_c";
    }
    myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (show_dpdtimeout,
       show_dpdtimeout_cmd,
       "dpdtimeout show",
       "dpdtimeout info\n"
       "show dpdtimeout info\n")
{
    char *myargv[10];
    if (0 == strcmp(ch1, IPSEC_MODE_STR1))
    {
        myargv[0] = "--show_dpdtimeout_s";
    }
    else if (0 == strcmp(ch1, IPSEC_MODE_STR2))
    {
        myargv[0] = "--show_dpdtimeout_c";
    }
	myargv[1] = ch3;

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}

DEFUN (update_cert,
       update_cert_cmd,
       "cert update",
       "cert info\n"
       "generate and update cert\n")
{
    char *myargv[10];
    myargv[0] = "--update_cert";

    return cmd_execute_system_command(IPSEC_FUNC, 1, myargv);
}

DEFUN (show_cert,
       show_cert_cmd,
       "cert show",
       "cert info\n"
       "show cert\n")
{
    char *myargv[10];
    myargv[0] = "--show_cert";

    return cmd_execute_system_command(IPSEC_FUNC, 1, myargv);
}

DEFUN (export_file,
       export_file_cmd,
       "file export WORD",
       "file info\n"
       "export file\n"
	   "file name\n")
{
    char *myargv[10];
    myargv[0] = "--export_file";
	myargv[1] = argv[0];

    return cmd_execute_system_command(IPSEC_FUNC, 2, myargv);
}


/***********************************************************/
int cmd_ipsec_init()
{
    cmd_install_element (IPSEC_NODE, &ipsec_m_cmd);	
    cmd_install_element (IPSEC_NODE, &update_cert_cmd);	
    cmd_install_element (IPSEC_NODE, &show_cert_cmd);	
    cmd_install_element (IPSEC_NODE, &export_file_cmd);	
    
	cmd_install_element (IPSEC_S_NODE, &start_ipsec_cmd);
    cmd_install_element (IPSEC_S_NODE, &stop_ipsec_cmd);
    cmd_install_element (IPSEC_S_NODE, &restart_ipsec_cmd);
    cmd_install_element (IPSEC_S_NODE, &status_ipsec_cmd);
    cmd_install_element (IPSEC_S_NODE, &statusall_ipsec_cmd);
    cmd_install_element (IPSEC_S_NODE, &up_ipsec_cmd);
    cmd_install_element (IPSEC_S_NODE, &down_ipsec_cmd);
    cmd_install_element (IPSEC_S_NODE, &update_ipsec_cmd);
    cmd_install_element (IPSEC_S_NODE, &reload_ipsec_cmd);
    cmd_install_element (IPSEC_C_NODE, &start_ipsec_cmd);
    cmd_install_element (IPSEC_C_NODE, &stop_ipsec_cmd);
    cmd_install_element (IPSEC_C_NODE, &restart_ipsec_cmd);
    cmd_install_element (IPSEC_C_NODE, &status_ipsec_cmd);
    cmd_install_element (IPSEC_C_NODE, &statusall_ipsec_cmd);
    cmd_install_element (IPSEC_C_NODE, &up_ipsec_cmd);
    cmd_install_element (IPSEC_C_NODE, &down_ipsec_cmd);
    cmd_install_element (IPSEC_C_NODE, &reload_ipsec_cmd);
    cmd_install_element (IPSEC_C_NODE, &reload_ipsec_cmd);
    
    cmd_install_element (IPSEC_C_NODE, &add_conn_cmd);
    cmd_install_element (IPSEC_C_NODE, &del_conn_cmd);
    cmd_install_element (IPSEC_C_NODE, &show_conn_cmd);
    cmd_install_element (IPSEC_S_NODE, &add_conn_cmd);
    cmd_install_element (IPSEC_S_NODE, &del_conn_cmd);
    cmd_install_element (IPSEC_S_NODE, &show_conn_cmd);

    cmd_install_element (CONN_C_NODE, &add_local_addr_cmd);
    cmd_install_element (CONN_C_NODE, &del_local_addr_cmd);
    cmd_install_element (CONN_C_NODE, &show_local_addr_cmd);
    cmd_install_element (CONN_C_NODE, &add_remote_addr_cmd);
    cmd_install_element (CONN_C_NODE, &del_remote_addr_cmd);
    cmd_install_element (CONN_C_NODE, &show_remote_addr_cmd);
    cmd_install_element (CONN_C_NODE, &set_local_id_cmd);
    cmd_install_element (CONN_C_NODE, &unset_local_id_cmd);
    cmd_install_element (CONN_C_NODE, &show_local_id_cmd);
    cmd_install_element (CONN_C_NODE, &set_remote_id_cmd);
    cmd_install_element (CONN_C_NODE, &unset_remote_id_cmd);
    cmd_install_element (CONN_C_NODE, &show_remote_id_cmd);
    cmd_install_element (CONN_C_NODE, &set_local_auth_cmd);
    cmd_install_element (CONN_C_NODE, &unset_local_auth_cmd);
    cmd_install_element (CONN_C_NODE, &show_local_auth_cmd);
    cmd_install_element (CONN_C_NODE, &set_remote_auth_cmd);
    cmd_install_element (CONN_C_NODE, &unset_remote_auth_cmd);
    cmd_install_element (CONN_C_NODE, &show_remote_auth_cmd);
    cmd_install_element (CONN_C_NODE, &add_remote_subnet_cmd);
    cmd_install_element (CONN_C_NODE, &del_remote_subnet_cmd);
    cmd_install_element (CONN_C_NODE, &show_remote_subnet_cmd);
    cmd_install_element (CONN_C_NODE, &set_local_sourceip_cmd);
    cmd_install_element (CONN_C_NODE, &unset_local_sourceip_cmd);
    cmd_install_element (CONN_C_NODE, &show_local_sourceip_cmd);
    cmd_install_element (CONN_C_NODE, &set_auto_cmd);
    cmd_install_element (CONN_C_NODE, &unset_auto_cmd);
    cmd_install_element (CONN_C_NODE, &show_auto_cmd);
    cmd_install_element (CONN_C_NODE, &set_dpdaction_cmd);
    cmd_install_element (CONN_C_NODE, &unset_dpdaction_cmd);
    cmd_install_element (CONN_C_NODE, &show_dpdaction_cmd);
    cmd_install_element (CONN_C_NODE, &set_dpddelay_cmd);
    cmd_install_element (CONN_C_NODE, &unset_dpddelay_cmd);
    cmd_install_element (CONN_C_NODE, &show_dpddelay_cmd);
    cmd_install_element (CONN_C_NODE, &set_dpdtimeout_cmd);
    cmd_install_element (CONN_C_NODE, &unset_dpdtimeout_cmd);
    cmd_install_element (CONN_C_NODE, &show_dpdtimeout_cmd);
    
    cmd_install_element (CONN_S_NODE, &add_local_addr_cmd);
    cmd_install_element (CONN_S_NODE, &del_local_addr_cmd);
    cmd_install_element (CONN_S_NODE, &show_local_addr_cmd);
    cmd_install_element (CONN_S_NODE, &add_remote_addr_cmd);
    cmd_install_element (CONN_S_NODE, &del_remote_addr_cmd);
    cmd_install_element (CONN_S_NODE, &show_remote_addr_cmd);
    cmd_install_element (CONN_S_NODE, &set_local_id_cmd);
    cmd_install_element (CONN_S_NODE, &unset_local_id_cmd);
    cmd_install_element (CONN_S_NODE, &show_local_id_cmd);
    cmd_install_element (CONN_S_NODE, &set_remote_id_cmd);
    cmd_install_element (CONN_S_NODE, &unset_remote_id_cmd);
    cmd_install_element (CONN_S_NODE, &show_remote_id_cmd);
    cmd_install_element (CONN_S_NODE, &set_local_auth_cmd);
    cmd_install_element (CONN_S_NODE, &unset_local_auth_cmd);
    cmd_install_element (CONN_S_NODE, &show_local_auth_cmd);
    cmd_install_element (CONN_S_NODE, &set_remote_auth_cmd);
    cmd_install_element (CONN_S_NODE, &unset_remote_auth_cmd);
    cmd_install_element (CONN_S_NODE, &show_remote_auth_cmd);
    cmd_install_element (CONN_S_NODE, &add_local_subnet_cmd);
    cmd_install_element (CONN_S_NODE, &del_local_subnet_cmd);
    cmd_install_element (CONN_S_NODE, &show_local_subnet_cmd);
    cmd_install_element (CONN_S_NODE, &set_remote_sourceip_cmd);
    cmd_install_element (CONN_S_NODE, &unset_remote_sourceip_cmd);
    cmd_install_element (CONN_S_NODE, &show_remote_sourceip_cmd);
    cmd_install_element (CONN_S_NODE, &set_auto_cmd);
    cmd_install_element (CONN_S_NODE, &unset_auto_cmd);
    cmd_install_element (CONN_S_NODE, &show_auto_cmd);
    cmd_install_element (CONN_S_NODE, &set_dpdaction_cmd);
    cmd_install_element (CONN_S_NODE, &unset_dpdaction_cmd);
    cmd_install_element (CONN_S_NODE, &show_dpdaction_cmd);
    cmd_install_element (CONN_S_NODE, &set_dpddelay_cmd);
    cmd_install_element (CONN_S_NODE, &unset_dpddelay_cmd);
    cmd_install_element (CONN_S_NODE, &show_dpddelay_cmd);
    cmd_install_element (CONN_S_NODE, &set_dpdtimeout_cmd);
    cmd_install_element (CONN_S_NODE, &unset_dpdtimeout_cmd);
    cmd_install_element (CONN_S_NODE, &show_dpdtimeout_cmd);

    cmd_install_element (IPSEC_NODE, &start_strongswan_cmd);
    cmd_install_element (IPSEC_NODE, &stop_strongswan_cmd);
    cmd_install_element (IPSEC_NODE, &restart_strongswan_cmd);
    cmd_install_element (IPSEC_NODE, &status_strongswan_cmd);

    return 0;
}
