delimiter $$
drop procedure if exists sleep;
create procedure sleep ()
begin
    select sleep(3);
end $$
delimiter ;

-- task 1
-- 转账
delimiter $$
drop procedure if exists transfer_money;
create procedure transfer_money
(
    in sno_ varchar(8),
    in money_ int
)
begin
	declare temp_balance int;
    start transaction;
    select balance from icbc_card
    where sno = sno_
    into temp_balance
    ;
    if (temp_balance > 200)
    then
		update icbc_card
        set balance = balance - money_
        where sno = sno_
        ;
        update campus_card
        set balance = balance + money_
        where sno = sno_
        ;
        commit;
	else
		rollback;
	end if;
end $$
delimiter ;

-- task 2
-- 读后延迟写 -> 修改丢失
delimiter $$
drop procedure if exists read_with_lazy_write;
create procedure read_with_lazy_write ()
begin
	declare temp_balance int;
    start transaction;
    select balance from campus_card
    where sno = '20200032' into temp_balance;
    call sleep;
    update campus_card set balance = temp_balance + 1
    where sno = '20200032';
    commit;
end $$
delimiter ;

-- 写后延迟回滚 -> 读脏数据-构造
delimiter $$
drop procedure if exists write_with_lazy_rollback;
create procedure write_with_lazy_rollback ()
begin
    start transaction;
	update campus_card set balance = balance + 666
    where sno = '20200032'
    ;
    call sleep;
    rollback;
end $$
delimiter ;

-- 回滚前读 -> 读脏数据-操作
delimiter $$
drop procedure if exists read_before_lazy_rollback;
create procedure read_before_lazy_rollback ()
begin
    start transaction;
	select balance from campus_card
    where sno = '20200032'
    ;
    commit;
end $$
delimiter ;

-- 读后延迟读 -> 不可重复读-操作1
delimiter $$
drop procedure if exists read_with_lazy_read;
create procedure read_with_lazy_read ()
begin
    start transaction;
	select balance from campus_card
    where sno = '20200032'
    ;
    call sleep;
    select balance from campus_card
    where sno = '20200032'
    ;
    commit;
end $$
delimiter ;

-- 延迟读间写 -> 不可重复读-操作2
delimiter $$
drop procedure if exists update_during_lazy_read;
create procedure update_during_lazy_read ()
begin
    start transaction;
    update campus_card set balance = balance + 2333
    where sno = '20200032'
    ;
    commit;
end $$
delimiter ;

-- 延迟两次读行 -> 幻读-操作1
delimiter $$
drop procedure if exists lazy_read_rows_twice;
create procedure lazy_read_rows_twice ()
begin
    start transaction;
	select * from campus_card;
    call sleep;
    select * from campus_card;
    commit;
end $$
delimiter ;

-- 读行间插入 -> 幻读-操作2
delimiter $$
drop procedure if exists insert_during_read_rows;
create procedure insert_during_read_rows ()
begin
    start transaction;
    insert into campus_card values ('66666666', 999);
    commit;
end $$
delimiter ;

-- task 3
-- 读后延迟写 -> 修改丢失 -> 使用排他锁
delimiter $$
drop procedure if exists read_with_lazy_write_exclusive;
create procedure read_with_lazy_write_exclusive ()
begin
	declare temp_balance int;
    start transaction;
    select balance from campus_card
    where sno = '20200032' into temp_balance
    for update
    ;
    call sleep;
    update campus_card set balance = temp_balance + 1
    where sno = '20200032';
    commit;
end $$
delimiter ;

-- task 4
delimiter $$
drop procedure if exists inc_campus_balance;
create procedure inc_campus_balance ()
begin
    start transaction;
	update campus_card set balance = balance + 1;
    call sleep;
	commit;
end $$
delimiter ;

-- task 5
-- 死锁构造-操作1
delimiter $$
drop procedure if exists dead_lock_ctor1;
create procedure dead_lock_ctor1 ()
begin
    start transaction;
	update campus_card set balance = balance + 1;
    call sleep;
    update icbc_card set balance = balance + 1;
	commit;
end $$
delimiter ;

-- 死锁构造-操作2
delimiter $$
drop procedure if exists dead_lock_ctor2;
create procedure dead_lock_ctor2 ()
begin
    start transaction;
	update icbc_card set balance = balance - 1;
    call sleep;
    update campus_card set balance = balance - 1;
    commit;
end $$
delimiter ;
