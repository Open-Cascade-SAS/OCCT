// Created on: 1991-01-08
// Created by: Didier Piffault
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Bnd_B2f_HeaderFile
#define _Bnd_B2f_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_ShortReal.hxx>
#include <Standard_Boolean.hxx>
#include <gp_XY.hxx>
#include <Standard_Real.hxx>
class gp_XY;
class gp_Pnt2d;
class gp_Trsf2d;
class gp_Ax2d;



class Bnd_B2f 
{
public:

  DEFINE_STANDARD_ALLOC

  
    Bnd_B2f();
  
    Bnd_B2f(const gp_XY& theCenter, const gp_XY& theHSize);
  
    Standard_Boolean IsVoid() const;
  
    void Clear();
  
  Standard_EXPORT void Add (const gp_XY& thePnt);
  
  Standard_EXPORT void Add (const gp_Pnt2d& thePnt);
  
    void Add (const Bnd_B2f& theBox);
  
    gp_XY CornerMin() const;
  
    gp_XY CornerMax() const;
  
    Standard_Real SquareExtent() const;
  
    void Enlarge (const Standard_Real theDiff);
  
  Standard_EXPORT Standard_Boolean Limit (const Bnd_B2f& theOtherBox);
  
  Standard_EXPORT Bnd_B2f Transformed (const gp_Trsf2d& theTrsf) const;
  
    Standard_Boolean IsOut (const gp_XY& thePnt) const;
  
  Standard_EXPORT Standard_Boolean IsOut (const gp_XY& theCenter, const Standard_Real theRadius, const Standard_Boolean isCircleHollow = Standard_False) const;
  
    Standard_Boolean IsOut (const Bnd_B2f& theOtherBox) const;
  
  Standard_EXPORT Standard_Boolean IsOut (const Bnd_B2f& theOtherBox, const gp_Trsf2d& theTrsf) const;
  
  Standard_EXPORT Standard_Boolean IsOut (const gp_Ax2d& theLine) const;
  
  Standard_EXPORT Standard_Boolean IsOut (const gp_XY& theP0, const gp_XY& theP1) const;
  
    Standard_Boolean IsIn (const Bnd_B2f& theBox) const;
  
  Standard_EXPORT Standard_Boolean IsIn (const Bnd_B2f& theBox, const gp_Trsf2d& theTrsf) const;
  
    void SetCenter (const gp_XY& theCenter);
  
    void SetHSize (const gp_XY& theHSize);




protected:



  Standard_ShortReal myCenter[2];
  Standard_ShortReal myHSize[2];


private:





};

#define RealType Standard_ShortReal
#define RealType_hxx <Standard_ShortReal.hxx>
#define Bnd_B2x Bnd_B2f
#define Bnd_B2x_hxx <Bnd_B2f.hxx>

#include <Bnd_B2x.lxx>

#undef RealType
#undef RealType_hxx
#undef Bnd_B2x
#undef Bnd_B2x_hxx




#endif // _Bnd_B2f_HeaderFile
