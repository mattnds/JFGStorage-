#if 0

//
// Perspective view of a color cube using LookAt() and Ortho()
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.
//
//  updated code with the Camera in the Display method



#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "Angel.h"
#include "simplemodels.h"
using namespace std;

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Zaxis;
GLfloat  Theta1[NumAxes] = { 0.0, 0.0, 0.0 };
GLfloat  Theta2[NumAxes] = { 0.0, 0.0, 0.0 };

float theta_x = 20, theta_y = 20, theta_z = 20;


size_t CUBE_OFFSET;
size_t CYLINDER_OFFSET;
size_t SPHERE_OFFSET;

GLuint program;	
GLuint vPosition;
GLuint vColor;

// OpenGL initialization
void
init()
{
	program = InitShader("vshader21.glsl", "fshader21.glsl");
	glUseProgram(program);

	//---------------------------------------------------------------------
	colorcube();
	colortube();
	colorsphere();
	//---------------------------------------------------------------------


	//----set offset variables

	CUBE_OFFSET = 0;
	CYLINDER_OFFSET = sizeof(points_cube) + sizeof(colors);
	SPHERE_OFFSET = sizeof(points_cube) + sizeof(points_cylinder) + sizeof(colors);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);  // removed 'APPLE' suffix for 3.2
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(points_cube) + sizeof(colors) + sizeof(points_cylinder) + sizeof(points_sphere) , NULL, GL_STATIC_DRAW);


	glBufferSubData(GL_ARRAY_BUFFER, CUBE_OFFSET, sizeof(points_cube), points_cube); //cube
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points_cube), sizeof(colors), colors);	//colors
	glBufferSubData(GL_ARRAY_BUFFER, CYLINDER_OFFSET, sizeof(points_cylinder), points_cylinder); //cylinder
	glBufferSubData(GL_ARRAY_BUFFER, SPHERE_OFFSET, sizeof(points_sphere), points_sphere); //sphere
	
	//---------------------------------------------------------------------

	// set up vertex arrays
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points_cube)));
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points_cylinder)));

	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


	//---------------------------------------------------------------------

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------

mat4 proj_matrix;
mat4 view_matrix;

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--------camera intrinsic para

	float left = -3.0;
	float right = 3.0;
	float bottom = -3.0;
	float top = 3.0;
	float zNear = -3.0;
	float zFar = 3.0;
	
	mat4 zt = Translate(0.0, 0.0, (zFar + zNear) / 2.0);
	proj_matrix = Scale(2.0 / (right - left), 2.0 / (top - bottom), 2.0 / (zNear - zFar)) * zt;

	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, proj_matrix);

	//---------camera extrinic parameters 

	vec4 eye = vec4(1.0, 0.0, 1.0, 1.0);
	vec4 at = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 up = vec4(0.0, 1.0, 0.0, 0.0);

	vec4 n = normalize(eye - at);
	vec4 u = vec4(normalize(cross(up, n)), 0.0);
	vec4 v = vec4(normalize(cross(n, u)), 0.0);
	vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
	mat4 c = mat4(u, v, n, t);

	view_matrix = c * Translate(-eye);

	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view_matrix);


	///cylinder 

	mat4 rot = RotateX(theta_x) * RotateY(theta_y) * RotateZ(theta_z);
	mat4 transform = Translate(0.5, 0.0, 0.0) * rot * Scale(.5,.5, .5);


	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET));
	glUniform1i(glGetUniformLocation(program, "obj_color_on"), false);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.8, 0.0, 0.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCylinder);


	mat4  rot2 = RotateX(theta_x) * RotateY(theta_y) * RotateZ(theta_z);
	mat4  transform2 = Translate(-0.5, 0.0, 0.0) * rot2 * Scale(.5, .5, .5);


	//cube

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform2);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glUniform1i(glGetUniformLocation(program, "obj_color_on"), false);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.8, 0.0, 0.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);


	mat4  rot3 = RotateX(theta_x) * RotateY(theta_y) * RotateZ(theta_z);
	mat4 transform3 = Translate(0.5, 0.5, 0.5) * rot3 * Scale(0.5, 0.5, 0.5);

	//std::cout << transform3;

	//sphere

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform3);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET));
	glUniform1i(glGetUniformLocation(program, "obj_color_on"), false);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.8, 0.0, 0.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);

	glutSwapBuffers();
}

//----------------------------------------------------------------------------


bool poly_mode = false;

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'x':  Axis = Xaxis;  break;
	case 'y':  Axis = Yaxis;  break;
	case 'z':  Axis = Zaxis;  break;
	case 'p':  poly_mode = true;  glutPostRedisplay(); break;
	case 'P':  poly_mode = false;  glutPostRedisplay(); break;

	}
}

//----------------------------------------------------------------------------
void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
		case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
		case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
		}
	}
}
//----------------------------------------------------------------------------

void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

//----------------------------------------------------------------------------


void
idle(void) {
	Theta1[Axis] = fmod(Theta1[Axis] + 1, 360.0);
	Theta2[Axis] = fmod(Theta2[Axis] + 2, 360.0);

	glutPostRedisplay();
}
int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800 );
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("WHERE IS THE CAMERA????");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}

#endif

#if 0
// sperical camera
// Perspective view of a color cube using LookAt() and Ortho()
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.
//
//  updated code with the Camera in the Display method



#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "Angel.h"
#include "simplemodels.h"
using namespace std;


// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Zaxis;
GLfloat  Theta1[NumAxes] = { 80.0, 0.0, 0.0 };
GLfloat  Theta2[NumAxes] = { 0.0, 30.0, 0.0 };
GLfloat  Theta3[NumAxes] = { 0.0, 0.0, 0.0 };

float theta_x = 20, theta_y = 20, theta_z = 20;


size_t CUBE_OFFSET;
size_t CYLINDER_OFFSET;
size_t SPHERE_OFFSET;

GLuint program;
GLuint vPosition;
GLuint vColor;

// OpenGL initialization
void
init()
{
	program = InitShader("vshader21.glsl", "fshader21.glsl");
	glUseProgram(program);

	//---------------------------------------------------------------------
	colorcube();
	colortube();
	colorsphere();
	//---------------------------------------------------------------------


	//----set offset variables

	CUBE_OFFSET = 0;
	CYLINDER_OFFSET = sizeof(points_cube) + sizeof(colors);
	SPHERE_OFFSET = sizeof(points_cube) + sizeof(points_cylinder) + sizeof(colors);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);  // removed 'APPLE' suffix for 3.2
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(points_cube) + sizeof(colors) + sizeof(points_cylinder) + sizeof(points_sphere), NULL, GL_STATIC_DRAW);


	glBufferSubData(GL_ARRAY_BUFFER, CUBE_OFFSET, sizeof(points_cube), points_cube); //cube
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points_cube), sizeof(colors), colors);	//colors
	glBufferSubData(GL_ARRAY_BUFFER, CYLINDER_OFFSET, sizeof(points_cylinder), points_cylinder); //cylinder
	glBufferSubData(GL_ARRAY_BUFFER, SPHERE_OFFSET, sizeof(points_sphere), points_sphere); //sphere

																						   //---------------------------------------------------------------------

																						   // set up vertex arrays
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points_cube)));
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points_cylinder)));

	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


	//---------------------------------------------------------------------

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------

mat4 proj_matrix;
mat4 view_matrix;

float r = 1.0;

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//---- camera intrinsic parameters

	float left = -3.0;
	float right = 3.0;
	float bottom = -3.0;
	float top = 3.0;
	float zNear = -3.0;
	float zFar = 3.0;

	proj_matrix = Scale(2.0 / (right - left), 2.0 / (top - bottom), 2.0 / (zNear - zFar));

	//proj_matrix = Ortho( left, right, bottom, top, zNear, zFar );

	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, proj_matrix);


	//---- camera extrinsic parameters

	float tr_y = Theta3[Yaxis] * M_PI / 180.0;
	float tr_z = Theta3[Zaxis] * M_PI / 180.0;
	float eye_z = r * (cos(tr_z)) * cos(tr_y);
	float eye_x = r * (cos(tr_z)) * sin(tr_y);
	float eye_y = r * sin(tr_z);

	vec4 up = vec4(0.0, cos(tr_z), 0.0, 0.0);
	cout << up << ' ' << normalize(up) << endl;

	view_matrix = LookAt(vec4(eye_x, eye_y, eye_z, 1.0), vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, cos(tr_z), 0.0, 0.0));

	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view_matrix);


	///cylinder 

	mat4 rot = RotateX(theta_x) * RotateY(theta_y) * RotateZ(theta_z);
	mat4 transform = Translate(0.5, 0.0, 0.0) * rot * Scale(.5, .5, .5);


	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET));
	glUniform1i(glGetUniformLocation(program, "obj_color_on"), false);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.8, 0.0, 0.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCylinder);


	mat4  rot2 = RotateX(theta_x) * RotateY(theta_y) * RotateZ(theta_z);
	mat4  transform2 = Translate(-0.5, 0.0, 0.0) * rot2 * Scale(.5, .5, .5);


	//cube

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform2);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glUniform1i(glGetUniformLocation(program, "obj_color_on"), false);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.8, 0.0, 0.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);


	mat4  rot3 = RotateX(theta_x) * RotateY(theta_y) * RotateZ(theta_z);
	mat4 transform3 = Translate(0.5, 0.5, 0.5) * rot3 * Scale(0.5, 0.5, 0.5);

	//std::cout << transform3;

	//sphere

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform3);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET));
	glUniform1i(glGetUniformLocation(program, "obj_color_on"), false);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.8, 0.0, 0.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);

	glutSwapBuffers();
}

//----------------------------------------------------------------------------


bool poly_mode = false;

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033:    // Escape key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;

	case 'y':    //---- theta
		Axis = Yaxis;
		Theta3[Axis] += 5.0;
		Theta3[Axis] = fmod(Theta3[Axis], 360.0);
		glutPostRedisplay();
		break;
	case 'z':    //---- phi
		Axis = Zaxis;
		Theta3[Axis] += 5.0;
		Theta3[Axis] = fmod(Theta3[Axis], 360.0);
		glutPostRedisplay();
		break;

	case 'r':    //---- increase radius
		r += 0.5;
		glutPostRedisplay();
		break;
	case 'R':    //---- decrease radius
		r -= 0.5;
		glutPostRedisplay();
		break;

	case 'u':
		//view_matrix = default_view_matrix;
		Theta3[Xaxis] = 0.0;
		Theta3[Yaxis] = 0.0;
		Theta3[Zaxis] = 0.0;
		glutPostRedisplay();
		break;
	}
}

//----------------------------------------------------------------------------
void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
		case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
		case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
		}
	}
}
//----------------------------------------------------------------------------

void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

//----------------------------------------------------------------------------


void
idle(void) {
	Theta1[Axis] = fmod(Theta1[Axis] + 1, 360.0);
	Theta2[Axis] = fmod(Theta2[Axis] + 2, 360.0);

	glutPostRedisplay();
}
int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("WHERE IS THE CAMERA????");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	//glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}
#endif

#if 0 //with custom camera
// spherical camera + persp
// Perspective view of a color cube using LookAt() and Ortho()
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.
//
//  updated code with the Camera in the Display method



