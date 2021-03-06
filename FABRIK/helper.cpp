#include <cstring>
#include <utility>
#include <map>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <AntTweakBar.h>

//Change png++ to a header only include
//#include "png++\png.hpp"

#include "helper.h"

//defines
#define M_PI 3.14159265359

extern cameraSystem cam;

extern glm::vec3 lightDir;
extern glm::mat4 projMatrix;
extern glm::mat4 viewMatrix;

//glut stuff
void keyboard(unsigned char key, int x, int y) {
	if (TwEventKeyboardGLUT(key, x, y)) {
		glutPostRedisplay();
		return;
	}

	switch(key) {

	case 27:
		std::cout << "exit" << std::endl;
		exit(0);
		break;
	case 'w':
		cam.moveForward(cam.delta);
		updateCamera();
		break;
	case 's':
		cam.moveBackward(cam.delta);
		updateCamera();
		break;
	case 'a':
		cam.moveLeft(cam.delta);
		updateCamera();
		break;
	case 'd':
		cam.moveRight(cam.delta);
		updateCamera();
		break;
	case 'q':
		cam.roll(0.01f*cam.mouseDelta);
		updateCamera();
		break;
	case 'e':
		cam.roll(0.01f*-cam.mouseDelta);
		updateCamera();
		break;
	case 'r':
		loadShader(false);
		break;
	case 'c':
		const glm::vec4& d = cam.viewDir;
		const glm::vec4& u = cam.upDir;
		const glm::vec4& p = cam.position;
		std::cout << "cam-dir: " << d.x << ", " << d.y << ", " << d.z << std::endl <<
		             "cam-up:  " << u.x << ", " << u.y << ", " << u.z << std::endl <<
					 "cam-pos: " << p.y << ", " << p.y << ", " << p.z << std::endl;
		break;
	}
	glutPostRedisplay();
}

void onMouseDown(int button, int state, int x, int y) {
	if (TwEventMouseButtonGLUT(button, state, x, y)) {
		glutPostRedisplay();
		return;
	}

	switch(button) {
	case 4:
		cam.delta *= 0.75f;
		break;
	case 3:
		cam.delta *= (4.f / 3.f);
		break;
	case 2:
		if (state == GLUT_DOWN)
			cam.rightMouseDown = true;
		else
			cam.rightMouseDown = false;
		break;
	case 1:
		if (state == GLUT_DOWN)
			cam.middleMouseDown = true;
		else
			cam.middleMouseDown = false;
		break;
	}

	cam.currentX = x;
	cam.currentY = y;
}


void onMouseMove(int x, int y) {
	if (TwEventMouseMotionGLUT(x, y)) {
		glutPostRedisplay();
		return;
	}

	const float dx = (cam.currentX-x);
	const float dy = (cam.currentY-y);

	if (cam.rightMouseDown) {
		cam.moveUp(-dy*0.03f);
		cam.moveRight(dx*0.03f);
	}
	else if (cam.middleMouseDown) {
		cam.moveForward(dy*0.1f*cam.mouseDelta);
		cam.roll(dx*0.001f*cam.mouseDelta);
	}
	else {
		cam.yaw(-dx*0.001f*cam.mouseDelta);
		cam.pitch(-dy*0.001f*cam.mouseDelta);
	}


	cam.currentX = x;
	cam.currentY = y;

	updateCamera();
}

void onIdle() {}

void initGL() {
	gl_check_error("before init GL");
	glClearDepth(1);
	glClearColor(0.1, 0.4, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST); // turn on the depth test

	loadShader(true);
	gl_check_error("shader built");

	const glm::vec3 eye =    glm::vec3(cam.position);
	const glm::vec3 center = glm::vec3(cam.position + cam.viewDir);
	const glm::vec3 up =     glm::vec3(cam.upDir);

	viewMatrix = glm::lookAt(eye,center,up);
	projMatrix =  glm::perspective(70.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 1.0f, 100.0f);
	reshape(WIDTH, HEIGHT);

	lightDir = glm::normalize(glm::vec3(0.f, 0.f, 1.f));
}

