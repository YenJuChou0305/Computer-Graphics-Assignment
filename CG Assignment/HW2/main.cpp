/* 
[CG hw2 stencil buffer]
You can focus on the part "You may need to do something here", and also "object_class.h"
If needed, please feel free to modify the code structure and all the source code we provided.
*/
#include <stdio.h>
#include <stdlib.h>
#include "../GL/glut.h"
#include "object_class.h"
#include "image_class.h"

#define obj_total 3

//*****************************************
typedef float GLvector4f[4];							// Typedef's For VMatMult Procedure
typedef float GLmatrix16f[16];							// Typedef's For VMatMult Procedure

glObject	obj;	
glObject	obj1;
glObject	obj2;

GLfloat		xrot = 0, xspeed = 0;							
GLfloat		yrot = 0, yspeed = 0;
GLfloat		zrot = 0, zspeed = 0;
GLfloat		xrot1 = 0, xspeed1 = 0;
GLfloat		yrot1 = 0, yspeed1 = 0;
GLfloat		zrot1 = 0, zspeed1 = 0;
GLfloat		xrot2 = 0, xspeed2 = 0;
GLfloat		yrot2 = 0, yspeed2 = 0;
GLfloat		zrot2 = 0, zspeed2 = 0;


float ObjPos[] = { -1.5f, 2.0f, -15.0f };					
float Obj1Pos[] = { 1.5f, -2.0f, -15.0f };					
float Obj2Pos[] = { 2.0f, 2.0f, -15.0f };

float SpherePos[] = { -4.0f,-5.0f,-6.0f };

int select_obj=0;


void VMatMult(GLmatrix16f M, GLvector4f v)
{
	GLfloat res[4];										// Hold Calculated Results
	res[0] = M[0] * v[0] + M[4] * v[1] + M[8] * v[2] + M[12] * v[3];
	res[1] = M[1] * v[0] + M[5] * v[1] + M[9] * v[2] + M[13] * v[3];
	res[2] = M[2] * v[0] + M[6] * v[1] + M[10] * v[2] + M[14] * v[3];
	res[3] = M[3] * v[0] + M[7] * v[1] + M[11] * v[2] + M[15] * v[3];
	v[0] = res[0];										// Results Are Stored Back In v[]
	v[1] = res[1];
	v[2] = res[2];
	v[3] = res[3];										// Homogenous Coordinate
}



namespace
{
	//const double CamaraPos[]={1.5, 0.5 ,18.0};
	const double CamaraPos[]={2.0, 1.0 ,18.0};
	const double BarrierPos[]={-2.0,-2.0,-0.5};

	float LightPos[] = { 2.4f, 2.2f, -12.9f };//global position
	float LightAmb[] = { 0.1f, 0.1f, 0.3f, 1.0f };			// Ambient Light Values
	float LightDif[] = { 0.8f, 0.8f, 1.0f, 1.0f };			// Diffuse Light Values
	float LightSpc[] = { 0.5f, 0.5f, 0.5f, 1.0f };			// Specular Light Values

	float MatAmb[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Material - Ambient Values
	float MatDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };			// Material - Diffuse Values
	float MatSpc[] = { 0.0f, 0.0f, 0.0f, 1.0f };			// Material - Specular Values
	float MatShn[] = { 0.1f };								// Material - Shininess
	GLUquadric* q;
/*
	int obj_counter=0;
	object_class* obj_ptr[obj_total];
	object_class obj1("../Data/Object.txt");
	object_class obj2("../Data/Object1.txt");
	object_class obj3("../Data/Object2.txt");
*/
	image_class* image;

	bool shadow_mode=true;
}

