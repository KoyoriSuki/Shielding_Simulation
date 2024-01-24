# 声明作业类型
Universe = vanilla

# 不发送通知
Notification = Never

# 复制⽤户环境配置
GetEnv = True

# 降低当前作业优先级
nice_user = True

# 提交⼀个作业后停顿 
next_job_start_delay = 3

# 作业执⾏⽂件
Executable = ./DBDecay

# 传⼊参数，这⾥是参数文件名
Arguments = na22.mac $(Process)

# 标准输出
Output = ./printfile/$(Process).out

# 错误输出
Error = ./printfile/$(Process).err

# 日志文件
Log = ./printfile/$(Process).log

# 提交作业数量
Queue 100


