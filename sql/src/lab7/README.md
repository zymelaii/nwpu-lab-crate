# 关于任务二的说明

操作前请先依次执行 `db.sql` 与 `impl.sql`

设置隔离级别 `set session transaction isolation level read uncommitted;`

以下凡涉及在两个连接中调用过程的地方，均需保证调用间隔在 sleep 时间内，详见 [impl.sql](./impl.sql)

## 修改丢失

两个连接中分别调用过程 `read_with_lazy_write`

## 读脏数据

两个连接中先后调用 `write_with_lazy_rollback` 与 `read_before_lazy_rollback`

## 不可重复读

两个连接中先后调用 `read_with_lazy_read` 与 `update_during_lazy_read`

## 幻读

两个连接中先后调用 `lazy_read_rows_twice` 与 `insert_during_read_rows`

# 关于任务八的说明

使用 `flush logs;` 刷新日志并建立新日志，当前日志信息可通过 `show master status;` 查看。

使用 `mysqlbinlog <log-file> -d db1` 查看日志文件，其中 `<log-file>` 为日志文件的路径。

定位待恢复的 SQL 语句 `drop table t1;`，记录对应的 `# at <pos>` 值。

使用 `mysqlbinlog <log-file> -d db1 --skip-gtids --stop-position <pos>` 打印选定的记录点区间内的 SQL 记录，将输出保存为 SQL 文件，在 mysql 中执行即可恢复。

> 若 mysqlbinlog 报错 ` [ERROR] unknown variable ...` 可以添加 `--no-defaults` 选项
