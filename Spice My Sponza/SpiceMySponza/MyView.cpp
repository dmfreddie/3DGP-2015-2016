#include "MyView.hpp"
#include <SceneModel/SceneModel.hpp>
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>
#include <stdlib.h>

MyView::
MyView() : first_program_(0),
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

#pragma region Toggle functions

/*
A public function that switches the rendering mode between the 3 standard ones: rendering the vertex points,
rendering the wireframe view and rendering the faces as per normal.
*/
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

/*
A public toggle function that toggles whether one of the special rendering debug modes should be enabled.
*/
void MyView::
ToggleOutlineMode()
{
	outlineMode = !outlineMode;
}

/*
Clamp the length of the linesbetween 0 and 5 so they dont invert of becom to extreame and then add the clamped value thats
passed in to the length variable so it can be kept over the application lifetime. This new value then overwrites the stored 
value and only needs to be passed to the shader once for efficiency as its kept between rendering frames.
@param The length to add to the line
*/
void MyView::
AddToLength(float val_)
{
	if (normalLineLength + val_ > 0 && normalLineLength + val_ < 5)
		normalLineLength += val_;
}

/*
Toggle the intensity of the specular smudge factor if your not able to clearly see it at default levels
@param The intensity of the smudge
*/
void MyView::
AddToSpecSmudge(float val_)
{
	if (specularSmudge + val_ > 0)
		specularSmudge += val_;
}

/*
Enable the spikey view rendering mode that utilises the second shader program and sets the initial normal line length
This only needs to be set once because the uniform value will carry over to the required frames so they dont need to be
constantly passed on every frame thats rendered.
*/
void MyView::
EnableSpikey()
{
	spikey = !spikey;
}

#pragma endregion 

#pragma region Shader compilation functions

/*
As this code is repeated for each shader type with only a few variables changing it made sense to cut down on the amount
of repeate code and is called at the start of the shaders being compiled. The only things that change is the shader
file name, the type of shader that OpenGL needs to compile and the handle the shader is associated with.
@param The name of the shader file
@param The OpenGL shadr type to be compiled
@param The OpenGL handle to the shader program
*/
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

/*
This code block is repeated for each shader program to check if the shader has been successfully linked. If the shadrer
hasn't been linked successfully then it will output the linked error and log to the output window.
@param The OpenGL handle to the shader program
@return Whethher the shader has been successfully linked
*/
bool MyView::
CheckLinkStatus(GLuint shaderProgram)
{
	GLint link_status = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetProgramInfoLog(shaderProgram, string_length, NULL, log);
		std::cerr << log << std::endl;
		return false;
	}
	return true;
}

/*
The shader compilation code was spun off into its own function so that it could be called when I want to compile 
the shaders using a hot key during run-time. This helped in speeding up development time as the entire application
would not need to be closed before hand. It compiles the shaders into 2 shader programmes. The first 'first_program_'
is used to drawn sponza normally and some basic graphics debugging features such as wireframe and point rendering.
The second shader program 'spikey_program_' is used draw a debug version of the line drawing along a vertex positions' 
normal vector. If there is an error when the shaders are compiled, then the error(s) for each shader are output to the
console window and reference a specific line in a specific file.
*/
void MyView::CompileShaders()
{
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
	if(CheckLinkStatus(first_program_) && CheckLinkStatus(spikey_program_))
		std::cout << "Shaders Compiled!" << std::endl;

	Getuniforms();
}


void MyView::Getuniforms()
{
#pragma region
	/*
	Get the uniform locations of the uniform variables in the shader for each program where the varibale needs to be placed and bind it to a GLuint
	inside the unordered map. An unordered map was used because every time a new unifrom Gluint is added it doesn't need to re-order the map. This is
	also performed on start because the locations of the uniforms wont need to be changed per frame so it will speed up the rendering function as it
	doesn't need to perform unnessacary computation.
	*/
	uniforms["projection_view_model_xform"] = glGetUniformLocation(first_program_, "projection_view_model_xform");
	uniforms["model_xform"] = glGetUniformLocation(first_program_, "model_xform");
	uniforms["vertex_diffuse_colour"] = glGetUniformLocation(first_program_, "vertex_diffuse_colour");
	uniforms["vertex_ambient_colour"] = glGetUniformLocation(first_program_, "vertex_ambient_colour");
	uniforms["vertex_spec_colour"] = glGetUniformLocation(first_program_, "vertex_spec_colour");
	uniforms["vertex_shininess"] = glGetUniformLocation(first_program_, "vertex_shininess");
	uniforms["specular_smudge_factor"] = glGetUniformLocation(first_program_, "specular_smudge_factor");
	uniforms["is_vertex_shiney"] = glGetUniformLocation(first_program_, "is_vertex_shiney");
	uniforms["camera_position"] = glGetUniformLocation(first_program_, "camera_position");

	uniforms["global_ambient_light"] = glGetUniformLocation(first_program_, "global_ambient_light");

	uniforms["MAX_LIGHTS"] = glGetUniformLocation(first_program_, "MAX_LIGHTS");
	uniforms["outline"] = glGetUniformLocation(first_program_, "outline");
	uniforms["has_diff_tex"] = glGetUniformLocation(first_program_, "has_diff_tex");

	uniforms["spikey_projection_view_model_xform"] = glGetUniformLocation(spikey_program_, "projection_view_model_xform");
	uniforms["spikey_model_xform"] = glGetUniformLocation(spikey_program_, "model_xform");
	uniforms["spikey_normal_xform"] = glGetUniformLocation(spikey_program_, "normal_xform");
	uniforms["spikey_normal_line_length"] = glGetUniformLocation(spikey_program_, "normal_vector_length");

#pragma endregion // Get the uniform locations
}

