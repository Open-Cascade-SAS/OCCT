// Created on: 1995-05-05
// Created by: Christophe MARION
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


#ifndef No_Exception
//#define No_Exception
#endif
#include <HLRAlgo_PolyShellData.ixx>
#include <HLRAlgo_ListIteratorOfListOfBPoint.hxx>
#include <HLRAlgo_BiPoint.hxx>
#include <HLRAlgo_PolyData.hxx>

#define PntX1  ((Standard_Real*)Coordinates)[ 0]
#define PntY1  ((Standard_Real*)Coordinates)[ 1]
#define PntZ1  ((Standard_Real*)Coordinates)[ 2]
#define PntX2  ((Standard_Real*)Coordinates)[ 3]
#define PntY2  ((Standard_Real*)Coordinates)[ 4]
#define PntZ2  ((Standard_Real*)Coordinates)[ 5]
#define PntXP1 ((Standard_Real*)Coordinates)[ 6]
#define PntYP1 ((Standard_Real*)Coordinates)[ 7]
#define PntZP1 ((Standard_Real*)Coordinates)[ 8]
#define PntXP2 ((Standard_Real*)Coordinates)[ 9]
#define PntYP2 ((Standard_Real*)Coordinates)[10]
#define PntZP2 ((Standard_Real*)Coordinates)[11]

#define TotXMin ((Standard_Real*)TotMinMax)[0]
#define TotYMin ((Standard_Real*)TotMinMax)[1]
#define TotZMin ((Standard_Real*)TotMinMax)[2]
#define TotXMax ((Standard_Real*)TotMinMax)[3]
#define TotYMax ((Standard_Real*)TotMinMax)[4]
#define TotZMax ((Standard_Real*)TotMinMax)[5]

//=======================================================================
//function : HLRAlgo_PolyShellData
//purpose  : 
//=======================================================================

HLRAlgo_PolyShellData::
HLRAlgo_PolyShellData (const Standard_Integer nbFace)
:  myPolyg(0,nbFace)
{}

//=======================================================================
//function : UpdateGlobalMinMax
//purpose  : 
//=======================================================================

void 
HLRAlgo_PolyShellData::
UpdateGlobalMinMax(const Standard_Address TotMinMax)
{
  HLRAlgo_ListIteratorOfListOfBPoint it;
  
  for (it.Initialize(mySegList); it.More(); it.Next()) {      
    HLRAlgo_BiPoint& BP = it.Value();
    const Standard_Address Coordinates = BP.Coordinates();
    if (PntXP1 < PntXP2) {
      if      (TotXMin > PntXP1) TotXMin = PntXP1;
      else if (TotXMax < PntXP2) TotXMax = PntXP2;
    }
    else             {
      if      (TotXMin > PntXP2) TotXMin = PntXP2;
      else if (TotXMax < PntXP1) TotXMax = PntXP1;
    }
    if (PntYP1 < PntYP2) {
      if      (TotYMin > PntYP1) TotYMin = PntYP1;
      else if (TotYMax < PntYP2) TotYMax = PntYP2;
    }
    else             {
      if      (TotYMin > PntYP2) TotYMin = PntYP2;
      else if (TotYMax < PntYP1) TotYMax = PntYP1;
    }
    if (PntZP1 < PntZP2) {
      if      (TotZMin > PntZP1) TotZMin = PntZP1;
      else if (TotZMax < PntZP2) TotZMax = PntZP2;
    }
    else             {
      if      (TotZMin > PntZP2) TotZMin = PntZP2;
      else if (TotZMax < PntZP1) TotZMax = PntZP1;
    }
  }
  Standard_Integer nbFace      = myPolyg.Upper();
  Handle(HLRAlgo_PolyData)* pd;
  if(nbFace > 0) pd =  (Handle(HLRAlgo_PolyData)*)&(myPolyg.ChangeValue(1));
  
  for (Standard_Integer i = 1; i <= nbFace; i++) {
    (*pd)->UpdateGlobalMinMax(TotMinMax);
    pd++;
  }
}

//=======================================================================
//function : UpdateHiding
//purpose  : 
//=======================================================================

void HLRAlgo_PolyShellData::
UpdateHiding (const Standard_Integer nbHiding)
{
  if (nbHiding > 0)
    myHPolHi = new TColStd_HArray1OfTransient(1,nbHiding);
  else myHPolHi.Nullify();
}
