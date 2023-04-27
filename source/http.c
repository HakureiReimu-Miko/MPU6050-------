#include "http.h"
#include "umath.h"
#include "timer.h"
#include <string.h>
#include "const.h"
#include "ui.h"
#include "norflash.h"

#if testSearch 
//�����õ�
code u8 anaCodeStr[]={"{\"code\": 0,\"message\": \"success\",\"data\":{\"type\": 0}}"};
#endif


#define BLOCK_SIZE 512
static u16 sourceTxtLen;//���32K-8���ֽ�
static u8 nowBlock;
static u8 sourceTxtTmp[BLOCK_SIZE];
u8 sourceTxt(u16 j)
{
    u16 blockNum;
    u16 byteNum;
    
    if(j>=32760)//32K-8���ֽ�
        return '\0';
    if(j<=sourceTxtLen)
    {
        blockNum = j >> 9;
        byteNum = j & 0x01ff;
        if(blockNum != nowBlock)
        {
            nowBlock = blockNum;
            // read_dgus_vp(0xC004+(nowBlock<<8),sourceTxtTmp,sizeof(sourceTxtTmp)>>1);
        #if testSearch    
            StrCopy(sourceTxtTmp,anaCodeStr,sizeof(anaCodeStr));//����Դ���ݵ�������������
        #else 
            read_dgus_vp(0xC002+(nowBlock<<8),sourceTxtTmp,sizeof(sourceTxtTmp)>>1);
        #endif    
        }
        return sourceTxtTmp[byteNum];
    }
    else 
        return '\0';
}

u16 sundaySearch(u16 serachTxtlen, u8 *searchTxt, u16 startPos)
{
	u16 i,j,k;
    s8 nextStep[256];
    
    if(sourceTxtLen==0 || serachTxtlen==0 || sourceTxtLen<serachTxtlen) return 0xffff;

    for (i = 0; i < 256; i++)
		nextStep[i] = -1;
	for (i = 0; i < serachTxtlen; i++)
		nextStep[searchTxt[i]] = i;//��Ҫ�������ַ�����������¼��nextstep����
    // nowBlock = 0xff;//��ʼ����ǰ�飬���ڵ�һ�ε���ʱ��VP�м�������
    //��ʼ����
	for (i = startPos; i <= sourceTxtLen - serachTxtlen;)
	{
		j = i;
		k = 0;
		for (; k<serachTxtlen && j<sourceTxtLen && sourceTxt(j)==searchTxt[k]; j++,k++);//do nothing
		if (k == serachTxtlen)//great! find it!
        {
            return i;
        }
		else
		{
			if (i + serachTxtlen < sourceTxtLen)
				i += (serachTxtlen - nextStep[sourceTxt(i+serachTxtlen)]);
			else
				return 0xffff;
		}
	}
	return 0xffff;
}

