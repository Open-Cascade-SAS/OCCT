// File		Graphic3d_ArrayOfQuadrangles.cxx
// Created	04/01/01 : GG : G005

#include <Graphic3d_ArrayOfQuadrangles.ixx>

Graphic3d_ArrayOfQuadrangles :: Graphic3d_ArrayOfQuadrangles (
                        const Standard_Integer maxVertexs,
                        const Standard_Integer maxEdges,
                        const Standard_Boolean hasVNormals,
                        const Standard_Boolean hasVColors,
                        const Standard_Boolean hasVTexels,
			const Standard_Boolean hasEdgesInfos)
	: Graphic3d_ArrayOfPrimitives(Graphic3d_TOPA_QUADRANGLES,maxVertexs,0,maxEdges,hasVNormals,hasVColors,Standard_False,hasVTexels,hasEdgesInfos) {}
