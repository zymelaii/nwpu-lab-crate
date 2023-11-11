# The proper term is pseudo_replica_mode, but we use this compatibility alias
# to make the statement usable on server versions 8.0.24 and older.
/*!50530 SET @@SESSION.PSEUDO_SLAVE_MODE=1*/;
/*!50003 SET @OLD_COMPLETION_TYPE=@@COMPLETION_TYPE,COMPLETION_TYPE=0*/;
DELIMITER /*!*/;
# at 4
#231111 16:04:06 server id 1  end_log_pos 126 CRC32 0xf09e9d44 	Start: binlog v 4, server v 8.1.0 created 231111 16:04:06
# Warning: this binlog is either in use or was not closed properly.
BINLOG '
djVPZQ8BAAAAegAAAH4AAAABAAQAOC4xLjAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAEwANAAgAAAAABAAEAAAAYgAEGggAAAAICAgCAAAACgoKKioAEjQA
CigAAUSdnvA=
'/*!*/;
/*!50616 SET @@SESSION.GTID_NEXT='AUTOMATIC'*//*!*/;
# at 126
# at 157
# at 234
#231111 16:04:06 server id 1  end_log_pos 345 CRC32 0xa4c6e143 	Query	thread_id=8	exec_time=0	error_code=0	Xid = 53
SET TIMESTAMP=1699689846/*!*/;
SET @@session.pseudo_thread_id=8/*!*/;
SET @@session.foreign_key_checks=1, @@session.sql_auto_is_null=0, @@session.unique_checks=1, @@session.autocommit=1/*!*/;
SET @@session.sql_mode=1168113696/*!*/;
SET @@session.auto_increment_increment=1, @@session.auto_increment_offset=1/*!*/;
/*!\C utf8mb3 *//*!*/;
SET @@session.character_set_client=33,@@session.collation_connection=33,@@session.collation_server=192/*!*/;
SET @@session.lc_time_names=0/*!*/;
SET @@session.collation_database=DEFAULT/*!*/;
/*!80011 SET @@session.default_collation_for_utf8mb4=255*//*!*/;
drop database if exists db1
/*!*/;
# at 345
# at 422
#231111 16:04:06 server id 1  end_log_pos 527 CRC32 0xa4837786 	Query	thread_id=8	exec_time=0	error_code=0	Xid = 55
SET TIMESTAMP=1699689846/*!*/;
/*!80016 SET @@session.default_table_encryption=0*//*!*/;
create database db1
/*!*/;
# at 527
# at 604
#231111 16:04:06 server id 1  end_log_pos 714 CRC32 0x7a660c49 	Query	thread_id=8	exec_time=0	error_code=0	Xid = 58
use `db1`/*!*/;
SET TIMESTAMP=1699689846/*!*/;
/*!80013 SET @@session.sql_require_primary_key=0*//*!*/;
create table t1 (id int)
/*!*/;
# at 714
# at 791
#231111 16:04:06 server id 1  end_log_pos 901 CRC32 0x967e5783 	Query	thread_id=8	exec_time=0	error_code=0	Xid = 59
SET TIMESTAMP=1699689846/*!*/;
/*!80013 SET @@session.sql_require_primary_key=0*//*!*/;
create table t2 (id int)
/*!*/;
# at 901
# at 980
#231111 16:04:06 server id 1  end_log_pos 1054 CRC32 0x9bf9e2d8 	Query	thread_id=8	exec_time=0	error_code=0
SET TIMESTAMP=1699689846/*!*/;
BEGIN
/*!*/;
# at 1054
#231111 16:04:06 server id 1  end_log_pos 1101 CRC32 0x34163558 	Table_map: `db1`.`t1` mapped to number 93
# at 1101
#231111 16:04:06 server id 1  end_log_pos 1141 CRC32 0x19328712 	Write_rows: table id 93 flags: STMT_END_F

BINLOG '
djVPZRMBAAAALwAAAE0EAAAAAF0AAAAAAAEAA2RiMQACdDEAAQMAAQEBAFg1FjQ=
djVPZR4BAAAAKAAAAHUEAAAAAF0AAAAAAAEAAgAB/wALAAAAEocyGQ==
'/*!*/;
# at 1141
#231111 16:04:06 server id 1  end_log_pos 1172 CRC32 0x8bd6e31c 	Xid = 61
COMMIT/*!*/;
# at 1172
# at 1251
#231111 16:04:06 server id 1  end_log_pos 1325 CRC32 0x5df7e7a0 	Query	thread_id=8	exec_time=0	error_code=0
SET TIMESTAMP=1699689846/*!*/;
BEGIN
/*!*/;
# at 1325
#231111 16:04:06 server id 1  end_log_pos 1372 CRC32 0x1b9d3376 	Table_map: `db1`.`t1` mapped to number 93
# at 1372
#231111 16:04:06 server id 1  end_log_pos 1412 CRC32 0x0577903b 	Write_rows: table id 93 flags: STMT_END_F

