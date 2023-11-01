### fork , exit中mm_struct的分析
阅读fork代码的着眼点：mm_struct

fork()
```c
SYSCALL_DEFINE0(fork){
	return do_fork(SIGCHLD , 0 , 0 , NULL , NULL);
}
`````
对于do_fork，这里需要说明一些第一个参数的作用:第一个参数是一个标志，分为两部分：
1. 进程结束的时候想父进程发送的信号是什么
2. fork的标志，比如CLONE_VM[clone的时候使用CLONE_VM的简单示例](../UNIX_code/process/clone_vm.c) , CLONE_VFORK等等

fork()->do_fork()
```c
long do_fork(...){
	...
	copy_process(clone_flags , stack_start , stack_size);
	...
}
`````
fork()->do_fork()->copy_process()
```c
static struct task_struct *copy_process(unsigned long clone_flags , ...){
	...
	struct task_struct* p;
	p = dup_task_struct(current);
	p->xxx = ;
	p->xxx = ;
	...

	copy_files(...);
	copy_fs(...);
	copy_sighand(...);
	...
	copy_mm(clone_flags , p);
	...
}
`````

copy_process主要的作用就是拷贝task_struct的内容了，主要分成两部分，一个是通过p->xxx这种方式直接初始化的，其次是形如copy_xxx()形式初始化的。我们着重关注copy_mm();

fork()->do_fork()->copy_process()->copy_mm()
```c
static int copy_mm(unsigned long clone_flags , struct task_struct *tsk){
	...
	dup_mm(tsk);
	...
}
`````
这个函数对新的task_struct里面关于mm的内容进行了一些初始化，然后调用dup_mm()进行更深层次的初始化，比如VAM等

fork()->do_fork()->copy_process()->copy_mm()->dup_mm()
```c
struct mm_struct* dup_mm(struct task_struct* tsk){
	...
	mm = allocate_mm();//分配mm_struct结构体
	...
	memcpy(mm , oldmm , sizeof(mm));//完全照搬父进程的mm_struct
	...
	dup_mmap(mm , oldmm);//进一步复制
	...

}
`````

这个函数的作用是申请一块mm_struct，然后完全拷贝父进程的mm_struct结构体内容，然后再使用dup_mmap()进一步复制。

fork()->do_fork()->copy_process()->copy_mm()->dup_mm()->dup_mmap(mm , oldmm);
```c
static int dup_mmap(struct mm_struct* mm , struct mm_struct* oldmm){
	for(){
		//循环复制vam
		...
		copy_page_range(mm , oldmm , mpnt);//这里用到了copy-on-write技术,涉及到虚存空间的VM_MAYWRITE,VM_SHARED属性
		...
	}
	if(){
	//争对文件映射页有进行特殊处理
	}
}
`````
这个函数就是复制虚存空间，并且在虚存空间具有VM_MAYWRITE属性时采用copy-on-write技术

至此大概看清楚了关于mm_struct在fork的过程中是怎么被处理的

```mermaid
flowchart LR
A["fork()"];
B["do_fork()"];
C["copy_process()"];
D["copy_mm()"];
E["dup_mm()"];
F["dup_mmap"];
A --> B
B --> C
C --> D
D --> E
E --> F
`````


---
阅读exit代码的着眼点：mm_struct；

入口如下：
```c
SYSCALL_DEFINE1(exit , int, error_code)
{
	do_exit((error_code&0xff)<<8);
}
`````
exit()->do_exit()
```c
void do_exit(long code){
	...
	exit_mm(tsk);//tsk指的是task_struct结构体
	exit_sem(tsk);
	exit_files(tsk);
	exit_fs(tsk);
	...
}
`````

可以看到这里的exit_xxx跟copy_process()里面的copy_xxx很像，其实大部分就是对应的。比如copy_files()对应的释放就是exit_files()

exit()->do_exit()->exit_mm()

```c
static void exit_mm(){
	...
	mm_release();
	...
	mmput(mm);//这个mm是指mm_struct结构体
}
`````

这里要说明一下mmput()才是释放mm_struct的操作,mm_release需要说明的是，我们知道采用vfork的时候，父进程是阻塞的，直到子进程退出或者调用了execv()等，所以这里会涉及到一个唤醒操作，请看下面mm_release()函数的最后一行：
```c
void mm_release(){
	...
	if(tsk->vfork_done)
		complete_vfork_done(tsk);
}
`````

这个complete_vfork_done函数里面就有唤醒父进程的操作。

到此，也基本说明了释放mm_struct的函数调用路径

```mermaid
flowchart LR
A["exit()"];
B["do_exit()"];
C["exit_mm()"];
A --> B
B --> C
`````


