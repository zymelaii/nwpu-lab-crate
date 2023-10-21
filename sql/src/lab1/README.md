# 数据库实验 Lab1 数据库和表的创建与管理

1. 数据库备份还原
2. 数据库、基本表定义、修改、删除

## 实验目的

1. 熟练掌握MySQL中命令行以及图形用户界面的基本连接方法。
2. 熟练掌握SQL语句创建数据库和表。
3. 熟练掌握数据库和基本表的修改和删除方法。
4. 掌握备份和还原数据库的基本方法。
5. 了解MySQL中数据库的逻辑结构和物理结构等。

## 实验内容

预备工作：使用 mysql 在本地默认端口创建数据库（以实验指导中给出的供应情况表 SPJ 为例），作为样例数据库用于进行进行实验内容。

数据库的创建由 db.sql 给出。

连接到 MySQL 服务后执行以下命令创建样例数据库（命名为 lab1_sample）：

```shell
create database lab1_sample;
connect lab1_sample;
\. db.sql
```

> PS: 提供的 db.sql 采用的是 UTF-8 编码，注意按情况调整 MySQL 的默认编码，如直接在配置 MySQL 服务时直接指定 server 使用的字符集，一劳永逸。

### 利用图形用户界面连接数据库

通过 MySQL 官网下载 [MySQL Workbench](https://downloads.mysql.com/archives/workbench/)，在该实验中，使用版本为 `8.0.33`。

### 利用命令行连接数据库

![](./assets/2.jpg)

### 利用图形用户界面创建、备份、删除和还原数据库和数据表

### 利用命令行创建、备份、删除和还原数据库和数据表

创建数据库与表部分详见 [work4.sql](./work4.sql)，在 mysql 命令行中 `\. work4.sql` 执行。

在完成 Student 数据库的创建后，使用 `mysqldump -uroot --databases Student --no-data | save -f work4_dump.sql`。

> PS: 此处使用的 Shell 环境为 nushell，根据自身情况修改命令调用。
