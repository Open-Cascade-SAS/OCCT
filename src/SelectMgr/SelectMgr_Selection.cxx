// Created on: 1995-02-16
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
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

#include <SelectMgr_Selection.ixx>


//==================================================
// Function: Create 
// Purpose :
//==================================================
SelectMgr_Selection
::SelectMgr_Selection (const Standard_Integer IdMode):
myMode(IdMode)
{}

//==================================================
// Function: Destroy
// Purpose :
//==================================================
void SelectMgr_Selection::Destroy()
{
  for (SelectBasics_ListIteratorOfListOfSensitive anIt(myentities); anIt.More(); anIt.Next())
  {
    anIt.Value()->Set (NULL);
  }
}

//==================================================
// Function: ADD
// Purpose :
//==================================================
void SelectMgr_Selection
::Add (const Handle(SelectBasics_SensitiveEntity)& aprimitive)
{
  // if input is null:
  // in debug mode raise exception
  Standard_NullObject_Raise_if
    (aprimitive.IsNull(), "Null sensitive entity is added to the selection");
  // in release mode do not add
  if (!aprimitive.IsNull())
    myentities.Append(aprimitive);
}	

//==================================================
// Function: Clear 
// Purpose :
//==================================================
void SelectMgr_Selection
::Clear () {myentities.Clear();}

//==================================================
// Function: IsEmpty 
// Purpose :
//==================================================
Standard_Boolean SelectMgr_Selection
::IsEmpty() const
{
  return myentities.IsEmpty();
}






