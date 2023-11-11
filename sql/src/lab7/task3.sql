-- 解决丢失修改 -> 排他锁
-- select ... for update;

-- 解决读脏数据
set session transaction isolation level read committed;

-- 解决不可重复读
set session transaction isolation level repeatable read;

-- 解决幻读
set session transaction isolation level serializable;
