// File		Graphic3d_ArrayOfSegments.cxx
// Created	04/01/01 : GG : G005

#include <Graphic3d_ArrayOfSegments.ixx>

Graphic3d_ArrayOfSegments :: Graphic3d_ArrayOfSegments (
                        const Standard_Integer maxVertexs,
                        const Standard_Integer maxEdges,
                        const Standard_Boolean hasVColors)
	: Graphic3d_ArrayOfPrimitives(Graphic3d_TOPA_SEGMENTS,maxVertexs,0,maxEdges,Standard_False,hasVColors,Standard_False,Standard_False,Standard_False) {}
