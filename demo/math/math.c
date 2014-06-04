#define u64 long long

void init()
{
    u64* video=(u64*)0x3028;
    u64 base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;
    int i;

    for(i=0;i<1024*768;i++)    //1024*768
    {
        video=(u64*)base;
        *video=0;
        base+=bpp;
    }
}


double cosine(double x)
{
    double ret=0,item=1.0,temp;
    int n=0,i,sign=1;
    if(x>2*3.1415||x<-2*3.1415){x-=((int)(x/(2*3.1415)))*(2*3.1415);}

    do{
        temp=item;
        for(i=1;i<=2*n;i++)temp/=i;
        ret+=sign*temp;
        item*=x*x;
        sign *=-1;
        n++;
      }while (temp>1e-10);
return ret;
}


double sine(double x)
{
    int m = 1,i;
    double temp,ret = 0.0;
    if(x>2*3.1415||x<-2*3.1415){x-=((int)(x/(2*3.1415)))*(2*3.1415);}

    do{
        i=0;
        if (m%2 == 0){temp= -1.0;}
        else{temp= 1.0;}
        for(i=1;i<=2*m-1;i++){temp = temp * x/i;}
        ret+= temp;
        m++;
    }while (temp<-.0000005||temp>0.0000005);
return ret;
}


char convert(char old)
{
    char* p;
    u64 rsi;
    char new=0x20;
    int i;

    rsi=0x6800;
    for(i=0;i<44;i++)
    {
        p=(char*)rsi;
        if(*p==old)
        {
            rsi++;
            p=(char*)rsi;
            new=*p;
            break;
        }
        rsi+=2;
    }
    return new;
}


void point(int x,int y,int z)
{
    u64* video=(u64*)0x3028;
    u64 base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;

    int* address;

    address=(int*)(base+(y*1024+x)*bpp);
    *address=z;
}


void print(int x,int y,char ch)
{
    int i,j;
    u64 rsi=0x6000;
    char temp;
    char* p;

    rsi+=ch<<4;
    x=8*x;
    y=16*y;

    for(i=0;i<16;i++)
    {
        p=(char*)rsi;
        for(j=0;j<8;j++)
        {
            temp=*p;
            temp=temp<<j;
            temp&=0x80;
            if(temp!=0){point(j+x,i+y,0xffffffff);}
            else{point(j+x,i+y,0);}
        }
    rsi++;
    }
}

void printdouble(int x,int y,double value)
{
    u64 temp;
    int i;
    if(value<0)
    {
        value=-value;
        print(x,y,'-');
    }
    temp=(u64)value;
    value=(value-(double)temp);      //清空小数点前面的
    value*=100000000;
    for(i=1;i<9;i++)
    {
        print(x+9-i,y,temp%10+0x30);
        temp/=10;
    }
    print(x+9,y,'.');
    temp=(u64)value;
    for(i=1;i<9;i++)
    {
        print(x+18-i,y,temp%10+0x30);
        temp/=10;
    }
}


void string(int x,int y,char* p)
{
    while(*p!='\0')
    {
    print(x,y,*p);
    p++;
    x++;
    }
}


