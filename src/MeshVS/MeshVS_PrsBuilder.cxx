// Created on: 2003-09-09
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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


#include <MeshVS_DataSource.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_PrsBuilder.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_PrsBuilder,Standard_Transient)

//================================================================
// Function : Constructor MeshVS_PrsBuilder
// Purpose  :
//================================================================
MeshVS_PrsBuilder::MeshVS_PrsBuilder ( const Handle(MeshVS_Mesh)& Parent,
                                       const MeshVS_DisplayModeFlags& Flags,
                                       const Handle(MeshVS_DataSource)& DS,
                                       const Standard_Integer Id,
                                       const MeshVS_BuilderPriority& Priority )
{
  if (Id<0 && ! Parent.IsNull())
    myId = Parent->GetFreeId();
  else
    myId = Id;

  myParentMesh = Parent.operator->();
  myDataSource = DS;
  myDrawer = 0;

  myFlags = Flags;
  myIsExcluding = Standard_False;

  myPriority = Priority;
}

//================================================================
// Function : CustomDraw
// Purpose  :
//================================================================
void MeshVS_PrsBuilder::CustomBuild ( const Handle(Prs3d_Presentation)&,
                                      const TColStd_PackedMapOfInteger&,
                                      TColStd_PackedMapOfInteger&,
                                      const Standard_Integer ) const
{
}

//================================================================
// Function : CustomSensitiveEntity
// Purpose  :
//================================================================
Handle( SelectBasics_SensitiveEntity ) MeshVS_PrsBuilder::CustomSensitiveEntity
  ( const Handle( SelectBasics_EntityOwner )&,
    const Standard_Integer ) const
{
  return 0;
}

//================================================================
// Function : DataSource
// Purpose  :
//================================================================
Handle (MeshVS_DataSource) MeshVS_PrsBuilder::DataSource () const
{
  return myDataSource;
}

//================================================================
// Function : GetDataSource
// Purpose  :
//================================================================
Handle (MeshVS_DataSource) MeshVS_PrsBuilder::GetDataSource () const
{
  if ( myDataSource.IsNull() )
    return myParentMesh->GetDataSource();
  else
    return myDataSource;
}

//================================================================
// Function : SetDataSource
// Purpose  :
//================================================================
void MeshVS_PrsBuilder::SetDataSource ( const Handle (MeshVS_DataSource)& DS )
{
  myDataSource = DS;
}

//================================================================
// Function : GetFlags
// Purpose  :
//================================================================
Standard_Integer MeshVS_PrsBuilder::GetFlags () const
{
  return myFlags;
}

//================================================================
// Function : GetId
// Purpose  :
//================================================================
Standard_Integer MeshVS_PrsBuilder::GetId () const
{
  return myId;
}

//================================================================
// Function : TestFlags
// Purpose  :
//================================================================
Standard_Boolean MeshVS_PrsBuilder::TestFlags ( const Standard_Integer DisplayMode ) const
{
  return ( ( DisplayMode & GetFlags() ) > 0 );
}

//================================================================
// Function : SetExcluding
// Purpose  :
//================================================================
void MeshVS_PrsBuilder::SetExcluding  ( const Standard_Boolean state )
{
  myIsExcluding = state;
}

//================================================================
// Function : IsExcludingOn
// Purpose  :
//================================================================
Standard_Boolean MeshVS_PrsBuilder::IsExcludingOn () const
{
  return myIsExcluding;
}

//================================================================
// Function : GetPriority
// Purpose  :
//================================================================
Standard_Integer MeshVS_PrsBuilder::GetPriority () const
{
  return myPriority;
}

//================================================================
// Function : GetDrawer
// Purpose  :
//================================================================
Handle (MeshVS_Drawer) MeshVS_PrsBuilder::GetDrawer () const
{
  if ( myDrawer.IsNull() )
    return myParentMesh->GetDrawer();
  else
    return myDrawer;
}

//================================================================
// Function : SetDataSource
// Purpose  :
//================================================================
void MeshVS_PrsBuilder::SetDrawer ( const Handle (MeshVS_Drawer)& Dr )
{
  myDrawer = Dr;
}

//================================================================
// Function : Drawer
// Purpose  :
//================================================================
Handle (MeshVS_Drawer) MeshVS_PrsBuilder::Drawer () const
{
  return myDrawer;
}

//================================================================
// Function : SetPresentationManager
// Purpose  : Set presentation manager. This method is used by 
//            MeshVS_Mesh::Compute methodto assign presentation 
//            manager to the builder. 
//================================================================
void MeshVS_PrsBuilder::SetPresentationManager( const Handle(PrsMgr_PresentationManager3d)& thePrsMgr )
{
  myPrsMgr = thePrsMgr;
}

//================================================================
// Function : GetPresentationManager
// Purpose  : Get presentation manager
//================================================================
Handle(PrsMgr_PresentationManager3d) MeshVS_PrsBuilder::GetPresentationManager() const
{
  return myPrsMgr;
}
