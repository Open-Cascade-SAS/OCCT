// Created on: 1996-10-17
// Created by: Odile OLIVIER
// Copyright (c) 1996-1999 Matra Datavision
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

#include <Select3D_SensitiveWire.ixx>
#include <SelectBasics_BasicTool.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Select3D_SensitiveEntitySequence.hxx>
#include <SelectBasics_ListIteratorOfListOfBox2d.hxx>
#include <SelectBasics_ListOfBox2d.hxx>
#include <Precision.hxx>

#include <Bnd_Box2d.hxx>
#include <ElCLib.hxx>

//static Standard_Boolean debugmode=Standard_False;

//=====================================================
// Function : Create
// Purpose  :Constructor
//=====================================================

Select3D_SensitiveWire::
Select3D_SensitiveWire(const Handle(SelectBasics_EntityOwner)& OwnerId,
                       const Standard_Integer /*MaxRect*/):
Select3D_SensitiveEntity(OwnerId),
myDetectedIndex(-1)
{}

//=====================================================
// Function : Add
// Purpose  :
//=====================================================

void Select3D_SensitiveWire
::Add(const Handle(Select3D_SensitiveEntity)& aSensitive)
{
  if(!aSensitive.IsNull()) 
    mysensitive.Append(aSensitive); 
}

//=======================================================================
//function : SetLocation
//purpose  :
//=======================================================================

void Select3D_SensitiveWire::SetLocation(const TopLoc_Location& aLoc)
{

  // evitons les problemes...
  if(aLoc.IsIdentity()) return;

  if(HasLocation())
    if(aLoc == Location()) return;

  Select3D_SensitiveEntity::SetLocation(aLoc);
  for(Standard_Integer i=1;i<=mysensitive.Length();i++){
    if(mysensitive(i)->HasLocation()){
      if(mysensitive(i)->Location()!=aLoc) 
        mysensitive(i)->SetLocation(mysensitive(i)->Location()*aLoc);
    }
    else
      mysensitive(i)->SetLocation(aLoc);

  }
}

//=======================================================================
//function : ResetLocation
//purpose  :
//=======================================================================

void Select3D_SensitiveWire::ResetLocation()
{
  if(!HasLocation()) return;
  for(Standard_Integer i=1;i<=mysensitive.Length();i++){
    if(mysensitive(i)->HasLocation() && mysensitive(i)->Location()!=Location())
      mysensitive(i)->SetLocation(mysensitive(i)->Location()*Location().Inverted());
    else
      mysensitive(i)->ResetLocation();

  }
  Select3D_SensitiveEntity::ResetLocation();
}

//=====================================================
// Function : Project
// Purpose  :
//=====================================================
void Select3D_SensitiveWire::Project(const Handle(Select3D_Projector)& aProj)
{
  for (Standard_Integer aSensIt = 1; aSensIt <= mysensitive.Length(); aSensIt++)
  {
    mysensitive (aSensIt)->Project (aProj);
  }
}

//=====================================================
// Function : Areas
// Purpose  :
//=====================================================

void Select3D_SensitiveWire
::Areas(SelectBasics_ListOfBox2d& theareas)
{
  Bnd_Box2d BB; // en attendant un nouveau champ rob-18-jun-97
  SelectBasics_ListOfBox2d BidL;
  Standard_Integer i;
  for (i=1; i<=mysensitive.Length(); i++)
    mysensitive.Value(i)->Areas(BidL);

  for(SelectBasics_ListIteratorOfListOfBox2d it(BidL);it.More();it.Next())
    BB.Add(it.Value());

  theareas.Append(BB);
}

//=====================================================
// Function : Matches
// Purpose  :
//=====================================================

