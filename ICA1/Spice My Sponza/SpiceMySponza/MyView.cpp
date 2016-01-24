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
		   first_program_(0)
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

void MyView::CompileShaders()
{

	std::cout << "Compiling Shaders...";
	GLint compile_status = 0;

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertex_shader_string = tygra::stringFromFile("sponza_vs.glsl");
	const char *vertex_shader_code = vertex_shader_string.c_str();
	glShaderSource(vertex_shader, 1,
		(const GLchar **)&vertex_shader_code, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(vertex_shader, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragment_shader_string = tygra::stringFromFile("sponza_fs.glsl");
	const char *fragment_shader_code = fragment_shader_string.c_str();
	glShaderSource(fragment_shader, 1,
		(const GLchar **)&fragment_shader_code, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(fragment_shader, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

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


	GLint link_status = 0;
	glGetProgramiv(first_program_, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetProgramInfoLog(first_program_, string_length, NULL, log);
		std::cerr << log << std::endl;
	}
	else
		std::cout << "Shader Compilation successful!" << std::endl;

	//Shader struct block
	/*blockIndex = glGetUniformBlockIndex(first_program_, "Light");

	glGetActiveUniformBlockiv(first_program_, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	blockBuffer = (GLubyte *)malloc(blockSize);
	// Query for the offsets of each block variable
	const GLchar *names[] = { "position", "range",
		"direction", "field_of_view", "projection_view_xform" };

	glGetUniformIndices(first_program_, 5, names, indices);

	glGetActiveUniformsiv(first_program_, 5, indices, GL_UNIFORM_OFFSET, offset);*/

}

void MyView::
windowViewWillStart(std::shared_ptr<tygra::Window> window)
{
	assert(scene_ != nullptr);

	CompileShaders();

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
	//LoadTexture("spec1.png", hexTexture);

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

	//LoadTexture("diff1.png", hex1Texture);
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

	/*glGenBuffers(1, &uboHandle);
	glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);*/

	for (int i = 0; i < scene_->getAllLights().size(); ++i)
	{
		Light light;
		light.position = scene_->getAllLights()[i].getPosition();
		light.range = scene_->getAllLights()[i].getRange();
		light.intensity = scene_->getAllLights()[i].getIntensity();
		lights.push_back(light);
	}

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


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hexTexture);
	glUniform1i(glGetUniformLocation(first_program_, "mystery_sampler"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hex1Texture);
	glUniform1i(glGetUniformLocation(first_program_, "mystery_sampler1"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hex2Texture);
	glUniform1i(glGetUniformLocation(first_program_, "mystery_sampler2"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hex3Texture);
	glUniform1i(glGetUniformLocation(first_program_, "mystery_sampler3"), 0);


	glm::vec3 camPos = scene_->getCamera().getPosition();

	GLuint camPosID = glGetUniformLocation(first_program_, "camera_position");
	glUniform3fv(camPosID, 3, glm::value_ptr(camPos));
	GLuint maxLightsID = glGetUniformLocation(first_program_, "MAX_LIGHTS");
	glUniform1f(maxLightsID, scene_->getAllLights().size());
	
	glm::vec3 positions[22];
	glm::vec3 intensities[22];
	float ranges[22];

	int count = scene_->getAllLights().size();
	for (int i = 0; i < scene_->getAllLights().size(); ++i)
	{
		positions[i] = scene_->getAllLights()[i].getPosition();
		ranges[i] = scene_->getAllLights()[i].getRange();
		intensities[i] = scene_->getAllLights()[i].getIntensity();
	}

	GLuint lightPosID = glGetUniformLocation(first_program_, "positions");
	glUniform3fv(lightPosID, count, (const float *)positions);
	GLuint lightRangeID = glGetUniformLocation(first_program_, "ranges");
	glUniform1fv(lightRangeID, count, (const float *)ranges);
	GLuint lightIntensityID = glGetUniformLocation(first_program_, "intensities");
	glUniform3fv(lightIntensityID, count, (const float *)intensities);

	glBindVertexArray(vao);

	for (const auto& instance : scene_->getAllInstances())
	{
		//Populate uniform variables

		glm::mat4 model_xform = glm::mat4(instance.getTransformationMatrix());
		const MeshGL& mesh = meshes_[instance.getMeshId()];

		glm::mat4 projection_view_mod_xform = projection_xform * view_xform * model_xform;
		glm::mat4 inverse_normal_xform = view_xform * model_xform;


		GLuint projection_id = glGetUniformLocation(first_program_, "projection_view_model_xform");
		glUniformMatrix4fv(projection_id, 1, GL_FALSE, glm::value_ptr(projection_view_mod_xform));

		GLuint model_xform_id = glGetUniformLocation(first_program_, "model_xform");
		glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

		GLfloat position[3] = { 0.0f, 5.0f, 0.0f };
		GLfloat direction[3] = { 1.0f, 1.0f, 1.0f };
		GLfloat range = 0.25f, field_of_view = scene_->getCamera().getVerticalFieldOfViewInDegrees();
		glm::mat4 projectionViewXForm = projection_xform * view_xform;




		//Drawing
		//glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh.element_count, GL_UNSIGNED_INT, (GLvoid*)(mesh.first_element_index * sizeof(int)), scene_->getInstancesByMeshId(instance.getMeshId()).size(), mesh.first_vertex_index);
		glDrawElementsBaseVertex(GL_TRIANGLES, mesh.element_count, GL_UNSIGNED_INT, (GLvoid*)(mesh.first_element_index * sizeof(int)), mesh.first_vertex_index);
	}
}