void updateCamera() {
	const glm::vec3 eye =    glm::vec3(cam.position);
	const glm::vec3 center = glm::vec3(cam.position + cam.viewDir);
	const glm::vec3 up =     glm::vec3(cam.upDir);
	viewMatrix = glm::lookAt(eye,center,up);
	glutPostRedisplay();
}



void reshape(int w, int h) {
	static bool initialization= true;

	if (initialization) {
		glViewport(0,0,(GLsizei)w, (GLsizei)h);
		projMatrix = glm::perspective(70.0f, (GLfloat)w / (GLfloat)h, 1.0f, 100.0f);
		initialization = false;
	}

	TwWindowSize(w,h);
}


// uniform helper
void uniform(int program, const std::string &name, const glm::mat4 &mat) {
	int loc = glGetUniformLocation(program, name.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(mat));
}
	
void uniform(int program, const std::string &name, const glm::vec4 &v) {
	int loc = glGetUniformLocation(program, name.c_str());
	glUniform4fv(loc, 1, value_ptr(v));
}
	
void uniform(int program, const std::string &name, const glm::vec3 &v) {
	int loc = glGetUniformLocation(program, name.c_str());
	glUniform3fv(loc, 1, value_ptr(v));
}
	
void uniform(int program, const std::string &name, int i) {
	int loc = glGetUniformLocation(program, name.c_str());
	glUniform1i(loc, i);
}

void uniform(int program, const std::string &name, int i, int k) {
	int loc = glGetUniformLocation(program, name.c_str());
	glUniform2i(loc, i ,k);
}

void uniform(int program, const std::string &name, float f) {
	int loc = glGetUniformLocation(program, name.c_str());
	glUniform1f(loc, f);
}

void uniform(int program, const std::string &name, const float* f, const int count) {
	int loc = glGetUniformLocation(program, name.c_str());
	glUniform1fv(loc, count, f);
}

void uniform(int program, const std::string &name, bool b) {
	uniform(program, name, (int)b);
}

bool gl_check_error(const char* arg)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cout << arg << std::endl << "GL error: " << glewGetErrorString(err) << "(" << err << ")" << std::endl;
		std::exit(1);
	}
	return err == 0;
};

static string textFileRead(const char *fileName) 
{
	string fileString;
	string line;
	
	ifstream file(fileName,ios_base::in);

	if (file.is_open()) 
	{
		while (!file.eof()) 
		{
			getline(file, line);
		  	fileString.append(line);
			fileString.append("\n");
		}
		file.close();
	}
	else
		cout<<"Unable to open "<<fileName<<"\n";

    return fileString;
}

bool createProgram_VF(const char *VSfile, const char *FSfile, GLuint *handle)
{
	
	GLint compiled;
	char infoLog[4096];
	int infoLogLength;
	
	string codeVS = textFileRead(VSfile);
	const char *VshaderCode = codeVS.c_str();

	string codeFS = textFileRead(FSfile);
	const char *FshaderCode = codeFS.c_str();


	//compile vertex shader:
	GLuint Vshader= glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(Vshader,1,&VshaderCode,0);
	glCompileShader(Vshader);
	glGetShaderiv(Vshader,GL_COMPILE_STATUS, &compiled);
	if ( !compiled) 
	{
		// Print out the info log
		glGetShaderInfoLog(Vshader, sizeof(infoLog), &infoLogLength, infoLog);
		if(infoLogLength > 0)
		{
			printf("CompileShader() infoLog for Vertex Shader %s \n%s\n",VSfile, infoLog);
		}
		glDeleteShader(Vshader);
		return false;
	}

	//compile Fragment shader:
	GLuint Fshader= glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(Fshader,1,&FshaderCode,0);
	glCompileShader(Fshader);
	glGetShaderiv(Fshader,GL_COMPILE_STATUS, &compiled);
	if ( !compiled) 
	{
		// Print out the info log
		glGetShaderInfoLog(Fshader, sizeof(infoLog), &infoLogLength, infoLog);
		if(infoLogLength > 0)
		{
			printf("CompileShader() infoLog for Fragment Shader %s\n%s\n", FSfile, infoLog);
		}
		glDeleteShader(Fshader);
		return false;
	}


	*handle = glCreateProgram();
	glAttachShader(*handle, Vshader);
	glAttachShader(*handle, Fshader);
	glDeleteShader(Vshader);
	glDeleteShader(Fshader);
	glLinkProgram(*handle);

	return true;
}

