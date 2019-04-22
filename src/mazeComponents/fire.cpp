
#include "../include/mazeComponents/fire.hpp"


#include <cmath>

bool Fire::init()
{
	//const char* textureFile = textures_path("fire.png");

	//if (!RenderManager::load_texture(textureFile, &m_texture, this)) return false;

	//float wr = m_texture.width * 0.5f;
    //float hr = m_texture.height * 0.5f;

	//TexturedVertex vertices[1];
	//vertices[0].position = { position.x, position.y, -0.02f };
    //vertices[0].texcoord = { 0.f, 0.f };
    m_win_time = -1;

    static const GLfloat screen_vertex_buffer[] = {
		-1.05f, -1.05f, 0.0f,
		1.05f, -1.05f, 0.0f,
		-1.05f,  1.05f, 0.0f,
		-1.05f,  1.05f, 0.0f,
		1.05f, -1.05f, 0.0f,
		1.05f,  1.05f, 0.0f,
	};

    gl_flush_errors();

	//uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };
	// Vertex Buffer creation
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertex_buffer), screen_vertex_buffer, GL_STATIC_DRAW);

	// // Index Buffer creation
    // glGenBuffers(1, &mesh.ibo);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);
	// Vertex Array (Container for Vertex + Index buffer)
    //glGenVertexArrays(1, &mesh.vao);
    if (gl_has_errors())
        return false;

    // Loading shaders
    if (!effect.load_from_file(shader_path("pointLight.vs.glsl"), shader_path("pointLight.fs.glsl")))
        return false;


	//if (!RenderManager::load_texture(textureFile, &m_texture, this)) return false;

	//if (!RenderManager::set_render_data(&m_texture, this)) return false;

    //set_position(position);

	// Setting initial values, scale is negative to make it face the opposite way
    //m_scale.x = 0.05f;
	//m_scale.y = 0.04f;
	//m_rotation = 0.f;

    //set_size();

	return true;
}

void Fire::destroy() {
	glDeleteBuffers(1, &mesh.vbo);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
    effect.release();
}

void Fire::draw(const mat3& projection)
{
	 // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glUseProgram(effect.program);

	// Getting uniform locations for glUniform* calls
    GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
    //GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
    //define uniform here
    ////GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
    GLuint screen_text_uloc = glGetUniformLocation(effect.program, "screen_texture");
    GLuint time_uloc = glGetUniformLocation(effect.program, "time");
    GLint light_pos = glGetUniformLocation(effect.program, "light_pos");
    GLint at_door = glGetUniformLocation(effect.program, "is_at_door");
    GLint origin_pos = glGetUniformLocation(effect.program, "origin_pos");
    GLint alive = glGetUniformLocation(effect.program, "alive");
    GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
    GLuint win_timer_uloc = glGetUniformLocation(effect.program, "win_timer");
    GLuint light_uloc = glGetUniformLocation(effect.program, "light_mode");
    //GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
    glEnableVertexAttribArray(in_position_loc);
    //glEnableVertexAttribArray(in_texcoord_loc);
    glUniform1i(screen_text_uloc, 0);


    float o_position[] = {px, py}; 
     //printf("%f", o_position[0]);
    glUniform2fv(origin_pos, 1, o_position);

    glUniform1f(alive, isAlive);

    float l_position[] = {tx/2.f + px, ty/2.f + py};
    glUniform2fv(light_pos, 1, l_position);
    glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
    glUniform1i(at_door, is_At_Door);
    //glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    //glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(vec3));
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    glUniform1f(win_timer_uloc, (m_win_time > 0) ? (float)((glfwGetTime() - m_win_time) * 80.0f) : -1);
    glUniform1i(light_uloc, isLightMode);
    

	// Setting vertices and indices
    //glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    ////glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	////GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
    //GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
    ////glEnableVertexAttribArray(in_position_loc);
    //glEnableVertexAttribArray(in_texcoord_loc);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, m_texture.id);

	// Setting uniform values to the currently bound program
    //glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
    //float color[] = { 1.f, 1.f, 1.f};
    //glUniform3fv(color_uloc, 1, color);
    //glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // magnifies texture to avoid it being blurry when scaled
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Draw
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	glDisableVertexAttribArray(0);
	
}


void Fire::originUpdate(float ox, float oy, float p_x, float p_y){
    tx = ox;
    ty = oy;
    px = p_x;
    py = p_y;
}

void Fire::set_player_dead(){
    isAlive = 0.f;
}

void Fire::reset_fire(){
    isAlive = 1.f;
    is_At_Door = 0.f;
}

void Fire::set_success(){
    is_At_Door = 1;
}

void Fire::set_light_level(int isLightOn){
    isLightMode = isLightOn;
}

void Fire::set_level_complete_time() {
	m_win_time = glfwGetTime();
}

void Fire::reset_player_win_time() {
	m_win_time = -1;
}