-- 1) jno 允许为空时
insert into view_spj values ('S999', 'P999', 0);
select * from view_spj;
select * from spj where jno is null;

-- 2)
update view_spj set qty = 300 where (sno, pno) = ('S1', 'P1');

-- 3)
delete from view_spj where sno = 'S5';