u16 projectTxtLen;
/*
����:�����ַ���    
    serachTxtlen:Դ�ַ�������
    searchTxt:Դ�ַ���
    destTxt:Ŀ���ַ���(���Աȵ��ַ���)
*/
u16 projectSundaySearch(u16 serachTxtlen, u8 *searchTxt, u8 *destTxt)//�����ַ�����βλ��
{
	u16 i,j,k;
    s8 nextStep[256];
    
    if(projectTxtLen==0 || serachTxtlen==0 || projectTxtLen<serachTxtlen) return 0xffff;

    for (i = 0; i < 256; i++)
		nextStep[i] = -1;
	for (i = 0; i < serachTxtlen; i++)
		nextStep[searchTxt[i]] = i;//��Ҫ�������ַ�����������¼��nextstep����
    //��ʼ����
	for (i = 0; i <= projectTxtLen - serachTxtlen;)
	{
		j = i;
		k = 0;
		for (; k<serachTxtlen && j<projectTxtLen && destTxt[j]==searchTxt[k]; j++,k++);//do nothing
		if (k == serachTxtlen)//great! find it!
        {
            return i+serachTxtlen;
        }
		else
		{
			if (i + serachTxtlen < projectTxtLen)
				i += (serachTxtlen - nextStep[destTxt[i+serachTxtlen]]);
			else
				return 0xffff;
		}
	}
	return 0xffff;
}
//�����ַ���
code u8 publicStr0[]={"http://"};
code u8 publicStr1[]={"/baiseService/api/v1/"};
code u8 publicStr2[]={"?verifyCode="};
code u8 publicStr3[]={"&json=%7B"};
code u8 publicStr4[]={"%22"};
code u8 publicStr5[]={"%7D"};
code u8 publicStr6[]={"%22workNo%22:"};//workNo
code u8 publicStr7[]={"%22roomNo%22:"};//roomNo
code u8 publicStr8[]={"%22source%22:"};//source
code u8 publicStr9[]={"%22deviceId%22%3A"};//deviceId
code u8 publicStr10[]={"%22type%22%3A"};//type
code u8 publicStr11[]={"%22typeCode%22%3A"};//typeCode
code u8 flagStr[]={"\"flag\":"};
code u8 codeStr[]={"\"code\":"};

#define PROJECT_SEARCH_MAXLEN 1024
#define MSG_OVER_TIMRE 5000
u8 *stringCopy(u8 *destString, u8 *sourString)
{
    if(destString==NULL || sourString==NULL)
        return destString;
    while(1)
    {
        if((*sourString != '\0')&&(*sourString != 0xff))
            *destString++ = *sourString++;
        else
            break;
    }
    return destString;
}

/* u8 *stringEnd(u8 *tmp)
{
    *(u16*)&tmp[0] =  0;//param len
    *(u16*)&tmp[2] =  32*1024-8;//maxlen�����ճ���
    *(u16*)&tmp[4] =  0xc000;//state addr
    *(u16*)&tmp[6] =  0xc001;//flag addr
    *(u16*)&tmp[8] =  0xc002;//result type addr
    *(u16*)&tmp[10] =  0xc003;//result len addr
    *(u16*)&tmp[12] =  0xc004;//result addr
    *(u16*)&tmp[14] =  2;//12.11�Ż������и�
    write_dgus_vp(0xb090,tmp,8);//12.11�Ż������и�
    return (tmp+16);//12.11�Ż������и�
} */

// u8 *stringCmd(u8* tmp, u8 *cmdString)
// {
//     tmp = stringCopy(tmp,publicStr0);//  http://
//     tmp = stringCopy(tmp,sysInfo.machineIP);// ip
//     *tmp++ = ':';
//     tmp = stringCopy(tmp,sysInfo.machinePort);// port
//     tmp = stringCopy(tmp,publicStr1);//  /baiseService/api/v1/
//     tmp = stringCopy(tmp,cmdString);//  cmd
//     tmp = stringCopy(tmp,publicStr2);//  ?verifyCode=
//     tmp = stringCopy(tmp,sysInfo.machineVerifyCode);//  Ĭ��000000
//     return tmp;
// }


/* //�����Ӧ�����ƺͲ���
u8 *stringStrName_StrParameter(u8* tmp,u8 *StrName, u8 *StrParameter)
{
    tmp = stringCopy(tmp,StrName);//  %22workNo%22:
    tmp = stringCopy(tmp,publicStr4);//  %22
    tmp = stringCopy(tmp,StrParameter);//  8828
    tmp = stringCopy(tmp,publicStr4);//  %22
    return tmp;
}

//�����Ӧ�����ƺͲ������ŵ��ַ���(�����������ַ���)
u8 *stringStrName_StrNumber(u8* tmp,u8 *StrName, u8 *StrNumber)
{
    tmp = stringCopy(tmp,StrName);//  %22workNo%22:
    tmp = stringCopy(tmp,StrNumber);//  2
    return tmp;
}
 */


