#include "renderer.h"
#include <iostream>
#include "../material/cubeMaterial.h"
#include "../material/rockFogMaterial.h"
#include <string>
#include <algorithm>

Renderer::Renderer() {

	mCubeShader = new Shader("assets/shaders/cube.vert", "assets/shaders/cube.frag");
	mRockFogShader = new Shader("assets/shaders/rockFog.vert", "assets/shaders/rockFog.frag");
}

Renderer::~Renderer() {

}

void Renderer::setClearColor(glm::vec3 color) {

	glClearColor(color.r, color.g, color.b, 1.0);
}

void Renderer::render(Scene* scene, Camera* camera, DirectionalLight* dirLight, AmbientLight* ambLight) {




	// 1 Depth and stencil test
	// 1.1 Depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	// 1.2 Polygon offset
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_POLYGON_OFFSET_LINE);

	// 1.3 Stencil test
	glEnable(GL_STENCIL_TEST);

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glStencilMask(0xFF);


	// 1.4 Blend
	glDisable(GL_BLEND);


	// 2 Clear canvas
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// 3 Clear two containers
	mOpacityObjects.clear();
	mTransparentObjects.clear();

	projectObject(scene);

	std::sort(
		mTransparentObjects.begin(),
		mTransparentObjects.end(),
		[camera](const Mesh* a, const Mesh* b) {


			auto viewMatrix = camera->getViewMatrix();

			auto modelMatrixA = a->getModelMatrix();
			auto worldPositionA = modelMatrixA * glm::vec4(0.0, 0.0, 0.0, 1.0);
			auto cameraPositionA = viewMatrix * worldPositionA;

			auto modelMatrixB = b->getModelMatrix();
			auto worldPositionB = modelMatrixB * glm::vec4(0.0, 0.0, 0.0, 1.0);
			auto cameraPositionB = viewMatrix * worldPositionB;

			return cameraPositionA.z < cameraPositionB.z;
		}
	);

	// 4 Render two containers
	for (int i = 0; i < mOpacityObjects.size(); i++) {

		renderObject(mOpacityObjects[i], camera, dirLight, ambLight);

	}


	for (int i = 0; i < mTransparentObjects.size(); i++) {

		renderObject(mTransparentObjects[i], camera, dirLight, ambLight);

	}


}

void Renderer::projectObject(Object* obj) {

	if (obj->getType() == ObjectType::Mesh || obj->getType() == ObjectType::InstancedMesh) {

		Mesh* mesh = (Mesh*)obj;
		auto material = mesh->mMaterial;
		if (material->mBlend) {

			mTransparentObjects.push_back(mesh);
		}
		else {

			mOpacityObjects.push_back(mesh);
		}
	}

	auto children = obj->getChildren();
	for (int i = 0; i < children.size(); i++) {

		projectObject(children[i]);
	}
}

Shader* Renderer::pickShader(MaterialType type) {

	Shader* result = nullptr;

	switch (type) {
	case MaterialType::CubeMaterial:
		result = mCubeShader;
		break;
	case MaterialType::RockFogMaterial:
		result = mRockFogShader;
		break;
	default:
		std::cout << "Unkown material type to shader" << std::endl;
		break;
	}

	return result;
}

void Renderer::renderObject(Object* object, Camera* camera, DirectionalLight* dirLight,AmbientLight* ambLight) {

	// 1 Render only mesh
	if (object->getType() == ObjectType::Mesh) {

		// 1.1 It is a mesh
		auto mesh = (Mesh*)object;
		auto geometry = mesh->mGeometry;

		Material* material = nullptr;
		if (mGlobalMaterial != nullptr) {

			material = mGlobalMaterial;
		}
		else {

			material = mesh->mMaterial;
		}


		// 2 Set rendering status
		setDepthState(material);
		setPolygonOffsetState(material);
		setStencilState(material);
		setBlendState(material);
		setFaceCullingState(material);

		// 3.1 Choose shader
		Shader* shader = pickShader(material->mType);

		// 3.2 Update uniform
		// 3.2.1 Create program
		shader->begin();

		switch (material->mType) {

		case MaterialType::CubeMaterial: {

			CubeMaterial* cubeMat = (CubeMaterial*)material;
			mesh->setPosition(camera->mPosition);
			// MVP matrix
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			// Texture bind and sampling
			shader->setInt("cubeSampler", 0);
			cubeMat->mDiffuse->bind();
		}
										break;
		case MaterialType::RockFogMaterial: {
			// pointer type change
			RockFogMaterial* phongMat = (RockFogMaterial*)material;

			// Texture bind and sampling
			shader->setInt("rockSampler", 0);
			phongMat->mDiffuse->bind();

			shader->setInt("parallaxMapSampler", 1);
			phongMat->mParallaxMap->bind();

			// 3.2.3 MVP matrix
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());


			// 3.2.3 Light
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);


			shader->setVector3("ambientColor", ambLight->mColor);


			// 3.2.4 Camera
			shader->setVector3("cameraPosition", camera->mPosition);


		}
										break;
		default:
			break;
		}

		// 3.3 VAO
		glBindVertexArray(geometry->getVao());

		// 3.4 Draw
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);

	}
}

void Renderer::setDepthState(Material* material) {

	if (material->mDepthTest) {

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(material->mDepthFunc);
	}
	else {

		glDisable(GL_DEPTH_TEST);
	}

	// 2.2 Depth write
	if (material->mDepthWrite) {

		glDepthMask(GL_TRUE);
	}
	else {

		glDepthMask(GL_FALSE);
	}

}

void Renderer::setPolygonOffsetState(Material* material) {

	if (material->mPolygonOffset) {

		glEnable(material->mPolygonOffsetType);
		glPolygonOffset(material->mFactor, material->mUnit);

	}
	else {
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_POLYGON_OFFSET_LINE);

	}

}

void Renderer::setStencilState(Material* material) {

	if (material->mStencilTest) {

		glEnable(GL_STENCIL_TEST);

		glStencilOp(material->mSFail, material->mZFail, material->mZPass);
		glStencilMask(material->mStencilMask);
		glStencilFunc(material->mStencilFunc, material->mStencilRef, material->mStencilFuncMask);

	}
	else {

		glDisable(GL_STENCIL_TEST);
	}
}

void Renderer::setBlendState(Material* material) {

	if (material->mBlend) {

		glEnable(GL_BLEND);
		glBlendFunc(material->mSFactor, material->mDFactor);
	}
	else {

		glDisable(GL_BLEND);
	}
}

void Renderer::setFaceCullingState(Material* material) {

	if (material->mFaceCulling) {

		glEnable(GL_CULL_FACE);
		glFrontFace(material->mFrontFace);
		glCullFace(material->mCullFace);

	}
	else {

		glDisable(GL_CULL_FACE);
	}
}