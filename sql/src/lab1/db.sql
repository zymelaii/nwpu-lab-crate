create table SPJ (
SNO varchar(4) not null,
PNO varchar(4) not null,
JNO varchar(4) not null,
QTY int not null
)
;

create table S (
SNO varchar(4) primary key,
SNAME varchar(10) not null,
STATUS int not null,
CITY varchar(10) not null
)
;

create table P (
PNO varchar(4) primary key,
PNAME varchar(10) not null,
COLOR varchar(2) not null,
WEIGHT int not null
)
;

create table J (
JNO varchar(4) primary key,
JNAME varchar(10) not null,
CITY varchar(10) not null
)
;

insert into SPJ values ("S1", "P1", "J1", 200);
insert into SPJ values ("S1", "P1", "J3", 100);
insert into SPJ values ("S1", "P1", "J4", 700);
insert into SPJ values ("S1", "P2", "J2", 100);
insert into SPJ values ("S2", "P3", "J1", 400);
insert into SPJ values ("S2", "P3", "J2", 200);
insert into SPJ values ("S2", "P3", "J4", 500);
insert into SPJ values ("S2", "P3", "J5", 400);
insert into SPJ values ("S2", "P5", "J1", 400);
insert into SPJ values ("S2", "P5", "J2", 100);
insert into SPJ values ("S3", "P1", "J1", 200);
insert into SPJ values ("S3", "P3", "J1", 200);
insert into SPJ values ("S4", "P5", "J1", 200);
insert into SPJ values ("S4", "P6", "J3", 100);
insert into SPJ values ("S4", "P6", "J4", 300);
insert into SPJ values ("S5", "P2", "J4", 100);
insert into SPJ values ("S5", "P3", "J1", 200);
insert into SPJ values ("S5", "P6", "J2", 200);
insert into SPJ values ("S5", "P6", "J4", 500);

insert into S values ("S1", "精益", 20, "天津");
insert into S values ("S2", "盛锡", 10, "北京");
insert into S values ("S3", "东方红", 30, "北京");
insert into S values ("S4", "丰泰盛", 20, "天津");
insert into S values ("S5", "为民", 30, "上海");

insert into P values ("P1", "螺母", "红", 12);
insert into P values ("P2", "螺栓", "绿", 17);
insert into P values ("P3", "螺丝刀", "蓝", 14);
insert into P values ("P4", "螺丝刀", "红", 14);
insert into P values ("P5", "凸轮", "蓝", 40);
insert into P values ("P6", "齿轮", "红", 30);

insert into J values ("J1", "三建", "北京");
insert into J values ("J2", "一汽", "长春");
insert into J values ("J3", "弹簧厂", "天津");
insert into J values ("J4", "造船厂", "天津");
insert into J values ("J5", "机车厂", "西安");
insert into J values ("J6", "无线电厂", "常州");
insert into J values ("J7", "半导体厂", "南京");