#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "Angel.h"
#include "simplemodels.h"
using namespace std;


// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Zaxis;
GLfloat  Theta1[NumAxes] = { 80.0, 0.0, 0.0 };
GLfloat  Theta2[NumAxes] = { 0.0, 30.0, 0.0 };
GLfloat  Theta3[NumAxes] = { 0.0, 0.0, 0.0 };

float theta_x = 20, theta_y = 20, theta_z = 20;


size_t CUBE_OFFSET;
size_t CYLINDER_OFFSET;
size_t SPHERE_OFFSET;

GLuint program;
GLuint vPosition;
GLuint vColor;

mat4 current_view_matrix;

vec4 eye = vec4(2.0, 2.0, 2.0, 1.0);
vec4 at = vec4(0.0, 0.0, 1.0, 1.0);
vec4 up = vec4(0.0, 1.0, 0.0, 0.0);

// OpenGL initialization
void
init()
{
	program = InitShader("vshader21.glsl", "fshader21.glsl");
	glUseProgram(program);

	//---------------------------------------------------------------------
	colorcube();
	colortube();
	colorsphere();
	//---------------------------------------------------------------------


	//----set offset variables

	CUBE_OFFSET = 0;
	CYLINDER_OFFSET = sizeof(points_cube) + sizeof(colors);
	SPHERE_OFFSET = sizeof(points_cube) + sizeof(points_cylinder) + sizeof(colors);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);  // removed 'APPLE' suffix for 3.2
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(points_cube) + sizeof(colors) + sizeof(points_cylinder) + sizeof(points_sphere), NULL, GL_STATIC_DRAW);


	glBufferSubData(GL_ARRAY_BUFFER, CUBE_OFFSET, sizeof(points_cube), points_cube); //cube
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points_cube), sizeof(colors), colors);	//colors
	glBufferSubData(GL_ARRAY_BUFFER, CYLINDER_OFFSET, sizeof(points_cylinder), points_cylinder); //cylinder
	glBufferSubData(GL_ARRAY_BUFFER, SPHERE_OFFSET, sizeof(points_sphere), points_sphere); //sphere

	//---------------------------------------------------------------------
	// set up vertex arrays
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points_cube)));
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points_cylinder)));

	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


	//---------------------------------------------------------------------

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 1.0, 1.0, 1.0);
	current_view_matrix = LookAt(eye, at, up);
}

//----------------------------------------------------------------------------

mat4 proj_matrix;
mat4 view_matrix;
float r = 3.5;
float pan = Theta3[Yaxis] * M_PI / 180.0;;
float tilt = Theta3[Yaxis] * M_PI / 180.0;;
float roll = pan * tilt;

float tr_y = Theta3[Yaxis] * M_PI / 180.0;
float tr_z = Theta3[Zaxis] * M_PI / 180.0;
float eye_z = r * (cos(tr_z)) * cos(tr_y);
float eye_x = r * (cos(tr_z)) * sin(tr_y);
float eye_y = r * sin(tr_z);

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//---- camera intrinsic parameters

	float left = -0.5;
	float right = 0.5;
	float bottom = -0.5;
	float top = 0.5;
	float zNear = 1.0;
	float zFar = 15.0;

	//proj_matrix = Scale(2.0/(right-left), 2.0/(top-bottom), 2.0/(zNear-zFar));
	proj_matrix = Frustum(left, right, bottom, top, zNear, zFar);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, proj_matrix);
	//---- camera extrinsic parameters
	//vec4 up = vec4(0.0, cos(tr_z), 0.0, 0.0);	
	//cout << up << ' ' << normalize(up) << endl;
	//x y z 
	//view_matrix = LookAt(vec4(eye_x, tilt, eye_z, 1.0), vec4(pan, tilt, cos(roll), 1.0), vec4(roll, cos(tr_z), 0.0, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, current_view_matrix);

	///cylinder 

	mat4 rot = RotateX(theta_x) * RotateY(theta_y) * RotateZ(theta_z);
	mat4 transform = Translate(0.5, 0.0, 0.0) * rot * Scale(.5, .5, .5);


	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET));
	glUniform1i(glGetUniformLocation(program, "obj_color_on"), false);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.8, 0.0, 0.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCylinder);


	mat4  rot2 = RotateX(theta_x) * RotateY(theta_y) * RotateZ(theta_z);
	mat4  transform2 = Translate(-0.5, 0.0, 0.0) * rot2 * Scale(.5, .5, .5);


	//cube

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform2);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glUniform1i(glGetUniformLocation(program, "obj_color_on"), false);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.8, 0.0, 0.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);


	mat4  rot3 = RotateX(theta_x) * RotateY(theta_y) * RotateZ(theta_z);
	mat4 transform3 = Translate(0.5, 0.5, 0.5) * rot3 * Scale(0.5, 0.5, 0.5);

	//std::cout << transform3;

	//sphere

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform3);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET));
	glUniform1i(glGetUniformLocation(program, "obj_color_on"), false);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.8, 0.0, 0.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);

	glutSwapBuffers();
}

//----------------------------------------------------------------------------


bool poly_mode = false;
#if 0
void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'x':  Axis = Xaxis;  break;
	case 'y':  Axis = Yaxis;  break;
	case 'z':  Axis = Zaxis;  break;
	}
}
#endif

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033:    // Escape key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'w': //move up?
		current_view_matrix = Translate(0.0, 0.0, 0.5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'a': //move left
		current_view_matrix = Translate(0.5, 0.0, 0.0)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 's': //move down?
		current_view_matrix = Translate(0.0, 0.0, -0.5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'd': //move right
		current_view_matrix = Translate(-0.5, 0.0, 0.0)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'p': //pan
		current_view_matrix = RotateY(5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'P': //pan
		current_view_matrix = RotateY(-5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 't': //pan
		current_view_matrix = RotateX(-5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'T': //tilt
		current_view_matrix = RotateX(5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'r': //roll
		current_view_matrix = RotateZ(5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'R': //roll
		current_view_matrix = RotateZ(-5)*current_view_matrix;
		glutPostRedisplay();
		break;		
	case 'u':
		//view_matrix = default_view_matrix;
		Theta3[Xaxis] = 0.0;
		Theta3[Yaxis] = 0.0;
		Theta3[Zaxis] = 0.0;
		glutPostRedisplay();
		break;
	}
}

//----------------------------------------------------------------------------
void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
		case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
		case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
		}
	}
}
//----------------------------------------------------------------------------

void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

//----------------------------------------------------------------------------


void
idle(void) {
	Theta1[Axis] = fmod(Theta1[Axis] + 1, 360.0);
	Theta2[Axis] = fmod(Theta2[Axis] + 2, 360.0);

	glutPostRedisplay();
}
int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1920, 1080);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("WHERE IS THE CAMERA????");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
//	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}

#endif

#if 0
//  wanted to show hierarchical animation better
//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "Angel.h"
#include "simplemodels.h"



// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Zaxis;
GLfloat  Theta1[NumAxes] = { 0.0, 0.0, 0.0 };
GLfloat  Theta2[NumAxes] = { 0.0, 0.0, 0.0 };

bool right_motion1 = true;
bool right_motion2 = true;

size_t CUBE_OFFSET;
size_t CYLINDER_OFFSET;


//----------------------------------------------------------------------------

GLuint program;
GLuint vPosition;
GLuint vColor;

void
init()
{
	// Load shaders and use the resulting shader program
	program = InitShader("vshader21.glsl", "fshader21.glsl");
	glUseProgram(program);

	//---------------------------------------------------------------------
	colorcube();
	colortube();
	//---------------------------------------------------------------------


	//----set offset variables

	CUBE_OFFSET = 0;
	CYLINDER_OFFSET = sizeof(points_cube) + sizeof(colors);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);  // removed 'APPLE' suffix for 3.2
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points_cube) + sizeof(colors) + sizeof(points_cylinder), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE_OFFSET, sizeof(points_cube), points_cube);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points_cube), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, CYLINDER_OFFSET, sizeof(points_cylinder), points_cylinder);

	//---------------------------------------------------------------------

	// set up vertex arrays
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points_cube)));
	//---------------------------------------------------------------------

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4  rot1 = (RotateX(Theta1[Xaxis]) * RotateY(Theta1[Yaxis]) * RotateZ(Theta1[Zaxis]));
	mat4 transform = Translate(0.0, 1.0, 0.0) * rot1 * Translate(0.0, -0.5, 0.0) * Scale(0.25, 1.0, 0.25);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET));

	glUniform1i(glGetUniformLocation(program, "obj_color_on"), true);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.8, 0.0, 0.0, 1.0));

	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCylinder);


	mat4  rot2 = (RotateX(Theta2[Xaxis]) * RotateY(Theta2[Yaxis]) * RotateZ(Theta2[Zaxis]));
	transform = Translate(0.0, 1.0, 0.0) * rot1 * Translate(0.0, -1.0, 0.0)* rot2 * Translate(0.0, -0.5, 0.0) * Scale(0.25, 1.0, 0.25);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));

	glUniform1i(glGetUniformLocation(program, "obj_color_on"), false);

	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'x':  Axis = Xaxis;  break;
	case 'y':  Axis = Yaxis;  break;
	case 'z':  Axis = Zaxis;  break;

	}
}

//----------------------------------------------------------------------------

void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
		case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
		case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
		}
	}
}

//----------------------------------------------------------------------------

void
idle(void)
{
	if (right_motion1)
		Theta1[Axis] += 1;
	else
		Theta1[Axis] -= 1;

	if (Theta1[Axis] > 30.0)
		right_motion1 = false;
	if (Theta1[Axis] < -30.0)
		right_motion1 = true;

	if (right_motion2)
		Theta2[Axis] += 2;
	else
		Theta2[Axis] -= 2;

	if (Theta2[Axis] > 60.0)
		right_motion2 = false;
	if (Theta2[Axis] < -60.0)
		right_motion2 = true;

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("WHERE IS THE CAMERA????");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}

#endif

#if 0
// spherical camera
//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "Angel.h"
#include "simplemodels.h"
using namespace std;


// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Zaxis;
GLfloat  Theta1[NumAxes] = { 80.0, 0.0, 0.0 };
GLfloat  Theta2[NumAxes] = { 0.0, 30.0, 0.0 };
GLfloat  Theta3[NumAxes] = { 0.0, 0.0, 0.0 };


size_t CUBE_OFFSET;
size_t CYLINDER_OFFSET;


//----------------------------------------------------------------------------

GLuint program;
GLuint vPosition;
GLuint vColor;

void
init()
{
	// Load shaders and use the resulting shader program
	program = InitShader("vshader21.glsl", "fshader21.glsl");
	glUseProgram(program);

	//---------------------------------------------------------------------
	colorcube();
	colortube();
	//---------------------------------------------------------------------


	//----set offset variables

	CUBE_OFFSET = 0;
	CYLINDER_OFFSET = sizeof(points_cube) + sizeof(colors);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);  // removed 'APPLE' suffix for 3.2
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points_cube) + sizeof(colors) + sizeof(points_cylinder), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE_OFFSET, sizeof(points_cube), points_cube);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points_cube), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, CYLINDER_OFFSET, sizeof(points_cylinder), points_cylinder);

	//---------------------------------------------------------------------

	// set up vertex arrays
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points_cube)));
	//---------------------------------------------------------------------

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------

