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

#include <MeshVS_PrsBuilder.hxx>

#include <MeshVS_DataSource.hxx>
#include <MeshVS_Drawer.hxx>
#include <MeshVS_Mesh.hxx>
#include <Prs3d_Presentation.hxx>
#include <Select3D_SensitiveEntity.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_PrsBuilder, Standard_Transient)

//=================================================================================================

MeshVS_PrsBuilder::MeshVS_PrsBuilder(const Handle(MeshVS_Mesh)&       Parent,
                                     const MeshVS_DisplayModeFlags&   Flags,
                                     const Handle(MeshVS_DataSource)& DS,
                                     const Standard_Integer           Id,
                                     const MeshVS_BuilderPriority&    Priority)
{
  if (Id < 0 && !Parent.IsNull())
    myId = Parent->GetFreeId();
  else
    myId = Id;

  myParentMesh = Parent.operator->();
  myDataSource = DS;
  myDrawer     = 0;

  myFlags       = Flags;
  myIsExcluding = Standard_False;

  myPriority = Priority;
}

//=================================================================================================

void MeshVS_PrsBuilder::CustomBuild(const Handle(Prs3d_Presentation)&,
                                    const TColStd_PackedMapOfInteger&,
                                    TColStd_PackedMapOfInteger&,
                                    const Standard_Integer) const
{
}

//=================================================================================================

Handle(Select3D_SensitiveEntity) MeshVS_PrsBuilder::CustomSensitiveEntity(
  const Handle(SelectMgr_EntityOwner)&,
  const Standard_Integer) const
{
  return 0;
}

//=================================================================================================

Handle(MeshVS_DataSource) MeshVS_PrsBuilder::DataSource() const
{
  return myDataSource;
}

//=================================================================================================

Handle(MeshVS_DataSource) MeshVS_PrsBuilder::GetDataSource() const
{
  if (myDataSource.IsNull())
    return myParentMesh->GetDataSource();
  else
    return myDataSource;
}

//=================================================================================================

void MeshVS_PrsBuilder::SetDataSource(const Handle(MeshVS_DataSource)& DS)
{
  myDataSource = DS;
}

//=================================================================================================

Standard_Integer MeshVS_PrsBuilder::GetFlags() const
{
  return myFlags;
}

//=================================================================================================

Standard_Integer MeshVS_PrsBuilder::GetId() const
{
  return myId;
}

//=================================================================================================

Standard_Boolean MeshVS_PrsBuilder::TestFlags(const Standard_Integer DisplayMode) const
{
  return ((DisplayMode & GetFlags()) > 0);
}

//=================================================================================================

void MeshVS_PrsBuilder::SetExcluding(const Standard_Boolean state)
{
  myIsExcluding = state;
}

//=================================================================================================

Standard_Boolean MeshVS_PrsBuilder::IsExcludingOn() const
{
  return myIsExcluding;
}

//=================================================================================================

Standard_Integer MeshVS_PrsBuilder::GetPriority() const
{
  return myPriority;
}

//=================================================================================================

Handle(MeshVS_Drawer) MeshVS_PrsBuilder::GetDrawer() const
{
  if (myDrawer.IsNull())
    return myParentMesh->GetDrawer();
  else
    return myDrawer;
}

//=================================================================================================

void MeshVS_PrsBuilder::SetDrawer(const Handle(MeshVS_Drawer)& Dr)
{
  myDrawer = Dr;
}

//=================================================================================================

Handle(MeshVS_Drawer) MeshVS_PrsBuilder::Drawer() const
{
  return myDrawer;
}

//================================================================
// Function : SetPresentationManager
// Purpose  : Set presentation manager. This method is used by
//            MeshVS_Mesh::Compute methodto assign presentation
//            manager to the builder.
//================================================================
void MeshVS_PrsBuilder::SetPresentationManager(const Handle(PrsMgr_PresentationManager)& thePrsMgr)
{
  myPrsMgr = thePrsMgr;
}

//================================================================
// Function : GetPresentationManager
// Purpose  : Get presentation manager
//================================================================
Handle(PrsMgr_PresentationManager) MeshVS_PrsBuilder::GetPresentationManager() const
{
  return myPrsMgr;
}
