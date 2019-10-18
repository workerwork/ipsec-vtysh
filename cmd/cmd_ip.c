#include "command.h"
#include "vtysh_config.h"

DEFUN (show_ip_address,
       show_ip_address_cmd,
       "show ip address",
       SHOW_STR
       IP_STR
       "iP address list\n")
{
	char *myargv[2];

	myargv[0] = "addr";
	return cmd_execute_system_command("ip", 1, myargv);
}

DEFUN (show_ip_config,
       show_ip_config_cmd,
       "show ip config",
       SHOW_STR
       IP_STR
       "iP config\n")
{
	char *myargv[2];

	myargv[0] = "-a";
	return cmd_execute_system_command("ifconfig", 1, myargv);
}

DEFUN (show_ip_config_name,
       show_ip_config_name_cmd,
       "show ip config WORD",
       SHOW_STR
       IP_STR
       "iP config name\n"
	   "the interface name\n")
{
	return cmd_execute_system_command("ifconfig", 1, argv);
}

DEFUN(show_ip_route, 
	  show_ip_route_cmd,
       "show ip route", 
       SHOW_STR
       IP_STR
       "iP routing table\n")
{
	char *myargv[2];

	myargv[0] = "-n";
	return cmd_execute_system_command("route", 1, myargv);
}


DEFUN (ip_address, ip_address_cmd, 
	"ip address ETHNAME A.B.C.D A.B.C.D", 
	IP_STR
	"config the ip address\n"
	"interface name\n"
	"ip address e.g. x.x.x.x\n"
	"ip netmask  e.g. 255.255.255.0 \n")
{
	char *myargv[10];
	char line[1024];

	sprintf(line, "ip address %s", argv[0]);
	config_del_line_byleft(config_top, line);
	sprintf(line, "ip address %s %s %s", argv[0], argv[1], argv[2]);
	config_add_line(config_top, line);

	ENSURE_CONFIG(vty);
	
	myargv[0] = argv[0];
	myargv[1] = argv[1];
	myargv[2] = "netmask";
	myargv[3] = argv[2];
	return cmd_execute_system_command("ifconfig", 4, myargv);
}

DEFUN (no_ip_address, no_ip_address_cmd, 
	"no ip address ETHNAME", 
	NO_STR
	IP_STR
	"disable the ip address\n"
	"address\n"
	"interface name\n")
{
	char *myargv[10];
	char line[1024];

	sprintf(line, "ip address %s", argv[0]);
	config_del_line_byleft(config_top, line);

	myargv[0] = argv[0];
	myargv[1] = "down";
	return cmd_execute_system_command("ifconfig", 2, myargv);
}

//route add -net 192.168.56.0 netmask 255.255.255.0 gw 192.168.54.1
DEFUN (ip_route, ip_route_cmd, 
	"ip route A.B.C.D A.B.C.D A.B.C.D", 
	IP_STR
	"config the ip route(temporary)\n"
	"destination address\n"
	"destination adress netmask e.g. 255.255.255.0\n"
	"nexthot address\n")
{
	char *myargv[10];
	char line[1024];

	sprintf(line, "ip route %s %s %s", argv[0], argv[1], argv[2]);
	config_del_line(config_top, line);
	config_add_line(config_top, line);

	ENSURE_CONFIG(vty);
	
	if(strcmp(argv[1], "255.255.255.255") == 0)
	{
		myargv[0] = "del";
		myargv[1] = "-host";
		myargv[2] = argv[0];
		sprintf(line, "route del -host %s > /dev/null 2>&1", argv[0]);
		system(line);

		myargv[0] = "add";
		myargv[1] = "-host";
		myargv[2] = argv[0];
		myargv[3] = "gw";
		myargv[4] = argv[2];
		return cmd_execute_system_command("route", 5, myargv);
	}
	else
	{
		myargv[0] = "del";
		myargv[1] = "-net";
		myargv[2] = argv[0];
		myargv[3] = "netmask";
		myargv[4] = argv[1];
		sprintf(line, "route del -net %s netmask %s> /dev/null 2>&1", argv[0], argv[1]);
		system(line);

		myargv[0] = "add";
		myargv[1] = "-net";
		myargv[2] = argv[0];
		myargv[3] = "netmask";
		myargv[4] = argv[1];
		myargv[5] = "gw";
		myargv[6] = argv[2];
		return cmd_execute_system_command("route", 7, myargv);
	}
}

