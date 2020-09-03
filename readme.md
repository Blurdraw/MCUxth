# 简介
在51单片机上实现的多任务。
# 使用方法
## 环境配置
在keil4 中新建工程，芯片建议选择stc89c52，将main.c xth.c xth.h 文件添加进入工程。
## 使用
create_task( functionname );使传入的函数执行，与此同时，主程序也会继续执行下去。
