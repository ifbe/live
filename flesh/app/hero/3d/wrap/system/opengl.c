#include<stdio.h>
#include<GL/freeglut.h>


void display(void) 
{
glPointSize(2.5f);
glColor3f(1.0, 1.0, 0);
glBegin(GL_POINTS);
    glVertex3f(0.0,0.0,0.0);  //��ɫ
    glVertex3f(0.1,0.1,0.0);  //��ɫ
    glVertex3f(0.2,0.2,0.0);  //��ɫ
glEnd();
    glutSwapBuffers();
}
void mouse(int x,int y)
{
	gluLookAt(0,0,0,0.01,0,-1,0,1,0.01);  //��
	//gluLookAt(0,0,0,-0.01,0,-1,0,1,-0.01);  //��
	//gluLookAt(0,0,0,0,-0.01,-1,0,1,-0.01);  //��
	//gluLookAt(0,0,0,0,0.01,-1,0,1,0.01);  //��
}
//int main(int argc, char **argv)
__attribute__((constructor)) void initopengl(int argc, char **argv)
{
    glutInit(&argc, argv);//��ʼ�� ����������GLUTʹ��֮ǰ����һ��
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);//��������Ļ�еĳ�ʼλ��
    glutInitWindowSize(500, 500);//���ڴ�С
    glutCreateWindow("draw");//��������,������Ϊ���ڱ���
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); 
glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    glutDisplayFunc(display);//����һ���Զ��庯��
    glutIdleFunc(display);// ָ�����ڴ�С�ı�ʱ���¼���ͶӰ�ķ�ʽ

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