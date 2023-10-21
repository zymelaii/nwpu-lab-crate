-- 1)
select pno, sum(qty) from view_spj group by pno;

-- 2)
select * from view_spj where sno = 'S1';
