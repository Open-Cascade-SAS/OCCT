// Copyright: 	Matra-Datavision 1995
// File:	SelectMgr_Selection.cxx
// Created:	Thu Feb 16 10:20:29 1995
// Author:	Mister rmi
//		<rmi>



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






