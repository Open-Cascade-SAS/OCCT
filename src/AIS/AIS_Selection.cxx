// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <AIS_InteractiveObject.hxx>
#include <AIS_Selection.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Standard_TypeMismatch.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfTransient.hxx>

#define MaxSizeOfResult 100000

//current selection (handle)
static Handle(AIS_Selection) theCurrentSelection;
static void AIS_Sel_CurrentSelection (Handle(AIS_Selection)& InputSel)     
{
  if(!InputSel.IsNull())
    theCurrentSelection = InputSel;
  else
    InputSel = theCurrentSelection;
}

static TColStd_SequenceOfTransient&  AIS_Sel_GetSelections()
{
  static TColStd_SequenceOfTransient Selections;
  return Selections;
}

//=======================================================================
//function : AIS_Selection
//purpose  : 
//=======================================================================
AIS_Selection::AIS_Selection(const Standard_CString aName) :
myName(TCollection_AsciiString(aName)),
myNb(0)
{
  myResultMap.ReSize( MaxSizeOfResult ); // for maximum performnace on medium selections ( < 100000 objects )
}

//=======================================================================
//function : CreateSelection
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Selection::CreateSelection(const Standard_CString aName)
{ 
  Handle(AIS_Selection) S = AIS_Selection::Selection(aName);
  if(!S.IsNull())
    return Standard_False;
  S = new AIS_Selection(aName);
  AIS_Sel_GetSelections().Prepend(S);
  AIS_Sel_CurrentSelection(S);
  return Standard_True;
}


//=======================================================================
//function : Selection
//purpose  : 
//=======================================================================
Handle(AIS_Selection) AIS_Selection::Selection(const Standard_CString aName) 
{
  Handle(AIS_Selection) S;
  if(AIS_Sel_GetSelections().IsEmpty()) return S;
  
  Handle(Standard_Transient) curobj;
  Handle(AIS_Selection) Sel;
//  Standard_Boolean found(Standard_False);
  for(Standard_Integer I =1; I<= AIS_Sel_GetSelections().Length();I++){
    curobj = AIS_Sel_GetSelections().Value(I);
    Sel = Handle(AIS_Selection)::DownCast (curobj);
    if(Sel->myName.IsEqual(aName))
      return Sel;
  }
  
  return S;
}

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Selection::Find(const Standard_CString aName) 
{
  Handle(AIS_Selection) S = AIS_Selection::Selection(aName);
  return !S.IsNull();
}

//=======================================================================
//function : SetCurrentSelection
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Selection::SetCurrentSelection (const Standard_CString aName) 
{  
  AIS_Selection::CreateSelection(aName); 


  Handle(AIS_Selection) anAISSelection = AIS_Selection::Selection(aName) ;
  AIS_Sel_CurrentSelection ( anAISSelection ) ;
  return Standard_True;
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
void AIS_Selection::Select() 
{
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);
  if(!S.IsNull()){
    S->myNb=0;
    S->myresult.Clear();
    S->myResultMap.Clear();
  }
}

