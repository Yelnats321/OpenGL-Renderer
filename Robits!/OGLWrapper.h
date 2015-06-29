#pragma once
#include <GL/glew.h>

class ObjFile;
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
class OGLWrapper{
	GLuint data;
public:
	OGLWrapper():data(0){}
	OGLWrapper(const OGLWrapper&) = delete;
	OGLWrapper& operator=(const OGLWrapper&) = delete;
	OGLWrapper(OGLWrapper && rhs){
		std::swap(data, rhs.data);
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
using Renderbuffer = OGLWrapper<decltype(&glGenRenderbuffers), &glGenRenderbuffers,
	decltype(&glDeleteRenderbuffers), &glDeleteRenderbuffers>;
using Framebuffer = OGLWrapper < decltype(&glGenFramebuffers), &glGenFramebuffers,
	decltype(&glDeleteFramebuffers), &glDeleteFramebuffers >;

class Program{
	GLuint data;
public:
	Program():data(0){}
	Program(Program &&);
	Program(const Program&) = delete;
	Program& operator=(const Program&) = delete;
	Program& operator=(Program&& rhs);
	void gen(string v, string f, string g = "");
	~Program();
	inline operator GLuint() const{ return data; }
};

struct Texture{
	GLuint data;
public:
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture &) = delete;

	~Texture();
	Texture():data(0){}
	Texture(Texture &&);
	Texture& operator=(Texture && rhs);
	void gen();
	void gen(string, bool = false, bool = true);
	inline operator GLuint() const{ return data; }
};

const ObjFile * loadFile(string);
}
