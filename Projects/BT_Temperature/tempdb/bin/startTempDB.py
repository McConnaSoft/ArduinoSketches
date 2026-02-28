#!/usr/bin/python

import os
import sys
import socket

DSN_BASE = "TempDB"
DSN = DSN_BASE + "_" + socket.gethostname().split(".")[0]
DB_ENGNAME = DSN
DB_NAME = DSN_BASE+".db"
CONS_HOST_PORT = "8080"
CONS_HTTP_PORT = "3009"

ISQL="dbisqlc -q -nogui"
DBDSN = "dbdsn"
DBSTOP = "dbstop"
DBSTART = "dbstart"
DBINIT = "dbinit"
DBERASE = "dberase"
DBSVC = "dbsvc"
DBSRV16 = "dbsrv16"
DBSPAWN = "dbspawn"

os.system(". /opt/sqlanywhere16/bin32/sa_config.sh")

db_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..' )
sql_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'sql')

print( "--START BOLD CONS DB---" )
print( DBSPAWN + " " + DBSRV16 + " -n " + DSN + " -x shmem,tcpip{port=" + CONS_HOST_PORT + "} -xs HTTP(port=" + CONS_HTTP_PORT + ") " + db_dir + "/" + DB_NAME )
os.system(DBSPAWN + " " + DBSRV16 + " -n " + DSN + " -x shmem,tcpip{port=" + CONS_HOST_PORT + "} " + db_dir + "/" + DB_NAME )





