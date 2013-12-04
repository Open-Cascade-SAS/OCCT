// Created on: 1998-04-16
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Select3D_SensitiveGroup.ixx>
#include <Select3D_ListIteratorOfListOfSensitive.hxx>
#include <Precision.hxx>

//=======================================================================
//function : Creation
//purpose  : 
//=======================================================================
Select3D_SensitiveGroup::Select3D_SensitiveGroup(const Handle(SelectBasics_EntityOwner)& OwnerId,
                                                 const Standard_Boolean MatchAll):
Select3D_SensitiveEntity(OwnerId),
myMustMatchAll(MatchAll)
{
}

//=======================================================================
//function : Creation
//purpose  : 
//=======================================================================

Select3D_SensitiveGroup::Select3D_SensitiveGroup(const Handle(SelectBasics_EntityOwner)& OwnerId,
                                                 Select3D_ListOfSensitive& TheList, 
                                                 const Standard_Boolean MatchAll):
Select3D_SensitiveEntity(OwnerId),
myMustMatchAll(MatchAll)
{
  myList.Append(TheList);
}

//=======================================================================
//function : Add
//purpose  : No control of  entities inside 
//=======================================================================

void Select3D_SensitiveGroup::Add(Select3D_ListOfSensitive& LL) 
{myList.Append(LL);}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void Select3D_SensitiveGroup::Add(const Handle(Select3D_SensitiveEntity)& aSensitive) 
{
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next())
  {
    if(It.Value()==aSensitive) return;
  }
  myList.Append(aSensitive);
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void Select3D_SensitiveGroup::Remove(const Handle(Select3D_SensitiveEntity)& aSensitive) 
{
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next())
  {
    if(It.Value()==aSensitive)
    {
      myList.Remove(It);
      return;
    }
  }
}

//=======================================================================
//function : IsIn
//purpose  : 
//=======================================================================

Standard_Boolean Select3D_SensitiveGroup::IsIn(const Handle(Select3D_SensitiveEntity)& aSensitive) const
{
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next())
  {
    if(It.Value()==aSensitive)
      return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void Select3D_SensitiveGroup::Clear()
{myList.Clear();}

//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

void Select3D_SensitiveGroup::Project(const Handle(Select3D_Projector)& aProjector) 
{
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()) 
  {
    It.Value()->Project(aProjector);
  }
}

//=======================================================================
//function : Areas
//purpose  : 
//=======================================================================

void Select3D_SensitiveGroup::Areas(SelectBasics_ListOfBox2d& boxes) 
{
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()) 
  {
    It.Value()->Areas(boxes);
  }
}

//=======================================================================
//function : GetConnected
//purpose  : 
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveGroup::GetConnected(const TopLoc_Location& aLocation) 
{
  Handle(Select3D_SensitiveGroup) newgroup = new Select3D_SensitiveGroup(myOwnerId,myMustMatchAll);
  Select3D_ListOfSensitive LL;
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()) 
  {
    LL.Append(It.Value()->GetConnected(aLocation));
  }
  newgroup->Add(LL);
  return newgroup;
}

//=======================================================================
//function : SetLocation
//purpose  : 
//=======================================================================

void Select3D_SensitiveGroup::SetLocation(const TopLoc_Location& aLoc) 
{
  if(aLoc.IsIdentity()) return;

  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next())
  {
    It.Value()->SetLocation(aLoc);
  }

  if(HasLocation())
    if(aLoc == Location()) return;
  
  Select3D_SensitiveEntity::SetLocation(aLoc);
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()) 
  {
    if(It.Value()->HasLocation())
    {
      if(It.Value()->Location()!=aLoc) 
        It.Value()->SetLocation(It.Value()->Location()*aLoc);
    }
    else
      It.Value()->SetLocation(aLoc);
  }
}

//=======================================================================
//function : ResetLocation
//purpose  : 
//=======================================================================

void Select3D_SensitiveGroup::ResetLocation() 
{
 if(!HasLocation()) return;
 for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next())
 {
   if(It.Value()->HasLocation() && It.Value()->Location()!=Location())
     It.Value()->SetLocation(It.Value()->Location()*Location().Inverted());
   else
     It.Value()->ResetLocation();
 }
 Select3D_SensitiveEntity::ResetLocation();
}

//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean Select3D_SensitiveGroup::Matches (const SelectBasics_PickArgs& thePickArgs,
                                                   Standard_Real& theMatchDMin,
                                                   Standard_Real& theMatchDepth)
{
  theMatchDMin = RealLast();
  theMatchDepth = RealLast();
  Standard_Real aChildDMin, aChildDepth;
  Standard_Boolean isMatched = Standard_False;

  Select3D_ListIteratorOfListOfSensitive anIt (myList);
  for (; anIt.More(); anIt.Next())
  {
    Handle(SelectBasics_SensitiveEntity)& aChild = anIt.Value();
    if (!aChild->Matches (thePickArgs, aChildDMin, aChildDepth))
    {
      continue;
    }

    if (!isMatched)
    {
      theMatchDMin = aChildDMin;
      isMatched = Standard_True;
    }

    theMatchDepth = Min (aChildDepth, theMatchDepth);
  }

  return isMatched;
}

//=======================================================================
//function : Matches
//purpose  :  si on doit tout matcher, on ne repond oui que si toutes
//            les primitives repondent oui
//=======================================================================
Standard_Boolean Select3D_SensitiveGroup::Matches(const Standard_Real XMin, 
                                                  const Standard_Real YMin, 
                                                  const Standard_Real XMax, 
                                                  const Standard_Real YMax, 
                                                  const Standard_Real aTol) 
{
  Standard_Boolean result(Standard_True);
  
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next())
  {
    if(It.Value()->Matches(XMin,YMin,XMax,YMax,aTol))
    {
      if(!myMustMatchAll)
        return Standard_True;
    }
    // ca ne matches pas..
    else 
    {
      if(myMustMatchAll) 
        return Standard_False;
      else 
        result = Standard_False;
    }
  }
  return result;
}

//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean Select3D_SensitiveGroup::
Matches (const TColgp_Array1OfPnt2d& aPoly,
         const Bnd_Box2d& aBox,
         const Standard_Real aTol)
{ 
  Standard_Boolean result(Standard_True);
  
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next())
  {
    if(It.Value()->Matches(aPoly, aBox, aTol))
    {
      if(!myMustMatchAll) 
        return Standard_True;
    }
    else 
    {
      if(myMustMatchAll) 
        return Standard_False;
      else 
        result = Standard_False;
    }
  }
  return result;
}

//=======================================================================
//function : MaxBoxes
//purpose  : 
//=======================================================================

Standard_Integer Select3D_SensitiveGroup::MaxBoxes() const
{
  Standard_Integer nbboxes(0);
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next()){
    nbboxes+=It.Value()->MaxBoxes();
  }
  return nbboxes;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void Select3D_SensitiveGroup::Set 
  (const Handle(SelectBasics_EntityOwner)& TheOwnerId)
{ 
  Select3D_SensitiveEntity::Set(TheOwnerId);
  // set TheOwnerId for each element of sensitive group
  for(Select3D_ListIteratorOfListOfSensitive It(myList);It.More();It.Next())
    It.Value()->Set(TheOwnerId);
}