// this function creates Shader Program which consists only of a vertex shader
bool createProgram_C(const char *CSfile, GLuint *handle) {

	GLint compiled;
	char infoLog[4096];
	int infoLogLength;
	string code = textFileRead(CSfile);

	const char *CshaderCode = code.c_str();

	//compile vertex shader:
	GLuint Cshader= glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(Cshader,1,&CshaderCode,0);
	glCompileShader(Cshader);
	glGetShaderiv(Cshader,GL_COMPILE_STATUS, &compiled);
	if ( !compiled) {
		// Print out the info log
		glGetShaderInfoLog(Cshader, sizeof(infoLog), &infoLogLength, infoLog);
		if(infoLogLength > 0)
		{
			printf("CompileShader() infoLog %s \n%s\n",CSfile, infoLog);
			glDeleteShader(Cshader);
			return false;
		}
	}

	*handle = glCreateProgram();
	glAttachShader(*handle, Cshader);
	glDeleteShader(Cshader);

	glLinkProgram(*handle);
	return true;
}

// Texture
Tex::Tex(int w, int h, int internal_format, int format, int type) 
	: w(w), h(h) {
		glGenTextures(1,&index);
		cout << "tex id " << index << endl;

		glBindTexture(GL_TEXTURE_2D,index);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, type, 0);
	}

void Tex::Bind() {
	glBindTexture(GL_TEXTURE_2D, index);
}

int Tex::Unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
	return index;
}

GLuint Tex::Index() const { return index; }


// Frame Buffer Object
Fbo::Fbo(const string &name, int w, int h, int attachments) 
	: name(name), w(w), h(h), next_att(0), depthbuffer(0) {
		glGenFramebuffers(1, &id);
	}

void Fbo::Bind() {
	gl_check_error("pre bind");
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	gl_check_error("in bind");
	if (depthbuffer == 0)
		glBindRenderbuffer(GL_RENDERBUFFER, alterante_depthbuffer_id);
	gl_check_error("in bind");
	if (next_att != 0)
		glDrawBuffers(next_att, &attachment_id[0]);
	else
		glDrawBuffer(GL_NONE);
	gl_check_error("post bind");
}

void Fbo::AddTextureAsColorbuffer(const string &name, const Tex *img) {
	attachment_name.push_back(name);
	attachment_texture.push_back(*img);
	attachment_id.push_back(GL_COLOR_ATTACHMENT0+next_att);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_id[next_att], GL_TEXTURE_2D, img->Index(), 0);
	++next_att;
	gl_check_error(("Error binding texture " + name + " to fbo " + Fbo::name).c_str());
}

void Fbo::AddTextureAsDepthbuffer(Tex *img) {
	depthbuffer = img;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, img->Index(), 0);
}

void Fbo::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Fbo::Check() {
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		cerr << "Framebuffer " << name << " is not complete: " << status << endl;
}

// screen space quad
void simpleQuad::upload() {
	vertices = vector<glm::vec3>(4);
	vertices = { glm::vec3(-1, -1, 0),
		         glm::vec3( 1, -1, 0),
				 glm::vec3( 1,  1, 0),
				 glm::vec3(-1,  1, 0) };

	indices = vector<unsigned int>(6);
	indices = { 0, 1, 3, 1, 2, 3 };
	

	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float)*3, vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

void simpleQuad::draw() {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}


// simple Model
simpleModel::simpleModel () {};

