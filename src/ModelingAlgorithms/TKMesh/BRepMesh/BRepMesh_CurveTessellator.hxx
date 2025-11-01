// Created on: 2016-04-19
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _BRepMesh_EdgeTessellator_HeaderFile
#define _BRepMesh_EdgeTessellator_HeaderFile

#include <IMeshTools_CurveTessellator.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <IMeshData_Types.hxx>

class Geom_Surface;
class Geom2d_Curve;
struct IMeshTools_Parameters;

//! Auxiliary class performing tessellation of passed edge according to specified parameters.
class BRepMesh_CurveTessellator : public IMeshTools_CurveTessellator
{
public:
  //! Constructor.
  Standard_EXPORT BRepMesh_CurveTessellator(const IMeshData::IEdgeHandle& theEdge,
                                            const IMeshTools_Parameters&  theParameters,
                                            const Standard_Integer        theMinPointsNb = 2);

  //! Constructor.
  Standard_EXPORT BRepMesh_CurveTessellator(const IMeshData::IEdgeHandle& theEdge,
                                            const TopAbs_Orientation      theOrientation,
                                            const IMeshData::IFaceHandle& theFace,
                                            const IMeshTools_Parameters&  theParameters,
                                            const Standard_Integer        theMinPointsNb = 2);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_CurveTessellator();

  //! Returns number of tessellation points.
  Standard_EXPORT virtual Standard_Integer PointsNb() const Standard_OVERRIDE;

  //! Returns parameters of solution with the given index.
  //! @param theIndex index of tessellation point.
  //! @param theParameter parameters on PCurve corresponded to the solution.
  //! @param thePoint tessellation point.
  //! @return True in case of valid result, false elewhere.
  Standard_EXPORT virtual Standard_Boolean Value(const Standard_Integer theIndex,
                                                 gp_Pnt&                thePoint,
                                                 Standard_Real&         theParameter) const
    Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BRepMesh_CurveTessellator, IMeshTools_CurveTessellator)

private:
  //! Performs initialization of this tool.
  void init();

  //! Adds internal vertices to discrete polygon.
  void addInternalVertices();

  // Check deflection in 2d space for improvement of edge tessellation.
  void splitByDeflection2d();

  void splitSegment(const Handle(Geom_Surface)& theSurf,
                    const Handle(Geom2d_Curve)& theCurve2d,
                    const Standard_Real         theFirst,
                    const Standard_Real         theLast,
                    const Standard_Integer      theNbIter);

  //! Checks whether the given point lies within tolerance of the vertex.
  Standard_Boolean isInToleranceOfVertex(const gp_Pnt&        thePoint,
                                         const TopoDS_Vertex& theVertex) const;

private:
  BRepMesh_CurveTessellator(const BRepMesh_CurveTessellator& theOther);

  void operator=(const BRepMesh_CurveTessellator& theOther);

private:
  const IMeshData::IEdgeHandle& myDEdge;
  const IMeshTools_Parameters&  myParameters;
  TopoDS_Edge                   myEdge;
  BRepAdaptor_Curve             myCurve;
  Standard_Integer              myMinPointsNb;
  GCPnts_TangentialDeflection   myDiscretTool;
  TopoDS_Vertex                 myFirstVertex;
  TopoDS_Vertex                 myLastVertex;
  Standard_Real                 mySquareEdgeDef;
  Standard_Real                 mySquareMinSize;
  Standard_Real                 myEdgeSqTol;
  Standard_Real                 myFaceRangeU[2];
  Standard_Real                 myFaceRangeV[2];
};

#endif