create table book_copy (
book_id int(11),
isbn varchar(30)
)
;

create table book_type (
isbn varchar(30),
book_name varchar(100),
authors varchar(200),
translators varchar(200),
price decimal(10, 2),
publisher varchar(100),
publish_date date,
total int(11),
reserve_qty int(11)
)
;

create table borrow (
r_id varchar(30),
book_id int(11),
borrow_time datetime,
return_time datetime
)
;

create table reader (
r_id varchar(30),
r_name varchar(100),
school_name varchar(20)
)
;

insert into book_copy values (1, '9787040406641');
insert into book_copy values (2, '9787040406641');
insert into book_copy values (3, '9787040406641');
insert into book_copy values (4, '9787040406641');
insert into book_copy values (5, '9787040406641');
insert into book_copy values (6, '9787040406641');
insert into book_copy values (7, '9787040406641');
insert into book_copy values (8, '9787040406641');
insert into book_copy values (9, '9787040406641');
insert into book_copy values (10, '9787040406641');
insert into book_copy values (11, '9787811177657');
insert into book_copy values (12, '9787811177657');
insert into book_copy values (13, '9787811177657');
insert into book_copy values (14, '9787811177657');
insert into book_copy values (15, '9787811177657');
insert into book_copy values (16, '9787811177657');
insert into book_copy values (17, '9787811177657');
insert into book_copy values (18, '9787811177657');
insert into book_copy values (19, '9787811177657');
insert into book_copy values (20, '9787811177657');
insert into book_copy values (21, '9787111681816');
insert into book_copy values (22, '9787111681816');
insert into book_copy values (23, '9787111681816');
insert into book_copy values (24, '9787111681816');
insert into book_copy values (25, '9787111681816');
insert into book_copy values (26, '9787115191120');
insert into book_copy values (27, '9787115191120');
insert into book_copy values (28, '9787115191120');
insert into book_copy values (29, '9787115191120');
insert into book_copy values (30, '9787115191120');
insert into book_copy values (31, '9787115547057');
insert into book_copy values (32, '9787115547057');
insert into book_copy values (33, '9787115547057');

insert into book_type values ('9787040406641', '数据库系统概论', '王珊, 萨师煊', null, 39.60, '高等教育出版社', '2014-09-01', 10, 10);
insert into book_type values ('9787811177657', '高等数学 第七版（上册）', '同济大学数学系', null, 51.80, '高等教育出版社', '2014-07-01', 10, 10);
insert into book_type values ('9787111681816', '数据库系统概念（原书第7版）', 'Abraham Silberschatz, Henry F. Korth, S. Sudarshan', '杨冬青, 李红燕, 张金波', 149.00, '机械工业出版社', '2021-06-01', 5, 5);
insert into book_type values ('9787115191120', 'MySQL必知必会(图灵出品）', 'Ben Forta', '刘晓霞, 钟鸣', 49.00, '人民邮电出版社', '2009-01-01', 5, 5);
insert into book_type values ('9787115547057', 'MySQL是怎样运行的 从根儿上理解MySQL', '小孩子4919', null, 99.00, '人民邮电出版社', '2020-11-01', 3, 3);

insert into borrow values ('20200001', 1 , '2021-09-15 00:00:00', '2021-09-30 00:00:00');
insert into borrow values ('20200002', 2 , '2021-09-20 00:00:00', '2022-10-10 00:00:00');
insert into borrow values ('20200003', 11, '2022-06-15 00:00:00', '2022-06-30 00:00:00');
insert into borrow values ('20200004', 12, '2022-09-15 00:00:00', '2022-09-30 00:00:00');
insert into borrow values ('20200001', 11, '2022-07-18 00:00:00', '2022-08-30 00:00:00');
insert into borrow values ('20200004', 11, '2022-09-15 00:00:00', '2022-09-30 00:00:00');
insert into borrow values ('20220001', 14, '2022-09-15 00:00:00', '2022-09-30 00:00:00');
insert into borrow values ('20220001', 21, '2022-09-15 00:00:00', '2022-09-30 00:00:00');
insert into borrow values ('20220001', 28, '2022-09-15 00:00:00', '2022-09-30 00:00:00');
insert into borrow values ('20220002', 31, '2022-10-15 00:00:00', '2022-10-16 00:00:00');
insert into borrow values ('20220002', 28, '2022-10-17 00:00:00', '2022-10-20 00:00:00');
insert into borrow values ('20210001', 28, '2022-09-15 00:00:00', null);
insert into borrow values ('20220003', 33, '2022-09-15 00:00:00', null);
insert into borrow values ('20220004', 4 , '2022-09-15 00:00:00', null);

insert into reader values ('20200001', '李白', '文学院');
insert into reader values ('20200002', '杜甫', '文学院');
insert into reader values ('20200003', '莫扎特', '艺术学院');
insert into reader values ('20200004', '秦始皇', '管理学院');
insert into reader values ('20200005', '诺贝尔', '化学学院');
insert into reader values ('20210001', '王一鸣', '计算机学院');
insert into reader values ('20210002', '李世民', '管理学院');
insert into reader values ('20210003', '张衡', '地质学院');
insert into reader values ('20210004', '迪士尼', '文学院');
insert into reader values ('20210005', '钟南山', '医学院');
insert into reader values ('20220001', '图灵', '计算机学院');
insert into reader values ('20220002', '姚期智', '计算机学院');
insert into reader values ('20220003', '诸葛亮', '数学学院');
insert into reader values ('20220004', '贝多芬', '艺术学院');
insert into reader values ('20220005', '徐悲鸿', '艺术学院');
insert into reader values ('20220006', '周瑜', '计算机学院');
