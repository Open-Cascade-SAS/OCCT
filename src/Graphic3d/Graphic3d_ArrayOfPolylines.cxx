// File		Graphic3d_ArrayOfPolylines.cxx
// Created	04/01/01 : GG : G005

#include <Graphic3d_ArrayOfPolylines.ixx>

Graphic3d_ArrayOfPolylines :: Graphic3d_ArrayOfPolylines (
                        const Standard_Integer maxVertexs,
                        const Standard_Integer maxBounds,
                        const Standard_Integer maxEdges,
                        const Standard_Boolean hasVColors,
                        const Standard_Boolean hasFColors,
			const Standard_Boolean hasEdgeInfos)
	: Graphic3d_ArrayOfPrimitives(Graphic3d_TOPA_POLYLINES,maxVertexs,maxBounds,maxEdges,Standard_False,hasVColors,hasFColors,Standard_False,hasEdgeInfos) {}
