create table e (
eid int primary key,
ename varchar(8),
age int,
job varchar(8),
salary int,
dept_id int
);

create table d (
dept_id int primary key,
title varchar(10),
manager varchar(8),
addr varchar(32),
telephone varchar(14)
);

insert into e values (1, '刘星', 18, '活着', 0, 1);
insert into e values (2, '张新', 18, '活着', 0, 1);
insert into e values (3, '周平', 18, '活着', 0, 1);
insert into e values (4, '杨兰', 18, '活着', 0, 1);
insert into d values (1, '人世部', '不可知', '猎户座左旋臂', '09091888091');
create user 'wangming'@'localhost';
create user 'liyong'@'localhost';
create user 'liuxing'@'localhost';
create user 'zhangxin'@'localhost';
create user 'zhouping'@'localhost';
create user 'yanglan'@'localhost';

-- task 1
grant select on student.d to 'wangming'@'localhost';
grant select on student.e to 'wangming'@'localhost';
revoke select on student.d from 'wangming'@'localhost';
revoke select on student.e from 'wangming'@'localhost';

-- task 2
grant insert, delete on student.d to 'liyong'@'localhost';
grant insert, delete on student.e to 'liyong'@'localhost';
revoke insert, delete on student.d from 'liyong'@'localhost';
revoke insert, delete on student.e from 'liyong'@'localhost';

-- task 3
create view student.employee_view
as select * from e
where concat(ename, '@localhost') = user();
create user '刘星'@'localhost';
grant select on student.employee_view to '刘星'@'localhost';
revoke select on student.employee_view from '刘星'@'localhost';

-- task 4
grant select, update(salary) on student.e to 'liuxing'@'localhost';

-- task 5
grant alter on student.e to 'zhangxin'@'localhost';
grant alter on student.d to 'zhangxin'@'localhost';

-- task 6
grant all on student.e to 'zhouping'@'localhost';
grant all on student.d to 'zhouping'@'localhost';
grant grant option on *.* to 'zhouping'@'localhost';

grant select on student.e to 'zhangxin'@'localhost' with grant option;
revoke select on student.e from 'zhangxin'@'localhost';

revoke all privileges on student.e from 'zhouping'@'localhost';
revoke all privileges on student.d from 'zhouping'@'localhost';

-- task 7
create view student.view_yanglan
as select max(salary), min(salary), avg(salary)
from e;
grant select on student.view_yanglan to 'yanglan'@'localhost';
