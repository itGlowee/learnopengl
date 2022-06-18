#include "rectangle.h"

static unsigned int rectIndices[] = {
    0,1,2,
    2,1,3
};
 
int inRect(struct Rectangle rect, int x, int y) {
    return rect.verts[0] + rect.transform[0] <= x && rect.verts[5] + rect.transform[0]>= x && rect.verts[1] + rect.transform[1] <= y && rect.verts[11] + rect.transform[1] >= y;
}

void makeRectangle(float width, float height, struct Rectangle *rectangle) {
    width *= 0.5;
    height *= 0.5;
    float rect[] = {
        -width, -height, 0.0f, 0.0f, 0.0f,    // left bottom corner
        width, -height, 0.0f, 1.0f, 0.0f,    // right bottom corner
        -width, height, 0.0f, 0.0f, 1.0f,    // left top corner
        width, height, 0.0f, 1.0f, 1.0f     // right top corner
    };
    memcpy(&rectangle->verts, &rect, sizeof(rect));
    glGenVertexArrays(1, &rectangle->VAO);
    glBindVertexArray(rectangle->VAO);

    glGenBuffers(1, &rectangle->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, rectangle->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rectangle->verts, GL_STATIC_DRAW);

    glGenBuffers(1, &rectangle->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectangle->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectIndices), rectIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_VERTEX_ARRAY, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    return;
}

static void bindRectBuffers(struct Rectangle *rect) {
    glBindVertexArray(rect->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, rect->VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect->EBO);
    return;
}

void drawRectangle(struct Rectangle *rect, unsigned int shader, float x, float y, float r, float g, float b, float a) {
    bindRectBuffers(rect);
    glUseProgram(shader);
    glUniform4f(glGetUniformLocation(shader, "col"), r, g, b, a);
    glUniform2f(glGetUniformLocation(shader, "transform"), x, y);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    rect->transform[0] = x;
    rect->transform[1] = y;
    return;
}