// File		Graphic3d_ArrayOfPoints.cxx
// Created	04/01/01 : GG : G005

#include <Graphic3d_ArrayOfPoints.ixx>

Graphic3d_ArrayOfPoints :: Graphic3d_ArrayOfPoints (
                        const Standard_Integer maxVertexs)
	: Graphic3d_ArrayOfPrimitives(Graphic3d_TOPA_POINTS,maxVertexs,0,0,Standard_False,Standard_False,Standard_False,Standard_False,Standard_False) {}