BINLOG '
djVPZRMBAAAALwAAAFwFAAAAAF0AAAAAAAEAA2RiMQACdDEAAQMAAQEBAHYznRs=
djVPZR4BAAAAKAAAAIQFAAAAAF0AAAAAAAEAAgAB/wAMAAAAO5B3BQ==
'/*!*/;
# at 1412
#231111 16:04:06 server id 1  end_log_pos 1443 CRC32 0x16d4d30e 	Xid = 62
COMMIT/*!*/;
# at 1443
# at 1522
#231111 16:04:06 server id 1  end_log_pos 1596 CRC32 0x323e21bc 	Query	thread_id=8	exec_time=0	error_code=0
SET TIMESTAMP=1699689846/*!*/;
BEGIN
/*!*/;
# at 1596
#231111 16:04:06 server id 1  end_log_pos 1643 CRC32 0x9a83b524 	Table_map: `db1`.`t1` mapped to number 93
# at 1643
#231111 16:04:06 server id 1  end_log_pos 1683 CRC32 0xf37844fc 	Write_rows: table id 93 flags: STMT_END_F

BINLOG '
djVPZRMBAAAALwAAAGsGAAAAAF0AAAAAAAEAA2RiMQACdDEAAQMAAQEBACS1g5o=
djVPZR4BAAAAKAAAAJMGAAAAAF0AAAAAAAEAAgAB/wANAAAA/ER48w==
'/*!*/;
# at 1683
#231111 16:04:06 server id 1  end_log_pos 1714 CRC32 0x9c096244 	Xid = 63
COMMIT/*!*/;
# at 1714
# at 1793
#231111 16:04:06 server id 1  end_log_pos 1867 CRC32 0x0a9aeb11 	Query	thread_id=8	exec_time=0	error_code=0
SET TIMESTAMP=1699689846/*!*/;
BEGIN
/*!*/;
# at 1867
#231111 16:04:06 server id 1  end_log_pos 1914 CRC32 0xd49b26e7 	Table_map: `db1`.`t2` mapped to number 94
# at 1914
#231111 16:04:06 server id 1  end_log_pos 1954 CRC32 0xba8364c8 	Write_rows: table id 94 flags: STMT_END_F

BINLOG '
djVPZRMBAAAALwAAAHoHAAAAAF4AAAAAAAEAA2RiMQACdDIAAQMAAQEBAOcmm9Q=
djVPZR4BAAAAKAAAAKIHAAAAAF4AAAAAAAEAAgAB/wAVAAAAyGSDug==
'/*!*/;
# at 1954
#231111 16:04:06 server id 1  end_log_pos 1985 CRC32 0xa2e1ae63 	Xid = 64
COMMIT/*!*/;
# at 1985
# at 2064
#231111 16:04:06 server id 1  end_log_pos 2138 CRC32 0xffaffce9 	Query	thread_id=8	exec_time=0	error_code=0
SET TIMESTAMP=1699689846/*!*/;
BEGIN
/*!*/;
# at 2138
#231111 16:04:06 server id 1  end_log_pos 2185 CRC32 0x74f6913d 	Table_map: `db1`.`t2` mapped to number 94
# at 2185
#231111 16:04:06 server id 1  end_log_pos 2225 CRC32 0x033b6c17 	Write_rows: table id 94 flags: STMT_END_F

BINLOG '
djVPZRMBAAAALwAAAIkIAAAAAF4AAAAAAAEAA2RiMQACdDIAAQMAAQEBAD2R9nQ=
djVPZR4BAAAAKAAAALEIAAAAAF4AAAAAAAEAAgAB/wAWAAAAF2w7Aw==
'/*!*/;
# at 2225
#231111 16:04:06 server id 1  end_log_pos 2256 CRC32 0xb286a6d2 	Xid = 65
COMMIT/*!*/;
# at 2256
# at 2335
#231111 16:04:06 server id 1  end_log_pos 2409 CRC32 0x39a1f991 	Query	thread_id=8	exec_time=0	error_code=0
SET TIMESTAMP=1699689846/*!*/;
BEGIN
/*!*/;
# at 2409
#231111 16:04:06 server id 1  end_log_pos 2456 CRC32 0x5b7d9713 	Table_map: `db1`.`t2` mapped to number 94
# at 2456
#231111 16:04:06 server id 1  end_log_pos 2496 CRC32 0x31811682 	Write_rows: table id 94 flags: STMT_END_F

BINLOG '
djVPZRMBAAAALwAAAJgJAAAAAF4AAAAAAAEAA2RiMQACdDIAAQMAAQEBABOXfVs=
djVPZR4BAAAAKAAAAMAJAAAAAF4AAAAAAAEAAgAB/wAXAAAAghaBMQ==
'/*!*/;
# at 2496
#231111 16:04:06 server id 1  end_log_pos 2527 CRC32 0xb70bcc42 	Xid = 66
COMMIT/*!*/;
DELIMITER ;
# End of log file
/*!50003 SET COMPLETION_TYPE=@OLD_COMPLETION_TYPE*/;
/*!50530 SET @@SESSION.PSEUDO_SLAVE_MODE=0*/;