mat4 proj_matrix;

mat4 view_matrix;

float r = 1.0;

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//---- camera intrinsic parameters

	float left = -3.0;
	float right = 3.0;
	float bottom = -3.0;
	float top = 3.0;
	float zNear = -3.0;
	float zFar = 3.0;

	proj_matrix = Scale(2.0 / (right - left), 2.0 / (top - bottom), 2.0 / (zNear - zFar));

	//proj_matrix = Ortho( left, right, bottom, top, zNear, zFar );

	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, proj_matrix);


	//---- camera extrinsic parameters

	float tr_y = Theta3[Yaxis] * M_PI / 180.0;
	float tr_z = Theta3[Zaxis] * M_PI / 180.0;
	float eye_z = r * (cos(tr_z)) * cos(tr_y);
	float eye_x = r * (cos(tr_z)) * sin(tr_y);
	float eye_y = r * sin(tr_z);

	vec4 up = vec4(0.0, cos(tr_z), 0.0, 0.0);
	cout << up << ' ' << normalize(up) << endl;

	view_matrix = LookAt(vec4(eye_x, eye_y, eye_z, 1.0), vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, cos(tr_z), 0.0, 0.0));

	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view_matrix);


	//---- cylinder 1

	mat4  rot1 = (RotateX(Theta1[Xaxis]) * RotateY(Theta1[Yaxis]) * RotateZ(Theta1[Zaxis]));
	mat4 transform = Translate(0.5, 0.2, 0.0) * rot1 * Scale(0.25, 1.0, 0.25);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET));

	glUniform1i(glGetUniformLocation(program, "obj_color_on"), true);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.9, 0.2, 0.2, 1.0));

	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCylinder);


	//---- cylinder 2

	rot1 = (RotateX(Theta1[Xaxis]) * RotateY(Theta1[Yaxis]) * RotateZ(Theta1[Zaxis]));
	transform = Translate(1.5, 0.2, 0.0) * rot1 * Scale(1.0, 0.5, 1.0);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET));

	glUniform1i(glGetUniformLocation(program, "obj_color_on"), true);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.9, 0.9, 0.0, 1.0));

	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCylinder);


	//---- cube 1

	mat4 rot2 = (RotateX(Theta2[Xaxis]) * RotateY(Theta2[Yaxis]) * RotateZ(Theta2[Zaxis]));
	transform = Translate(-0.5, 0.0, 0.0) * rot2 * Scale(0.25, 1.0, 0.25);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));

	glUniform1i(glGetUniformLocation(program, "obj_color_on"), false);

	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- cube 2

	rot2 = (RotateX(Theta2[Xaxis]) * RotateY(Theta2[Yaxis]) * RotateZ(Theta2[Zaxis]));
	transform = Translate(-1.5, 0.0, 0.0) * rot2 * Scale(0.25, 1.0, 0.25);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));

	glUniform1i(glGetUniformLocation(program, "obj_color_on"), true);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.0, 0.9, 0.0, 1.0));

	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);


	glutSwapBuffers();
}


//----------------------------------------------------------------------------


void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033:    // Escape key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;

	case 'y':    //---- theta
		Axis = Yaxis;
		Theta3[Axis] += 5.0;
		Theta3[Axis] = fmod(Theta3[Axis], 360.0);
		glutPostRedisplay();
		break;
	case 'z':    //---- phi
		Axis = Zaxis;
		Theta3[Axis] += 5.0;
		Theta3[Axis] = fmod(Theta3[Axis], 360.0);
		glutPostRedisplay();
		break;

	case 'r':    //---- increase radius
		r += 0.5;
		glutPostRedisplay();
		break;
	case 'R':    //---- decrease radius
		r -= 0.5;
		glutPostRedisplay();
		break;

	case 'u':
		//view_matrix = default_view_matrix;
		Theta3[Xaxis] = 0.0;
		Theta3[Yaxis] = 0.0;
		Theta3[Zaxis] = 0.0;
		glutPostRedisplay();
		break;
	}
}




//----------------------------------------------------------------------------

void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
		case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
		case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
		}
	}
}

//----------------------------------------------------------------------------

void
idle(void)
{
	Theta1[Axis] = fmod(Theta1[Axis] + 1, 360.0);
	Theta2[Axis] = fmod(Theta2[Axis] + 2, 360.0);

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("WHERE IS THE CAMERA????");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	//	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}
#endif

#if 0

// spherical camera + persp + lighting
//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.


#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "Angel.h"
#include "simplemodels.h"
using namespace std;


/// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Zaxis;
GLfloat  Theta1[NumAxes] = { 80.0, 0.0, 0.0 };
GLfloat  Theta2[NumAxes] = { 0.0, 30.0, 0.0 };
GLfloat  Theta3[NumAxes] = { 0.0, 0.0, 0.0 };
mat4 current_view_matrix;

vec4 eye = vec4(2.0, 2.0, 2.0, 1.0);
vec4 at = vec4(0.0, 0.0, 1.0, 1.0);
vec4 up = vec4(0.0, 1.0, 0.0, 0.0);
//----------------------------------------------------------------------------

size_t CUBE_OFFSET;
size_t COLOR_OFFSET;
size_t CUBE_NORMALS_OFFSET;
size_t CYLINDER_OFFSET;
size_t CYLINDER_NORMALS_OFFSET;
size_t SPHERE_OFFSET;
size_t SPHERE_NORMALS_OFFSET;

GLuint program;
GLuint vPosition;
GLuint vNormal;
GLuint vColor;


void
init()
{
	// Load shaders and use the resulting shader program
	program = InitShader("vshader21.glsl", "fshader21.glsl");
	glUseProgram(program);

	//---------------------------------------------------------------------
	colorcube();
	colortube();
	colorbube();
	//---------------------------------------------------------------------


	//----set offset variables

	CUBE_OFFSET = 0;
	COLOR_OFFSET = sizeof(points_cube);
	CUBE_NORMALS_OFFSET = COLOR_OFFSET + sizeof(colors);
	CYLINDER_OFFSET = CUBE_NORMALS_OFFSET + sizeof(normals_cube);
	CYLINDER_NORMALS_OFFSET = CYLINDER_OFFSET + sizeof(points_cylinder);
	SPHERE_OFFSET = CYLINDER_NORMALS_OFFSET + sizeof(normals_cylinder);
	SPHERE_NORMALS_OFFSET = SPHERE_OFFSET + sizeof(points_sphere);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);  // removed 'APPLE' suffix for 3.2
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points_cube) + sizeof(normals_cube) + sizeof(colors) + sizeof(points_cylinder) + sizeof(normals_cylinder) + sizeof(points_sphere) + sizeof(normals_sphere), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE_OFFSET, sizeof(points_cube), points_cube);
	glBufferSubData(GL_ARRAY_BUFFER, COLOR_OFFSET, sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE_NORMALS_OFFSET, sizeof(normals_cube), normals_cube);
	glBufferSubData(GL_ARRAY_BUFFER, CYLINDER_OFFSET, sizeof(points_cylinder), points_cylinder);
	glBufferSubData(GL_ARRAY_BUFFER, CYLINDER_NORMALS_OFFSET, sizeof(normals_cylinder), normals_cylinder);
	glBufferSubData(GL_ARRAY_BUFFER, SPHERE_OFFSET, sizeof(points_sphere), points_sphere);
	glBufferSubData(GL_ARRAY_BUFFER, SPHERE_NORMALS_OFFSET, sizeof(normals_sphere), normals_sphere);


	//---------------------------------------------------------------------

	// set up vertex arrays
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));

	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));

	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(COLOR_OFFSET));
	//---------------------------------------------------------------------

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.90, 0.9, 0.9, 0.9);
	current_view_matrix = LookAt(eye, at, up);
}


//----------------------------------------------------------------------------

mat4 proj_matrix;
mat4 view_matrix;
float r = 3.5;
float pan = Theta3[Yaxis] * M_PI / 180.0;;
float tilt = Theta3[Yaxis] * M_PI / 180.0;;
float roll = pan * tilt;

float tr_y = Theta3[Yaxis] * M_PI / 180.0;
float tr_z = Theta3[Zaxis] * M_PI / 180.0;
float eye_z = r * (cos(tr_z)) * cos(tr_y);
float eye_x = r * (cos(tr_z)) * sin(tr_y);
float eye_y = r * sin(tr_z);


void 
setMat(vec4 ka, vec4 kd, vec4 ks, float s) {

	
	glUniform4fv(glGetUniformLocation(program, "ka"), 1, ka);
	glUniform4fv(glGetUniformLocation(program, "kd"), 1, kd);
	glUniform4fv(glGetUniformLocation(program, "ks"), 1, ks);
	glUniform1f(glGetUniformLocation(program, "Shininess"), s);

}

void
setLight(vec4 lpos, vec4 la, vec4 ld, vec4 ls) {

	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, lpos);
	glUniform4fv(glGetUniformLocation(program, "AmbientLight"), 1, la);
	glUniform4fv(glGetUniformLocation(program, "DiffuseLight"), 1, ld);
	glUniform4fv(glGetUniformLocation(program, "SpecularLight"), 1, ls);
}


