#include "command.h"
#include "vtysh_config.h"
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>

DEFUN(config_ping,
        config_ping_cmd,
        "ping WORD",
        "send echo messages\n"
        "ping destination address or hostname\n")
{
    cmd_execute_system_command ("ping", 1, argv);
    return CMD_SUCCESS;
}

DEFUN(config_ping_count,
        config_ping_cmd_count,
        "ping WORD NUM",
        "send count echo messages\n"
        "ping destination address or hostname\n"
        "stop after sending NUM ECHO_REQUEST packets\n")
{
    char *myargv[5];

    if(!isdigit(argv[1][0]))
    {
        vty_out(vty, "Invalid number '%s', Please input the count number\n", argv[1]);
        return CMD_ERR_NOTHING_TODO;
    }
    myargv[0] = argv[0];
    myargv[1] = "-c";
    myargv[2] = argv[1];
    cmd_execute_system_command ("ping", 3, myargv);
    return CMD_SUCCESS;
}

DEFUN(config_traceroute,
        config_traceroute_cmd,
        "traceroute WORD",
        "trace route to destination\n"
        "trace route to destination address or hostname\n")
{
    cmd_execute_system_command ("traceroute", 1, argv);
    return CMD_SUCCESS;
}

DEFUN(config_telnet,
        config_telnet_cmd,
        "telnet WORD",
        "open a telnet connection\n"
        "ip address or hostname of a remote system\n")
{
    cmd_execute_system_command ("telnet", 1, argv);
    return CMD_SUCCESS;
}

DEFUN(config_telnet_port,
        config_telnet_port_cmd,
        "telnet WORD PORT",
        "open a telnet connection\n"
        "ip address or hostname of a remote system\n"
        "tcp port number\n")
{
    cmd_execute_system_command ("telnet", 2, argv);
    return CMD_SUCCESS;
}

DEFUN(config_reboot,
        config_reboot_cmd,
        "reboot",
        "reboot the system\n"
        "reboot the system\n")
{
    vty_out(vty, "System will reboot, please waiting...\n");
    system("sleep 1; reboot&");
    exit(1);
    return CMD_SUCCESS;
}

DEFUN(config_poweroff,
        config_poweroff_cmd,
        "poweroff",
        "power off the system\n"
        "power off the system\n")
{
    vty_out(vty, "System power off!\n");
    system("sleep 1; poweroff&");
    return CMD_SUCCESS;
}

DEFUN(config_tftp,
        config_tftp_cmd,
        "tftp (get|put) WORD WORD A.B.C.D",
        "begin a tftp session\n"
        "get file from remote\n"
        "put file to remote\n"
        "local file name\n"
        "remote file name\n"
        "tftp server address\n")
{
    char *myargv[10];
    char localfile[256];

    if(strchr(argv[1], '/'))
    {
        vty_out(vty, "invalid local file name.please remove the '/' character.\n");
        return CMD_ERR_NOTHING_TODO;
    }
    sprintf(localfile, "%s/%s", CONFIG_DIR, argv[1]);
    if(strcmp(argv[0], "put") == 0)
        myargv[0] = "-p";
    else
        myargv[0] = "-g";
    myargv[1] = "-l";
    myargv[2] = localfile;
    myargv[3] = "-r";
    myargv[4] = argv[2];
    myargv[5] = argv[3];
    return cmd_execute_system_command("tftp", 6, myargv);;
}

DEFUN(config_snmpwalk,
        config_snmpwalk_cmd,
        "snmpwalk (v1|v2) WORD A.B.C.D",
        "begin a snmp walk test session\n"
        "v1 use the snmp 1 version\n"
        "v2 use the snmp v2c version\n"
        "the snmp read community\n"
        "the snmp server address\n")
{
    char szCmd[1024];
    char szFile[1024];

    sprintf(szFile, "/tmp/logs/debuglog/snmptest-%s.tgz", argv[2]);	

    sprintf(szCmd, "snmpwalk %s -C c -c \"%s\" \"%s\" | tee %s",
            strcmp(argv[0], "v1") == 0 ? "-v1":"-v2c",
            argv[1], argv[2], szFile); 
    vty_out(vty, "get the snmp public info from %s, this may take a few minutes\n", argv[2]);
    system(szCmd);
    sprintf(szCmd, "snmpwalk %s -C c -c \"%s\" \"%s\"  SNMPv2-SMI::enterprises | tee -a %s",
            strcmp(argv[0], "v1") == 0 ? "-v1":"-v2c",
            argv[1], argv[2], szFile); 
    vty_out(vty, "get the snmp private info from %s, this may take a few minutes\n", argv[2]);
    system(szCmd);

    vty_out(vty, "OK! please use the web to get the snmptest-%s.snmpwalk file\n", argv[2]);
    return 0;
}


