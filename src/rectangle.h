#ifndef _RECTANGLE_H_
#define _RECTANGLE_H_
#include <cglm/cglm.h>
#include <string.h>
#include "glad/glad.h"

struct Rectangle {
    unsigned int VAO, VBO, EBO;
    float verts[12];
};

void makeRectangle(vec2 p1, vec2 p2, struct Rectangle *rectangle);
int inRect(struct Rectangle rect, int x, int y);
void drawRectangle(struct Rectangle *rect);
static void bindRectBuffers(struct Rectangle *rect);

#endif