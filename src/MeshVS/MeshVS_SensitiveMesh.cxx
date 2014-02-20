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

#include <MeshVS_SensitiveMesh.ixx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <Select3D_Projector.hxx>
#include <TopLoc_Location.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_MeshOwner.hxx>

//=======================================================================
// name    : MeshVS_SensitiveMesh::MeshVS_SensitiveMesh
// Purpose :
//=======================================================================
MeshVS_SensitiveMesh::MeshVS_SensitiveMesh (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                            const Standard_Integer theMode)
: Select3D_SensitiveEntity( theOwnerId )
{
  myMode = theMode;
  mybox.SetVoid();
  Handle(MeshVS_MeshOwner) anOwner = Handle(MeshVS_MeshOwner)::DownCast( OwnerId() );
  if( !anOwner.IsNull() )
  {
    Handle(MeshVS_DataSource) aDS = anOwner->GetDataSource();
    if( !aDS.IsNull() )
      mybox = aDS->GetBoundingBox();
  }
}

//================================================================
// Function : GetMode
// Purpose  :
//================================================================
Standard_Integer MeshVS_SensitiveMesh::GetMode () const
{
  return myMode;
}

//=======================================================================
// name    : Matches
// Purpose :
//=======================================================================
Standard_Boolean MeshVS_SensitiveMesh::Matches (const SelectBasics_PickArgs& thePickArgs,
                                                Standard_Real& theMatchDMin,
                                                Standard_Real& theMatchDepth)
{
  theMatchDMin = 0.0;
  theMatchDepth = Precision::Infinite();

  Handle(MeshVS_MeshOwner) anOwner = Handle(MeshVS_MeshOwner)::DownCast( OwnerId() );
  if( anOwner.IsNull() ) return Standard_False;
  Handle(MeshVS_Mesh) aMeshPrs = Handle(MeshVS_Mesh)::DownCast( anOwner->Selectable() );
  if( aMeshPrs.IsNull() ) return Standard_False;
  Handle(MeshVS_DataSource) aDS = anOwner->GetDataSource();
  if( aDS.IsNull() ) return Standard_False;
  Handle(TColStd_HPackedMapOfInteger) NodesMap;
  Handle(TColStd_HPackedMapOfInteger) ElemsMap;

  // Mesh data source should provide the algorithm for computation
  // of detected entities from 2D point
  Standard_Boolean isDetected =
    aDS->GetDetectedEntities (aMeshPrs, thePickArgs.X(), thePickArgs.Y(),
                              thePickArgs.Tolerance(), NodesMap,
                              ElemsMap, theMatchDMin);

  // The detected entites will be available from mesh owner
  anOwner->SetDetectedEntities( NodesMap, ElemsMap );

  return isDetected;
}

//=======================================================================
// name    : Matches
// Purpose :
//=======================================================================
Standard_Boolean MeshVS_SensitiveMesh::Matches(const Standard_Real XMin,
					       const Standard_Real YMin,
					       const Standard_Real XMax,
					       const Standard_Real YMax,
					       const Standard_Real aTol)
{
  Handle(MeshVS_MeshOwner) anOwner = Handle(MeshVS_MeshOwner)::DownCast( OwnerId() );
  if( anOwner.IsNull() ) return Standard_False;
  Handle(MeshVS_Mesh) aMeshPrs = Handle(MeshVS_Mesh)::DownCast( anOwner->Selectable() );
  if( aMeshPrs.IsNull() ) return Standard_False;
  Handle(MeshVS_DataSource) aDS = anOwner->GetDataSource();
  if( aDS.IsNull() ) return Standard_False;
  Handle(TColStd_HPackedMapOfInteger) NodesMap;
  Handle(TColStd_HPackedMapOfInteger) ElemsMap;
  // Mesh data source should provide the algorithm for computation
  // of detected entities from 2D box area
  Standard_Boolean isDetected = aDS->GetDetectedEntities( aMeshPrs, XMin, YMin, XMax, YMax, aTol, NodesMap, ElemsMap );
  // The detected entites will be available from mesh owner
  anOwner->SetDetectedEntities( NodesMap, ElemsMap );
  
  return isDetected;
}

