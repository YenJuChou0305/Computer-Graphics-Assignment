/*HW1-A Simple Solar System
in this homework, you need to modify:
init() function, for texture building.
we provide code that read 24-bit bmp into a char* buffer(inside struct Image)

Display() function, for drawing required scene

and you also need to find the proper place(outside of init func.) to put the setting function of light source position
so that the light source truly lies at world coordinate (0,0,0), where the sun is located
**modifying LightPos[] is forbidden**(you can modify it to see the diffrence, but it should be (0,0,0,1) in your final code)

You can define your own function/datatype for drawing certain object or perform certain transformation...
*/
#include "../GL/glut.h"
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

//IMPORTANT data type for image texture, you need to acesse its member for texture building
struct Image 
{
	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
};
typedef struct Image Image;

//number of textures desired, you may want to change it to get bonus point
#define TEXTURE_NUM 7
//directories of image files
char* texture_name[TEXTURE_NUM] = 
{
	"../Resource/sun.bmp",
	"../Resource/mercury.bmp",
	"../Resource/earth.bmp",
	"../Resource/moon.bmp",
	"../Resource/jupiter.bmp",
	"../Resource/europa.bmp",
	"../Resource/checker.bmp",
	//you may add additional image files here
};
//texture id array
GLuint texture[TEXTURE_NUM];

#define checkImageWidth 128
#define checkImageHeight 128
GLubyte checkImage[checkImageWidth][checkImageHeight][3];

void makeCheckImage(void) 
{
	int i,  j, c;

	for (i = 0; i < checkImageWidth; i++) 
		for (j = 0; j < checkImageHeight; j++) 
		{
			c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;

			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
		}
}


//time parameter for helping coordinate your animation, you may utilize it to help perform animation
#define deltaTime 100
double time = 0.0;

//light sorce parameter, no need to modify
//actually, modification of these parameter is forbidden in this homework

float LightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };//Light position
float LightAmb[] = { 0.0f, 0.0f, 0.0f, 1.0f };//Ambient Light Values
float LightDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };//Diffuse Light Values
float LightSpc[] = { 1.0f, 1.0f, 1.0f, 1.0f };//Specular Light Values



//24-bit bmp loading function, no need to modify it
int ImageLoad(char *filename, Image *image) 
{
	FILE *file;
	unsigned long size; // size of the image in bytes.
	unsigned long i; // standard counter.
	unsigned short int planes; // number of planes in image (must be 1)
	unsigned short int bpp; // number of bits per pixel (must be 24)
	char temp; // temporary color storage for bgr-rgb conversion.
			   // make sure the file is there.
	if ((file = fopen(filename, "rb")) == NULL) 
	{
		printf("File Not Found : %s\n", filename);
		return 0;
	}

	// seek through the bmp header, up to the width/height:
	fseek(file, 18, SEEK_CUR);
	// read the width
	if ((i = fread(&image->sizeX, 4, 1, file)) != 1) 
	{
		printf("Error reading width from %s.\n", filename);
		return 0;
	}

	//printf("Width of %s: %lu\n", filename, image->sizeX);
	// read the height
	if ((i = fread(&image->sizeY, 4, 1, file)) != 1) 
	{
		printf("Error reading height from %s.\n", filename);
		return 0;
	}
	//printf("Height of %s: %lu\n", filename, image->sizeY);
	// calculate the size (assuming 24 bits or 3 bytes per pixel).
	size = image->sizeX * image->sizeY * 3;
	// read the planes
	if ((fread(&planes, 2, 1, file)) != 1) 
	{
		printf("Error reading planes from %s.\n", filename);
		return 0;
	}
	if (planes != 1) 
	{
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return 0;
	}
	// read the bitsperpixel
	if ((i = fread(&bpp, 2, 1, file)) != 1) 
	{
		printf("Error reading bpp from %s.\n", filename);
		return 0;
	}
	if (bpp != 24) 
	{
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return 0;
	}
	// seek past the rest of the bitmap header.
	fseek(file, 24, SEEK_CUR);
	// read the data.
	image->data = (char *)malloc(size);
	if (image->data == NULL) 
	{
		printf("Error allocating memory for color-corrected image data");
		return 0;
	}
	if ((i = fread(image->data, size, 1, file)) != 1) 
	{
		printf("Error reading image data from %s.\n", filename);
		return 0;
	}
	for (i = 0; i<size; i += 3) 
	{ // reverse all of the colors. (bgr -> rgb)
		temp = image->data[i];
		image->data[i] = image->data[i + 2];
		image->data[i + 2] = temp;
	}
	// we're done.
	return 1;
}

