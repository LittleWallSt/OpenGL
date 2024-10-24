#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "OPENGLTEST", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //Initialize Glew, Library that gets functions from the GPU drivers
    if (glewInit() != GLEW_OK)
        std::cout << "error" << std::endl;

    //Check the version of OPENGL
    std::cout << glGetString(GL_VERSION) << std::endl;

    
    // >> DATA FOR TRIANGLE
    // vertex POSITIONS, Vertexes have a lot more data in them(Color, position, normals etc..)
    float positions[6] = {
        -0.5f, -0.5,
        0,  0.5f,
        0.5f, -0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), positions, GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    // First parameter, where we start to read
    //Second parameter, dimensions of vector ex 2 for 2, 3 for 3D, 4 for RGBA
    //Third parameter type of data to represent
    //Fourth parameter if it should be normalized
    //Fifth parameter is the size of all the information in 1 vertex, 
    // here it's 8 because we are only holding the information of the positon
    // Sixth parameter is offset of change of what information of the vertex is being accessed
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float),0);
    // << DATA FOR TRIANGLE
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Draw call, is blank because we don't have a shader implementation yet
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}