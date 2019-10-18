#include "command.h"
#include "vtysh_config.h"
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>


DEFUN (bridge_add,
       bridge_add_cmd,
       "bridge NUM WORD WORD",
       "add/modify the bridge information\n"
	   "the bridge number[1-9]\n"
	   "the bridge interface1 name\n"
	   "the bridge interface2 name\n")
{
	int nNum = atoi(argv[0]);
	char *pEth1 = argv[1];
	char *pEth2 = argv[2];
	char szInfo[1024];

	if(nNum <1  || nNum > 9)
	{
		vty_out(vty, "bridge number %s error\n", argv[0]);
		return CMD_WARNING;
	}
	
	sprintf(szInfo, "ifconfig \"%s\" > /dev/null 2>&1", pEth1);
	if((memcmp(pEth1,"eth", 3) != 0) || (strchr(pEth1, '\"') != NULL) || system(szInfo) != 0)
	{
		vty_out(vty, "interface %s invalid\n", pEth1);
		return CMD_WARNING;
	}
	sprintf(szInfo, "ifconfig \"%s\" > /dev/null 2>&1", pEth2);
	if((memcmp(pEth2,"eth", 3) != 0) || (strchr(pEth2, '\"') != NULL) || system(szInfo) != 0)
	{
		vty_out(vty, "interface %s invalid\n", pEth2);
		return CMD_WARNING;
	}
	snprintf(szInfo, sizeof(szInfo), "bridge %d", nNum);
	config_del_line_byleft(config_top, szInfo);
	snprintf(szInfo, sizeof(szInfo), "bridge %d %s %s", nNum, pEth1, pEth2);
	config_add_line(config_top, szInfo);

	ENSURE_CONFIG(vty);

	sprintf(szInfo, "ifconfig %s 0.0.0.0 > /dev/null 2>&1", pEth1);
	system(szInfo);
	sprintf(szInfo, "ifconfig %s 0.0.0.0 > /dev/null 2>&1", pEth2);
	system(szInfo);
	sprintf(szInfo, "brctl addbr brad%d", nNum);
	system(szInfo);
	sprintf(szInfo, "brctl addif brad%d %s %s > /dev/null 2>&1", nNum, pEth1, pEth2);
	if(system(szInfo) != 0)
	{
		snprintf(szInfo, sizeof(szInfo), "bridge %d", nNum);
		config_del_line_byleft(config_top, szInfo);
	}
	else
	{	
		sprintf(szInfo, "brctl setfd brad%d 1 > /dev/null 2>&1", nNum);
		system(szInfo);
		sprintf(szInfo, "bridge brad%d Add Success\n", nNum);
		vty_out(vty, szInfo);
	}
	return CMD_SUCCESS;
}

DEFUN (no_bridge_add,
       no_bridge_add_cmd,
       "no bridge NUM",
	   NO_STR
       "delete the bridge\n")
{
	char szInfo[1024];
	int nNum = atoi(argv[0]);

	if(nNum <1  || nNum > 9)
	{
		vty_out(vty, "bridge number %s error\n", argv[0]);
		return CMD_WARNING;
	}
	snprintf(szInfo, sizeof(szInfo), "bridge %d", nNum);
	config_del_line_byleft(config_top, szInfo);

	ENSURE_CONFIG(vty);
	sprintf(szInfo, "ifconfig brad%d down> /dev/null 2>&1", nNum);
	system(szInfo);
	sprintf(szInfo, "brctl delbr brad%d > /dev/null 2>&1", nNum);
	system(szInfo);
	vty_out(vty, "bridge remove OK!\n");
	return CMD_SUCCESS;
}

DEFUN (show_bridge,
       show_bridge_cmd,
       "show bridge",
       SHOW_STR
       "show the bridge info\n")
{
	system("brctl show");
	return CMD_SUCCESS;
}

DEFUN (show_bridge_mac,
       show_bridge_mac_cmd,
       "show bridge WORD",
       SHOW_STR
       "show the bridge info\n"
       "bridge name\n")
{
	char *myargv[4];

	myargv[0] = "showmacs";
	myargv[1] = argv[0];

	if(memcmp(argv[0], "brad", 4) != 0)
	{
		vty_out(vty, "invalid bridge name\n");
		return CMD_WARNING;
	}
	return cmd_execute_system_command("brctl", 2, myargv);
}



int cmd_bridge_init()
{
  	cmd_install_element (ENABLE_NODE, &show_bridge_cmd);
  	cmd_install_element (ENABLE_NODE, &show_bridge_mac_cmd);
	cmd_install_element (CONFIG_NODE, &bridge_add_cmd);
	cmd_install_element (CONFIG_NODE, &no_bridge_add_cmd);
	return 0;
}
