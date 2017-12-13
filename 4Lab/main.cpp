//internal includes
#include "common.h"
#include "ShaderProgram.h"
#include "Camera.h"

//External dependencies
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <random>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "SOIL/SOIL.h"
using namespace std;
static const GLsizei WIDTH = 1024, HEIGHT = 1024, TERRAIN_SIZE = 257; //размеры окна

static int filling = 0;
static bool keys[1024]; //массив состояний кнопок - нажата/не нажата
static GLfloat lastX = 400, lastY = 300; //исходное положение мыши
static bool firstMouse = true;
static bool g_captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
static bool g_capturedMouseJustNow = false;
string str = "X";
vector<string> *trees = new vector<string>();
const float PI = 3.14, DEPTH = 6;
float ANGLE = 20, depth = 0;

#define ROUGHNESS 0.17;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

float yy[TERRAIN_SIZE][TERRAIN_SIZE];

Camera camera(float3(11.193, 200, 11.9907));

//функция для обработки нажатий на кнопки клавиатуры
void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//std::cout << key << std::endl;
	switch (key)
	{
	case GLFW_KEY_ESCAPE: //на Esc выходим из программы
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_SPACE: //на пробел переключение в каркасный режим и обратно
		if (action == GLFW_PRESS)
		{
			if (filling == 0)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				filling = 1;
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				filling = 0;
			}
		}
		break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
	default:
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

