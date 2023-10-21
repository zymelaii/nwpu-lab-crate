create view view_dbsel (sno, sname, sgender, grade)
as select s.sno, sname, sgender, grade
from sc
natural join s
natural join c
where cname = '数据库' and sbirth like '2001%';
