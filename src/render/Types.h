#pragma once

#define LAYOUT {3, 4, 2, 1}

struct Vertex2D {
	float pos[3];
	float color[4];
	float tex[2];
	float tex_index;

	Vertex2D(float x, float y, float *color, float tx, float ty, float tex_index) {
		setPos(x, y, 0);
		setTex(tx, ty);
		setColor(color);
		this->tex_index = tex_index;
	}

	Vertex2D(float x, float y, float z, float *color, float tx, float ty, float tex_index) {
		setPos(x, y, z);
		setTex(tx, ty);
		setColor(color);
		this->tex_index = tex_index;
	}

	Vertex2D(float x, float y, float *color) {
		setPos(x, y, 0);
		setColor(color);
	}

	Vertex2D(float x, float y, float z, float *color) {
		setPos(x, y, z);
		setColor(color);
	}

private:
	void setPos(float x, float y, float z) {
		pos[0] = x;
		pos[1] = y;
		pos[2] = z;
	}

	void setTex(float x, float y) {
		tex[0] = x;
		tex[1] = y;
	}

	void setColor(float r, float g, float b, float a) {
		this->color[0] = r;
		this->color[1] = g;
		this->color[2] = b;
		this->color[3] = a;
	}

	void setColor(float *color) {
		this->color[0] = color[0];
		this->color[1] = color[1];
		this->color[2] = color[2];
		this->color[3] = color[3];
	}
};


struct Rect {
	int x, y, w, h;
};