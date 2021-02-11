/*
	UNIVERSIDAD NACIONAL AUTÓNOMA DE MÉXICO
	FACULTAD DE INGENIERÍA
	COMPUTACIÓN GRÁFICA E INTERACCIÓN HUMANO-COMPUTADORA
	PROYECTO FINAL "Sistema Auditivo"
	SEMESTRE 2021-1
	ALUMNOS:
	 - LEYVA BEJARANO MIGUEL ANGEL
	 - 
	 -
	 -
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include"Model.h"
#include "Skybox.h"
#include"SpotLight.h"

const float toRadians = 3.14159265f / 180.0f;
float rotRadio, rotRadioOffset;
bool sentidoRadio;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

// Declaración de cámara
Camera camera;
Camera camera2;

// Delaración de las Texturas
Texture plainTexture;
Texture plainBlackTexture;
Texture pielTexture;
Texture pisoTexture;

//Declaración de materiales
Material Material_brillante;
Material Material_opaco;

//Declaración de luz direccional
DirectionalLight mainLight;

//Declaración de las luces tipo spotlight y pointlights
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

//Declaración de modelos

///Modelo Celula
Model pilares;
Model base_celula;

///Modelo Sistema Auditivo
Model sistema_auditivo;
Model timpano_der;
Model timpano_izq;

///Modelo Escena 1
Model radio;
Model mesa;
Model humano;

///Modelo Escena 2
Model onda;
Model cabeza;



//Declaración del skybox
Skybox skybox;

//Declaración de la variables de tiempo
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

//Posición onda
glm::vec3 pos_onda = glm::vec3(3.0f, 2.0f, 20.5f);

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";
//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount, 
						unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);
		
		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects() 
{
	unsigned int indices[] = {		
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
	//	x      y      z			u	  v			nx	  ny    nz
		-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};


	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main() 
{
	mainWindow = Window(1366, 768); // 1280 x 1024 or  1024 x 768  or 1366 x 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	/*----------------------------POS Y CONFIG DE LA CAMARA----------------------------------*/
									//pos					Up
	camera = Camera(glm::vec3(15.0f, 2.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 7.0f, 1.0f);
	camera2 = Camera(pos_onda, glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 7.0f, 0.7f);


	/*------------------------------------TEXTURAS-------------------------------------------*/
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();

	plainBlackTexture = Texture("Textures/plainBlack.png");
	plainBlackTexture.LoadTextureA();
	
	pielTexture = Texture("Textures/piel.png");
	pielTexture.LoadTextureA();

	pisoTexture = Texture("Textures/piso.png");
	pisoTexture.LoadTextureA();

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	/*------------------------------------MODELOS--------------------------------------------*/

	///Modelos de las primera dos escenas
	radio = Model();
	radio.LoadModel("Models/radio.obj");

	mesa = Model();
	mesa.LoadModel("Models/mesa.obj");

	humano = Model();
	humano.LoadModel("Models/humano.obj");

	cabeza = Model();
	cabeza.LoadModel("Models/cabeza.obj");

	onda = Model();
	onda.LoadModel("Models/onda_sonido.obj");

	///Modelo del sistema auditivo
	sistema_auditivo = Model();
	sistema_auditivo.LoadModel("Models/cerebro.obj");

	timpano_der = Model();
	timpano_der.LoadModel("Models/timpano_der.obj");

	timpano_izq = Model();
	timpano_izq.LoadModel("Models/timpano_izq.obj");

	/// Modelo de la Celula
	pilares = Model();
	pilares.LoadModel("Models/pilares.obj");

	base_celula = Model();
	base_celula.LoadModel("Models/base_celula.obj");


	/*---------------------------------------LUCES--------------------------------------------*/

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
								0.3f, 0.3f,
								0.0f, 0.0f, -1.0f);

	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	//Declaración de primer luz puntual
	pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,  //Color
								0.0f, 3.0f,			 //Intensity
								0.0f, 2.0f, 20.3f,	//pos
								0.3f, 0.2f, 0.1f);  //con, lin, exp
	pointLightCount++;
	
	unsigned int spotLightCount = 0;

	//LUZ DEL ILUMINA TABLERO
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,	//Color
		0.0f, 1.0f,							//Intensity
		0.0f, 200.0f, 0.0f,						//Pos
		0.0f, -1.0f, 0.0f,						//Dir
		1.0f, 0.0f, 0.0f,						//con, lin, exp
		30.0f);									//Edg
	spotLightCount++;

	//linterna
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;


	/*---------------------------------------SKYBOX--------------------------------------------*/


	std::vector<std::string> skyboxFaces;

	skyboxFaces.push_back("Textures/Skybox/neurona_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/neurona_lf.tga");
	
	skyboxFaces.push_back("Textures/Skybox/neurona_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/neurona_up.tga");
	
	skyboxFaces.push_back("Textures/Skybox/neurona_ft.tga");
	skyboxFaces.push_back("Textures/Skybox/neurona_bk.tga");
	
	skybox = Skybox(skyboxFaces);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);
	
	

	/*---------------------------------------POSICIONES OBJETOS --------------------------------------------*/

	glm::vec3 posCelulas[]{
		glm::vec3(15.0f, -2.0f, 2.0f),
		glm::vec3(17.0f, -2.0f, 3.0f),
		glm::vec3(18.0f, -2.0f, 1.0f),
		glm::vec3(17.0f, -2.0f, -2.0f),
		glm::vec3(16.0f, -2.0f, -4.0f),
		glm::vec3(19.0f, -2.0f, -3.0f),
		glm::vec3(18.0f, -2.0f, -5.0f),
		glm::vec3(21.0f, -2.0f, -2.0f),
		glm::vec3(22.0f, -2.0f, -4.0f),
		glm::vec3(24.0f, -2.0f, -4.0f),
		glm::vec3(26.0f, -2.0f, -2.0f),
		glm::vec3(24.0f, -2.0f, -4.0f),
		glm::vec3(22.0f, -2.0f, 2.0f),
		glm::vec3(24.0f, -2.0f, 4.0f),
		glm::vec3(25.0f, -2.0f, 1.0f),
		glm::vec3(26.0f, -2.0f, 3.0f),
		glm::vec3(14.0f, -2.0f, -3.0f),
		glm::vec3(15.0f, -2.0f, -1.0f),
		glm::vec3(20.0f, -2.0f, 2.0f),
		glm::vec3(21.0f, -2.0f, 4.0f),
		glm::vec3(25.0f, -2.0f, 5.0f),
		glm::vec3(23.0f, -2.0f, 0.0f),
		glm::vec3(24.0f, -2.0f, 2.0f),
		glm::vec3(20.0f, -2.0f, 0.0f),

	};


	/*---------------------------------------VARIABLES PARA  ANIMACIONES --------------------------------------------*/

	// Radio
	float rotRadio = 0.0f;
	float rotRadioOffset = 30.0f;
	float sentidoRadio = true;
	float flagRotRadio = 0.0f;

	// Celulas
	float rotCelulas = 0.0f;
	float rotCelulasOffset = 20.0f;
	float sentidoCelulas = true;
	float flagRotCelulas = 0.0f;

	// Onda de sonido
	float movOnda = 23.0f;
	float movOndaY = 2.0f;
	float movOndaY_ = 2.0f;
	float movOndaOffset = 0.5f;
	bool avanza = true;

	// Onda de sonido 2
	float movOnda2 = 3.0f;
	bool avanza2 = true;

	// Timpanos
	float rotTimpano = 0.0f;
	float rotTimpanoOffset = 10.0f;
	float sentidoTimpano = true;
	float flagRotTimpano = 0.0f;


	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		
		//Variables para la animación
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime; 
		lastTime = now;

		
		//Recibir eventos del usuario
		glfwPollEvents();
		// Control de teclado para el cambio de cámara
		if (mainWindow.getCamaraCanica() == GL_TRUE) {  //Camara de canica Activa
			camera2.keyControl(mainWindow.getsKeys(), deltaTime);
			camera2.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		if (mainWindow.getCamaraCanica() == GL_FALSE) { // Camara de tablero activado
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Cargamos el skybox
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();


		/*-------------------------------------------------------------------------------------------*/
		/*-------------------------------- CONTROL DE LUCES -----------------------------------------*/
		/*-------------------------------------------------------------------------------------------*/

		//Asociamos la cámara con la luz de la linterna
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[1].SetFlash(lowerLight, camera.getCameraDirection());

		
		//Cargamos la luces al shader
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		//shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Pender y apagar la linterna (P)
		if (mainWindow.getOnOff() == 1.0) {
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}
		else {
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
		}


		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// Control para el cambio de cámara
		if (mainWindow.getCamaraCanica() == GL_TRUE) {  //Camara de canica Activa
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera2.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera2.getCameraPosition().x, camera2.getCameraPosition().y, camera2.getCameraPosition().z);
		}
		if (mainWindow.getCamaraCanica() == GL_FALSE) { // Camara de tablero activa
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		}

		
		/*-------------------------------------------------------------------------------------------*/
		/*---------------------------------------DIBUJADO--------------------------------------------*/
		/*-------------------------------------------------------------------------------------------*/
		
		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);


		/*---------------------------------------Plano--------------------------------------------*/

		//Plano para las Celulas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pielTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		// Plano para el Radio y Humano
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f, -2.0f, 20.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Plano para Onda de sonido y cabeza
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 20.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Plano para el sistema auditivo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pielTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Plano  general
		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.2f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainBlackTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();*/


		/*---------------------------------------Celulas--------------------------------------------*/

		//Animación
		if (sentidoCelulas) {
			if (flagRotCelulas > 1.0f) { sentidoCelulas = false; flagRotCelulas = 0.0f; }
			else { flagRotCelulas += 1.0f;  rotCelulas = 20.0f; }
		}
		else {
			if (flagRotCelulas > 1.0f) { sentidoCelulas = true; flagRotCelulas = 0.0f; }
			else { flagRotCelulas += 1.0f; rotCelulas = -20.0f; }
		}

		//Modelo
		for (unsigned int i = 0; i < 25; i++)
		{

			model = glm::mat4(1.0);
			model = glm::translate(model, posCelulas[i]);
			model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			base_celula.RenderModel();

			model = glm::mat4(1.0);
			model = glm::translate(model, posCelulas[i]);
			model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
			//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, rotCelulas * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			pilares.RenderModel();

			pointLights[0].SetPos(posCelulas[i]);
		
		}


		/*--------------------------------------- RADIO --------------------------------------------*/

		///Animación
		if (sentidoRadio) {
			if (flagRotRadio > 3.0f) {  sentidoRadio = false; flagRotRadio = 0.0f;}
			else { flagRotRadio += 1.0f;  rotRadio = 30.0f;}		
		}
		else {
			if (flagRotRadio > 3.0f) { sentidoRadio = true; flagRotRadio = 0.0f; }
			else { flagRotRadio += 1.0f; rotRadio = -30.0f; }
		}
		

		///Modelo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.0f, 2.5f, 20.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotRadio * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		radio.RenderModel();


		/*--------------------------------------- MESA --------------------------------------------*/

		///Modelo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.0f, 1.0f, 20.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesa.RenderModel();


		/*--------------------------------------- HUMANO --------------------------------------------*/

		///Modelo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 2.0f, 20.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		humano.RenderModel();

		/*--------------------------------------- CABEZA --------------------------------------------*/

		///Modelo cabeza
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 20.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cabeza.RenderModel();

		/// Animación onda
		if (avanza2) {
			if (movOnda2 > 0.5f) {
				movOnda2 -= movOndaOffset;
				avanza2 = true;
			}
			else { avanza2 = false; }
		}
		else {
			movOnda2 = 3.0f;
			avanza2 = true;
		}

		/// Modelo onda
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movOnda2, 2.0f, 20.3f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		onda.RenderModel();


		/*---------------------------------------Onda --------------------------------------------*/
		/// Animación
		if (avanza) {
			if (movOnda > 16.0f) { 
				movOnda -= movOndaOffset; 
				movOndaY += movOndaOffset; 
				movOndaY_ -= movOndaOffset;  
				avanza = true; 
			}
			else { avanza = false; }
		}
		else {
			movOnda = 23.0f;
			movOndaY = 2.0f;
			movOndaY_ = 2.0f;
			avanza = true;
		}

		///Modelos

		// Movimiento en línea recta
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movOnda + 1.0f, 2.5f, 19.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		onda.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movOnda, 2.0f, 20.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		onda.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movOnda+0.5f, 1.5f, 21.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		onda.RenderModel();

		
		// Diagonal hacia arriba
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movOnda, movOndaY, 18.5f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		onda.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movOnda, movOndaY, 19.5f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		onda.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movOnda, movOndaY, 20.5f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		onda.RenderModel();

		//Diagonal hacia abajo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movOnda, movOndaY_, 18.5f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		onda.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movOnda, movOndaY_, 19.5f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		onda.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movOnda, movOndaY_, 20.5f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		onda.RenderModel();

	


		/*-----------------------------------Sistema Auditivo --------------------------------------------*/
	    
		///Animación timpanos
		if (sentidoTimpano) {
			if (flagRotTimpano > 1.0f) { sentidoTimpano = false; flagRotTimpano = 0.0f; }
			else { flagRotTimpano += 0.07f;  rotTimpano = 0.8f; }
		}
		else {
			if (flagRotTimpano > 1.0f) { sentidoTimpano = true; flagRotTimpano = 0.0f; }
			else { flagRotTimpano += 0.07f; rotTimpano = -0.8f; }
		}

		///Sistema auditivo sin timpano
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		sistema_auditivo.RenderModel();

		///Timpano derecho
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::rotate(model, rotTimpano * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		timpano_der.RenderModel();

		///Timpano izquierdo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::rotate(model, rotTimpano * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		timpano_izq.RenderModel();


		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}