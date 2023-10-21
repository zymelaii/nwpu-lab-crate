alter table s
add constraint s_chk_sgender check (sgender in ('男', '女'));

alter table s
drop constraint s_chk_sgender;

alter table sc
drop constraint sc_fk_cno;
alter table sc
drop constraint sc_fk_sno;

alter table s
add tname varchar(32)
check (tname regexp '[a-zA-Z]{8,}');
