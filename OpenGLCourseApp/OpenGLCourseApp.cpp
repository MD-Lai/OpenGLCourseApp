#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cmath> 
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM (make sure to install it)
// #include <glm/mat4x4.hpp> // as example of working import
// be aware glm::mat4 model; no longer works
// replace with glm::mat4 model(1.0f);
// or glm::mat4 model = glm::mat4(1.0f)
// and in subsequent re-inits like model = glm::mat(1.0f); evertime you initialise an identity matrix

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"

// window dimensions
const GLint WIDTH  = 800; // GLint is just an int type defined by opengl
const GLint HEIGHT = 600;

const float toRadians = 3.14159265f / 180.0f;

std::vector<Mesh*> meshList;

GLuint VAO, VBO, IBO, shader; // usually there are multiple vao, vbo for each object, so this is not good practice to global scope them
GLuint uniformModel;
GLuint uniformProjection;

bool direction = true; // true is right
float triOffset = 0.0f; // how much to move triangle, nothing to do with shader itself so not GLfloat or others
float triMaxOffset = 0.7f; // limit before direction switch
float triIncrement = 0.0005f; // how much to move each update

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Vertex shader; usually done in external files
// gl_Position is a built in variable
// defines the ultimate position of a vertex
// fragment shader is where visual effects are usually done, vertex shader is like preprocessing
// gl_Position added weights are to demonstrate that vertex can move vertices around 
static const char* vShader = " \n\
#version 330 \n\
layout (location = 0) in vec3 pos; \n\
out vec4 vCol;\n\
uniform mat4 model; \n\
uniform mat4 projection; \n\
void main(){ \n\
    gl_Position = projection * model * vec4(pos, 1.0); \n\
    vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f); \n\
}";

// Fragment shader
// there is only one out value in fragment shader, so its name can be anything and opengl just assumes you mean this out vec4 is the pixel colour
static const char* fShader = " \n\
#version 330 \n\
out vec4 colour; \n\
in vec4 vCol; \n\
void main(){ \n\
    colour = vCol; \n\
}";

void CreateTriangle() {
    // define the indices of the points that compose each triangle
    unsigned int indices[] = {
        0,3,1,
        1,3,2,
        2,3,0,
        0,1,2
    };
    // define the points of the triangle/shape
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
         0.0f, -1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f
    };

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 12, 12); // just watch magic numbers for later. In the end the num will be provided in files so we don't need to calculate it
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 12, 12); // just watch magic numbers for later. In the end the num will be provided in files so we don't need to calculate it
    meshList.push_back(obj2);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
    GLuint theShader = glCreateShader(shaderType); // create empty shader of given type
    const GLchar* theCode[1]; 
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    GLint result = 0; // result of the programs
    GLchar eLog[1024] = { 0 }; // place to log error

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result); // check it linked correctly
    if (!result) {
        glGetProgramInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
        return;
    }

    glAttachShader(theProgram, theShader); 
}

void CompileShaders() {
    shader = glCreateProgram();
    if (!shader) {
        printf("Error creating shader program");
        return; // will crash the program if not handled properly (this is not proper handling)
    }
    AddShader(shader, vShader, GL_VERTEX_SHADER);
    AddShader(shader, fShader, GL_FRAGMENT_SHADER);

    GLint result = 0; // result of the programs
    GLchar eLog[1024] = { 0 }; // place to log error

    glLinkProgram(shader); // creates executable on gpu,
    glGetProgramiv(shader, GL_LINK_STATUS, &result); // check it linked correctly
    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error linking program: '%s'\n", eLog);
        return;
    }
    glValidateProgram(shader); // check if shader is valid in current context
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result); // check it linked correctly
    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error validating program: '%s'\n", eLog);
        return;
    }

    uniformModel = glGetUniformLocation(shader, "model"); // shader program, name of variable we're looking for
    uniformProjection = glGetUniformLocation(shader, "projection"); // shader program, name of variable we're looking for
    // remember that the returned value is the location, or address, of the variable, hence GLuint rather than GLfloat
}

int main() {
    //std::cout << "Hello World!\n";
    // initialise glfw and check for error
    if (!glfwInit()) {
        printf("GLFW initialisation failed");
        glfwTerminate();
        return 1;
    }

    // Setup GLFW window properties
    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Core profile = no backwards compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // allow forward compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);

    if (!mainWindow) {
        printf("GLFW window creation failed");
        glfwTerminate();
        return 1;
    }

    // Get buffer size info
    // the part that holds the opengl info being passed into window
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // set context for GLEW to use
    // you can switch this around at runtime to determine which window to draw on, eg if your game runs on separate windows
    glfwMakeContextCurrent(mainWindow); 

    // allow modern extension features
    // this course might not use it, but it's not a bad idea to use it, as it allows more features
    glewExperimental = GL_TRUE;
    
    // initialise GLEW
    if (glewInit() != GLEW_OK) { // basically just have to remember which ones are !function() or function != OK to check for error
        printf("GLEW initialisation failed");
        // therefore we need to destroy the window and then terminate the context
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST); // otherwise depth testing is disabled and triangles get drawn without depth (drawn over each other)

    // setup viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);
    
    CreateTriangle();
    CompileShaders();

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.01f, 10.0f);

    float r=0.0f, g=0.0f, b=0.0f;
    // loop until window closed (equiv to update loop?)
    while (!glfwWindowShouldClose(mainWindow)) {
        // Get & handle user input events
        glfwPollEvents();
        if (direction) {
            triOffset += triIncrement;
        }
        else {
            triOffset -= triIncrement;
        }
        if (abs(triOffset) >= triMaxOffset) {
            direction = !direction; 
        }
        if (sizeDirection) {
            curSize += 0.001f;
        }
        else {
            curSize -= 0.001f;
        }
        if (curSize >= maxSize || curSize <= minSize) {
            sizeDirection = !sizeDirection;
        }
        // clear window with a solid color (for starters this is all we'll do)
        // intent is to begin drawing on a clear window, so you don't draw over a frame (unless that's what you planned)
        //r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        //g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        //b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

        curAngle = fmod(curAngle + 0.01f, 360.0f);

        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear just color data of pixel (excluding things like depth, etc), other buffer bits exist

        glUseProgram(shader); // usually you would iterate through shaders 

        glm::mat4 model = glm::mat4(1.0f); // doesn't automattical init as identity, make sure to init it properly

        model = glm::translate(model, glm::vec3(0.0f, triOffset, -2.5f)); 
        //model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0)); // rotation axis
        // remember that the model has its own axes defined in the model matrix, which is why translate doesn't only move it left and right relative to the screen
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        // without a projection matrix, everything is calculated relative to the screen size

        //glUniform1f(uniformXMove, triOffset); // set variable at location obtained in uniformXMove to triOffset 
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

        meshList[0]->RenderMesh();
        
        model = glm::mat4(1.0f); // remember 1.0f is required to initialise to not junk values.
        model = glm::translate(model, glm::vec3(-triOffset, 0.0f, -2.5f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[1]->RenderMesh();

        glUseProgram(0);

        // there are two buffers, one being displayed and one being drawn, we just swap between the two when the drawing one is done
        glfwSwapBuffers(mainWindow); // swap between working buffer and displaying buffer
    }

    return 0;
}
