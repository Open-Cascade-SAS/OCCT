// Created on: 2007-01-29
// Created by: Sergey KOCHETKOV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <MeshVS_SensitiveMesh.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <TopLoc_Location.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_MeshOwner.hxx>
#include <NCollection_Vec4.hxx>


IMPLEMENT_STANDARD_RTTIEXT(MeshVS_SensitiveMesh,Select3D_SensitiveEntity)

//=======================================================================
// name    : MeshVS_SensitiveMesh::MeshVS_SensitiveMesh
// Purpose :
//=======================================================================
MeshVS_SensitiveMesh::MeshVS_SensitiveMesh (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                            const Standard_Integer theMode)
: Select3D_SensitiveEntity (theOwnerId)
{
  myMode = theMode;
  Handle(MeshVS_MeshOwner) anOwner = Handle(MeshVS_MeshOwner)::DownCast (OwnerId());
  if( !anOwner.IsNull() )
  {
    Handle(MeshVS_DataSource) aDS = anOwner->GetDataSource();
    if (!aDS.IsNull())
    {
      Bnd_Box aBox = aDS->GetBoundingBox();
      Standard_Real aXMin, aYMin, aZMin;
      Standard_Real aXMax, aYMax, aZMax;
      aBox.Get (aXMin, aYMin, aZMin,
                aXMax, aYMax, aZMax);
      Select3D_Vec3 aMinPnt (aXMin, aYMin, aZMin);
      Select3D_Vec3 aMaxPnt (aXMax, aYMax, aZMax);
      myBndBox = Select3D_BndBox3d (aMinPnt, aMaxPnt);
    }
  }
}

//================================================================
// Function : GetMode
// Purpose  :
//================================================================
Standard_Integer MeshVS_SensitiveMesh::GetMode() const
{
  return myMode;
}

//=======================================================================
// name    : GetConnected
// Purpose :
//=======================================================================
Handle(Select3D_SensitiveEntity) MeshVS_SensitiveMesh::GetConnected()
{
  Handle(MeshVS_SensitiveMesh) aMeshEnt = new MeshVS_SensitiveMesh (myOwnerId);
  return aMeshEnt;
}

//=======================================================================
// function : NbSubElements
// purpose  : Returns the amount of mesh nodes
//=======================================================================
Standard_Integer MeshVS_SensitiveMesh::NbSubElements()
{
  Handle(MeshVS_MeshOwner) anOwner = Handle(MeshVS_MeshOwner)::DownCast (OwnerId());
  if (anOwner.IsNull())
    return -1;
  Handle(MeshVS_DataSource) aDataSource = anOwner->GetDataSource();
  if (aDataSource.IsNull())
    return -1;
  return aDataSource->GetAllNodes().Extent();
}

//=======================================================================
// function : BoundingBox
// purpose  : Returns bounding box of mesh
//=======================================================================
Select3D_BndBox3d MeshVS_SensitiveMesh::BoundingBox()
{
  return myBndBox;
}

//=======================================================================
// function : CenterOfGeometry
// purpose  : Returns center of mesh
//=======================================================================
gp_Pnt MeshVS_SensitiveMesh::CenterOfGeometry() const
{
  if (!myBndBox.IsValid())
    return gp_Pnt (0.0, 0.0, 0.0);

  SelectMgr_Vec3 aCenter = (myBndBox.CornerMax() + myBndBox.CornerMin()) * 0.5;
  return gp_Pnt (aCenter.x(), aCenter.y(), aCenter.z());
}