simpleModel::simpleModel (const char *filename) {
	std::vector<glm::vec3> normals_tmp;
	std::vector<unsigned int> nIndices;

	FILE * file = fopen(filename, "r");
	if (file == NULL) {
		cerr << "Model file not found: " << filename << endl;
		exit(0);
	}

	while (1) {
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;


		if (strcmp (lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);
		} else if (strcmp (lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			normals_tmp.push_back(normal);
		} else if (strcmp (lineHeader, "f") == 0) {
			unsigned int vIndex[3], uvIndex[3], nIndex[3];
			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vIndex[0], &nIndex[0],
								                                       &vIndex[1], &nIndex[1],
								                                       &vIndex[2], &nIndex[2]);
			if (matches < 6)  {
				cerr << "Information missin in obj file. " << matches  << ", " << lineHeader<< endl;
				exit(0);
			}


			for (int i = 0; i < 3; ++i)  {
				indices.push_back(vIndex[i] - 1);
				nIndices.push_back(nIndex[i] -1);
			}
		}
	}


	std::vector<glm::vec3> extra_verts;
	std::vector<glm::vec3> extra_norms;
	std::map<std::pair<int, int>, int> mapping;

	normals.resize(vertices.size());
	std::vector<bool> tested(vertices.size(), false);

	for (int i = 0; i < indices.size(); ++i) {
		int vIdx = indices[i];
		int nIdx = nIndices[i];

		if (tested[vIdx] && vIdx != nIdx ) {
			auto it = mapping.find(std::pair<int, int>(vIdx, nIdx));
			if (it != mapping.end())
				indices[i] = vertices.size() + it->second -1;
			else {
				extra_verts.push_back(vertices[indices[i]]);
				extra_norms.push_back(normals_tmp[nIndices[i]]);
				indices[i] = vertices.size() + extra_verts.size() -1;
				mapping[std::pair<int, int>(vIdx, nIdx)] = extra_verts.size();
			}
		}
		else {
			normals[indices[i]] = normals_tmp[nIndices[i]];
			tested[indices[i]] = true;
		}
	}

	for (auto it = tested.begin(); it != tested.end(); ++it)
		if (*it != true)
			cout << "probs" << endl;


	vertices.insert(vertices.end(), extra_verts.begin(), extra_verts.end());
	normals.insert(normals.end(), extra_norms.begin(), extra_norms.end());

	mapping.clear();
	extra_verts.clear();
	extra_norms.clear();
	nIndices.clear();
	normals_tmp.clear();

	
	if (vertices.size() != normals.size()) {
		cerr << "Object data (vertices/normals) incensitent." << endl;
		exit(0);
	} else
		cout << "model loaded: " << filename << endl;


}

simpleModel::~simpleModel() {
	glDeleteBuffers(3, vbo);
	indices.clear();
	vertices.clear();
	normals.clear();
}

void simpleModel::upload() {
	glGenBuffers(3, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float)*3, vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float)*3, normals.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

void simpleModel::draw() {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}


groundPlane::groundPlane(const float height, const float width) {
	vertices = vector<glm::vec3>(4);
	vertices = { glm::vec3(-width, height, -width),
		         glm::vec3(-width, height,  width),
				 glm::vec3( width, height,  width),
				 glm::vec3( width, height, -width) };

	normals = vector<glm::vec3>(4, glm::vec3(0.f, 1.f, 0.f));

	indices = vector<unsigned int>(6);
	indices = { 0, 1, 3, 1, 2, 3 };
}

triangleList::triangleList(vector<glm::vec3> _vertices) {
	vertices = _vertices;

	//TODO: Sinnvoll berechnen falls notwendig
	//normals = vector<glm::vec3>(3, glm::vec3(0.f, 1.f, 0.f));
	//indices = { 0,1,2 };

	for (int i = 0; i < vertices.size()/3; i++) {
		indices.push_back(3 * i);
		indices.push_back(3 * i + 1);
		indices.push_back(3 * i + 2);

		glm::vec3 normalVector = glm::cross((vertices[3 * i + 1] - vertices[3 * i]), (vertices[3 * i + 2] - vertices[3 * i]));

		normals.push_back(normalVector);
	}
}

