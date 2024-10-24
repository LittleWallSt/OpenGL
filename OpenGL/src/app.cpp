#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCALL(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError() {
    while (glGetError()!= GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL error] (" << error << ")" <<function 
           << " " << file << ": " << line << std::endl;
        return false; 
    }
    return true; 
}

struct ShaderProgramSource {
    std::string VertexSource; 
    std::string FragmentSource; 
};

/// <summary>
/// Reads from the shader file and extracts the VERTEX and FRAGMENT shaders
/// </summary>
/// <param name="filePath">filepath to the shader to extract</param>
/// <returns>VERTEX and FRAGMENT shader in string format</returns>
static ShaderProgramSource ParseShader(const std::string& filePath) {
    std::ifstream stream(filePath); 
    
    enum class ShaderType {
        NONE = -1, 
        VERTEX = 0, 
        FRAGMENT = 1
    };

    std::string line; 
    std::stringstream ss[2]; 
    ShaderType type = ShaderType::NONE; 
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            
            if (line.find("vertex") != std::string::npos) {
                //set mode to vertex
                type = ShaderType::VERTEX; 
            }
            else if (line.find("fragment") != std::string::npos) {
                //set mode to fragment
                type = ShaderType::FRAGMENT; 
            }
        }
        else {
            ss[(int)type] << line << '\n'; 
        }
    }

    return { ss[0].str(), ss[1].str() }; 
}

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
    ShaderProgramSource source = ParseShader("res/shaders/basic.shader"); 
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource); 
    // << SHADER

    
    // >> DATA FOR TRIANGLE
    // vertex POSITIONS, Vertexes have a lot more data in them(Color, position, normals etc..)
    float positions[] = {
        -0.5f, -0.5f, //0
         0.5f, -0.5f, //1
         0.5f,  0.5f, //2
        -0.5f,  0.5f, //3
    };

    unsigned int indices[] = {
        0, 1, 2, 
        2, 3, 0
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6*2*sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    // First parameter, where we start to read
    //Second parameter, dimensions of vector ex 2 for 2, 3 for 3D, 4 for RGBA
    //Third parameter type of data to represent
    //Fourth parameter if it should be normalized
    //Fifth parameter is the size of all the information in 1 vertex, 
    // here it's 8 because we are only holding the information of the positon
    // Sixth parameter is offset of change of what information of the vertex is being accessed
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float),0);

    // IBO >> INDEX BUFFER OBJECT
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    // << DATA FOR TRIANGLE

    glUseProgram(shader); 
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        GLClearError(); 
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));  

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader); 

    glfwTerminate();
    return 0;
}