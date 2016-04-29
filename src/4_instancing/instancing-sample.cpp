#include "sample.hpp"

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <signal.h>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

class InstancingSample : public Sample
{
public:
	virtual bool initContents()
	{
		glClearColor(0.0f, 0.0f, 0.3f, 0.0f);

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		
		if (_vao == -1)
			return false;

		_program = LoadShaders("instancing.vert", "instancing.frag");
		assert(_program != -1);

		static const GLfloat g_vertex_buffer_data[] = {
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			 0.0f,  1.0f, 0.0f,
		};

		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		assert(_vbo != -1);

		glBindVertexArray(0);

		glUseProgram(_program);

		_MID = glGetUniformLocation(_program, "M");

		auto T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		auto R = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		auto S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		auto M = T * R * S;

		auto V = glm::lookAt(
				glm::vec3(4,3,3),
				glm::vec3(0,0,0),
				glm::vec3(0,1,0)
				);

		auto P = glm::perspective(45.0f, (float)windowWidth() / windowHeight(),
				0.1f, 100.0f);

		auto MVP = P * V * M;

		glUniformMatrix4fv(_MID, 1, false, glm::value_ptr(MVP));

		glUseProgram(0);

		_globalTimer = 0.0f;

		return true;
	}

	virtual void destroyContents()
	{
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_vbo);

		glDeleteProgram(_program);
	}

	virtual void update(float dt)
	{
		auto T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		auto R = glm::rotate(glm::mat4(1.0f), _globalTimer, glm::vec3(0.0f, 1.0f, 0.0f));
		auto S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		auto M = T * R * S;

		auto V = glm::lookAt(
				glm::vec3(4,3,3),
				glm::vec3(0,0,0),
				glm::vec3(0,1,0)
				);

		auto P = glm::perspective(45.0f, (float)windowWidth() / windowHeight(),
				0.1f, 100.0f);

		auto MVP = P * V * M;

		glUseProgram(_program);
		glUniformMatrix4fv(_MID, 1, false, glm::value_ptr(MVP));
		glUseProgram(0);

		_globalTimer += dt;
	}

	virtual void render()
	{
		glViewport(0, 0, windowWidth(), windowHeight());
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(_program);

		glBindVertexArray(_vao);

		glEnableVertexAttribArray(0);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(0);

		glUseProgram(0);
	}

private:
	GLuint _vao, _vbo;

	GLuint _program;
	GLuint _MID;

	float _globalTimer;
};

Sample* instancingSample = nullptr;

void funCatch(int signal)
{
	if (instancingSample == nullptr)
		return;

	instancingSample->destroy();

	if (instancingSample) {
		delete instancingSample;
		instancingSample = nullptr;
	}

	printf("interrupted!\n");

	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
	if (signal(SIGINT, funCatch) == SIG_ERR) {
		fprintf(stderr, "cannot catch signal\n");
	}

	instancingSample = new InstancingSample();
	instancingSample->init();

	instancingSample->run();

	instancingSample->destroy();

	if (instancingSample) {
		delete instancingSample;
		instancingSample = nullptr;
	}

	return 0;
}
