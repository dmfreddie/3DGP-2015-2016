#include "MyView.hpp"
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include "FractionalBrownianMotion.h"
#include "Frustum.h"


MyView::
MyView()
{
}

MyView::
~MyView()
{
}

void MyView::
setScene(std::shared_ptr<const SceneModel::Context> scene)
{
    scene_ = scene;
}

void MyView::
toggleShading()
{
    shade_normals_ = !shade_normals_;
}



void MyView::
windowViewWillStart(std::shared_ptr<tygra::Window> window)
{
    assert(scene_ != nullptr);

    GLint compile_status = 0;
    GLint link_status = 0;

    
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertex_shader_string = tygra::stringFromFile("terrain_vs.glsl");
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
    std::string fragment_shader_string = tygra::stringFromFile("terrain_fs.glsl");
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

    terrain_sp_ = glCreateProgram();
    glAttachShader(terrain_sp_, vertex_shader);
    glDeleteShader(vertex_shader);
    glAttachShader(terrain_sp_, fragment_shader);
    glDeleteShader(fragment_shader);
    glLinkProgram(terrain_sp_);

    glGetProgramiv(terrain_sp_, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length]= "";
        glGetProgramInfoLog(terrain_sp_, string_length, NULL, log);
        std::cerr << log << std::endl;
    }


    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    vertex_shader_string = tygra::stringFromFile("shapes_vs.glsl");
    vertex_shader_code = vertex_shader_string.c_str();
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
    fragment_shader_string = tygra::stringFromFile("shapes_fs.glsl");
    fragment_shader_code = fragment_shader_string.c_str();
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

    shapes_sp_ = glCreateProgram();
    glAttachShader(shapes_sp_, vertex_shader);
    glDeleteShader(vertex_shader);
    glAttachShader(shapes_sp_, fragment_shader);
    glDeleteShader(fragment_shader);
    glLinkProgram(shapes_sp_);

    glGetProgramiv(shapes_sp_, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length] = "";
        glGetProgramInfoLog(terrain_sp_, string_length, NULL, log);
        std::cerr << log << std::endl;
    }


    glGenVertexArrays(1, &cube_vao_);
    glBindVertexArray(cube_vao_);
    glGenBuffers(1, &cube_vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_);
    float cube_vertices[] = {
        -0.5f, 0.f, -0.5f,   0.5f, 0.f, -0.5f,   0.5f, 0.f,  0.5f,
        -0.5f, 0.f, -0.5f,   0.5f, 0.f,  0.5f,  -0.5f, 0.f,  0.5f,
        -0.5f, 0.f,  0.5f,   0.5f, 0.f,  0.5f,   0.5f, 1.f,  0.5f,
        -0.5f, 0.f,  0.5f,   0.5f, 1.f,  0.5f,  -0.5f, 1.f,  0.5f,
         0.5f, 0.f,  0.5f,   0.5f, 0.f, -0.5f,   0.5f, 1.f, -0.5f,
         0.5f, 0.f,  0.5f,   0.5f, 1.f, -0.5f,   0.5f, 1.f,  0.5f,
         0.5f, 0.f, -0.5f,  -0.5f, 0.f, -0.5f,  -0.5f, 1.f, -0.5f,
         0.5f, 0.f, -0.5f,  -0.5f, 1.f, -0.5f,   0.5f, 1.f, -0.5f,
        -0.5f, 0.f, -0.5f,  -0.5f, 0.f,  0.5f,  -0.5f, 1.f,  0.5f,
        -0.5f, 0.f, -0.5f,  -0.5f, 1.f,  0.5f,  -0.5f, 1.f, -0.5f,
        -0.5f, 1.f,  0.5f,   0.5f, 1.f,  0.5f,   0.5f, 1.f, -0.5f,
        -0.5f, 1.f,  0.5f,   0.5f, 1.f, -0.5f,  -0.5f, 1.f, -0.5f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices),
                 cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(kVertexPosition);
    glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE,
        sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // X and -Z are on the ground, Y is up
    const float sizeX = scene_->getTerrainSizeX();
    const float sizeY = scene_->getTerrainSizeY();
    const float sizeZ = scene_->getTerrainSizeZ();


    const auto& bezier_patches = scene_->getTerrainPatches();
    const size_t number_of_patches = bezier_patches.size();
    // below is an example of accessing a control point from the second patch
    
	std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>(Mesh::GenerateIndexedGrid(256, 256, (const int)sizeX, (const int)sizeZ));
	terrain_mesh_.element_count = newMesh->elements.size();

	ApplyBezierSurface(bezier_patches, number_of_patches, *newMesh);

    tygra::Image displace_image = tygra::imageFromPNG(scene_->getTerrainDisplacementMapName());
	ApplyHeightImage(displace_image, 180.0f, *newMesh);

	ApplyNoise(12, *newMesh, 12, SimplexNoise(0.5385f,10.98f,2, 0.25f), 10);

	glGenBuffers(1, &vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		newMesh->verticies.size() * sizeof(Vertex), // size of data in bytes
		newMesh->verticies.data(), // pointer to the data
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &element_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, element_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		newMesh->elements.size() * sizeof(unsigned int), // size of data in bytes
		newMesh->elements.data(), // pointer to the data
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
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), TGL_BUFFER_OFFSET_OF(Vertex, uv));

	// make nothing active (deactivate vbo and vao)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	tree = new FreddieBabord::Quadtree(0, 0, sizeX, sizeZ, 0, 5);
	for (const auto& pos : scene_->getAllShapePositions())
	{
		auto position = glm::vec3(pos.x, pos.y, 64);
		FreddieBabord::Object* objectToAdd = new FreddieBabord::Object(position);
		tree->AddObject(objectToAdd);
	}
}