DEFUN (no_ip_route, no_ip_route_cmd, 
	"no ip route A.B.C.D A.B.C.D", 
	NO_STR
	IP_STR
	"disable the ip route\n"
	"destination address\n"
	"destination adress netmask e.g. 255.255.255.0\n")
{
	char *myargv[10];
	char line[1024];

	sprintf(line, "ip route %s %s", argv[0], argv[1]);
	config_del_line_byleft(config_top, line);
	if(strcmp(argv[1], "255.255.255.255") == 0)
	{
		myargv[0] = "del";
		myargv[1] = "-host";
		myargv[2] = argv[0];
		return cmd_execute_system_command("route", 3, myargv);
	}
	else
	{
		myargv[0] = "del";
		myargv[1] = "-net";
		myargv[2] = argv[0];
		myargv[3] = "netmask";
		myargv[4] = argv[1];
		return cmd_execute_system_command("route", 5, myargv);
	}
}

DEFUN (config_dns, config_dns_cmd, 
	"nameserver A.B.C.D A.B.C.D", 
	"config the dns server\n"
	"the first dns server ip\n"
	"the second dns server ip\n")
{
	FILE *fp;
	config_del_line_byleft(config_top, "nameserver");
	config_add_line(config_top, "nameserver %s %s", argv[0], argv[1]);

	ENSURE_CONFIG(vty);

	fp = fopen("/etc/resolv.conf", "w");
	if(fp == NULL)
	{
		vty_out(vty, "open the resolv.conf error\n");
		return CMD_ERR_NOTHING_TODO;
	}
	fprintf(fp, "nameserver %s\n", argv[0]);
	fprintf(fp, "nameserver %s\n", argv[1]);
	fclose(fp);

	return CMD_SUCCESS;
}

DEFUN (config_no_dns, config_no_dns_cmd, 
	"no nameserverr",
	NO_STR
	"remove the dns server\n")
{
	FILE *fp;
	config_del_line_byleft(config_top, "nameserver");

	ENSURE_CONFIG(vty);

	fp = fopen("/etc/resolv.conf", "w");
	if(fp == NULL)
	{
		vty_out(vty, "open the resolv.conf error\n");
		return CMD_ERR_NOTHING_TODO;
	}
	fprintf(fp, "nameserver 127.0.0.1\n");
	fclose(fp);

	return CMD_SUCCESS;
}

DEFUN(show_ip_arp, 
	  show_ip_arp_cmd,
       "show arp", 
       SHOW_STR
       "the arp table")
{
	char *myargv[2];

	myargv[0] = "-na";
	return cmd_execute_system_command("arp", 1, myargv);
}

int cmd_ip_init()
{
  cmd_install_element (ENABLE_NODE, &show_ip_address_cmd);
  cmd_install_element (ENABLE_NODE, &show_ip_config_cmd);
  cmd_install_element (ENABLE_NODE, &show_ip_config_name_cmd);
  cmd_install_element (ENABLE_NODE, &show_ip_route_cmd);
  cmd_install_element (ENABLE_NODE, &show_ip_arp_cmd);

  cmd_install_element (CONFIG_NODE, &show_ip_address_cmd);
  cmd_install_element (CONFIG_NODE, &show_ip_config_cmd);
  cmd_install_element (CONFIG_NODE, &show_ip_config_name_cmd);
  cmd_install_element (CONFIG_NODE, &show_ip_route_cmd);
  cmd_install_element (CONFIG_NODE, &show_ip_arp_cmd);

  cmd_install_element (CONFIG_NODE, &ip_address_cmd);
  cmd_install_element (CONFIG_NODE, &no_ip_address_cmd);
  cmd_install_element (CONFIG_NODE, &ip_route_cmd);
  cmd_install_element (CONFIG_NODE, &no_ip_route_cmd);
  cmd_install_element (CONFIG_NODE, &config_dns_cmd);
  cmd_install_element (CONFIG_NODE, &config_no_dns_cmd);

  return 0;
}
