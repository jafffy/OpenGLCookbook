#include "sample.hpp"

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <vector>

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

		GLfloat* transform_buffer_data = new float[16 * 4];
		{
			for (int i = 0; i < 4; ++i) {
				auto T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (float)i, 0.0f));
				auto R = glm::rotate(glm::mat4(1.0f), _globalTimer, glm::vec3(0.0f, 1.0f, 0.0f));
				auto S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

				auto M = T * R * S;

				memcpy(transform_buffer_data + 16 * i, glm::value_ptr(M), sizeof(float) * 16);
			}

			glGenBuffers(1, &_transformBufferObject);
			glBindBuffer(GL_TEXTURE_BUFFER, _transformBufferObject);
			glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * 16 * 4,
					transform_buffer_data, GL_STATIC_DRAW);

			glGenTextures(1, &_transformTBO);
			glBindTexture(GL_TEXTURE_BUFFER, _transformTBO);
			glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, _transformBufferObject);
		}
		if (transform_buffer_data) {
			delete[] transform_buffer_data;
			transform_buffer_data = nullptr;
		}

		glBindVertexArray(0);

		glUseProgram(_program);

		_VPID = glGetUniformLocation(_program, "VP");
		_MsID = glGetUniformLocation(_program, "Ms");

		glUseProgram(0);

		_globalTimer = 0.0f;

		return true;
	}

	virtual void destroyContents()
	{
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_vbo);
		glDeleteBuffers(1, &_transformBufferObject);

		glDeleteTextures(1, &_transformTBO);

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

		auto VP = P * V;

		glUseProgram(_program);
		glUniform1i(_MsID, 0);
		glUniformMatrix4fv(_VPID, 1, false, glm::value_ptr(VP));
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

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_BUFFER, _transformTBO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 4);

		glDisableVertexAttribArray(0);

		glUseProgram(0);
	}

private:
	GLuint _vao, _vbo;
	GLuint _transformBufferObject;
	GLuint _transformTBO;

	GLuint _program;
	GLuint _VPID;
	GLuint _MsID;

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
