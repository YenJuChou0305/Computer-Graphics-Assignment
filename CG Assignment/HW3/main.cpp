/*
CG Homework3 - Bump Mapping
Objective - learning GLSL, glm model datatype(for .obj file) and bump mapping
In this homework, you should load "Ball.obj" and normal map "NormalMap.ppm" with glm.c(done by TA)
and render the object with color texure and normal mapping with Phong shading(and Phong lighting model of course).
Please focus on the part with comment like "you may need to do somting here".
If you don't know how to access vertices information of the model,
I suggest you refer to glm.c for _GLMmodel structure and glm.h for glmReadOBJ() and glmDraw() function.
And the infomation printed by print_model_info(model); of glm_helper.h helps as well!
Finally, please pay attention to the datatype of the variable you use(might be a ID list or value array)
Good luck!
*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /*for function: offsetof */
#include <math.h>
#include <string.h>
#include "../GL/glew.h"
#include "../GL/glut.h""
#include "../shader_lib/shader.h"
#include "glm/glm.h"

extern "C"
{
	#include "glm_helper.h"
}

/*you may need to do somting here
you may use the following struct type to perform your single VBO method,
or you can define/declare multiple VBOs for VAO method.
Please feel free to modify it*/
struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
	GLfloat tangent[3];
	GLfloat bitangent[3];
};
typedef struct Vertex Vertex;

Vertex *vertices;
GLuint vbo_id;
GLuint program;

GLuint Position_VBO, Norm_VBO, Texcoord_VBO;
GLuint Tangent_VBO, Bitangent_VBO;

int bump = 1;



//no need to modify the following function declarations and gloabal variables
void init(void);
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void idle(void);
void draw_light_bulb(void);
void camera_light_ball_move();
GLuint load_normal_map(char* name);
namespace
{
	char *obj_file_dir = "../Resources/Ball.obj";
	char *normal_map_dir = "../Resources/NormalMap.ppm";
	GLfloat light_rad = 0.05;//radius of the light bulb
	float eyet = 0.0;//theta in degree
	float eyep = 90.0;//phi in degree
	bool mleft = false;
	bool mright = false;
	bool mmiddle = false;
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool lforward = false;
	bool lbackward = false;
	bool lleft = false;
	bool lright = false;
	bool lup = false;
	bool ldown = false;
	bool bforward = false;
	bool bbackward = false;
	bool bleft = false;
	bool bright = false;
	bool bup = false;
	bool bdown = false;
	bool bx = false;
	bool by = false;
	bool bz = false;
	bool brx = false;
	bool bry = false;
	bool brz = false;
	int mousex = 0;
	int mousey = 0;
}

//you can modify the moving/rotating speed if it's too fast/slow for you
const float speed = 0.005; //camera/light/ball moving speed
const float rotation_speed = 0.05; //ball rotation speed
//you may need to use some of the following variables in your program 
GLuint normalTextureID;//TA has already loaded the normal texture for you
GLMmodel *model;//TA has already loaded the model for you(!but you still need to convert it to VBO(s)!)
float eyex = 0.0;
float eyey = 0.0;
float eyez = 3.0;
GLfloat light_pos[] = { 1, 1, 1 };
GLfloat ball_pos[] = { 0.0, 0.0, 0.0 };
GLfloat ball_rot[] = { 0.0, 0.0, 0.0 };
GLfloat normalWid, normalHei;

GLfloat light_pos_local[] = { 0, 0, 0 };

