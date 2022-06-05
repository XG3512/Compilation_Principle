#include<string.h>
#include <iostream>

using namespace std;
char Project[80];
char Token[8];
char KeyChar;
int sum=0;
int row;
int n;
int p;
int m = 0;
int syn;
//����ؼ���
char *ListOfCodeKeywords[6]={"begin", "if", "then", "while", "do", "end"};

//�ʷ�����������Ҫ����������Ҫ��Ϊ�������֣���ʶ�������֡�����
void SourceScan()
{
    for(n=0;n<8;n++) Token[n]=NULL;
    KeyChar=Project[p++];
    while(KeyChar == ' ')
    {
        KeyChar=Project[p];
        p++;
    }
    //��������Ǳ�ʾ�����߱�����
    if((KeyChar >= 'a' && KeyChar <= 'z') || (KeyChar >= 'A' && KeyChar <= 'Z'))
    {
        m=0;
        while((KeyChar >= '0' && KeyChar <= '9') || (KeyChar >= 'a' && KeyChar <= 'z') || (KeyChar >= 'A' && KeyChar <= 'Z'))
        {
            Token[m++]=KeyChar;
            KeyChar=Project[p++];
        }
        Token[m++]='\0';
        p--;
        syn=10;
        for(n=0;n<6;n++)  //��ʶ��������ַ����Ѷ���ı�ʾ�����Ƚϣ�
            if(strcmp(Token, ListOfCodeKeywords[n]) == 0)
            {
                syn=n+1;
                break;
            }
    }
    else if((KeyChar >= '0' && KeyChar <= '9'))  //����
    {
        {
            sum=0;
            while((KeyChar >= '0' && KeyChar <= '9'))
            {
                sum=sum*10 + KeyChar - '0';
                KeyChar=Project[p++];
            }
        }
        p--;
        syn=11;
        if(sum>32767)
            syn=-1;
    }
    else switch(KeyChar)   //�����ַ�
        {
            case'<':m=0;Token[m++]=KeyChar;
                KeyChar=Project[p++];
                if(KeyChar == '>')
                {
                    syn=21;
                    Token[m++]=KeyChar;
                }
                else if(KeyChar == '=')
                {
                    syn=22;
                    Token[m++]=KeyChar;
                }
                else
                {
                    syn=23;
                    p--;
                }
                break;
            case'>':m=0;Token[m++]=KeyChar;
                KeyChar=Project[p++];
                if(KeyChar == '=')
                {
                    syn=24;
                    Token[m++]=KeyChar;
                }
                else
                {
                    syn=20;
                    p--;
                }
                break;
            case':':m=0;Token[m++]=KeyChar;
                KeyChar=Project[p++];
                if(KeyChar == '=')
                {
                    syn=18;
                    Token[m++]=KeyChar;
                }
                else
                {
                    syn=17;
                    p--;
                }
                break;
            case'*':syn=13;Token[0]=KeyChar;break;
            case'/':syn=14;Token[0]=KeyChar;break;
            case'+':syn=15;Token[0]=KeyChar;break;
            case'-':syn=16;Token[0]=KeyChar;break;
            case'=':syn=25;Token[0]=KeyChar;break;
            case';':syn=26;Token[0]=KeyChar;break;
            case'(':syn=27;Token[0]=KeyChar;break;
            case')':syn=28;Token[0]=KeyChar;break;
            case'#':syn=0;Token[0]=KeyChar;break;
            case'\n':syn=-2;break;
            default: syn=-1;break;
        }
}

int main()
{
    p=0;
    row=1;
    cout<<"Please input string:"<<endl;
    do
    {
        cin.get(KeyChar);
        Project[p++]=KeyChar;
    }
    while(KeyChar != '#');
    p=0;
    do
    {
        SourceScan();
        switch(syn)
        {
            case 11: cout<<"("<<syn<<","<<sum<<")"<<endl; break;
            case -1: cout<<"Error in row "<<row<<"!"<<endl; break;
            case -2: row=row++;break;
            default: cout << "(" << syn << "," << Token << ")" << endl;break;
        }
    }
    while (syn!=0);
}