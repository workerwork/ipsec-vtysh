#!/bin/bash -

source ./ipsec-functions
DIR_CERT=/home/dongfeng/df/test/vtysh/cert
DIR_FILE=/home/dongfeng/df/test/vtysh/cert
DIR_CLIENT=./ipsec.client
DIR_SERVER=./ipsec.server

[[ ! -d $DIR_CERT ]] && mkdir -p $DIR_CERT
[[ ! -d $DIR_CLIENT ]] && mkdir -p $DIR_CLIENT
[[ ! -d $DIR_SERVER ]] && mkdir -p $DIR_SERVER

case "$1" in
--update_cert)
	echo "update_cert"
	update_cert $DIR_CERT
	exit;;
--show_cert)
	echo "show_cert"
	show_cert $DIR_CERT
	exit;;
--export_file)
	echo "export_file"
	export_file $DIR_FILE $2
	exit;;
--create_conn_s)
    echo "create_conn_s"
	create_conn $DIR_SERVER $2
    exit;;
--create_conn_c)
    echo "create_conn_c"
    create_conn $DIR_CLIENT $2
	exit;;
--del_conn_s)
    echo "del_conn_s"
    del_conn $DIR_SERVER $2
	exit;;
--del_conn_c)
    echo "del_conn_c"
    del_conn $DIR_CLIENT $2
	exit;;
--show_conn_s)
	echo "show_conn_s"
	show_conn $DIR_SERVER $2 $3
	exit;;
--show_conn_c)
	echo "show_conn_c"
	show_conn $DIR_CLIENT $2 $3
	exit;;
--add_local_addr_s)
    echo "add_local_addr_s"
	add_local_addr $DIR_SERVER $2 $3
	exit;;	
--add_local_addr_c)
    echo "add_local_addr_c"
	add_local_addr $DIR_CLIENT $2 $3
	exit;;	
--del_local_addr_s)
	echo "del_local_addr_s"
	del_local_addr $DIR_SERVER $2 $3
	exit;;
--del_local_addr_c)
	echo "del_local_addr_c"
	del_local_addr $DIR_CLIENT $2 $3
	exit;;
--show_local_addr_s)
	echo "show_local_addr_s"
	show_local_addr $DIR_SERVER $2
	exit;;
--show_local_addr_c)
	echo "show_local_addr_c"
	show_local_addr $DIR_CLIENT $2
	exit;;
--add_remote_addr_s)
    echo "add_remote_addr_s"
	add_remote_addr $DIR_SERVER $2 $3
	exit;;	
--add_remote_addr_c)
    echo "add_remote_addr_c"
	add_remote_addr $DIR_CLIENT $2 $3
	exit;;	
--del_remote_addr_s)
	echo "del_remote_addr_s"
	del_remote_addr $DIR_SERVER $2 $3
	exit;;
--del_remote_addr_c)
	echo "del_remote_addr_c"
	del_remote_addr $DIR_CLIENT $2 $3
	exit;;
--show_remote_addr_s)
	echo "show_remote_addr_s"
	show_remote_addr $DIR_SERVER $2
	exit;;
--show_remote_addr_c)
	echo "show_remote_addr_c"
	show_remote_addr $DIR_CLIENT $2
	exit;;
--set_local_id_s)
	echo "set_local_id_s"
	set_local_id $DIR_SERVER $2 $3
	exit;;
--set_local_id_c)
	echo "set_local_id_c"
	set_local_id $DIR_CLIENT $2 $3
	exit;;
--unset_local_id_s)
	echo "unset_local_id_s"
	unset_local_id $DIR_SERVER $2
	exit;;
--unset_local_id_c)
	echo "unset_local_id_c"
	unset_local_id $DIR_CLIENT $2
	exit;;
--show_local_id_s)
	echo "show_local_id_s"
	show_local_id $DIR_SERVER $2
	exit;;
--show_local_id_c)
	echo "show_local_id_c"
	show_local_id $DIR_CLIENT $2
	exit;;
--set_remote_id_s)
	echo "set_remote_id_s"
	set_remote_id $DIR_SERVER $2 $3
	exit;;
--set_remote_id_c)
	echo "set_remote_id_c"
	set_remote_id $DIR_CLIENT $2 $3
	exit;;
--unset_remote_id_s)
	echo "unset_remote_id_s"
	unset_remote_id $DIR_SERVER $2
	exit;;
