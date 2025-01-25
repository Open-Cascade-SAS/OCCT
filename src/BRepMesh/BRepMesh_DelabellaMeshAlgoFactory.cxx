// Created on: 2019-07-05
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <BRepMesh_DelabellaMeshAlgoFactory.hxx>
#include <BRepMesh_SphereRangeSplitter.hxx>
#include <BRepMesh_CylinderRangeSplitter.hxx>
#include <BRepMesh_ConeRangeSplitter.hxx>
#include <BRepMesh_TorusRangeSplitter.hxx>
#include <BRepMesh_DelaunayBaseMeshAlgo.hxx>
#include <BRepMesh_DelabellaBaseMeshAlgo.hxx>
#include <BRepMesh_CustomDelaunayBaseMeshAlgo.hxx>
#include <BRepMesh_DelaunayDeflectionControlMeshAlgo.hxx>
#include <BRepMesh_BoundaryParamsRangeSplitter.hxx>

namespace
{
struct DefaultBaseMeshAlgo
{
  typedef BRepMesh_DelaunayBaseMeshAlgo Type;
};

template <class RangeSplitter>
struct DefaultNodeInsertionMeshAlgo
{
  typedef BRepMesh_DelaunayNodeInsertionMeshAlgo<RangeSplitter, BRepMesh_DelaunayBaseMeshAlgo> Type;
};

struct BaseMeshAlgo
{
  typedef BRepMesh_DelabellaBaseMeshAlgo Type;
};

template <class RangeSplitter>
struct NodeInsertionMeshAlgo
{
  typedef BRepMesh_DelaunayNodeInsertionMeshAlgo<
    RangeSplitter,
    BRepMesh_CustomDelaunayBaseMeshAlgo<BRepMesh_DelabellaBaseMeshAlgo>>
    Type;
};

template <class RangeSplitter>
struct DeflectionControlMeshAlgo
{
  typedef BRepMesh_DelaunayDeflectionControlMeshAlgo<
    RangeSplitter,
    BRepMesh_CustomDelaunayBaseMeshAlgo<BRepMesh_DelabellaBaseMeshAlgo>>
    Type;
};
} // namespace

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_DelabellaMeshAlgoFactory, IMeshTools_MeshAlgoFactory)

//=================================================================================================

BRepMesh_DelabellaMeshAlgoFactory::BRepMesh_DelabellaMeshAlgoFactory() {}

//=================================================================================================

BRepMesh_DelabellaMeshAlgoFactory::~BRepMesh_DelabellaMeshAlgoFactory() {}

//=================================================================================================

Handle(IMeshTools_MeshAlgo) BRepMesh_DelabellaMeshAlgoFactory::GetAlgo(
  const GeomAbs_SurfaceType    theSurfaceType,
  const IMeshTools_Parameters& theParameters) const
{
  switch (theSurfaceType)
  {
    case GeomAbs_Plane:
      return theParameters.InternalVerticesMode
               ? new NodeInsertionMeshAlgo<BRepMesh_DefaultRangeSplitter>::Type
               : new BaseMeshAlgo::Type;
      break;

    case GeomAbs_Sphere: {
      NodeInsertionMeshAlgo<BRepMesh_SphereRangeSplitter>::Type* aMeshAlgo =
        new NodeInsertionMeshAlgo<BRepMesh_SphereRangeSplitter>::Type;
      aMeshAlgo->SetPreProcessSurfaceNodes(Standard_True);
      return aMeshAlgo;
    }
    break;

    case GeomAbs_Cylinder:
      return theParameters.InternalVerticesMode
               ? new DefaultNodeInsertionMeshAlgo<BRepMesh_CylinderRangeSplitter>::Type
               : new DefaultBaseMeshAlgo::Type;
      break;

    case GeomAbs_Cone: {
      NodeInsertionMeshAlgo<BRepMesh_ConeRangeSplitter>::Type* aMeshAlgo =
        new NodeInsertionMeshAlgo<BRepMesh_ConeRangeSplitter>::Type;
      aMeshAlgo->SetPreProcessSurfaceNodes(Standard_True);
      return aMeshAlgo;
    }
    break;

    case GeomAbs_Torus: {
      NodeInsertionMeshAlgo<BRepMesh_TorusRangeSplitter>::Type* aMeshAlgo =
        new NodeInsertionMeshAlgo<BRepMesh_TorusRangeSplitter>::Type;
      aMeshAlgo->SetPreProcessSurfaceNodes(Standard_True);
      return aMeshAlgo;
    }
    break;

    case GeomAbs_SurfaceOfRevolution: {
      DeflectionControlMeshAlgo<BRepMesh_BoundaryParamsRangeSplitter>::Type* aMeshAlgo =
        new DeflectionControlMeshAlgo<BRepMesh_BoundaryParamsRangeSplitter>::Type;
      aMeshAlgo->SetPreProcessSurfaceNodes(Standard_True);
      return aMeshAlgo;
    }
    break;

    default: {
      DeflectionControlMeshAlgo<BRepMesh_NURBSRangeSplitter>::Type* aMeshAlgo =
        new DeflectionControlMeshAlgo<BRepMesh_NURBSRangeSplitter>::Type;
      aMeshAlgo->SetPreProcessSurfaceNodes(Standard_True);
      return aMeshAlgo;
    }
  }
}
