start transaction;
savepoint sp_user1;
select balance from campus_card where sno = '20200032';
update campus_card set balance = balance + 1;
savepoint sp_user2;
select balance from campus_card where sno = '20200032';
rollback to sp_user1;
select balance from campus_card where sno = '20200032';
rollback to sp_user2;
commit;

select balance from campus_card where sno = '20200032';
