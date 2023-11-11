drop database if exists student;
create database student;
use student;

create table icbc_card (
icbcid int,
sno varchar(8),
balance int
)
;

create table campus_card (
sno varchar(8),
balance int
)
;

insert into campus_card values ('20200032', 1);
insert into campus_card values ('20200033', 100);
insert into icbc_card values (1, '20200032', 300);
insert into icbc_card values (2, '20200033', 400);
