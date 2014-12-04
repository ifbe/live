void cubie(int x,int y,int z)
{
	int i,j,color;
	for(j=y*160;j<(y+1)*160;j++)
	{
		for(i=x*160;i<x*160+5;i++)
		{
			point(i,j,0x44444444);
			point(i+155,j,0x44444444);
		}
	}
	for(i=x*160;i<(x+1)*160;i++)
	{
		for(j=y*160;j<y*160+5;j++)
		{
			point(i,j,0x44444444);
			point(i,j+155,0x44444444);
		}
	}

	switch(z)
	{
		case 0:color=0x55555555;break;
		case 2:color=0xfffffff0;break;
		case 4:color=0xffffffc0;break;
		case 8:color=0x995000;break;
		case 16:color=0xc05000;break;
		case 32:color=0xb03000;break;
		case 64:color=0xff0000;break;
		case 128:color=0xffffa0;break;
		case 256:color=0xffff80;break;
		case 512:color=0xffff00;break;
		case 1024:color=0xffffb0;break;
		case 2048:color=0xffffffff;break;
		case 4096:color=0xffffffff;break;
	}
	for(i=x*160+5;i<x*160+155;i++)
		for(j=y*160+5;j<y*160+155;j++)
			point(i,j,color);

	if(z!=0) decimal(4+x*10,4+y*10,z);
}


int power(int in)
{
	if(in==0) return 1;

	int temp=2;
	for(in-=1;in>0;in--) temp*=2;
	return temp;
}


