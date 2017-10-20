#pragma once
#include <ShapeData.h>

typedef unsigned int uint;

class ShapeGenerator
{

	static ShapeData makePlaneVerts(uint dimensions);
	static ShapeData makePlaneIndices(uint dimensions);

public:
	static ShapeData makeTriangle();
	static ShapeData makeCube();
	static ShapeData makeArrow();
	static ShapeData makePlane(uint dimensions = 10);
	static ShapeData makeTeapot(uint tesselation = 10, const glm::mat4& lidTransform = glm::mat4());
	static ShapeData generateNormals(const ShapeData& data);
};

