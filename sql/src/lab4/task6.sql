use student

-- 1)
create index ix_cname on c (cname);

-- 2)
create index ix_ngd on s (sname asc, sgender asc, sdept asc);

-- 3)
drop index ix_cno on c;

-- 4)
explain select * from c;
explain select * from c where cno = '1';
explain select * from c where cname = '数据库';
explain select * from c where cname like '%数据库%';
explain select * from c where cname like '数据库%';
explain select * from s where sname = '张立' and sno = '2001';
explain select * from s where sname = '张立' and sgender = '男' and sdept = 'IS';
explain select * from s where sname = '张立' and sgender = '男';
explain select * from s where sname = '张立';
explain select * from s where sgender = '男';
explain select * from s where sgender = '男' and sdept = 'IS';