void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//---- Material
	//vec4 ka(0.9, 0.1, 0.1, 1.0);
	//vec4 kd(1.0, 0.0, 0.0, 1.0);
	//vec4 ks(1.0, 0.4, 0.4, 1.0);
	//float s = 5.0;

	//vec4 lpos = vec4(3.0, 3.0, 3.0, 1.0);

	//vec4 la(0.2, 0.2, 0.2, 1.0);
	//vec4 ld(0.7, 0.7, 0.7, 1.0);
	//vec4 ls(0.7, 0.7, 0.7, 1.0);
	//


	//---- camera intrinsic parameters

	float left = -1.0;
	float right = 1.0;
	float bottom = -1.0;
	float top = 1.0;
	float zNear = 1.0;
	float zFar = 100.0;

	//proj_matrix = Scale(2.0/(right-left), 2.0/(top-bottom), 2.0/(zNear-zFar));
	proj_matrix = Frustum(left, right, bottom, top, zNear, zFar);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, proj_matrix);
	//---- camera extrinsic parameters
	//vec4 up = vec4(0.0, cos(tr_z), 0.0, 0.0);	
	//cout << up << ' ' << normalize(up) << endl;
	//x y z 
	//view_matrix = LookAt(vec4(eye_x, tilt, eye_z, 1.0), vec4(pan, tilt, cos(roll), 1.0), vec4(roll, cos(tr_z), 0.0, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, current_view_matrix);


	////---- cylinder 1  pole  ---group one

	mat4 rot1 = (RotateX(0.0) * RotateY(0.0) * RotateZ(0.0));
	mat4 transform = Translate(2.5, 0.2, 0.0) * rot1 * Scale(0.05, 1.0, 0.05);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_NORMALS_OFFSET));


	setMat(vec4(0.0, 0.0, 0.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0), 1.0);
	setLight(vec4(2.5, 0.6, 0.0, 1.0), vec4(0.0, 0.0, 0.0, 0.0), vec4(0.9, 0.9, 0.9, 1.0), vec4(0.6, 0.6, 0.6, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCylinder);


	//---- cylinder 2  base  ---group one

	rot1 = (RotateX(0.0) * RotateY(0.0) * RotateZ(0.0));
	transform = Translate(2.5, 0.0, 0.0) * rot1 * Scale(.5, 0.05, .5);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_NORMALS_OFFSET));


	setMat(vec4(0.0, 0.0, 0.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0), 1.0);
	//setLight(vec4(2.5, 0.2, 0.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCylinder);

	//---- cube 1 grass ---group one

	mat4 rot2 = (RotateX(0.0) * RotateY(0.0) * RotateZ(0.0));
	transform = Translate(2.0, 0.0, 0.0) * rot2 * Scale(4.0, 0.002, 8.0);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));

	setMat(vec4(0.0, 1.0, 0.0, 0.0), vec4(0.0, 1.0, 0.0, 0.0), vec4(0.0, 1.0, 0.0, 0.0), 1.0);
	//setLight(vec4(1.0, 0.0, 0.0, 0.0), vec4(0.0, 1.0, 0.0, 0.0), vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET));
	glUniform1i(glGetUniformLocation(program, "obj_color_on"), false);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.8, 0.0, 0.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);

	///CUBE ROAD  ---group one
	
	rot2 = (RotateX(0.0) * RotateY(0.0) * RotateZ(0.0));
	transform = Translate(0.0, 0.01, 0.0) * rot2 * Scale(2.5, 0.005, 8.0);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));

	setMat(vec4(0.7, 0.7, 0.0, 0.0), vec4(0.7, 0.7, 0.0, 0.0), vec4(0.7, 0.7, 0.0, 0.0), 1.0);
	//setLight(vec4(1.0, 0.0, 0.0, 0.0), vec4(0.0, 1.0, 0.0, 0.0), vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET));
	glUniform1i(glGetUniformLocation(program, "obj_color_on"), false);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.8, 0.0, 0.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);

	////cube next to lamp   ---group one
	rot2 = (RotateX(0.0) * RotateY(0.0) * RotateZ(0.0));
	transform = Translate(2.5, 0.25, 0.8) * rot2 * Scale(0.5, 0.5, 0.5);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));

	setMat(vec4(1.0, 0.0, 0.0, 0.0), vec4(1.0, 0.0, 0.0, 0.0), vec4(1.0, 0.0, 0.0, 0.0), 1.0);
	//setLight(vec4(1.0, 0.0, 0.0, 0.0), vec4(0.0, 1.0, 0.0, 0.0), vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET));
	glUniform1i(glGetUniformLocation(program, "obj_color_on"), false);
	glUniform4fv(glGetUniformLocation(program, "obj_color"), 1, vec4(0.8, 0.0, 0.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);

	//--- SPHERE   ---group one
	rot1 = (RotateX(0.0) * RotateY(0.0) * RotateZ(0.0));
	transform = Translate(2.5, 0.6, 0.0) * rot1 * Scale(.25, 0.25, .25);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_NORMALS_OFFSET));
	setMat(vec4(0.9, 0.9, 0.9, 1.0), vec4(0.9, 0.9, 0.9, 1.0), vec4(0.9, 0.9, 0.9, 1.0), 4.0);
	setLight(vec4(2.5, 0.6, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);


	//---- cube 2 grass ---group two

	rot2 = (RotateX(0.0) * RotateY(0.0) * RotateZ(0.0));
	transform = Translate(-2.0, 0.0, 0.0) * rot2 * Scale(4.0, 0.002, 8.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	setMat(vec4(0.0, 1.0, 0.0, 0.0), vec4(0.0, 1.0, 0.0, 0.0), vec4(0.0, 1.0, 0.0, 0.0), 1.0);
	setLight(vec4(-2.5, 0.6, 0.0, 1.0), vec4(0.0, 0.0, 0.0, 0.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.6, 0.6, 0.6, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	////---- cylinder 1   ---group two

	rot1 = (RotateX(0.0) * RotateY(0.0) * RotateZ(0.0));
	transform = Translate(-2.5, 0.2, 0.0) * rot1 * Scale(0.05, 1.0, 0.05);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_NORMALS_OFFSET));
	setMat(vec4(0.0, 0.0, 0.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0), 1.0);
	setLight(vec4(2.5, 0.6, 0.0, 1.0), vec4(0.0, 0.0, 0.0, 0.0), vec4(0.9, 0.9, 0.9, 1.0), vec4(0.6, 0.6, 0.6, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCylinder);


	//---- cylinder 2  ---group two

	rot1 = (RotateX(0.0) * RotateY(0.0) * RotateZ(0.0));
	transform = Translate(-2.5, 0.0, 0.0) * rot1 * Scale(.5, 0.05, .5);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_NORMALS_OFFSET));
	setMat(vec4(0.0, 0.0, 0.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0), vec4(0.0, 0.0, 0.0, 0.0), 1.0);
	setLight(vec4(2.5, 0.6, 0.0, 1.0), vec4(0.0, 0.0, 0.0, 0.0), vec4(0.9, 0.9, 0.9, 1.0), vec4(0.6, 0.6, 0.6, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCylinder);

	//--- SPHERE  ---group two
	rot1 = (RotateX(0.0) * RotateY(0.0) * RotateZ(0.0));
	transform = Translate(-2.5, 0.6, 0.0) * rot1 * Scale(.25, 0.25, .25);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_NORMALS_OFFSET));
	setMat(vec4(0.9, 0.9, 0.9, 1.0), vec4(0.9, 0.9, 0.9, 1.0), vec4(0.9, 0.9, 0.9, 1.0), 4.0);
	setLight(vec4(2.5, 0.6, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);

	glutSwapBuffers();
}


//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033:    // Escape key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'w': //move up?
		current_view_matrix = Translate(0.0, 0.0, 0.5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'a': //move left
		current_view_matrix = Translate(0.5, 0.0, 0.0)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 's': //move down?
		current_view_matrix = Translate(0.0, 0.0, -0.5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'd': //move right
		current_view_matrix = Translate(-0.5, 0.0, 0.0)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'p': //pan
		current_view_matrix = RotateY(5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'P': //pan
		current_view_matrix = RotateY(-5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 't': //pan
		current_view_matrix = RotateX(-5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'T': //tilt
		current_view_matrix = RotateX(5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'r': //roll
		current_view_matrix = RotateZ(5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'R': //roll
		current_view_matrix = RotateZ(-5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'u':
		//view_matrix = default_view_matrix;
		Theta3[Xaxis] = 0.0;
		Theta3[Yaxis] = 0.0;
		Theta3[Zaxis] = 0.0;
		glutPostRedisplay();
		break;
	}
}




//----------------------------------------------------------------------------

void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
		case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
		case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
		}
	}
}

//----------------------------------------------------------------------------

void
idle(void)
{
	Theta1[Axis] = fmod(Theta1[Axis] + 1, 360.0);
	Theta2[Axis] = fmod(Theta2[Axis] + 2, 360.0);

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1440, 1440);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("WHERE IS THE LIGHT????");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	//	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}

#endif

#if 0

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "Angel.h"
#include "simplemodels.h"
using namespace std;


GLuint loadBMP_custom(unsigned char** data, int* w, int* h, const char * imagepath);


// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Zaxis;
GLfloat  Theta1[NumAxes] = { 80.0, 0.0, 0.0 };
GLfloat  Theta2[NumAxes] = { 0.0, 30.0, 0.0 };
GLfloat  Theta3[NumAxes] = { 0.0, 0.0, 0.0 };
GLfloat  Theta4[NumAxes] = { 0.0, 0.0, 0.0 };
mat4 current_view_matrix;
vec4 eye = vec4(2.0, 2.0, 2.0, 1.0);
vec4 at = vec4(0.0, 0.0, 1.0, 1.0);
vec4 up = vec4(0.0, 1.0, 0.0, 0.0);

size_t CUBE_OFFSET;
size_t COLOR_OFFSET;
size_t CUBE_NORMALS_OFFSET;
size_t CUBE_TEXCOORDS_OFFSET;
size_t CYLINDER_OFFSET;
size_t CYLINDER_NORMALS_OFFSET;
size_t CYLINDER_TEXCOORDS_OFFSET;
size_t SPHERE_OFFSET;
size_t SPHERE_NORMALS_OFFSET;
size_t SPHERE_TEXCOORDS_OFFSET;


//----------------------------------------------------------------------------

GLuint program;

GLuint vPosition;
GLuint vNormal;
GLuint vColor;
GLuint vTexCoord;

GLuint textures[2];

void
init()
{
	// Load shaders and use the resulting shader program
	program = InitShader("vshader22.glsl", "fshader22.glsl");   ///REMEMBER IT'S NOT 21
	glUseProgram(program);

	//---------------------------------------------------------------------
	colorcube();
	colortube();
	colorbube();
	//---------------------------------------------------------------------

	//----- generate a checkerboard pattern for texture mapping
	const int  TextureSize = 256;
	GLubyte checker_tex[TextureSize][TextureSize][3];

	for (int i = 0; i < TextureSize; i++)
	{
		for (int j = 0; j < TextureSize; j++)
		{
			//GLubyte c = (((i & 0x4) == 0) ^ ((j & 0x1)  == 0)) * 255;
			GLubyte c = (((j & 0x2) == 0)) * 255;
			checker_tex[i][j][0] = c;
			checker_tex[i][j][1] = c;
			checker_tex[i][j][2] = c;
		}
	}

	//---- Initialize texture objects
	glGenTextures(2, textures);

	glActiveTexture(GL_TEXTURE0);


	unsigned char* pic2 = NULL;
	int w, h;
	loadBMP_custom(&pic2, &w, &h, "pano.bmp");
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_BGR, GL_UNSIGNED_BYTE, checker_tex);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic2);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	unsigned char* pic1 = NULL;
	loadBMP_custom(&pic1, &w, &h, "planettex.bmp");
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic1);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	

	unsigned char* pic3 = NULL;
	loadBMP_custom(&pic3, &w, &h, "football.bmp");
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic3);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_BGR, GL_UNSIGNED_BYTE, pic3);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	unsigned char* pic4 = NULL;
	loadBMP_custom(&pic4, &w, &h, "pep.bmp");
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic4);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_BGR, GL_UNSIGNED_BYTE, pic3);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glEnable(GL_DEPTH_CLAMP);
/*	glEnable(GL_MULTISAMPLE);
	glEnable(GL_POLYGON_SMOOTH)*/;



	//----set offset variables

	CUBE_OFFSET = 0;
	COLOR_OFFSET = sizeof(points_cube);
	CUBE_NORMALS_OFFSET = COLOR_OFFSET + sizeof(colors);
	CUBE_TEXCOORDS_OFFSET = CUBE_NORMALS_OFFSET + sizeof(normals_cube);
	CYLINDER_OFFSET = CUBE_TEXCOORDS_OFFSET + sizeof(tex_coords_cube);
	CYLINDER_NORMALS_OFFSET = CYLINDER_OFFSET + sizeof(points_cylinder);
	CYLINDER_TEXCOORDS_OFFSET = CYLINDER_NORMALS_OFFSET + sizeof(normals_cylinder);
	SPHERE_OFFSET = CYLINDER_NORMALS_OFFSET + sizeof(normals_cylinder);
	SPHERE_NORMALS_OFFSET = SPHERE_OFFSET + sizeof(points_sphere);
	SPHERE_TEXCOORDS_OFFSET = SPHERE_NORMALS_OFFSET + sizeof(normals_sphere);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);  // removed 'APPLE' suffix for 3.2
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points_cube) + sizeof(normals_cube) + sizeof(tex_coords_cube) + sizeof(colors) + sizeof(points_cylinder) + sizeof(normals_cylinder) + sizeof(points_sphere) + sizeof(normals_sphere) + sizeof(tex_coords_sphere), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE_OFFSET, sizeof(points_cube), points_cube);
	glBufferSubData(GL_ARRAY_BUFFER, COLOR_OFFSET, sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE_NORMALS_OFFSET, sizeof(normals_cube), normals_cube);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE_TEXCOORDS_OFFSET, sizeof(tex_coords_cube), tex_coords_cube);
	glBufferSubData(GL_ARRAY_BUFFER, CYLINDER_OFFSET, sizeof(points_cylinder), points_cylinder);
	glBufferSubData(GL_ARRAY_BUFFER, CYLINDER_NORMALS_OFFSET, sizeof(normals_cylinder), normals_cylinder);
	glBufferSubData(GL_ARRAY_BUFFER, CYLINDER_TEXCOORDS_OFFSET, sizeof(tex_coords_cylinder), tex_coords_cylinder);
	glBufferSubData(GL_ARRAY_BUFFER, SPHERE_OFFSET, sizeof(points_sphere), points_sphere);
	glBufferSubData(GL_ARRAY_BUFFER, SPHERE_NORMALS_OFFSET, sizeof(normals_sphere), normals_sphere);
	glBufferSubData(GL_ARRAY_BUFFER, SPHERE_TEXCOORDS_OFFSET, sizeof(tex_coords_sphere), tex_coords_sphere);


	//---------------------------------------------------------------------

	// set up vertex arrays
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));

	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));

	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(COLOR_OFFSET));

	vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_TEXCOORDS_OFFSET));

	// Set the value of the fragment shader texture sampler variable (myTextureSampler) to GL_TEXTURE0
	glUniform1i(glGetUniformLocation(program, "myTextureSampler"), 0);

	//---------------------------------------------------------------------

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	current_view_matrix = LookAt(eye, at, up);
}




