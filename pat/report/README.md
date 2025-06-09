# 报告导出说明

1. 修改封面文档 `cover-task[1-4].docx`，填写自己的学号和姓名
2. 完成 `task[1-4].md` 4 份报告的编写
3. 自行将 `task[1-4].md` 的 Markdown 文本转换到 PDF 格式的 `dist/task[1-4].pdf`
4. 安装依赖库 `pip install -r requirements.txt`
5. 运行 `python merge.py -u <你的学号> -n <你的名字>` 得到最终报告 `dist/<学号>-<姓名>-task[1-4].pdf`

> 关于 Markdown 转 PDF，可以尝试 pandoc、在线转换、VSCode 插件转换等等，或者直接跳过 Markdown 编写，自行选择报告编写格式导出 pdf 存放到 dist 目录下。

> 如果完成了大作业，请按照模板自行添加或另起文档编写。
