### fork , exit分析
阅读fork代码的着眼点：mm_struct

fork()
```c
SYSCALL_DEFINE0(fork){
	return do_fork(SIGCHLD , 0 , 0 , NULL , NULL);
}
`````
对于do_fork，这里需要说明一些第一个参数的作用:第一个参数是一个标志，分为两部分：
1. 进程结束的时候想父进程发送的信号是什么
2. fork的标志，比如CLONE_VM , CLONE_VFORK等等

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

