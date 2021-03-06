
/**
 * TODO: Make a texture shader
 * TODO: Render a line
 * TODO: Figure out what's the best way to encapsulate shape rendering
 * TODO: Encapsulate Text stuff
 * TODO: Encapsulate shader stuff
 * TODO: Make more interactive stuff
 * TODO: Make cool shaders.
 * TODO: Maybe try 2.5d stuff
 * TODO: Actually compile the libraries so I don't look dumb
 * DONE: FPS counter
 * DONE: Render a circle
 */


#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <malloc.h>
#include <math.h>

#include "rectangle.h"
#include "colors.h"

unsigned short WINDOWWIDTH  = 400;
unsigned short WINDOWHEIGHT = 600;
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
struct Keys {
    unsigned int left_arrow  :   1;
    unsigned int up_arrow    :   1;
    unsigned int right_arrow :   1;
    unsigned int down_arrow  :   1;
    unsigned int escape      :   1;
} keyboard;
struct Mouse {
    double x, y;
    unsigned int left       : 1;
    unsigned int right      : 1;
} mouse;

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    mouse.x = xpos;
    mouse.y = ypos;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
        keyboard.escape = 1;
    }
    else if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
        keyboard.escape = 0;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        keyboard.left_arrow = 1;
    }
    else if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) {
        keyboard.left_arrow = 0;
    }
    if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        keyboard.right_arrow = 1;
    }
    else if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) {
        keyboard.right_arrow = 0;
    }
    if(key == GLFW_KEY_UP && action == GLFW_PRESS) {
        keyboard.up_arrow = 1;
    }
    else if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
        keyboard.up_arrow = 0;
    }
    if(key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        keyboard.down_arrow = 1;
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
        keyboard.down_arrow = 0;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouse.left = 1;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mouse.left = 0;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        mouse.right = 1;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        mouse.right = 0;
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

void RenderText(unsigned int shader, char *text, float x, float y, float scale, const vec3 color) {
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

void unbindBuffers() {
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    WINDOWWIDTH = mode->width;
    WINDOWHEIGHT = mode->height;
    GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "LearnOpenGL", glfwGetPrimaryMonitor(), NULL);
    //GLFWwindow *window = glfwCreateWindow(WINDOWWIDTH, WINDOWHEIGHT, "LearnOpenGL", NULL, NULL);
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
    //glViewport(0, 0, WINDOWWIDTH, WINDOWHEIGHT);
    glViewport(0, 0, mode->width, mode->height);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // load font
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        printf("ERROR::FREETYPE: Could not init FreeType Library\n");
        return -1;
    }
    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
    {
        printf("ERROR::FREETYPE: Failed to load font\n");
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load font letters into "charset"
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
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);  
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    mat4 projection;
    unsigned int textShader = Shader("shaders/glyphs.vs", "shaders/glyphs.fs");
    unsigned int relativeShader = Shader("shaders/orange.vs", "shaders/orange.fs");
    unsigned int absoluteShader = Shader("shaders/absolute.vs", "shaders/absolute.fs");
    unsigned int circleShader = Shader("shaders/circle.vs", "shaders/circle.fs");

    // wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    char width[32];
    char height[32];
    char mousex[32];
    char mousey[32];
    char FPS[32];
    const unsigned int FPSsamplesize = 1000;
    double previous[FPSsamplesize];

    struct Rectangle frameRect;
    makeRectangle(2.0f, 2.0f, &frameRect);

    struct Rectangle backgroundRect;
    makeRectangle(1.9f, 1.9f, &backgroundRect);

    struct Rectangle rect2;
    makeRectangle(75.0f, 60.0f, &rect2);

    struct Rectangle mouseRect;
    makeRectangle(10.0f, 10.0f, &mouseRect);

    struct Rectangle button1;
    makeRectangle(40.0f, 20.0f, &button1);

    struct Rectangle circle1;
    makeRectangle(100.0f, 100.0f, &circle1);

    float line[] = {
        20.0f,                  300.0f,                 0.0f,
        WINDOWWIDTH - 40.0f,    300.0f,                 0.0f,
        WINDOWWIDTH - 40.0f,    WINDOWHEIGHT - 100.0f,  0.0f,
        20.0f,                  WINDOWHEIGHT - 100.0f,  0.0f
    };
    glLineWidth(10.0f);
    glEnable(GL_LINE_SMOOTH);
    

    unsigned int lineVAO, lineVBO;
    glGenVertexArrays(1, &lineVAO);
    glBindVertexArray(lineVAO);

    glGenBuffers(1, &lineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    while(!glfwWindowShouldClose(window)) {
        static double deltaTime;
        static double average;
        static unsigned short int c;
        static vec2 movement = { -50.0f, -50.0f };
        double time = glfwGetTime();
        previous[c % FPSsamplesize] = deltaTime;
        average = 0;
        for (unsigned int i = 0; i < FPSsamplesize; i++) {
            average += previous[i];
        }
        average /= FPSsamplesize;
        if ((c++ % 128) == 0){
            printf("Time: %4.1lf\n", time);
        }
        if (keyboard.left_arrow) {
            movement[0] -= 500.0f * deltaTime;
        }
        if (keyboard.right_arrow) {
            movement[0] += 500.0f * deltaTime;
        }
        if (keyboard.up_arrow) {
            movement[1] += 500.0f * deltaTime;
        }
        if (keyboard.down_arrow) {
            movement[1] -= 500.0f * deltaTime;
        }
        // calculate projection
        glm_ortho(0.0f, (float)WINDOWWIDTH, 0.0f, (float)WINDOWHEIGHT, 0.001f, -1000.0f, projection);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(relativeShader);
        vec3 color = { fabs(sin(time * 0.3f)), fabs(sin(time * 0.5f)), fabs(sin(time)) };
        drawRectangle(&frameRect, relativeShader, 0, 0, color, 1.0f);
       
        glUseProgram(absoluteShader);
        glUniformMatrix4fv(glGetUniformLocation(absoluteShader, "projection"), 1, GL_FALSE, &projection[0][0]);

        float border = 2.0f;
        float rect[] = {
        //  X                       Y                       Z       U       V
            border,                 border,                 0.0f,   0.0f,   0.0f,    // left bottom corner
            WINDOWWIDTH - border,   border,                 0.0f,   1.0f,   0.0f,    // right bottom corner
            border,                 WINDOWHEIGHT - border,  0.0f,   0.0f,   1.0f,    // left top corner
            WINDOWWIDTH - border,   WINDOWHEIGHT - border,  0.0f,   1.0f,   1.0f     // right top corner
        };
        updateVerticies(&backgroundRect, (const float*)&rect);

        drawRectangle(&backgroundRect, absoluteShader, 0, 0, BACKGROUND, 1.0f);
        drawRectangle(&rect2, absoluteShader, movement[0], movement[1], WHITE, 1.0f);

        if (inRect(button1, (int)mouse.x, (int)(WINDOWHEIGHT - mouse.y)) && mouse.left) {
            drawRectangle(&button1, absoluteShader, WINDOWWIDTH - 50.0f, WINDOWHEIGHT - 30.0f, RED, 1.0f);
        }
        else {
            drawRectangle(&button1, absoluteShader, WINDOWWIDTH - 50.0f, WINDOWHEIGHT - 30.0f, BLUE, 1.0f);
        }

        glUniform4f(glGetUniformLocation(absoluteShader, "col"), 1.0f, 1.0f, 1.0f, 1.0f);
        glUniform2f(glGetUniformLocation(absoluteShader, "transform"), 0, 0);
        glBindVertexArray(lineVAO);
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        glUseProgram(circleShader);
        glUniformMatrix4fv(glGetUniformLocation(circleShader, "projection"), 1, GL_FALSE, &projection[0][0]);
        drawRectangle(&circle1, circleShader, 300.0f, fabs(sin(time * 2)) * 300.0f + 45.0f, GREEN, 1.0f);
        drawRectangle(&mouseRect, circleShader, mouse.x, WINDOWHEIGHT - mouse.y, RED, 1.0f);


        glUseProgram(textShader);
        glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, &projection[0][0]);
        sprintf(width, "Window x: %d", WINDOWWIDTH);
        sprintf(height, "Window y: %d", WINDOWHEIGHT);
        RenderText(textShader, width, 20.0f, 25.0f, 0.5f, WHITE);
        RenderText(textShader, height, 20.0f, 50.0f, 0.5f, WHITE);

        sprintf(mousex, "mousex: %d", (int)mouse.x);
        sprintf(mousey, "mousey: %d", (int)(WINDOWHEIGHT - mouse.y));
        RenderText(textShader, mousex, 20.0f, 75.0f, 0.5f, WHITE);
        RenderText(textShader, mousey, 20.0f, 100.0f, 0.5f, WHITE);
        
        sprintf(FPS, "FPS: %d", (int) (1.0f / average));
        RenderText(textShader, FPS, 20.0f, 125.0f, 0.5f, WHITE);
        glFlush();
        //glfwSwapBuffers(window);
        glfwPollEvents();
        deltaTime = glfwGetTime() - time;
    }
    glfwTerminate();
    return 0;
}