void DrawGLRoom()										// Draw The Room (Box)
{
	
	glDisable(GL_LIGHTING);
	glColor3d(1.0,1.0,1.0);
	glEnable(GL_TEXTURE_2D);
	image[0].BuildTexture();
	glBegin(GL_QUADS);									// Begin Drawing Quads
	// Floor
	glNormal3f(0.0f, 1.0f, 0.0f);					// Normal Pointing Up
	glTexCoord2d(0.0,1.0);
	glVertex3f(-8.0f, -8.0f, -20.0f);				// Back Left
	glTexCoord2d(0.0,0.0);
	glVertex3f(-8.0f, -8.0f, 20.0f);				// Front Left
	glTexCoord2d(1.0,0.0);
	glVertex3f(8.0f, -8.0f, 20.0f);					// Front Right
	glTexCoord2d(1.0,1.0);
	glVertex3f(8.0f, -8.0f, -20.0f);				// Back Right
	glEnd();
	image[1].BuildTexture();
	glBegin(GL_QUADS);
	// Ceiling
	glNormal3f(0.0f, -1.0f, 0.0f);					// Normal Point Down
	glTexCoord2d(0.0,1.0);
	glVertex3f(-8.0f, 8.0f, 20.0f);					// Front Left
	glTexCoord2d(0.0,0.0);
	glVertex3f(-8.0f, 8.0f, -20.0f);				// Back Left
	glTexCoord2d(1.0,0.0);
	glVertex3f(8.0f, 8.0f, -20.0f);					// Back Right
	glTexCoord2d(1.0,1.0);
	glVertex3f(8.0f, 8.0f, 20.0f);					// Front Right
	glEnd();
	image[2].BuildTexture();
	glBegin(GL_QUADS);
	// Front Wall
	glNormal3f(0.0f, 0.0f, 1.0f);					// Normal Pointing Away From Viewer
	glTexCoord2d(0.0,1.0);
	glVertex3f(-8.0f, 8.0f, -20.0f);				// Top Left
	glTexCoord2d(0.0,0.0);
	glVertex3f(-8.0f, -8.0f, -20.0f);				// Bottom Left
	glTexCoord2d(1.0,0.0);
	glVertex3f(8.0f, -8.0f, -20.0f);				// Bottom Right
	glTexCoord2d(1.0,1.0);
	glVertex3f(8.0f, 8.0f, -20.0f);					// Top Right
	glEnd();
	image[3].BuildTexture();
	glBegin(GL_QUADS);
	// Back Wall
	glNormal3f(0.0f, 0.0f, -1.0f);					// Normal Pointing Towards Viewer
	glTexCoord2d(0.0,1.0);
	glVertex3f(8.0f, 8.0f, 20.0f);					// Top Right
	glTexCoord2d(0.0,0.0);
	glVertex3f(8.0f, -8.0f, 20.0f);					// Bottom Right
	glTexCoord2d(1.0,0.0);
	glVertex3f(-8.0f, -8.0f, 20.0f);				// Bottom Left
	glTexCoord2d(1.0,1.0);
	glVertex3f(-8.0f, 8.0f, 20.0f);					// Top Left
	glEnd();
	image[4].BuildTexture();
	glBegin(GL_QUADS);
	// Left Wall
	glNormal3f(1.0f, 0.0f, 0.0f);					// Normal Pointing Right
	glTexCoord2d(0.0,1.0);
	glVertex3f(-8.0f, 8.0f, 20.0f);					// Top Front
	glTexCoord2d(0.0,0.0);
	glVertex3f(-8.0f, -8.0f, 20.0f);				// Bottom Front
	glTexCoord2d(1.0,0.0);
	glVertex3f(-8.0f, -8.0f, -20.0f);				// Bottom Back
	glTexCoord2d(1.0,1.0);
	glVertex3f(-8.0f, 8.0f, -20.0f);				// Top Back
	glEnd();
	image[5].BuildTexture();
	glBegin(GL_QUADS);
	// Right Wall
	glNormal3f(-1.0f, 0.0f, 0.0f);					// Normal Pointing Left
	glTexCoord2d(0.0,1.0);
	glVertex3f(8.0f, 8.0f, -20.0f);					// Top Back
	glTexCoord2d(0.0,0.0);
	glVertex3f(8.0f, -8.0f, -20.0f);				// Bottom Back
	glTexCoord2d(1.0,0.0);
	glVertex3f(8.0f, -8.0f, 20.0f);					// Bottom Front
	glTexCoord2d(1.0,1.0);
	glVertex3f(8.0f, 8.0f, 20.0f);					// Top Front
	glEnd();										// Done Drawing Quads
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	
	
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);				//指定使用不透明黑色清背景
	
	glShadeModel(GL_SMOOTH);
	q = gluNewQuadric();
	gluQuadricNormals(q, GL_SMOOTH);					// Enable Smooth Normal Generation
	//gluQuadricDrawStyle(q, GLU_FILL);


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);



	glEnable(GL_LIGHT1);// Enable Light1
	glEnable(GL_LIGHTING);// Enable Lighting
	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);// Set Light1 Position
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmb);// Set Light1 Ambience
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDif);// Set Light1 Diffuse
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpc);// Set Light1 Specular

	glMaterialfv(GL_FRONT, GL_AMBIENT, MatAmb);			// Set Material Ambience
	glMaterialfv(GL_FRONT, GL_DIFFUSE, MatDif);			// Set Material Diffuse
	glMaterialfv(GL_FRONT, GL_SPECULAR, MatSpc);		// Set Material Specular
	glMaterialfv(GL_FRONT, GL_SHININESS, MatShn);		// Set Material Shininess

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);


