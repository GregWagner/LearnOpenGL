#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <array>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

constexpr unsigned int SCR_WIDTH{800};
constexpr unsigned int SCR_HEIGHT{600};
constexpr int infoLogSize{512};

constexpr float clearColorRed{0.2F};
constexpr float clearColorGreen{0.3F};
constexpr float clearColorBlue{0.3F};
constexpr float clearColorAlpha{1.0F};


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

GLFWwindow *createWindow() {
    if (glfwInit() == GLFW_FALSE) {
        std::cerr << "Failed to initialize GLFW\n";
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    auto *window{glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr)};
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    return window;
}

bool initGlad() {
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }
    return true;
}

void checkCompileErrors(unsigned int object, const std::string &type) {
    int success{};
    std::array<char, infoLogSize> infoLog{};

    if (type != "PROGRAM") {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE) {
            glGetShaderInfoLog(object, infoLogSize, nullptr, infoLog.data());
            std::cerr << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog.data() << '\n';
        }
    } else {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (success == GL_FALSE) {
            glGetProgramInfoLog(object, infoLogSize, nullptr, infoLog.data());
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << '\n' << infoLog.data() << '\n';
        }
    }
}

std::string readShaderFile(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << path << '\n';
        return {};
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int buildVertexShader() {
    auto vertexShader{glCreateShader(GL_VERTEX_SHADER)};
    const std::string vertexShaderSource{readShaderFile("vertShader.glsl")};
    const char *vertexShaderSourcePtr{vertexShaderSource.c_str()};
    glShaderSource(vertexShader, 1, &vertexShaderSourcePtr, nullptr);
    glCompileShader(vertexShader);

    // check for shader compile errors
    checkCompileErrors(vertexShader, "VERTEX");
    return vertexShader;
}

unsigned int buildFragmentShader() {
    auto fragmentShader{glCreateShader(GL_FRAGMENT_SHADER)};
    const std::string fragmentShaderSource{readShaderFile("fragShader.glsl")};
    const char *fragmentShaderSourcePtr{fragmentShaderSource.c_str()};
    glShaderSource(fragmentShader, 1, &fragmentShaderSourcePtr, nullptr);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    checkCompileErrors(fragmentShader, "FRAGMENT");
    return fragmentShader;
}

unsigned int linkShaders(unsigned int vertexShader, unsigned int fragmentShader) {
    auto shaderProgram{glCreateProgram()};
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    checkCompileErrors(shaderProgram, "PROGRAM");
    return shaderProgram;
}

void cleanUpShaders(unsigned int vertexShader, unsigned int fragmentShader) {
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

unsigned int buildShaderProgram() {
    auto vertexShader{buildVertexShader()};
    auto fragmentShader{buildFragmentShader()};
    auto shaderProgram{linkShaders(vertexShader, fragmentShader)};
    cleanUpShaders(vertexShader, fragmentShader);
    return shaderProgram;
}

// setup the triangle geometry on the GPU
unsigned int createVertexObjects() {
    std::array<float, 9> triangleVertices = {
        -0.5F,
        -1.0F / 3.0F,
        0.0F, // left
        0.5F,
        -1.0F / 3.0F,
        0.0F, // right
        0.0F,
        2.0F / 3.0F,
        0.0F // top
    };

    unsigned int VBO{};
    glGenBuffers(1, &VBO);

    unsigned int VAO{};
    glGenVertexArrays(1, &VAO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    return VAO;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void initializeViewport(GLFWwindow *window) {
    // Query the actual framebuffer size instead of assuming SCR_WIDTH/SCR_HEIGHT:
    // on HiDPI displays the framebuffer can be larger than the window, and
    // using the wrong size would leave the viewport/geometry off-center.
    int framebufferWidth{};
    int framebufferHeight{};
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void renderLoop(GLFWwindow *window, unsigned int shaderProgram, unsigned int VAO) {
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        processInput(window);

        glClearColor(clearColorRed, clearColorGreen, clearColorBlue, clearColorAlpha);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
} // namespace

int main() {
    GLFWwindow *window{createWindow()};
    if (window == nullptr) {
        return -1;
    }

    if (!initGlad()) {
        glfwTerminate();
        return -1;
    }
    initializeViewport(window);

    auto shaderProgram{buildShaderProgram()};
    auto VAO{createVertexObjects()};

    renderLoop(window, shaderProgram, VAO);

    glfwTerminate();
}
