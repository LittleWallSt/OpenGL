#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

static unsigned int CompileShader( unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result; 
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); 
    if (result == GL_FALSE)
    {
        int length; 
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)malloc(length * sizeof(char)); 
        glGetShaderInfoLog(id, length, &length, message); 

        std::cout << "Failed to compile " <<
            (type == GL_FRAGMENT_SHADER ? "fragment " : "vertex ") << "shader" << std::endl; 
        std::cout << message << std::endl; 
        
        glDeleteShader(id); 
        return 0; 
    }


    //TODO: ERROR HANDLING 
    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

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

    // >> SHADER
    std::string vertexShader = 
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n";
    
    std::string fragmentShader = 
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;"
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

    unsigned int shader = CreateShader(vertexShader, fragmentShader); 
    // << SHADER

    
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

    glUseProgram(shader); 
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

    glDeleteProgram(shader); 

    glfwTerminate();
    return 0;
}