DEFUN (service_telnet_server,
        service_telnet_server_cmd,
        "service telnet-server",
        //"set up telnet daemon service\n"
		SERVICE_ADD_STR
        "enable telnet daemon\n")
{
    config_del_line(config_top, "service telnet-server");
    config_add_line(config_top, "service telnet-server");

    ENSURE_CONFIG(vty);

    return cmd_execute_system_command("telnetd", 0, NULL);
}

DEFUN (no_service_telnet_server,
        no_service_telnet_server_cmd,
        "no service telnet-server",
        NO_STR
        //"disable telnet server service\n"
		SERVICE_DEL_STR
        "disable telnet server\n")
{
    char *myargv[2];

    config_del_line(config_top, "service telnet-server");	
    myargv[0] = "telnetd";
    return cmd_execute_system_command("killall", 1, myargv);
}

DEFUN (service_sads_server,
        service_sads_server_cmd,
        "service sads-server",
        //"set up sads daemon service\n"
		SERVICE_ADD_STR
        "enable sads daemon\n")
{
    config_del_line(config_top, "service sads-server");
    config_add_line(config_top, "service sads-server");

    ENSURE_CONFIG(vty);

    return cmd_execute_system_command("squidads", 0, NULL);
}

DEFUN (no_service_sads_server,
        no_service_sads_server_cmd,
        "no service sads-server",
        NO_STR
        //"disable sads server service\n"
		SERVICE_DEL_STR
        "disable sads server\n")
{
    char *myargv[2];

    config_del_line(config_top, "service sads-server");	
    myargv[0] = "stop";
    return cmd_execute_system_command("squidads", 1, myargv);
}

DEFUN (service_http_server,
        service_http_server_cmd,
        "service http-server",
        //"set up http daemon service\n"
		SERVICE_ADD_STR
        "enable http daemon\n")
{
    char *myargv[10];

    config_del_line(config_top, "service http-server");
    config_add_line(config_top, "service http-server");
    
    ENSURE_CONFIG(vty);

    myargv[0] = "-u";
    myargv[1] = "nobody";
    myargv[2] = "-h";
    myargv[3] = "/etc/ads";
    
    return cmd_execute_system_command("httpd", 4, myargv);
}

DEFUN (no_service_http_server,
        no_service_http_server_cmd,
        "no service http-server",
        NO_STR
        //"disable http server service\n"
		SERVICE_DEL_STR
        "disable http server\n")
{
    char *myargv[2];

    config_del_line(config_top, "service http-server");	
    myargv[0] = "httpd";
    return cmd_execute_system_command("killall", 1, myargv);
}


DEFUN (service_ssh_server,
        service_ssh_server_cmd,
        "service ssh-server",
        //"set up ssh daemon service\n"
		SERVICE_ADD_STR
        "enable ssh daemon\n")
{
    config_del_line(config_top, "service ssh-server");
    config_add_line(config_top, "service ssh-server");
    ENSURE_CONFIG(vty);
    return cmd_execute_system_command("dropbear", 0, NULL);
}

DEFUN (no_service_ssh_server,
        no_service_ssh_server_cmd,
        "no service ssh-server",
        NO_STR
        //"disable ssh server service\n"
		SERVICE_DEL_STR
        "disable ssh server\n")
{
    char *myargv[2];

    config_del_line(config_top, "service ssh-server");	
    myargv[0] = "dropbear";
    return cmd_execute_system_command("killall", 1, myargv);
}

#include <sys/vfs.h>    /* or <sys/statfs.h> */

DEFUN(config_dir,
        config_dir_cmd,
        "dir",
        "list the flash files\n")
{
    char *myargv[10];
    struct statfs buf;

    myargv[0] = "-Alh";
    myargv[1] = CONFIG_DIR;
    cmd_execute_system_command ("ls", 2, myargv);

    memset(&buf, 0, sizeof(buf));
    if(statfs(CONFIG_DIR, &buf) < 0)
    {
        myargv[0] = "-h";
        myargv[1] = CONFIG_DIR;
        return cmd_execute_system_command ("df", 2, myargv);
    }
    vty_out(vty, "\nTotal size:%dM free:%dM\n", 
            (buf.f_bsize >> 10) * (buf.f_blocks >> 10), 
            (buf.f_bsize >> 10) * (buf.f_bavail >> 10)
           );
    return 0;
}