char *textFileRead(char *fn) 
{

	FILE *fp;
	char *content = NULL;

	int count = 0;

	if (fn != NULL) {
		fp = fopen(fn, "rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//remember to replace "YourStudentID" with your own student ID
	glutCreateWindow("CG_HW3_0656151");
	glutReshapeWindow(512, 512);

	glewInit();

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();

	glmDelete(model);
	return 0;
}

void init(void) 
{

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glEnable(GL_CULL_FACE);
	model = glmReadOBJ(obj_file_dir);
	normalTextureID = load_normal_map(normal_map_dir);
	glmUnitize(model);
	glmFacetNormals(model);
	glmVertexNormals(model, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(model);

	//you may need to do somting here(create shaders/program(s) and create vbo(s)/vao from GLMmodel model)

	//**********************************************************************************************************

	
	GLuint vert = createShader("Shaders/bump.vert", "vertex");
	GLuint frag = createShader("Shaders/bump.frag", "fragment");
	program = createProgram(vert, frag);
	
	//***************************************************************************************************

	GLMgroup *group = model->groups;
	GLfloat *vertices, *vtextures, *vnormals;
	GLfloat *tangent, *bitangent;


	vertices = (GLfloat*)malloc(sizeof(GLfloat) * model->numtriangles * 9);
	vnormals = (GLfloat*)malloc(sizeof(GLfloat) * model->numtriangles * 9);
	vtextures = (GLfloat*)malloc(sizeof(GLfloat) * model->numtriangles * 6);

	tangent = (GLfloat*)malloc(sizeof(GLfloat) * model->numtriangles * 9);
	bitangent = (GLfloat*)malloc(sizeof(GLfloat) * model->numtriangles * 9);

	GLfloat deltaPos1_x, deltaPos1_y, deltaPos1_z;
	GLfloat deltaPos2_x, deltaPos2_y, deltaPos2_z;
	GLfloat deltaUV1_x, deltaUV1_y;
	GLfloat deltaUV2_x, deltaUV2_y;
	GLfloat tangent_x, tangent_y, tangent_z;
	GLfloat bitangent_x, bitangent_y, bitangent_z;


	while (group)
	{
		for (unsigned int i = 0; i<group->numtriangles; i++)
		{
			// triangle index
			int triangleID = group->triangles[i];

			// the index of each vertex
			int indv1 = model->triangles[triangleID].vindices[0];
			int indv2 = model->triangles[triangleID].vindices[1];
			int indv3 = model->triangles[triangleID].vindices[2];

			// vertices
			GLfloat vx, vy, vz;
			vx = model->vertices[indv1 * 3];
			vy = model->vertices[indv1 * 3 + 1];
			vz = model->vertices[indv1 * 3 + 2];
			//printf("%f %f %f\n", vx, vy, vz);
			vertices[i * 9 + 0] = vx;
			vertices[i * 9 + 1] = vy;
			vertices[i * 9 + 2] = vz;
			vx = model->vertices[indv2 * 3];
			vy = model->vertices[indv2 * 3 + 1];
			vz = model->vertices[indv2 * 3 + 2];
			//printf("%f %f %f\n", vx, vy, vz);
			vertices[i * 9 + 3] = vx;
			vertices[i * 9 + 4] = vy;
			vertices[i * 9 + 5] = vz;
			vx = model->vertices[indv3 * 3];
			vy = model->vertices[indv3 * 3 + 1];
			vz = model->vertices[indv3 * 3 + 2];
			//printf("%f %f %f\n", vx, vy, vz);
			vertices[i * 9 + 6] = vx;
			vertices[i * 9 + 7] = vy;
			vertices[i * 9 + 8] = vz;
			//printf("\n");

			//**************************************************************
			deltaPos1_x = vertices[i * 9 + 0] - vertices[i * 9 + 3];
			deltaPos1_y = vertices[i * 9 + 1] - vertices[i * 9 + 4];
			deltaPos1_z = vertices[i * 9 + 2] - vertices[i * 9 + 5];

			deltaPos2_x = vertices[i * 9 + 0] - vertices[i * 9 + 6];
			deltaPos2_y = vertices[i * 9 + 1] - vertices[i * 9 + 7];
			deltaPos2_z = vertices[i * 9 + 2] - vertices[i * 9 + 8];


			// normal

			int indn1 = model->triangles[triangleID].nindices[0];
			int indn2 = model->triangles[triangleID].nindices[1];
			int indn3 = model->triangles[triangleID].nindices[2];	

			GLfloat nx, ny, nz;
			nx = model->normals[indn1 * 3];
			ny = model->normals[indn1 * 3 + 1];
			nz = model->normals[indn1 * 3 + 2];

			vnormals[i * 9 + 0] = nx;
			vnormals[i * 9 + 1] = ny;
			vnormals[i * 9 + 2] = nz;

			nx = model->normals[indn2 * 3];
			ny = model->normals[indn2 * 3 + 1];
			nz = model->normals[indn2 * 3 + 2];

			vnormals[i * 9 + 3] = nx;
			vnormals[i * 9 + 4] = ny;
			vnormals[i * 9 + 5] = nz;

			nx = model->normals[indn3 * 3];
			ny = model->normals[indn3 * 3 + 1];
			nz = model->normals[indn3 * 3 + 2];

			vnormals[i * 9 + 6] = nx;
			vnormals[i * 9 + 7] = ny;
			vnormals[i * 9 + 8] = nz;



			int indt1 = model->triangles[triangleID].tindices[0];
			int indt2 = model->triangles[triangleID].tindices[1];
			int indt3 = model->triangles[triangleID].tindices[2];


			//texture

			GLfloat tx, ty;

			tx = model->texcoords[indt1 * 2];
			ty = model->texcoords[indt1 * 2 + 1];

			vtextures[i * 6 + 0] = tx;
			vtextures[i * 6 + 1] = ty;

			tx = model->texcoords[indt2 * 2];
			ty = model->texcoords[indt2 * 2 + 1];

			vtextures[i * 6 + 2] = tx;
			vtextures[i * 6 + 3] = ty;

			tx = model->texcoords[indt3 * 2];
			ty = model->texcoords[indt3 * 2 + 1];

			vtextures[i * 6 + 4] = tx;
			vtextures[i * 6 + 5] = ty;


			deltaUV1_x = vtextures[i * 6 + 0] - vtextures[i * 6 + 2];
			deltaUV1_y = vtextures[i * 6 + 1] - vtextures[i * 6 + 3];

			deltaUV2_x = vtextures[i * 6 + 0] - vtextures[i * 6 + 4];
			deltaUV2_y = vtextures[i * 6 + 1] - vtextures[i * 6 + 5];


			//*******************************************************************

			float r = 1.0f / (deltaUV1_x * deltaUV2_y - deltaUV1_y * deltaUV2_x);
			//glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
			tangent_x = (deltaPos1_x * deltaUV2_y - deltaPos2_x * deltaUV1_y) * r;
			tangent_y = (deltaPos1_y * deltaUV2_y - deltaPos2_y * deltaUV1_y) * r;
			tangent_z = (deltaPos1_z * deltaUV2_y - deltaPos2_z * deltaUV1_y) * r;
			//glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;
			bitangent_x = (deltaPos2_x * deltaUV1_x - deltaPos1_x * deltaUV2_x) * r;
			bitangent_y = (deltaPos2_y * deltaUV1_x - deltaPos1_y * deltaUV2_x) * r;
			bitangent_z = (deltaPos2_z * deltaUV1_x - deltaPos1_z * deltaUV2_x) * r;

			//tangent
			tangent[i * 9 + 0] = tangent_x;
			tangent[i * 9 + 1] = tangent_y;
			tangent[i * 9 + 2] = tangent_z;

			tangent[i * 9 + 3] = tangent_x;
			tangent[i * 9 + 4] = tangent_y;
			tangent[i * 9 + 5] = tangent_z;

			tangent[i * 9 + 6] = tangent_x;
			tangent[i * 9 + 7] = tangent_y;
			tangent[i * 9 + 8] = tangent_z;

			//bitangent
			bitangent[i * 9 + 0] = bitangent_x;
			bitangent[i * 9 + 1] = bitangent_y;
			bitangent[i * 9 + 2] = bitangent_z;

			bitangent[i * 9 + 3] = bitangent_x;
			bitangent[i * 9 + 4] = bitangent_y;
			bitangent[i * 9 + 5] = bitangent_z;

			bitangent[i * 9 + 6] = bitangent_x;
			bitangent[i * 9 + 7] = bitangent_y;
			bitangent[i * 9 + 8] = bitangent_z;


		}

		group = group->next;
		
	}


	glGenBuffers(1, &Position_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Position_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model->numtriangles * 9, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &Norm_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Norm_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model->numtriangles * 9, vnormals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &Texcoord_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Texcoord_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model->numtriangles * 6, vtextures, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &Tangent_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Tangent_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model->numtriangles * 9, tangent, GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &Bitangent_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Bitangent_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model->numtriangles * 9, bitangent, GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);



}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//you may need to do somting here(declare some local variables you need and maybe load inverse Model matrix here...)
	


	//please try not to modify the following block of code(you can but you are not supposed to)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez,
		eyex+cos(eyet*M_PI/180)*cos(eyep*M_PI / 180), eyey+sin(eyet*M_PI / 180), eyez-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180),
		0.0, 1.0, 0.0);
	draw_light_bulb();


	glPushMatrix();


	
		glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);
		glRotatef(ball_rot[0], 1, 0, 0);
		glRotatef(ball_rot[1], 0, 1, 0);
		glRotatef(ball_rot[2], 0, 0, 1);
		
	//please try not to modify the previous block of code

	//you may need to do something here(pass the uniform variable to shader and render the model)
		//glmDraw(model,GLM_TEXTURE);//please delete this line in your final code! It's just a preview of rendered object

		GLfloat MV[16];
		GLfloat M[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, MV);
		glLoadIdentity();
		glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);
		glRotatef(ball_rot[0], 1, 0, 0);
		glRotatef(ball_rot[1], 0, 1, 0);
		glRotatef(ball_rot[2], 0, 0, 1);
		glGetFloatv(GL_MODELVIEW_MATRIX, M);
		glLoadMatrixf(MV);

		glUseProgram(program);

		//GLfloat MV[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, MV);

		GLfloat P[16];
		glGetFloatv(GL_PROJECTION_MATRIX, P);

		//Texture
		GLuint Tex_loc = glGetUniformLocation(program, "Tex");
		GLuint Normal_Tex_loc = glGetUniformLocation(program, "Normal_Tex");

		GLuint eye_loc = glGetUniformLocation(program, "Eye");
		GLuint light_loc = glGetUniformLocation(program, "Light");
		GLuint MV_loc = glGetUniformLocation(program, "MV");
		GLuint M_loc = glGetUniformLocation(program, "M");
		GLuint P_loc = glGetUniformLocation(program, "P");

		glUniform3f(eye_loc, eyex, eyey, eyez);
		glUniform3fv(light_loc, 1, light_pos);
		//glUniform3fv(light_loc, 1, light_pos_local);
		
		glUniformMatrix4fv(MV_loc, 1, GL_FALSE, MV);
		glUniformMatrix4fv(P_loc, 1, GL_FALSE, P);
		glUniformMatrix4fv(M_loc, 1, GL_FALSE, M);
		

		//tangent & bitagent
		GLuint Tangent_loc = glGetUniformLocation(program, "Tangent");
		GLuint Bitangent_loc = glGetUniformLocation(program, "Bitangent");

		//*********************************************************************************************************
		
		// Lighting & Material
		GLint iLocMaterialAmbient = glGetUniformLocation(program, "Ambient");
		GLint iLocMaterialDiffuse = glGetUniformLocation(program, "Diffuse");
		GLint iLocMaterialSpecular = glGetUniformLocation(program, "Specular");
		GLint iLocMaterialShininess = glGetUniformLocation(program, "Shiness");

		
		GLint iLocLightPosition;
		GLint iLocLightAmbient = glGetUniformLocation(program, "Ambient_Light");
		GLint iLocLightDiffuse = glGetUniformLocation(program, "Diffuse_Light");
		GLint iLocLightSpecular = glGetUniformLocation(program, "Specular_Light");


		//Material

		GLMgroup *group = model->groups;

		while (group)
		{
			glUniform3fv(iLocMaterialAmbient, 1, model->materials[group->material].ambient);
			glUniform3fv(iLocMaterialDiffuse, 1, model->materials[group->material].diffuse);
			glUniform3fv(iLocMaterialSpecular, 1, model->materials[group->material].specular);
			glUniform1f(iLocMaterialShininess, model->materials[group->material].shininess);

			group = group->next;
		}

		

		

		//Lighting

		GLfloat LightSource_ambient[3];
		GLfloat LightSource_diffuse[3];
		GLfloat LightSource_specular[3];
		//****
		for (int i = 0; i < 3; i++)
		{
			LightSource_ambient[i] = 1;
			LightSource_diffuse[i] = 1;
			LightSource_specular[i] = 1;
		}

		glUniform3fv(iLocLightAmbient, 1, LightSource_ambient);
		glUniform3fv(iLocLightDiffuse, 1, LightSource_diffuse);
		glUniform3fv(iLocLightSpecular, 1, LightSource_specular);


		//*********************************************************************************************************
		
		//Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model->textures[0].id);
		glUniform1i(Tex_loc, 0);
		
		//Draw Array
		glDrawArrays(GL_TRIANGLES, 0, model->groups->numtriangles * 3);
		glBindTexture(GL_TEXTURE_2D, NULL);

		//normal texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalTextureID);
		glUniform1i(Normal_Tex_loc, 1);

		//bump
		GLuint bump_loc = glGetUniformLocation(program, "Bump");
		glUniform1i(bump_loc, bump);
		
		
		glUseProgram(NULL);
	glPopMatrix();
	


	glutSwapBuffers();
	camera_light_ball_move();
}

