--
-- This script file reloads a database that was unloaded using "dbunload".
--
-- (Version:  16.0.0.1324)
--


-- Database file: /Users/i826438/Perforce/bolcode/BOLDemo_DB/db/BOLDemo_CONS.db
-- Database CHAR collation: UTF8BIN, NCHAR collation: UCA
-- Connection Character Set: UTF-8
--
-- CREATE DATABASE command: CREATE DATABASE '/Users/i826438/Perforce/bolcode/BOLDemo_DB/db/BOLDemo_CONS.db' LOG ON '/Users/i826438/Perforce/bolcode/BOLDemo_DB/db/BOLDemo_CONS.log' CASE IGNORE ACCENT IGNORE PAGE SIZE 4096 COLLATION 'UTF8BIN' NCHAR COLLATION 'UCA' BLANK PADDING OFF JCONNECT ON CHECKSUM ON SYSTEM PROC AS DEFINER OFF
--


SET OPTION date_order          = 'YMD'
go

SET OPTION PUBLIC.preserve_source_format = 'OFF'
go

SET TEMPORARY OPTION tsql_outer_joins = 'ON'
go

SET TEMPORARY OPTION st_geometry_describe_type = 'binary'
go

SET TEMPORARY OPTION st_geometry_on_invalid = 'Ignore'
go

SET OPTION PUBLIC.reserved_keywords = ''
go


-------------------------------------------------
--   Create dbspaces
-------------------------------------------------


-------------------------------------------------
--   Create ldap servers
-------------------------------------------------


-------------------------------------------------
--   Create login policies
-------------------------------------------------


-------------------------------------------------
--   Create users
-------------------------------------------------

GRANT CONNECT TO "DBA" IDENTIFIED BY sql
go


-------------------------------------------------
--   Create role definitions
-------------------------------------------------

GRANT ROLE "SYS_AUTH_DBA_ROLE" TO "DBA" WITH ADMIN OPTION WITH NO SYSTEM PRIVILEGE INHERITANCE
go

GRANT ROLE "SYS_AUTH_DBA_ROLE" TO "dbo" WITH ADMIN OPTION WITH NO SYSTEM PRIVILEGE INHERITANCE
go

GRANT ROLE "SYS_AUTH_RESOURCE_ROLE" TO "DBA" WITH NO SYSTEM PRIVILEGE INHERITANCE
go

GRANT ROLE "SYS_AUTH_RESOURCE_ROLE" TO "dbo" WITH NO SYSTEM PRIVILEGE INHERITANCE
go


-------------------------------------------------
--   Create user types
-------------------------------------------------


-------------------------------------------------
--   Create spatial units of measure
-------------------------------------------------

CREATE  SPATIAL UNIT OF MEASURE IF NOT EXISTS  "degree"
	TYPE ANGULAR
	CONVERT USING .017453292519943
go

CREATE  SPATIAL UNIT OF MEASURE IF NOT EXISTS  "meter"
	TYPE LINEAR
	CONVERT USING 1
go

CREATE  SPATIAL UNIT OF MEASURE IF NOT EXISTS  "planar degree"
	TYPE LINEAR
	CONVERT USING 111120
go


-------------------------------------------------
--   Create spatial reference systems
-------------------------------------------------

CREATE  SPATIAL REFERENCE SYSTEM IF NOT EXISTS  "Default"
	IDENTIFIED BY 0
	ORGANIZATION 'Sybase' IDENTIFIED BY 0
	LINEAR UNIT OF MEASURE "metre"
	TYPE PLANAR
	SNAP TO GRID .000001
	TOLERANCE .000001
	AXIS ORDER 'x/y/z/m'
	POLYGON FORMAT 'EvenOdd'
	COORDINATE X BETWEEN -1000000 AND 1000000
	COORDINATE Y BETWEEN -1000000 AND 1000000
go

