#include "MyView.hpp"
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

MyView::
MyView()
{
    // Create water simulation - these are good arguments, don't modify them
    waterSim_ = std::make_unique<tsl::Water>(1.f / 60, 64.f, 128, 0.25f, 30.f, 15.f);
}

MyView::
~MyView()
{
}

void MyView::
windowViewWillStart(std::shared_ptr<tygra::Window> window)
{
    /*
     *  Compile shaders and create a shader program
     */
    GLint compile_status = 0;
    
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertex_shader_string = tygra::stringFromFile("water_vs.glsl");
    const char *vertex_shader_code = vertex_shader_string.c_str();
    glShaderSource(vertex_shader, 1,
                   (const GLchar **) &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length]= "";
        glGetShaderInfoLog(vertex_shader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }
 
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragment_shader_string = tygra::stringFromFile("water_fs.glsl");
    const char *fragment_shader_code = fragment_shader_string.c_str();
    glShaderSource(fragment_shader, 1,
                   (const GLchar **) &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length]= "";
        glGetShaderInfoLog(fragment_shader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }

    water_sp_ = glCreateProgram();
    glAttachShader(water_sp_, vertex_shader);
    glBindAttribLocation(water_sp_, kVertexPosition, "vertex_position");
    glDeleteShader(vertex_shader);
    glAttachShader(water_sp_, fragment_shader);
    glBindFragDataLocation(water_sp_, 0, "fragment_colour");
    glDeleteShader(fragment_shader);
    glLinkProgram(water_sp_);

    GLint link_status = 0;
    glGetProgramiv(water_sp_, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length]= "";
        glGetProgramInfoLog(water_sp_, string_length, NULL, log);
        std::cerr << log << std::endl;
    }


    /*
     *  TODO: Create vertex buffers and a vertex array object for the water mesh
     */
    std::vector<int> elements(waterSim_->IndexCount());
    waterSim_->GetIndices(elements.data());
	
    glGenBuffers(1, &waterMesh_.position_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, waterMesh_.position_vbo);
	glBufferData(GL_ARRAY_BUFFER, waterSim_->VertexCount() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &waterMesh_.normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, waterMesh_.normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, waterSim_->VertexCount() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &waterMesh_.tex_coord_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, waterMesh_.tex_coord_vbo);
	glBufferData(GL_ARRAY_BUFFER, waterSim_->VertexCount() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &waterMesh_.element_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterMesh_.element_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        elements.size() * sizeof(unsigned int),
        elements.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    waterMesh_.element_count = elements.size();

    glGenVertexArrays(1, &waterMesh_.vao);
    glBindVertexArray(waterMesh_.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterMesh_.element_vbo);
	
    glBindBuffer(GL_ARRAY_BUFFER, waterMesh_.position_vbo);
	glEnableVertexAttribArray(kVertexPosition);
	glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, waterMesh_.normal_vbo);
	glEnableVertexAttribArray(kVertexNormal);
	glVertexAttribPointer(kVertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), TGL_BUFFER_OFFSET(1));

	glBindBuffer(GL_ARRAY_BUFFER, waterMesh_.tex_coord_vbo);
	glEnableVertexAttribArray(kVertexTexcoord);
	glVertexAttribPointer(kVertexTexcoord, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), TGL_BUFFER_OFFSET(2));

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), elements.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
    glDeleteProgram(water_sp_);

    glDeleteBuffers(1, &waterMesh_.position_vbo);
	glDeleteBuffers(1, &waterMesh_.normal_vbo);
    glDeleteBuffers(1, &waterMesh_.element_vbo);
    glDeleteVertexArrays(1, &waterMesh_.vao);
}

void MyView::
windowViewRender(std::shared_ptr<tygra::Window> window)
{

	if (reset)
	{
		glGenQueries(1, &query);
		glBeginQuery(GL_TIME_ELAPSED, query);
	}

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const float aspect_ratio = viewport[2] / (float)viewport[3];

    glm::mat4 projection_xform = glm::perspective(50.f, aspect_ratio, 0.1f, 1000.f);
    glm::vec3 camera_pos{ 16, -50, 20 };
    glm::vec3 camera_at{ 0, 0, 0 };
    glm::vec3 world_up{ 0, 0, 1 };
    glm::mat4 view_xform = glm::lookAt(camera_pos, camera_at, world_up);

    glClearColor(0.f, 0.f, 0.25f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glUseProgram(water_sp_);


    static std::vector<glm::vec3> positions(waterSim_->VertexCount());
    static std::vector<glm::vec3> normals(waterSim_->VertexCount());
	static std::vector<glm::vec2> normals(waterSim_->VertexCount());
    waterSim_->Advance(1, (float*)positions.data(), sizeof(glm::vec3), (float*)normals.data(), sizeof(glm::vec3));

	glBindBuffer(GL_ARRAY_BUFFER, waterMesh_.position_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, waterSim_->VertexCount() * sizeof(glm::vec3), positions.data());
	glBindBuffer(GL_ARRAY_BUFFER, waterMesh_.normal_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, waterSim_->VertexCount() * sizeof(glm::vec3), normals.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glm::mat4 world_xform{ 1.f };


	GLuint xform_id = glGetUniformLocation(water_sp_,	"projection_view_world_xform");
	glm::mat4 projection_view_world_xform;

	for (int y = -5; y <= 5; ++y)
	{
		for (int x = -5; x <= 5; ++x)
		{
			world_xform = glm::mat4{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				x * 64, y*64, 0, 1 };
			
			projection_view_world_xform = projection_xform * view_xform * world_xform;

			
			glUniformMatrix4fv(xform_id, 1, GL_FALSE, glm::value_ptr(projection_view_world_xform));

			glBindVertexArray(waterMesh_.vao);
			glDrawElements(GL_TRIANGLES, waterMesh_.element_count, GL_UNSIGNED_INT, 0);
		}
	}
	if (reset)
	{
		glEndQuery(GL_TIME_ELAPSED);
		reset = false;
	}

	if (!done)
		glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &done);
	else
	{
		// get the query result
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsed_time);
		printf("Time Elapsed: %f ms\n", elapsed_time / 1000000.0);
		elapsed_time = 0;
		done = 0;
		reset = true;
	}


}