double calculator(char* infix)
{
    char postfix[128];
    char stack[128];
    char result[128];
    double stack2[20];
    int num=0,top=0,temp;

    for(temp=0;temp<127;temp++){stack[temp]=postfix[temp]=result[temp]=0x20;}
    for(temp=0;temp<20;temp++){stack2[temp]=0.00;}
    stack[127]=postfix[127]=result[127]='\0';

    while(*infix!='\0')
    {
        switch(*infix)
        {
            case 0x20:break;            //空格惹祸
            case '+':case '-':{
                postfix[num]=' ';          //分隔符
                num++;
                while((stack[top]!='[')&&(stack[top]!=0x20))   //栈顶不是左括号又不是空格的话
                {
                    postfix[num]=stack[top];                    //就先输出栈顶
                    num++;
                    stack[top]=0x20;                           //再清理栈顶
                    top--;
                }
                top++;                                    //直到是左括号或者空格
                stack[top]=*infix;                       //符号进栈
                break;
            }
            case '*':case '/':{
                postfix[num]=' ';
                num++;
                if(stack[top]=='*'||stack[top]=='/')
                {
                    postfix[num]=stack[top];
                    num++;
                    stack[top]=*infix;
                }
                else{top++;stack[top]=*infix;}

                break;
            }
            case '[':{
                top++;
                stack[top]='[';         //啥都不管闭着眼睛直接进栈
                break;
            }
            case ']':{
                num++;
                while((stack[top]!='[')&&(stack[top]!=0x20))
                {
                    postfix[num]=stack[top];    //一直弹出直到遇到左括号或者空格
                    num++;
                    stack[top]=0x20;
                    top--;
                }
                if(stack[top]!=0x20)          //不是空栈
                {
                    stack[top]=0x20;          //那一定是左括号了
                    top--;                    //弹出左括号扔掉
                }
                break;
            }
            default:{
                postfix[num]=*infix;
                num++;
            }
        }
        infix++;
    }
    while(stack[top]!=0x20)       //不怕空的，上面stack[0]一定是0x20
    {
        postfix[num]=0x20;
        num++;
        postfix[num]=stack[top];
        num++;
        stack[top]=0x20;          //清理栈顶
        top--;
    }

//    string(0,2,postfix);       //显示中缀转后缀结束,结果在postfix[128]里面

    num=0;
    top=0;
    while(num<127)
    {
        switch(postfix[num])
        {
            case 0x20:{
                if(postfix[num-1]>='0'&&postfix[num-1]<='9'){top++;}
                break;  //遇空格说明数字结束
            }
            case '.':{
                temp=1;
                num++;
                while(postfix[num]!=0x20)
                {
                    temp*=10;
                    stack2[top]+=(double)(postfix[num]-0x30)/(double)temp;
                    num++;
                }
                top++;
                break;
            }
            case '+':{
                top--;
                stack2[top-1]+=stack2[top];
                stack2[top]=0.00;
                break;
            }
            case '-':{
                top--;
                stack2[top-1]-=stack2[top];
                stack2[top]=0.00;
                break;
            }
            case '*':{
                top--;
                stack2[top-1]*=stack2[top];
                stack2[top]=0.00;
                break;
            }
            case '/':{
                top--;
                if((stack2[top]>0.0001)||(stack2[top]<-0.0001))
                {
                    stack2[top-1]/=stack2[top];
                }
                else
                {
                    stack2[top-1]=1.7e+308;
                }
                stack2[top]=0.00;
                break;
            }
            default:{        //如果是数字
                stack2[top]=10*stack2[top]+(double)(postfix[num]-0x30);
            }
        }
    num++;
    }
    return stack2[0];
}


void analyse(char* anscii)
{
    int i=0;

    while(*(anscii+i)!='\0')
    {
        i++;
    }

    init();
    string(0,0,anscii);
    printdouble(50,20,calculator(anscii));
}


void main()
{
    char buffer[128];
    char anscii[128];
    int i;
    anscii[127]='\0';        //字符串结尾
    for(i=0;i<127;i++){anscii[i]=0x20;}
    i=0;

    init();
    string(55,20,"calculator 0.0001");

    while(1)
    {
        buffer[i]=hltwait();
        switch(buffer[i])
        {
            case 0x01:break;
            case 0x0e:{
                print(i-1,0,0x20);
                anscii[i]=0x20;
                if(i!=0)i--;
                break;
            }
            case 0x1c:{
                analyse(anscii);           //进行计算
                for(i=0;i<127;i++){anscii[i]=0x20;} //处理完毕，清空
                i=0;                           //从头再来
                break;
            }
            default:{
                anscii[i]=convert(buffer[i]);  //进来一个convert一个
                string(0,0,anscii);
                i=(i+1)%127;
            }
        }
    }
}