void MyView::
ApplyBezierSurface(std::vector<Array2d<glm::vec3, 4, 4>> bezier_patches, size_t number_of_patches, Mesh& mesh)
{
	//Apply Bezier Surfaces
	std::vector<std::vector<std::vector<glm::vec3>>> patchesCurves;
	std::vector<std::vector<glm::vec3>> cps;

	for (size_t p = 0; p < number_of_patches; ++p)
	{
		for (int i = 0; i < (int)bezier_patches[p].sizeX(); ++i)
		{
			std::vector<glm::vec3> pathcps;
			for (int j = 0; j < (int)bezier_patches[p].sizeY(); ++j)
			{
				pathcps.push_back(bezier_patches[p].at(i, j));
			}
			cps.push_back(pathcps);
		}
		patchesCurves.push_back(cps);
		cps.clear();
	}

	mesh.ApplyBezier(patchesCurves);
	mesh.RecalculateNormals();
}



void MyView::
ApplyHeightImage(tygra::Image image, float outputScale, Mesh& mesh)
{

	for (int i = 0; i < mesh.GetVertexCount(); ++i)
	{
		auto& p = mesh.verticies[i].position;
		auto& n = mesh.verticies[i].normal;
		auto& t = mesh.verticies[i].uv;
		const int x = int(t.x * image.width());
		const int y = int(t.y * image.height());

		uint8_t height = *(uint8_t*)image(x, y);
		p += n * outputScale * (height / 255.f);
	}

#pragma region // Atempted image interpolation

	//float x_ratio = //((float)(image.width() )) / mesh.Width();
	//	mesh.Width() / image.width();
	//float y_ratio = //((float)(image.height() )) / mesh.Height();
	//	mesh.Height() / image.height();
	//float x_diff, y_diff, ya, yb;
	//uint8_t A, B, C, D;


	//for (int i = 0; i < mesh.Height(); i++) {
	//	for (int j = 0; j<mesh.Width(); j++) {
	//		
	//		int x = (int)(x_ratio * j);
	//		int y = (int)(y_ratio * i);
	//		x_diff = (x_ratio * j) - x;
	//		y_diff = (y_ratio * i) - y;
	//		int index = y*mesh.Width() + x;


	//		auto& p = mesh.verticies[index].position;
	//		auto& n = mesh.verticies[index].normal;
	//		auto& ta = mesh.verticies[index].uv;
	//		auto& tb = mesh.verticies[index + 1].uv;
	//		auto& tc = mesh.verticies[index + mesh.Width()].uv;
	//		auto& td = mesh.verticies[index + mesh.Height() + 1].uv;

	//		A = *(uint8_t*)image(ta.x, ta.y);
	//		B = *(uint8_t*)image(tb.x, tb.y);
	//		C = *(uint8_t*)image(tc.x, tc.y);
	//		D = *(uint8_t*)image(td.x, td.y);

	//		

	//		// Y = A(1-w)(1-h) + B(w)(1-h) + C(h)(1-w) + Dwh
	//		uint8_t gray = (uint8_t)(
	//				A*(1 - x_diff)*(1 - y_diff) + 
	//				B*(x_diff)*(1 - y_diff) +
	//				C*(y_diff)*(1 - x_diff) + 
	//				D*(x_diff*y_diff)
	//			);
	//		
	//		p += n * outputScale * (gray / 255.f);

	//	}
	//}

#pragma endregion

	mesh.RecalculateNormals();

}

