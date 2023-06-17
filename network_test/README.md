## 本仓库是记录阅读linux源码后，产生的思考，并编码测试的过程
### test1目录
对于四次挥手有这样一个问题,以及bind的时候传入的地址不是本机ip的时候会怎么样\
对于第二个问题，发现错误是can not assign address\
第一个问题请看源码,主要是看四次挥手与代码是怎么对应的,::smile::四次挥手的过程都说烂了，但需要经过编码验证哦！\
对于第二个问题，我跟进源码去看
```mermaid
flowchart LR
A["bind()"];
B["inet_bind()通过socket->ops->bind调用"]
A --> B
`````
在这inet_bind()会判断addr的有效性,如下代码
```c
... inet_bind(){
...

if (!sysctl_ip_nonlocal_bind &&
!(inet->freebind || inet->transparent) &&
addr->sin_addr.s_addr != htonl(INADDR_ANY) &&
chk_addr_ret != RTN_LOCAL &&
chk_addr_ret != RTN_MULTICAST &&//多播地址
chk_addr_ret != RTN_BROADCAST)//广播地址
goto out;//这里跳转到return err,从这里看出这个if就是判断addr是不是合法的关键逻辑

...
}
`````