//=======================================================================
//function : CurrentSelection
//purpose  : 
//=======================================================================
Handle(AIS_Selection) AIS_Selection::CurrentSelection() {
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);
  return S;
}
//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
AIS_SelectStatus AIS_Selection::Select(const Handle(Standard_Transient)& anObject) 
{
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);
  if(S.IsNull()) return AIS_SS_NotDone;
  Handle(AIS_InteractiveObject) anAISObj;
  Handle(SelectMgr_EntityOwner) owner = Handle(SelectMgr_EntityOwner)::DownCast( anObject );
  if ( owner.IsNull() )
    anAISObj = Handle(AIS_InteractiveObject)::DownCast( anObject );
  if ( S->myResultMap.IsBound( anObject ) ){
    AIS_NListTransient::Iterator aListIter = S->myResultMap.Find( anObject );
//skt-----------------------------------------------------------------
    if( S->myIterator == aListIter ) {
      if( S->myIterator.More() )
          S->myIterator.Next();
      else
          S->myIterator = AIS_NListTransient::Iterator();
    }
//--------------------------------------------------------------------
    // In the mode of advanced mesh selection only one owner is created
    // for all selection modes. It is necessary to check the current detected
    // entity and remove the owner from map only if the detected entity is
    // the same as previous selected (IsForcedHilight call)
    if( !anAISObj.IsNull() || ( !owner.IsNull() && !owner->IsForcedHilight() ) )
    {
      S->myresult.Remove( aListIter );
      S->myResultMap.UnBind( anObject );
    
      // update list iterator for next object in <myresult> list if any
      if ( aListIter.More() ){
	const Handle(Standard_Transient)& aNextObject = aListIter.Value();
	if ( S->myResultMap.IsBound( aNextObject ) )
	  S->myResultMap( aNextObject ) = aListIter;
	else
	  S->myResultMap.Bind( aNextObject, aListIter );
      }
      return AIS_SS_Removed;
    }
    else
      return AIS_SS_Added;
  }
  
  AIS_NListTransient::Iterator aListIter;
  S->myresult.Append( anObject, aListIter );
  S->myResultMap.Bind( anObject, aListIter );
  return AIS_SS_Added;
}

//=======================================================================
//function : AddSelect
//purpose  : Always add int the selection
//=======================================================================
AIS_SelectStatus AIS_Selection::AddSelect(const Handle(Standard_Transient)& anObject) 
{
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);
  if(S.IsNull()) return AIS_SS_NotDone;

  if ( S->myResultMap.IsBound( anObject ) )
    return AIS_SS_NotDone;
      
  AIS_NListTransient::Iterator aListIter;
  S->myresult.Append( anObject, aListIter );
  S->myResultMap.Bind( anObject, aListIter );
  return AIS_SS_Added;
}


//=======================================================================
//function : ClearAndSelect
//purpose  : 
//=======================================================================

void AIS_Selection::ClearAndSelect(const Handle(Standard_Transient)& anObject) {
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);
  if(S.IsNull()) return;
  
  Select();
  Select(anObject);

}


//=======================================================================
//function : Extent
//purpose  : 
//=======================================================================
Standard_Integer AIS_Selection::Extent() {
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);

  if (S.IsNull())
    return 0;

  return S->myresult.Extent();
}

//=======================================================================
//function : Single
//purpose  : 
//=======================================================================
Handle(Standard_Transient)  AIS_Selection::Single() 
{
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);

  if (S.IsNull())
    return Handle(Standard_Transient)();

  S->Init();
  return S->Value();
}
//=======================================================================
//function : IsSelected
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Selection::IsSelected(const Handle(Standard_Transient)& anObject)
{
  Handle(AIS_Selection) S;
  AIS_Sel_CurrentSelection (S);
  if(S.IsNull()) return Standard_False;
  return S->myResultMap.IsBound( anObject );
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer AIS_Selection::Index(const Standard_CString aName)
{
  Handle (Standard_Transient) curobj;
  for(Standard_Integer I =1; I<= AIS_Sel_GetSelections().Length();I++){
    curobj = AIS_Sel_GetSelections().Value(I);
    if(Handle(AIS_Selection)::DownCast (curobj)->myName.IsEqual(aName))
      return I;
  }
  return 0;
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void AIS_Selection::Remove(const Standard_CString aName) 
{
  Standard_Integer I = AIS_Selection::Index(aName);
  if(I!=0) {
    Handle(AIS_Selection) selection = Handle(AIS_Selection)::DownCast( AIS_Sel_GetSelections().Value(I) );
    Standard_Integer stored = selection->NbStored();
    if ( stored )
      selection->Select();
    AIS_Sel_GetSelections().Remove(I);
  }
}

// clean the static current selection handle
void AIS_Selection::ClearCurrentSelection()
{
    theCurrentSelection.Nullify();
}

