#include "Render.h"

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"
#define pi 3.1415926


bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}




//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("kekeke.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	GLuint texId;
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}





void Render(OpenGL *ogl)
{       	
	
	
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);\
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

    //���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� ���������� ��������
	double vec1[3];
	double vec2[3];
	double vec3[3];

	double A[] = { 1.987,2.498,0 };
	double B[] = { 4,3,0 };
	double C[] = { 2,9,0 };
	double D[] = { 8,6,0 };
	double E[] = { 12,10,0 };
	double F[] = { 9,0,0 };
	double N[] = { 6,2,0 };

	glBegin(GL_TRIANGLES);
	glColor3d(0.31, 0.123, 0.222);
	glNormal3d(0,0,-1);
	glTexCoord2d(0.383, 0.773);
	glVertex3dv(D);
	glTexCoord2d(0.556, 0.9455);
	glVertex3dv(E);
	glTexCoord2d(0.4278, 0.515);
	glVertex3dv(F);
	glEnd();

	//������ �����
	double A1[] = { 1.987,2.498,2 };
	double B1[] = { 4,3,2 };
	double C1[] = { 2,9,2 };
	double D1[] = { 8,6,2 };
	double E1[] = { 12,10,2 };
	double F1[] = { 9,0,2 };
	double N1[] = { 6,2,2 };
	glBegin(GL_TRIANGLES);
	glColor3d(0.31, 0.123, 0.222);
	glNormal3d(0, 0, 1);
	glTexCoord2d(0.383, 0.773);
	glVertex3dv(D1);
	glTexCoord2d(0.556, 0.9455);
	glVertex3dv(E1);
	glTexCoord2d(0.4278, 0.515);
	glVertex3dv(F1);
	glEnd();

	//�������
	vec1[0] = 0;
	vec1[1] = 0;
	vec1[2] = -2;

	glBegin(GL_QUADS);
	glColor3d(0, 0.5, 0.5);
	vec2[0] = 4;
	vec2[1] = 1;
	vec2[2] = 0;
	vec3[0] = vec2[1] * vec1[2] - vec2[2] * vec1[1];
	vec3[1] = 0 - vec2[0] * vec1[2] - vec2[2] * vec1[0];
	vec3[2] = vec2[0] * vec1[1] - vec2[1] * vec1[0];
	glNormal3d(vec3[0], vec3[1], vec3[2]);
	glTexCoord2d(0.633,0.044);
	glVertex3dv(A);
	glTexCoord2d(0.03, 0.044);
	glVertex3dv(B);
	glTexCoord2d(0.03, 0.232);
	glVertex3dv(B1);
	glTexCoord2d(0.633, 0.232);
	glVertex3dv(A1);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 0.5, 0.5);
	vec2[0] = -2;
	vec2[1] = 6;
	vec2[2] = 0;
	vec3[0] = vec2[1] * vec1[2] - vec2[2] * vec1[1];
	vec3[1] = 0 - vec2[0] * vec1[2] - vec2[2] * vec1[0];
	vec3[2] = vec2[0] * vec1[1] - vec2[1] * vec1[0];
	glNormal3d(vec3[0], vec3[1], vec3[2]);
	glTexCoord2d(0.633, 0.044);
	glVertex3dv(B);
	glTexCoord2d(0.03, 0.044);
	glVertex3dv(C);
	glTexCoord2d(0.03, 0.232);
	glVertex3dv(C1);
	glTexCoord2d(0.633, 0.232);
	glVertex3dv(B1);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 0.5, 0.5);
	vec2[0] = 6;
	vec2[1] = -3;
	vec2[2] = 0;
	vec3[0] = vec2[1] * vec1[2] - vec2[2] * vec1[1];
	vec3[1] = 0 - vec2[0] * vec1[2] - vec2[2] * vec1[0];
	vec3[2] = vec2[0] * vec1[1] - vec2[1] * vec1[0];
	glNormal3d(vec3[0], vec3[1], vec3[2]);
	glTexCoord2d(0.633, 0.044);
	glVertex3dv(C);
	glTexCoord2d(0.03, 0.044);
	glVertex3dv(D);
	glTexCoord2d(0.03, 0.232);
	glVertex3dv(D1);
	glTexCoord2d(0.633, 0.232);
	glVertex3dv(C1);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 0.5, 0.5);
	vec2[0] = 4;
	vec2[1] = 4;
	vec2[2] = 0;
	vec3[0] = vec2[1] * vec1[2] - vec2[2] * vec1[1];
	vec3[1] = 0 - vec2[0] * vec1[2] - vec2[2] * vec1[0];
	vec3[2] = vec2[0] * vec1[1] - vec2[1] * vec1[0];
	glNormal3d(vec3[0], vec3[1], vec3[2]);
	glTexCoord2d(0.633, 0.044);
	glVertex3dv(D);
	glTexCoord2d(0.03, 0.044);
	glVertex3dv(E);
	glTexCoord2d(0.03, 0.232);
	glVertex3dv(E1);
	glTexCoord2d(0.633, 0.232);
	glVertex3dv(D1);
	glEnd();


	double x, y, x1, y1;
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, 1);
	glColor3d(0.5, 0.5, 0.5);
	glTexCoord2d(0.491, 0.729);
	glVertex3d(10.5, 5, 2);
	for (double a = -47.69; a < 14.3; a += 0.01) {
		x = 2.16667 + cos(a*pi / 180)*10.15;
		y = 7.5 + sin(a*pi / 180)*10.15;
		x1 = 0.13259 + cos(a*pi / 180)*0.44;
		y1 = 0.83725 + sin(a*pi / 180)*0.44;
		glTexCoord2d(x1,y1);
		glVertex3d(x, y, 2);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, -1);
	glColor3d(0.31, 0.123, 0.222);
	glTexCoord2d(0.491, 0.729);
	glVertex3d(10.5, 5, 0);
	for (double a = -47.69; a < 14.3; a += 0.01) {
		x = 2.16667 + cos(a*pi / 180)*10.15;
		y = 7.5 + sin(a*pi / 180)*10.15;
		x1 = 0.13259 + cos(a*pi / 180)*0.44;
		y1 = 0.83725 + sin(a*pi / 180)*0.44;
		glTexCoord2d(x1, y1);
		glVertex3d(x, y, 0);
	}
	glEnd();

	double S[3], T[3], U[3], O[3];
	x1 = 12;
	y1 = 10;

	glBegin(GL_QUADS);
	vec1[0] = 0;
	vec1[1] = 0;
	vec1[2] = 2;
	double pip = 0.633;
	double pip1 = 0.633;
	glColor3d(0.31, 0.5, 0.222);
	for (double a = -47.69; a < 14.3; a += 0.01) {
		x = 2.16667 + cos(a*pi / 180)*10.15;
		y = 7.5 + sin(a*pi / 180)*10.15;
		pip -= 0.00009727375;
		vec2[0] = x - x1;
		vec2[1] = y - y1;
		vec2[2] = 0;
		vec3[0] = vec2[1] * vec1[2] - vec2[2] * vec1[1];
		vec3[1] = 0 - vec2[0] * vec1[2] - vec2[2] * vec1[0];
		vec3[2] = vec2[0] * vec1[1] - vec2[1] * vec1[0];

		S[0] = x1;
		S[1] = y1;
		S[2] = 2;
		T[0] = x1;
		T[1] = y1;
		T[2] = 0;
		U[0] = x;
		U[1] = y;
		U[2] = 0;
		O[0] = x;
		O[1] = y;
		O[2] = 2;
		glNormal3d(vec3[0], vec3[1], vec3[2]);
		glTexCoord2d(pip1, 0.232);
		glVertex3dv(S);
		glTexCoord2d(pip1, 0.044);
		glVertex3dv(T);
		glTexCoord2d(pip, 0.044);
		glVertex3dv(U);
		glTexCoord2d(pip, 0.232);
		glVertex3dv(O);
		x1 = x;
		y1 = y;
		pip1 -= 0.00009727375;
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.31, 0.123, 0.222);
	glNormal3d(0, 0, -1);
	glTexCoord2d(0.383, 0.773);
	glVertex3d(8, 6, 0);
	for (double a = 43.75; a < 80; a += 0.01) {
		x = 3 + cos(a*pi / 180)*8.31;
		y = -5.75 + sin(a*pi / 180)*8.31;
		x1 = 0.168 + cos(a*pi / 180)*0.36;
		y1 = 0.26274 + sin(a*pi / 180)*0.36;
		glTexCoord2d(x1, y1);
		glVertex3d(x, y, 0);
	}
	double kek = x;
	double shpek = y;
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, -1);
	glColor3d(0.31, 0.123, 0.222);
	glTexCoord2d(0.21, 0.643);
	glVertex3d(4, 3, 0);
	for (double a = 80; a < 97; a += 0.01) {
		x = 3 + cos(a*pi / 180)*8.31;
		y = -5.75 + sin(a*pi / 180)*8.31;
		x1 = 0.168 + cos(a*pi / 180)*0.36;
		y1 = 0.26274 + sin(a*pi / 180)*0.36;
		glTexCoord2d(x1, y1);
		glVertex3d(x, y, 0);
	}
	glEnd();
	x1 = 9;
	y1 = 0;
	vec1[0] = 0;
	vec1[1] = 0;
	vec1[2] = -2;
	glBegin(GL_QUADS);
	pip = 0.633;
	pip1 = 0.633;
	glColor3d(0.5, 0.123, 0.222);
	for (double a = 43.75; a < 80; a += 0.01) {
		x = 3 + cos(a*pi / 180)*8.31;
		y = -5.75 + sin(a*pi / 180)*8.31;
		pip -= 0.00011323943;
		vec2[0] = x - x1;
		vec2[1] = y - y1;
		vec2[2] = 0;
		vec3[0] = vec2[1] * vec1[2] - vec2[2] * vec1[1];
		vec3[1] = 0 - vec2[0] * vec1[2] - vec2[2] * vec1[0];
		vec3[2] = vec2[0] * vec1[1] - vec2[1] * vec1[0];
		S[0] = x1;
		S[1] = y1;
		S[2] = 2;
		T[0] = x1;
		T[1] = y1;
		T[2] = 0;
		U[0] = x;
		U[1] = y;
		U[2] = 0;
		O[0] = x;
		O[1] = y;
		O[2] = 2;
		glNormal3d(vec3[0], vec3[1], vec3[2]);
		glTexCoord2d(pip1, 0.232);
		glVertex3dv(S);
		glTexCoord2d(pip1, 0.044);
		glVertex3dv(T);
		glTexCoord2d(pip, 0.044);
		glVertex3dv(U);
		glTexCoord2d(pip, 0.232);
		glVertex3dv(O);
		x1 = x;
		y1 = y;
		pip1 -= 0.00011323943;
	}
	for (double a = 80; a < 97; a += 0.01) {
		x = 3 + cos(a*pi / 180)*8.31;
		y = -5.75 + sin(a*pi / 180)*8.31;
		pip -= 0.00011323943;
		vec2[0] = x - x1;
		vec2[1] = y - y1;
		vec2[2] = 0;
		vec3[0] = vec2[1] * vec1[2] - vec2[2] * vec1[1];
		vec3[1] = 0 - vec2[0] * vec1[2] - vec2[2] * vec1[0];
		vec3[2] = vec2[0] * vec1[1] - vec2[1] * vec1[0];
		S[0] = x1;
		S[1] = y1;
		S[2] = 2;
		T[0] = x1;
		T[1] = y1;
		T[2] = 0;
		U[0] = x;
		U[1] = y;
		U[2] = 0;
		O[0] = x;
		O[1] = y;
		O[2] = 2;
		glNormal3d(vec3[0], vec3[1], vec3[2]);
		glTexCoord2d(pip1, 0.232);
		glVertex3dv(S);
		glTexCoord2d(pip1, 0.044);
		glVertex3dv(T);
		glTexCoord2d(pip, 0.044);
		glVertex3dv(U);
		glTexCoord2d(pip, 0.232);
		glVertex3dv(O);
		x1 = x;
		y1 = y;
		pip1 -= 0.00011323943;
	}
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, 1);
	glColor3d(0.31, 0.123, 0.222);
	glTexCoord2d(0.383, 0.773);
	glVertex3d(8, 6, 2);
	for (double a = 43.75; a < 80; a += 0.01) {
		x = 3 + cos(a*pi / 180)*8.31;
		y = -5.75 + sin(a*pi / 180)*8.31;
		x1 = 0.168 + cos(a*pi / 180)*0.36;
		y1 = 0.26274 + sin(a*pi / 180)*0.36;
		glTexCoord2d(x1, y1);
		glVertex3d(x, y, 2);
	}
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, 1);
	glColor3d(0.31, 0.123, 0.222);
	glTexCoord2d(0.21, 0.643);
	glVertex3d(4, 3, 2);
	for (double a = 80; a < 97; a += 0.01) {
		x = 3 + cos(a*pi / 180)*8.31;
		y = -5.75 + sin(a*pi / 180)*8.31;
		x1 = 0.168 + cos(a*pi / 180)*0.36;
		y1 = 0.26274 + sin(a*pi / 180)*0.36;
		glTexCoord2d(x1, y1);
		glVertex3d(x, y, 2);
	}
	glEnd();
	double lal[] = { kek, shpek, 0 };
	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glColor3d(1, 0.5, 0.5);
	glTexCoord2d(0.125,0.9);
	glVertex3dv(C);
	glTexCoord2d(0.383, 0.773);
	glVertex3dv(D);
	glTexCoord2d(0.23, 0.62);
	glVertex3dv(lal);
	glTexCoord2d(0.21, 0.643);
	glVertex3dv(B);
	glEnd();
	double lal1[] = { kek, shpek, 2.0 };

	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glColor3d(0, 0.2, 0.5);
	glTexCoord2d(0.125, 0.9);
	glVertex3dv(C1);
	glTexCoord2d(0.383, 0.773);
	glVertex3dv(D1);
	glTexCoord2d(0.23, 0.62);
	glVertex3dv(lal1);
	glTexCoord2d(0.21, 0.643);
	glVertex3dv(B1);
	glEnd();
	//����� ��������� ���������� ��������
    
	
	//����� ��������� ������ �����, ���� ������� - ���������������, ��� �������� =)
	char c[250];  //������������ ����� ���������
	sprintf_s(c, "(T)�������� - %d\n(L)���� - %d\n\n���������� ������:\n"
		"G - ����������� � �������������� ���������,\nG+���+����������� �� ������������ �����\n"
		"R - ���������� ������ � ���� � ��������� ���������\n"
		"F - ����������� ���� � ����� ������", textureMode, lightMode);
	ogl->message = std::string(c);




}   //����� ���� �������

