// Created on: 1995-12-08
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


#include <TopOpeBRepDS_CurveExplorer.ixx>
#include <TopOpeBRepDS_Curve.hxx>

#define MYDS (*((TopOpeBRepDS_DataStructure*)myDS))
static TopOpeBRepDS_Curve* CEX_PEMPTY = NULL;

//=======================================================================
//function : TopOpeBRepDS_CurveExplorer
//purpose  : 
//=======================================================================

TopOpeBRepDS_CurveExplorer::TopOpeBRepDS_CurveExplorer() 
: myIndex(1),
  myMax(0),
  myDS(NULL),
  myFound(Standard_False),
  myFindKeep(Standard_False)
{
}

//=======================================================================
//function : TopOpeBRepDS_CurveExplorer
//purpose  : 
//=======================================================================

TopOpeBRepDS_CurveExplorer::TopOpeBRepDS_CurveExplorer
(const TopOpeBRepDS_DataStructure& DS,
 const Standard_Boolean FindKeep)
{ 
  Init(DS,FindKeep);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRepDS_CurveExplorer::Init
(const TopOpeBRepDS_DataStructure& DS,
 const Standard_Boolean FindKeep)
{
  myDS = (TopOpeBRepDS_DataStructure*)&DS;
  myIndex = 1; 
  myMax = DS.NbCurves();
  myFindKeep = FindKeep;
  Find();
}


//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

void TopOpeBRepDS_CurveExplorer::Find()
{
  myFound = Standard_False;
  while (myIndex <= myMax) {
    if (myFindKeep) {
      myFound = IsCurveKeep(myIndex);
    }
    else {
      myFound = IsCurve(myIndex);
    }
    if (myFound) break;
    else myIndex++;
  }
}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_CurveExplorer::More() const
{
  return myFound;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TopOpeBRepDS_CurveExplorer::Next()
{
  myIndex++;
  Find();
}

//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

const TopOpeBRepDS_Curve& TopOpeBRepDS_CurveExplorer::Curve()const
{
  if ( myFound ) {
    return MYDS.Curve(myIndex);
  }
  else {
    if ( CEX_PEMPTY == NULL ) {
      CEX_PEMPTY = new TopOpeBRepDS_Curve();
    }
    return *CEX_PEMPTY;
  }
}

//=======================================================================
//function : IsCurve
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_CurveExplorer::IsCurve
   (const Standard_Integer I)const
{
  Standard_Boolean b = MYDS.myCurves.IsBound(I);
  return b;
}

//=======================================================================
//function : IsCurveKeep
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_CurveExplorer::IsCurveKeep
   (const Standard_Integer I)const
{
  Standard_Boolean b = MYDS.myCurves.IsBound(I);
  if (b) b = MYDS.Curve(I).Keep();
  return b;
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

const TopOpeBRepDS_Curve& TopOpeBRepDS_CurveExplorer::Curve
(const Standard_Integer I)const
{
  if ( IsCurve(I) ) {
    const TopOpeBRepDS_Curve& C = MYDS.Curve(I);
    return C;
  }

  if ( CEX_PEMPTY == NULL ) {
    CEX_PEMPTY = new TopOpeBRepDS_Curve();
  }
  return *CEX_PEMPTY;
}

//=======================================================================
//function : NbCurve
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepDS_CurveExplorer::NbCurve()
{
  myIndex = 1; myMax = MYDS.NbCurves();
  Find();
  Standard_Integer n = 0;
  for (; More(); Next() ) n++;
  return n;
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepDS_CurveExplorer::Index()const
{
  return myIndex;
}
