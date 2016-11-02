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


#include <Graphic3d_Group.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_MeshEntityOwner.hxx>
#include <Prs3d_Root.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <Standard_Type.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_MeshEntityOwner,SelectMgr_EntityOwner)

#ifndef MeshVS_PRSBUILDERHXX
#include <MeshVS_PrsBuilder.hxx>
#endif


//================================================================
// Function : Constructor MeshVS_MeshEntityOwner
// Purpose  :
//================================================================
MeshVS_MeshEntityOwner::MeshVS_MeshEntityOwner
                                   ( const SelectMgr_SOPtr& SelObj,
                                     const Standard_Integer ID,
                                     const Standard_Address MeshEntity,
                                     const MeshVS_EntityType& Type,
                                     const Standard_Integer Priority,
                                     const Standard_Boolean IsGroup )
: SelectMgr_EntityOwner ( SelObj, Priority ),
  myAddr    ( MeshEntity ),
  myType    ( Type ),
  myID      ( ID ),
  myIsGroup ( IsGroup )
{
  SelectBasics_EntityOwner::Set ( Priority );
}

//================================================================
// Function : Owner
// Purpose  :
//================================================================
Standard_Address MeshVS_MeshEntityOwner::Owner() const
{
  return myAddr;
}

//================================================================
// Function : Type
// Purpose  :
//================================================================
MeshVS_EntityType MeshVS_MeshEntityOwner::Type() const
{
  return myType;
}

//================================================================
// Function : IsGroup
// Purpose  :
//================================================================
Standard_Boolean MeshVS_MeshEntityOwner::IsGroup() const
{
  return myIsGroup;
}

//================================================================
// Function : IsHilighted
// Purpose  :
//================================================================
Standard_Boolean MeshVS_MeshEntityOwner::IsHilighted ( const Handle(PrsMgr_PresentationManager)&,
                                                 const Standard_Integer ) const
{
  return Standard_False;
}

//================================================================
// Function : HilightWithColor
// Purpose  :
//================================================================
void MeshVS_MeshEntityOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                               const Handle(Prs3d_Drawer)& theStyle,
                                               const Standard_Integer /*theMode*/ )
{
  Handle( SelectMgr_SelectableObject ) aSelObj;
  if ( HasSelectable() )
    aSelObj = Selectable();

  if ( thePM->IsImmediateModeOn() && aSelObj->IsKind( STANDARD_TYPE( MeshVS_Mesh ) ) )
  {
    Handle( MeshVS_Mesh ) aMesh = Handle( MeshVS_Mesh )::DownCast ( aSelObj );
    aMesh->HilightOwnerWithColor ( thePM, theStyle, this );
  }
}

//================================================================
// Function : Unhilight
// Purpose  :
//================================================================
void MeshVS_MeshEntityOwner::Unhilight ( const Handle(PrsMgr_PresentationManager)&,
                                   const Standard_Integer )
{
}

//================================================================
// Function : Clear
// Purpose  :
//================================================================
void MeshVS_MeshEntityOwner::Clear ( const Handle(PrsMgr_PresentationManager)&,
                               const Standard_Integer )
{
}

//================================================================
// Function : ID
// Purpose  :
//================================================================
Standard_Integer MeshVS_MeshEntityOwner::ID () const
{
  return myID;
}