CREATE  SPATIAL REFERENCE SYSTEM IF NOT EXISTS  "sa_octahedral_gnomonic"
	IDENTIFIED BY 2147483647
	ORGANIZATION 'Sybase' IDENTIFIED BY 2147483647
	LINEAR UNIT OF MEASURE "metre"
	TYPE PLANAR
	SNAP TO GRID .000000000001
	TOLERANCE .000000000001
	AXIS ORDER 'x/y/z/m'
	POLYGON FORMAT 'EvenOdd'
	COORDINATE X BETWEEN 0 AND 1
	COORDINATE Y BETWEEN -1 AND 1
go

CREATE  SPATIAL REFERENCE SYSTEM IF NOT EXISTS  "sa_planar_unbounded"
	IDENTIFIED BY 2147483646
	ORGANIZATION 'Sybase' IDENTIFIED BY 2147483646
	LINEAR UNIT OF MEASURE "metre"
	TYPE PLANAR
	SNAP TO GRID 0
	TOLERANCE 0
	AXIS ORDER 'x/y/z/m'
	POLYGON FORMAT 'EvenOdd'
	COORDINATE X BETWEEN -1.79769313E308 AND 1.79769313E308
	COORDINATE Y BETWEEN -1.79769313E308 AND 1.79769313E308
go

CREATE  SPATIAL REFERENCE SYSTEM IF NOT EXISTS  "WGS 84"
	IDENTIFIED BY 4326
	ORGANIZATION 'EPSG' IDENTIFIED BY 4326
	LINEAR UNIT OF MEASURE "metre"
	ANGULAR UNIT OF MEASURE "degree"
	TYPE ROUND EARTH
	SNAP TO GRID 0
	TOLERANCE 0
	ELLIPSOID SEMI MAJOR AXIS 6378137 INVERSE FLATTENING 298.257223563
	AXIS ORDER 'long/lat/z/m'
	POLYGON FORMAT 'EvenOdd'
	COORDINATE LATITUDE BETWEEN -90 AND 90
	COORDINATE LONGITUDE BETWEEN -180 AND 180
	DEFINITION 'GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.01745329251994328,AUTHORITY["EPSG","9122"]],AUTHORITY["EPSG","4326"]]'
	TRANSFORM DEFINITION '+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs'
go

CREATE  SPATIAL REFERENCE SYSTEM IF NOT EXISTS  "WGS 84 (planar)"
	IDENTIFIED BY 1000004326
	ORGANIZATION 'EPSG' IDENTIFIED BY 4326
	LINEAR UNIT OF MEASURE "planar degree"
	ANGULAR UNIT OF MEASURE "degree"
	TYPE PLANAR
	SNAP TO GRID .000000001
	TOLERANCE .000000001
	ELLIPSOID SEMI MAJOR AXIS 6378137 INVERSE FLATTENING 298.257223563
	AXIS ORDER 'long/lat/z/m'
	POLYGON FORMAT 'EvenOdd'
	COORDINATE LATITUDE BETWEEN -90 AND 90
	COORDINATE LONGITUDE BETWEEN -180 AND 180
	DEFINITION 'GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.01745329251994328,AUTHORITY["EPSG","9122"]],AUTHORITY["EPSG","4326"]]'
	TRANSFORM DEFINITION '+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs'
go


-------------------------------------------------
--   Create remote servers
-------------------------------------------------


-------------------------------------------------
--   Create dbspace permissions
-------------------------------------------------

begin
    for dbspaces as dbcurs cursor for 
	select privilege_type, dbspace_name, user_name 
		from SYS.SYSDBSPACEPERM p 
		join SYS.SYSDBSPACE d on p.dbspace_id = d.dbspace_id
		join SYS.SYSUSER u on u.user_id = p.grantee
    do
	execute immediate 'revoke ' + if privilege_type = 1 then 'CREATE' else 'UNKNOWN' endif + ' on "' + dbspace_name + '" from "' + user_name + '"'
    end for;
end

go

grant CREATE on "system" to "PUBLIC"
go

grant CREATE on "temporary" to "PUBLIC"
go