Standard_Boolean Select3D_SensitiveWire::Matches (const SelectBasics_PickArgs& thePickArgs,
                                                  Standard_Real& theMatchDMin,
                                                  Standard_Real& theMatchDepth)
{
  theMatchDMin = RealLast();
  theMatchDepth = RealLast();
  Standard_Real aSegDMin, aSegDepth;
  Standard_Boolean isMatched = Standard_False;
  myDetectedIndex = -1;

  for (Standard_Integer aSegIt = 1; aSegIt <= mysensitive.Length(); aSegIt++)
  {
    const Handle(SelectBasics_SensitiveEntity)& aSeg = mysensitive.Value (aSegIt);
    if (!aSeg->Matches (thePickArgs, aSegDMin, aSegDepth))
    {
      continue;
    }

    isMatched = Standard_True;
    if (aSegDMin > theMatchDMin)
    {
      continue;
    }

    myDetectedIndex = aSegIt;
    theMatchDMin    = aSegDMin;
    theMatchDepth   = aSegDepth;
  }

  return isMatched;
}

//=====================================================
// Function : Matches
// Purpose  :
//=====================================================

Standard_Boolean Select3D_SensitiveWire::
Matches (const Standard_Real XMin,
         const Standard_Real YMin,
         const Standard_Real XMax,
         const Standard_Real YMax,
         const Standard_Real aTol)
{
  Standard_Integer i;
  for (i=1; i<=mysensitive.Length(); i++) 
  {
    if (!(mysensitive.Value(i)->Matches(XMin,YMin,XMax,YMax,aTol)))
      return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : Matches
//purpose  :
//=======================================================================

Standard_Boolean Select3D_SensitiveWire::
Matches (const TColgp_Array1OfPnt2d& aPoly,
         const Bnd_Box2d& aBox,
         const Standard_Real aTol)
{
  Standard_Integer i;
  for (i=1; i<=mysensitive.Length(); i++) 
  {
    if (!(mysensitive.Value(i)->Matches(aPoly, aBox, aTol)))
      return Standard_False;
  }
  return Standard_True;
}


//=====================================================
// Function : MaxBoxes
// Purpose  :
//=====================================================

Standard_Integer Select3D_SensitiveWire::
MaxBoxes () const
{
  return 1;
}

//=======================================================================
//function : GetConnected
//purpose  :
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveWire::GetConnected(const TopLoc_Location& aLoc)
{
  Handle(Select3D_SensitiveWire) SWIR = new Select3D_SensitiveWire(myOwnerId);
  for(Standard_Integer i=1;i<=mysensitive.Length();i++)
    SWIR->Add(mysensitive(i)->GetConnected(aLoc));

  if(HasLocation())
    SWIR->SetLocation(Location()*aLoc);
  else
    SWIR->SetLocation(aLoc);
  return SWIR;
}


//=======================================================================
//function : Dump
//purpose  :
//=======================================================================

void Select3D_SensitiveWire::Dump(Standard_OStream& S,const Standard_Boolean FullDump) const
{
  S<<"\tSensitiveWire 3D :"<<endl;;
  if(HasLocation())
    S<<"\t\tExisting Location"<<endl;
  S<<"\t\tComposed Of "<<mysensitive.Length()<<" Sensitive Entities"<<endl;

  for(Standard_Integer i=1;i<= mysensitive.Length();i++){
    S<<"Sensitive #"<<i<<" : "<<endl;
    mysensitive(i)->Dump(S,FullDump);}

  S<<"\tEnd Of Sensitive Wire"<<endl;

}

//=======================================================================
//function : GetEdges
//purpose  : returns the sensitive edges stored in this wire
//=======================================================================

void Select3D_SensitiveWire::GetEdges( Select3D_SensitiveEntitySequence& theEdges )
{
  theEdges.Clear();
  theEdges.Append(mysensitive);
}

//=============================================================================
// Function : GetLastDetected
// Purpose  :
//=============================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveWire::GetLastDetected() const
{
  Handle(Select3D_SensitiveEntity) aRes;

  if ( myDetectedIndex >= 1 && myDetectedIndex <= mysensitive.Length() )
    aRes = mysensitive.Value( myDetectedIndex );

  return aRes;
}

//=======================================================================
//function : Set
//purpose  :
//=======================================================================

void Select3D_SensitiveWire::Set(const Handle_SelectBasics_EntityOwner &TheOwnerId) 
{
  Select3D_SensitiveEntity::Set(TheOwnerId);

  // Set TheOwnerId for each element of sensitive wire
  for (Standard_Integer i = 1; i <= mysensitive.Length(); ++i) 
  {
    mysensitive.Value(i)->Set(TheOwnerId);
  }
}