#pragma endregion 

#pragma region Texture Loading
/*Load the texture and pass the texture into an unordered map using the texture files' name as the key.
The textures need to be in the same folder as the application executable
@param The name of the shader file
*/
void MyView::
LoadTexture(std::string textureName)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLuint texture = GLuint(0);
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
		textures[textureName] = texture;
	}
}

#pragma endregion

void MyView::ResetConsole()
{
	system("cls");
	//Application instructions
	std::cout << "ABOUT" << std::endl;
	std::cout << "Spice My Sponza - 3D Graphics Programming ICA1" << std::endl;
	std::cout << "P4011584 - Frederic Babord 2015 - 2016" << std::endl << std::endl;
	std::cout << "Submission date: 04th February 2016" << std::endl << std::endl;
	std::cout << "INSTRUCTIONS" << std::endl;
	std::cout << "Press F1 to enable camera animation." << std::endl;
	std::cout << "Press F2 to view the direction of the vertex normals." << std::endl;
	std::cout << "Press F3 to enable wireframe mode." << std::endl;
	std::cout << "Press F4 to change rendering mode (Fill, Line, Point)." << std::endl;
	std::cout << "Press F5 to recompile the shader." << std::endl << std::endl;
	std::cout << "Press Q to reduce the normal line length." << std::endl;
	std::cout << "Press E to increase the normal line length." << std::endl;
	std::cout << "Press Z to reduce the specular intensity smudge factor." << std::endl;
	std::cout << "Press C to increase the specular intensity smudge factor." << std::endl << std::endl;
	std::cout << "Press Esc to Close." << std::endl << std::endl;
	CompileShaders();
}

void MyView::
windowViewWillStart(std::shared_ptr<tygra::Window> window)
{
	assert(scene_ != nullptr);
	
	

	ResetConsole();
	
	/*
	Create and fill the vbos and vao in an interleaved manner from the scene data. Interleaving was chosen because
	the data would arrive to the gpu in a stream that would be in the order it would need for the shader and specific
	draw function to expect speeding up chache hits. The textures are also loaded on start so they can be assigned to 
	maps and used in rendering the specific mesh using its material data.
	*/

#pragma region
	SceneModel::GeometryBuilder builder;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> elements;
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

#pragma endregion //Load the mesh into buffers

#pragma region

	//Load the textures into a map of handles
	LoadTexture("diff0.png");
	LoadTexture("diff1.png");
	LoadTexture("spec1.png");
	LoadTexture("spec2.png");
	
	//Create the light vector so there will be memory already reserved that can just be overwritten if values have been changed. This has been done on 
	//start for effiences in the constant render loop function.
	for (unsigned int i = 0; i < scene_->getAllLights().size(); ++i)
	{
		Light light;
		light.position = scene_->getAllLights()[i].getPosition();
		light.range = scene_->getAllLights()[i].getRange();
		light.intensity = scene_->getAllLights()[i].getIntensity();
		lights.push_back(light);
	}

#pragma endregion // Textures and Lights



}