void MyView::
windowViewDidReset(std::shared_ptr<tygra::Window> window,
                   int width,
                   int height)
{
    glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}

void MyView::
windowViewDidStop(std::shared_ptr<tygra::Window> window)
{
    glDeleteProgram(terrain_sp_);
    glDeleteProgram(shapes_sp_);

	glDeleteBuffers(1, &vertex_vbo);
	glDeleteBuffers(1, &element_vbo);
	glDeleteVertexArrays(1, &vao);
}

void MyView::
windowViewRender(std::shared_ptr<tygra::Window> window)
{
    assert(scene_ != nullptr);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const float aspect_ratio = viewport[2] / (float)viewport[3];

    const auto& camera = scene_->getCamera();
    glm::mat4 projection_xform = glm::perspective(camera.getVerticalFieldOfViewInDegrees(),
                                                  aspect_ratio,
                                                  camera.getNearPlaneDistance(),
                                                  camera.getFarPlaneDistance());
    glm::vec3 camera_pos = camera.getPosition();
    glm::vec3 camera_at = camera.getPosition() + camera.getDirection();
    glm::vec3 world_up{ 0, 1, 0 };
    glm::mat4 view_xform = glm::lookAt(camera_pos, camera_at, world_up);

	FreddieBabord::Frustum frustum;
	frustum.setCamInternals(camera.getVerticalFieldOfViewInDegrees(), aspect_ratio,
		camera.getNearPlaneDistance(), camera.getFarPlaneDistance());
	frustum.setCamDef(camera_pos, -camera_at, world_up);

    /* TODO: you are free to modify any of the drawing code below */


	glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, shade_normals_ ? GL_FILL : GL_LINE);

    glUseProgram(terrain_sp_);

    GLuint shading_id = glGetUniformLocation(terrain_sp_, "use_normal");
    glUniform1i(shading_id, shade_normals_);

    glm::mat4 world_xform = glm::mat4(1);
    glm::mat4 view_world_xform = view_xform * world_xform;

    GLuint projection_xform_id = glGetUniformLocation(terrain_sp_,
                                                      "projection_xform");
    glUniformMatrix4fv(projection_xform_id, 1, GL_FALSE,
                       glm::value_ptr(projection_xform));

    GLuint view_world_xform_id = glGetUniformLocation(terrain_sp_,
                                                      "view_world_xform");
    glUniformMatrix4fv(view_world_xform_id, 1, GL_FALSE,
                       glm::value_ptr(view_world_xform));

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, terrain_mesh_.element_count, GL_UNSIGNED_INT, 0);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUseProgram(shapes_sp_);

    projection_xform_id = glGetUniformLocation(shapes_sp_,
                                               "projection_xform");
    glUniformMatrix4fv(projection_xform_id, 1, GL_FALSE,
                       glm::value_ptr(projection_xform));

    glBindVertexArray(cube_vao_);

	for (const auto& pos : scene_->getAllShapePositions())
	{

		if (frustum.pointInFrustum(glm::vec3(pos.x, 64, -pos.y)) != FreddieBabord::Frustum::Outside)
		{

			world_xform = glm::translate(glm::mat4(1), glm::vec3(pos.x, 64, -pos.y));
			view_world_xform = view_xform * world_xform;

			view_world_xform_id = glGetUniformLocation(shapes_sp_, "view_world_xform");

			glUniformMatrix4fv(view_world_xform_id, 1, GL_FALSE, glm::value_ptr(view_world_xform));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

#pragma region // Attempted frustum culling

	//auto& objects = tree->GetObjectsAt(1, 1);

	//for (int x = 0; x < scene_->getTerrainSizeX(); ++x)
	//{
	//	for (int y = 0; y < scene_->getTerrainSizeX(); ++y)
	//	{
	//		FreddieBabord::AABB aabb = FreddieBabord::AABB(glm::vec3(1,1,1), glm::vec3(x, 0, -y));
	//		if (frustum.boxInFrustum(aabb))
	//		{
	//			auto& objects = tree->GetObjectsAt(x, -y);
	//			for (const auto& pos : objects)
	//			{
	//				world_xform = glm::translate(glm::mat4(1), glm::vec3(pos->x, 64, -pos->y));
	//				view_world_xform = view_xform * world_xform;

	//				view_world_xform_id = glGetUniformLocation(shapes_sp_, "view_world_xform");

	//				glUniformMatrix4fv(view_world_xform_id, 1, GL_FALSE, glm::value_ptr(view_world_xform));

	//				glDrawArrays(GL_TRIANGLES, 0, 36);

	//			}
	//		}
	//	}
	//}

#pragma endregion
}

