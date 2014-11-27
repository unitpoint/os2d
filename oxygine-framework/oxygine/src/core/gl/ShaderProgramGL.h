#pragma once
#include "../ShaderProgram.h"
#include "oxgl.h"
namespace oxygine
{
	class VertexDeclarationGL;
	class ShaderProgramGL: public ShaderProgram
	{
	public:
		OS_DECLARE_CLASSINFO(ShaderProgramGL);

		ShaderProgramGL();
		~ShaderProgramGL();

		void init(GLuint p, const VertexDeclarationGL*);

		unsigned int	getID() const;
		int				getUniformLocation(const char *id) const;

		const VertexDeclarationGL * getVdecl() const { return _vdecl; }

		static unsigned int createShader(unsigned int type, const char* data, const char *prepend, const char *append);
		static unsigned int createProgram(int vs, int fs, const VertexDeclarationGL *decl);

	private:

		GLuint _program;
		const VertexDeclarationGL * _vdecl;
	};


}
