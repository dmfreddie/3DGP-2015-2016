#include "MyView.hpp"
#include <SceneModel/SceneModel.hpp>
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>

MyView::
MyView() : hexTexture(0),
		   hex1Texture(0),
		   hex2Texture(0),
		   hex3Texture(0),
		   first_program_(0),
		   spikey_program_(0)
{
}

MyView::
~MyView() {
}

void MyView::
setScene(std::shared_ptr<const SceneModel::Context> scene)
{
	scene_ = scene;
}

void MyView::
EnableSpikey()
{
	spikey = !spikey;
}

void MyView::
RenderMode()
{
	switch (currentRenderMode)
	{
	case MyView::Point:
		currentRenderMode = RenderMode_::Wireframe;
		break;
	case MyView::Wireframe:
		currentRenderMode = RenderMode_::Face;
		break;
	case MyView::Face:
		currentRenderMode = RenderMode_::Point;
		break;
	default:
		break;
	}
}

void MyView::
ToggleOutlineMode()
{
	outlineMode = !outlineMode;
}

void MyView::
CompileShader(std::string shaderFileName, GLenum shaderType, GLuint& shaderVariable)
{
	GLint compile_status = 0;
	shaderVariable = glCreateShader(shaderType);
	std::string shader_string = tygra::stringFromFile(shaderFileName);
	const char *shader_code = shader_string.c_str();
	glShaderSource(shaderVariable, 1, (const GLchar **)&shader_code, NULL);
	glCompileShader(shaderVariable);
	glGetShaderiv(shaderVariable, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(shaderVariable, string_length, NULL, log);
		std::cerr << log << std::endl;
	}
}

void MyView::CompileShaders()
{

	std::cout << "Compiling Shaders...";

	CompileShader("sponza_vs.glsl", GL_VERTEX_SHADER, vertex_shader);
	CompileShader("sponza_fs.glsl", GL_FRAGMENT_SHADER, fragment_shader);
	CompileShader("spikeysponza_vs.glsl", GL_VERTEX_SHADER, spikeyvertex_shader);
	CompileShader("sponza_gs.glsl", GL_GEOMETRY_SHADER, geometary_shader);
	CompileShader("spikeysponza_fs.glsl", GL_FRAGMENT_SHADER, spikeyfragment_shader);


	first_program_ = glCreateProgram();
	glAttachShader(first_program_, vertex_shader);
	glBindAttribLocation(first_program_, 0, "vertex_position");
	glBindAttribLocation(first_program_, 1, "vertex_normal");
	glBindAttribLocation(first_program_, 2, "vertex_tangent");
	glBindAttribLocation(first_program_, 3, "vertex_texcoord");
	glDeleteShader(vertex_shader);
	glAttachShader(first_program_, fragment_shader);
	glDeleteShader(fragment_shader);
	glLinkProgram(first_program_);

	spikey_program_ = glCreateProgram();
	glAttachShader(spikey_program_, spikeyvertex_shader);
	glBindAttribLocation(spikey_program_, 0, "vertex_position");
	glBindAttribLocation(spikey_program_, 1, "vertex_normal");
	glDeleteShader(spikeyvertex_shader);
	glAttachShader(spikey_program_, geometary_shader);
	glDeleteShader(geometary_shader);
	glAttachShader(spikey_program_, spikeyfragment_shader);
	glDeleteShader(spikeyfragment_shader);
	glLinkProgram(spikey_program_);


	GLint link_status_normal = 0;
	GLint link_status_spikey = 0;
	glGetProgramiv(first_program_, GL_LINK_STATUS, &link_status_normal);
	glGetProgramiv(spikey_program_, GL_LINK_STATUS, &link_status_spikey);
	if (link_status_normal != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetProgramInfoLog(first_program_, string_length, NULL, log);
		std::cerr << log << std::endl;
	}
	else
		std::cout << "Standard Shader Compilation successful!" << std::endl;

	if (link_status_spikey != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetProgramInfoLog(spikey_program_, string_length, NULL, log);
		std::cerr << log << std::endl;
	}
	else
		std::cout << "Spikey Shader Compilation successful!" << std::endl;

}

