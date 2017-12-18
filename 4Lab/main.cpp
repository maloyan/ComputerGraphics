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
static const GLsizei WIDTH = 1024, HEIGHT = 1024, TERRAIN_SIZE = 513; //размеры окна

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
enum {SKY_LEFT=0,SKY_BACK,SKY_RIGHT,SKY_FRONT,SKY_TOP,SKY_BOTTOM};
unsigned int skybox[6]; //the ids for the textures
int KEYBOARD = 1;
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
    KEYBOARD = 1;
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    KEYBOARD = 2;
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

static int createSkyBox(int size, GLuint &vao) 
{
/*
    GLfloat vertices_vec[] = {
         size,  size, size,   // Top Right
         size, -size, size,   // Bottom Right
        -size, -size, size,   // Bottom Left
        -size,  size, size/*,   // Top Left
         size,  size, -size,  // Top Right
         size, -size, -size,  // Bottom Right
        -size, -size, -size,  // Bottom Left
        -size,  size, -size   // Top Left 
    };
    GLuint indices_vec[] = {  // Note that we start from 0!
        0, 3, 1,
        1, 3, 2/*,
        3, 7, 2,
        2, 7, 6,
        7, 0, 3,
        7, 4, 0,
        2, 5, 6,
        2, 1, 5,
        0, 4, 5,
        0, 5, 1,
        4, 7, 6,
        4, 6, 5
    };

    GLfloat texcoords_vec[] = {
      // front
      0.0, 0.0,
      0.0, 1.0,
      1.0, 0.0,
      1.0, 1.0,
    };

  GLuint vboVertices, vboIndices, vboNormals, vboTexCoords;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vboVertices);
  glGenBuffers(1, &vboIndices);
  glGenBuffers(1, &vboTexCoords);

  glBindVertexArray(vao); GL_CHECK_ERRORS;
  {

    //передаем в шейдерную программу атрибут координат вершин
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_vec), &vertices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(0); GL_CHECK_ERRORS;

    //передаем в шейдерную программу атрибут текстурных координат
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords_vec), &texcoords_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(2); GL_CHECK_ERRORS;

    //передаем в шейдерную программу индексы
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices); GL_CHECK_ERRORS;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_vec), &indices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;

    //glEnable(GL_PRIMITIVE_RESTART); GL_CHECK_ERRORS;
    //glPrimitiveRestartIndex(primRestart); GL_CHECK_ERRORS;
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);


  return sizeof(indices_vec)/sizeof(indices_vec[0]);
*/
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

unsigned char *loadImage(const char* filename, int &width, int &height)
{
  unsigned char* image;
  image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
  return image;
}

GLuint loadTexture(const char* filename)  //load the filename named texture
{
  int width, height;
  GLuint texture;
  unsigned char* image = loadImage(filename, width, height);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);
  return texture;
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

  //типа текстуры
  GLuint textureGrass    = loadTexture("./textures/grass.jpg");
  GLuint textureWater    = loadTexture("./textures/water.jpg");
  GLuint textureSky      = loadTexture("./textures/test.jpg");

  //создание шейдерной программы из двух файлов с исходниками шейдеров
  //используется класс-обертка ShaderProgram
  std::unordered_map<GLenum, std::string> shaders;
  shaders[GL_VERTEX_SHADER]   = "vertex.glsl";
  shaders[GL_FRAGMENT_SHADER] = "fragment.glsl";
  ShaderProgram program(shaders); GL_CHECK_ERRORS;

  //типа небо
  // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] = {
        // Positions                                  // Colors           // Texture Coords
         TERRAIN_SIZE,  TERRAIN_SIZE, TERRAIN_SIZE,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
         TERRAIN_SIZE, -TERRAIN_SIZE, TERRAIN_SIZE,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
        -TERRAIN_SIZE, -TERRAIN_SIZE, TERRAIN_SIZE,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
        -TERRAIN_SIZE,  TERRAIN_SIZE, TERRAIN_SIZE,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // Top Left 
         TERRAIN_SIZE,  TERRAIN_SIZE, -TERRAIN_SIZE,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
         TERRAIN_SIZE, -TERRAIN_SIZE, -TERRAIN_SIZE,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
        -TERRAIN_SIZE, -TERRAIN_SIZE, -TERRAIN_SIZE,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
        -TERRAIN_SIZE,  TERRAIN_SIZE, -TERRAIN_SIZE,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
    };
    GLuint indices[] = {  // Note that we start from 0!
      0, 1, 3, // First Triangle
      1, 2, 3,  // Second Triangle
      3, 7, 2,
        2, 7, 6,
        7, 0, 3,
        7, 4, 0,
        2, 5, 6,
        2, 1, 5,
        0, 4, 5,
        0, 5, 1,
        4, 7, 6,
        4, 6, 5
    };
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO

  std::unordered_map<GLenum, std::string> skybox_shaders;
  skybox_shaders[GL_VERTEX_SHADER]   = "skybox_vertex.glsl";
  skybox_shaders[GL_FRAGMENT_SHADER] = "skybox_fragment.glsl";
  ShaderProgram skybox_program(skybox_shaders); GL_CHECK_ERRORS;

    
  //Создаем и загружаем геометрию поверхности
  GLuint vaoTriStrip;
  int triStripIndices = createTriStrip(TERRAIN_SIZE, TERRAIN_SIZE, 40, vaoTriStrip);
/*
  GLuint vaoSkyBox;
  int skyBoxIndices  = createSkyBox(TERRAIN_SIZE, vaoSkyBox);
*/
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
                    //модельная матрица, определяющая положение объекта в мировом пространстве
    float4x4 model; //начинаем с единичной матрицы

    //загружаем uniform-переменные в шейдерную программу (одинаковые для всех параллельно запускаемых копий шейдера)
    program.SetUniform("view",       view);       GL_CHECK_ERRORS;
    program.SetUniform("projection", projection); GL_CHECK_ERRORS;
    program.SetUniform("model",      model);
    program.SetUniform("state",      KEYBOARD);

    //рисуем плоскость
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureWater);
    program.SetUniform("textureWater", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureGrass);
    program.SetUniform("textureGrass", 1);


    glBindVertexArray(vaoTriStrip);
    glDrawElements(GL_TRIANGLE_STRIP, triStripIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
    glBindVertexArray(0); GL_CHECK_ERRORS;

    program.StopUseShader();

    skybox_program.StartUseShader();

    skybox_program.SetUniform("view",       view);       GL_CHECK_ERRORS;
    skybox_program.SetUniform("projection", projection); GL_CHECK_ERRORS;
    skybox_program.SetUniform("model",      model);
    skybox_program.SetUniform("campos",     camera.pos);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureSky);
    skybox_program.SetUniform("textureSky", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36/*skyBoxIndices*/, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    skybox_program.StopUseShader();


    glfwSwapBuffers(window); 
  }

  //очищаем vao перед закрытием программ
  glDeleteVertexArrays(1, &vaoTriStrip);
  glfwTerminate();
  return 0;
}
