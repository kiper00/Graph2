#include "main.h"

void inittexture() {
	playerArrayTex = stbiloader::GenArray_tex("Texture/1x.png", 1, 7);
	glUseProgram(_ArrayProgram);
	glBindTexture(GL_TEXTURE_2D_ARRAY, playerArrayTex);
	glUniform1i(glGetUniformLocation(_ArrayProgram, "Array_tex"), 0);
}

void initPlayer() {
	player[0] = Point(-300, -300);
	player[1] = Point(-300 + player_Width, -300);

	player[2] = Point(-300 + player_Width, -300 + player_Height);
	player[3] = Point(-300, -300 + player_Height);

	player_UV[0] = Point(0, 0);
	player_UV[1] = Point(1, 0);
	player_UV[2] = Point(1, 1);
	player_UV[3] = Point(0, 1);
}

void init() {
	glClearColor(0.0, 0.0, 0.0, 1);//black screen
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_LIGHTING);
	//glDepthFunc(GL_LESS);
	//glCullFace(GL_BACK);
	//glEnable(GL_CULL_FACE);

	ShaderInfo ArrayTexShader[] = {
		{ GL_VERTEX_SHADER, "Shader/ArrayTexture.vs" },//vertex shader
		{ GL_FRAGMENT_SHADER, "Shader/ArrayTexture.fs" },//fragment shader
		{ GL_NONE, NULL } };

	_ArrayProgram = LoadShaders(ArrayTexShader);

	initPlayer();
	inittexture();
}

#define DOR(angle) (angle*3.1415/180);
mat4 translate(float x, float y, float z) {
	vec4 t = vec4(x, y, z, 1);//w = 1 ,�hx,y,z=0�ɤ]��translate
	vec4 c1 = vec4(1, 0, 0, 0);
	vec4 c2 = vec4(0, 1, 0, 0);
	vec4 c3 = vec4(0, 0, 1, 0);
	mat4 M = mat4(c1, c2, c3, t);
	return M;
}
mat4 scale(float x, float y, float z) {
	vec4 c1 = vec4(x, 0, 0, 0);
	vec4 c2 = vec4(0, y, 0, 0);
	vec4 c3 = vec4(0, 0, z, 0);
	vec4 c4 = vec4(0, 0, 0, 1);
	mat4 M = mat4(c1, c2, c3, c4);
	return M;
}
mat4 rotate(float angle, float x, float y, float z) {
	float r = DOR(angle);
	mat4 M = mat4(1);
	vec4 c1 = vec4(cos(r) + (1 - cos(r))*x*x, (1 - cos(r))*y*x + sin(r)*z, (1 - cos(r))*z*x - sin(r)*y, 0);
	vec4 c2 = vec4((1 - cos(r))*y*x - sin(r)*z, cos(r) + (1 - cos(r))*y*y, (1 - cos(r))*z*y + sin(r)*x, 0);
	vec4 c3 = vec4((1 - cos(r))*z*x + sin(r)*y, (1 - cos(r))*z*y - sin(r)*x, cos(r) + (1 - cos(r))*z*z, 0);
	vec4 c4 = vec4(0, 0, 0, 1);
	M = mat4(c1, c2, c3, c4);
	return M;
}
void updateModels() {
	glm::mat4 Translation = glm::mat4(1.0f);
	glm::mat4 Scale = glm::mat4(1.0f);
	glm::mat4 Rotate = glm::mat4(1.0f);
	Model = glm::mat4(1.0f);

	Model = translate(move_x, jump_y, 0.0);
}



void DrawPlayer()
{
	glUseProgram(_ArrayProgram);
	_Comera = glGetUniformLocation(_ArrayProgram, "cameraPos");
	_Proj = glGetUniformLocation(_ArrayProgram, "proj");
	_View = glGetUniformLocation(_ArrayProgram, "view");
	_Model = glGetUniformLocation(_ArrayProgram, "model");

	glUniform3fv(_Comera, 1, &CameraPos[0]);
	glUniformMatrix4fv(_Proj, 1, GL_FALSE, &Projection[0][0]);
	glUniformMatrix4fv(_View, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(_Model, 1, GL_FALSE, &Model[0][0]);

	glActiveTexture(GL_TEXTURE0);
	if (state == 0) 
		glUniform1i(glGetUniformLocation(_ArrayProgram, "SpriteIndex"), 0);
	else if (state == 1) 
		glUniform1i(glGetUniformLocation(_ArrayProgram, "SpriteIndex"), SpriteIndex);
	
	float tri_pos[] = {
		//position					//UV
		player[0].x, player[0].y,  player_UV[0].x, player_UV[0].y,
		player[1].x, player[1].y,  player_UV[1].x, player_UV[1].y,
		player[2].x, player[2].y,  player_UV[2].x, player_UV[2].y,

		player[0].x, player[0].y,  player_UV[0].x, player_UV[0].y,
		player[2].x, player[2].y,  player_UV[2].x, player_UV[2].y,
		player[3].x, player[3].y,  player_UV[3].x, player_UV[3].y,
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri_pos), &tri_pos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
	glBindVertexArray(0);
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/////////////////////////�]�w�۾�/////////////////////////
	CameraPos = vec3(0.0f, 0.0f, 10.0f);
	Projection = ortho(-300.0f, 300.0f, -300.0f, 300.0f, 0.1f, 100.0f);
	View = lookAt(CameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	updateModels();
	DrawPlayer();

	glFlush();//�j�����W����OpenGL commands
	glutSwapBuffers();//�մ��e�x�M��xbuffer ,����Obuffer�e���M�e�xbuffer�洫�ϧڭ̬ݨ���
}


void PressKey(unsigned char key, int x, int y) {

	switch (key)
	{
	case 'd':
		player_UV[0] = Point(0, 0);
		player_UV[1] = Point(1, 0);
		player_UV[2] = Point(1, 1);
		player_UV[3] = Point(0, 1);
		moveRight = true;
		moveNow = true;
		state = 1;
		break;
	case 'a':
		player_UV[0] = Point(1, 0);
		player_UV[1] = Point(0, 0);
		player_UV[2] = Point(0, 1);
		player_UV[3] = Point(1, 1);
		moveRight = false;
		moveNow = true;
		state = 1;
		break;
	case 's':
		moveNow = false;
		state = 0;
		break;
	}
	glutPostRedisplay();
}


void Timer(int x) {

	if (moveRight && moveNow)
		move_x += player_Speed;
	else if (moveNow)
		move_x -= player_Speed;
 	switch (state)
 	{
 		case 0:
			SpriteIndex = 0;
 			break;
 		case 1:
			SpriteIndex = (SpriteIndex + 1 >= 6) ? 1 : SpriteIndex + 1;
 			break;
 	}

	glutTimerFunc(50, Timer, 0);
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Hello World");

	glewExperimental = GL_TRUE; //�m��glewInit()���e
	if (glewInit()) {
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;//c error
		exit(EXIT_FAILURE);
	}


	glutDisplayFunc(display);
	glutKeyboardFunc(PressKey);
	glutTimerFunc(50, Timer, 0);

	init();


	glutMainLoop();

	system("Pause");
	return 0;
}