//----------------------------------------------------------------------------

void
SetMaterial(vec4 ka, vec4 kd, vec4 ks, float s)
{
	glUniform4fv(glGetUniformLocation(program, "ka"), 1, ka);
	glUniform4fv(glGetUniformLocation(program, "kd"), 1, kd);
	glUniform4fv(glGetUniformLocation(program, "ks"), 1, ks);
	glUniform1f(glGetUniformLocation(program, "Shininess"), s);
}

//----------------------------------------------------------------------------

void
SetLight(vec4 lpos, vec4 la, vec4 ld, vec4 ls)
{
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, lpos);
	glUniform4fv(glGetUniformLocation(program, "AmbientLight"), 1, la);
	glUniform4fv(glGetUniformLocation(program, "DiffuseLight"), 1, ld);
	glUniform4fv(glGetUniformLocation(program, "SpecularLight"), 1, ls);
}


//----------------------------------------------------------------------------

mat4 proj_matrix;

mat4 view_matrix;

float r = 5.0;

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//---- lights

	SetLight(vec4(2.0, 2.0, 2.0, 1.0), vec4(0.4, 0.4, 0.4, 1.0), vec4(0.7, 0.7, 0.7, 1.0), vec4(0.7, 0.7, 0.7, 1.0));

	//---- camera intrinsic parameters

	float left = -1.0;
	float right = 1.0;
	float bottom = -1.0;
	float top = 1.0;
	float zNear = 1.0;
	float zFar = 15.0;

	//proj_matrix = Scale(2.0/(right-left), 2.0/(top-bottom), 2.0/(zNear-zFar));
	proj_matrix = Frustum(left, right, bottom, top, zNear, zFar);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, proj_matrix);
	//---- camera extrinsic parameters
	//vec4 up = vec4(0.0, cos(tr_z), 0.0, 0.0);	
	//cout << up << ' ' << normalize(up) << endl;
	//x y z 
	//view_matrix = LookAt(vec4(eye_x, tilt, eye_z, 1.0), vec4(pan, tilt, cos(roll), 1.0), vec4(roll, cos(tr_z), 0.0, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, current_view_matrix);



	//---- camera extrinsic parameters

	float tr_y = Theta3[Yaxis] * M_PI / 180.0;
	float tr_z = Theta3[Zaxis] * M_PI / 180.0;
	float eye_z = r * (cos(tr_z)) * cos(tr_y);
	float eye_x = r * (cos(tr_z)) * sin(tr_y);
	float eye_y = r * sin(tr_z);

	vec4 up = vec4(0.0, cos(tr_z), 0.0, 0.0);
	cout << up << ' ' << normalize(up) << endl;

	view_matrix = LookAt(vec4(eye_x, eye_y, eye_z, 1.0), vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, cos(tr_z), 0.0, 0.0));

//	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view_matrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, current_view_matrix);


	//---- action


	SetLight(vec4(0.0, 0.0, 0.0, 1.0), vec4(0.7, 0.7, 0.7, 1.0), vec4(0.9, 0.9, 0.9, 1.0), vec4(0.5, 0.5, 0.5, 1.0));

	//---- spherical sun
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.0, 1.0), 0.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), true);

	mat4 transform_bube1 = Translate(0.0, 0.0, 0.0) * Scale(3, 3, 3);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_bube1);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);


	//---- spherical planet
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.2, 0.7, 1.0), vec4(0.5, 0.2, 0.7, 1.0), vec4(0.7, 0.2, 0.8, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	mat4 transform_bube2 = RotateY(Theta4[Xaxis]) * Translate(3.0, 0.0, 0.0) * Scale(2.0, -2.0, 2.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_bube2);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);


	//---- spherical planet
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.2, 0.7, 1.0), vec4(0.5, 0.2, 0.7, 1.0), vec4(0.7, 0.2, 0.8, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_bube2 = RotateY(Theta4[Xaxis] + 30) * Translate(4.0, 0.0, 0.0) * Scale(1.0, 1.0, 1.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_bube2);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_NORMALS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);

	//---- cube
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.2, 0.7, 1.0), vec4(0.5, 0.2, 0.7, 1.0), vec4(0.7, 0.2, 0.8, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	mat4 transform_cube = Translate(0.0, -5.0, 0.0) * Scale(20.0, 0.0, 20.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays( GL_TRIANGLES, 0, NumVerticesCube );

	//for (int i = 0; i<6; i++)
	//{
	//	glbindtexture(gl_texture_2d, textures[i % 2]);
	//	glvertexattribpointer(vposition, 4, gl_float, gl_false, 0, buffer_offset(cube_offset + 6 * i * sizeof(vec4)));
	//	glvertexattribpointer(vnormal, 3, gl_float, gl_false, 0, buffer_offset(cube_normals_offset + 6 * i * sizeof(vec3)));
	//	glvertexattribpointer(vtexcoord, 2, gl_float, gl_false, 0, buffer_offset(cube_texcoords_offset + 6 * i * sizeof(vec2)));
	//	gldrawarrays(gl_triangles, 0, 6);
	//}


	//---- cylindrical object
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.7, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	mat4 transform_tube = Translate(0.0, -3.5, 0.0) * Scale(1.5, 2.0, 1.5);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_tube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCylinder);


	

	glutSwapBuffers();
}


