create table s (
sno int primary key,
sname varchar(4) unique,
sgender varchar(2) default '男',
sbirth date not null,
sdept varchar(2)
);

create table c (
cno int primary key,
cname varchar(16),
cpno int,
credit int,
key c_fk_cpno_idx (cpno),
constraint c_fk_cpno foreign key (cpno) references c (cno)
);

create table sc (
id int primary key auto_increment,
sno int,
cno int,
grade int,
key sc_fk_sno_idx (sno),
key sc_fk_cno_idx (cno),
constraint sc_fk_cno foreign key (cno) references c (cno),
constraint sc_fk_sno foreign key (sno) references s (sno),
check (grade >= 0 and grade <= 100)
);

insert into c values (1, '数据库', null, 2);
insert into c values (2, '高级数据库', 1, 2);
-- insert into c values (3, '不存在的数据库', 2333, 2);
insert into s values (1, '李华', default, current_date(), 'CS');
insert into sc values (default, 1, 1, 100);
-- update sc set grade = 120 where id = 1;
-- insert into sc values (default, 999, 999, 100);
insert into sc values (default, 1, 2, 80);
