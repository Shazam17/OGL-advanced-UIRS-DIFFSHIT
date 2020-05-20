#pragma once
#include <glad/glad.h>
#include <vector>
#include "VBO.h"
#include "IBO.h"

class VAO {

	unsigned int id;
	unsigned int layoutCount;
public:
	inline struct tupple {
		unsigned int vecS;
		unsigned int type;
		unsigned int glBool;
	};
	inline class Layout {
		unsigned int iCount;
		std::vector <tupple> toLoad;
		unsigned int stride;

	public:
		Layout() {
			iCount = 0;
			stride = 0;
		}
		tupple getT(int c) { return toLoad[c]; }
		template<typename T>
		void add(unsigned int count) {
			iCount++;
		}
		template<>
		void add<float>(unsigned int count) {
			iCount++;
			toLoad.push_back({ count , GL_FLOAT , GL_FALSE });
			stride += count * sizeof(float);
		}
		unsigned int getCount() {
			return iCount;
		}unsigned int getStride() {
			return stride;
		}
	};
	VAO() {
		layoutCount = 0;
		glGenVertexArrays(1, &id);
	}
	void addLayout(Layout layout, VBO vb, IBO ib) {
		bind();
		vb.bind();
		ib.bind();
		unsigned int offset = 0;
		for (int i = 0; i < layout.getCount(); i++) {
			glEnableVertexAttribArray(layoutCount);
			glVertexAttribPointer(layoutCount, layout.getT(i).vecS, layout.getT(i).type, layout.getT(i).glBool, layout.getStride(), (void*)offset);
			offset += sizeof(float) * layout.getT(i).vecS;
			layoutCount++;
		}
		unBind();
		vb.unBind();
	}
	void add(tupple layout) {
		bind();
		glEnableVertexAttribArray(layoutCount);
		glVertexAttribPointer(layoutCount, layout.vecS, layout.type, layout.glBool, 0, 0);
		layoutCount++;
		unBind();
	}
	void bind() const {
		glBindVertexArray(id);
	}
	void unBind() {
		glBindVertexArray(0);
	}

};