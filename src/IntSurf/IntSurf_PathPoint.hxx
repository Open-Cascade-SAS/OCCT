// Created on: 1992-11-10
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IntSurf_PathPoint_HeaderFile
#define _IntSurf_PathPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <Standard_Boolean.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir2d.hxx>
#include <TColgp_HSequenceOfXY.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class StdFail_UndefinedDerivative;
class Standard_OutOfRange;
class gp_Pnt;
class gp_Vec;
class gp_Dir2d;



class IntSurf_PathPoint 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntSurf_PathPoint();
  
  Standard_EXPORT IntSurf_PathPoint(const gp_Pnt& P, const Standard_Real U, const Standard_Real V);
  
  Standard_EXPORT void SetValue (const gp_Pnt& P, const Standard_Real U, const Standard_Real V);
  
    void AddUV (const Standard_Real U, const Standard_Real V);
  
    void SetDirections (const gp_Vec& V, const gp_Dir2d& D);
  
    void SetTangency (const Standard_Boolean Tang);
  
    void SetPassing (const Standard_Boolean Pass);
  
    const gp_Pnt& Value() const;
  
    void Value2d (Standard_Real& U, Standard_Real& V) const;
  
    Standard_Boolean IsPassingPnt() const;
  
    Standard_Boolean IsTangent() const;
  
    const gp_Vec& Direction3d() const;
  
    const gp_Dir2d& Direction2d() const;
  
    Standard_Integer Multiplicity() const;
  
    void Parameters (const Standard_Integer Index, Standard_Real& U, Standard_Real& V) const;




protected:





private:



  gp_Pnt pt;
  Standard_Boolean ispass;
  Standard_Boolean istgt;
  gp_Vec vectg;
  gp_Dir2d dirtg;
  Handle(TColgp_HSequenceOfXY) sequv;


};


#include <IntSurf_PathPoint.lxx>





#endif // _IntSurf_PathPoint_HeaderFile
