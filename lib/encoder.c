#include "sys.h"
#include "encoder.h"

//IO�ڵø���ʵ�ʵ�Ӳ�����Ķ�
#define CWPORT	1
#define CWPIN	3
#define CCWPORT	1
#define CCWPIN	5

//���ձ���������   ����ֵ :0δת��  1��ת  2��ת
u16 Encoder_recevie(void)	
{
	u8 Encoder_flag = 0;//1��ת  2��ת  0δת��
	u8 KEY_CW_PORT;		//����CW�ĵ�ƽֵ
	u8 KEY_CCW_PORT;
	static u8 rEncoder_Code = 0;
	static u8 rEn_Right_Save = 1;
	static u8 rEn_Left_Save = 1;
	static u8 F_Encoder_Check=0;
	static u8 rEn_Right_Read;
	static u8 rEn_Left_Read;
	
	delay_us(250);//��ʱһ��ʱ�䣬�����޷���ȡ����
	if(GetPinIn(CWPORT,CWPIN))//����ʵ�ʵ�IO�ھ���
		KEY_CW_PORT = 1;
	else
		KEY_CW_PORT = 0;
	if(GetPinIn(CCWPORT,CCWPIN))//����ʵ�ʵ�IO�ھ���
		KEY_CCW_PORT = 1;
	else
		KEY_CCW_PORT = 0;
	
	if((KEY_CW_PORT == 1)&&(KEY_CCW_PORT == 1))   //   I/O�˿� CW˳ʱ��  CCW��ʱ��
	{
		if(F_Encoder_Check)//��־λ 1ת��һȦ   0 δת��һȦ
		{
			F_Encoder_Check = 0 ;	//�����־λ
			if(rEncoder_Code ==0x42)   //˳ʱ��ת���� 0x24Ϊ��ʱ��   0x42Ϊ˳ʱ�룿����
			{
				Encoder_flag = 1;
			}
			else if(rEncoder_Code ==0x24)  //��ʱ��ת����
			{
				Encoder_flag = 2;
			}
		}
		rEncoder_Code = 0 ;
		rEn_Right_Save = 1 ;
		rEn_Left_Save = 1 ;
	}
	else		//ת���������
	{
		F_Encoder_Check = 1 ;	//ת����־λ
		if(KEY_CW_PORT)		//˳ʱ��ת
		{
		  rEn_Right_Read = 1 ;
		}
		else
		{
		  rEn_Right_Read = 0 ;
		}
		
		if(KEY_CCW_PORT)		//��ʱ��ת
		{
		  rEn_Left_Read = 1 ;
		}
		else
		{
		  rEn_Left_Read = 0 ;
		} 
		
		//��ȡ�ͱ���Ĳ�һ��
		if((rEn_Right_Read !=rEn_Right_Save)||(rEn_Left_Read !=rEn_Left_Save)) 
		{
		   rEn_Right_Save = rEn_Right_Read ;//��ȡ�Ĵ�������
		   rEn_Left_Save = rEn_Left_Read ;
		   rEncoder_Code = rEn_Right_Read + rEncoder_Code ;
		   rEncoder_Code = rEncoder_Code<<1;
		   rEncoder_Code = rEn_Left_Read + rEncoder_Code ;
		   rEncoder_Code = rEncoder_Code<<1;   
		}            // rEncoder_Code ----��������������ֵ	
	}
	
	//��ȡ����������ת���
	if(Encoder_flag == 1)			//��ת
	{
		return 1;
	}
	else if(Encoder_flag == 2)		//��ת
	{
		return 2;
	}
	return 0;
}

/*��ת������ģ����������
	@unit_value	��λ�仯ֵ��ÿ�α仯�ķ���
	@upper_limit_value ���õ�����ֵ
	@lower_limiting_value ���õ�����ֵ
	@set_value ��Ҫ���õ�ֵ
ͨ���ж���ת��������ת�����������һ��ֵ��˳ʱ�����ӣ���ʱ����٣�
	���ӵ�ֵ��������ֵ�󣬻�ص�����ֵ��ͬ������ʱҲ�ǡ�
���ó�����ֻ����һ���̶��ĵ�λ�仯ֵ�������޺�������ȷ��
		Ŀǰֻ������СΪ0������ֵ������ֵ��Ҫ����u16�ķ�Χ
*/
u16 Encoder_Set_Value(u16 unit_value,u16 upper_limit_value,u16 lower_limiting_value,u16 set_value)
{
	u16 encoder_tmp = 0;			//����ʱ��
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