//функция для обработки клавиш мыши
void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    g_captureMouse = !g_captureMouse;


  if (g_captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    g_capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

//функция для обработки перемещения мыши
void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (firstMouse)
  {
    lastX = float(xpos);
    lastY = float(ypos);
    firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - lastX;
  GLfloat yoffset = lastY - float(ypos);  

  lastX = float(xpos);
  lastY = float(ypos);

  if (g_captureMouse)
    camera.ProcessMouseMove(xoffset, yoffset);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(GLfloat(yoffset));
}

void doCameraMovement(Camera &camera, GLfloat deltaTime)
{
  if (keys[GLFW_KEY_W])
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (keys[GLFW_KEY_A])
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (keys[GLFW_KEY_S])
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (keys[GLFW_KEY_D])
    camera.ProcessKeyboard(RIGHT, deltaTime);
}
// Подгружаем текстуры
/*
    struct texture {
        int ID;
        unsigned int target;
    };
    static const int _nTextures = 20;
    struct texture _textures[_nTextures];
    
void set_texture(const GLuint textureIndex, int textureID, const char* uniformName, GLenum target) {

    /// Create a texture if no ID was passed.
    if(textureID < 0)
        glGenTextures(1, (GLuint*)&textureID);

    /// Bind the newly created texture to the context :
    /// all future texture functions will modify this texture.
    glBindTexture(target, textureID);

    /// Put the texture index value in the Sampler uniform.
    GLuint uniformID = glGetUniformLocation(_programID, uniformName);
    glUniform1i(uniformID, textureIndex);

    _textures[textureIndex].ID = textureID;
    _textures[textureIndex].target = target;

}


void load_texture(const char * imagepath) const {

    /// Read the file.
    if(glfwLoadTexture2D(imagepath, 0)) {

        /// We want to repeat the texture for texture and normal mapping.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        /// Nice trilinear filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

    } else {
        std::cout << "Cannot load texture file : " << imagepath << std::endl;
        exit(EXIT_FAILURE);
    }

void textInit() {
  set_texture(2, -1, "sandTex", GL_TEXTURE_2D);
  load_texture("../../textures/sand.tga");
  set_texture(3, -1, "iceMoutainTex", GL_TEXTURE_2D);
  load_texture("../../textures/dordona_range.tga");
  set_texture(4, -1, "treeTex", GL_TEXTURE_2D);
  load_texture("../../textures/Mossy_Rock.tga");
  set_texture(5, -1, "stoneTex", GL_TEXTURE_2D);
  load_texture("../../textures/Fault_Zone.tga");
  set_texture(6, -1, "underWaterTex", GL_TEXTURE_2D);
  load_texture("../../textures/under_water.tga");
  set_texture(7, -1, "snowTex", GL_TEXTURE_2D);
  load_texture("../../textures/snow.tga");
}
*/
// Алгоритм для генерации деревьев
/*
void expand(float num){
  string ch = "";

  for (int i = 0; i < str.length(); i++){
    ch = str.at(i);

    if (ch.compare("D") == 0){
      str.replace(i, 1, "DD");
      i = i + 1;
    } else if (ch.compare("X") == 0){
      
      if (num < 0.4){
        //LSystem.replace(i, 1, "D[LX]D[RX]LX");
      str.replace(i, 1, "D[LXV]D[RXV]LX");

      } else {
        //LSystem.replace(i, 1, "D[RX]D[LX]RX");
        str.replace(i, 1, "D[RXV]D[LXV]RX");
      }
      i = i + 13; //11
    } 

  } 
  trees->push_back(str);
}

void draw(){
  string ch = "";
  string LSystem = trees->at(depth);
  for (int i = 0; i < LSystem.length(); i++){
    ch = LSystem.at(i);

    if (ch.compare("D") == 0 || ch.compare("X") == 0){
      drawLine();
    } else if (ch.compare("[") == 0){
      push();
    } else if (ch.compare("]") == 0){
      pop();
    } else if (ch.compare("V") == 0){
      leaf();
    } else if (ch.compare("R") == 0){
      rotR();
    } else if (ch.compare("L") == 0){
      rotL();
    }
  }
}
*/
// Алгоритм для генерации ландшафта
float randTerrain(int size) {
  return (float)(rand() % size - size / 2) * ROUGHNESS;
}

float checkYY(int i, int j) {
  if(i < 0 || j < 0 || i > TERRAIN_SIZE - 1 || j > TERRAIN_SIZE - 1) {
    return 0;
  } else {
    return yy[i][j];
  }
}

void squareDiamond(float yy[][TERRAIN_SIZE], int size) {
    // Центр квадрата
    for (int start_i = size/2; start_i < TERRAIN_SIZE; start_i += size)
    for (int start_j = size/2; start_j < TERRAIN_SIZE; start_j += size)
      yy[start_i][start_j] = (yy[start_i - size / 2][start_j - size / 2]      + 
                              yy[start_i - size / 2][start_j + size / 2]      + 
                              yy[start_i + size / 2][start_j - size / 2]      + 
                              yy[start_i + size / 2][start_j + size / 2]) / 4 +
                              randTerrain(size);

    // Середины сторон
    for (int start_i = 0; start_i < TERRAIN_SIZE - 1; start_i += size)
    for (int start_j = 0; start_j < TERRAIN_SIZE - 1; start_j += size) {
      yy[start_i][start_j + size / 2] = (checkYY(start_i - size / 2, start_j + size / 2) +
                                         checkYY(start_i + size / 2, start_j + size / 2) +
                                         checkYY(start_i, start_j)                       +
                                         checkYY(start_i, start_j + size)) / 4           +
                                         randTerrain(size);
      
      yy[start_i + size / 2][start_j] = (checkYY(start_i + size, start_j)                     +
                                         checkYY(start_i, start_j)                            +
                                         checkYY(start_i + size / 2, start_j - size / 2)      +
                                         checkYY(start_i + size / 2, start_j + size / 2)) / 4 +
                                         randTerrain(size);

      yy[start_i + size][start_j + size / 2] = (checkYY(start_i + size * 3 / 2, start_j + size / 2) +
                                                checkYY(start_i + size / 2,     start_j + size / 2) +
                                                checkYY(start_i + size, start_j)                    +
                                                checkYY(start_i + size, start_j + size)) / 4        +
                                                randTerrain(size);
      yy[start_i + size / 2][start_j + size]= (checkYY(start_i + size, start_j + size)                  +
                                               checkYY(start_i, start_j + size)                         +
                                               checkYY(start_i + size / 2, start_j + size / 2)          +
                                               checkYY(start_i + size / 2, start_j + size * 3 / 2)) / 4 +
                                               randTerrain(size);
  }
}

void terrainGeneration() {
  yy[0][0]                               = randTerrain(TERRAIN_SIZE);
  yy[0][TERRAIN_SIZE - 1]                = randTerrain(TERRAIN_SIZE);
  yy[TERRAIN_SIZE - 1][0]                = randTerrain(TERRAIN_SIZE);
  yy[TERRAIN_SIZE - 1][TERRAIN_SIZE - 1] = randTerrain(TERRAIN_SIZE);
  int size = TERRAIN_SIZE - 1;
  while (size > 1) {
    squareDiamond(yy, size);
    size /= 2;
  }
}
/*
\brief создать triangle strip плоскость и загрузить её в шейдерную программу
\param rows - число строк
\param cols - число столбцов
\param size - размер плоскости
\param vao - vertex array object, связанный с созданной плоскостью
*/
static int createTriStrip(int rows, int cols, float size, GLuint &vao)
{

  int numIndices = 2 * cols*(rows - 1) + rows - 1;

  std::vector<GLfloat> vertices_vec; //вектор атрибута координат вершин
  vertices_vec.reserve(rows * cols * 3);

  std::vector<GLfloat> normals_vec; //вектор атрибута нормалей к вершинам
  normals_vec.reserve(rows * cols * 3);

  std::vector<GLfloat> texcoords_vec; //вектор атрибут текстурных координат вершин
  texcoords_vec.reserve(rows * cols * 2);

  std::vector<float3> normals_vec_tmp(rows * cols, float3(0.0f, 0.0f, 0.0f)); //временный вектор нормалей, используемый для расчетов

  std::vector<int3> faces;         //вектор граней (треугольников), каждая грань - три индекса вершин, её составляющих; используется для удобства расчета нормалей
  faces.reserve(numIndices / 3);

  std::vector<GLuint> indices_vec; //вектор индексов вершин для передачи шейдерной программе
  indices_vec.reserve(numIndices);

  terrainGeneration();

  for (int z = 0; z < rows; ++z)
  {
    for (int x = 0; x < cols; ++x)
    {
      vertices_vec.push_back(x);
      vertices_vec.push_back(yy[z][x]);
      vertices_vec.push_back(z);

      texcoords_vec.push_back(x / float(cols - 1)); // вычисляем первую текстурную координату u, для плоскости это просто относительное положение вершины
      texcoords_vec.push_back(z / float(rows - 1)); // аналогично вычисляем вторую текстурную координату v
    }
  }
  //primitive restart - специальный индекс, который обозначает конец строки из треугольников в triangle_strip
  //после этого индекса формирование треугольников из массива индексов начнется заново - будут взяты следующие 3 индекса для первого треугольника
  //и далее каждый последующий индекс будет добавлять один новый треугольник пока снова не встретится primitive restart index

  int primRestart = cols * rows;

  for (int x = 0; x < cols - 1; ++x)
  {
    for (int z = 0; z < rows - 1; ++z)
    {
      int offset = x*cols + z;

      //каждую итерацию добавляем по два треугольника, которые вместе формируют четырехугольник
      if (z == 0) //если мы в начале строки треугольников, нам нужны первые четыре индекса
      {
        indices_vec.push_back(offset + 0);
        indices_vec.push_back(offset + rows);
        indices_vec.push_back(offset + 1);
        indices_vec.push_back(offset + rows + 1);
      }
      else // иначе нам достаточно двух индексов, чтобы добавить два треугольника
      {
        indices_vec.push_back(offset + 1);
        indices_vec.push_back(offset + rows + 1);

        if (z == rows - 2) indices_vec.push_back(primRestart); // если мы дошли до конца строки, вставляем primRestart, чтобы обозначить переход на следующую строку
      }
    }
  }

  ///////////////////////
  //формируем вектор граней(треугольников) по 3 индекса на каждый
  int currFace = 1;
  for (int i = 0; i < indices_vec.size() - 2; ++i)
  {
    int3 face;

    int index0 = indices_vec.at(i);
    int index1 = indices_vec.at(i + 1);
    int index2 = indices_vec.at(i + 2);

    if (index0 != primRestart && index1 != primRestart && index2 != primRestart)
    {
      if (currFace % 2 != 0) //если это нечетный треугольник, то индексы и так в правильном порядке обхода - против часовой стрелки
      {
        face.x = indices_vec.at(i);
        face.y = indices_vec.at(i + 1);
        face.z = indices_vec.at(i + 2);

        currFace++;
      }
      else //если треугольник четный, то нужно поменять местами 2-й и 3-й индекс;
      {    //при отрисовке opengl делает это за нас, но при расчете нормалей нам нужно это сделать самостоятельно
        face.x = indices_vec.at(i);
        face.y = indices_vec.at(i + 2);
        face.z = indices_vec.at(i + 1);

        currFace++;
      }
      faces.push_back(face);
    }
  }


  ///////////////////////
  //расчет нормалей
  for (int i = 0; i < faces.size(); ++i)
  {
    //получаем из вектора вершин координаты каждой из вершин одного треугольника
    float3 A(vertices_vec.at(3 * faces.at(i).x + 0), vertices_vec.at(3 * faces.at(i).x + 1), vertices_vec.at(3 * faces.at(i).x + 2));
    float3 B(vertices_vec.at(3 * faces.at(i).y + 0), vertices_vec.at(3 * faces.at(i).y + 1), vertices_vec.at(3 * faces.at(i).y + 2));
    float3 C(vertices_vec.at(3 * faces.at(i).z + 0), vertices_vec.at(3 * faces.at(i).z + 1), vertices_vec.at(3 * faces.at(i).z + 2));

    //получаем векторы для ребер треугольника из каждой из 3-х вершин
    float3 edge1A(normalize(B - A));
    float3 edge2A(normalize(C - A));

    float3 edge1B(normalize(A - B));
    float3 edge2B(normalize(C - B));

    float3 edge1C(normalize(A - C));
    float3 edge2C(normalize(B - C));

    //нормаль к треугольнику - векторное произведение любой пары векторов из одной вершины
    float3 face_normal = cross(edge1A, edge2A);

    //простой подход: нормаль к вершине = средняя по треугольникам, к которым принадлежит вершина
    normals_vec_tmp.at(faces.at(i).x) += face_normal;
    normals_vec_tmp.at(faces.at(i).y) += face_normal;
    normals_vec_tmp.at(faces.at(i).z) += face_normal;
  }

  //нормализуем векторы нормалей и записываем их в вектор из GLFloat, который будет передан в шейдерную программу
  for (int i = 0; i < normals_vec_tmp.size(); ++i)
  {
    float3 N = normalize(normals_vec_tmp.at(i));

    normals_vec.push_back(N.x);
    normals_vec.push_back(N.y);
    normals_vec.push_back(N.z);
  }


  GLuint vboVertices, vboIndices, vboNormals, vboTexCoords;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vboVertices);
  glGenBuffers(1, &vboIndices);
  glGenBuffers(1, &vboNormals);
  glGenBuffers(1, &vboTexCoords);

  glBindVertexArray(vao); GL_CHECK_ERRORS;
  {

    //передаем в шейдерную программу атрибут координат вершин
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, vertices_vec.size() * sizeof(GL_FLOAT), &vertices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(0); GL_CHECK_ERRORS;

    //передаем в шейдерную программу атрибут нормалей
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, normals_vec.size() * sizeof(GL_FLOAT), &normals_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(1); GL_CHECK_ERRORS;

    //передаем в шейдерную программу атрибут текстурных координат
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, texcoords_vec.size() * sizeof(GL_FLOAT), &texcoords_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(2); GL_CHECK_ERRORS;

    //передаем в шейдерную программу индексы
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices); GL_CHECK_ERRORS;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_vec.size() * sizeof(GLuint), &indices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;

    glEnable(GL_PRIMITIVE_RESTART); GL_CHECK_ERRORS;
    glPrimitiveRestartIndex(primRestart); GL_CHECK_ERRORS;
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);


  return numIndices;
}


