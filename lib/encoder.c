#include "sys.h"
#include "encoder.h"

//IO口得根据实际的硬件来改动
#define CWPORT	1
#define CWPIN	3
#define CCWPORT	1
#define CCWPIN	5

//接收编码器数据   返回值 :0未转动  1正转  2反转
u16 Encoder_recevie(void)	
{
	u8 Encoder_flag = 0;//1正转  2反转  0未转动
	u8 KEY_CW_PORT;		//接收CW的电平值
	u8 KEY_CCW_PORT;
	static u8 rEncoder_Code = 0;
	static u8 rEn_Right_Save = 1;
	static u8 rEn_Left_Save = 1;
	static u8 F_Encoder_Check=0;
	static u8 rEn_Right_Read;
	static u8 rEn_Left_Read;
	
	delay_us(250);//延时一段时间，否则无法获取数据
	if(GetPinIn(CWPORT,CWPIN))//根据实际的IO口决定
		KEY_CW_PORT = 1;
	else
		KEY_CW_PORT = 0;
	if(GetPinIn(CCWPORT,CCWPIN))//根据实际的IO口决定
		KEY_CCW_PORT = 1;
	else
		KEY_CCW_PORT = 0;
	
	if((KEY_CW_PORT == 1)&&(KEY_CCW_PORT == 1))   //   I/O端口 CW顺时针  CCW逆时针
	{
		if(F_Encoder_Check)//标志位 1转完一圈   0 未转过一圈
		{
			F_Encoder_Check = 0 ;	//清除标志位
			if(rEncoder_Code ==0x42)   //顺时针转动了 0x24为逆时针   0x42为顺时针？？？
			{
				Encoder_flag = 1;
			}
			else if(rEncoder_Code ==0x24)  //逆时针转动了
			{
				Encoder_flag = 2;
			}
		}
		rEncoder_Code = 0 ;
		rEn_Right_Save = 1 ;
		rEn_Left_Save = 1 ;
	}
	else		//转动的情况下
	{
		F_Encoder_Check = 1 ;	//转动标志位
		if(KEY_CW_PORT)		//顺时针转
		{
		  rEn_Right_Read = 1 ;
		}
		else
		{
		  rEn_Right_Read = 0 ;
		}
		
		if(KEY_CCW_PORT)		//逆时针转
		{
		  rEn_Left_Read = 1 ;
		}
		else
		{
		  rEn_Left_Read = 0 ;
		} 
		
		//读取和保存的不一致
		if((rEn_Right_Read !=rEn_Right_Save)||(rEn_Left_Read !=rEn_Left_Save)) 
		{
		   rEn_Right_Save = rEn_Right_Read ;//读取的传给保存
		   rEn_Left_Save = rEn_Left_Read ;
		   rEncoder_Code = rEn_Right_Read + rEncoder_Code ;
		   rEncoder_Code = rEncoder_Code<<1;
		   rEncoder_Code = rEn_Left_Read + rEncoder_Code ;
		   rEncoder_Code = rEncoder_Code<<1;   
		}            // rEncoder_Code ----读到编码器动作值	
	}
	
	//获取编码器的旋转情况
	if(Encoder_flag == 1)			//正转
	{
		return 1;
	}
	else if(Encoder_flag == 2)		//反转
	{
		return 2;
	}
	return 0;
}

/*旋转编码器模拟增量调节
	@unit_value	单位变化值，每次变化的幅度
	@upper_limit_value 设置的上限值
	@lower_limiting_value 设置的下限值
	@set_value 需要设置的值
通过判断旋转编码器的转动情况来设置一个值，顺时针增加，逆时针减少，
	增加的值超过上限值后，会回到下限值。同理，减少时也是。
适用场景：只存在一个固定的单位变化值，且上限和下限明确。
		目前只存在最小为0的下限值，上限值不要超过u16的范围
*/
u16 Encoder_Set_Value(u16 unit_value,u16 upper_limit_value,u16 lower_limiting_value,u16 set_value)
{
	u16 encoder_tmp = 0;			//设置时间
	encoder_tmp = Encoder_recevie();
	if(encoder_tmp == 1)
	{
		if(set_value < upper_limit_value)
		{
			set_value += unit_value;
		}
		else if(set_value >= upper_limit_value)
		{
			set_value = lower_limiting_value;
		}
	}
	else if(encoder_tmp == 2)
	{
		if(set_value > lower_limiting_value)
		{
			set_value -= unit_value;
		}
		else if(set_value <= lower_limiting_value)
		{
			set_value = upper_limit_value;
		}				
	}
	return set_value;
}