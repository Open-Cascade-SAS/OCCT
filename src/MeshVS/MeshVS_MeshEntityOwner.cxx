// Created on: 2003-09-09
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <MeshVS_MeshEntityOwner.ixx>

#include <SelectBasics_EntityOwner.hxx>
#include <Graphic3d_Group.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <Prs3d_Root.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <MeshVS_Mesh.hxx>


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
// Function : Hilight
// Purpose  :
//================================================================
void MeshVS_MeshEntityOwner::Hilight ()
{
}

//================================================================
// Function : Hilight
// Purpose  :
//================================================================
void MeshVS_MeshEntityOwner::Hilight ( const Handle(PrsMgr_PresentationManager)&,
                                 const Standard_Integer )
{
}

//================================================================
// Function : HilightWithColor
// Purpose  :
//================================================================
void MeshVS_MeshEntityOwner::HilightWithColor ( const Handle(PrsMgr_PresentationManager3d)& thePM,
                                          const Quantity_NameOfColor theColor,
                                          const Standard_Integer /*theMode*/ )
{
  Handle( SelectMgr_SelectableObject ) aSelObj;
  if ( HasSelectable() )
    aSelObj = Selectable();

  if ( thePM->IsImmediateModeOn() && aSelObj->IsKind( STANDARD_TYPE( MeshVS_Mesh ) ) )
  {
    Handle( MeshVS_Mesh ) aMesh = Handle( MeshVS_Mesh )::DownCast ( aSelObj );
    aMesh->HilightOwnerWithColor ( thePM, theColor, this );
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