/*	
	obj_ptr[0] = &obj1;
	obj_ptr[0]->scaling(0.5f, 0.5f, 0.5f);
	obj_ptr[1] = &obj2;
	obj_ptr[1]->scaling(0.7f, 0.7f, 0.7f);
	obj_ptr[1]->position[0] = -1.5;
	obj_ptr[1]->position[2] = 1.5;
	obj_ptr[2] = &obj3;
	obj_ptr[2]->scaling(0.9f, 0.9f, 0.9f);
	obj_ptr[2]->position[0] = 1.5;
	obj_ptr[2]->position[2] = -1.5;
	obj_counter=0;
	*/

	image=(image_class*) operator new(6*sizeof(image_class));
	new (image+0) image_class("../Data/bottom.bmp");
	new (image+1) image_class("../Data/ceiling.bmp");
	new (image+2) image_class("../Data/front.bmp");
	new (image+3) image_class("../Data/back.bmp");
	new (image+4) image_class("../Data/left.bmp");
	new (image+5) image_class("../Data/right.bmp");

	//***************************************************
	ReadObject("../Data/Object.txt", &obj);
	ReadObject("../Data/Object1.txt", &obj1);
	ReadObject("../Data/Object2.txt", &obj2);
	/*
	if (!ReadObject("../Data/Object2.txt", &obj))			
	{
		printf("GGGGGGGGGGGGGGGGGGGGGG\n");									
	}
	*/
	SetConnectivity(&obj);
	SetConnectivity(&obj1);
	SetConnectivity(&obj2);

	for (unsigned int i = 0; i<obj.nPlanes; i++)			
		CalcPlane(obj, &(obj.planes[i]));				
	for (unsigned int i = 0; i<obj1.nPlanes; i++)
		CalcPlane(obj1, &(obj1.planes[i]));
	for (unsigned int i = 0; i<obj2.nPlanes; i++)
		CalcPlane(obj2, &(obj2.planes[i]));

}

void idle()
{
	//printf("idle\n");
	glutPostRedisplay();
}
//---------負責視窗及繪圖內容的比例---------
void WindowSize(int w, int h)
{
	printf("目前視窗大小為%d X %d\n", w, h);
	glViewport(0, 0, w, h);							//當視窗長寬改變時，畫面也跟著變
	glMatrixMode(GL_PROJECTION);					//選擇投影矩陣模式
	glLoadIdentity();
	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 0.001f, 100.0f);
	//gluLookAt(CamaraPos[0], CamaraPos[1], CamaraPos[2], 0, 0, 0, 0, 1, 0);				//螢幕鏡頭的座標及方向
	glMatrixMode(GL_MODELVIEW);						//model模式最常用，沒事就切回這個模式
	glLoadIdentity();
}

