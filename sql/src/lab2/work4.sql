create database Student;
use Student;

create table S (
SNO int primary key,
SNAME varchar(4),
SGENDER varchar(2),
SBIRTH date,
SDEPT varchar(2)
)
;

create table C (
CNO int primary key,
CNAME varchar(16),
CPNO int,
CREDIT int
)
;

create table SC (
SNO int,
CNO int,
GRADE int
)
;

insert into s values (2001,	"李勇", "男", "2000/01/01", "MA");
insert into s values (2002,	"刘晨", "女", "2001/02/01", "IS");
insert into s values (2003,	"王敏", "女", "1999/10/01", "CS");
insert into s values (2004,	"张立", "男", "2001/06/01", "IS");

insert into sc values (2001, 1, 92);
insert into sc values (2001, 2, 85);
insert into sc values (2001, 3, 90);
insert into sc values (2002, 2, 78);
insert into sc values (2002, 3, 84);
insert into sc values (2003, 6, 91);

insert into c values (1, "数据库", 2, 3);
insert into c values (2, "高等数学", null, 5);
insert into c values (3, "信息系统", 1, 2);
insert into c values (4, "操作系统", 5, 3);
insert into c values (5, "数据结构", 6, 3);
insert into c values (6, "C语言", null, 2);
