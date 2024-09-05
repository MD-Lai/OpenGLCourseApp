#include "Mesh.h"

Mesh::Mesh() {
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;
}

void Mesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices) {
	indexCount = numOfIndices;

    glGenVertexArrays(1, &VAO); // create (potentially multiple) VAOs using given address 
    glBindVertexArray(VAO); // tell opengl we are now working on THIS VAO 

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // element buffer is a buffer of indices aka elements (sometimes IBO = EBO) 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW); // this is just boilerplate stuff

    // some people like to indent here to indicate that this context is using the VAO bound above 
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // insert the buffer into the bound VAO

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW); // sizeof vertices is ok in this instance, but it's not a great practice due to sizeof not being entirely comprehensive
    // can use GL_DYNAMIC_DRAW but it's more complicated to use, and in most programs we will just use GL_STATIC_DRAW

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // basically indicates the drawing loop parameters, data size, number of points etc. refer to variable names
    glEnableVertexAttribArray(0);

    // now that it's been modularised, we definitely need to unbind the buffers otherwise you'd get write-over issues
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffers in reverse order of binding them by binding them to 0
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    

    glBindVertexArray(0);
}

void Mesh::RenderMesh() {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0); // for indexed draws, this is what you call to draw it. 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// this just clears the mesh, like emptying the contentx
void Mesh::ClearMesh() {
    // not deleting buffers causes it to stack up and cause memory leaks
    if (IBO != 0) {
        glDeleteBuffers(1, &IBO); 
        IBO = 0;
    }

    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO != 0) {
        glDeleteBuffers(1, &VAO);
        VAO = 0;
    }
    indexCount = 0;
}

// this actually deletes the object, but there's some risk with accidentally not creating a variable on the heap, 
// and deleting at the end of a function scope rather than passing a pointer or whatnot. Just be careful with pointers
Mesh::~Mesh() {
    ClearMesh();
}