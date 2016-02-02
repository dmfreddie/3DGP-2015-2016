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

    void
    setScene(std::shared_ptr<const SceneModel::Context> scene);
	void CompileShaders();
	void EnableSpikey();
	void RenderMode();
	void ToggleOutlineMode();

private:
	void CompileShader(std::string shaderFileName, GLenum shaderType, GLuint& shaderVariable);

	std::string GenerateUniforms(GLuint program, std::string name_){}
	
	void LoadTexture(std::string textureName, GLuint texture);

    void windowViewWillStart(std::shared_ptr<tygra::Window> window) override;
	
    void
    windowViewDidReset(std::shared_ptr<tygra::Window> window,
                       int width,
                       int height) override;

    void
    windowViewDidStop(std::shared_ptr<tygra::Window> window) override;
    
    void
    windowViewRender(std::shared_ptr<tygra::Window> window) override;

    std::shared_ptr<const SceneModel::Context> scene_;

	GLuint hexTexture;
	GLuint hex1Texture;
	GLuint hex2Texture;
	GLuint hex3Texture;
	GLuint first_program_;
	GLuint spikey_program_;
	/*GLuint blockIndex;
	GLint blockSize;
	GLubyte * blockBuffer;
	GLuint indices[5];
	GLint offset[5];
	GLuint uboHandle;*/
	
	struct Light
	{
		glm::vec3 position;
		float range;
		glm::vec3 direction;
		float field_of_view;
		glm::mat4 projection_view_xform;
		glm::vec3 intensity;
	};

	std::vector<Light> lights;

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec2 texcoord;
	};

	GLuint vertex_vbo;
	GLuint element_vbo; // VertexBufferObject for the elements (indices)
	GLuint vao; // VertexArrayObject for the shape's vertex array settings

	struct MeshGL
	{
		GLuint first_element_index;
		GLuint element_count;
		GLuint first_vertex_index;
	};

	GLuint vertex_shader;
	GLuint fragment_shader;

	GLuint spikeyvertex_shader;
	GLuint spikeyfragment_shader;
	GLuint geometary_shader;

	std::map<SceneModel::MeshId, MeshGL> meshes_;
	int windowWidth, windowHeight = 0;

    
	std::unordered_map<std::string, GLuint> textures;
	std::unordered_map<std::string, GLuint> uniforms;
	bool spikey = false;

	enum RenderMode_
	{
		Point,
		Wireframe,
		Face
	};

	bool outlineMode = false;
	RenderMode_ currentRenderMode = RenderMode_::Face;
};