-------------------------------------------------
--   Create external environments
-------------------------------------------------

IF EXISTS( SELECT * FROM SYS.SYSEXTERNENV WHERE name = 'java' ) THEN 
    ALTER EXTERNAL ENVIRONMENT "java"
        LOCATION '' 
END IF 
go

IF EXISTS( SELECT * FROM SYS.SYSEXTERNENV WHERE name = 'perl' ) THEN 
    ALTER EXTERNAL ENVIRONMENT "perl"
        LOCATION 'perl' 
END IF 
go

IF EXISTS( SELECT * FROM SYS.SYSEXTERNENV WHERE name = 'clr' ) THEN 
    ALTER EXTERNAL ENVIRONMENT "clr"
        LOCATION 'dbextclr[VER_MAJOR]' 
END IF 
go

IF EXISTS( SELECT * FROM SYS.SYSEXTERNENV WHERE name = 'php' ) THEN 
    ALTER EXTERNAL ENVIRONMENT "php"
        LOCATION 'php' 
END IF 
go

IF EXISTS( SELECT * FROM SYS.SYSEXTERNENV WHERE name = 'c_esql32' ) THEN 
    ALTER EXTERNAL ENVIRONMENT "c_esql32"
        LOCATION 'bin32[SLASH]dbexternc[VER_MAJOR]' 
END IF 
go

IF EXISTS( SELECT * FROM SYS.SYSEXTERNENV WHERE name = 'c_odbc32' ) THEN 
    ALTER EXTERNAL ENVIRONMENT "c_odbc32"
        LOCATION 'bin32[SLASH]dbexternc[VER_MAJOR]' 
END IF 
go

IF EXISTS( SELECT * FROM SYS.SYSEXTERNENV WHERE name = 'c_esql64' ) THEN 
    ALTER EXTERNAL ENVIRONMENT "c_esql64"
        LOCATION 'bin64[SLASH]dbexternc[VER_MAJOR]' 
END IF 
go

IF EXISTS( SELECT * FROM SYS.SYSEXTERNENV WHERE name = 'c_odbc64' ) THEN 
    ALTER EXTERNAL ENVIRONMENT "c_odbc64"
        LOCATION 'bin64[SLASH]dbexternc[VER_MAJOR]' 
END IF 
go

IF EXISTS( SELECT * FROM SYS.SYSEXTERNENV WHERE name = 'dbmlsync' ) THEN 
    ALTER EXTERNAL ENVIRONMENT "dbmlsync"
        LOCATION '' 
END IF 
go


-------------------------------------------------
--   Create external environment objects
-------------------------------------------------


-------------------------------------------------
--   Create certificates
-------------------------------------------------

create temporary procedure sa_unload_display_table_status( 
    msgid int, ord int, numtabs int, user_name char(128), table_name char(128) )
begin 
  declare @fullmsg long varchar; 
  set @fullmsg = lang_message( msgid ) ||
      ' (' || ord || '/' || numtabs || ') ' ||
      '"' || user_name || '"."' || table_name || '"'; 
  message @fullmsg type info to client; 
end
go


-------------------------------------------------
--   Create sequences
-------------------------------------------------

-------------------------------------------------
--   Create tables
-------------------------------------------------

CREATE TABLE "DBA"."SENSOR" (
    "SENSOR_ID"                      integer NOT NULL DEFAULT autoincrement
    ,"NAME"                          varchar(128) NOT NULL
    ,"CREATED_DATE"                  "datetime" NOT NULL DEFAULT NOW()
    ,PRIMARY KEY ("SENSOR_ID" ASC)                          
)
go

CREATE TABLE "DBA"."TEMP_READING" (
    "READING_ID"                     integer NOT NULL DEFAULT autoincrement
   ,"SENSOR_ID"                      integer NOT NULL  
   ,"TEMPERATURE"                    float NOT NULL
   ,"PRESSURE"                       float NULL
   ,"READING_DATE"                   "datetime" NOT NULL DEFAULT NOW()
   ,PRIMARY KEY ("READING_ID" ASC) 
)
go

