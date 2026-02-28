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

os.system(". /opt/sqlanywhere16/bin32/sa_config.sh")

db_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..' )
sql_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'sql')


print( "---DSN REMOVE AND INSTALL---" )
os.system(DBDSN + " -d " + DSN + " -y")
os.system(DBDSN + " -w " + DSN + " -y -v -c \"uid=dba;pwd=sql;eng=" + DSN + ";start=dbsrv16 -p 16000 -n " + DSN 
                                 + " -x shmem,tcpip(port=" + CONS_HOST_PORT + ") " 
								 + db_dir + "/" + DB_NAME + " -xs HTTP(port=" + CONS_HTTP_PORT + ")\"")

dbexists = os.path.exists(db_dir +  "/" + DB_NAME)

if (dbexists):
	print ( "---STOPPING DATABASE---" )
	os.system(DBSTOP+ " -y -c \"uid=dba;pwd=sql;eng=" + DSN + "\"")
	print ( "---ERASING DATABASE---" )
	os.system(DBERASE + " -y " + db_dir + "/" + DB_NAME)

print ( "---CREATING DATABASES---" )
os.system (DBINIT + " -z 1252LATIN1 \"" + db_dir + "/" + DB_NAME + "\"")

print ( "---LOADING DATABASE SCHEMA---" )
print ( ISQL + " -c \"dsn=" + DSN + "\" read " + sql_dir + "/TempDB_Schema.sql" )
os.system(ISQL + " -c \"dsn=" + DSN + "\" read " + sql_dir + "/TempDB_Schema.sql")

print ("---DONE---")