solidTorus::solidTorus(const float r, const float R, const float sides, const float rings) {
	int i, j;
	float theta, phi, theta1;
	float cosTheta, sinTheta;
	float cosTheta1, sinTheta1;
	float ringDelta, sideDelta;


	ringDelta = 2.0 * M_PI / rings;
	sideDelta = 2.0 * M_PI / sides;

	theta = 0.0;
	cosTheta = 1.0;
	sinTheta = 0.0;


	for (i = rings - 1; i >=0; i--) {
		theta1 = theta + ringDelta;
		cosTheta1 = cos(theta1);
		sinTheta1 = sin(theta1);
		phi = 0.0;
		for (j = sides; j >= 0; j--) {
			float cosPhi, sinPhi, dist;

			phi += sideDelta;
			cosPhi = cos(phi);
			sinPhi = sin(phi);
			dist = R + r * cosPhi;

			normals.push_back(glm::vec3(cosTheta1 * cosPhi, -sinTheta1 * cosPhi, sinPhi));
			vertices.push_back(glm::vec3(cosTheta1 * dist, -sinTheta1 * dist, r * sinPhi));

			normals.push_back(glm::vec3(cosTheta * cosPhi, -sinTheta * cosPhi, sinPhi));
			vertices.push_back(glm::vec3(cosTheta * dist, -sinTheta * dist,  r * sinPhi));

			if (j < sides) {
				indices.push_back(vertices.size()-1 -2);
				indices.push_back(vertices.size()-1);
				indices.push_back(vertices.size()-1 -1);

				indices.push_back(vertices.size()-1 -2);
				indices.push_back(vertices.size()-1 -1);
				indices.push_back(vertices.size()-1 -3);
			}
		}

		theta = theta1;
		cosTheta = cosTheta1;
		sinTheta = sinTheta1;

	}
}
	

solidSphere::solidSphere(const float r, const int slices, const int stacks) {
	
	const float dTheta = 2.0*M_PI/(float)stacks;
	const float dPhi = M_PI/(float)slices;  
	
	for (int i = stacks; i>=0; i--) {
		glm::vec2 t(1-i*dTheta/(2.0*M_PI),1.0f);
    	glm::vec3 p(0,r,0);  
		vertices.push_back(p);
		uvs.push_back(t);
    } 

	const int tmpSize = vertices.size();
   
	//North pole
	for (int i = stacks; i>=0; i--) {
		glm::vec2 t(1-i*dTheta/(2.0*M_PI),(M_PI-dPhi)/M_PI);
		glm::vec3 p(r*sin(dPhi)*cos(i*dTheta), r*cos(dPhi), r*sin(dPhi)*sin(i*dTheta));		
		vertices.push_back(p);
		uvs.push_back(t);
    } 

	int triangleID = 0;
	for ( ;triangleID < stacks; triangleID++) 
	{
		indices.push_back(triangleID);
		indices.push_back(triangleID+tmpSize);
		indices.push_back(triangleID+tmpSize+1);
	}

	indices.push_back(stacks-1);
	indices.push_back(stacks*2 -1);
	indices.push_back(stacks);
	
	
	// Middle Part 
	 
	//	v0--- v2
	//  |  	/ |
	//  |  /  | 
	//  | /   |
	//  v1--- v3

	for (int j=1; j<slices-1; j++) 
	{
		for (int i = stacks; i>=0; i--) 
		{    			
			glm::vec2 t = glm::vec2 (1-i*dTheta/(2.0*M_PI),(M_PI-(j+1)*dPhi)/M_PI); 
			glm::vec3 p = glm::vec3 (r*sin((j+1)*dPhi)*cos(i*dTheta), r*cos((j+1)*dPhi), r*sin((j+1)*dPhi)*sin(i*dTheta));
			vertices.push_back(p);
			uvs.push_back(t);

			//add two triangles
 
			indices.push_back(vertices.size()  - stacks-2);	//v0
			indices.push_back(vertices.size() -1);			//v1
			indices.push_back(vertices.size()  - stacks-1);	//v2
 					 
			indices.push_back(vertices.size() - stacks-1);	//v2
			indices.push_back(vertices.size() - 1);			//v1
			indices.push_back(vertices.size() );			//v3
			 
		}
		
	}     

	const int lastVertex=vertices.size()-1;

	//South Pole
	for (int i = stacks; i>=0; i--) {
		glm::vec2 t(1-i*dTheta/(2.0*M_PI),0.0f);
		glm::vec3 p = glm::vec3 (0,-r,0);
		vertices.push_back(p);
		uvs.push_back(t);
    } 

	triangleID = 0;
	for ( ;triangleID < stacks; triangleID++) 
	{
		indices.push_back(lastVertex-stacks+triangleID);
		indices.push_back(lastVertex+triangleID+1);
		indices.push_back(lastVertex-stacks+triangleID+1);
	}

}

