# 2023秋-练习1

数据库样例见 db.sql。

## QID-19

查询读者号为 '20200001' 的读者所借阅的所有图书号与书名。

```sql
select v.book_id, u.book_name
from book_type u
inner join book_copy v on v.isbn = u.isbn
inner join borrow w on w.book_id = v.book_id
where w.r_id = '20200001'
;
```

## QID-20

检索 '计算机学院' 在 2022 年从未有过借还书记录的读者号以及读者姓名，结果按照读者号降序排列。

```sql
select u.r_id, u.r_name
from reader u
left join borrow v on v.r_id = u.r_id
where u.school_name = '计算机学院' and (
    v.r_id is null or
    year(v.borrow_time) <> 2022 and
    year(v.return_time) <> 2022
)
order by u.r_id desc
;
```

## QID-21

更新读者表，将所有学院为 '计算机学院' 的读者的统一修改学院名为 'CS'。

```sql
update reader
set school_name = 'CS'
where school_name = '计算机学院'
;
```

## QID-22

将读者 '莫扎特' 所借图书的信息从借阅表中删除。

```sql
delete from borrow
where r_id in (
    select r_id
    from reader
    where r_name = '莫扎特'
)
;
```

## QID-23

查询 2022 年度借书册数最多（不论是否已经归还）的前 3 名读者姓名、所在学院、总借书册数。

```sql
select s.r_name, s.school_name, t.total
from reader s
inner join (
    select r_id, count(*) as total
    from borrow
    where year(borrow_time) = 2022
    group by r_id
    order by total desc
    limit 3
) t on s.r_id = t.r_id
;
```
