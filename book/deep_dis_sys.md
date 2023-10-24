### 这本书的大体框架如下：

what?(分布式系统是什么)->分布式系统长什么样->数据->共识->事务->...

## 第一章 认识分布式系统

## 第二章 分布式系统模型
这章主要从不同的角度对分布式系统进行分类。<font color=red>系统模型是设计系统架构的基础</font>

不同角度下的分类
1. 网络通信链路
	1. <font color=red>可靠链路</font>
	2. 公平损失链路
	3. 任意链路
2. 节点故障类型
	1. <font color=red>崩溃-停止</font>
	2. <font color=red>崩溃-恢复</font>
	3. 拜占庭故障
3. 时间或者同步类型
	1. 同步系统
	2. <font color=red>部分同步系统</font>
	3. 异步系统

**红色部分表示的是本书讨论的分布式系统模型** 

## 第三章 分布式数据基础

主要讲了关于数据中的两种技术：
1. 分区
2. 复制

以及一致性模型和几个定理（CAP等）

---

#### 分区
分区这里主要讲了水平分区的情况里面的分区算法（怎么划分数据）和一些挑战
1. 分区算法：
	1. 哈希
	2. 范围分区
2. 挑战：
	1. 不管是哪一个分区（水平，垂直）都会在一些特定的情况下出现效率低下的情况。
	2. 实时事务

#### 复制(复制会带来一致性问题，可以使用恰当的共识算法来解决)

<font color=red>我的一些思考：数据的备份有两个好处：一是可以将业务分流；二是可以有一定的容错，即使一台down掉了，另外一台也可以继续工作</font>

复制这一块主要讲解了3种复制<font color=red>方法</font>

1. 单主复制:这种方式的优点就是数据不容易产生冲突；缺点就是对于写请求有一定的瓶颈

<p align="center">
<img src="https://raw.githubusercontent.com/Mr-77-18/linux/main/image/1.png">
</p>

2. 多主复制:这种方式的优点是写请求的瓶颈解决了，但是冲突容易产生。

3. 无主复制:(从多个节点读，向多个节点写)这种方式的话很奇妙。它可以使用Quorum的数据冗余机制(这个机制其实就是告诉我们要像几个节点读，要向几个节点写)达到数据的不冲突

#### 线性一致性
概念：如果一个并发操作的历史（H）能够转换成一个具有线性一致性的顺序历史，那么我们称这个并发的历史是线性一致的。

用c语言实现简单的线性一致性

一致性和隔离级别的对比：

该小节中有一段文字比较晦涩：

对于最严格的一致性模型和隔离级别--线性一致性和串行化，还有一个重要的区别是，线性一致性提供了实时保证，而串行化则没有。这意味着线性一致性保证操作在客户端调用和客户端收到响应之间的某个时刻生效，而串行化只保证多个并发事务的效果，以及它们以串行的顺序执行，至于串行的顺序是否与实时的顺序一样，它并没有保证。

这个串行化是指服务端只给你的各种操作保证了原子隔离，但是这些操作的先后顺序没有给你保证（因为cpu并没有保证指令执行的顺序性）。比如你有一堆操作（1 ， 2 ， 3 ， 4），这些操作在没有串行化的时候可能会出现并行执行从而出现脏读等异常情况，但是加上串行化之后，可以保证这4个操作都是串行的，但是顺序没有办法给你保证，有可能是（2 ， 1 ， 3 ， 4），也有可能是（2 ， 1 ， 4 ， 3）等等

## 第四章：分布式共识
实现raft共识算法

整体逻辑应该是什么样的？
```c
//首先明确有两个RPC：
//1. RequestVote RPC
//2. AppendEntries RPC;

//由Candidate->others
Ret RequestVote(Args){
	...	
}

//由Leader->others
Ret AppendEntries(Args){
	...
}

`````

考虑这两个RPC的出发时机
1. RequestVote RPC
	1. 当在Leader状态的时候以一个固定的时间发送心跳

2. AppendEntries RPc
	1. 当在Candidate状态的时候以一个固定的时间发送


**这里引出状态变化的内容** 
1. 对于leader:
	1. leader->follower，<font color=red>触发时机:recive a RPC with high term</font>

2. 对于follower:
	1. follower->Candidate,<font color=red>触发时机：after a random time out</font><++>