void solidSphere::upload() {
	glGenBuffers(3, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float)*3, vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(float)*2, uvs.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

void solidSphere::draw() {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

solidCone::solidCone(const int slices) {

	float height = 1.0f;
	glm::vec3 normalizedDir = glm::vec3(0.0f, 1.0f, 0.0f);

	vertices.push_back(glm::vec3(0.0f));

	float r = 1.0f;
	float angle = M_PI / 4;

	float anglePerSlice = (2 * M_PI) / slices;
	float t = 0;

	vertices.push_back(glm::vec3(r*glm::cos(t), 0.0f, r*glm::sin(t))+ height*normalizedDir);

	for (int i = 2; i < slices + 2; i++) {
		t += anglePerSlice;
		vertices.push_back(glm::vec3(r*glm::cos(t), 0.0f, r*glm::sin(t))+height*normalizedDir);

		indices.push_back(i);
		indices.push_back(i-1);
		indices.push_back(0);

		normals.push_back(glm::cross((vertices[0] - vertices[i]), (vertices[0] - vertices[i-1])));

	}
}

solidCone::solidCone(const float angle, const int slices, const glm::vec3 direction, const float height) {

	glm::vec3 normalizedDir = glm::normalize(direction);

	vertices.push_back(glm::vec3(0.0f));

	float r = height * glm::tan(angle);

	float anglePerSlice = (2 * M_PI) / slices;
	float t = 0;

	vertices.push_back(glm::vec3(r*glm::cos(t), 0.0f, r*glm::sin(t)) + height*normalizedDir);

	for (int i = 2; i < slices + 2; i++) {
		t += anglePerSlice;
		vertices.push_back(glm::vec3(r*glm::cos(t), 0.0f, r*glm::sin(t)) + height*normalizedDir);

		indices.push_back(i);
		indices.push_back(i - 1);
		indices.push_back(0);

		normals.push_back(glm::cross((vertices[0] - vertices[i]), (vertices[0] - vertices[i - 1])));

	}
}

void solidCone::upload() {
	glGenBuffers(3, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float) * 3, normals.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

void solidCone::draw() {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

//SolidEllipticCone
solidEllipticCone::solidEllipticCone(const int slicesPerQuarter, float width0, float width1, float width2, float width3) {
	/*
							|width 1
							|
	width2 -----------------|--------- width 0
							|
							|
							|
							|width 3
	
	*/

	float height = 1.0f;
	glm::vec3 normalizedDir = glm::vec3(0.0f, 1.0f, 0.0f);
	

	float anglePerSlice = (0.5f * M_PI) / slicesPerQuarter;
	float t = 0;

	vertices.push_back(glm::vec3(0.0f));

	glm::vec2 ellipse = calc2dEllipse(width0, width1, t);
	vertices.push_back(glm::vec3(ellipse.x, height, ellipse.y));

	int index = 2;

	for (index; index < slicesPerQuarter + 2; index++) {
		t += anglePerSlice;
		ellipse = calc2dEllipse(width0, width1, t);
		vertices.push_back(glm::vec3(ellipse.x, height, ellipse.y));

		indices.push_back(index);
		indices.push_back(index - 1);
		indices.push_back(0);

		normals.push_back(glm::cross((vertices[0] - vertices[index]), (vertices[0] - vertices[index - 1])));
	}

	for (index; index < 2*slicesPerQuarter + 2; index++) {
		t += anglePerSlice;
		ellipse = calc2dEllipse(width2, width1, t);
		vertices.push_back(glm::vec3(ellipse.x, height, ellipse.y));

		indices.push_back(index);
		indices.push_back(index - 1);
		indices.push_back(0);

		normals.push_back(glm::cross((vertices[0] - vertices[index]), (vertices[0] - vertices[index - 1])));
	}

	for (index; index < 3 * slicesPerQuarter + 2; index++) {
		t += anglePerSlice;
		ellipse = calc2dEllipse(width2, width3, t);
		vertices.push_back(glm::vec3(ellipse.x, height, ellipse.y));

		indices.push_back(index);
		indices.push_back(index - 1);
		indices.push_back(0);

		normals.push_back(glm::cross((vertices[0] - vertices[index]), (vertices[0] - vertices[index - 1])));
	}

	for (index; index < 4 * slicesPerQuarter + 2; index++) {
		t += anglePerSlice;

		ellipse = calc2dEllipse(width0, width3, t);

		vertices.push_back(glm::vec3(ellipse.x, height, ellipse.y));
		indices.push_back(index);
		indices.push_back(index - 1);
		indices.push_back(0);

		normals.push_back(glm::cross((vertices[0] - vertices[index]), (vertices[0] - vertices[index - 1])));
	}

}

void solidEllipticCone::upload() {
	glGenBuffers(3, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float) * 3, normals.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

void solidEllipticCone::draw() {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

glm::vec2 solidEllipticCone::calc2dEllipse(float width, float height, float currentAngle)
{	
	//Web: https://math.stackexchange.com/questions/22064/calculating-a-point-that-lies-on-an-ellipse-given-an-angle
	float x = (width * height) / (glm::sqrt(height*height + width*width*glm::tan(currentAngle)*glm::tan(currentAngle)));
	float y = (width * height * glm::tan(currentAngle)) / (glm::sqrt(height*height + width*width*glm::tan(currentAngle)*glm::tan(currentAngle)));
	
	//Clamp the angle to [0, 2*Pi]
	currentAngle = glm::mod(currentAngle, 2.0f*(float)M_PI);

	if ((currentAngle >= 0 && currentAngle <= M_PI / 2.0f) || (currentAngle >= 1.5f*M_PI && currentAngle <= 2.0f*M_PI)) {
		//std::cout << "debug me 1" << std::endl;
		return glm::vec2(x, y);
	}
	else {
		//std::cout << "debug me 2" << std::endl;
		return glm::vec2(-x, -y);
	}
}

solidCylinder::solidCylinder(const int slices) {

	//glm::vec3 normalizedDir = glm::normalize(direction);

	float r = 0.5f;
	float h = 1.0f;

	float anglePerSlice = (2 * M_PI) / slices;
	float t = 0;

	glm::vec3 cylHeight = glm::vec3(0.0f, h, 0.0f);

	vertices.push_back(glm::vec3(0.0f)); //index 0 bottom mid
	vertices.push_back(cylHeight); //index 1 top mid
	int cv = 2; //center vertices to offset index by


	for (int i = 0; i < slices; i++) {
		glm::vec3 cyclePosition = glm::vec3(r*glm::cos(t), 0.0f, r*glm::sin(t));
		//std::cout << cyclePosition.x << ", " << cyclePosition.y << ", " << cyclePosition.z << std::endl;
		vertices.push_back(cyclePosition);
		vertices.push_back(cyclePosition + cylHeight);
		t += anglePerSlice;
		//std::cout << t << std::endl;
	}

	int numVerts = vertices.size() - 2;
	for (int i = 0; i < numVerts; i+=2) {
		//triangle with base bottom
		indices.push_back(i + cv);
		indices.push_back((i + 2) % numVerts + cv);
		indices.push_back((i + 1) % numVerts + cv);
		
		//triangle with base top
		indices.push_back((i + 2) % numVerts +cv);
		indices.push_back((i + 3) % numVerts +cv);
		indices.push_back((i + 1) % numVerts +cv);

		//triangle for bottom cap
		indices.push_back(i + cv);
		indices.push_back((i + 2) % numVerts + cv);
		indices.push_back(0); //bottom center

		//triangle for top cap
		indices.push_back((i + 1) % numVerts + cv);
		indices.push_back((i + 3) % numVerts + cv);
		indices.push_back(1); //top center
	}

	//for (int i = 0; i < indices.size(); i++)
	//{
	//	std::cout << indices[i] << ", ";
	//}
}

void solidCylinder::upload() {
	glGenBuffers(3, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float) * 3, normals.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

void solidCylinder::draw() {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

solidCircle::solidCircle(const int slices)
{
	vertices.push_back(glm::vec3(0.0f));

	float r = 0.5f;
	float t = 0.0f;
	float anglePerSlice = (2 * M_PI) / slices;

	vertices.push_back(glm::vec3(r*glm::cos(t), 0.0f, r*glm::sin(t)));
	t += anglePerSlice;
	vertices.push_back(glm::vec3(r*glm::cos(t), 0.0f, r*glm::sin(t)));

	for (int i = 2; i < slices + 2; i++) {
		t += anglePerSlice;
		vertices.push_back(glm::vec3(r*glm::cos(t), 0.0f, r*glm::sin(t)));

		indices.push_back(i);
		indices.push_back(i - 1);
		indices.push_back(0);

		normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	}
}

void solidCircle::upload()
{
	glGenBuffers(3, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float) * 3, normals.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

void solidCircle::draw()
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
// Image / Texture
Image::Image(const char* filename) {
	//png::image<png::rgb_pixel> image(filename);
	//height = image.get_height();
	//width = image.get_width();
	//pixels = new unsigned char[width*height*3];
	//for (size_t y = 0; y < height; ++y)
	//	for (size_t x = 0; x < width; ++x) {
	//		png::rgb_pixel px = image.get_pixel(x, height-y-1);
	//		const int i = 3*(y*width+x);
	//		pixels[i+0] = px.red;
	//		pixels[i+1] = px.green;
	//		pixels[i+2] = px.blue;
	//	}
}

Image::~Image() {
	delete[] pixels;
	pixels = NULL;
}

int Image::makeTexture() {
	//GLuint id[1];
	//glGenTextures(1, id);
	//int index = id[0];
	//glBindTexture(GL_TEXTURE_2D, index);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	//return index;

	return 0;
}


// timer

Timer::Timer() {
	gettimeofday(&tv, 0);
	startTime = (double)tv.tv_sec + (double)tv.tv_usec/1000.0;
}

void Timer::update() {
	gettimeofday(&tv, 0);
	double stoppedAt = (double)tv.tv_sec + (double)tv.tv_usec/(1000.0 * 1000.0);
	intervall = stoppedAt - startTime;
	startTime = stoppedAt;
}



/// Camera stuff
cameraSystem::cameraSystem(float delta, float mouseDelta, glm::vec3 pos) : delta(delta), mouseDelta(mouseDelta) {
	position = glm::vec4(pos, 1.0f);
	viewDir = glm::normalize(-position);
	upDir    = glm::vec4(0,1, 0,0);
	rightDir = glm::vec4(glm::normalize(glm::cross(glm::vec3(viewDir), glm::vec3(upDir))), 0.f);
	upDir    = glm::vec4(glm::normalize(glm::cross(glm::vec3(rightDir), glm::vec3(viewDir))), 0.f);

}

void cameraSystem::moveForward(float delta) {
	position = position + (delta*viewDir);
}

void cameraSystem::moveBackward(float delta) {
	position = position - (delta*viewDir);
}

void cameraSystem::moveUp(float delta) {
	position = position + (delta*upDir);
}

void cameraSystem::moveDown(float delta) {
	position = position - (delta*upDir);
}

void cameraSystem::moveRight(float delta) {
	position = position + (delta*rightDir);
}

void cameraSystem::moveLeft(float delta) {
	position = position - (delta*rightDir);
}

void cameraSystem::yaw(float angle) {
	glm::mat4 R = glm::rotate(angle, glm::vec3(upDir)); 
	viewDir = R*viewDir;
	rightDir = R*rightDir;
}

void cameraSystem::pitch(float angle) {
	glm::mat4 R = glm::rotate(angle, glm::vec3(rightDir)); 
	viewDir = R*viewDir;
	upDir = R*upDir;
}

void cameraSystem::roll(float angle) {
	glm::mat4 R = glm::rotate(angle, glm::vec3(viewDir)); 
	rightDir = R*rightDir;
	upDir = R*upDir;
}
