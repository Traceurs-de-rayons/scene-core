
#include "Scene.hpp"
#include <string.h>

uint32_t Scene::getVertexAmount()
{
	uint32_t res = 0;

	for (const auto& [key, asset] : assets_)
	{
		if (!std::holds_alternative<Asset::ObjectData>(asset.content_)) continue;

		for (const auto& mesh : std::get<Asset::ObjectData>(asset.content_).meshes)
			res += mesh->vertices_.size();
	}
}

uint32_t Scene::getIndexAmount()
{
	uint32_t res = 0;

	for (const auto& [key, asset] : assets_)
	{
		if (!std::holds_alternative<Asset::ObjectData>(asset.content_)) continue;

		for (const auto& mesh : std::get<Asset::ObjectData>(asset.content_).meshes)
		{
			for (const auto& submesh : mesh->subMeshes_)
				res += submesh->indices_.size();
		}
	}
}

void Scene::loadVertices(Vertex *buffer)
{
	uint32_t pos = 0;

	for (const auto& [key, asset] : assets_)
	{
		if (!std::holds_alternative<Asset::ObjectData>(asset.content_)) continue;

		for (const auto& mesh : std::get<Asset::ObjectData>(asset.content_).meshes)
		{
			const auto& verts = mesh->vertices_;
			memcpy(buffer + pos, verts.data(), verts.size() * sizeof(Vertex));
			mesh->vertexCount_ = mesh->vertices_.size();
			mesh->vertexBufferOffset_ = pos;
			pos += mesh->vertices_.size();
		}
	}
}

void Scene::loadVerticesSSBO(VertexSSBO *buffer)
{

}

void Scene::loadIndices(uint32_t *buffer)
{
	uint32_t pos = 0;

	for (const auto& [key, asset] : assets_)
	{
		if (!std::holds_alternative<Asset::ObjectData>(asset.content_)) continue;

		for (const auto& mesh : std::get<Asset::ObjectData>(asset.content_).meshes)
		{
			for (const auto& submesh : mesh->subMeshes_)
			{
				const auto& indices = submesh->indices_;
				memcpy(buffer + pos, indices.data(), indices.size() * sizeof(uint32_t));
				submesh->indexCount_ = submesh->indices_.size();
				submesh->indexBufferOffset_ = pos;
				pos += submesh->indices_.size();
			}
		}
	}
}