/* //��������CharAscii��صĲ���,�ַ��� "abc"
void Func_projectSundaySearch_getCharAscii(u8 *paraStr,u8 *tmpProject,u8 *tmp,u8 tmpValue)
{
    s16 pos;
    u8 strTmp[32];

    StrClear(strTmp,sizeof(strTmp));
    stringCopy(strTmp,paraStr);

    pos = projectSundaySearch(sizeArray(strTmp),strTmp,tmpProject);
    if(0xffff != pos)
    {
        getCharAscii(&tmpProject[pos],&tmp[tmpValue]);
    }
}
//��������CharValueASCII��صĲ���,�����ַ��� "123"
void Func_projectSundaySearch_getCharValueASCII(u8 *paraStr,u8 *tmpProject,u8 *tmp,u8 tmpValue)
{
    s16 pos;
    u8 strTmp[32];

    StrClear(strTmp,sizeof(strTmp));
    stringCopy(strTmp,paraStr);

    pos = projectSundaySearch(sizeArray(strTmp),strTmp,tmpProject);
    if(0xffff != pos)
    {
        getCharValueASCII(&tmpProject[pos],&tmp[tmpValue]);
    }
}
 */
code u8 messageStr[]={"\"message\":"};//message

/*
//ʵ�ʽ������յ�����
void anaWS(u8 *tmpProject,u16 *i,u8 *returnTmp)
{
    s16 pos;
    u8 tmp8[16]={0};
    u32 idTmp;

    //��ȡws����״̬,������Ȩ��
    pos = projectSundaySearch(sizeof(need_update_apikeyStr)-1,need_update_apikeyStr,tmpProject);//"need_update_apikeyStr": 0, 0������ 1��Ҫ����
    if(0xffff != pos)
    {
        idTmp=0;
        getCharValue2(&tmpProject[pos],(u32*)&idTmp);
        if(idTmp)//��Ҫ����api
        {
            pos = projectSundaySearch(sizeof(new_apikeyStr)-1,new_apikeyStr,tmpProject);
            if(0xffff != pos)
            {
                getCharAscii(&tmpProject[pos],(u8*)&sysInfo.apikey);
                //���籣��
                Nor_Flash_write(NorflashAddrSysInfo,(u8*)&sysInfo,sizeof(SYSINFO)/2);
            }
        }
    }
    
    //��ʼ���豸��Ϣ
    pos = projectSundaySearch(sizeof(typeStr)-1,typeStr,tmpProject);//"type": 0,  �豸���͡�0����1Ⱥ��
    if(0xffff != pos)
    {
        wsData.initActionFlag=1;//�����豸��ʼ�������
        getCharValue(&tmpProject[pos],(u16*)&wsData.type);
    }

    //��״̬���������false����δ���û���չʾ��ά�룬��ά������Ϊ�̶�url+device_idƴ�ӡ�
    pos = projectSundaySearch(sizeof(bind_statusStr)-1,bind_statusStr,tmpProject);//��״̬
    if(0xffff != pos)
    {
        // memset(tmp8,0,sizeof(tmp8));
        getCharAscii(&tmpProject[pos],&tmp8[0]);
        write_dgus_vp(0x1F5B,tmp8,2);
        
        if(0==mStrCmp(trueStr,tmp8))
        {
            wsData.bind_status=1;
        }
        else if(0==mStrCmp(falseStr,tmp8))
        {
            wsData.bind_status=0;
        }
        else 
        {
            wsData.bind_status=0;
        }
    }

    //�����б�
    pos = projectSundaySearch(sizeof(idStr)-1,idStr,tmpProject);//"idStr": 1,  ����id
    if(0xffff != pos)
    {
        idTmp=0;
        getCharValue2(&tmpProject[pos],(u32*)&idTmp);
        if(idTmp)
        {
            getCharValueASCII(&tmpProject[pos],(u8*)&wsData.childPara[*i].idcode);//��ȡid�����ַ���

            wsData.childPara[*i].id=idTmp;
            pos = projectSundaySearch(sizeof(nameStr)-1,nameStr,tmpProject);//��������
            if(0xffff != pos)
            {
                getCharAscii(&tmpProject[pos],(u8*)&wsData.childPara[*i].name[0]);
            }
            //��ʾ��Ϣ���͡���0����ʾ��1��2��3��4�ֱ������뵽�ڻ���7�졢��������7��֮�ڡ���������7�졢�������ʧ�ܣ�
            pos = projectSundaySearch(sizeof(tip_typeStr)-1,tip_typeStr,tmpProject);//��ʾ��Ϣ
            if(0xffff != pos)
            {
                getCharValue(&tmpProject[pos],(u16*)&wsData.childPara[*i].tip_type);
            }
            //tip_typeΪ1��2��3ʱ��ʾ��Ϣ�е�������
            pos = projectSundaySearch(sizeof(diff_daysStr)-1,diff_daysStr,tmpProject);//����
            if(0xffff != pos)
            {
                getCharValue(&tmpProject[pos],(u16*)&wsData.childPara[*i].diff_days);
            }
            (*i)++;
            if((*i)>=100)
            (*returnTmp)=0xFF;//��������while()��Ҫ�˳��� 
        }
    }
}
*/


