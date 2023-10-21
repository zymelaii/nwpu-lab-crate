create view view_spj
as select sno, pno, qty from spj
where jno in (select jno from j where jname = '三建');

create view view_spj_2
as select sno, pno, qty from spj, j
where j.jno = spj.jno and jname = '三建';
