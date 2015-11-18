#define u64 long long
static unsigned char buffer[128];
static int current;

static double result;



double calculator()
{
	unsigned char* infix=buffer;
    char postfix[128];
    char stack[128];
    double stack2[20];
    int num=0,top=0,temp;

    for(temp=0;temp<127;temp++){stack[temp]=postfix[temp]=0x20;}
    for(temp=0;temp<20;temp++){stack2[temp]=0.00;}
    stack[127]=postfix[127]='\0';

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
	result=stack2[0];
    return stack2[0];
}


void printworld()
{
	int x,y;
	for(x=0;x<1024;x++)
	{
		for(y=0;y<768;y++)
		{
			point(x,y,0x44444444);
		}
	}
    string(0,0,buffer);
    printdouble(50,20,result);
	writescreen();
}


void main()
{
	int i;
    //string(55,20,"calculator 0.0001");
    for(i=0;i<127;i++){buffer[i]=0x20;}
    buffer[127]='\0';        //字符串结尾
    current=0;


    while(1)
    {
		printworld();
		
        int key=waitevent();

        switch(key)
        {
            case -1:return;
            case 8:{
                anscii(current-1,0,0x20);
                buffer[current]=0x20;
                if(current!=0)current--;
                break;
            }
            case 0xd:{
                calculator();           //进行计算
                for(i=0;i<127;i++){buffer[i]=0x20;} //处理完毕，清空
                current=0;                           //从头再来
                break;
            }
            default:{
                buffer[current]=key;  //进来一个convert一个
                string(0,0,buffer);
                current=(current+1)%127;
            }
        }
    }
}
