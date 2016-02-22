#pragma once

#include <SceneModel/SceneModel_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>

class MyView : public tygra::WindowViewDelegate
{
public:
	
    MyView();
	
    ~MyView();

    void setScene(std::shared_ptr<const SceneModel::Context> scene);

#pragma region My External Faceing Function Definitions

	void ResetConsole();
	void EnableSpikey();
	void RenderMode();
	void ToggleOutlineMode();
	void AddToLength(float val_);
	void AddToSpecSmudge(float val_);
#pragma endregion

private:

#pragma region Private Function Definitions

	void CompileShader(std::string shaderFileName, GLenum shaderType, GLuint& shaderVariable);
	
	void CompileShaders();

	bool CheckLinkStatus(GLuint shaderProgram);

	std::string GenerateUniforms(GLuint program, std::string name_){}
	
	void LoadTexture(std::string textureName);

    void windowViewWillStart(std::shared_ptr<tygra::Window> window) override;
	
    void windowViewDidReset(std::shared_ptr<tygra::Window> window, int width, int height) override;

    void windowViewDidStop(std::shared_ptr<tygra::Window> window) override;
    
    void windowViewRender(std::shared_ptr<tygra::Window> window) override;

	void Getuniforms();

#pragma endregion

#pragma region Enumeration Definitions
	enum RenderMode_
	{
		Point,
		Wireframe,
		Face
	};
#pragma endregion

#pragma region Structure Definitions
	struct Light
	{
		glm::vec3 position;
		float range;
		glm::vec3 direction;
		float field_of_view;
		glm::mat4 projection_view_xform;
		glm::vec3 intensity;
	};

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec2 texcoord;
	};

	struct MeshGL
	{
		GLuint first_element_index;
		GLuint element_count;
		GLuint first_vertex_index;
	};

#pragma endregion

#pragma region STL Container variables

	std::vector<Light> lights;
	std::map<SceneModel::MeshId, MeshGL> meshes_;
	std::unordered_map<std::string, GLuint> textures;
	std::unordered_map<std::string, GLuint> uniforms;

#pragma endregion

#pragma region Standard Variables

	std::shared_ptr<const SceneModel::Context> scene_;

	GLuint first_program_;
	GLuint spikey_program_;

	GLuint vertex_vbo;
	GLuint element_vbo; // VertexBufferObject for the elements (indices)
	GLuint vao; // VertexArrayObject for the shape's vertex array settings

	GLuint vertex_shader;
	GLuint fragment_shader;

	GLuint spikeyvertex_shader;
	GLuint spikeyfragment_shader;
	GLuint geometary_shader;
	
	bool spikey = false;
	bool outlineMode = false;

	float specularSmudge = 2.0f;
	float normalLineLength = 1.0f;
	int windowWidth, windowHeight = 0;
	
	RenderMode_ currentRenderMode = RenderMode_::Face;
#pragma endregion

};