//=======================================================================
// name    : Matches
// Purpose :
//=======================================================================
Standard_Boolean MeshVS_SensitiveMesh::Matches(const TColgp_Array1OfPnt2d& Polyline,
					       const Bnd_Box2d&            aBox,
					       const Standard_Real         aTol)
{
  Handle(MeshVS_MeshOwner) anOwner = Handle(MeshVS_MeshOwner)::DownCast( OwnerId() );
  if( anOwner.IsNull() ) return Standard_False;
  Handle(MeshVS_Mesh) aMeshPrs = Handle(MeshVS_Mesh)::DownCast( anOwner->Selectable() );
  if( aMeshPrs.IsNull() ) return Standard_False;
  Handle(MeshVS_DataSource) aDS = anOwner->GetDataSource();
  if( aDS.IsNull() ) return Standard_False;
  Handle(TColStd_HPackedMapOfInteger) NodesMap;
  Handle(TColStd_HPackedMapOfInteger) ElemsMap;
  // Mesh data source should provide the algorithm for computation
  // of detected entities from 2D polyline
  Standard_Boolean isDetected = aDS->GetDetectedEntities( aMeshPrs, Polyline, aBox, aTol, NodesMap, ElemsMap );
  // The detected entites will be available from mesh owner
  anOwner->SetDetectedEntities( NodesMap, ElemsMap );
 
  return isDetected;
}

//=======================================================================
// name    : GetConnected
// Purpose :
//=======================================================================
Handle(Select3D_SensitiveEntity) MeshVS_SensitiveMesh::GetConnected( const TopLoc_Location& aLoc ) 
{
  Handle(MeshVS_SensitiveMesh) aMeshEnt = new MeshVS_SensitiveMesh( myOwnerId );
  if(HasLocation()) aMeshEnt->SetLocation( Location() );
  aMeshEnt->UpdateLocation( aLoc );
  return aMeshEnt;
}

//==================================================
// Function: ProjectOneCorner
// Purpose :
//==================================================
void MeshVS_SensitiveMesh::ProjectOneCorner(const Handle(Select3D_Projector)& theProj,
					    const Standard_Real theX, 
					    const Standard_Real theY, 
					    const Standard_Real theZ)
{
  gp_Pnt aPnt( theX, theY, theZ );  
  gp_Pnt2d aProjPnt;
  if( HasLocation() )
    theProj->Project( aPnt.Transformed(Location().Transformation()), aProjPnt );
  else 
    theProj->Project( aPnt, aProjPnt );
  mybox2d.Add( aProjPnt );
}

//==================================================
// Function: Project
// Purpose :
//==================================================
void MeshVS_SensitiveMesh::Project(const Handle(Select3D_Projector)& aProj)
{
  mybox2d.SetVoid();
  if (mybox.IsVoid())
    return;
  // Compute the 2D bounding box - projection of mesh bounding box
  Handle(MeshVS_MeshOwner) anOwner = Handle(MeshVS_MeshOwner)::DownCast( OwnerId() );
  if( anOwner.IsNull() ) return;
  Handle(MeshVS_DataSource) aDS = anOwner->GetDataSource();
  if( aDS.IsNull() ) return;

  Standard_Real XMin, YMin, ZMin, XMax, YMax, ZMax;
  mybox.Get( XMin, YMin, ZMin, XMax, YMax, ZMax );  

  ProjectOneCorner (aProj, XMin, YMin, ZMin);
  ProjectOneCorner (aProj, XMin, YMin, ZMax);
  ProjectOneCorner (aProj, XMin, YMax, ZMin);
  ProjectOneCorner (aProj, XMin, YMax, ZMax);
  ProjectOneCorner (aProj, XMax, YMin, ZMin);
  ProjectOneCorner (aProj, XMax, YMin, ZMax);
  ProjectOneCorner (aProj, XMax, YMax, ZMin);
  ProjectOneCorner (aProj, XMax, YMax, ZMax);
}

//==================================================
// Function: Areas 
// Purpose :
//==================================================
void MeshVS_SensitiveMesh::Areas( SelectBasics_ListOfBox2d& aSeq )
{
  aSeq.Append(mybox2d);
}
