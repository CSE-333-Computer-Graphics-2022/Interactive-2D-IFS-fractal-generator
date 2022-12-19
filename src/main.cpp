// Saumik Shashwat | 2020404 | Computer Graphics Project | Interactive 2D Fractal Generator

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Rectangle {
    glm::vec2 center;
    glm::vec2 size;
    float angle;
};

class FractalGenerator {
public:
    FractalGenerator() {
        rectangles.resize(5);
    }

    void addRectangle(const Rectangle& rect) {
        rectangles.push_back(rect);
    }

    void removeRectangle(int index) {
        rectangles.erase(rectangles.begin() + index);
    }

    void setRectangle(int index, const Rectangle& rect) {
        rectangles[index] = rect;
    }

    std::vector<glm::vec2> generateFractal(int numIterations) const {
        std::vector<glm::vec2> points;
        for (int i = 0; i < numIterations; i++) {
            for (const auto& rect : rectangles) {
                glm::mat2x2 transform;
                transform = glm::rotate(transform, rect.angle);
                transform = glm::scale(transform, glm::vec2(rect.size));
                transform = glm::translate(transform, rect.center);
                for (auto& point : points) {
                    point = transform * point;
                }
            }
        }
        return points;
    }

private:
    std::vector<Rectangle> rectangles;
};

int main() {
    // Initialize GLFW and create a window
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "IFS Fractal Generator", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set up the viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Set up the projection matrix
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    // Set up the model-view matrix
    glm::mat4 modelView = glm::mat4(1.0f);

    // Set up the vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Set up the vertex buffer object
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Set up the vertex attribute
    GLint posAttrib = 0;
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    // Set up the shaders
    const char* vertexShaderSource = R"(
    #version 330 core
    in vec2 pos;
    uniform mat4 modelView;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * modelView * vec4(pos, 0.0, 1.0);
    }
    )";

    const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 color;
    void main() {
        color = vec4(1.0, 1.0, 1.0, 1.0);
    }
    )";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Set up the model-view uniform
    GLint modelViewUniform = glGetUniformLocation(shaderProgram, "modelView");
    glUniformMatrix4fv(modelViewUniform, 1, GL_FALSE, glm::value_ptr(modelView));

    // Set up the projection uniform
    GLint projectionUniform = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));

    // Set up the fractal generator
    FractalGenerator fractalGenerator;

    // Set up the main loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Generate the fractal points
        std::vector<glm::vec2> points = fractalGenerator.generateFractal(10000);

        // Update the vertex buffer object
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec2), points.data(), GL_STATIC_DRAW);

        // Draw the fractal
        glDrawArrays(GL_POINTS, 0, points.size());

        // Swap the buffers
        glfwSwapBuffers(window);

        // Poll for events
        glfwPollEvents();
    }

    // Clean up
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glfwTerminate();

    return 0;
}