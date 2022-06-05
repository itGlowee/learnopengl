#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <malloc.h>
#define WINDOWWIDTH 800
#define WINDOWHEIGHT 600
#define CHARSETSIZE 128


#if WINDOWS
#include <windows.h>
#define FONTS "C:/Windows/Fonts/"
#define DEFAULTFONT "arial.ttf"
#elif LINUX
#define FONTS "/usr/share/fonts/truetype/"
#define DEFAULTFONT "ubuntu/Ubuntu-Th.ttf"
#else
#define FONTS "/usr/share/fonts/truetype/"
#define DEFAULTFONT "ubuntu/Ubuntu-Th.ttf"
#endif

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}
unsigned int Shader (const char *vertexPath, const char *fragmentPath) {

	FILE *vertexFile = fopen(vertexPath, "r");
	FILE *fragmentFile = fopen(fragmentPath, "r");

    char vertexCode[4096];
    char fragmentCode[4096];

    if (!vertexFile || !fragmentFile) {
        printf("Loading shaderfiles failed :(\n");
        return 0;
    }
    int ch = getc(vertexFile);
    int i;
    for (i = 0; ch != EOF && i < 4096 -1; i++) {
        vertexCode[i] = ch;
        ch = getc(vertexFile);
    }
    fclose(vertexFile);
    vertexCode[i] = '\0';

    ch = getc(fragmentFile);
    for (i = 0; ch != EOF && i < 4096 -1; i++) {
        fragmentCode[i] = ch;
        ch = getc(fragmentFile);
    }
    fclose(fragmentFile);
    fragmentCode[i] = '\0';

    const char *vertexCodeConst = vertexCode;
    const char *fragmentCodeConst = fragmentCode;

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCodeConst, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("ERROR::SHADER:VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentCodeConst, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
	}        
	unsigned int ID; // Program id
    ID = glCreateProgram();

    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);

	return ID;
}


unsigned int VAO, VBO, EBO;

struct Character {
    unsigned int TextureID;
    ivec2 Size;
    ivec2 Bearing;
    unsigned int Advance;
} charset[CHARSETSIZE];

// TODO: Finish this when shader system is complete.
// https://learnopengl.com/In-Practice/Text-Rendering
/*
void RenderText(Shader &s, char *text, float x, float y, float scale, vec3 color) {
    s.Use();
    glUniform3f(glGetUniformLocation(s.Program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (int i = 0; text[i] != '\0'; i++) {
        Character ch = charset[text[i]];

    }


}
*/

float vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
};
unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};  

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

    /*
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        printf("ERROR::FREETYPE: Could not init FreeType Library\n");
        return -1;
    }
    FT_Face face;
    if (FT_New_Face(ft, FONTS DEFAULTFONT, 0, &face))
    {
        printf("ERROR::FREETYPE: Failed to load font\n");
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mat4 projection;
    glm_ortho(0.0f, WINDOWWIDTH, 0.0f, WINDOWHEIGHT, 0.1f, 1000.0f, projection);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    for (unsigned char c = 0; c < CHARSETSIZE; c++) {

        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            printf("ERROR::FREETYPE: Failed to load Glyph");
            continue;
        }

        //generate texture for glyph
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        struct Character character = {
            texture,
            { face->glyph->bitmap.width, face->glyph->bitmap.rows },
            { face->glyph->bitmap_left, face->glyph->bitmap_top },
            face->glyph->advance.x
        };

        charset[c] = character;
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  */

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);

    unsigned int myShader = Shader("shaders/orange.vs", "shaders/orange.fs");
	glUseProgram(myShader);

	// wireframe
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while(!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

