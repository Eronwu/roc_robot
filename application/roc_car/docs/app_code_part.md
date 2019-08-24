# 小车应用编写篇
-------------
### 代码目录

application\roc_car\applications



### 代码架构

roc_car

│
├─docs
│      *.md							// 文档介绍
│
├─applications
│      main.c						// 主入口
│      main.h
│      protocol.h     			// 协议头文件
│      protocol_parser.c	// 协议解析文件
│	 protocol_parser.h
│	 roc_robot.c				// 小车控制函数文件
│	 roc_robot.h
│	 tcprecv.c					// tcp 接受数据函数
│	 wifi_connect.c
│	 
├─ports
└─SConscript                // RT-Thread 默认的构建脚本



### 协议部分

	typedef struct
	{
	    rt_uint8_t start_code ;   // 8bit 0xAA
	    rt_uint8_t len;           // protocol package data length
	    E_ROBOT_TYPE type;
	    rt_uint8_t addr;
	    E_CONTOROL_FUNC function;      // 8 bit
	    rt_uint8_t *data;         // n bit
	    rt_uint16_t sum;       // check sum 16bit
	    rt_uint8_t end_code;      // 8bit 0x55
	} ST_PROTOCOL; // wifi数据字节流结构体


​	typedef enum
​	{
​	    E_BATTERY = 1,
​	    E_LED,
​	    E_BUZZER,
​	    E_INFO,
​	    E_ROBOT_CONTROL_DIRECTION,
​	    E_ROBOT_CONTROL_SPEED,
​	    E_TEMPERATURE,
​	    E_INFRARED_TRACKING,
​	    E_ULTRASONIC,
​	    E_INFRARED_REMOTE,
​	    E_INFRARED_AVOIDANCE,
​	    E_CONTROL_MODE,  //12
​	    E_BUTTON,
​	    E_LED_MAXTRIX,
​	    E_CMD_LINE,
​	    E_VERSION,
​	    E_UPGRADE,
​	    E_PHOTORESISTOR,
​	    E_SERVER_DEGREE,
​	    E_CONTOROL_CODE_MAX,
​	} E_CONTOROL_FUNC; // wifi控制指令功能部分



### 代码流程

```flow
st=>start: main.c
op1=>operation: Wifi connect|past
op2=>operation: rt-robot init|current
op3=>operation: wifi led|current
io=>inputoutput: data receive|request
op4=>operation: execute distruction|current
sub1=>subroutine: My Subroutine|invalid
c2=>condition: Good idea|rejected

st->op1->op2->io->op4->io
sub1(left)->op1

```



main.c:

wifi_auto_connect(); // 自动链接wifi热点函数

roc_robot_init(0); // 使用RT-Robot框架初始化RT-Robot驱动, 函数前进后退方法都在roc_robot.c里面

rt_thread_create("led_flash", led_flash... // 判断wifi连接情况起的线程

ret = rt_thread_create("wifi_control", wifi_control... // 将接受的wifi数据转化为指令执行

tcprecvserv((void *)pareser_package.buffer); // 接受wifi数据到data下



### 手机wifi遥控器代码

见[手机wifi遥控器代码](mobile_wifi_code_part.md)