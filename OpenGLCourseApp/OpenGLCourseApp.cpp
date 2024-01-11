#include <iostream>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// window dimensions
const GLint WIDTH  = 800; // GLint is just an int type defined by opengl
const GLint HEIGHT = 600;

GLuint VAO, VBO, shader; // usually there are multiple vao, vbo for each object, so this is not good practice to global scope them

// Vertex shader; usually done in external files
// gl_Position is a built in variable
// defines the ultimate position of a vertex
// fragment shader is where visual effects are usually done, vertex shader is like preprocessing
// gl_Position added weights are to demonstrate that vertex can move vertices around 
static const char* vShader = " \n\
#version 330 \n\
layout (location = 0) in vec3 pos; \n\
void main(){ \n\
    gl_Position = vec4(0.4*pos.x, 0.4*pos.y, 0.4*pos.z, 1.0); \n\
}";

// Fragment shader
// there is only one out value in fragment shader, so its name can be anything and opengl just assumes you mean this out vec4 is the pixel colour
static const char* fShader = " \n\
#version 330 \n\
out vec4 colour; \n\
void main(){ \n\
    colour = vec4(1.0, 0, 1.0, 1.0); \n\
}";

void CreateTriangle() {
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f
    };
    glGenVertexArrays(1, &VAO); // create (potentially multiple) VAOs using given address 
    glBindVertexArray(VAO); // tell opengl we are now working on THIS VAO 

    // some people like to indent here to indicate that this context is using the VAO bound above 
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // insert the buffer into the bound VAO

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // sizeof vertices is ok in this instance, but it's not a great practice due to sizeof not being entirely comprehensive
    // can use GL_DYNAMIC_DRAW but it's more complicated to use, and in most programs we will just use GL_STATIC_DRAW

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // basically indicates the drawing loop parameters, data size, number of points etc. refer to variable names
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffers in reverse order of binding them by binding them to 0

    glBindVertexArray(0);

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

    // setup viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);
    
    CreateTriangle();
    CompileShaders();

    float r=0.0f, g=1.0f, b=0.0f;
    // loop until window closed (equiv to update loop?)
    while (!glfwWindowShouldClose(mainWindow)) {
        // Get & handle user input events
        glfwPollEvents();

        // clear window with a solid color (for starters this is all we'll do)
        // intent is to begin drawing on a clear window, so you don't draw over a frame (unless that's what you planned)
        //r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        //g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        //b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); // clear just color data of pixel (excluding things like depth, etc), other buffer bits exist

        glUseProgram(shader); // usually you would iterate through shaders 

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 3); // normally you would store the number of points in the object, or if you were to store multiple objects you would edit the offset etc.

        glBindVertexArray(0);

        glUseProgram(0);

        // there are two buffers, one being displayed and one being drawn, we just swap between the two when the drawing one is done
        glfwSwapBuffers(mainWindow); // swap between working buffer and displaying buffer
    }

    return 0;
}