#include <sys/vfs.h>    /* or <sys/statfs.h> */

DEFUN(config_sysinfo,
        config_sysinfo_cmd,
        "show sysinfo",
        SHOW_STR
        "display the system info\n")
{
    vty_out(vty, "please waiting ...\n");
    system("top -n 1 | head -n 10");
    return 0;
}

DEFUN(config_delete,
        config_delete_cmd,
        "delete WORD",
        "delete the flash files\n"
        "the file need delete\n")
{
    char *myargv[10];
    char filename[1024];
    char linkname[1024];

    memset(linkname, 0, sizeof(linkname));
    sprintf(filename, "%s/ASM.bin", CONFIG_DIR);
    readlink(filename, linkname, sizeof(linkname));
    if((strcmp(argv[0], "ASM.bin") == 0) || (strcmp(linkname, argv[0]) == 0))
    {
        vty_out(vty, "The boot image is not allowed to delete\n");
        return CMD_ERR_NOTHING_TODO;
    }

    sprintf(filename, "%s/%s", CONFIG_DIR, argv[0]);
    myargv[0] = "-f";
    myargv[1] = filename;
    return cmd_execute_system_command ("rm", 2, myargv);
}

DEFUN (config_tcpdump,
        config_tcpdump_cmd,
        "tcpdump INTERFACE EXPRESSION",
        "tcpdump packet \n"
        "the interface name\n"
        "expression, example:\"port 80 and host 192.168.0.1\"")
{
    char *myargv[10];
    int i;

    for(i = 0; i < strlen(argv[1]); i ++)
    {
        if(argv[1][i] == '-')
            argv[1][i] = ' ';
    }

    myargv[0] = "-i";
    myargv[1] = argv[0];
    myargv[2] = "-n";
    myargv[3] = "-Z";
    myargv[4] = RUN_USER;
    myargv[5] = "-XX";
    myargv[6] = argv[1];
    return cmd_execute_system_command ("tcpdump", 7, myargv);
}

DEFUN(config_logging,
        config_logging_cmd,
        "logging (memory|WORD)",
        "config the logging\n"
        "logging to the memory only\n"
        "loggint to the memory and host.the ip address or ip:port,eg 192.168.1.1:514")
{
    char *myargv[10];

    config_del_line_byleft(config_top, "logging");


    if(strcmp(argv[0], "memory") == 0)
    {
        config_add_line(config_top, "logging memory");
        ENSURE_CONFIG(vty);

    	system("killall klogd > /dev/null 2>&1");
	system("klogd");

        myargv[0] = "syslogd";
        cmd_execute_system_command("killall", 1, myargv);
        return cmd_execute_system_command("syslogd", 0, myargv);
    }
    else
    {
        config_add_line(config_top, "logging %s", argv[0]);
        ENSURE_CONFIG(vty);

    	system("killall klogd > /dev/null 2>&1");
	system("klogd");

        myargv[0] = "syslogd";
        cmd_execute_system_command("killall", 1, myargv);
        myargv[0] = "-L";
        myargv[1] = "-R";
        myargv[2] = argv[0];
        return cmd_execute_system_command("syslogd", 3, myargv);
    }
}

DEFUN(config_no_logging,
        config_no_logging_cmd,
        "no logging",
        NO_STR
        "disable the logging\n")
{	
    char *myargv[10];
    config_del_line_byleft(config_top, "logging");

    system("killall klogd > /dev/null 2>&1");
    myargv[0] = "syslogd";
    return cmd_execute_system_command("killall", 1, myargv);
}

DEFUN(config_date,
        config_date_cmd,
        "date WORD WORD",
        "set the date\n"
        "the date YYYY-MM-DD\n"
        "the time HH:MM:SS"
     )
{	
    char *myargv[10];
    char date[256];

    myargv[0] = "-s";
    sprintf(date, "%s %s", argv[0], argv[1]);
    myargv[1] = date;
    cmd_execute_system_command("date", 2, myargv);
    myargv[0] = "-w";
    return cmd_execute_system_command("hwclock", 1, myargv);
}

/* Show date. */
DEFUN (show_date,
        show_date_cmd,
        "show date",
        SHOW_STR
        "displays the current date\n")
{
    return cmd_execute_system_command("date", 0, argv);
}

