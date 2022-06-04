#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#define WINDOWWIDTH 800
#define WINDOWHEIGHT 600

#define WINDOWS 1
#define LINUX 0

#if WINDOWS
#define FONTS "C:/Windows/Fonts/"
#elif LINUX
#define FONTS "/usr/share/fonts/truetype/"
#endif

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

    GLFWwindow *window = glfwCreateWindow(WINDOWWIDTH, WINDOWHEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window :(\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialze GLAD\n");
        return -1;
    }
    glViewport(0, 0, WINDOWWIDTH, WINDOWHEIGHT);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    FT_Library ft;
    FT_Error error;
    if (FT_Init_FreeType(&ft))
    {
        printf("ERROR::FREETYPE: Could not init FreeType Library\n");
        return -1;
    }

    FT_Face face;
    if (error = FT_New_Face(ft, FONTS "arial.ttf", 0, &face))
    {
        printf("ERROR::FREETYPE: Failed to load font\n");
        printf("%d\n", error);
        return -1;
    }



    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}