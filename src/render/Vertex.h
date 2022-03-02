#pragma once

#include "../define.h"

struct VertexChunk {
	uint8_t pos[3];
	float color[3];

	VertexChunk(uint8_t x, uint8_t y, uint8_t z, float tx, float ty, float tex_index) {
		setPos(x, y, z);
		setColor(tx, ty, tex_index, 0);
	}

private:
	void setPos(uint8_t x, uint8_t y, uint8_t z) {
		pos[0] = x;
		pos[1] = y;
		pos[2] = z;
	}

	void setColor(float r, float g, float b, float a) {
		this->color[0] = r;
		this->color[1] = g;
		this->color[2] = b;
		// this->color[3] = a;
	}

	void setColor(float *color) {
		this->color[0] = color[0];
		this->color[1] = color[1];
		this->color[2] = color[2];
		// this->color[3] = color[3];
	}
};


struct Vertex3D {
	float pos[3];
	float color[3];

	Vertex3D(float x, float y, float z, float tx, float ty, float tex_index) {
		setPos(x, y, z);
		setColor(tx, ty, tex_index, 0);
	}

	Vertex3D(float x, float y, float z, float *color) {
		setPos(x, y, z);
		setColor(color);
	}

private:
	void setPos(float x, float y, float z) {
		pos[0] = x;
		pos[1] = y;
		pos[2] = z;
	}

	void setColor(float r, float g, float b, float a) {
		this->color[0] = r;
		this->color[1] = g;
		this->color[2] = b;
		// this->color[3] = a;
	}

	void setColor(float *color) {
		this->color[0] = color[0];
		this->color[1] = color[1];
		this->color[2] = color[2];
		// this->color[3] = color[3];
	}
};


struct Vertex2D {
	float pos[2];
	float color[4];

	Vertex2D(float x, float y, float tx, float ty, float tex_index) {
		setPos(x, y);
		setColor(tx, ty, tex_index, 0);
	}

	Vertex2D(float x, float y, float *color) {
		setPos(x, y);
		setColor(color);
	}

private:
	void setPos(float x, float y) {
		pos[0] = x;
		pos[1] = y;
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
