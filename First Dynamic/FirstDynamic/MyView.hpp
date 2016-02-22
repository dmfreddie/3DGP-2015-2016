#pragma once

#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <tsl/water.hpp>
#include <glm/glm.hpp>
#include <chrono>
#include <map>

class MyView : public tygra::WindowViewDelegate
{
public:

    MyView();

    ~MyView();

private:

    void
    windowViewWillStart(std::shared_ptr<tygra::Window> window) override;

    void
    windowViewDidReset(std::shared_ptr<tygra::Window> window,
                       int width,
                       int height) override;

    void
    windowViewDidStop(std::shared_ptr<tygra::Window> window) override;

    void
    windowViewRender(std::shared_ptr<tygra::Window> window) override;

private:

    // Water simulation object
    std::unique_ptr<tsl::Water> waterSim_{ nullptr };

    // Shader program used for draw the water patches
    GLuint water_sp_{ 0 };

    // VBOs and VAO for a GL mesh
    struct MeshGL
    {
        GLuint position_vbo{ 0 };
		GLuint normal_vbo{ 0 };
		GLuint tex_coord_vbo{ 0 };
        GLuint element_vbo{ 0 };
        GLuint vao{ 0 };
        int element_count{ 0 };
    };
    MeshGL waterMesh_;

    // Constants to use for the vertex attrib indices
    enum
    {
        kVertexPosition,
        kVertexNormal,
        kVertexTexcoord
    };

	GLint done = 0;
	bool reset = true;
	GLuint query;
	GLuint64 elapsed_time;

};
