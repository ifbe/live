//#include<stdio.h>
//input
static unsigned char buffer[128];
static int current=0;
//节点
struct node{
	int parent;

	int p[6];
	int data[6];
};				//5指针4数据的B树
static struct node node[100];
static int head=1;



void printnode(int line,int row,int num)
{
//printf("node[%d]:%d,%d,%d,%d,%d\n",num,node[num].data[0],node[num].data[1],node[num].data[2],node[num].data[3],node[num].data[4],node[num].data[5]);
	int i,j;
	int x,y;
	//得到具体坐标
	y=16*2*line;
	if(line==0)x=1024/2;		//第0行只有一个，屏幕中间
	else if(line==1)x=1024/5*row;		//第1行5个
	else if(line==2)x=1024/25*row;	//第2行25个
	else return;
	//竖线
	for(j=y;j<y+16;j++)
	{
		point(x,j,0);
		point(x+16,j,0);
		point(x+32,j,0);
		point(x+48,j,0);
		point(x+64,j,0);
	}
	//横线
	for(i=x;i<x+64;i++)
	{
		point(i,y,0);
		point(i,y+16,0);
	}

	//框里的数字
	for(i=0;i<5;i++)
	{
		decimal(x/8+i*2,y/16,node[num].data[i]);
	}

	//下级节点
	for(i=0;i<5;i++)
	{
		if(node[num].p[i] != 0)
		{
			printnode(line+1,row*5+i,node[num].p[i]);
		}
	}
}
void printworld()
{
	int i,j;
	//清屏
	for(i=0;i<1024;i++)    //1024*768
	{
		for(j=0;j<768;j++)
		{
			point(i,j,0x44444444);
		}
	}
	//从根节点开始
	printnode(0,0,head);
	//第一行,buffer里面的输入的字符
	string(0,0,buffer);
	//写屏
	writescreen();
}
int value()
{
	int temp=0;
	int i;
	//得到值,然后清空buffer,检查得到的值,0或者错误的就返回
	for(i=0;i<current;i++)
	{
		if( (buffer[i]<0x3a) && (buffer[i]>=0x30) )
		{
			temp=temp*10+(buffer[i]-0x30);
		}
		else break;
	}
	current=0;
	for(i=0;i<128;i++)buffer[i]=0;

	return temp;
}
int newnode()
{
	int i;
	for(i=1;i<100;i++)
	{
		if(node[i].data[0] == 0)
		{
			if(node[i].p[0] == 0)return i;
		}
	}
	return 0;		//没有新的节点了
}
void putdataandp(int this,int value,int pointer)
{
	int count;
	int where;
	int i;
	//算出最后一个在哪儿，比如:
	//空的得到where=0
	//有一个数据得到where=1
	//有两个数据得到where=2
	for(count=0;count<5;count++)
	{
		if(node[this].data[count] == 0)break;
	}
	//算出应该放在哪儿，比如:
	//value=3数据是4，5，8得到where=0
	//value=6数据是3，7，11，13，得到where=1
	for(where=0;where<count;where++)
	{
		if(value <= node[this].data[where])break;
	}
	//挪动直到按顺序排列几个数字,然后放value
	for(i=count;i>where;i--)
	{
		node[this].data[i]=node[this].data[i-1];
		node[this].p[i+1]=node[this].p[i];
	}
	node[this].data[where]=value;
	node[this].p[where+1]=pointer;
}
void split(int this)
{
	//如果这个是根节点
	//节点里面一定是这样：p0,data0,p1,data1,p2,data2,p3,data3,p4,data4,p5
	//head里面:this,data2,brother,0000000000
	//brother里面:p3,data3,p4,data4,p5,00000000000
	//this里面:p0,data0,p1,data1,p2,0000000000000
	//如果不是根节点，往parent里放一个data和一个p
	//如果parent也满了，再裂一次

	int parent;
	int brother;
	//抽个象使得根和非根节点同样处理
	if(node[this].parent == 0)
	{
		head=newnode();
		parent=head;
		node[head].parent=0;
		node[head].p[0]=this;
		brother=newnode();
	}
	else
	{
		parent=node[this].parent;
		brother=newnode();
	}
	//printf("spliting:parent=%d,this=%d,brother=%d\n",parent,this,brother);
	//上移一个data和一个p
	putdataandp(parent,node[this].data[2],brother);
	//brother
	node[brother].parent=parent;
	node[brother].data[0]=node[this].data[3];
	node[brother].data[1]=node[this].data[4];
	node[brother].p[0]=node[this].p[3];
	node[brother].p[1]=node[this].p[4];
	node[brother].p[2]=node[this].p[5];
	//this
	node[this].parent=parent;
	node[this].data[2]=0;		//data0,data1不变
	node[this].data[3]=0;
	node[this].data[4]=0;
	node[this].p[3]=0;		//p0,p1,p2不变
	node[this].p[4]=0;
	node[this].p[5]=0;
	//再裂
	if(node[this].data[4] != 0)split(node[this].parent);
}
void insert(int this,int value)
{
	int i;
	if(node[this].p[0] != 0)
	{
		//不是叶子,一定不放进本节点，所以不会出现分裂问题，并且正常非叶子节点data的数量+1=p的数量
		//判断往哪个子节点放就行
		for(i=0;i<5;i++)
		{
			if(node[this].data[i] == 0)break;
			if(value <= node[this].data[i])break;
		}
		insert(node[this].p[i],value);
	}
	else
	{
		//是叶子，先不管直接放
		putdataandp(this,value,0);
		//放完多了就分裂
		if(node[this].data[4] != 0)split(this);
	}
}
void main()
{
	//第0步:初始化
	int i,j;
	head=1;
	for(i=0;i<127;i++)buffer[i]=0;
	for(i=0;i<100;i++)
	{
		node[i].parent=0;
		for(j=0;j<6;j++)
		{
			node[i].p[j]=0;
			node[i].data[j]=0;
		}
	}/*
	node[0].parent=0;
	node[0].data[0]=1;
	node[0].data[0]=2;
	node[0].data[0]=3;
	node[0].data[0]=4;
	node[0].p[0]=1;
	node[0].p[1]=2;
	node[0].p[2]=3;
	node[1].data[0]=3;
	node[1].p[0]=9;
	node[9].data[0]=9;*/

	while(1)
	{
		//
		printworld();
		//
		int key=waitevent();
		//
		switch(key)
		{
			case -1:return;
			case 0x1b:return;		//esc
			case 0xd:				//回车
			{
				//得到数字
				int temp=value();
				//非0就从头开始往树里放
				if(temp != 0)insert(head,temp);
				//
				break;
			}
			case 0x08:				//删除一个字符
			{
				if(current >= 1)current--;
				buffer[current]=0;

				break;
			}
			default:				//输入一个字符
			{
				buffer[current]=key;
				current=(current+1)%127;
			}
		}//switch
	}//while
}//main