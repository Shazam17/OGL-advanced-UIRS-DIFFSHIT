#pragma once
#include <glad/glad.h>
#include <vector>


class IBO {
	unsigned id;

public:

	IBO() {
		id = 0;
	}

	IBO(void * data, unsigned size) {
		glCreateBuffers(1, &id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	}

	void bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}
};
