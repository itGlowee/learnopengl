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

void makeRectangle(float width, float height, struct Rectangle *rectangle);
int inRect(struct Rectangle rect, int x, int y);
void drawRectangle(struct Rectangle *rect, unsigned int shader, float x, float y, vec3 color, float a);
void updateVerticies(struct Rectangle *rect, float *values);

#endif