// Created on: 1999-03-05
// Created by: Fabrice SERVANT
// Copyright (c) 1999 Matra Datavision
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

#ifndef _IntPolyh_Point_HeaderFile
#define _IntPolyh_Point_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Adaptor3d_HSurface;



class IntPolyh_Point 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntPolyh_Point();
  
  Standard_EXPORT IntPolyh_Point(const Standard_Real xx, const Standard_Real yy, const Standard_Real zz, const Standard_Real uu, const Standard_Real vv);
  
  Standard_EXPORT Standard_Real X() const;
  
  Standard_EXPORT Standard_Real Y() const;
  
  Standard_EXPORT Standard_Real Z() const;
  
  Standard_EXPORT Standard_Real U() const;
  
  Standard_EXPORT Standard_Real V() const;
  
  Standard_EXPORT Standard_Integer PartOfCommon() const;
  
  Standard_EXPORT void Equal (const IntPolyh_Point& Pt);
void operator = (const IntPolyh_Point& Pt)
{
  Equal(Pt);
}
  
  Standard_EXPORT void Set (const Standard_Real v1, const Standard_Real v2, const Standard_Real v3, const Standard_Real v4, const Standard_Real v5, const Standard_Integer II = 1);
  
  Standard_EXPORT void SetX (const Standard_Real v);
  
  Standard_EXPORT void SetY (const Standard_Real v);
  
  Standard_EXPORT void SetZ (const Standard_Real v);
  
  Standard_EXPORT void SetU (const Standard_Real v);
  
  Standard_EXPORT void SetV (const Standard_Real v);
  
  Standard_EXPORT void SetPartOfCommon (const Standard_Integer ii);
  
  Standard_EXPORT void Middle (const Handle(Adaptor3d_HSurface)& MySurface, const IntPolyh_Point& P1, const IntPolyh_Point& P2);
  
  Standard_EXPORT IntPolyh_Point Add (const IntPolyh_Point& P1) const;
IntPolyh_Point operator + (const IntPolyh_Point& P1) const
{
  return Add(P1);
}
  
  Standard_EXPORT IntPolyh_Point Sub (const IntPolyh_Point& P1) const;
IntPolyh_Point operator - (const IntPolyh_Point& P1) const
{
  return Sub(P1);
}
  
  Standard_EXPORT IntPolyh_Point Divide (const Standard_Real rr) const;
IntPolyh_Point operator / (const Standard_Real rr) const
{
  return Divide(rr);
}
  
  Standard_EXPORT IntPolyh_Point Multiplication (const Standard_Real rr) const;
IntPolyh_Point operator * (const Standard_Real rr) const
{
  return Multiplication(rr);
}
  
  Standard_EXPORT Standard_Real SquareModulus() const;
  
  Standard_EXPORT Standard_Real SquareDistance (const IntPolyh_Point& P2) const;
  
  Standard_EXPORT Standard_Real Dot (const IntPolyh_Point& P2) const;
  
  Standard_EXPORT void Cross (const IntPolyh_Point& P1, const IntPolyh_Point& P2);
  
  Standard_EXPORT void Dump() const;
  
  Standard_EXPORT void Dump (const Standard_Integer i) const;
  
  Standard_EXPORT void SetDegenerated (const Standard_Boolean theFlag);
  
  Standard_EXPORT Standard_Boolean Degenerated() const;




protected:





private:



  Standard_Real x;
  Standard_Real y;
  Standard_Real z;
  Standard_Real u;
  Standard_Real v;
  Standard_Integer POC;
  Standard_Boolean myDegenerated;


};







#endif // _IntPolyh_Point_HeaderFile
