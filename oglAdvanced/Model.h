
#include "Mesh.h"
#include "Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
public:
	vector<Mesh> meshes;
	string directory;

	void loadModel(string path) {
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			cout << "ASSIMP:LOAD MODEL ERROR" << endl;
			return;
		}

		directory = path.substr(0, path.find_first_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode* root, const aiScene* scene) {

		for (unsigned i = 0; i < root->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[root->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));

		}

		for (unsigned i = 0; i < root->mNumChildren; i++) {
			processNode(root->mChildren[i], scene);
		}
	}

	Vertex aiVertexConv(aiMesh* mesh, unsigned i) {
		Vertex vert;
		aiVector3D inVert = mesh->mVertices[i];
		aiVector3D normal = mesh->mNormals[i];
		
		vert.position.x = inVert.x;
		vert.position.y = inVert.y;
		vert.position.z = inVert.z;

		vert.normal.x = normal.x;
		vert.normal.y = normal.y;
		vert.normal.z = normal.z;

		if (mesh->mTangents != NULL) {

			aiVector3D tangent = mesh->mTangents[i];
			aiVector3D bitTangent = mesh->mBitangents[i];


			vert.tang.x = tangent.x;
			vert.tang.y = tangent.y;
			vert.tang.z = tangent.z;

			vert.bitTang.x = bitTangent.x;
			vert.bitTang.y = bitTangent.y;
			vert.bitTang.z = bitTangent.z;
		}
		

		if (mesh->mTextureCoords[0]) {
			vert.tCord.x = mesh->mTextureCoords[0][i].x;
			vert.tCord.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			vert.tCord.x = vert.tCord.y = 0.0f;
		}

		return vert;
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		vector<Vertex> vertices;
		vector<unsigned> indices;
		vector<Texture> textures;

		for (unsigned i = 0; i < mesh->mNumVertices; i++) {
			vertices.push_back(aiVertexConv(mesh, i));
		}

		for (unsigned i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];

			for (unsigned j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}
		return Mesh(vertices, indices, textures);
	}

	vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName) {
		vector<Texture> textures;

		for (unsigned i = 0; i < material->GetTextureCount(type); i++) {

			aiString str;
			material->GetTexture(type, i, &str);

			string path = directory + "/" + str.C_Str();

			Texture txt(path, typeName);
			textures.push_back(txt);
		}


		return textures;
	}


public:
	Model(string path) {
		loadModel(path);

	}

	void draw(Shader shad) {
		for (const auto &mesh : meshes) {
			mesh.draw(shad);
		}
	}
};
