// Created on: 1997-03-17
// Created by: Prestataire Mary FABIEN
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


#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TestTopOpeTools_Mesure.hxx>

//=======================================================================
//function : TestTopOpeTools_Mesure
//purpose  : 
//=======================================================================
TestTopOpeTools_Mesure::TestTopOpeTools_Mesure()
:myPnts(new TColgp_HArray1OfPnt(1,100)),myNPnts(0){}

//=======================================================================
//function : TestTopOpeTools_Mesure
//purpose  : 
//=======================================================================

TestTopOpeTools_Mesure::TestTopOpeTools_Mesure
(const TCollection_AsciiString& Name)
: myName(Name),myPnts(new TColgp_HArray1OfPnt(1,100)),myNPnts(0)
{
  myNPnts = 1;
  myPnts->SetValue(myNPnts,gp_Pnt(0.,0.,0.));
}

//=======================================================================
//function : TestTopOpeTools_Mesure
//purpose  : 
//=======================================================================

TestTopOpeTools_Mesure::TestTopOpeTools_Mesure
(const Handle(TColgp_HArray1OfPnt)& P)
: myName(""),myPnts(new TColgp_HArray1OfPnt(1,100)),myNPnts(0)
{
  myPnts = P;
  myNPnts = myPnts->Length();
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void TestTopOpeTools_Mesure::Add(const Standard_Integer n,
				 const Standard_Real t)
{
  if (myPnts.IsNull()) return;
  if (myNPnts+1 > myPnts->Upper()) {
    Handle(TColgp_HArray1OfPnt) Ploc = new TColgp_HArray1OfPnt(1,10*myNPnts);
    for(Standard_Integer i = 1;i <= myNPnts;i++)
      Ploc->SetValue(i,myPnts->Value(i));
    myPnts->ChangeArray1() = Ploc->Array1();
  }
  myNPnts++;
  myPnts->SetValue(myNPnts,gp_Pnt((Standard_Real)n,t,0.));
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

const TCollection_AsciiString& TestTopOpeTools_Mesure::Name() const 
{
  return myName; 
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

void TestTopOpeTools_Mesure::SetName(const TCollection_AsciiString& Name) 
{
  myName = Name; 
}

//=======================================================================
//function : Pnts
//purpose  : 
//=======================================================================

const TColgp_Array1OfPnt& TestTopOpeTools_Mesure::Pnts() const
{
  return myPnts->Array1();
}

//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

const gp_Pnt& TestTopOpeTools_Mesure::Pnt(const Standard_Integer I) const
{
  return myPnts->Value(I);
}

//=======================================================================
//function : NPnts
//purpose  : 
//=======================================================================

Standard_Integer TestTopOpeTools_Mesure::NPnts() const
{
  return myNPnts; 
}

//=======================================================================
//function : Clear
//purpose : 
//=======================================================================

void TestTopOpeTools_Mesure::Clear()
{
  myNPnts = 0;
}
