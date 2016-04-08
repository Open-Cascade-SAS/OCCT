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

IMPLEMENT_STANDARD_RTTIEXT(AIS_Selection,MMgt_TShared)

#define MaxSizeOfResult 100000

//=======================================================================
//function : AIS_Selection
//purpose  : 
//=======================================================================
AIS_Selection::AIS_Selection() :
myNb(0)
{
  myResultMap.ReSize( MaxSizeOfResult ); // for maximum performnace on medium selections ( < 100000 objects )
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
void AIS_Selection::Select() 
{
  myNb=0;
  myresult.Clear();
  myResultMap.Clear();
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
AIS_SelectStatus AIS_Selection::Select(const Handle(Standard_Transient)& anObject) 
{
  Handle(AIS_InteractiveObject) anAISObj;
  Handle(SelectMgr_EntityOwner) owner = Handle(SelectMgr_EntityOwner)::DownCast( anObject );
  if ( owner.IsNull() )
    anAISObj = Handle(AIS_InteractiveObject)::DownCast( anObject );
  if ( myResultMap.IsBound( anObject ) ){
    AIS_NListTransient::Iterator aListIter = myResultMap.Find( anObject );
//skt-----------------------------------------------------------------
    if( myIterator == aListIter ) {
      if( myIterator.More() )
          myIterator.Next();
      else
          myIterator = AIS_NListTransient::Iterator();
    }
//--------------------------------------------------------------------
    // In the mode of advanced mesh selection only one owner is created
    // for all selection modes. It is necessary to check the current detected
    // entity and remove the owner from map only if the detected entity is
    // the same as previous selected (IsForcedHilight call)
    if( !anAISObj.IsNull() || ( !owner.IsNull() && !owner->IsForcedHilight() ) )
    {
      myresult.Remove( aListIter );
      myResultMap.UnBind( anObject );
    
      // update list iterator for next object in <myresult> list if any
      if ( aListIter.More() ){
	const Handle(Standard_Transient)& aNextObject = aListIter.Value();
	if ( myResultMap.IsBound( aNextObject ) )
	  myResultMap( aNextObject ) = aListIter;
	else
	  myResultMap.Bind( aNextObject, aListIter );
      }
      return AIS_SS_Removed;
    }
    else
      return AIS_SS_Added;
  }
  
  AIS_NListTransient::Iterator aListIter;
  myresult.Append( anObject, aListIter );
  myResultMap.Bind( anObject, aListIter );
  return AIS_SS_Added;
}

//=======================================================================
//function : AddSelect
//purpose  : Always add int the selection
//=======================================================================
AIS_SelectStatus AIS_Selection::AddSelect(const Handle(Standard_Transient)& anObject) 
{
  if ( myResultMap.IsBound( anObject ) )
    return AIS_SS_NotDone;
      
  AIS_NListTransient::Iterator aListIter;
  myresult.Append( anObject, aListIter );
  myResultMap.Bind( anObject, aListIter );
  return AIS_SS_Added;
}


//=======================================================================
//function : ClearAndSelect
//purpose  : 
//=======================================================================

void AIS_Selection::ClearAndSelect(const Handle(Standard_Transient)& anObject)
{
  Select();
  Select(anObject);
}


//=======================================================================
//function : Extent
//purpose  : 
//=======================================================================
Standard_Integer AIS_Selection::Extent() const
{
  return myresult.Extent();
}

//=======================================================================
//function : Single
//purpose  : 
//=======================================================================
Handle(Standard_Transient)  AIS_Selection::Single() 
{
  Init();
  return Value();
}
//=======================================================================
//function : IsSelected
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Selection::IsSelected(const Handle(Standard_Transient)& anObject) const
{
  return myResultMap.IsBound( anObject );
}


