#include "rectangle.h"

static unsigned int rectIndices[] = {
    0,1,2,
    2,1,3
};
 
int inRect(struct Rectangle rect, int x, int y) {
    return rect.verts[0] <= x && rect.verts[5] >= x && rect.verts[1] <= y && rect.verts[11] >= y;
}

void makeRectangle(vec2 p1, vec2 p2, struct Rectangle *rectangle) {
    float rect[] = {
        p1[0], p1[1], 0.0f, 0.0f, 0.0f,    // left bottom corner
        p2[0], p1[1], 0.0f, 1.0f, 0.0f,    // right bottom corner
        p1[0], p2[1], 0.0f, 0.0f, 1.0f,    // left top corner
        p2[0], p2[1], 0.0f, 1.0f, 1.0f     // right top corner
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

void drawRectangle(struct Rectangle *rect) {
    bindRectBuffers(rect);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    return;
}