--unset_remote_id_c)
	echo "unset_remote_id_c"
	unset_remote_id $DIR_CLIENT $2
	exit;;
--show_remote_id_s)
	echo "show_remote_id_s"
	show_remote_id $DIR_SERVER $2
	exit;;
--show_remote_id_c)
	echo "show_remote_id_c"
	show_remote_id $DIR_CLIENT $2
	exit;;
--add_local_subnet_s)
    echo "add_local_subnet_s"
	add_local_subnet $DIR_SERVER $2 $3
	exit;;	
--add_local_subnet_c)
    echo "add_local_subnet_c"
	add_local_subnet $DIR_CLIENT $2 $3
	exit;;	
--del_local_subnet_s)
	echo "del_local_subnet_s"
	del_local_subnet $DIR_SERVER $2 $3
	exit;;
--del_local_subnet_c)
	echo "del_local_subnet_c"
	del_local_subnet $DIR_CLIENT $2 $3
	exit;;
--show_local_subnet_s)
	echo "show_local_subnet_s"
	show_local_subnet $DIR_SERVER $2
	exit;;
--show_local_subnet_c)
	echo "show_local_subnet_c"
	show_local_subnet $DIR_CLIENT $2
	exit;;
--add_remote_subnet_s)
    echo "add_remote_subnet_s"
	add_remote_subnet $DIR_SERVER $2 $3
	exit;;	
--add_remote_subnet_c)
    echo "add_remote_subnet_c"
	add_remote_subnet $DIR_CLIENT $2 $3
	exit;;	
--del_remote_subnet_s)
	echo "del_remote_subnet_s"
	del_remote_subnet $DIR_SERVER $2 $3
	exit;;
--del_remote_subnet_c)
	echo "del_remote_subnet_c"
	del_remote_subnet $DIR_CLIENT $2 $3
	exit;;
--show_remote_subnet_s)
	echo "show_remote_subnet_s"
	show_remote_subnet $DIR_SERVER $2
	exit;;
--show_remote_subnet_c)
	echo "show_remote_subnet_c"
	show_remote_subnet $DIR_CLIENT $2
	exit;;
--set_local_sourceip_s)
	echo "set_local_sourceip_s"
	set_local_sourceip $DIR_SERVER $2 $3
	exit;;
--set_local_sourceip_c)
	echo "set_local_sourceip_c"
	set_local_sourceip $DIR_CLIENT $2 $3
	exit;;
--unset_local_sourceip_s)
	echo "unset_local_sourceip_s"
	unset_local_sourceip $DIR_SERVER $2 
	exit;;
--unset_local_sourceip_c)
	echo "unset_local_sourceip_c"
	unset_local_sourceip $DIR_CLIENT $2
	exit;;
--show_local_sourceip_s)
	echo "show_local_sourceip_s"
	show_local_sourceip $DIR_SERVER $2
	exit;;
--show_local_sourceip_c)
	echo "show_local_sourceip_c"
	show_local_sourceip $DIR_CLIENT $2
	exit;;
--set_remote_sourceip_s)
	echo "set_remote_sourceip_s"
	set_remote_sourceip $DIR_SERVER $2 $3
	exit;;
--set_remote_sourceip_c)
	echo "set_remote_sourceip_c"
	set_remote_sourceip $DIR_CLIENT $2 $3
	exit;;
--unset_remote_sourceip_s)
	echo "unset_remote_sourceip_s"
	unset_remote_sourceip $DIR_SERVER $2
	exit;;
--unset_remote_sourceip_c)
	echo "unset_remote_sourceip_c"
	unset_remote_sourceip $DIR_CLIENT $2
	exit;;
--show_remote_sourceip_s)
	echo "show_remote_sourceip_s"
	show_remote_sourceip $DIR_SERVER $2
	exit;;
--show_remote_sourceip_c)
	echo "show_remote_sourceip_c"
	show_remote_sourceip $DIR_CLIENT $2
	exit;;
--set_local_auth_s)
	echo "set_local_auth_s"
	set_local_auth $DIR_SERVER $2 $3
	exit;;
--set_local_auth_c)
	echo "set_local_auth_c"
	set_local_auth $DIR_CLIENT $2 $3
	exit;;
--unset_local_auth_s)
	echo "unset_local_auth_s"
	unset_local_auth $DIR_SERVER $2
	exit;;
--unset_local_auth_c)
	echo "unset_local_auth_c"
	unset_local_auth $DIR_CLIENT $2
	exit;;