void main()
{
	//第0步:初始化
	int i,j,temp;
	int table[4][4];
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
				//temp=j*4 +i;
				//table[i][j]=power(temp);
				table[i][j]=0;
		}
	}
	table[random() & 0x3][random() & 0x3]=power( (random() &0x1) +1 );


	while(1)
	{
		//第1步:显示世界
		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				cubie(i,j,table[j][i]);
		writescreen();
		
		
		//第2步:等待虫子
		int key=waitevent();
		

		//第3步:虫子改变世界
		if(key<=0) break;
		else if(key==0x40000050)	//left
		{
		for(i=0;i<4;i++)
		{
			int count=0;
			int temp;
			for(j=0;j<4;j++)	//整理
			{
			if(table[i][j] != 0)
			{
				temp=table[i][j];
				table[i][j]=0;
				table[i][count++]=temp;
			}
			}

			//if(count==1)
			if(count==2){
				if(table[i][0]==table[i][1]){
					table[i][0] *=2;
					table[i][1]=0;
				}
			}

			if(count==3){
				if(table[i][0]==table[i][1]){
					table[i][0] *=2;
					table[i][1]=table[i][2];
					table[i][2]=0;
				}
				else if(table[i][1]==table[i][2]){
					table[i][1] *=2;
					table[i][2]=0;
				}
			}
			if(count==4){
				if(table[i][0]==table[i][1]){
					table[i][0] *=2;
					table[i][1]=table[i][2];
					table[i][2]=table[i][3];
					table[i][3]=0;

					if(table[i][1]==table[i][2]){
						table[i][1] *=2;
						table[i][2]=0;
					}
				}
				else if(table[i][1]==table[i][2]){
					table[i][1] *=2;
					table[i][2]=table[i][3];
					table[i][3]=0;
				}
				else if(table[i][2]==table[i][3]){
					table[i][2] *=2;
					table[i][3]=0;
				}
			}

		}
		}
		else if(key==0x4000004f)	//right
		{
		for(i=0;i<4;i++)
		{
			int count=0;
			int temp;
			for(j=3;j>=0;j--)
			{
			if(table[i][j] != 0)
			{
				temp=table[i][j];
				table[i][j]=0;
				table[i][3-count]=temp;
				count++;
			}
			}


			//if(count==1)
			if(count==2){
				if(table[i][3]==table[i][2]){
					table[i][3] *=2;
					table[i][2]=0;
				}
			}

			if(count==3){
				if(table[i][3]==table[i][2]){
					table[i][3] *=2;
					table[i][2]=table[i][1];
					table[i][1]=0;
				}
				else if(table[i][2]==table[i][1]){
					table[i][2] *=2;
					table[i][1]=0;
				}
			}

			if(count==4){
				if(table[i][3]==table[i][2]){
					table[i][3] *=2;
					table[i][2]=table[i][1];
					table[i][1]=table[i][0];
					table[i][0]=0;

					if(table[i][1]==table[i][2]){
						table[i][2] *=2;
						table[i][1]=0;
					}
				}
				else if(table[i][2]==table[i][1]){
					table[i][2] *=2;
					table[i][1]=table[i][0];
					table[i][0]=0;
				}
				else if(table[i][1]==table[i][0]){
					table[i][1] *=2;
					table[i][0]=0;
				}
			}

		}
		}
		else if(key==0x40000052)	//up
		{
		for(j=0;j<4;j++)
		{
			int count=0;
			int temp;
			for(i=0;i<4;i++)
			{
			if(table[i][j] != 0)
			{
				temp=table[i][j];
				table[i][j]=0;
				table[count++][j]=temp;
			}
			}

			//if(count==1)
			if(count==2){
				if(table[0][j]==table[1][j]){
					table[0][j] *=2;
					table[1][j]=0;
				}
			}
			if(count==3){
				if(table[0][j]==table[1][j]){
					table[0][j] *=2;
					table[1][j]=table[2][j];
					table[2][j]=0;
				}
				else if(table[1][j]==table[2][j]){
					table[1][j] *=2;
					table[2][j]=0;
				}
			}
			if(count==4){
				if(table[0][j]==table[1][j]){
					table[0][j] *=2;
					table[1][j]=table[2][j];
					table[2][j]=table[3][j];
					table[3][j]=0;

					if(table[1][j]==table[2][j]){
						table[1][j] *=2;
						table[2][j]=0;
					}
				}
				else if(table[1][j]==table[2][j]){
					table[1][j] *=2;
					table[2][j]=table[3][j];
					table[3][j]=0;
				}
				else if(table[2][j]==table[3][j]){
					table[2][j] *=2;
					table[3][j]=0;
				}
			}

		}
		}
		else if(key==0x40000051)	//down
		{
		for(j=0;j<4;j++)
		{
			int count=0;
			int temp;
			for(i=3;i>=0;i--)
			{
			if(table[i][j] != 0)
			{
				temp=table[i][j];
				table[i][j]=0;
				table[3-count][j]=temp;
				count++;
			}
			}

			//if(count==1)
			if(count==2){
				if(table[3][j]==table[2][j]){
					table[3][j] *=2;
					table[2][j]=0;
				}
			}
			if(count==3){
				if(table[3][j]==table[2][j]){
					table[3][j] *=2;
					table[2][j]=table[1][j];
					table[1][j]=0;
				}
				else if(table[1][j]==table[2][j]){
					table[2][j] *=2;
					table[1][j]=0;
				}
			}
			if(count==4){
				if(table[3][j]==table[2][j]){
					table[3][j] *=2;
					table[2][j]=table[1][j];
					table[1][j]=table[0][j];
					table[0][j]=0;

					if(table[1][j]==table[2][j]){
						table[2][j] *=2;
						table[1][j]=0;
					}
				}
				else if(table[2][j]==table[1][j]){
					table[2][j] *=2;
					table[1][j]=table[0][j];
					table[0][j]=0;
				}
				else if(table[1][j]==table[0][j]){
					table[1][j] *=2;
					table[0][j]=0;
				}
			}
		}
		}
		else continue;

		int count=0;
		int temp;
		for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if(table[i][j]==0) count++;
		}
		}

		if(count!=0){
			temp=random() %count;
			for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				if(table[i][j] == 0){
					if(temp==0){
					table[i][j]=power((random() & 0x1) +1 );
					//decimal(100,20,table[i][j]);
					goto breakall;
					}
					else temp--;
				}
			}
			}
		}
		breakall:
		temp=0;
	}
}
