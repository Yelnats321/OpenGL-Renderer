#pragma once
#include <GL/glew.h>

namespace gl{
/*
using TBuilder = decltype(&glGenBuffers);
using TFreer = decltype(&glDeleteBuffers);

template<std::size_t N, TBuilder TCons, TFreer TDes>
struct OGLWrapper{
	std::array<GLuint, N> data;
	OGLWrapper():data{}{}
	void gen(){
		(*TCons)(N, &data[0]);
	}
	~OGLWrapper(){
		(*TDes)(N, &data[0]);
	}
};*/

template<class TB, TB TCons, class TF, TF TDes>
struct OGLWrapper{
	GLuint data;
	OGLWrapper():data(0){}
	OGLWrapper(GLuint d):data(d){}
	OGLWrapper(const OGLWrapper&) = delete;
	OGLWrapper& operator=(const OGLWrapper&) = delete;
	OGLWrapper& operator=(GLuint rhs){
		data = rhs;
		return *this;
	}
	OGLWrapper& operator=(OGLWrapper&& rhs){
		std::swap(data, rhs.data);
		return *this;
	}
	void gen(){
		(*TCons)(1, &data);
	}
	~OGLWrapper(){
		(*TDes)(1, &data);
	}
	inline operator GLuint() const{ return data; }
};

using VAO = OGLWrapper<decltype(&glGenVertexArrays), &glGenVertexArrays,
	decltype(&glDeleteVertexArrays), &glDeleteVertexArrays>;
using Buffer = OGLWrapper<decltype(&glGenBuffers), &glGenBuffers,
	decltype(&glDeleteBuffers), &glDeleteBuffers>;
using Texture = OGLWrapper<decltype(&glGenTextures), &glGenTextures,
	decltype(&glDeleteTextures), &glDeleteTextures>;
using Renderbuffer = OGLWrapper<decltype(&glGenRenderbuffers), &glGenRenderbuffers,
	decltype(&glDeleteRenderbuffers), &glDeleteRenderbuffers>;
using Framebuffer = OGLWrapper < decltype(&glGenFramebuffers), &glGenFramebuffers,
	decltype(&glDeleteFramebuffers), &glDeleteFramebuffers >;
}