//please implement bump mapping toggle(enable/disable bump mapping) in case 'b'(lowercase)
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	{	//ESC
		break;
	}
	case 'b'://toggle bump mapping
	{
		//you may need to do somting here
		if (bump == 1)
			bump = 0;
		else
			bump = 1;
		break;
	}
	case 'd':
	{
		right = true;
		break;
	}
	case 'a':
	{
		left = true;
		break;
	}
	case 'w':
	{
		forward = true;
		break;
	}
	case 's':
	{
		backward = true;
		break;
	}
	case 'q':
	{
		up = true;
		break;
	}
	case 'e':
	{
		down = true;
		break;
	}
	case 't':
	{
		lforward = true;
		break;
	}
	case 'g':
	{
		lbackward = true;
		break;
	}
	case 'h':
	{
		lright = true;
		break;
	}
	case 'f':
	{
		lleft = true;
		break;
	}
	case 'r':
	{
		lup = true;
		break;
	}
	case 'y':
	{
		ldown = true;
		break;
	}
	case 'i':
	{
		bforward = true;
		break;
	}
	case 'k':
	{
		bbackward = true;
		break;
	}
	case 'l':
	{
		bright = true;
		break;
	}
	case 'j':
	{
		bleft = true;
		break;
	}
	case 'u':
	{
		bup = true;
		break;
	}
	case 'o':
	{
		bdown = true;
		break;
	}
	case '7':
	{
		bx = true;
		break;
	}
	case '8':
	{
		by = true;
		break;
	}
	case '9':
	{
		bz = true;
		break;
	}
	case '4':
	{
		brx = true;
		break;
	}
	case '5':
	{
		bry = true;
		break;
	}
	case '6':
	{
		brz = true;
		break;
	}

	//special function key
	case 'z'://move light source to front of camera
	{
		light_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180);
		light_pos[1] = eyey + sin(eyet*M_PI / 180);
		light_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180);
		break;
	}
	case 'x'://move ball to front of camera
	{
		ball_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) * 3;
		ball_pos[1] = eyey + sin(eyet*M_PI / 180) * 5;
		ball_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180) * 3;
		break;
	}
	case 'c'://reset all pose
	{
		light_pos[0] = 1;
		light_pos[1] = 1;
		light_pos[2] = 1;
		ball_pos[0] = 0;
		ball_pos[1] = 0;
		ball_pos[2] = 0;
		ball_rot[0] = 0;
		ball_rot[1] = 0;
		ball_rot[2] = 0;
		eyex = 0;
		eyey = 0;
		eyez = 3;
		eyet = 0;
		eyep = 90;
		break;
	}
	default:
	{
		break;
	}
	}
}

