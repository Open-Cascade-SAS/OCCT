// Created on: 1995-12-08
// Created by: Jean Yves LEBEY
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

#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>

#define MYDS (*((TopOpeBRepDS_DataStructure*)myDS))
static TopOpeBRepDS_Curve* CEX_PEMPTY = NULL;

//=================================================================================================

TopOpeBRepDS_CurveExplorer::TopOpeBRepDS_CurveExplorer()
    : myIndex(1),
      myMax(0),
      myDS(NULL),
      myFound(Standard_False),
      myFindKeep(Standard_False)
{
}

//=================================================================================================

TopOpeBRepDS_CurveExplorer::TopOpeBRepDS_CurveExplorer(const TopOpeBRepDS_DataStructure& DS,
                                                       const Standard_Boolean            FindKeep)
{
  Init(DS, FindKeep);
}

//=================================================================================================

void TopOpeBRepDS_CurveExplorer::Init(const TopOpeBRepDS_DataStructure& DS,
                                      const Standard_Boolean            FindKeep)
{
  myDS       = (TopOpeBRepDS_DataStructure*)&DS;
  myIndex    = 1;
  myMax      = DS.NbCurves();
  myFindKeep = FindKeep;
  Find();
}

//=================================================================================================

void TopOpeBRepDS_CurveExplorer::Find()
{
  myFound = Standard_False;
  while (myIndex <= myMax)
  {
    if (myFindKeep)
    {
      myFound = IsCurveKeep(myIndex);
    }
    else
    {
      myFound = IsCurve(myIndex);
    }
    if (myFound)
      break;
    else
      myIndex++;
  }
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_CurveExplorer::More() const
{
  return myFound;
}

//=================================================================================================

void TopOpeBRepDS_CurveExplorer::Next()
{
  myIndex++;
  Find();
}

//=================================================================================================

const TopOpeBRepDS_Curve& TopOpeBRepDS_CurveExplorer::Curve() const
{
  if (myFound)
  {
    return MYDS.Curve(myIndex);
  }
  else
  {
    if (CEX_PEMPTY == NULL)
    {
      CEX_PEMPTY = new TopOpeBRepDS_Curve();
    }
    return *CEX_PEMPTY;
  }
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_CurveExplorer::IsCurve(const Standard_Integer I) const
{
  Standard_Boolean b = MYDS.myCurves.IsBound(I);
  return b;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_CurveExplorer::IsCurveKeep(const Standard_Integer I) const
{
  Standard_Boolean b = MYDS.myCurves.IsBound(I);
  if (b)
    b = MYDS.Curve(I).Keep();
  return b;
}

//=================================================================================================

const TopOpeBRepDS_Curve& TopOpeBRepDS_CurveExplorer::Curve(const Standard_Integer I) const
{
  if (IsCurve(I))
  {
    const TopOpeBRepDS_Curve& C = MYDS.Curve(I);
    return C;
  }

  if (CEX_PEMPTY == NULL)
  {
    CEX_PEMPTY = new TopOpeBRepDS_Curve();
  }
  return *CEX_PEMPTY;
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_CurveExplorer::NbCurve()
{
  myIndex = 1;
  myMax   = MYDS.NbCurves();
  Find();
  Standard_Integer n = 0;
  for (; More(); Next())
    n++;
  return n;
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_CurveExplorer::Index() const
{
  return myIndex;
}