3. 对于Candidate
	1. Candidate->Leader,<font color=red>触发时机：receives votes from majority of server</font>
	2. Candidate->follower,<font color=red>触发时机：discovers current leader or new term</font>
	3. Candidate->Candidate,<font color=red>触发时机：time out , new election(term++)</font>

现在我们站在一个进程的角度思考代码如何编写：

从以上分析可以看出，这个进程包含着两部分动作（不管是处于哪一种状态）
1. 主动动作: 发送RPC
2. 被动动作: time out之后的动作 ; 接收RPC调用

好，那么这个进程需要有一些定时器，用来特定执行某些代码,其次就是主流程里面的发送RPC或者接收RPC了

定时器的设计可以参考《Linux高性能服务器编程》的第11章,Linux提供三种方法实现定时任务：
1. socket选项的SO_RCVTIMEO和SO_SNDTIMEO
2. SIGALRM信号
3. I/O复用系统调用的超时参数

这里给出使用第一种方法实现的简单定时器实现：
```c
 14 //定义状态
 15 #define Leader 0
 16 #define Candidate 1
 17 #define Follower 2
 18
 19 //刚开始的时候都是Fllower;
 20 int states = Follower;
 21
 22 //const char* ip, int  port, int time
 23 struct Argc_To_Thread{
 24   char* ip;
 25   int port;
 26   int time;
 27 };
 28
 29 void* time_thread(void* arg)
 30 {
 31   //处理参数：
 32   struct Argc_To_Thread* p_argc_to_thread = (struct Argc_To_Thread*)arg;
 33
 34   char* ip = p_argc_to_thread->ip;
 35   int port = p_argc_to_thread->port;
 36   int time = p_argc_to_thread->time;
 37
 38
 39   int ret = 0;
 40   struct sockaddr_in address;
 41   bzero(&address , sizeof(address));
 42
 43   //设置地址
 44   address.sin_family = AF_INET;
 46   address.sin_port = htons(port);
 47		inet_pton(AF_INET , ip , &address.sin_addr);
 48   int sockfd = socket(PF_INET , SOCK_STREAM , 0);
 49   assert(sockfd >= 0);
 50
 51   //设置超时时间
 52   struct timeval timeout;
 53   timeout.tv_sec = time;
 54   timeout.tv_usec = 0;
 55   socklen_t len = sizeof(timeout);
 56   ret = setsockopt(sockfd , SOL_SOCKET , SO_SNDTIMEO , &timeout , len);
 57   assert(ret != -1);
 58
 59   while(1){
 60     //启动连接
 61     ret = connect(sockfd , (struct sockaddr*)&address , sizeof(address));
 62
 63     if (ret == -1) {
 64       if (errno == EINPROGRESS) {
 65         printf("connecting timeout , process timeout logic\n");
 66         switch (states) {
 67           case Leader:
 68             break;
 69           case Candidate:
 70             break;
 71           case Follower:
 72             states = Candidate;
 73             break;
 74           default:
 75             break;
 76         }
 77       }
 78       printf("error occur when connecting to server\n");
 79     }
 80   }
 81   pthread_exit(0);
 82 }
 83
 84 //主线程
 85 int main(int argc, char *argv[])
 86 {
 87   pthread_t thid;
 88   if (argc <= 2) {
 89     printf("usage: %s ip_address port_number\n" , argv[0]);
 90     return 1;
 91   }
 92
 93   //准备线程参数
 94   struct Argc_To_Thread argc_to_thread;
 95   argc_to_thread.ip = argv[1];
 96   argc_to_thread.port = atoi(argv[2]);
 97   argc_to_thread.time = 1;
 98
 99   pthread_create(&thid , NULL , time_thread , &argc_to_thread);
100
101
102   while(1){
103     switch (states) {
104       case Leader:
105         printf("in leader states\n");
106         break;
107       case Candidate:
108         printf("in candidate states\n");
109         break;
110       case Follower:
111         printf("in Fllower states\n");
112         break;
113       default:
114         break;
115     }
116   }
117
118   pthread_join(thid , NULL);
119
120   return 0;
121 }态转变，状态转变之后会有不同的动作。
`````
在Raft这个例子当中，这个time out是需要随机生成的，触发的动作是状态转变，状态转变之后会有不同的动作。

UNIX_code里的raft_1.c模拟了一个节点的心跳发送和超时后进入Candidate状态,随后的raft_2.c在1的基础之上实现真正的rpc调用，这里又设计到C语言里面的rpc调用。[请点击跳转](../UNIX_code/time/c_rpc.md)
