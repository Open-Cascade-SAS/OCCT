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

#ifndef _Bnd_B3f_HeaderFile
#define _Bnd_B3f_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_ShortReal.hxx>
#include <Standard_Boolean.hxx>
#include <gp_XYZ.hxx>
#include <Standard_Real.hxx>
class gp_XYZ;
class gp_Pnt;
class gp_Trsf;
class gp_Ax1;
class gp_Ax3;



class Bnd_B3f 
{
public:

  DEFINE_STANDARD_ALLOC

  
    Bnd_B3f();
  
    Bnd_B3f(const gp_XYZ& theCenter, const gp_XYZ& theHSize);
  
    Standard_Boolean IsVoid() const;
  
    void Clear();
  
  Standard_EXPORT void Add (const gp_XYZ& thePnt);
  
    void Add (const gp_Pnt& thePnt);
  
    void Add (const Bnd_B3f& theBox);
  
    gp_XYZ CornerMin() const;
  
    gp_XYZ CornerMax() const;
  
    Standard_Real SquareExtent() const;
  
    void Enlarge (const Standard_Real theDiff);
  
  Standard_EXPORT Standard_Boolean Limit (const Bnd_B3f& theOtherBox);
  
  Standard_EXPORT Bnd_B3f Transformed (const gp_Trsf& theTrsf) const;
  
    Standard_Boolean IsOut (const gp_XYZ& thePnt) const;
  
  Standard_EXPORT Standard_Boolean IsOut (const gp_XYZ& theCenter, const Standard_Real theRadius, const Standard_Boolean isSphereHollow = Standard_False) const;
  
    Standard_Boolean IsOut (const Bnd_B3f& theOtherBox) const;
  
  Standard_EXPORT Standard_Boolean IsOut (const Bnd_B3f& theOtherBox, const gp_Trsf& theTrsf) const;
  
  Standard_EXPORT Standard_Boolean IsOut (const gp_Ax1& theLine, const Standard_Boolean isRay = Standard_False, const Standard_Real theOverthickness = 0.0) const;
  
  Standard_EXPORT Standard_Boolean IsOut (const gp_Ax3& thePlane) const;
  
    Standard_Boolean IsIn (const Bnd_B3f& theBox) const;
  
  Standard_EXPORT Standard_Boolean IsIn (const Bnd_B3f& theBox, const gp_Trsf& theTrsf) const;
  
    void SetCenter (const gp_XYZ& theCenter);
  
    void SetHSize (const gp_XYZ& theHSize);




protected:



  Standard_ShortReal myCenter[3];
  Standard_ShortReal myHSize[3];


private:





};

#define RealType Standard_ShortReal
#define RealType_hxx <Standard_ShortReal.hxx>
#define Bnd_B3x Bnd_B3f
#define Bnd_B3x_hxx <Bnd_B3f.hxx>

#include <Bnd_B3x.lxx>

#undef RealType
#undef RealType_hxx
#undef Bnd_B3x
#undef Bnd_B3x_hxx




#endif // _Bnd_B3f_HeaderFile