//memory allocation and file reading for an Image datatype, no need to modify it
Image * loadTexture(char *filename) 
{
	Image *image;
	// allocate space for texture
	image = (Image *)malloc(sizeof(Image));
	if (image == NULL) 
	{
		printf("Error allocating space for image");
		getchar();
		exit(0);
	}
	if (!ImageLoad(filename, image)) 
	{
		getchar();
		exit(1);
	}
	return image;
}

void around_E()
{
	glRotatef(10 * time, 0, 1, 0);
	glTranslatef(10, 0, 0.0);

	//glRotatef(100 * time, 0, 1, 0);
	glTranslatef(1.5, 0.0, 0.0);
	//glRotatef(100 * time, 0, 1, 0);

	glScalef(0.1, 0.1, 0.1);
	glRotatef(45, -1, -1, 0);
}

void around_J()
{
	glRotatef(15 * time, 0, 1, 0);
	glTranslatef(15, 0, 0.0);

	//glRotatef(100 * time, 0, 1, 0);
	glTranslatef(1.5, 0.0, 0.0);
	//glRotatef(100 * time, 0, 1, 0);

	glScalef(0.1, 0.1, 0.1);
	glRotatef(45, -1, -1, 0);
}

//callback function for drawing a frame
void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_TEXTURE_2D);

	


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0, 20, 0, 0, 0, 0, 1, 0); //set the view part of modelview matrix
	
	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);//Set Light1 Position, this setting function should be at another place


	//glDisable(GL_TEXTURE_2D);//when you draw something without texture, be sure to disable GL_TEXTURE_2D

	glDisable(GL_LIGHTING);
	 // artificial satellite around earth
	glPushMatrix();
		
		around_E();

		glTranslatef(2, 0, 1);
		glColor3f(1, 1, 0);	
		glScalef(3,1,0.5);

		glutSolidCube(1);
	glPopMatrix();
	
	glPushMatrix();
	
		around_E();

		glTranslatef(-2, 0, 1);
		glColor3f(1, 1, 0);
		glScalef(3, 1, 0.5);

		glutSolidCube(1);
	glPopMatrix();
	
	glPushMatrix();
		
		around_E();

		glTranslatef(0, 0, -1);
		glColor3f(1, 1, 0);

		glutSolidCone(3, 1, 100, 100);
	glPopMatrix();
	
	GLUquadricObj* quad = gluNewQuadric(); 
	glPushMatrix();

		around_E();

		glTranslatef(0, 0, 0);
		glColor3f(1, 1, 0.0);

		gluCylinder(quad, 1, 1, 2, 100, 100);
	glPopMatrix();

	GLUquadricObj* quad2 = gluNewQuadric();
	glPushMatrix();

		around_E();

		glTranslatef(0, 0, 2);
		glColor3f(1, 1, 0.0);

		gluDisk(quad2, 0, 1, 100, 3);
	glPopMatrix();
	
	
	// artificial satellite around jupiter
	glPushMatrix();

		around_J();

		glTranslatef(2, 0, 1);
		glColor3f(0, 1, 0);
		glScalef(3, 1, 0.5);

		glutSolidCube(1);
		glPopMatrix();

	glPushMatrix();

		around_J();

		glTranslatef(-2, 0, 1);
		glColor3f(0, 1, 0);
		glScalef(3, 1, 0.5);

		glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();

		around_J();

		glTranslatef(0, 0, -1);
		glColor3f(0, 1, 0);

		glutSolidCone(3, 1, 100, 100);
	glPopMatrix();

		GLUquadricObj* quad3 = gluNewQuadric();
		glPushMatrix();

		around_J();

		glTranslatef(0, 0, 0);
		glColor3f(0, 1, 0.0);

		gluCylinder(quad3, 1, 1, 2, 100, 100);
	glPopMatrix();

	GLUquadricObj* quad4 = gluNewQuadric();
	glPushMatrix();

		around_J();

		glTranslatef(0, 0, 2);
		glColor3f(0, 1, 0);

		gluDisk(quad4, 0, 1, 100, 3);
		glColor3f(1, 1, 1);
	glPopMatrix();

	glEnable(GL_LIGHTING);//Enable Lighting

	
	//glEnable(GL_TEXTURE_2D);
	 // sun


	glPushMatrix();
	//glTranslatef(0.0, 0.0, 0.0);
	glRotatef(1*time, 0, 1, 0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	GLUquadricObj* sun = gluNewQuadric();
	gluQuadricTexture(sun, GLU_TRUE);
	
	glDisable(GL_LIGHTING);

	gluSphere(sun, 1, 100, 100);

	glEnable(GL_LIGHTING);//Enable Lighting

	glPopMatrix();


	// earth

	glPushMatrix();

	glRotatef(10 * time, 0, 1, 0);
	glTranslatef(10, 0, 0.0);
	glRotatef(40 * time, 0, 1, 1);
	
	glScalef(1.3, 1, 1.3);

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	GLUquadricObj* E = gluNewQuadric();
	gluQuadricTexture(E, GLU_TRUE);
	gluSphere(E, 1, 100, 100);

	glPopMatrix();
	

	// moon	
	glPushMatrix();

	glRotatef(10 * time, 0, 1, 0);
	glTranslatef(10, 0, 0.0);

	glRotatef(40 * time, 0, 1, 1);
	glTranslatef(2, 0.0, 0.0);
	glRotatef(40*time, 0, 1, 1);

	glScalef(0.5, 0.5, 0.5);
	
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	GLUquadricObj* moon = gluNewQuadric();
	gluQuadricTexture(moon, GLU_TRUE);
	gluSphere(moon, 0.5, 100, 100);
	glPopMatrix();


	

	// mercury

	glPushMatrix();

	glRotatef(20* time, 0, 2, 1);
	glTranslatef(3, 0, 0.0);
	glRotatef(50 * time, 0, 1, 0);

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	GLUquadricObj* mercury = gluNewQuadric();
	gluQuadricTexture(mercury, GLU_TRUE);
	gluSphere(mercury, 0.5, 100, 100);

	glPopMatrix();


	// jupiter
	
	glPushMatrix();

	glRotatef(15 * time, 0, 1, 0);
	glTranslatef(15, 0, 0.0);
	glRotatef(40 * time, 1, 1, 0);

	glBindTexture(GL_TEXTURE_2D, texture[4]);
	GLUquadricObj* jupiter = gluNewQuadric();
	gluQuadricTexture(jupiter, GLU_TRUE);
	
	gluSphere(jupiter, 1, 100, 100);

	glPopMatrix();

	// europa
	
	glPushMatrix();

	glRotatef(15 * time, 0, 1, 0);
	glTranslatef(15, 0, 0.0);

	glRotatef(40 * time, 0, 1, 1);
	glTranslatef(2, 0.0, 0.0);
	glRotatef(40 * time, 0, 1, 1);

	glBindTexture(GL_TEXTURE_2D, texture[5]);
	GLUquadricObj* europa = gluNewQuadric();
	gluQuadricTexture(europa, GLU_TRUE);
	gluSphere(europa, 0.5, 100, 100);

	glPopMatrix();

	// C

	glPushMatrix();

	glRotatef(90 * time, 0, 1, 1);
	glTranslatef(12, 0, 0.0);
	//glRotatef(90 * time, 0, 1, 0);

	glScalef(0.5, 0.5, 0.5);
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	GLUquadricObj* C = gluNewQuadric();
	gluQuadricTexture(C, GLU_TRUE);
	gluSphere(C, 0.75, 100, 100);

	glPopMatrix();
	
	

	glEnable(GL_TEXTURE_2D);
	


	//glEnd();



	
	glutSwapBuffers();//swap the drawn buffer to the window
}

