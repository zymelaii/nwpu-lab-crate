create user 'wangming'@'localhost';
create user 'liyong'@'localhost';

grant select, insert on student.* to 'wangming'@'localhost';
grant select, insert, delete, update, create on student to 'liyong'@'localhost';
