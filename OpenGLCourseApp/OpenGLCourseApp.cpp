#include <iostream>
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// window dimensions
const GLint WIDTH  = 800; // GLint is just an int type defined by opengl
const GLint HEIGHT = 600;

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
    float r=0.0f, g=0.0f, b=0.0f;
    // loop until window closed (equiv to update loop?)
    while (!glfwWindowShouldClose(mainWindow)) {
        // Get & handle user input events
        glfwPollEvents();

        // clear window with a solid color (for starters this is all we'll do)
        // intent is to begin drawing on a clear window, so you don't draw over a frame (unless that's what you planned)
        r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); // clear just color data of pixel (excluding things like depth, etc), other buffer bits exist

        // there are two buffers, one being displayed and one being drawn, we just swap between the two when the drawing one is done
        glfwSwapBuffers(mainWindow); // swap between working buffer and displaying buffer

    }

    return 0;
}
