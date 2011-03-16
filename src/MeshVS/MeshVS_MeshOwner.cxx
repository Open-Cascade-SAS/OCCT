// File:	MeshVS_MeshOwner.cxx
// Created:	Thu Jan 25 2007
// Author:	Sergey KOCHETKOV
// Copyright:	Open CASCADE 2007

#include <MeshVS_MeshOwner.ixx>

#include <SelectBasics_EntityOwner.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <MeshVS_Mesh.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>


#ifndef MeshVS_PRSBUILDERHXX
#include <MeshVS_PrsBuilder.hxx>
#endif


//================================================================
// Function : Constructor MeshVS_MeshOwner
// Purpose  :
//================================================================
MeshVS_MeshOwner::MeshVS_MeshOwner (const SelectMgr_SOPtr&           theSelObj,
				    const Handle(MeshVS_DataSource)& theDS,
				    const Standard_Integer           thePriority)
: SelectMgr_EntityOwner ( theSelObj, thePriority )
{
  myLastID = -1;
  if( !theDS.IsNull() )
    myDataSource = theDS;
  SelectBasics_EntityOwner::Set ( thePriority );
}

//================================================================
// Function : GetDataSource
// Purpose  :
//================================================================
const Handle(MeshVS_DataSource)& MeshVS_MeshOwner::GetDataSource () const
{
  return myDataSource;
}

//================================================================
// Function : GetSelectedNodes
// Purpose  :
//================================================================
const Handle(TColStd_HPackedMapOfInteger)& MeshVS_MeshOwner::GetSelectedNodes () const
{
  return mySelectedNodes;
}

//================================================================
// Function : GetSelectedElements
// Purpose  :
//================================================================
const Handle(TColStd_HPackedMapOfInteger)& MeshVS_MeshOwner::GetSelectedElements () const
{
  return mySelectedElems;
}

//================================================================
// Function : AddSelectedEntities
// Purpose  :
//================================================================
void MeshVS_MeshOwner::AddSelectedEntities (const Handle(TColStd_HPackedMapOfInteger)& Nodes,
					    const Handle(TColStd_HPackedMapOfInteger)& Elems)
{
  if( mySelectedNodes.IsNull() )
    mySelectedNodes = Nodes;
  else if( !Nodes.IsNull() )
    mySelectedNodes->ChangeMap().Unite( Nodes->Map() );
  if( mySelectedElems.IsNull() )
    mySelectedElems = Elems;
  else if( !Elems.IsNull() )
    mySelectedElems->ChangeMap().Unite( Elems->Map() );
}

//================================================================
// Function : ClearSelectedEntities
// Purpose  :
//================================================================
void MeshVS_MeshOwner::ClearSelectedEntities ()
{
  mySelectedNodes.Nullify();
  mySelectedElems.Nullify();
}

//================================================================
// Function : GetDetectedNodes
// Purpose  :
//================================================================
const Handle(TColStd_HPackedMapOfInteger)& MeshVS_MeshOwner::GetDetectedNodes () const
{
  return myDetectedNodes;
}

//================================================================
// Function : GetDetectedElements
// Purpose  :
//================================================================
const Handle(TColStd_HPackedMapOfInteger)& MeshVS_MeshOwner::GetDetectedElements () const
{
  return myDetectedElems;
}

//================================================================
// Function : SetDetectedEntities
// Purpose  :
//================================================================
void MeshVS_MeshOwner::SetDetectedEntities (const Handle(TColStd_HPackedMapOfInteger)& Nodes,
					    const Handle(TColStd_HPackedMapOfInteger)& Elems)
{
  myDetectedNodes = Nodes;
  myDetectedElems = Elems;
  if( State() ) State( 0 );
}

//================================================================
// Function : HilightWithColor
// Purpose  :
//================================================================
void MeshVS_MeshOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
					 const Quantity_NameOfColor theColor,
					 const Standard_Integer /*theMode*/)
{
  Handle( SelectMgr_SelectableObject ) aSelObj;
  if ( HasSelectable() )
    aSelObj = Selectable();

  if ( thePM->IsImmediateModeOn() && aSelObj->IsKind( STANDARD_TYPE( MeshVS_Mesh ) ) )
  {
    // Update last detected entity ID
    Handle(TColStd_HPackedMapOfInteger) aNodes = GetDetectedNodes();
    Handle(TColStd_HPackedMapOfInteger) aElems = GetDetectedElements(); 
    if( !aNodes.IsNull() && aNodes->Map().Extent() == 1 )
    {
      TColStd_MapIteratorOfPackedMapOfInteger anIt( aNodes->Map() );
      for( ; anIt.More(); anIt.Next() )
      {
	if( myLastID != anIt.Key() )
	  myLastID = anIt.Key();
	break;
      }
    }  
    else if( !aElems.IsNull() && aElems->Map().Extent() == 1 )
    {
      TColStd_MapIteratorOfPackedMapOfInteger anIt( aElems->Map() );
      for( ; anIt.More(); anIt.Next() )
      {
	if( myLastID != anIt.Key() )
	  myLastID = anIt.Key();
	break;
      }
    }

    // hilight detected entities
    Handle( MeshVS_Mesh ) aMesh = Handle( MeshVS_Mesh )::DownCast ( aSelObj );
    aMesh->HilightOwnerWithColor ( thePM, theColor, this );
  }
}

void MeshVS_MeshOwner::Unhilight(const Handle(PrsMgr_PresentationManager)& thePM,
				 const Standard_Integer theMode)
{
  SelectMgr_EntityOwner::Unhilight( thePM, theMode );

  Handle(TColStd_HPackedMapOfInteger) aNodes = GetDetectedNodes();
  Handle(TColStd_HPackedMapOfInteger) aElems = GetDetectedElements();  
  if( ( !aNodes.IsNull() && !aNodes->Map().Contains( myLastID ) ) ||
      ( !aElems.IsNull() && !aElems->Map().Contains( myLastID ) ) )
    return;
  // Reset last detected ID
  myLastID = -1;
}

Standard_Boolean MeshVS_MeshOwner::IsForcedHilight () const
{
  Standard_Boolean aHilight = Standard_True;
  Standard_Integer aKey = -1;
  if( myLastID > 0 )
  {
    // Check the detected entity and 
    // allow to hilight it if it differs from the last detected entity <myLastID>
    Handle(TColStd_HPackedMapOfInteger) aNodes = GetDetectedNodes();
    if( !aNodes.IsNull() && aNodes->Map().Extent() == 1 )
    {
      TColStd_MapIteratorOfPackedMapOfInteger anIt( aNodes->Map() );
      for( ; anIt.More(); anIt.Next() )
      {
	aKey = anIt.Key();
	if( myLastID == aKey )
	  aHilight = Standard_False;
	break;
      }
    }  
    Handle(TColStd_HPackedMapOfInteger) aElems = GetDetectedElements();
    if( !aElems.IsNull() && aElems->Map().Extent() == 1 )
    {
      TColStd_MapIteratorOfPackedMapOfInteger anIt( aElems->Map() );
      for( ; anIt.More(); anIt.Next() )
      {
	aKey = anIt.Key();
	if( myLastID == aKey )
	  aHilight = Standard_False;
	break;
      }
    }
  } 
  return aHilight;
}
