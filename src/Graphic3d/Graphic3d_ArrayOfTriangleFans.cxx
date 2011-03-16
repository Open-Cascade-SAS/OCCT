// File		Graphic3d_ArrayOfTriangleFans.cxx
// Created	04/01/01 : GG : G005

#include <Graphic3d_ArrayOfTriangleFans.ixx>

Graphic3d_ArrayOfTriangleFans :: Graphic3d_ArrayOfTriangleFans (
                        const Standard_Integer maxVertexs,
                        const Standard_Integer maxFans,
                        const Standard_Boolean hasVNormals,
                        const Standard_Boolean hasVColors,
                        const Standard_Boolean hasFColors,
                        const Standard_Boolean hasVTexels)
	: Graphic3d_ArrayOfPrimitives(Graphic3d_TOPA_TRIANGLEFANS,maxVertexs,maxFans,0,hasVNormals,hasVColors,hasFColors,hasVTexels,Standard_False) {}