//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033:    // Escape key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'w': //move up?
		current_view_matrix = Translate(0.0, 0.0, 0.5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'a': //move left
		current_view_matrix = Translate(0.5, 0.0, 0.0)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 's': //move down?
		current_view_matrix = Translate(0.0, 0.0, -0.5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'd': //move right
		current_view_matrix = Translate(-0.5, 0.0, 0.0)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'p': //pan
		current_view_matrix = RotateY(5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'P': //pan
		current_view_matrix = RotateY(-5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 't': //pan
		current_view_matrix = RotateX(-5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'T': //tilt
		current_view_matrix = RotateX(5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'r': //roll
		current_view_matrix = RotateZ(5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'R': //roll
		current_view_matrix = RotateZ(-5)*current_view_matrix;
		glutPostRedisplay();
		break;
	case 'u':
		//view_matrix = default_view_matrix;
		Theta3[Xaxis] = 0.0;
		Theta3[Yaxis] = 0.0;
		Theta3[Zaxis] = 0.0;
		glutPostRedisplay();
		break;
	}
}




//----------------------------------------------------------------------------

void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
		case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
		case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
		}
	}
}

//----------------------------------------------------------------------------

void
idle(void)
{
	//Theta1[Axis] = fmod(Theta1[Axis]+1, 360.0);
	//Theta2[Axis] = fmod(Theta2[Axis]+2, 360.0);

	Theta4[Xaxis] = fmod(Theta4[Xaxis] + 0.5, 360.0);


	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1920, 1080);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("WHERE IS THE LIGHT????");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}


#endif

#if 1
// spherical camera + persp + lighting + texture
//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "Angel.h"
#include "simplemodels.h"
using namespace std;

GLuint loadBMP_custom(unsigned char** data, int* w, int* h, const char * imagepath);


// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Zaxis;
GLfloat  Theta1[NumAxes] = { 80.0, 0.0, 0.0 };
GLfloat  Theta2[NumAxes] = { 0.0, 30.0, 0.0 };
GLfloat  Theta3[NumAxes] = { 0.0, 0.0, 0.0 };
GLfloat  Theta4[NumAxes] = { 0.0, 0.0, 0.0 };

GLfloat  At[NumAxes] = { 0.0, 0.0, 0.0 };

GLfloat camUp[NumAxes] = { 0.0, 1.0, 0.0 };

GLfloat upPhi = 90.0;
GLfloat at_x = 0.0;
GLfloat at_y = 0.0;
//GLfloat up_y = 1.0;
//GLfloat up_x = 1.0;

int at_xFlag = 1;
int at_yFlag = 1;


size_t CUBE_OFFSET;
size_t COLOR_OFFSET;
size_t CUBE_NORMALS_OFFSET;
size_t CUBE_TEXCOORDS_OFFSET;
size_t CYLINDER_OFFSET;
size_t CYLINDER_NORMALS_OFFSET;
size_t SPHERE_OFFSET;
size_t SPHERE_NORMALS_OFFSET;
size_t SPHERE_TEXCOORDS_OFFSET;


//----------------------------------------------------------------------------

GLuint program;

GLuint vPosition;
GLuint vNormal;
GLuint vColor;
GLuint vTexCoord;

GLuint textures[20];
GLuint spaceShip;


mat4 proj_matrix;
mat4 view_matrix;
mat4 currentView;


float r = 3.5;
float tr_y = Theta3[Yaxis] * M_PI / 180.0;
float tr_z = Theta3[Zaxis] * M_PI / 180.0;
float eye_z = r * (cos(tr_z)) * cos(tr_y);
float eye_x = r * (cos(tr_z)) * sin(tr_y);
float eye_y = r * sin(tr_z);

float crystalRot = 0.0;

void
init()
{
	// Load shaders and use the resulting shader program
	program = InitShader("vshader22.glsl", "fshader22.glsl");
	glUseProgram(program);

	//---------------------------------------------------------------------
	colorcube();
	colortube();
	colorbube();
	//---------------------------------------------------------------------

	//----- generate a checkerboard pattern for texture mapping
	const int  TextureSize = 256;
	GLubyte checker_tex[TextureSize][TextureSize][3];

	for (int i = 0; i < TextureSize; i++)
	{
		for (int j = 0; j < TextureSize; j++)
		{
			//GLubyte c = (((i & 0x4) == 0) ^ ((j & 0x1)  == 0)) * 255;
			GLubyte c = (((j & 0x2) == 0)) * 255;
			checker_tex[i][j][0] = c;
			checker_tex[i][j][1] = c;
			checker_tex[i][j][2] = c;
		}
	}

	//---- Initialize texture objects
	glGenTextures(20, textures);

	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, textures[4]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_BGR, GL_UNSIGNED_BYTE, checker_tex);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);


	unsigned char* pic1 = NULL;
	int w, h;
	loadBMP_custom(&pic1, &w, &h, "yellowMetal.bmp");

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic1);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	unsigned char* pic2 = NULL;
	loadBMP_custom(&pic2, &w, &h, "steelBlue.bmp");

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic2);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	unsigned char* pic3 = NULL;
	//loadBMP_custom(&pic3, &w, &h, "checkerBlue.bmp");

	loadBMP_custom(&pic3, &w, &h, "blueaf.bmp");

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic3);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	unsigned char* mars = NULL;
	loadBMP_custom(&mars, &w, &h, "blue_crate.bmp");

	glBindTexture(GL_TEXTURE_2D, textures[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, mars);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	unsigned char* pic4 = NULL;
	loadBMP_custom(&pic4, &w, &h, "redWood.bmp");

	glBindTexture(GL_TEXTURE_2D, textures[5]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic4);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	unsigned char* pic5 = NULL;
	loadBMP_custom(&pic5, &w, &h, "doorTex.bmp");

	glBindTexture(GL_TEXTURE_2D, textures[6]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic5);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	unsigned char* pic6 = NULL;
	loadBMP_custom(&pic6, &w, &h, "redMetal.bmp");

	glBindTexture(GL_TEXTURE_2D, textures[7]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic6);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	unsigned char* pic7 = NULL;
	loadBMP_custom(&pic7, &w, &h, "spaceDoor.bmp");

	glBindTexture(GL_TEXTURE_2D, textures[8]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic7);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	unsigned char* pic8 = NULL;
	loadBMP_custom(&pic8, &w, &h, "jfg metal copy.bmp");

	glBindTexture(GL_TEXTURE_2D, textures[9]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic8);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	unsigned char* pic9 = NULL;
	loadBMP_custom(&pic9, &w, &h, "yellowScratch.bmp");

	glBindTexture(GL_TEXTURE_2D, textures[10]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic9);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	unsigned char* pic10 = NULL;
	loadBMP_custom(&pic10, &w, &h, "craneCrate.bmp");

	glBindTexture(GL_TEXTURE_2D, textures[11]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, pic10);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	//----set offset variables

	CUBE_OFFSET = 0;
	COLOR_OFFSET = sizeof(points_cube);
	CUBE_NORMALS_OFFSET = COLOR_OFFSET + sizeof(colors);
	CUBE_TEXCOORDS_OFFSET = CUBE_NORMALS_OFFSET + sizeof(normals_cube);
	CYLINDER_OFFSET = CUBE_TEXCOORDS_OFFSET + sizeof(tex_coords_cube);
	CYLINDER_NORMALS_OFFSET = CYLINDER_OFFSET + sizeof(points_cylinder);
	SPHERE_OFFSET = CYLINDER_NORMALS_OFFSET + sizeof(normals_cylinder);
	SPHERE_NORMALS_OFFSET = SPHERE_OFFSET + sizeof(points_sphere);
	SPHERE_TEXCOORDS_OFFSET = SPHERE_NORMALS_OFFSET + sizeof(normals_sphere);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);  // removed 'APPLE' suffix for 3.2
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points_cube) + sizeof(normals_cube) + sizeof(tex_coords_cube) + sizeof(colors) + sizeof(points_cylinder) + sizeof(normals_cylinder) + sizeof(points_sphere) + sizeof(normals_sphere) + sizeof(tex_coords_sphere), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE_OFFSET, sizeof(points_cube), points_cube);
	glBufferSubData(GL_ARRAY_BUFFER, COLOR_OFFSET, sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE_NORMALS_OFFSET, sizeof(normals_cube), normals_cube);
	glBufferSubData(GL_ARRAY_BUFFER, CUBE_TEXCOORDS_OFFSET, sizeof(tex_coords_cube), tex_coords_cube);
	glBufferSubData(GL_ARRAY_BUFFER, CYLINDER_OFFSET, sizeof(points_cylinder), points_cylinder);
	glBufferSubData(GL_ARRAY_BUFFER, CYLINDER_NORMALS_OFFSET, sizeof(normals_cylinder), normals_cylinder);
	glBufferSubData(GL_ARRAY_BUFFER, SPHERE_OFFSET, sizeof(points_sphere), points_sphere);
	glBufferSubData(GL_ARRAY_BUFFER, SPHERE_NORMALS_OFFSET, sizeof(normals_sphere), normals_sphere);
	glBufferSubData(GL_ARRAY_BUFFER, SPHERE_TEXCOORDS_OFFSET, sizeof(tex_coords_sphere), tex_coords_sphere);


	//---------------------------------------------------------------------

	// set up vertex arrays
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));

	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));

	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(COLOR_OFFSET));

	vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_TEXCOORDS_OFFSET));

	// Set the value of the fragment shader texture sampler variable (myTextureSampler) to GL_TEXTURE0
	glUniform1i(glGetUniformLocation(program, "myTextureSampler"), 0);

	//---------------------------------------------------------------------


	//REMOVE IF BROKEN
	vec4 up = vec4(0.0, cos(tr_z), 0.0, 0.0);
	cout << up << ' ' << normalize(up) << endl;

	float ur = upPhi * M_PI / 180.0;
	float up_x = cos(ur);
	float up_y = sin(ur);
	currentView = LookAt(vec4(eye_x, eye_y, eye_z, 1.0), vec4(at_x, at_y, 0.0, 1.0), vec4(up_x, up_y, 0.0, 0.0));
	//


	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}




//----------------------------------------------------------------------------

void
SetMaterial(vec4 ka, vec4 kd, vec4 ks, float s)
{
	glUniform4fv(glGetUniformLocation(program, "ka"), 1, ka);
	glUniform4fv(glGetUniformLocation(program, "kd"), 1, kd);
	glUniform4fv(glGetUniformLocation(program, "ks"), 1, ks);
	glUniform1f(glGetUniformLocation(program, "Shininess"), s);
}

//----------------------------------------------------------------------------

void
SetLight(vec4 lpos, vec4 la, vec4 ld, vec4 ls)
{
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, lpos);
	glUniform4fv(glGetUniformLocation(program, "AmbientLight"), 1, la);
	glUniform4fv(glGetUniformLocation(program, "DiffuseLight"), 1, ld);
	glUniform4fv(glGetUniformLocation(program, "SpecularLight"), 1, ls);
}


//----------------------------------------------------------------------------