commit work
go


-------------------------------------------------
--   Create indexes
-------------------------------------------------

call sa_unload_display_table_status( 17738, 1, 7, 'DBA', 'BOLD_FEED' )
go

call sa_unload_display_table_status( 17738, 3, 7, 'DBA', 'BOLD_USER' )
go

call sa_unload_display_table_status( 17738, 5, 7, 'DBA', 'BOLD_SAMPLE' )
go

ALTER TABLE "DBA"."TEMP_READING"
    ADD NOT NULL FOREIGN KEY "SENSOR" ("SENSOR_ID" ASC)
    REFERENCES "DBA"."SENSOR" ("SENSOR_ID")
    ON DELETE CASCADE 
go

commit work
go


-------------------------------------------------
--   Create immediate materialized views
-------------------------------------------------

commit
go



-------------------------------------------------
--   Create functions
-------------------------------------------------

commit
go



-------------------------------------------------
--   Create views
-------------------------------------------------

commit
go


SET TEMPORARY OPTION force_view_creation='ON'
go

SET TEMPORARY OPTION force_view_creation='OFF'
go

call dbo.sa_recompile_views(1)
go


-------------------------------------------------
--   Create user messages
-------------------------------------------------


-------------------------------------------------
--   Create procedures
-------------------------------------------------

commit
go


begin 
  declare prev_count int;
  declare new_count int;
  declare local temporary table dependent_proc ( 
    proc_id    unsigned int    not null, 
    primary key (proc_id) 
  ) in system not transactional; 
  set prev_count = -1;
  lp: loop
      truncate table dependent_proc;
      insert into dependent_proc 
	select proc_id from SYS.SYSPROCEDURE p 
	where exists (select * from SYS.SYSPROCPARM pp 
	    where pp.proc_id = p.proc_id 
	    and parm_name = 'expression' 
	    and parm_type = 1 
	   and domain_id = 1); 
      select count(*) into new_count from dependent_proc;
      if new_count = 0 or (new_count >= prev_count and prev_count >= 0) then
        leave lp;
      end if;
      set prev_count = new_count;
      for l1 as c1 cursor for 
	select u.user_name, proc_name
	from SYS.SYSPROCEDURE p
	    join dependent_proc d on (d.proc_id = p.proc_id)
	    join SYS.SYSUSER u on (p.creator = u.user_id)
      do
        begin
	   execute immediate with quotes on
            'alter procedure "' || user_name || '"."' || proc_name || '" recompile';
          exception when others then
        end
      end for;
  end loop;
end

go

call dbo.sa_recompile_views(0)
go

-------------------------------------------------
--   Create triggers
-------------------------------------------------

commit
go

-------------------------------------------------
--   Create procedures, functions, and web services
-------------------------------------------------
commit
go


-------------------------------------------------
--   Create events
-------------------------------------------------

commit
go



-------------------------------------------------
--   Create services
-------------------------------------------------

commit
go



-------------------------------------------------
--   Create mirror options and servers
-------------------------------------------------


-------------------------------------------------
--   Set DBA password
-------------------------------------------------

GRANT CONNECT TO DBA IDENTIFIED BY ENCRYPTED '\x01\x44\x7f\xd1\xe7\xdf\x7b\x94\x27\x90\x36\x5b\x02\xa4\xe0\x1c\xb2\xd7\x48\x3d\x75\x46\x98\x7f\x9f\x69\x89\x71\x9d\xe3\x70\x6f\xb3\xcd\x7f\x2e\x4f'
go


-------------------------------------------------
--   Create options
-------------------------------------------------

SET OPTION date_order =
go

SET OPTION PUBLIC.preserve_source_format =
go

SET OPTION "PUBLIC"."preserve_source_format"='On'
go

-------------------------------------------------
--   Setup Data
-------------------------------------------------
commit
go