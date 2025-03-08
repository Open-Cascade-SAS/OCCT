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

#include <BRepMesh_MeshAlgoFactory.hxx>
#include <BRepMesh_SphereRangeSplitter.hxx>
#include <BRepMesh_CylinderRangeSplitter.hxx>
#include <BRepMesh_ConeRangeSplitter.hxx>
#include <BRepMesh_TorusRangeSplitter.hxx>
#include <BRepMesh_DelaunayBaseMeshAlgo.hxx>
#include <BRepMesh_DelaunayDeflectionControlMeshAlgo.hxx>
#include <BRepMesh_BoundaryParamsRangeSplitter.hxx>
#include <BRepMesh_ExtrusionRangeSplitter.hxx>
#include <BRepMesh_UndefinedRangeSplitter.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_MeshAlgoFactory, IMeshTools_MeshAlgoFactory)

namespace
{
struct BaseMeshAlgo
{
  typedef BRepMesh_DelaunayBaseMeshAlgo Type;
};

template <class RangeSplitter>
struct NodeInsertionMeshAlgo
{
  typedef BRepMesh_DelaunayNodeInsertionMeshAlgo<RangeSplitter, BRepMesh_DelaunayBaseMeshAlgo> Type;
};

template <class RangeSplitter>
struct DeflectionControlMeshAlgo
{
  typedef BRepMesh_DelaunayDeflectionControlMeshAlgo<RangeSplitter, BRepMesh_DelaunayBaseMeshAlgo>
    Type;
};
} // namespace

//=================================================================================================

BRepMesh_MeshAlgoFactory::BRepMesh_MeshAlgoFactory() {}

//=================================================================================================

BRepMesh_MeshAlgoFactory::~BRepMesh_MeshAlgoFactory() {}

//=================================================================================================

Handle(IMeshTools_MeshAlgo) BRepMesh_MeshAlgoFactory::GetAlgo(
  const GeomAbs_SurfaceType    theSurfaceType,
  const IMeshTools_Parameters& theParameters) const
{
  switch (theSurfaceType)
  {
    case GeomAbs_Plane:
      return theParameters.EnableControlSurfaceDeflectionAllSurfaces
               ? new DeflectionControlMeshAlgo<BRepMesh_DefaultRangeSplitter>::Type
               : (theParameters.InternalVerticesMode
                    ? new NodeInsertionMeshAlgo<BRepMesh_DefaultRangeSplitter>::Type
                    : new BaseMeshAlgo::Type);
      break;

    case GeomAbs_Sphere:
      return theParameters.EnableControlSurfaceDeflectionAllSurfaces
               ? new DeflectionControlMeshAlgo<BRepMesh_SphereRangeSplitter>::Type
               : new NodeInsertionMeshAlgo<BRepMesh_SphereRangeSplitter>::Type;
      break;

    case GeomAbs_Cylinder:
      return theParameters.EnableControlSurfaceDeflectionAllSurfaces
               ? new DeflectionControlMeshAlgo<BRepMesh_CylinderRangeSplitter>::Type
               : (theParameters.InternalVerticesMode
                    ? new NodeInsertionMeshAlgo<BRepMesh_CylinderRangeSplitter>::Type
                    : new BaseMeshAlgo::Type);
      break;

    case GeomAbs_Cone:
      return theParameters.EnableControlSurfaceDeflectionAllSurfaces
               ? new DeflectionControlMeshAlgo<BRepMesh_ConeRangeSplitter>::Type
               : new NodeInsertionMeshAlgo<BRepMesh_ConeRangeSplitter>::Type;
      break;

    case GeomAbs_Torus:
      return theParameters.EnableControlSurfaceDeflectionAllSurfaces
               ? new DeflectionControlMeshAlgo<BRepMesh_TorusRangeSplitter>::Type
               : new NodeInsertionMeshAlgo<BRepMesh_TorusRangeSplitter>::Type;
      break;

    case GeomAbs_SurfaceOfRevolution:
      return new DeflectionControlMeshAlgo<BRepMesh_BoundaryParamsRangeSplitter>::Type;
      break;

    case GeomAbs_SurfaceOfExtrusion:
      return new DeflectionControlMeshAlgo<BRepMesh_ExtrusionRangeSplitter>::Type;
      break;

    case GeomAbs_BezierSurface:
    case GeomAbs_BSplineSurface:
      return new DeflectionControlMeshAlgo<BRepMesh_NURBSRangeSplitter>::Type;
      break;

    case GeomAbs_OffsetSurface:
    case GeomAbs_OtherSurface:
    default:
      return new DeflectionControlMeshAlgo<BRepMesh_UndefinedRangeSplitter>::Type;
  }
}