/* Show version. */
DEFUN (show_logging,
        show_logging_cmd,
        "show logging",
        SHOW_STR
        "displays the logging information\n")
{
    char *myargv[10];

    if(access("/var/log/messages", R_OK) != 0)
    {
        vty_out(vty, "Please first enable the logging\n");
        return CMD_ERR_NOTHING_TODO;
    }
    myargv[0] = "-n";
    myargv[1] = "1000";
    myargv[2] = "/var/log/messages";
    return cmd_execute_system_command("tail", 3, myargv);
}

DEFUN(net_netstat_info,
        net_netstat_info_cmd,
        "netstat",
        "show netstat\n"
        "params Ctrl+C stop\n")

{ 
    char *myargv[10];
    
    myargv[0] = "-n";
    myargv[1] = "-a";
    return cmd_execute_system_command ("netstat", 2, myargv);
} 
/*****************************end******************************************/

int cmd_tool_init()
{
    /* Each node's basic commands. */
    cmd_install_element (VIEW_NODE, &config_ping_cmd);
    cmd_install_element (VIEW_NODE, &config_ping_cmd_count);
    cmd_install_element (VIEW_NODE, &config_traceroute_cmd);
    cmd_install_element (VIEW_NODE, &config_telnet_cmd);
    cmd_install_element (VIEW_NODE, &config_telnet_port_cmd);

    cmd_install_element (ENABLE_NODE, &config_ping_cmd);
    cmd_install_element (ENABLE_NODE, &config_ping_cmd_count);
    cmd_install_element (ENABLE_NODE, &config_traceroute_cmd);
    cmd_install_element (ENABLE_NODE, &config_telnet_cmd);
    cmd_install_element (ENABLE_NODE, &config_telnet_port_cmd);
	//cmd_install_element (ENABLE_NODE, &config_tftp_cmd);
	//cmd_install_element (ENABLE_NODE, &config_dir_cmd);
	cmd_install_element (ENABLE_NODE, &config_reboot_cmd);
    cmd_install_element (ENABLE_NODE, &config_poweroff_cmd);
    cmd_install_element (ENABLE_NODE, &config_tftp_cmd);
    cmd_install_element (ENABLE_NODE, &config_dir_cmd);
    cmd_install_element (ENABLE_NODE, &config_delete_cmd);
    cmd_install_element (ENABLE_NODE, &config_tcpdump_cmd);
    //cmd_install_element (ENABLE_NODE, &net_netstat_info_cmd);
    //cmd_install_element (ENABLE_NODE, &config_snmpwalk_cmd);
    cmd_install_element (ENABLE_NODE, &show_logging_cmd);
    cmd_install_element (ENABLE_NODE, &config_sysinfo_cmd);
    cmd_install_element (ENABLE_NODE, &show_date_cmd);

    cmd_install_element (CONFIG_NODE, &config_ping_cmd);
    cmd_install_element (CONFIG_NODE, &config_ping_cmd_count);
    cmd_install_element (CONFIG_NODE, &config_traceroute_cmd);
    cmd_install_element (CONFIG_NODE, &config_telnet_cmd);
    cmd_install_element (CONFIG_NODE, &config_telnet_port_cmd);
    cmd_install_element (CONFIG_NODE, &service_telnet_server_cmd);
    cmd_install_element (CONFIG_NODE, &no_service_telnet_server_cmd);
    cmd_install_element (CONFIG_NODE, &service_sads_server_cmd);
    cmd_install_element (CONFIG_NODE, &no_service_sads_server_cmd);
    cmd_install_element (CONFIG_NODE, &service_ssh_server_cmd);
    cmd_install_element (CONFIG_NODE, &no_service_ssh_server_cmd);
    cmd_install_element (CONFIG_NODE, &service_http_server_cmd);
    cmd_install_element (CONFIG_NODE, &no_service_http_server_cmd);
    //  cmd_install_element (CONFIG_NODE, &config_tftp_cmd);
    //  cmd_install_element (CONFIG_NODE, &config_dir_cmd);
    cmd_install_element (CONFIG_NODE, &config_logging_cmd);
    cmd_install_element (CONFIG_NODE, &config_no_logging_cmd);
    cmd_install_element (CONFIG_NODE, &show_logging_cmd);
    cmd_install_element (CONFIG_NODE, &config_date_cmd);
    cmd_install_element (CONFIG_NODE, &show_date_cmd);

    return 0;
}
