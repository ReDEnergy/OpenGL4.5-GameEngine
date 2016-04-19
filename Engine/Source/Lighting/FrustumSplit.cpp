#include "FrustumSplit.h"

#include <include/math.h>
#include <include/gl.h>
#include <include/glm.h>
#include <include/glm_utils.h>

#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>
#include <Component/AABB.h>
#include <Component/Mesh.h>
#include <Component/Transform/Transform.h>
#include <GPU/Shader.h>

#include <Lighting/DirectionalLight.h>

#include <Utils/3D.h>

#include <Manager/Manager.h>
#include <Manager/RenderingSystem.h>
#include <Debugging/TextureDebugger.h>

using namespace std;

FrustumSplit::FrustumSplit()
{
	nrFrustumSections = 5;
	lightViews.resize(nrFrustumSections);
	lightProjections.resize(nrFrustumSections);
}

FrustumSplit::~FrustumSplit()
{
}

void FrustumSplit::Update(DirectionalLight *Ref)
{
	// Compute bounding light space aligned AABB
	auto position = Ref->transform->GetWorldPosition();
	auto PI = Ref->GetProjectionInfo();

	for (unsigned int i = 0; i < nrFrustumSections; i++) {
		zones[i]->aabb->Update(Ref->transform->GetWorldRotation());
		Transform *T = zones[i]->aabb->transform;
		float size = max(T->GetScale().x, T->GetScale().y);

		// Change frustum area ViewProjection matrixes
		Ref->SetOrthgraphic(size, size, PI.zNear, PI.zFar);
		Ref->SetPosition(T->GetWorldPosition() + Ref->transform->GetLocalOZVector() * (PI.zFar / 2));
		lightViews[i] = Ref->View;
		lightProjections[i] = Ref->Projection;
	}

	//viewCamera->aabb->Update(Ref->transform->GetWorldRotation());
	//Transform *T = viewCamera->aabb->transform;
	//float size = max(T->GetScale().x, T->GetScale().z);
	//Ref->SetOrthgraphic(PI.width, PI.height, 0.1f, 100);
	Ref->SetPosition(position);
}

void FrustumSplit::SetCamera(const Camera* camera)
{
	viewCamera = camera;
	SplitFrustrum();
}

void FrustumSplit::RenderDebug(const Shader * shader) const
{
	for (auto zone : zones) {
		zone->RenderDebug(shader);
	}
}

void FrustumSplit::BindViewProjection(const Shader * shader, unsigned int index) const
{
	glUniformMatrix4fv(shader->loc_view_matrix, 1, false, glm::value_ptr(lightViews[index]));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, false, glm::value_ptr(lightProjections[index]));

}

void FrustumSplit::BindForUse(const Shader * shader) const
{
	glUniform1fv(shader->CSM_SplitDistance, nrFrustumSections, &splitDistances[1]);
	glUniformMatrix4fv(shader->CSM_LightView, nrFrustumSections, false, glm::value_ptr(lightViews[0]));
	glUniformMatrix4fv(shader->CSM_LightProjection, nrFrustumSections, false, glm::value_ptr(lightProjections[0]));
}

unsigned int FrustumSplit::GetNrSections() const
{
	return nrFrustumSections;
}

void FrustumSplit::SplitFrustrum()
{
	zones.clear();
	zones.reserve(nrFrustumSections);

	auto PI = viewCamera->GetProjectionInfo();

	splitDistances.clear();
	splitDistances.resize(nrFrustumSections + 1);
	splitDistances[0] = PI.zNear;
	vector<glm::vec3> planes;

	auto points = Utils3D::GetPerspectiveSection(splitDistances[0], PI.FoV, PI.aspectRatio);
	for (auto point : points)
		planes.push_back(point);

	// Use logarithmic split scale
	float scale = log(PI.zFar - PI.zNear);

	for (unsigned int i = 1; i <= nrFrustumSections; i++) {
		splitDistances[i] = (float)pow(M_E, ((i * scale) / nrFrustumSections)) + PI.zNear;
		auto points = Utils3D::GetPerspectiveSection(splitDistances[i], PI.FoV, PI.aspectRatio);
		for (auto point : points)
			planes.push_back(point);
	}

	///////////////////////////////////////////////////////////////////////////
	// Create AABB for each split section
	{
		vector<glm::vec2> text_coord;
		vector<glm::vec3> normals;
		vector<unsigned short> indices;

		for (int k = 0; k < 8; k++)
			normals.push_back(glm::vec3(0, 1, 0));

		Utils3D::PushQuadTriangle(indices, 4, 5, 1, 0);
		Utils3D::PushQuadTriangle(indices, 6, 7, 3, 2);
		Utils3D::PushQuadTriangle(indices, 4, 0, 3, 7);
		Utils3D::PushQuadTriangle(indices, 5, 6, 2, 1);

		// Create an AABB for each split section
		for (unsigned int i = 0; i < nrFrustumSections; i++) {
			GameObject *fz = new GameObject("split-zone");
			fz->transform = viewCamera->transform;
			auto mesh = new Mesh();

			for (int k = 0; k < 8; k++)
				mesh->positions.push_back(planes[i * 4 + k]);

			mesh->InitFromData(mesh->positions, normals, text_coord, indices);
			fz->SetMesh(mesh);

			zones.push_back(fz);
		}
	}
}

// Compute bounding light space aligned AABB
void FrustumSplit::UpdateBoundingBox(DirectionalLight *Ref) const
{
	//// TODO - move this to DirectionalLight (inclusive zones) - because they are light space aligned so not related at all with the gameCamera, except camera properties
	//for (unsigned int i = 0; i < nrFrustumSections; i++) {
	//	zones[i]->aabb->Update(Ref->transform->GetWorldRotation());
	//	Transform *T = zones[i]->aabb->transform;
	//	float size = max(T->GetScale().x, T->GetScale().y);

	//	// Change frustum area ViewProjection matrixes
	//	Ref->SetOrthgraphic(size, size, Ref->zNear, Ref->zFar);
	//	Ref->SetPosition(T->GetWorldPosition() + Ref->transform->GetLocalOZVector() * Ref->distanceTo);
	//	Ref->lightViews[i] = Ref->View;
	//	Ref->lightProjections[i] = Ref->Projection;
	//}

	//aabb->Update(Ref->transform->GetWorldRotation());
	//Transform *T = aabb->transform;
	//float size = max(T->GetScale().x, T->GetScale().z);
	//Ref->SetOrthgraphic(size, size, Ref->zNear, Ref->zFar);
	//Ref->SetPosition(T->GetWorldPosition() + Ref->transform->GetLocalOZVector() * Ref->distanceTo);
}