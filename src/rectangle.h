#ifndef _RECTANGLE_H_
#define _RECTANGLE_H_
#include <string.h>
#include "cglm/cglm.h"
#include "glad/glad.h"

struct Rectangle {
    float verts[20];
    float transform[2];
    unsigned int VAO, VBO, EBO;
};

void makeRectangle(const float width, const float height, struct Rectangle *rectangle);
int inRect(struct Rectangle rect, const int x, const int y);
void drawRectangle(struct Rectangle *rect, const unsigned int shader, const float x, const float y, const vec3 color, const float a);
void updateVerticies(struct Rectangle *rect, const float *values);

#endif