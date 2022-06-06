#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <malloc.h>
#include <math.h>
unsigned short WINDOWWIDTH  = 300;
unsigned short WINDOWHEIGHT = 200;
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
    WINDOWWIDTH = width;
    WINDOWHEIGHT = height;
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


unsigned int textVAO, textVBO;

struct Character {
    unsigned int TextureID;
    ivec2 Size;
    ivec2 Bearing;
    unsigned int Advance;
} charset[CHARSETSIZE];

void RenderText(unsigned int shader, char *text, float x, float y, float scale, vec3 color) {
	glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader, "textColor"), color[0], color[1], color[2]);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, textVBO);

    for (int i = 0; text[i] != '\0'; i++) {
        struct Character ch = charset[text[i]];
        float xpos = x + ch.Bearing[0] * scale;
        float ypos = y - (ch.Size[1] - ch.Bearing[1]) * scale;

        float w = ch.Size[0] * scale;
        float h = ch.Size[1] * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    return;
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
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    float points1[] = {
        -0.8f, -0.8f, // point a x,y
        0.8f, 0.8f, // point b x,y
    };

    float rect1[] = {
        points1[0], points1[1], 0.0f, // left bottom corner
        points1[2], points1[1], 0.0f, // right bottom corner
        points1[0], points1[3], 0.0f, // left top corner
        points1[2], points1[3], 0.0f, // right top corner
    };

    float points2[] = {
        25.0f, 25.0f,
        100.0f, 100.0f
    };
    float rect2[] = {
        points2[0], points2[1], 0.0f, // left bottom corner
        points2[2], points2[1], 0.0f, // right bottom corner
        points2[0], points2[3], 0.0f, // left top corner
        points2[2], points2[3], 0.0f, // right top corner
    };

    unsigned int indices[] = {
        0,1,2,
        2,1,3
    };


    
    /**
     *      NORMAL
     */


    /**
     *  rect1
     */
    unsigned int normalVAO, normalVBO, normalEBO;
    glGenVertexArrays(1, &normalVAO);
    glGenBuffers(1, &normalVBO);
    glGenBuffers(1, &normalEBO);

    glBindVertexArray(normalVAO);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect1), rect1, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normalEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);


    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    /**
     *      FOR TEXT
     */

    // GENERATE VAO
    glGenVertexArrays(1, &textVAO);
    // GENERATE VBO
    glGenBuffers(1, &textVBO);
    // BIND VAO
    glBindVertexArray(textVAO);
    // BIND VBO
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    // ALLOCATE MEMORY FOR TEXT TRIANGLES
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    // ???
    glEnableVertexAttribArray(0);
    // SPECIFY HOW TO INTERPRET THE VERTEX BUFFER DATA. THIS IS STORED IN CURRENTLY BOUND VAO!
    // index, size, type, normalized, stride, offset
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // UNBIND VAO
    glBindVertexArray(0);


    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    // LOAD TEXTSHADERS
    unsigned int textShader = Shader("shaders/glyphs.vs", "shaders/glyphs.fs");
    mat4 projection;
    glm_ortho(0.0f, (float)WINDOWWIDTH, 0.0f, (float)WINDOWHEIGHT, 0.001f, -1000.0f, projection);
    // FEED SHADER THIS PROJECTION MATRIX
    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, &projection[0][0]);

    unsigned int relativeShader = Shader("shaders/orange.vs", "shaders/orange.fs");
    unsigned int absoluteShader = Shader("shaders/absolute.vs", "shaders/absolute.fs");

	// wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    char width[32] = "Window width: ";
    char height[32] = "Window height: ";
    sprintf(width, "Window width: %d", WINDOWWIDTH);
    while(!glfwWindowShouldClose(window)) {
        static float time;
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // USE RELATIVE SHADER AND SET ITS VALUES
        glUseProgram(relativeShader);
        glUniform3f(glGetUniformLocation(relativeShader, "col"), fabs(sin(time * 0.3f)), fabs(sin(time * 0.5f)), fabs(sin(time)));
        glBindVertexArray(normalVAO);

        glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rect1), rect1, GL_STATIC_DRAW);
        // DRAW BIG RECTANGLE
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // LOAD RECT2
        glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rect2), rect2, GL_STATIC_DRAW);

        // USE ABSOLUTE SHADER AND SET ITS VALUES
        glUseProgram(absoluteShader);
        glUniform4f(glGetUniformLocation(absoluteShader, "col"), 1.0f, 1.0f, 1.0f, 8.0f);
        glUniformMatrix4fv(glGetUniformLocation(absoluteShader, "projection"), 1, GL_FALSE, &projection[0][0]);
        //glUniformMatrix4fv(glGetUniformLocation(absoluteShader, "transform"), 1, GL_FALSE, &transform[0][0]);
        // DRAW RECT2
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glUseProgram(textShader);
        glm_ortho(0.0f, (float)WINDOWWIDTH, 0.0f, (float)WINDOWHEIGHT, 0.001f, -1000.0f, projection);
        // FEED SHADER THIS PROJECTION MATRIX
        glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, &projection[0][0]);

        vec3 color = {1.0f, 1.0f, 1.0f};
        sprintf(width, "Window x: %d", WINDOWWIDTH);
        sprintf(height, "Window y: %d", WINDOWHEIGHT);
        RenderText(textShader, width, 20.0f, 25.0f, 0.5f, color);
        RenderText(textShader, height, 20.0f, 50.0f, 0.5f, color);

        glfwSwapBuffers(window);
        glfwPollEvents();
        time += 0.001f;
    }
    glfwTerminate();
    return 0;
}