void MyView::
windowViewDidReset(std::shared_ptr<tygra::Window> window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void MyView::
windowViewDidStop(std::shared_ptr<tygra::Window> window)
{
	/*
	Delete the buffers in sponza and the shader programmes
	*/
	glDeleteProgram(first_program_);
	glDeleteProgram(spikey_program_);

	glDeleteBuffers(1, &vertex_vbo);
	glDeleteBuffers(1, &element_vbo);
	glDeleteVertexArrays(1, &vao);
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

	//Use the initial shader program to render sponza normally
	glUseProgram(first_program_);

	glm::mat4 projection_xform = glm::perspective(scene_->getCamera().getVerticalFieldOfViewInDegrees(), aspect_ratio, scene_->getCamera().getNearPlaneDistance(), scene_->getCamera().getFarPlaneDistance());
	glm::mat4 view_xform = glm::lookAt(scene_->getCamera().getPosition(), scene_->getCamera().getPosition() + scene_->getCamera().getDirection(), scene_->getUpDirection());

	glUniform3fv(uniforms["camera_position"], 1, glm::value_ptr(scene_->getCamera().getPosition()));
	

#pragma region Update sponzas lighting for this frame

	/*
	Get the light data from the scene and pass out the data into a uniform array of light structs in the vertex shader
	for each point light and the ambient light for the scene. The shader also needs to know how many lights are currently 
	active and so that value is also passed out into the shader.
	*/
	
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
	glUniform3fv(uniforms["global_ambient_light"], 1, glm::value_ptr(scene_->getAmbientLightIntensity()));

#pragma endregion 

	glBindVertexArray(vao);
	
#pragma region Set current rendering mode
	/*
	This swicthed the render mode based on what the current enumerated value is.
	*/
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
#pragma endregion 

#pragma region Draw call for rendering normal sponza

	float outLineInt = 0;
	glUniform1f(uniforms["outline"], outLineInt);

	glm::mat4 projection_view_mod_xform;
	glm::mat4 inverse_normal_xform;

	//Initialised to first element as there is no default constructor for a material
	SceneModel::Material material = scene_->getAllMaterials()[0];

	/*
	Populate the material uniform variables and the model uniform variables and then draw sponza normally
	*/
	for (const auto& instance : scene_->getAllInstances())
	{
		glm::mat4 model_xform = glm::mat4(instance.getTransformationMatrix());
		const MeshGL& mesh = meshes_[instance.getMeshId()];

		projection_view_mod_xform = projection_xform * view_xform * model_xform;
		inverse_normal_xform = view_xform * model_xform;

		material = scene_->getMaterialById(instance.getMaterialId());

		glUniformMatrix4fv(uniforms["projection_view_model_xform"], 1, GL_FALSE, glm::value_ptr(projection_view_mod_xform));
		glUniformMatrix4fv(uniforms["model_xform"], 1, GL_FALSE, glm::value_ptr(model_xform));
		glUniform3fv(uniforms["vertex_diffuse_colour"], 1, glm::value_ptr(material.getDiffuseColour()));
		glUniform3fv(uniforms["vertex_ambient_colour"], 1, glm::value_ptr(material.getAmbientColour()));
		glUniform3fv(uniforms["vertex_spec_colour"], 1, glm::value_ptr(material.getSpecularColour()));
		glUniform1f(uniforms["vertex_shininess"], material.getShininess());
		glUniform1f(uniforms["is_vertex_shiney"], (float)material.isShiny());
		glUniform1f(uniforms["specular_smudge_factor"], specularSmudge);

		if (material.getDiffuseTexture() != "")
		{
			glUniform1f(uniforms["has_diff_tex"], 1);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[material.getDiffuseTexture()]);
			glUniform1i(glGetUniformLocation(first_program_, "diffuse_texture"), 0);
		}
		else
			glUniform1f(uniforms["has_diff_tex"], 0);

		if (material.isShiny())
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textures[material.getSpecularTexture()]);
			glUniform1i(glGetUniformLocation(first_program_, "specular_texture"), 1);
		}
		
		glDrawElementsBaseVertex(GL_TRIANGLES, mesh.element_count, GL_UNSIGNED_INT, (GLvoid*)(mesh.first_element_index * sizeof(int)), mesh.first_vertex_index);

		
	}

#pragma endregion 

#pragma region Draw call for outline mode of normal sponza
	/*
	When the user presses the F3 key, they enable the wireframe mode, in this mode, sponza is drawn a second time but
	no lighting calculations are done and it switches the rendering mode from filling the triangles to only rendering
	lines between the vertex points. This is another useful method of debugging as it is simpler to see how the textures
	align with the meshs vertecies and whether they are in the approriate locations with no deformaties.
	*/
	if (outlineMode)
	{
		outLineInt = 1;
		glUniform1f(uniforms["outline"], outLineInt);

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
#pragma endregion 

#pragma region Draw call for Spike My Sponza
	/*
	When the user presses the F2 key, a second shader program is used which has been created with the addition of
	a geometary shader. The geometary shader takes in specific uniforms in order to draw lines along the vertex's 
	normal vector. The geomerty shader was used to create the toggelable lines because it was the cleanest method 
	that I found after reaserching how to do this without defaulting to an older version of OpenGL due to depreciated 
	functions. I have also found there is more control over how the lines can be drawn and how they are represented in 
	this manner with a simpler and more understanding workflow through the opengl pipeline. Switch shader programms so 
	a normal debug version of the shaders can be used for the next render call
	*/
	glUseProgram(spikey_program_);
	glUniform1f(uniforms["spikey_normal_line_length"], normalLineLength);

	if (spikey)
	{
		for (const auto& instance : scene_->getAllInstances())
		{
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
#pragma endregion 
}