//callback funtion as a timer, no need to modify it
void Tick(int id)
{
	double d = deltaTime / 1000.0;
	time += d;
	glutPostRedisplay();
	glutTimerFunc(deltaTime, Tick, 0); // 100ms for time step size
}

//callback function when the window size is changed, no need to modify it
void WindowSize(int w, int h)
{
	glViewport(0, 0, w, h);							//changing the buffer size the same to the window size
	glMatrixMode(GL_PROJECTION);					//choose the projection matrix
	glLoadIdentity();
	gluPerspective(60.0, (double)w /(double)h, 1.0, 1000.0);//set the projection matrix as perspective mode
	glMatrixMode(GL_MODELVIEW);						//it is suggested that modelview matrix is chosen if nothing specific being performed
	glLoadIdentity();
}

//initialization for parameters of this program, you must perform something here
void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);//set what color should be used when we clean the color buffer
	
	
	
	glEnable(GL_LIGHT1);//Enable Light1
	glEnable(GL_LIGHTING);//Enable Lighting
	//***********
	//glLightfv(GL_LIGHT1, GL_POSITION, LightPos);//Set Light1 Position, this setting function should be at another place
	//***********
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmb);//Set Light1 Ambience
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDif);//Set Light1 Diffuse
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpc);//Set Light1 Specular
	//since a light source is also an object in the 3D scene, we set its position in the display function after gluLookAt()
	//you should know that straight texture mapping(without modifying shader) may not have shading effect
	//you need to tweak certain parameter(when building texture) to obtain a lit and textured object
	glShadeModel(GL_SMOOTH);//shading model

	
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);//you can choose which part of lighting model should be modified by texture mapping
	glEnable(GL_COLOR_MATERIAL);//enable this parameter to use glColor() as material of lighting model

	//please load all the textures here
	//use Image* loadTexture(file_directory) function to obtain char* data and size info.
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);


	Image *image0 = loadTexture(texture_name[0]); //sun
	Image *image1 = loadTexture(texture_name[1]); //mercury
	Image *image2 = loadTexture(texture_name[2]); //earth 
	Image *image3 = loadTexture(texture_name[3]); //moon
	Image *image4 = loadTexture(texture_name[4]); //jupiter
	Image *image5 = loadTexture(texture_name[5]); //europa
	Image *image6 = loadTexture(texture_name[6]); 

	// Create Texture

	glGenTextures(6, texture);

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image0->sizeX, image0->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image0->data);

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);


	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->sizeX, image2->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image2->data);

	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image3->sizeX, image3->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image3->data);


	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image4->sizeX, image4->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image4->data);
	
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image5->sizeX, image5->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image5->data);
	

	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image6->sizeX, image6->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image6->data);
	


}


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);//glut function for simplifying OpenGL initialization
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//demanding: double-framed buffer | RGB colorbuffer | depth buffer
	glutInitWindowPosition(100, 50);//set the initial window position
	glutInitWindowSize(800, 600);//set the initial window size
	//**************************
	glutCreateWindow("CG_HW1_0656151");//IMPORTANT!!­«­n!! Create the window and set its title, please replace 0123456 with your own student ID
	//**************************
	
	glutDisplayFunc(Display);//callback funtion for drawing a frame
	glutReshapeFunc(WindowSize);//callback function when the window size is changed
	glutTimerFunc(deltaTime, Tick, 0);//timer function
	//you may want to write your own callback funtion for other events(not demanded nor forbidden)
	init();//self-defined initialization function for the elegance of your code

	glutMainLoop();
	return 0;
}