//��������
void AnalyticFunction(void)
{
    u8 tmp[64];
	u16 i,j;
    s16 pos,pos1,pos2;
    u16 codeTmp=0;
    u8 projectStart,projectEnd;
    u8 tmpProject[PROJECT_SEARCH_MAXLEN];
    //0x6aa6(2) +len(2)+message
#if testSearch    
    if(1)
    {
        sourceTxtLen = sizeof(anaCodeStr)-1;//����
#else 
    read_dgus_vp(0xc000,tmp,4);
    if(tmp[0]==0x6a && tmp[1]==0xa6)//֡ͷ
    {
        //testDisplayNum(0x1f57,1);
        tmp[0] = 0;
        tmp[1] = 0;
        write_dgus_vp(0xc000,tmp,1);
        sourceTxtLen = *(u16*)&tmp[2];//����
#endif
        nowBlock = 0xff;//��ʼ����ǰ�������ڿ飬���ڵ�һ��������ѯ����
       
        pos = sundaySearch(sizeof(codeStr)-1,codeStr,1);//��һ������,startPos���ó�1,��ͷ��ʼ����
        if(pos != 0xffff)
        {
            getCharValue(&sourceTxtTmp[pos+7],&codeTmp);//��ȡ"code":0 ��ֵ
            if(1==codeTmp)//"code": 1,ʧ����
            {
                pos = sundaySearch(sizeof(messageStr)-1,messageStr,pos);
                
            }
            else //"code": 0
            { 
                i = 0;
                projectStart = '{';
                projectEnd = '}';
                while(1)
                {
                    pos1 = sundaySearch(1,&projectStart,pos);//Ѱ����ʼ��{
                    if(pos1!=0xffff)
                    {
                        pos2 = sundaySearch(1,&projectEnd,pos1);//Ѱ��}
                        if(pos2!=0xffff)
                        {
                            projectTxtLen = pos2-pos1;//���㳤��
                            if(projectTxtLen>1 && projectTxtLen<PROJECT_SEARCH_MAXLEN)//û�����ݣ��������������󣬶�������
                            {
                                for(j=pos1;j<=pos2;j++)//20211208��:j<=pos2,������'}'Ҳ��ȡ��ȥ,��ֹ�Դ������ַ�����β���Ҳ����������
                                {
                                    tmpProject[j-pos1] = sourceTxt(j);//��ȡ���ݵ�RAM����
                                }
                                
                                //�˴���ʼ����
                                //anaWS(&tmpProject,&i,&returnTmp);
//                                
//                                if(returnTmp>=0xFF)//������Ҫ�˳��˴�whileѭ��
//                                    break;
                            }
                            pos = pos2;//����һ����β��Ϊ��һ����ͷ
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
}