void MyView::
windowViewWillStart(std::shared_ptr<tygra::Window> window)
{
	assert(scene_ != nullptr);

	CompileShaders();
	std::cout << "Press F2 to view the direction of the vertex normals." << std::endl;
	std::cout << "Press F3 to enable wireframe mode." << std::endl;
	std::cout << "Press F4 to change rendering mode (Fill, Line, Point)." << std::endl;
	std::cout << "Press F5 to recompile the shader." << std::endl;
	

	SceneModel::GeometryBuilder builder;
	std::vector<Vertex> vertices;
	std::vector<SceneModel::InstanceId> elements;
	const auto& scene_meshes = builder.getAllMeshes();

	for (const auto& scene_mesh : scene_meshes) {

		MeshGL& newMesh = meshes_[scene_mesh.getId()];
		const auto& source_mesh = builder.getMeshById(scene_mesh.getId());
		const auto& positions = source_mesh.getPositionArray();
		const auto& elementsArr = source_mesh.getElementArray();
		const auto& normals = source_mesh.getNormalArray();
		const auto& tangents = source_mesh.getTangentArray();
		const auto& text_coord = source_mesh.getTextureCoordinateArray();

		newMesh.first_vertex_index = vertices.size();
		vertices.reserve(vertices.size() + positions.size());

		for (unsigned int i = 0; i < positions.size(); ++i)
		{
			Vertex vertex;
			vertex.position = positions[i];
			vertex.normal = normals[i];
			vertex.tangent = tangents[i];
			vertex.texcoord = text_coord[i];
			vertices.push_back(vertex);
		}

		newMesh.first_element_index = elements.size();
		elements.insert(elements.end(), elementsArr.begin(), elementsArr.end());
		newMesh.element_count = elementsArr.size();
	}

	glGenBuffers(1, &vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		vertices.size() * sizeof(Vertex), // size of data in bytes
		vertices.data(), // pointer to the data
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &element_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, element_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		elements.size() * sizeof(unsigned int), // size of data in bytes
		elements.data(), // pointer to the data
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), TGL_BUFFER_OFFSET_OF(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), TGL_BUFFER_OFFSET_OF(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), TGL_BUFFER_OFFSET_OF(Vertex, tangent));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), TGL_BUFFER_OFFSET_OF(Vertex, texcoord));

	// make nothing active (deactivate vbo and vao)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//LoadTexture("spec1.png", hex2Texture);
	//LoadTexture("spec0.png", hexTexture);
	tygra::Image texture_image = tygra::imageFromPNG("spec0.png");
	if (texture_image.containsData()) {
		glGenTextures(1, &hexTexture);
		glBindTexture(GL_TEXTURE_2D, hexTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			texture_image.width(),
			texture_image.height(),
			0,
			pixel_formats[texture_image.componentsPerPixel()],
			texture_image.bytesPerComponent() == 1 ? GL_UNSIGNED_BYTE
			: GL_UNSIGNED_SHORT,
			texture_image.pixels());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	textures["spec0.png"] = hexTexture;

	//LoadTexture("diff1.png", hex1Texture);
	//LoadTexture("diff0.png", hex3Texture);
	texture_image = tygra::imageFromPNG("spec1.png");
	if (texture_image.containsData()) {
		glGenTextures(1, &hex1Texture);
		glBindTexture(GL_TEXTURE_2D, hex1Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			texture_image.width(),
			texture_image.height(),
			0,
			pixel_formats[texture_image.componentsPerPixel()],
			texture_image.bytesPerComponent() == 1 ? GL_UNSIGNED_BYTE
			: GL_UNSIGNED_SHORT,
			texture_image.pixels());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	textures["spec1.png"] = hex1Texture;

	texture_image = tygra::imageFromPNG("diff0.png");
	if (texture_image.containsData()) {
		glGenTextures(1, &hex2Texture);
		glBindTexture(GL_TEXTURE_2D, hex2Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			texture_image.width(),
			texture_image.height(),
			0,
			pixel_formats[texture_image.componentsPerPixel()],
			texture_image.bytesPerComponent() == 1 ? GL_UNSIGNED_BYTE
			: GL_UNSIGNED_SHORT,
			texture_image.pixels());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	textures["diff0.png"] = hex2Texture;


	texture_image = tygra::imageFromPNG("diff1.png");
	if (texture_image.containsData()) {
		glGenTextures(1, &hex3Texture);
		glBindTexture(GL_TEXTURE_2D, hex3Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			texture_image.width(),
			texture_image.height(),
			0,
			pixel_formats[texture_image.componentsPerPixel()],
			texture_image.bytesPerComponent() == 1 ? GL_UNSIGNED_BYTE
			: GL_UNSIGNED_SHORT,
			texture_image.pixels());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	textures["diff1.png"] = hex3Texture;


	for (unsigned int i = 0; i < scene_->getAllLights().size(); ++i)
	{
		Light light;
		light.position = scene_->getAllLights()[i].getPosition();
		light.range = scene_->getAllLights()[i].getRange();
		light.intensity = scene_->getAllLights()[i].getIntensity();
		lights.push_back(light);
	}


	//Uniforms
	uniforms["projection_view_model_xform"] = glGetUniformLocation(first_program_, "projection_view_model_xform");
	uniforms["model_xform"] = glGetUniformLocation(first_program_, "model_xform");
	uniforms["vertex_diffuse_colour"] = glGetUniformLocation(first_program_, "vertex_diffuse_colour");
	uniforms["vertex_ambient_colour"] = glGetUniformLocation(first_program_, "vertex_ambient_colour");
	uniforms["vertex_spec_colour"] = glGetUniformLocation(first_program_, "vertex_spec_colour");
	uniforms["vertex_shininess"] = glGetUniformLocation(first_program_, "vertex_shininess");
	uniforms["is_vertex_shiney"] = glGetUniformLocation(first_program_, "is_vertex_shiney");
	uniforms["camera_position"] = glGetUniformLocation(first_program_, "camera_position");
	uniforms["MAX_LIGHTS"] = glGetUniformLocation(first_program_, "MAX_LIGHTS");
	uniforms["outline"] = glGetUniformLocation(first_program_, "outline");

	uniforms["spikey_projection_view_model_xform"] = glGetUniformLocation(spikey_program_, "projection_view_model_xform");
	uniforms["spikey_model_xform"] = glGetUniformLocation(spikey_program_, "model_xform");
	uniforms["spikey_normal_xform"] = glGetUniformLocation(spikey_program_, "normal_xform");
}

void MyView::LoadTexture(std::string textureName, GLuint texture)
{
	tygra::Image texture_image = tygra::imageFromPNG(textureName);
	if (texture_image.containsData()) {
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			texture_image.width(),
			texture_image.height(),
			0,
			pixel_formats[texture_image.componentsPerPixel()],
			texture_image.bytesPerComponent() == 1 ? GL_UNSIGNED_BYTE
			: GL_UNSIGNED_SHORT,
			texture_image.pixels());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	textures[textureName] = texture;

}

void MyView::
windowViewDidReset(std::shared_ptr<tygra::Window> window,
int width,
int height)
{
	glViewport(0, 0, width, height);
}

void MyView::
windowViewDidStop(std::shared_ptr<tygra::Window> window)
{
	glDeleteProgram(first_program_);
	glDeleteProgram(spikey_program_);
	for (const auto& instance : scene_->getAllInstances()) {

		/*glDeleteBuffers(1, &meshes_[instance.getMeshId()].position_vbo);
		glDeleteBuffers(1, &meshes_[instance.getMeshId()].normal_vbo);
		glDeleteBuffers(1, &meshes_[instance.getId()].tangent_vbo);
		glDeleteBuffers(1, &meshes_[instance.getMeshId()].position_vbo);
		glDeleteBuffers(1, &meshes_[instance.getMeshId()].textcoord_vbo);*/
		/*glDeleteBuffers(1, &meshes_[instance.getMeshId()].element_vbo);
		glDeleteVertexArrays(1, &meshes_[instance.getMeshId()].vao);*/
	}
}

void MyView::
windowViewRender(std::shared_ptr<tygra::Window> window)
{
	assert(scene_ != nullptr);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.f, 0.f, 0.25f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	GLint viewport_size[4];
	glGetIntegerv(GL_VIEWPORT, viewport_size);
	const float aspect_ratio = viewport_size[2] / (float)viewport_size[3];

	glm::mat4 projection_xform = glm::perspective(scene_->getCamera().getVerticalFieldOfViewInDegrees(), aspect_ratio, scene_->getCamera().getNearPlaneDistance(), scene_->getCamera().getFarPlaneDistance());
	glm::mat4 view_xform = glm::lookAt(scene_->getCamera().getPosition(), scene_->getCamera().getPosition() + scene_->getCamera().getDirection(), scene_->getUpDirection());

	glUseProgram(first_program_);

	glUniform3fv(uniforms["camera_position"], 1, glm::value_ptr(scene_->getCamera().getPosition()));
	glUniform1f(uniforms["MAX_LIGHTS"], (GLfloat)scene_->getAllLights().size());

	for (unsigned int i = 0; i < scene_->getAllLights().size(); ++i)
	{
		lights[i].position = scene_->getAllLights()[i].getPosition();
		lights[i].range = scene_->getAllLights()[i].getRange();
		lights[i].intensity = scene_->getAllLights()[i].getIntensity();
	}

	GLuint lightPosID = 0;
	GLuint lightRangeID = 0;
	GLuint lightIntensityID = 0;

	for (unsigned int i = 0; i < scene_->getAllLights().size(); ++i)
	{
		std::string pos = "LightSource[" + std::to_string(i) + "].position";
		std::string range = "LightSource[" + std::to_string(i) + "].range";
		std::string intensity = "LightSource[" + std::to_string(i) + "].intensity";

		lightPosID = glGetUniformLocation(first_program_, pos.c_str());
		glUniform3fv(lightPosID, 1, glm::value_ptr(lights[i].position));
		lightRangeID = glGetUniformLocation(first_program_, range.c_str());
		glUniform1f(lightRangeID, lights[i].range);
		lightIntensityID = glGetUniformLocation(first_program_, intensity.c_str());
		glUniform3fv(lightIntensityID, 1, glm::value_ptr(lights[i].intensity));
	}

	glBindVertexArray(vao);

	SceneModel::Material material = scene_->getAllMaterials()[0];
	glm::mat4 projection_view_mod_xform;
	glm::mat4 inverse_normal_xform;

	glm::vec3 diffuse;
	glm::vec3 ambient;
	glm::vec3 spec;
	std::string diffTex;
	std::string specTex;
	float shininess;
	bool isShiney;

	switch (currentRenderMode)
	{
	case MyView::Point:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case MyView::Wireframe:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case MyView::Face:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	default:
		break;
	}


	float outLineInt = 0;

	glUniform1f(uniforms["outline"], outLineInt);

	for (const auto& instance : scene_->getAllInstances())
	{
		//Populate uniform variables

		glm::mat4 model_xform = glm::mat4(instance.getTransformationMatrix());
		const MeshGL& mesh = meshes_[instance.getMeshId()];

		projection_view_mod_xform = projection_xform * view_xform * model_xform;
		inverse_normal_xform = view_xform * model_xform;

		material = scene_->getMaterialById(instance.getMaterialId());
		diffuse = material.getDiffuseColour();
		ambient = material.getAmbientColour();
		spec = material.getSpecularColour();
		diffTex = material.getDiffuseTexture();
		specTex = material.getSpecularTexture();
		shininess = material.getShininess();
		isShiney = material.isShiny();


		glUniformMatrix4fv(uniforms["projection_view_model_xform"], 1, GL_FALSE, glm::value_ptr(projection_view_mod_xform));
		glUniformMatrix4fv(uniforms["model_xform"], 1, GL_FALSE, glm::value_ptr(model_xform));
		glUniform3fv(uniforms["vertex_diffuse_colour"], 1, glm::value_ptr(diffuse));
		glUniform3fv(uniforms["vertex_ambient_colour"], 1, glm::value_ptr(ambient));
		glUniform3fv(uniforms["vertex_spec_colour"], 1, glm::value_ptr(spec));
		glUniform1f(uniforms["vertex_shininess"], shininess);
		glUniform1f(uniforms["is_vertex_shiney"], (float)isShiney);

		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[diffTex]);
		glUniform1i(glGetUniformLocation(first_program_, "diffuse_texture"), 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[specTex]);
		glUniform1i(glGetUniformLocation(first_program_, "specular_texture"), 0);*/


		//Drawing
		//glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh.element_count, GL_UNSIGNED_INT, (GLvoid*)(mesh.first_element_index * sizeof(int)), scene_->getInstancesByMeshId(instance.getMeshId()).size(), mesh.first_vertex_index); 

		glDrawElementsBaseVertex(GL_TRIANGLES, mesh.element_count, GL_UNSIGNED_INT, (GLvoid*)(mesh.first_element_index * sizeof(int)), mesh.first_vertex_index);

		
	}
	if (outlineMode)
	{
		outLineInt = 1;
		glUniform1f(uniforms["outline"], outLineInt);

		//Populate uniform variables
		for (const auto& instance : scene_->getAllInstances())
		{
			glm::mat4 model_xform = glm::mat4(instance.getTransformationMatrix());
			const MeshGL& mesh = meshes_[instance.getMeshId()];

			projection_view_mod_xform = projection_xform * view_xform * model_xform;

			glUniformMatrix4fv(uniforms["projection_view_model_xform"], 1, GL_FALSE, glm::value_ptr(projection_view_mod_xform));
			glUniformMatrix4fv(uniforms["model_xform"], 1, GL_FALSE, glm::value_ptr(model_xform));

			

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElementsBaseVertex(GL_TRIANGLES, mesh.element_count, GL_UNSIGNED_INT, (GLvoid*)(mesh.first_element_index * sizeof(int)), mesh.first_vertex_index);
		}
	}
	if (spikey)
	{
		glUseProgram(spikey_program_);

		for (const auto& instance : scene_->getAllInstances())
		{
			//Populate uniform variables

			glm::mat4 model_xform = glm::mat4(instance.getTransformationMatrix());
			const MeshGL& mesh = meshes_[instance.getMeshId()];

			glm::mat4 projection_view_mod_xform = projection_xform * view_xform * model_xform;
			glm::mat4 inverse_normal_xform = glm::transpose(glm::inverse(model_xform));

			glUniformMatrix4fv(uniforms["spikey_projection_view_model_xform"], 1, GL_FALSE, glm::value_ptr(projection_view_mod_xform));
			glUniformMatrix4fv(uniforms["spikey_model_xform"], 1, GL_FALSE, glm::value_ptr(model_xform));
			glUniformMatrix4fv(uniforms["spikey_normal_xform"], 1, GL_FALSE, glm::value_ptr(inverse_normal_xform));
			
			glDrawElementsBaseVertex(GL_POINTS, mesh.element_count, GL_UNSIGNED_INT, (GLvoid*)(mesh.first_element_index * sizeof(int)), mesh.first_vertex_index);

		}
	}
}