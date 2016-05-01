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

class FBOSample : public Sample
{
public:
	virtual bool initContents();
	virtual void destroyContents();
	virtual void update(float dt);
	virtual void render();

private:
	GLuint _contentVAO, _contentVBO;
	GLuint _contentTransformBO;
	GLuint _contentTransformTBO;

	GLuint _contentProgram;
	GLuint _contentVPID;
	GLuint _contentMsID;

	float _globalTimer;

	GLuint _fboVAO, _fboVBO;
	GLuint _fboFBO, _fboRenderedTBO;

	GLuint _fboRenderedTBOID;

	GLuint _fboProgram;
};

Sample* sample = nullptr;

void interruptHandler(int signal)
{
	if (sample == nullptr)
		return;

	sample->destroy();

	if (sample) {
		delete sample;
		sample = nullptr;
	}

	printf("interrupted!\n");

	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
	if (signal(SIGINT, interruptHandler) == SIG_ERR) {
		fprintf(stderr, "cannot catch signal\n");
	}

	sample = new FBOSample();
	sample->init();

	sample->run();

	sample->destroy();

	if (sample) {
		delete sample;
		sample = nullptr;
	}

	return 0;
}

bool FBOSample::initContents()
{
	glClearColor(0.0f, 0.0f, 0.3f, 0.0f);

	// Init Content
	glGenVertexArrays(1, &_contentVAO);
	assert(_contentVAO != -1);

	glBindVertexArray(_contentVAO);
	{
		_contentProgram = LoadShaders("content.vert", "content.frag");
		assert(_contentProgram != -1);

		_contentVPID = glGetUniformLocation(_contentProgram, "VP");
		_contentMsID = glGetUniformLocation(_contentProgram, "Ms");

		static const GLfloat g_vertex_buffer_data[] = {
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			 0.0f,  1.0f, 0.0f,
		};

		glGenBuffers(1, &_contentVBO);
		assert(_contentVBO != -1);

		glBindBuffer(GL_ARRAY_BUFFER, _contentVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		GLfloat* transform_buffer_data = new float[16 * 4];
		{
			for (int i = 0; i < 4; ++i) {
				auto T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (float)i, 0.0f));
				auto R = glm::rotate(glm::mat4(1.0f), _globalTimer, glm::vec3(0.0f, 1.0f, 0.0f));
				auto S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

				auto M = T * R * S;

				memcpy(transform_buffer_data + 16 * i, glm::value_ptr(M), sizeof(float) * 16);
			}

			glGenBuffers(1, &_contentTransformBO);
			glBindBuffer(GL_TEXTURE_BUFFER, _contentTransformBO);
			glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * 16 * 4,
					transform_buffer_data, GL_STATIC_DRAW);

			glGenTextures(1, &_contentTransformTBO);
			glBindTexture(GL_TEXTURE_BUFFER, _contentTransformTBO);
			glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, _contentTransformBO);
		}
		if (transform_buffer_data) {
			delete[] transform_buffer_data;
			transform_buffer_data = nullptr;
		}
	}
	glBindVertexArray(0);

	glGenVertexArrays(1, &_fboVAO);
	glBindVertexArray(_fboVAO);
	{
		glGenFramebuffers(1, &_fboFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, _fboFBO);

		glGenTextures(1, &_fboRenderedTBO);
		glBindTexture(GL_TEXTURE_2D, _fboRenderedTBO);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth(), windowHeight(), 
			0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _fboRenderedTBO, 0);

		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;

		_fboProgram = LoadShaders("fbo.vert", "fbo.frag");
		assert(_fboProgram != -1);

		_fboRenderedTBOID = glGetUniformLocation(_fboProgram, "renderedTextureSampler");
		assert(_fboRenderedTBOID != -1);

		glUseProgram(_fboProgram);
		glUniform1i(_fboRenderedTBOID, 0);
		glUseProgram(0);

		float vertices[] = 
		{
			-1.0f,  1.0f,	0.0f, 1.0f,
			-1.0f, -1.0f,	0.0f, 0.0f,
			 1.0f, -1.0f,	1.0f, 0.0f,
			 1.0f,  1.0f,	1.0f, 1.0f,
		};

		glGenBuffers(1, &_fboVBO);
		glBindBuffer(GL_ARRAY_BUFFER, _fboVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
			vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(float) * 4, (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	}
	glBindVertexArray(0);

	_globalTimer = 0.0f;

	return true;
}

void FBOSample::destroyContents()
{
	glDeleteVertexArrays(1, &_fboVAO);
	glDeleteBuffers(1, &_fboVBO);

	glDeleteProgram(_fboProgram);

	glDeleteVertexArrays(1, &_contentVAO);
	glDeleteBuffers(1, &_contentVBO);
	glDeleteBuffers(1, &_contentTransformBO);

	glDeleteTextures(1, &_contentTransformTBO);

	glDeleteProgram(_contentProgram);
}

void FBOSample::update(float dt)
{
	glBindVertexArray(_contentVAO);
	glBindBuffer(GL_TEXTURE_BUFFER, _contentTransformBO);

	float* pointer = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, sizeof(float) * 16 * 4, GL_MAP_WRITE_BIT);
	assert(pointer);

	for (int i = 0; i < 4; ++i) {
		auto T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (float)i, 0.0f));
		auto R = glm::rotate(glm::mat4(1.0f), _globalTimer, glm::vec3(0.0f, 1.0f, 0.0f));
		auto S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		auto M = T * R * S;

		memcpy(pointer + 16 * i, glm::value_ptr(M), sizeof(float) * 16);
	}

	glUnmapBuffer(GL_TEXTURE_BUFFER);

	auto V = glm::lookAt(
			glm::vec3(3,4,10),
			glm::vec3(0,0,0),
			glm::vec3(0,1,0)
			);

	auto P = glm::perspective(45.0f, (float)windowWidth() / windowHeight(),
			0.1f, 100.0f);

	auto VP = P * V;

	glUseProgram(_contentProgram);
	glUniform1i(_contentMsID, 0);
	glUniformMatrix4fv(_contentVPID, 1, false, glm::value_ptr(VP));
	glUseProgram(0);

	_globalTimer += dt;
}

void FBOSample::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fboFBO);
	glViewport(0, 0, windowWidth(), windowHeight());
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(_contentProgram);

	glBindVertexArray(_contentVAO);
	{
		glEnableVertexAttribArray(0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_BUFFER, _contentTransformTBO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 4);

		glDisable(GL_BLEND);

		glDisableVertexAttribArray(0);
	}

	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth(), windowHeight());
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(_fboProgram);
	{
		glBindVertexArray(_fboVAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _fboRenderedTBO);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glBindVertexArray(0);
	}
	glUseProgram(0);
}


