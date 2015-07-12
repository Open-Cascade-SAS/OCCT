// Created on: 1999-04-06
// Created by: Fabrice SERVANT
// Copyright (c) 1999-1999 Matra Datavision
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

// modified by Edward AGAPOV (eap) Thu Feb 14 2002 (occ139)
// Add Prepend(), replace array with sequence

#include <IntPolyh_SectionLine.hxx>
#include <IntPolyh_StartPoint.hxx>
#include <IntPolyh_Triangle.hxx>

#include <stdio.h>
//=======================================================================
//function : IntPolyh_SectionLine
//purpose  : 
//=======================================================================
IntPolyh_SectionLine::IntPolyh_SectionLine() /*: n(0),nbstartpoints(0),ptr(0)*/ { }

//=======================================================================
//function : IntPolyh_SectionLine
//purpose  : 
//=======================================================================

IntPolyh_SectionLine::IntPolyh_SectionLine(const Standard_Integer N)/* : nbstartpoints(0)*/{ 
  Init(N);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void IntPolyh_SectionLine::Init(const Standard_Integer /*N*/) { 
//   ptr = (void*) (new IntPolyh_StartPoint [N]);
//   n=N;
  if (!mySeqOfSPoints.Length()) IncrementNbStartPoints();
}

//=======================================================================
//function : GetN
//purpose  : 
//=======================================================================

Standard_Integer IntPolyh_SectionLine::GetN() const { 
  //return(n);
  return mySeqOfSPoints.Length();
}

//=======================================================================
//function : NbStartPoints
//purpose  : 
//=======================================================================

Standard_Integer IntPolyh_SectionLine::NbStartPoints() const { 
//  return(nbstartpoints); 
  return mySeqOfSPoints.Length() - 1;
}

//=======================================================================
//function : IncrementNbStartPoints
//purpose  : 
//=======================================================================

void IntPolyh_SectionLine::IncrementNbStartPoints() { 
//  nbstartpoints++;
  IntPolyh_StartPoint aSP;
  mySeqOfSPoints.Append(aSP);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const IntPolyh_StartPoint& IntPolyh_SectionLine::Value(const Standard_Integer Index) const
{ 
  return mySeqOfSPoints(Index+1);
}

//=======================================================================
//function : ChangeValue
//purpose  : 
//=======================================================================

IntPolyh_StartPoint& IntPolyh_SectionLine::ChangeValue(const Standard_Integer Index)
{
  return mySeqOfSPoints(Index+1);
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void IntPolyh_SectionLine::Destroy() { 
//   if(n) { 
//     if(ptr) { 
//       IntPolyh_StartPoint *ptrstpoint = (IntPolyh_StartPoint *)ptr;
//       delete [] ptrstpoint;
//       ptr=0;
//       n=0;
//     }
//   }
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

IntPolyh_SectionLine & IntPolyh_SectionLine::Copy(const IntPolyh_SectionLine& Other) { 
//   if(ptr==Other.ptr) return(*this);
//   Destroy();
//   n=Other.n;
//   ptr = (void *) (new IntPolyh_StartPoint[n]);
//   for(Standard_Integer i=0;i<=n;i++) { 
//     (*this)[i]=Other[i];
//   }
  mySeqOfSPoints = Other.mySeqOfSPoints;
  return(*this);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void IntPolyh_SectionLine::Dump() const{ 
  printf("\n SectionLine 0-> %d",/*nbstartpoints*/NbStartPoints()-1);
  for(Standard_Integer i=0;i<NbStartPoints();i++) { 
    //(*this)[i].Dump(i);
    Value(i).Dump(i);
//     const IntPolyh_StartPoint& SP = Value(i);
//     cout << "point P" << i << " " << SP.X() << " " << SP.Y() << " " << SP.Z() << endl;
  }
  printf("\n");
}

//=======================================================================
//function : Prepend
//purpose  : 
//=======================================================================

void IntPolyh_SectionLine::Prepend(const IntPolyh_StartPoint& SP)
{
  mySeqOfSPoints.Prepend(SP);
}



