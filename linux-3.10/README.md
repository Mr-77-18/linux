## 初始化部分
1. 网络子系统初始化, net/core/, cpu专属数据结构 ；两个软中断处理函数
2. 协议栈初始化, net/ipv4/, inet_protos , ptype_base
3. 驱动初始化, drivers/net/ethenet/intel/igb/ , igb_netdev_ops ；一个启动NAPI机制的igb_poll函数 ；为网卡中的每一个队列注册一个中断，函数是igb_msix_ring() 

## 发送，传输
三对函数：最终都汇集到了__sock_sendmsg()
1. send() , recv()
2. sendto() , recvfrom()
3. sendmsg() , recvmsg()

## 画一些图帮助理解
```mermaid
flowchart LR

A["socket()"]
B["sock_crete()"]
C["__sock_create()"]
D["sock_alloc()"]
E["net_families[family]->create()"]
F["inet_create()初始化socket->ops , 分配sock并初始化"]
A --> B
B --> C
C --> D
C --> E
E -- "AF_INET(./ipv4/af_inet.c)" --> F

`````

其中的难点在于有许多函数调用都是回调，这其实是为了兼容各种不同的协议。比如**net_families[family]->create()** 调用的是inet_create()【如果是AF_INET协议】,再如在inet_create()中用到的**inetsw[sock->type]** 是利用参数type来选择对应的inet_protosw。
```mermaid
flowchart LR
A[" "]
B["找到family所属的net_proto_family"]
C["根据type找到所属的inet_protosw"]
A -- "根据参数family" --> B
B -- "根据type" --> C
`````



```c
//通过参数type选择,./ipv4/af_inet.c
static struct inet_protosw inetsw_array[] =                                                                                                                            
{
{
.type =       SOCK_STREAM,
.protocol =   IPPROTO_TCP,
.prot =       &tcp_prot,
.ops =        &inet_stream_ops,
.no_check =   0,   
.flags =      INET_PROTOSW_PERMANENT |
INET_PROTOSW_ICSK,

},
...
};
`````
```c
//通过参数familly选择,./ipv4/af_inet.c
static const struct net_proto_family inet_family_ops = {
    .family = PF_INET,
    .create = inet_create,
    .owner  = THIS_MODULE,
};
````
## 迎接数据的到来
```mermaid
flowchart LR
A["GRO特性是将多个skb合成一个，减少向上请求的次数"]
B["RPS，即receive packet steering，将网络包分散到多个cpu去处理，提高性能"]
C["pcap"]
E["ptype_base"]
F["ptype_all"]

`````
我们从中断函数开始看起（拿igb网卡为例），函数是igb_msix_ring()
```mermaid
flowchart LR
A["igb_msix_ring()"]
B["napi_schedule()"]
C["__napi_schedule()"]
D["___napi_schedule()这个是真正干活的，设备加入poll_list,启动软中断"]
A --> B
B --> C
C --> D
`````

后续就是等着软中断的到来，软中断执行的是net_rx_action(),这个内核网络子模块初始化的时候注册的。net_rx_action会从per-cpu value的sofdata_net中调用igb_poll()函数，然后：\
igb_poll()-->igb_clean_rx_irq()-->napi_grp_receive()-->napi_skb_finish()-->ne  tif_receive_skb()-->__netif_receive_skb()-->__netif_receive_skb_core()-->deliver_skb()
```mermaid
flowchart LR
A["deliver_skb()"]
B["ip_ecv()交给ip协议处理"]
C["arp_rcv()交给arp协议处理"]
A --"ptype_base()" --> B
A -- "ptype_base()" --> C
`````

截至目前为止，即把数据包送到ip或者arp协议之前，有一下几个点值得注意
1. 循规蹈矩进行函数调用，一步一步接近ip和arp协议
2. 网络优化之将多个包封装成一个，减少向上请求的次数，如GRO特性
3. 网络优化之将包分散到多个cpu去处理，提高处理速度，如RPS,Ringbuffer的多队列
4. 除此之外，还有各种有利于工具使用的东西，如tcpdump抓包点，pcap逻辑

**接下来将进入ip层处理** ,好像这层有很多钩子函数哦，用于包过滤什么的