//no need to modify the following functions
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.001f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}
void motion(int x, int y) {
	if (mleft)
	{
		eyep -= (x-mousex)*0.1;
		eyet -= (y - mousey)*0.12;
		if (eyet > 89.9)
			eyet = 89.9;
		else if (eyet < -89.9)
			eyet = -89.9;
		if (eyep > 360)
			eyep -= 360;
		else if (eyep < 0)
			eyep += 360;
	}
	mousex = x;
	mousey = y;
}
void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN && !mright && !mmiddle)
		{
			mleft = true;
			mousex = x;
			mousey = y;
		}
		else
			mleft = false;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mmiddle)
		{
			mright = true;
			mousex = x;
			mousey = y;
		}
		else
			mright = false;
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mright)
		{
			mmiddle = true;
			mousex = x;
			mousey = y;
		}
		else
			mmiddle = false;
	}
}
void camera_light_ball_move()
{
	GLfloat dx = 0, dy = 0, dz=0;
	if(left|| right || forward || backward || up || down)
	{ 
		if (left)
			dx = -speed;
		else if (right)
			dx = speed;
		if (forward)
			dy = speed;
		else if (backward)
			dy = -speed;
		eyex += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		eyey += dy*sin(eyet*M_PI / 180);
		eyez += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (up)
			eyey += speed;
		else if (down)
			eyey -= speed;
	}
	if(lleft || lright || lforward || lbackward || lup || ldown)
	{
		dx = 0;
		dy = 0;
		if (lleft)
			dx = -speed;
		else if (lright)
			dx = speed;
		if (lforward)
			dy = speed;
		else if (lbackward)
			dy = -speed;
		light_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		light_pos[1] += dy*sin(eyet*M_PI / 180);
		light_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (lup)
			light_pos[1] += speed;
		else if(ldown)
			light_pos[1] -= speed;
	}
	if (bleft || bright || bforward || bbackward || bup || bdown)
	{
		dx = 0;
		dy = 0;
		if (bleft)
			dx = -speed;
		else if (bright)
			dx = speed;
		if (bforward)
			dy = speed;
		else if (bbackward)
			dy = -speed;
		ball_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		ball_pos[1] += dy*sin(eyet*M_PI / 180);
		ball_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (bup)
			ball_pos[1] += speed;
		else if (bdown)
			ball_pos[1] -= speed;
	}
	if(bx||by||bz || brx || bry || brz)
	{
		dx = 0;
		dy = 0;
		dz = 0;
		if (bx)
			dx = -rotation_speed;
		else if (brx)
			dx = rotation_speed;
		if (by)
			dy = rotation_speed;
		else if (bry)
			dy = -rotation_speed;
		if (bz)
			dz = rotation_speed;
		else if (brz)
			dz = -rotation_speed;
		ball_rot[0] += dx;
		ball_rot[1] += dy;
		ball_rot[2] += dz;
	}
}
void draw_light_bulb()
{
	GLUquadric *quad;
	quad = gluNewQuadric();
	glPushMatrix();
	glColor3f(0.4, 0.5, 0);
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
	gluSphere(quad, light_rad, 40, 20);
	glPopMatrix();
}
void keyboardup(unsigned char key, int x, int y) {
	switch (key) {
	case 'd':
	{
		right =false;
		break;
	}
	case 'a':
	{
		left = false;
		break;
	}
	case 'w':
	{
		forward = false;
		break;
	}
	case 's':
	{
		backward = false;
		break;
	}
	case 'q':
	{
		up = false;
		break;
	}
	case 'e':
	{
		down = false;
		break;
	}
	case 't':
	{
		lforward = false;
		break;
	}
	case 'g':
	{
		lbackward = false;
		break;
	}
	case 'h':
	{
		lright = false;
		break;
	}
	case 'f':
	{
		lleft = false;
		break;
	}
	case 'r':
	{
		lup = false;
		break;
	}
	case 'y':
	{
		ldown = false;
		break;
	}
	case 'i':
	{
		bforward = false;
		break;
	}
	case 'k':
	{
		bbackward = false;
		break;
	}
	case 'l':
	{
		bright = false;
		break;
	}
	case 'j':
	{
		bleft = false;
		break;
	}
	case 'u':
	{
		bup = false;
		break;
	}
	case 'o':
	{
		bdown = false;
		break;
	}
	case '7':
	{
		bx = false;
		break;
	}
	case '8':
	{
		by = false;
		break;
	}
	case '9':
	{
		bz = false;
		break;
	}
	case '4':
	{
		brx = false;
		break;
	}
	case '5':
	{
		bry = false;
		break;
	}
	case '6':
	{
		brz = false;
		break;
	}

	default:
	{
		break;
	}
	}
}
void idle(void)
{
	glutPostRedisplay();
}
GLuint load_normal_map(char* name)
{
	return glmLoadTexture(name, false, true, true, true, &normalWid, &normalHei);
}
