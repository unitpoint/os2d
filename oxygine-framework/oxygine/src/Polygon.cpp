#include "Polygon.h"
#include "Serialize.h"
#include "RenderState.h"
#include "core/VertexDeclaration.h"
#include <sstream>

namespace oxygine
{
	Polygon::Polygon() :_verticesSize(0), _verticesData(0), _own(false), _vdecl(0)
	{

	}

	Polygon::~Polygon()
	{
		if (_own)
			delete[] _verticesData;
	}

	void Polygon::copyFrom(const Polygon &src, cloneOptions opt)
	{
		Sprite::copyFrom(src, opt);
	}

	void Polygon::serialize(serializedata* data)	
	{
		Sprite::serialize(data);

		pugi::xml_node node = data->node;
		node.set_name("Polygon");
	}

	void Polygon::deserialize(const deserializedata* data)
	{
		Sprite::deserialize(data);
	}

	std::string Polygon::dump(const dumpOptions &options) const
	{
		stringstream stream;
		stream << "{Polygon}\n";

		if (_verticesSize)
		{
			stream << "vertices=" << _verticesSize/_vdecl->size << " ";
		}

		stream << Sprite::dump(options);
		return stream.str();
	}
	
	void Polygon::setVertices(const void* data, int size, int bformat, bool own)
	{
		if (_own && data != _verticesData)
		{
			delete[] _verticesData;
		}

		_own = own;
		_verticesData = reinterpret_cast<const unsigned char*>(data);
		_verticesSize = size;
		_vdecl = IVideoDriver::instance->getVertexDeclaration(bformat);
	}

	template <class T>
	void append(vector<unsigned char> &buff, const T &t)
	{
		const unsigned char *ptr = (const unsigned char *)&t;
		buff.insert(buff.end(), ptr, ptr + sizeof(t));
	}

	void Polygon::doRender(const RenderState &rs)
	{
		if (!_verticesSize)
			return;

		//Sprite
		_vstyle._apply(rs);
		const Diffuse &df = _frame.getDiffuse();
		
		//if (df.base)
		{
			//rs.renderer->setDiffuse(df);
			static vector<unsigned char> buff;
			buff.reserve(_verticesSize);
			buff.resize(0);
			int num = _verticesSize / _vdecl->size;
			
			const unsigned char *ptr = (const unsigned char *)_verticesData;
			for (int i = 0; i < num; ++i)
			{
				const Vector2 *pos = (Vector2 *)ptr;
				Vector2 t = rs.transform.transform(*pos);

				append(buff, t);
				buff.insert(buff.end(), ptr + sizeof(t), ptr + sizeof(t) + _vdecl->size - sizeof(t));

				ptr += _vdecl->size;
			}

			rs.renderer->addVertices(&buff.front(), buff.size());
		}
	}
}