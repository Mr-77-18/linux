# 丰富自己的武器库
角度：(编程视角)
1. 线程
2. 进程
事件：(统一事件源)
1. I/O
2. 定时器
3. 信号

---

### 线程(线程控制，环境，通信)
1. 线程的创建
2. 线程的销毁
3. 线程之间通信,同步
4. 实际案例：
	1.	线程池的实现：[查看源码](../UNIX_code/thread/thread_pool.c)

---

### 进程(进程控制，环境，通信)
1. 进程的创建

pthread_create();

2. 进程的结束

pthread_exit();

pthread_join();

3. 进程之间的通信，同步
	1. mutex(init , destroy , lock , unlock , trylock)
	2. cond(init , destroy , broadcast , signal , wait)
	3. 信号量(init , destroy , wait , post , trywait)

---

### I/O
### 定时器
定时器的实现方式：
1. socket选项的SO_RCVTIMEO和SO_SNDTIMEO
2. SIGALRM信号
3. I/O复用系统调用的超时参数

---

### 信号
1. 线程中使用信号
2. 进程中使用信号
