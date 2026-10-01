#include <PinChangeInt.h>    //外部中断

/////////////////////////// 本例程接线方法 ///////////////////////////
//  ArduinoUNO主板引脚 ------ AT8236带稳压模块
//        引脚5        ---        BIN1
//        引脚6        ---        BIN2
//        引脚9        ---        AIN1
//        引脚10       ---        AIN2
//        引脚A5       ---        ADC
//        引脚8        ---        E1A
//        引脚4        ---        E1B
//        引脚7        ---        E2A
//        引脚2        ---        E2B
//         GND        ---        GND


//  AT8236带稳压模块 ------ 电机编码器
//      AO1        ------  电机-
//      5V         ------  编码器5V
//      E1B        ------  A相
//      E1A        ------  B相
//      GND        ------  GND
//      AO2        ------  电机+
//
//      BO1        ------  电机-
//      5V         ------  编码器5V
//      E2B        ------  A相
//      E2A        ------  B相
//      GND        ------  GND
//      BO2        ------  电机+

/////////PWM输出引脚////////
#define BIN1 5
#define BIN2 6
#define AIN1 9
#define AIN2 10
#define Voltage A5 //模拟引脚读取电源电压

/////////编码器引脚////////
#define ENCODER_L 8  //编码器采集引脚 每路2个 共4个
#define DIRECTION_L 4
#define ENCODER_R 7
#define DIRECTION_R 2

double V; //存放电压变量
unsigned long TimeA=0;
unsigned long EncoderTime=0;
bool EncoderFlag=1;
bool Mode=0;
unsigned char LED_Count;
volatile long Velocity_L, Velocity_R ;   //左右轮编码器数据
float Velocity_Left, Velocity_Right = 0;   //左右轮速度
int delayShow=0;
unsigned long TimeB=0;
bool MotorFlag=1;
bool TimeFlag=1;

//默认的PWM值，可直接通过串口设置不同转速
int putPWM = 128;

void setup() {
  // put your setup code here, to run once:
  //PWM引脚
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  //LED引脚
  pinMode(13, OUTPUT);

  //模拟输入引脚
  pinMode(Voltage,INPUT); //初始化作为输入端

  //编码器引脚
  pinMode(ENCODER_L, INPUT);  
  pinMode(DIRECTION_L, INPUT);
  pinMode(ENCODER_R, INPUT);
  pinMode(DIRECTION_R, INPUT);

  attachInterrupt(0, READ_ENCODER_R, CHANGE);           //开启外部中断 编码器接口1
  attachPinChangeInterrupt(4, READ_ENCODER_L, CHANGE);  //开启外部中断 编码器接口2

  //PWM引脚置零保证电机不乱转 参数范围是0~255 ，255为满幅占空比
  analogWrite(AIN1, 0);
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 0);
  analogWrite(BIN2, 0);

 //初始化串口，用于输出电池电压
  Serial.begin(9600);
 
}

//设置电机A的PWM值，范围是-255 ~ 255，其中0~255对应占空比 0~100，负数表示电机反转
void Set_PWMA(int pwm)
{
  if(pwm>0)
  {
    analogWrite(AIN1, 255);
    analogWrite(AIN2, 255-pwm);
  }
  else
  {
    analogWrite(AIN2, 255);
    analogWrite(AIN1, 255+pwm);
  }
}

//设置电机B的PWM值，范围是-255 ~ 255，其中0~255对应占空比 0~100，负数表示电机反转
void Set_PWMB(int pwm)
{
  if(pwm>0)
  {
    analogWrite(BIN1, 255);
    analogWrite(BIN2, 255-pwm);
  }
  else
  {
    analogWrite(BIN2, 255);
    analogWrite(BIN1, 255+pwm);
  }
}

/*****函数功能：外部中断读取编码器数据，具有二倍频功能 注意外部中断是跳变沿触发********/
void READ_ENCODER_L() {
  if (digitalRead(ENCODER_L) == LOW) {     //如果是下降沿触发的中断
    if (digitalRead(DIRECTION_L) == LOW)      Velocity_L--;  //根据另外一相电平判定方向
    else      Velocity_L++;
  }
  else {     //如果是上升沿触发的中断
    if (digitalRead(DIRECTION_L) == LOW)      Velocity_L++; //根据另外一相电平判定方向
    else     Velocity_L--;
  }
}
/*****函数功能：外部中断读取编码器数据，具有二倍频功能 注意外部中断是跳变沿触发********/
void READ_ENCODER_R() {
  if (digitalRead(ENCODER_R) == LOW) { //如果是下降沿触发的中断
    if (digitalRead(DIRECTION_R) == LOW)      Velocity_R++;//根据另外一相电平判定方向
    else      Velocity_R--;
  }
  else {   //如果是上升沿触发的中断
    if (digitalRead(DIRECTION_R) == LOW)      Velocity_R--; //根据另外一相电平判定方向
    else     Velocity_R++;
  }
}

void loop() {

  // put your main code here, to run repeatedly:
  TimeA = millis(); //获取开机时间

  //记录时间戳
  if(EncoderFlag) EncoderFlag=0,EncoderTime = TimeA;
  if(TimeFlag) TimeFlag=0,TimeB = TimeA;

  //串口改变PWM值改变电机转速
  if(Serial.available())
  {
    static int last_getInt=0;
    int getInt = Serial.parseInt(); //解析出串口数据里的整数
    if(getInt>255) getInt=255;
    if(getInt<-255) getInt=-255;
    if(last_getInt==0 && getInt!=0)putPWM = getInt;
    if(last_getInt==0 && getInt==0)putPWM = 0;
    last_getInt=getInt;
  }

  //4秒改变一次电机旋转方向
  if(TimeA-TimeB>3999) TimeFlag=1,MotorFlag=!MotorFlag;
  if(MotorFlag)Set_PWMA(putPWM),Set_PWMB(putPWM);
  else Set_PWMA(-putPWM),Set_PWMB(-putPWM);


  //1s闪烁LED1次
  if(LED_Count==99) LED_Count=0,Mode=!Mode, digitalWrite(13,Mode);
  
  //10ms读取1次编码器数据
  if(TimeA-EncoderTime>9)
  {
   EncoderFlag=1;
   Velocity_Left = Velocity_L;    Velocity_L = 0;  //读取左轮编码器数据，并清零，这就是通过M法测速（单位时间内的脉冲数）得到速度。
   Velocity_Right = Velocity_R;    Velocity_R = 0; //读取右轮编码器数据，并清零

   Velocity_Left = (Velocity_Left/780.0f)*100*60;
   Velocity_Right = (Velocity_Right/780.0f)*100*60;

   delayShow++;
   LED_Count++;

   //50ms 显示1次数据
   if(delayShow==50)
   {
     delayShow=0;
     Serial.print("Velocity_L = ");
     Serial.print(Velocity_Left);
     Serial.println("        转/分");
  
     Serial.print("Velocity_R = ");
     Serial.print(Velocity_Right);
     Serial.println("        转/分");
     
     V=analogRead(Voltage); //读取模拟引脚A0模拟量
     Serial.print("输入电压 = ");
     Serial.print(V*0.05371);  //对模拟量转换并通过串口输出
     Serial.println("V");

     Serial.print("PWM = ");
     Serial.println(putPWM);
  
     Serial.println("");
     Serial.println("");
   }
  }
}
