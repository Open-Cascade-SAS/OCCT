// Created on: 1993-06-07
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IntPatch_ArcFunction_HeaderFile
#define _IntPatch_ArcFunction_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntSurf_Quadric.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class Adaptor2d_HCurve2d;
class Adaptor3d_HSurface;
class IntSurf_Quadric;
class gp_Pnt;



class IntPatch_ArcFunction  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntPatch_ArcFunction();
  
    void SetQuadric (const IntSurf_Quadric& Q);
  
    void Set (const Handle(Adaptor2d_HCurve2d)& A);
  
    void Set (const Handle(Adaptor3d_HSurface)& S);
  
  Standard_EXPORT Standard_Boolean Value (const Standard_Real X, Standard_Real& F) Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean Derivative (const Standard_Real X, Standard_Real& D) Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean Values (const Standard_Real X, Standard_Real& F, Standard_Real& D) Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbSamples() const;
  
  Standard_EXPORT virtual Standard_Integer GetStateNumber() Standard_OVERRIDE;
  
    const gp_Pnt& Valpoint (const Standard_Integer Index) const;
  
    const IntSurf_Quadric& Quadric() const;
  
    const Handle(Adaptor2d_HCurve2d)& Arc() const;
  
    const Handle(Adaptor3d_HSurface)& Surface() const;

    //! Returns the point, which has been computed
    //! while the last calling Value() method
    const gp_Pnt& LastComputedPoint() const;



protected:





private:



  Handle(Adaptor2d_HCurve2d) myArc;
  Handle(Adaptor3d_HSurface) mySurf;
  IntSurf_Quadric myQuad;
  gp_Pnt ptsol;
  TColgp_SequenceOfPnt seqpt;


};


#include <IntPatch_ArcFunction.lxx>





#endif // _IntPatch_ArcFunction_HeaderFile
