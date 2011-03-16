// File		Graphic3d_ArrayOfPolygons.cxx
// Created	04/01/01 : GG : G005

#include <Graphic3d_ArrayOfPolygons.ixx>

Graphic3d_ArrayOfPolygons :: Graphic3d_ArrayOfPolygons (
                        const Standard_Integer maxVertexs,
                        const Standard_Integer maxBounds,
                        const Standard_Integer maxEdges,
                        const Standard_Boolean hasVNormals,
                        const Standard_Boolean hasVColors,
                        const Standard_Boolean hasFColors,
                        const Standard_Boolean hasVTexels,
			const Standard_Boolean hasEdgeInfos)
	: Graphic3d_ArrayOfPrimitives(Graphic3d_TOPA_POLYGONS,maxVertexs,maxBounds,maxEdges,hasVNormals,hasVColors,hasFColors,hasVTexels,hasEdgeInfos) {}
