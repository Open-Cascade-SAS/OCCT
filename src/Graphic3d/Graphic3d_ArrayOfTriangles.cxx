// File		Graphic3d_ArrayOfTriangles.cxx
// Created	04/01/01 : GG : G005

#include <Graphic3d_ArrayOfTriangles.ixx>

Graphic3d_ArrayOfTriangles :: Graphic3d_ArrayOfTriangles (
                        const Standard_Integer maxVertexs,
                        const Standard_Integer maxEdges,
                        const Standard_Boolean hasVNormals,
                        const Standard_Boolean hasVColors,
                        const Standard_Boolean hasVTexels,
			const Standard_Boolean hasEdgeInfos)
	: Graphic3d_ArrayOfPrimitives(Graphic3d_TOPA_TRIANGLES,maxVertexs,0,maxEdges,hasVNormals,hasVColors,Standard_False,hasVTexels,hasEdgeInfos) {}
