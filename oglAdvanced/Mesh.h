#pragma once
#include "VBO.h"
#include "VAO.h"
#include "IBO.h"
#include "Shader.h"
#include "Vertex.h"
#include "Texture.h"

#include <string>
using namespace std;

class Mesh {

	VAO va;
	VBO vb;
	IBO ib;


	void setupMesh() {
		va = VAO();
		va.bind();

		vb = VBO(&vertices[0], vertices.size() * sizeof(Vertex));
		ib = IBO(&indices[0], indices.size() * sizeof(unsigned));

		VAO::Layout la;
		la.add<float>(3);
		la.add<float>(3);
		la.add<float>(2);
		la.add<float>(3);
		la.add<float>(3);


		va.addLayout(la, vb, ib);

	}
public:
	vector<Vertex> vertices;
	vector<unsigned> indices;
	vector<Texture> textures;

	Mesh(vector<Vertex> verts, vector<unsigned> inds, vector<Texture> txts) {
		vertices = verts;
		indices = inds;
		textures = txts;

		setupMesh();
	}

	void draw(Shader shader) const {
		unsigned diffuseN = 1;
		unsigned specularN = 1;

		string number;
		for (unsigned i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);

			if (textures[i].type == "texture_diffuse") {
				number = to_string(diffuseN);
				diffuseN++;
			}
			if (textures[i].type == "texture_diffuse") {
				number = to_string(specularN);
				specularN++;
			}

			shader.setUniform1i(("material." + textures[i].type + number).c_str(), i);
			textures[i].bind();
		}

		va.bind();
		shader.bind();
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}


};