//---------獲取鍵盤輸入---------
void Keyboard(unsigned char key, int x, int y)
{
	printf("你所按按鍵的碼是%x\t此時視窗內的滑鼠座標是(%d,%d)\n", key, x, y);
	if(key==0x62) shadow_mode=!shadow_mode;//b 開關陰影
	if(key==0x76)//v 選擇物體
	{
		select_obj = (select_obj + 1) % 3;
		printf("Selecting Object: %d\n", select_obj + 1);
		//obj_counter=(obj_counter+1)%3;
		//printf("Selecting Object: %d\n", obj_counter+1);
	}
	if(key==0x61) LightPos[0]-=0.1f;//a
	if(key==0x64) LightPos[0]+=0.1f;//d
	if(key==0x77) LightPos[1]+=0.1f;//w
	if(key==0x73) LightPos[1]-=0.1f;//s
	if(key==0x71) LightPos[2]-=0.1f;//q
	if(key==0x65) LightPos[2]+=0.1f;//e

	/*

	if(key==0x6a) obj_ptr[obj_counter]->rotate_speed[0]-=0.2f;//j
	if(key==0x6c) obj_ptr[obj_counter]->rotate_speed[0]+=0.2f;//l
	if(key==0x69) obj_ptr[obj_counter]->rotate_speed[1]+=0.2f;//i
	if(key==0x6b) obj_ptr[obj_counter]->rotate_speed[1]-=0.2f;//k
	if(key==0x75) obj_ptr[obj_counter]->rotate_speed[2]-=0.2f;//u
	if(key==0x6f) obj_ptr[obj_counter]->rotate_speed[2]+=0.2f;//o

	if(key==0x34) {obj_ptr[obj_counter]->position[0]-=0.2f;}//numpad4
	if(key==0x36) {obj_ptr[obj_counter]->position[0]+=0.2f;}//numpad6
	if(key==0x35) {obj_ptr[obj_counter]->position[1]-=0.2f;}//numpad5
	if(key==0x38) {obj_ptr[obj_counter]->position[1]+=0.2f;}//numpad8
	if(key==0x37) {obj_ptr[obj_counter]->position[2]-=0.2f;}//numpad7
	if(key==0x39) {obj_ptr[obj_counter]->position[2]+=0.2f;}//numpad9

	*/

	

	// Adjust Object's Position
	if (select_obj==0) 
	{
		if (key == 0x36) ObjPos[0] += 0.05f;			// 'Numpad6' Move Object Right
		if (key == 0x34) ObjPos[0] -= 0.05f;			// 'Numpad4' Move Object Left
		if (key == 0x38) ObjPos[1] += 0.05f;			// 'Numpad8' Move Object Up
		if (key == 0x35) ObjPos[1] -= 0.05f;			// 'Numpad5' Move Object Down
		if (key == 0x39) ObjPos[2] += 0.05f;			// 'Numpad9' Move Object Toward Viewer
		if (key == 0x37) ObjPos[2] -= 0.05f;			// 'Numpad7' Move Object Away From Viewer

		if (key == 0x6a) xspeed -= 0.2f;//j
		if (key == 0x6c) xspeed += 0.2f;//l
		if (key == 0x69) yspeed += 0.2f;//i
		if (key == 0x6b) yspeed -= 0.2f;//k
		if (key == 0x75) zspeed -= 0.2f;//u
		if (key == 0x6f) zspeed += 0.2f;//o
	}
	else if (select_obj == 1)
	{
		if (key == 0x36) Obj1Pos[0] += 0.05f;			// 'Numpad6' Move Object Right
		if (key == 0x34) Obj1Pos[0] -= 0.05f;			// 'Numpad4' Move Object Left
		if (key == 0x38) Obj1Pos[1] += 0.05f;			// 'Numpad8' Move Object Up
		if (key == 0x35) Obj1Pos[1] -= 0.05f;			// 'Numpad5' Move Object Down
		if (key == 0x39) Obj1Pos[2] += 0.05f;			// 'Numpad9' Move Object Toward Viewer
		if (key == 0x37) Obj1Pos[2] -= 0.05f;			// 'Numpad7' Move Object Away From Viewer

		if (key == 0x6a) xspeed1 -= 0.2f;//j
		if (key == 0x6c) xspeed1 += 0.2f;//l
		if (key == 0x69) yspeed1 += 0.2f;//i
		if (key == 0x6b) yspeed1 -= 0.2f;//k
		if (key == 0x75) zspeed1 -= 0.2f;//u
		if (key == 0x6f) zspeed1 += 0.2f;//o
	}
	else
	{
		if (key == 0x36) Obj2Pos[0] += 0.05f;			// 'Numpad6' Move Object Right
		if (key == 0x34) Obj2Pos[0] -= 0.05f;			// 'Numpad4' Move Object Left
		if (key == 0x38) Obj2Pos[1] += 0.05f;			// 'Numpad8' Move Object Up
		if (key == 0x35) Obj2Pos[1] -= 0.05f;			// 'Numpad5' Move Object Down
		if (key == 0x39) Obj2Pos[2] += 0.05f;			// 'Numpad9' Move Object Toward Viewer
		if (key == 0x37) Obj2Pos[2] -= 0.05f;			// 'Numpad7' Move Object Away From Viewer

		if (key == 0x6a) xspeed2 -= 0.2f;//j
		if (key == 0x6c) xspeed2 += 0.2f;//l
		if (key == 0x69) yspeed2 += 0.2f;//i
		if (key == 0x6b) yspeed2 -= 0.2f;//k
		if (key == 0x75) zspeed2 -= 0.2f;//u
		if (key == 0x6f) zspeed2 += 0.2f;//o
	
	}


	if(key==0x20) std::cout<<"light pos:"<<LightPos[0]<<'\t'<<LightPos[1]<<'\t'<<LightPos[2]<<'\n';//space
	
}