void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//---- lights

	SetLight(vec4(0.0, 0.0, 0.0, 1.0), vec4(0.4, 0.4, 0.4, 1.0), vec4(0.7, 0.7, 0.7, 1.0), vec4(0.7, 0.7, 0.7, 1.0));

	float left = -1.0;
	float right = 1.0;
	float bottom = -1.0;
	float top = 1.0;
	float zNear = 1.0;
	float zFar = 100.0;

	proj_matrix = Frustum(left, right, bottom, top, zNear, zFar);

	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, proj_matrix);

	//---- camera extrinsic parameters

	/*float tr_y = Theta3[Yaxis]* M_PI/180.0;
	float tr_z = Theta3[Zaxis]* M_PI/180.0;
	float eye_z = r * (cos(tr_z)) * cos(tr_y);
	float eye_x = r * (cos(tr_z)) * sin(tr_y);
	float eye_y = r * sin(tr_z);*/


	//cout << up << ' ' << normalize(up) << endl;

	view_matrix = currentView;

	//view_matrix = LookAt( vec4(eye_x, eye_y, eye_z, 1.0), vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 0.0));


	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, view_matrix);




	//---- action


	SetLight(vec4(0.0, 29.5, 0.0, 1.0), vec4(0.7, 0.7, 0.7, 1.0), vec4(0.9, 0.9, 0.9, 1.0), vec4(0.7, 0.7, 0.7, 1.0));

	//---- LIGHT
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.9, 0.9, 0.9, 1.0), vec4(0.9, 0.9, 0.9, 1.0), vec4(0.9, 0.9, 0.9, 1.0), 0.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), true);

	mat4 transform_bube1 = Translate(0.0, 29.0, 0.0) * Scale(1.0, 1.0, 1.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_bube1);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesSphere);

	//---- world cube

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	mat4 transform_cube = Translate(0.0, 0.0, 0.0) * Scale(60.0, 60.0, 60.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);


	//---- floor

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(0.0, -29.5, 0.0) * Scale(60.0, 1.0, 60.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- JFG

	glBindTexture(GL_TEXTURE_2D, textures[9]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(0.0, -28.95, 0.0) * Scale(8.0, 0.1, 8.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);


	//---- platform

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(26.2, -22.0, 0.0) * Scale(7.0, 15.0, 60.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- door

	glBindTexture(GL_TEXTURE_2D, textures[6]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(-29.9, -16.5, 0.0) * Scale(0.1, 25.0, 18.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- rolly part of door

	glBindTexture(GL_TEXTURE_2D, textures[7]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(-28.75, -3.0, 0.0) * Scale(2.0, 2.0, 20.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- RAILING

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(22.75, -13.25, 0.0) * Scale(0.125, 0.2, 60.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- RAILING POST MIDDLE

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(22.75, -13.9, 0.0) * Scale(0.125, 1.25, 0.125);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- RAILING POST 1

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(22.75, -13.9, -15.0) * Scale(0.125, 1.25, 0.125);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- RAILING POST 2

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(22.75, -13.9, 15.0) * Scale(0.125, 1.25, 0.125);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crate1 on platform

	glBindTexture(GL_TEXTURE_2D, textures[5]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(28.0, -12.5, -28.0) * Scale(4.0, 4.0, 4.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);


	//---- crate2 on platform

	glBindTexture(GL_TEXTURE_2D, textures[5]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(28.0, -12.5, -23.9) * Scale(4.0, 4.0, 4.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- SET LIGHT FOR GEMINI CRYSTAL1
	SetLight(vec4(28.0, -9.75, -28.0, 1.0), vec4(0.4, 0.4, 0.4, 1.0), vec4(0.7, 0.7, 0.7, 1.0), vec4(0.7, 0.7, 0.7, 1.0));

	//---- crystal on crate1 on platform

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), false);

	SetMaterial(vec4(0.55, 0.0, 0.85, 0.0), vec4(0.55, 0.0, 0.85, 0.0), vec4(0.55, 0.0, 0.85, 0.0), 1.0);
	glUniform1i(glGetUniformLocation(program, "light_out"), true);

	transform_cube = Translate(28.0, -9.75, -28.0) * Scale(0.1, 1.0, 1.0) * RotateY(crystalRot) * RotateX(45);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- SET LIGHT FOR GEMINI CRYSTAL2
	SetLight(vec4(28.0, -9.75, -23.9, 1.0), vec4(0.4, 0.4, 0.4, 1.0), vec4(0.7, 0.7, 0.7, 1.0), vec4(0.7, 0.7, 0.7, 1.0));

	//---- crystal on crate2 on platform

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), false);

	SetMaterial(vec4(0.55, 0.0, 0.85, 1.0), vec4(0.55, 0.0, 0.85, 1.0), vec4(0.55, 0.0, 0.85, 1.0), 1.0);
	glUniform1i(glGetUniformLocation(program, "light_out"), true);

	transform_cube = Translate(28.0, -9.75, -23.9) * Scale(0.1, 1.0, 1.0) * RotateY(crystalRot) * RotateX(45);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);


	//---- RESET LIGHT FOR REST
	SetLight(vec4(2.0, 2.0, 2.0, 1.0), vec4(0.4, 0.4, 0.4, 1.0), vec4(0.7, 0.7, 0.7, 1.0), vec4(0.7, 0.7, 0.7, 1.0));


	//---- WALKWAY PT.1

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(-28.0, 10.0, 23.0) * Scale(4.0, 0.5, 14.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- WALKWAY PT.2

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(-6.0, 10.0, 23.0) * Scale(4.0, 0.5, 14.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- WALKWAY PT. MIDDLE

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(-17.0, 10.0, 18.0) * Scale(18.0, 0.5, 4.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- WALKWAY door 

	glBindTexture(GL_TEXTURE_2D, textures[8]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(-6.0, 14.0, 29.9) * Scale(4.0, 8.0, 0.2) * RotateZ(180);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);


	//---- cylindrical object
	glUniform1i(glGetUniformLocation(program, "texture_on"), false);

	SetMaterial(vec4(0.7, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	mat4 transform_tube = Translate(26.0, -12.5, 26.0) * Scale(3.0, 4.0, 3.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_tube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_NORMALS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCylinder);

	//---- cylindrical object2
	glUniform1i(glGetUniformLocation(program, "texture_on"), false);

	SetMaterial(vec4(0.7, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_tube = Translate(26.0, -12.5, 22.0) * Scale(3.0, 4.0, 3.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_tube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_NORMALS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCylinder);

	//---- crane base and all the other shit

	glBindTexture(GL_TEXTURE_2D, textures[10]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(18.0, -27.5, 20.0) * Scale(3.0, 3.0, 3.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	float yMovement = 2.5;
	for (int i = 0; i < 10; i++)
	{


		glBindTexture(GL_TEXTURE_2D, textures[10]);
		glUniform1i(glGetUniformLocation(program, "texture_on"), true);

		SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
		glUniform1i(glGetUniformLocation(program, "light_out"), false);

		transform_cube = Translate(18.0, -27.5 + yMovement, 20.0) * Scale(2.0, 2.0, 2.0);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
		glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
		yMovement += 2.0;
	}
	float finalY = yMovement - 27.5;
	float craneScale = 1.0;
	float zMovementCrane = 0;
	for (int i = 0; i < 5; i++)
	{

		glBindTexture(GL_TEXTURE_2D, textures[10]);
		glUniform1i(glGetUniformLocation(program, "texture_on"), true);

		SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
		glUniform1i(glGetUniformLocation(program, "light_out"), false);

		transform_cube = Translate(18.0, finalY - (1 - craneScale), 20.0 + zMovementCrane - 0.3) * Scale(2.0 * craneScale, 2.0 * craneScale, 2.0 * craneScale);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
		glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
		zMovementCrane += 2.0 * craneScale;
		craneScale -= 0.1;
	}

	craneScale = 1.0;
	zMovementCrane = 0;
	for (int i = 0; i < 7; i++)
	{

		glBindTexture(GL_TEXTURE_2D, textures[10]);
		glUniform1i(glGetUniformLocation(program, "texture_on"), true);

		SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
		glUniform1i(glGetUniformLocation(program, "light_out"), false);

		transform_cube = Translate(18.0, finalY - (1 - craneScale), 20.0 - zMovementCrane + 0.3) * Scale(2.0 * craneScale, 2.0 * craneScale, 2.0 * craneScale);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
		glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
		zMovementCrane += 2.0 * craneScale;
		craneScale -= 0.05;
	}

	//---- POLE
	zMovementCrane -= 2.0 * craneScale;
	float finalZ = 20.0 - zMovementCrane + 0.4;

	glUniform1i(glGetUniformLocation(program, "texture_on"), false);

	SetMaterial(vec4(0.7, 0.7, 0.7, 1.0), vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_tube = Translate(18.0, finalY - 4.5, finalZ) * Scale(0.1, 8.0, 0.1);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_tube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_NORMALS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCylinder);


	//---- CRANE CRATE

	glBindTexture(GL_TEXTURE_2D, textures[11]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(18.0, finalY - 4.5 - 4.5, finalZ) * Scale(3.0, 3.0, 3.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crate1

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(10.0, -28.0, 16.0) * Scale(2.0, 2.0, 2.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crate2

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(10.0, -28.0, 18.1) * Scale(2.0, 2.0, 2.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crate3 on crate1

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(10.0, -26.0, 16.0) * Scale(2.0, 2.0, 2.0) * RotateY(30);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crate4

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(10.0, -28.0, 20.2) * Scale(2.0, 2.0, 2.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crate5 on crate 2 and 4

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(10.0, -26.0, 19.5) * Scale(2.0, 2.0, 2.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crate6 NEW LOCATION

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(8.0, -27.5, -15.0) * Scale(3.0, 3.0, 3.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crate7

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(4.8, -27.5, -15.0) * Scale(3.0, 3.0, 3.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crate8

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(11.2, -27.5, -15.0) * Scale(3.0, 3.0, 3.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crate9

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(8.0, -27.5, -18.2) * Scale(3.0, 3.0, 3.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crate10

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(4.8, -27.5, -18.2) * Scale(3.0, 3.0, 3.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crate11

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(11.2, -27.5, -18.2) * Scale(3.0, 3.0, 3.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crate12 ON TOP of NEW LOCATION CRATES

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(5.75, -24.0, -16.75) * Scale(4.0, 4.0, 4.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crate13 ON TOP of NEW LOCATION CRATES

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glUniform1i(glGetUniformLocation(program, "texture_on"), true);

	SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
	glUniform1i(glGetUniformLocation(program, "light_out"), false);

	transform_cube = Translate(10.25, -24.0, -16.75) * Scale(4.0, 4.0, 4.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
	glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);

	//---- crates in corner area
	float xMovement = 0.0;
	//---- ground crates
	for (int i = 0; i <= 6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glUniform1i(glGetUniformLocation(program, "texture_on"), true);

		SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
		glUniform1i(glGetUniformLocation(program, "light_out"), false);

		transform_cube = Translate(-28.5 + xMovement, -27.5, 28.5) * Scale(3.0, 3.0, 3.0);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
		glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
		xMovement += 3.1;
	}
	xMovement = 0.0;
	//---- on top of crates
	for (int i = 0; i <= 5; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glUniform1i(glGetUniformLocation(program, "texture_on"), true);

		SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
		glUniform1i(glGetUniformLocation(program, "light_out"), false);

		transform_cube = Translate(-28.5 + xMovement + 1.55, -24.5, 28.5) * Scale(3.0, 3.0, 3.0);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
		glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
		xMovement += 3.1;
	}

	xMovement = 0.0;
	//---- on top of crates2
	for (int i = 0; i <= 4; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glUniform1i(glGetUniformLocation(program, "texture_on"), true);

		SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
		glUniform1i(glGetUniformLocation(program, "light_out"), false);

		transform_cube = Translate(-28.5 + xMovement + 1.55 + 1.55, -21.5, 28.5) * Scale(3.0, 3.0, 3.0);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
		glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
		xMovement += 3.1;
	}

	xMovement = 0.0;
	//---- on top of crates3
	for (int i = 0; i <= 3; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glUniform1i(glGetUniformLocation(program, "texture_on"), true);

		SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
		glUniform1i(glGetUniformLocation(program, "light_out"), false);

		transform_cube = Translate(-28.5 + xMovement + (3 * 1.55), -18.5, 28.5) * Scale(3.0, 3.0, 3.0);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
		glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
		xMovement += 3.1;
	}


	//---- NEW CRATES 
	xMovement = 0.0;
	//---- on top of crates2
	for (int i = 0; i <= 4; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glUniform1i(glGetUniformLocation(program, "texture_on"), true);

		SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
		glUniform1i(glGetUniformLocation(program, "light_out"), false);

		transform_cube = Translate(-28.5 + xMovement + (2 * 1.55), -27.5, 20.0) * Scale(3.0, 3.0, 3.0);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
		glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
		xMovement += 3.1;
	}

	xMovement = 0.0;
	//---- rotated 45 crates
	for (int i = 0; i <= 3; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glUniform1i(glGetUniformLocation(program, "texture_on"), true);

		SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
		glUniform1i(glGetUniformLocation(program, "light_out"), false);

		transform_cube = Translate(-28.5 + xMovement + (2 * 1.55), -24.5, 20.0) * Scale(3.0, 3.0, 3.0) * RotateY(45);
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
		glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
		xMovement += 4.333;
	}

	//---- crates in other corner area
	xMovement = 0.0;
	float zPosition = -28.5;
	//---- ground crates
	for (int i = 0; i <= 4; i++)
	{

		for (int i = 0; i <= 4; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textures[2]);
			glUniform1i(glGetUniformLocation(program, "texture_on"), true);

			SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
			glUniform1i(glGetUniformLocation(program, "light_out"), false);

			transform_cube = Translate(-28.5 + xMovement, -27.5, zPosition) * Scale(3.0, 3.0, 3.0);
			glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

			glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
			glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
			glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
			glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
			xMovement += 3.1;
		}
		zPosition += 3.1;
		xMovement = 0.0;
	}

	xMovement = 0.0;
	zPosition = -28.5;
	//---- ground crates
	for (int i = 0; i <= 3; i++)
	{

		for (int i = 0; i <= 3; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textures[2]);
			glUniform1i(glGetUniformLocation(program, "texture_on"), true);

			SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
			glUniform1i(glGetUniformLocation(program, "light_out"), false);

			transform_cube = Translate(-28.5 + xMovement, -24.5, zPosition) * Scale(3.0, 3.0, 3.0);
			glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

			glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
			glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
			glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
			glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
			xMovement += 3.1;
		}
		zPosition += 3.1;
		xMovement = 0.0;
	}

	xMovement = 0.0;
	zPosition = -28.5;
	//---- ground crates
	for (int i = 0; i <= 2; i++)
	{

		for (int i = 0; i <= 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textures[2]);
			glUniform1i(glGetUniformLocation(program, "texture_on"), true);

			SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
			glUniform1i(glGetUniformLocation(program, "light_out"), false);

			transform_cube = Translate(-28.5 + xMovement, -21.5, zPosition) * Scale(3.0, 3.0, 3.0);
			glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

			glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
			glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
			glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
			glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
			xMovement += 3.1;
		}
		zPosition += 3.1;
		xMovement = 0.0;
	}

	xMovement = 0.0;
	zPosition = -28.5;
	//---- ground crates
	for (int i = 0; i <= 1; i++)
	{

		for (int i = 0; i <= 1; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textures[2]);
			glUniform1i(glGetUniformLocation(program, "texture_on"), true);

			SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
			glUniform1i(glGetUniformLocation(program, "light_out"), false);

			transform_cube = Translate(-28.5 + xMovement, -18.5, zPosition) * Scale(3.0, 3.0, 3.0);
			glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

			glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
			glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
			glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
			glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
			xMovement += 3.1;
		}
		zPosition += 3.1;
		xMovement = 0.0;
	}

	xMovement = 0.0;
	zPosition = -28.5;
	//---- ground crates
	for (int i = 0; i <= 0; i++)
	{

		for (int i = 0; i <= 0; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textures[2]);
			glUniform1i(glGetUniformLocation(program, "texture_on"), true);

			SetMaterial(vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), vec4(0.5, 0.5, 0.5, 1.0), 5.5);
			glUniform1i(glGetUniformLocation(program, "light_out"), false);

			transform_cube = Translate(-28.5 + xMovement, -15.5, zPosition) * Scale(3.0, 3.0, 3.0);
			glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, transform_cube);

			glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET));
			glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET));
			glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET));
			glDrawArrays(GL_TRIANGLES, 0, NumVerticesCube);
			xMovement += 3.1;
		}
		zPosition += 3.1;
		xMovement = 0.0;
	}




	//---- spherical sun

	//glBindTexture( GL_TEXTURE_2D, textures[2] );
	//glUniform1i( glGetUniformLocation(program, "texture_on"), true );

	/*SetMaterial( vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.5, 1.0), vec4(0.9, 0.9, 0.5, 1.0), 0.5);
	glUniform1i( glGetUniformLocation(program, "light_out"), false );

	mat4 transform_bube1 = Translate( 0.0, 0.0, 0.0 ) * Scale(50.0, 50.0, 50.0);
	glUniformMatrix4fv( glGetUniformLocation( program, "model" ), 1, GL_TRUE, transform_bube1 );

	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET) );
	glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_NORMALS_OFFSET) );
	glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_TEXCOORDS_OFFSET) );
	glDrawArrays( GL_TRIANGLES, 0, NumVerticesSphere );


	//---- sky
	glBindTexture( GL_TEXTURE_2D, textures[0] );
	glUniform1i( glGetUniformLocation(program, "texture_on"), true );
	mat4 rot1 = ( RotateX( 0 ) * RotateY( 0 ) * RotateZ( 0 ) );
	mat4 transform = Translate( 0.0, 0.0, 0.0 ) * rot1 * Scale(20.0, 20.0, 20.0);

	glUniformMatrix4fv( glGetUniformLocation( program, "model" ), 1, GL_TRUE, transform );

	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET) );
	glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_NORMALS_OFFSET) );
	glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_TEXCOORDS_OFFSET) );

	glUniform1i( glGetUniformLocation(program, "obj_color_on"), true );
	glUniform4fv( glGetUniformLocation(program, "obj_color"), 1, vec4(0.0, 0.0, 0.0, 1.0) );

	SetMaterial(vec4(0.85, 0.9, 1.0, 1.0), vec4(0.85, 0.9, 1.0, 1.0), vec4(0.85, 0.9, 1.0, 1.0), 500.0);

	glUniform1i( glGetUniformLocation(program, "light_out"), false );
	glDrawArrays( GL_TRIANGLES, 0, NumVerticesSphere );*/

	/*
	//---- spherical planet
	glBindTexture( GL_TEXTURE_2D, textures[0] );
	glUniform1i( glGetUniformLocation(program, "texture_on"), true );

	SetMaterial( vec4(0.5, 0.2, 0.7, 1.0), vec4(0.5, 0.2, 0.7, 1.0), vec4(0.7, 0.2, 0.8, 1.0), 5.5);
	glUniform1i( glGetUniformLocation(program, "light_out"), false );

	mat4 transform_bube2 = RotateY( Theta4[Xaxis] ) * Translate( 3.0, 0.0, 0.0 ) * Scale(2.0, -2.0, 2.0);
	glUniformMatrix4fv( glGetUniformLocation( program, "model" ), 1, GL_TRUE, transform_bube2 );

	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET) );
	glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_NORMALS_OFFSET) );
	glDrawArrays( GL_TRIANGLES, 0, NumVerticesSphere );


	//---- spherical planet
	glBindTexture( GL_TEXTURE_2D, textures[3] );
	glUniform1i( glGetUniformLocation(program, "texture_on"), true );

	SetMaterial( vec4(0.5, 0.2, 0.7, 1.0), vec4(0.5, 0.2, 0.7, 1.0), vec4(0.7, 0.2, 0.8, 1.0), 5.5);
	glUniform1i( glGetUniformLocation(program, "light_out"), false );

	transform_bube2 = RotateY( Theta4[Xaxis]+30 ) * Translate( 4.0, 0.0, 0.0 ) * Scale(1.0, 1.0, 1.0);
	glUniformMatrix4fv( glGetUniformLocation( program, "model" ), 1, GL_TRUE, transform_bube2 );

	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_OFFSET) );
	glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(SPHERE_NORMALS_OFFSET) );
	glDrawArrays( GL_TRIANGLES, 0, NumVerticesSphere );


	//---- cylindrical object
	glUniform1i( glGetUniformLocation(program, "texture_on"), false );

	SetMaterial( vec4(0.7, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.5, 0.5, 1.0), 5.5);
	glUniform1i( glGetUniformLocation(program, "light_out"), false );

	mat4 transform_tube = Translate( 0.0, -2.5, 0.0 ) * Scale(1.5, 2.0, 1.5);
	glUniformMatrix4fv( glGetUniformLocation( program, "model" ), 1, GL_TRUE, transform_tube );

	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_OFFSET) );
	glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CYLINDER_NORMALS_OFFSET) );
	glDrawArrays( GL_TRIANGLES, 0, NumVerticesCylinder );


	//---- cube
	glBindTexture( GL_TEXTURE_2D, textures[4] );
	glUniform1i( glGetUniformLocation(program, "texture_on"), true );

	SetMaterial( vec4(0.5, 0.2, 0.7, 1.0), vec4(0.5, 0.2, 0.7, 1.0), vec4(0.7, 0.2, 0.8, 1.0), 5.5);
	glUniform1i( glGetUniformLocation(program, "light_out"), false );

	mat4 transform_cube = Translate( 0.0, 3.0, 0.0 ) * RotateZ( Theta4[Xaxis] ) * Scale(2.0, -2.0, 2.0);
	glUniformMatrix4fv( glGetUniformLocation( program, "model" ), 1, GL_TRUE, transform_cube );

	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET) );
	glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET) );
	glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET) );
	glDrawArrays( GL_TRIANGLES, 0, NumVerticesCube );


	*/


	/*for(int i=0; i<6; i++)
	{
	glBindTexture( GL_TEXTURE_2D, textures[i%2] );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_OFFSET+6*i*sizeof(vec4)));
	glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_NORMALS_OFFSET + 6*i*sizeof(vec3)));
	glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(CUBE_TEXCOORDS_OFFSET + 6*i*sizeof(vec2)));
	glDrawArrays( GL_TRIANGLES, 0, 6 );
	}*/

	glutSwapBuffers();
}


//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033:    // Escape key
				 //case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;

	case 'p':
		currentView = RotateY(5)*currentView;
		glutPostRedisplay();
		break;
	case 't':
		currentView = RotateX(5)*currentView;
		glutPostRedisplay();
		break;
	case 'z':
		currentView = RotateZ(5)*currentView;
		glutPostRedisplay();
		break;
	case 'P':
		currentView = RotateY(-5)*currentView;
		glutPostRedisplay();
		break;
	case 'T':
		currentView = RotateX(-5)*currentView;
		glutPostRedisplay();
		break;
	case 'Z':
		currentView = RotateZ(-5)*currentView;
		glutPostRedisplay();
		break;
	case 'w':
		currentView = Translate(0.0, 0.0, 0.5) * currentView;
		glutPostRedisplay();
		break;
	case 's':
		currentView = Translate(0.0, 0.0, -0.5) * currentView;
		glutPostRedisplay();
		break;
	case 'q':
		currentView = Translate(0.0, 0.5, 0.0) * currentView;
		glutPostRedisplay();
		break;
	case 'e':
		currentView = Translate(0.0, -0.5, 0.0) * currentView;
		glutPostRedisplay();
		break;
	case 'a':
		currentView = Translate(0.5, 0.0, 0.0) * currentView;
		glutPostRedisplay();
		break;
	case 'd':
		currentView = Translate(-0.5, 0.0, 0.0) * currentView;
		glutPostRedisplay();
		break;
	case 'u':
		//view_matrix = default_view_matrix;
		Theta3[Xaxis] = 0.0;
		Theta3[Yaxis] = 0.0;
		Theta3[Zaxis] = 0.0;
		glutPostRedisplay();
		break;
	case '1':
		Theta3[Xaxis] = 0.0;
		Theta3[Yaxis] = 90.0;
		Theta3[Zaxis] = 0.0;
		glutPostRedisplay();
		break;
	case '2':
		Theta3[Xaxis] = 0.0;
		Theta3[Yaxis] = 0.0;
		Theta3[Zaxis] = 90.0;
		glutPostRedisplay();
		break;
	case '3':
		Theta3[Xaxis] = 0.0;
		Theta3[Yaxis] = 90.0;
		Theta3[Zaxis] = 90.0;
		glutPostRedisplay();
		break;
	case '4':
		Theta3[Xaxis] = 0.0;
		Theta3[Yaxis] = 180.0;
		Theta3[Zaxis] = 180.0;
		glutPostRedisplay();
		break;
	case '5':
		Theta3[Xaxis] = 180.0;
		Theta3[Yaxis] = 180.0;
		Theta3[Zaxis] = 0.0;
		glutPostRedisplay();
		break;
	case '6':
		Theta3[Xaxis] = 180.0;
		Theta3[Yaxis] = 180.0;
		Theta3[Zaxis] = 180.0;
		glutPostRedisplay();
		break;

	}
}




//----------------------------------------------------------------------------

void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
		case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
		case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
		}
	}
}

//----------------------------------------------------------------------------

void
idle(void)
{
	//Theta1[Axis] = fmod(Theta1[Axis]+1, 360.0);
	//Theta2[Axis] = fmod(Theta2[Axis]+2, 360.0);

	//Theta4[Xaxis] = fmod(Theta4[Xaxis]+0.5, 360.0);
	crystalRot = fmod(crystalRot + 0.5, 360.0);

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("????");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}

#endif