--show_local_auth_s)
	echo "show_local_auth_s"
	show_local_auth $DIR_SERVER $2
	exit;;
--show_local_auth_c)
	echo "show_local_auth_c"
	show_local_auth $DIR_CLIENT $2
	exit;;
--set_remote_auth_s)
	echo "set_remote_auth_s"
	set_remote_auth $DIR_SERVER $2 $3
	exit;;
--set_remote_auth_c)
	echo "set_remote_auth_c"
	set_remote_auth $DIR_CLIENT $2 $3
	exit;;
--unset_remote_auth_s)
	echo "unset_remote_auth_s"
	set_remote_auth $DIR_SERVER $2
	exit;;
--unset_remote_auth_c)
	echo "unset_remote_auth_c"
	set_remote_auth $DIR_CLIENT $2
	exit;;
--show_remote_auth_s)
	echo "show_remote_auth_s"
	show_remote_auth $DIR_SERVER $2
	exit;;
--show_remote_auth_c)
	echo "show_remote_auth_c"
	show_remote_auth $DIR_CLIENT $2
	exit;;
--set_auto_s)
	echo "set_auto_s"
	set_auto $DIR_SERVER $2 $3
	exit;;
--set_auto_c)
	echo "set_auto_c"
	set_auto $DIR_CLIENT $2 $3
	exit;;
--unset_auto_s)
	echo "unset_auto_s"
	set_auto $DIR_SERVER $2
	exit;;
--unset_auto_c)
	echo "unset_auto_c"
	set_auto $DIR_CLIENT $2
	exit;;
--show_auto_s)
	echo "show_auto_s"
	show_auto $DIR_SERVER $2
	exit;;
--show_auto_c)
	echo "show_auto_c"
	show_auto $DIR_CLIENT $2
	exit;;
--set_dpdaction_s)
	echo "set_dpdaction_s"
	set_dpdaction $DIR_SERVER $2 $3
	exit;;
--set_dpdaction_c)
	echo "set_dpdaction_c"
	set_dpdaction $DIR_CLIENT $2 $3
	exit;;
--unset_dpdaction_s)
	echo "unset_dpdaction_s"
	set_dpdaction $DIR_SERVER $2
	exit;;
--unset_dpdaction_c)
	echo "unset_dpdaction_c"
	set_dpdaction $DIR_CLIENT $2
	exit;;
--show_dpdaction_s)
	echo "show_dpdaction_s"
	show_dpdaction $DIR_SERVER $2
	exit;;
--show_dpdaction_c)
	echo "show_dpdaction_c"
	show_dpdaction $DIR_CLIENT $2
	exit;;
--set_dpddelay_s)
	echo "set_dpddelay_s"
	set_dpddelay $DIR_SERVER $2 $3
	exit;;
--set_dpddelay_c)
	echo "set_dpddelay_c"
	set_dpddelay $DIR_CLIENT $2 $3
	exit;;
--unset_dpddelay_s)
	echo "unset_dpddelay_s"
	set_dpddelay $DIR_SERVER $2
	exit;;
--unset_dpddelay_c)
	echo "unset_dpddelay_c"
	set_dpddelay $DIR_CLIENT $2
	exit;;
--show_dpddelay_s)
	echo "show_dpddelay_s"
	show_dpddelay $DIR_SERVER $2
	exit;;
--show_dpddelay_c)
	echo "show_dpddelay_c"
	show_dpddelay $DIR_CLIENT $2
	exit;;
--set_dpdtimeout_s)
	echo "set_dpdtimeout_s"
	set_dpdtimeout $DIR_SERVER $2 $3
	exit;;
--set_dpdtimeout_c)
	echo "set_dpdtimeout_c"
	set_dpdtimeout $DIR_CLIENT $2 $3
	exit;;
--unset_dpdtimeout_s)
	echo "unset_dpdtimeout_s"
	set_dpdtimeout $DIR_SERVER $2
	exit;;
--unset_dpdtimeout_c)
	echo "unset_dpdtimeout_c"
	set_dpdtimeout $DIR_CLIENT $2
	exit;;
--show_dpdtimeout_s)
	echo "show_dpdtimeout_s"
	show_dpdtimeout $DIR_SERVER $2
	exit;;
--show_dpdtimeout_c)
	echo "show_dpdtimeout_c"
	show_dpdtimeout $DIR_CLIENT $2
	exit;;
esac
