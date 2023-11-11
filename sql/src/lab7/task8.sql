flush logs;

-- step 1
drop database if exists db1;
create database db1;
use db1;

create table t1 (id int);
create table t2 (id int);

-- step 2
insert into t1 values (11);
insert into t1 values (12);
insert into t1 values (13);

insert into t2 values (21);
insert into t2 values (22);
insert into t2 values (23);

-- step 3
drop table t1;

-- step 4
insert into t2 values (24);
