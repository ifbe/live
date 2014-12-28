#include<stdio.h>
#include<GL/freeglut.h>


void display(void) 
{
glPointSize(2.5f);
glColor3f(1.0, 1.0, 0);
glBegin(GL_POINTS);
    glVertex3f(0.0,0.0,0.0);  //彩色
    glVertex3f(0.1,0.1,0.0);  //彩色
    glVertex3f(0.2,0.2,0.0);  //彩色
glEnd();
    glutSwapBuffers();
}
void mouse(int x,int y)
{
	gluLookAt(0,0,0,0.01,0,-1,0,1,0.01);  //左
	//gluLookAt(0,0,0,-0.01,0,-1,0,1,-0.01);  //右
	//gluLookAt(0,0,0,0,-0.01,-1,0,1,-0.01);  //下
	//gluLookAt(0,0,0,0,0.01,-1,0,1,0.01);  //上
}
//int main(int argc, char **argv)
__attribute__((constructor)) void initopengl(int argc, char **argv)
{
    glutInit(&argc, argv);//初始化 必须在其他GLUT使用之前调用一次
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);//窗口在屏幕中的初始位置
    glutInitWindowSize(500, 500);//窗口大小
    glutCreateWindow("draw");//创建窗口,参数作为窗口标题
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); 
glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    glutDisplayFunc(display);//调用一个自定义函数
    glutIdleFunc(display);// 指定窗口大小改变时重新计算投影的方式

}
__attribute__((destructor)) void destoryopengl()
{
	glutLeaveMainLoop();
	printf("destruct");
}








void writescreen()
{
	glutMainLoopEvent();
	glutPostRedisplay();
}