int initGL()
{
	int res = 0;

	//грузим функции opengl через glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	//выводим в консоль некоторую информацию о драйвере и контексте opengl
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "press left mose button to capture/release mouse cursor  "<< std::endl;
  std::cout << "press spacebar to alternate between shaded wireframe and fill display modes" << std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}

int main(int argc, char** argv)
{

  srand(time(NULL));
	if(!glfwInit())
    return -1;

	//запрашиваем контекст opengl версии 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 


  GLFWwindow*  window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL basic sample", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 

	//регистрируем коллбеки для обработки сообщений от пользователя - клавиатура, мышь..
	glfwSetKeyCallback        (window, OnKeyboardPressed);  
	glfwSetCursorPosCallback  (window, OnMouseMove); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback     (window, OnMouseScroll);
	glfwSetInputMode          (window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

	if(initGL() != 0) 
		return -1;
	
  //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

	//создание шейдерной программы из двух файлов с исходниками шейдеров
	//используется класс-обертка ShaderProgram
	std::unordered_map<GLenum, std::string> shaders;
	shaders[GL_VERTEX_SHADER]   = "vertex.glsl";
	shaders[GL_FRAGMENT_SHADER] = "fragment.glsl";
	ShaderProgram program(shaders); GL_CHECK_ERRORS;


  //типа текстуры
  int width, height;
  GLuint textureGrass;
  GLuint textureWater;
  GLuint textureSnow;
  GLuint textureSand;
  GLuint textureRock;
  unsigned char* image;

  //типа зелень
  image = SOIL_load_image("./textures/grass.jpg", &width, &height, 0, SOIL_LOAD_RGB);
  glGenTextures(1, &textureGrass);
  glBindTexture(GL_TEXTURE_2D, textureGrass);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  //типа вода
  image = SOIL_load_image("./textures/water.jpg", &width, &height, 0, SOIL_LOAD_RGB);
  glGenTextures(1, &textureWater);
  glBindTexture(GL_TEXTURE_2D, textureWater);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);

  //типа снег
  image = SOIL_load_image("./textures/snow.jpg", &width, &height, 0, SOIL_LOAD_RGB);
  glGenTextures(1, &textureSnow);
  glBindTexture(GL_TEXTURE_2D, textureSnow);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);

  //типа песок 
  image = SOIL_load_image("./textures/sand.jpg", &width, &height, 0, SOIL_LOAD_RGB);
  glGenTextures(1, &textureSand);
  glBindTexture(GL_TEXTURE_2D, textureSand);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);

  //Типа камни
  image = SOIL_load_image("./textures/rock.jpg", &width, &height, 0, SOIL_LOAD_RGB);
  glGenTextures(1, &textureRock);
  glBindTexture(GL_TEXTURE_2D, textureRock);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);

  //Создаем и загружаем геометрию поверхности
  GLuint vaoTriStrip;
  int triStripIndices = createTriStrip(TERRAIN_SIZE, TERRAIN_SIZE, 40, vaoTriStrip);
  
  glViewport(0, 0, WIDTH, HEIGHT);  GL_CHECK_ERRORS;
  glEnable(GL_DEPTH_TEST);  GL_CHECK_ERRORS;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//цикл обработки сообщений и отрисовки сцены каждый кадр
	while (!glfwWindowShouldClose(window))
	{
		//считаем сколько времени прошло за кадр
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
    doCameraMovement(camera, deltaTime);

		//очищаем экран каждый кадр
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); GL_CHECK_ERRORS;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;

    program.StartUseShader(); GL_CHECK_ERRORS;

		//обновляем матрицы камеры и проекции каждый кадр
    float4x4 view       = camera.GetViewMatrix();
    float4x4 projection = projectionMatrixTransposed(camera.zoom, float(WIDTH) / float(HEIGHT), 0.1f, 1000.0f);
    // cout << camera.pos.x << " " << camera.pos.y << " "<< camera.pos.z << endl;
		                //модельная матрица, определяющая положение объекта в мировом пространстве
		float4x4 model; //начинаем с единичной матрицы

    program.StartUseShader();

    //загружаем uniform-переменные в шейдерную программу (одинаковые для всех параллельно запускаемых копий шейдера)
    program.SetUniform("view",       view);       GL_CHECK_ERRORS;
    program.SetUniform("projection", projection); GL_CHECK_ERRORS;
    program.SetUniform("model",      model);
    
    
    //рисуем плоскость
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureWater);
    program.SetUniform("ourTexture1", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureSand);
    program.SetUniform("ourTexture2", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureGrass);
    program.SetUniform("ourTexture3", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textureRock);
    program.SetUniform("ourTexture4", 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, textureSnow);
    program.SetUniform("ourTexture5", 4);


    glBindVertexArray(vaoTriStrip);
    glDrawElements(GL_TRIANGLE_STRIP, triStripIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
    glBindVertexArray(0); GL_CHECK_ERRORS;

    program.StopUseShader();

		glfwSwapBuffers(window); 
	}

	//очищаем vao перед закрытием программы
	glDeleteVertexArrays(1, &vaoTriStrip);

	glfwTerminate();
	return 0;
}