//---------描繪畫面---------
void Display(void)
{
	
	GLmatrix16f Minv;
	GLmatrix16f Minv1;
	GLmatrix16f Minv2;
	GLvector4f wlp, lp;
	GLvector4f wlp1, lp1;
	GLvector4f wlp2, lp2;

	// Clear Color Buffer, Depth Buffer, Stencil Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glLoadIdentity();									// Reset Modelview Matrix
	glTranslatef(0.0f, 0.0f, -20.0f);					// Zoom Into Screen 20 Units
	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);		// Position Light1
	glTranslated(BarrierPos[0], BarrierPos[1], BarrierPos[2]);
	glutSolidSphere(1.0, 16, 16);//小球，只是個障礙物

	//**********************************************************************************													
	glLoadIdentity();									// Reset Matrix
					
	glRotatef(-xrot, 1.0f, 0.0f, 0.0f);	
	glRotatef(-yrot, 0.0f, 1.0f, 0.0f);
	glRotatef(-zrot, 0.0f, 0.0f, 1.0f);

	glRotatef(-xrot1, 1.0f, 0.0f, 0.0f);				
	glRotatef(-yrot1, 0.0f, 1.0f, 0.0f);					
	glRotatef(-zrot1, 0.0f, 0.0f, 1.0f);

	glRotatef(-xrot2, 1.0f, 0.0f, 0.0f);					
	glRotatef(-yrot2, 0.0f, 1.0f, 0.0f);					
	glRotatef(-zrot2, 0.0f, 0.0f, 1.0f);

	glGetFloatv(GL_MODELVIEW_MATRIX, Minv);				// Retrieve ModelView Matrix (Stores In Minv)
	glGetFloatv(GL_MODELVIEW_MATRIX, Minv1);
	glGetFloatv(GL_MODELVIEW_MATRIX, Minv2);


	lp[0] = LightPos[0];								
	lp[1] = LightPos[1];								
	lp[2] = LightPos[2];								
	lp[3] = LightPos[3];								

	lp1[0] = LightPos[0];								
	lp1[1] = LightPos[1];								
	lp1[2] = LightPos[2];							
	lp1[3] = LightPos[3];							

	lp2[0] = LightPos[0];								
	lp2[1] = LightPos[1];								
	lp2[2] = LightPos[2];								
	lp2[3] = LightPos[3];								
	
	VMatMult(Minv, lp);									// We Store Rotated Light Vector In 'lp' Array
	VMatMult(Minv1, lp1);
	VMatMult(Minv2, lp2);
	
	
	glTranslatef(-ObjPos[0], -ObjPos[1], -ObjPos[2]);	
	glTranslatef(-Obj1Pos[0], -Obj1Pos[1], -Obj1Pos[2]);
	glTranslatef(-Obj2Pos[0], -Obj2Pos[1], -Obj2Pos[2]);
	
	glGetFloatv(GL_MODELVIEW_MATRIX, Minv);				// Retrieve ModelView Matrix From Minv
	glGetFloatv(GL_MODELVIEW_MATRIX, Minv1);
	glGetFloatv(GL_MODELVIEW_MATRIX, Minv2);


	wlp[0] = 0.0f;										
	wlp[1] = 0.0f;										
	wlp[2] = 0.0f;										
	wlp[3] = 1.0f;

	wlp1[0] = 0.0f;										
	wlp1[1] = 0.0f;									
	wlp1[2] = 0.0f;										
	wlp1[3] = 1.0f;

	wlp2[0] = 0.0f;								
	wlp2[1] = 0.0f;									
	wlp2[2] = 0.0f;										
	wlp2[3] = 1.0f;
	
	VMatMult(Minv, wlp);							
	VMatMult(Minv1, wlp1);
	VMatMult(Minv2, wlp2);
	
	
	lp[0] += wlp[0];									
	lp[1] += wlp[1];									
	lp[2] += wlp[2];								

	lp1[0] += wlp1[0];									
	lp1[1] += wlp1[1];									
	lp1[2] += wlp1[2];								

	lp2[0] += wlp2[0];									
	lp2[1] += wlp2[1];									
	lp2[2] += wlp2[2];							



	//**************************************************************************************************
	//glColor4f(0.7f, 0.4f, 0.0f, 1.0f);					// Set Color To An Orange
	glLoadIdentity();									// Reset Modelview Matrix
	glTranslatef(0.0f, 0.0f, -20.0f);					// Zoom Into The Screen 20 Units
	DrawGLRoom();										// Draw The Room
	
	glLoadIdentity();
	glTranslatef(ObjPos[0], ObjPos[1], ObjPos[2]);		
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);					
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);
	glScalef(0.5, 0.5, 0.5); 
	
	DrawGLObject(obj);									
	if (shadow_mode)
		CastShadow(&obj, lp);								
	
	glLoadIdentity();
	glTranslatef(Obj2Pos[0], Obj2Pos[1], Obj2Pos[2]);
	glRotatef(xrot2, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot2, 0.0f, 1.0f, 0.0f);
	glRotatef(zrot2, 0.0f, 0.0f, 1.0f);
	glScalef(0.7, 0.7, 0.7);
	DrawGLObject(obj2);
	if (shadow_mode)
		CastShadow(&obj2, lp2);

	glLoadIdentity();
	glTranslatef(Obj1Pos[0], Obj1Pos[1], Obj1Pos[2]);
	glRotatef(xrot1, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot1, 0.0f, 1.0f, 0.0f);
	glRotatef(zrot1, 0.0f, 0.0f, 1.0f);
	glScalef(0.9, 0.9, 0.9);
	DrawGLObject(obj1);
	if (shadow_mode)
		CastShadow(&obj1, lp1);

	//glLoadIdentity();
	
	

	//glColor4f(0.0f, 0.6f, 1.0f, 1.0f);					
	glDisable(GL_LIGHTING);								
	glDepthMask(GL_FALSE);								
	glTranslatef(lp[0], lp[1], lp[2]);					

	//glColor4f(0.0f, 0.6f, 1.0f, 1.0f);					
	glDisable(GL_LIGHTING);								
	glDepthMask(GL_FALSE);								
	glTranslatef(lp1[0], lp1[1], lp1[2]);					

	//glColor4f(0.0f, 0.6f, 1.0f, 1.0f);			
	glDisable(GL_LIGHTING);								
	glDepthMask(GL_FALSE);								
	glTranslatef(lp2[0], lp2[1], lp2[2]);					


	
		
	//單純讓使用者知道光源在哪
	glLoadIdentity();//重置，畫光
	glTranslatef(LightPos[0], LightPos[1], LightPos[2]);
	glColor4f(0.0f, 0.6f, 1.0f, 1.0f);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	
	gluSphere(q, 0.1f, 16, 8);//光
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);
	
	//******************************************************************************
	xrot += xspeed;										
	yrot += yspeed;
	zrot += zspeed;
	xrot1 += xspeed1;
	yrot1 += yspeed1;
	zrot1 += zspeed1;
	xrot2 += xspeed2;
	yrot2 += yspeed2;
	zrot2 += zspeed2;

	//glFlush();
	glutSwapBuffers();								//更新畫面
}

int main()
{
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);		//描繪模式使用雙緩衝區以及紅綠藍Alpha顏色順序
	glutInitWindowSize(600, 600);					//視窗長寬
	glutInitWindowPosition(300, 200);				//視窗左上角的位置
	glutCreateWindow("HW2_shadow_0656151");		//建立視窗並打上標題
	init();
	//下面三個與Callback函式有關
	glutReshapeFunc(WindowSize);					//當視窗改變大小時會獲取新的視窗長寬
	glutKeyboardFunc(Keyboard);						//獲取鍵盤輸入訊息
	//glutSpecialFunc(SpecialInput);//可以用來得到上下左右等特殊鍵，不過用不到
	glutDisplayFunc(Display);						//負責描繪
	glutIdleFunc(idle);
	glutMainLoop();									//進入主迴圈

	for(int i=0;i<6;i++) image[i].~image_class();//叫的土地要清掉
	operator delete(image);

	return 0;
}