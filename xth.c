#include <REGX52.H>
#define TIMESLICE 250
#define STACKSIZE 18
#define TASKSIZE 3
#define RUNSTATE 0
#define READYSTATE 1
#define WAITSTATE 2

typedef unsigned int u16;
typedef unsigned char u8;
void task_change(u8);
void main_app();

typedef struct 
{
	u8 state;
	u8 sp;      //stack里最后一个元素的下标
	u8 stack[STACKSIZE];
}TASK_INFO;

struct OS_INFO
{
	u8 total;
	u8 current;
	TASK_INFO tasks[TASKSIZE];
} os_info;


/**************************栈函数***********************************/
/*根据taskid返回此task的栈顶绝对地址*/
u8 get_sp(u8 taskid)
{
	return os_info.tasks[taskid].sp + os_info.tasks[taskid].stack;
}

/*根据taskid返回此task的栈顶相对地址*/
u8 get_rsp(u8 taskid)
{
	return os_info.tasks[taskid].sp;
}
/*将sp保存给当前的task，并保存为相对地址*/
void save_rsp(u8 sp)
{
	os_info.tasks[os_info.current].sp = sp - (u8)os_info.tasks[os_info.current].stack;
}

void sleep()
{
	
}

void delay(unsigned int c)
{
	unsigned char a,b;
	while(c--)
	{
		a=2;
		b=239;
		do
		{
		  while(--b);
		}while(--a);
	}
}

void sleep_task()
{
	while(1);
}

void timer_init(ms)
{
	TMOD = 0x01;
	TF0 = 0;  
	TR0 = 1; //start
	TL0 = (65536 - ms * 1000) % 256;
	TH0 = (65536 - ms * 1000) / 256;
	ET0 = 1;
	EA = 1;  //enable
}
//中断程序会自动保存状态
void timer_routine () interrupt 1
{
	EA = 0;
	P1_0 = ~P1_0;
	save_rsp(SP);
	os_info.tasks[os_info.current].state = READYSTATE;
	if(os_info.current == TASKSIZE - 2)   //减去二是因为最后一个task为空闲task
		os_info.current = 0;
	else
		os_info.current ++;
	os_info.tasks[os_info.current].state = RUNSTATE;
	SP = get_sp(os_info.current);
	if(get_rsp(os_info.current)<=2)
	{
		__asm PUSH 	ACC
		__asm PUSH 	B
		__asm PUSH 	DPH
		__asm PUSH 	DPL
		__asm PUSH 	PSW
		__asm PUSH 	AR0
		__asm PUSH 	AR1
		__asm PUSH 	AR2
		__asm PUSH 	AR3
		__asm PUSH 	AR4
		__asm PUSH 	AR5
		__asm PUSH 	AR6
		__asm PUSH 	AR7
	}
	TL0 = (65535 - TIMESLICE * 1000) % 256;
	TH0 = (65535 - TIMESLICE * 1000) / 256;
	EA = 1;
	__asm
}

void tasks_init()
{
	TASK_INFO idata newtask;
  newtask.stack[0] = (u16)sleep_task & 0xFF;
	newtask.stack[1] = ((u16)sleep_task >> 8 );
	newtask.sp = 1;
	newtask.state = READYSTATE;
	os_info.tasks[TASKSIZE -1] = newtask;
}
void create_task(void (*func)())
{
	EA = 0;
	if(os_info.total < TASKSIZE -1)
	{
		TASK_INFO idata newtask;
		newtask.stack[0] = (u16)func & 0xFF;
		newtask.stack[1] = ((u16)func >> 8 );
		newtask.sp = 1;
		newtask.state = READYSTATE;
		os_info.tasks[os_info.total] = newtask;
		os_info.total = os_info.total+1;
	}
	EA = 1;
}

void task_start(u8 taskid)
{
	//_timer_init(200);
	os_info.current = taskid;
	os_info.tasks[taskid].state = RUNSTATE;
	SP = get_sp(os_info.current);        //此函数退出后，cpu会自动取出栈里的函数首地址，相当于开始了一个新任务。
}
void task_change(u8 task_id)
{
	//注意，进入此函数时，SP已经保存了PC，占2字节
	//现在把其他重要的的寄存器信息入栈
	__asm PUSH ACC
	__asm PUSH B
	__asm PUSH DPH
	__asm PUSH DPL
  __asm PUSH PSW
  __asm PUSH AR0
  __asm PUSH AR1
  __asm PUSH AR2
  __asm PUSH AR3
	__asm PUSH AR4
	__asm PUSH AR5
	__asm PUSH AR6
	__asm PUSH AR7
	
	save_rsp(SP);
	os_info.current = task_id;
	SP = get_sp(task_id); 
	if(get_rsp(task_id) > 1)   //任务刚被创建时，只占了两个字节用来存函数首地址，除非是已经保存过状态，否则无需恢复。
	{
	  __asm POP AR7
		__asm POP AR6
		__asm POP AR5
		__asm POP AR4
		__asm POP AR3
		__asm POP AR2
		__asm POP AR1
		__asm POP AR0
		__asm POP PSW
		__asm POP DPL
		__asm POP DPH
		__asm POP B
		__asm POP ACC
	}
}

void main()
{
	os_info.total = 0;
	tasks_init();
	create_task(main_app);
	timer_init(20);
	task_start(0);
	while(1);
}

