// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

//      	-----------------------
// Version:	0.0
//Version	Date		Purpose
//		0.0	Nov 20 1997	Creation

#include <Standard_GUID.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelNode.hxx>
#include <TDF_LabelNodePtr.hxx>

#define ChildIDIterator_FindNext \
{ while( myItr.More() &&  !myItr.Value().FindAttribute(myID,myAtt)) myItr.Next(); }



//=======================================================================
//function : TDF_ChildIDIterator
//purpose  : 
//=======================================================================

TDF_ChildIDIterator::TDF_ChildIDIterator()
{}


//=======================================================================
//function : TDF_ChildIDIterator
//purpose  : 
//=======================================================================

TDF_ChildIDIterator::TDF_ChildIDIterator
(const TDF_Label& aLabel,
 const Standard_GUID& anID,
 const Standard_Boolean allLevels)
: myID(anID),
  myItr(aLabel,allLevels)
{ ChildIDIterator_FindNext; }


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void TDF_ChildIDIterator::Initialize
(const TDF_Label& aLabel,
 const Standard_GUID& anID,
 const Standard_Boolean allLevels)
{
  myID = anID;
  myItr.Initialize(aLabel,allLevels);
  myAtt.Nullify();
  ChildIDIterator_FindNext;
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TDF_ChildIDIterator::Next() 
{
  myAtt.Nullify();
  if (myItr.More()) {
    myItr.Next();
    ChildIDIterator_FindNext;
  }
}


//=======================================================================
//function : NextBrother
//purpose  : 
//=======================================================================

void TDF_ChildIDIterator::NextBrother() 
{
  myAtt.Nullify();
  if (myItr.More()) {
    myItr.NextBrother();
    while (myItr.More() && !myItr.Value().FindAttribute(myID,myAtt))
      myItr.NextBrother();
  }
}


