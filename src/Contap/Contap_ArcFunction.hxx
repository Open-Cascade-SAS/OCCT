// Created on: 1993-06-03
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

#ifndef _Contap_ArcFunction_HeaderFile
#define _Contap_ArcFunction_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Contap_TFunction.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <IntSurf_Quadric.hxx>
#include <math_FunctionWithDerivative.hxx>

class Contap_ArcFunction  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Contap_ArcFunction();
  
  Standard_EXPORT void Set (const Handle(Adaptor3d_Surface)& S);
  
    void Set (const gp_Dir& Direction);
  
    void Set (const gp_Dir& Direction, const Standard_Real Angle);
  
    void Set (const gp_Pnt& Eye);
  
    void Set (const gp_Pnt& Eye, const Standard_Real Angle);
  
    void Set (const Handle(Adaptor2d_Curve2d)& A);
  
  Standard_EXPORT Standard_Boolean Value (const Standard_Real X, Standard_Real& F) Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean Derivative (const Standard_Real X, Standard_Real& D) Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean Values (const Standard_Real X, Standard_Real& F, Standard_Real& D) Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbSamples() const;
  
  Standard_EXPORT virtual Standard_Integer GetStateNumber() Standard_OVERRIDE;
  
    const gp_Pnt& Valpoint (const Standard_Integer Index) const;
  
  Standard_EXPORT const IntSurf_Quadric& Quadric() const;

  //! Returns mySurf field
  const Handle(Adaptor3d_Surface)& Surface() const;

  //! Returns the point, which has been computed
  //! while the last calling Value() method
  const gp_Pnt& LastComputedPoint() const;


protected:





private:



  Handle(Adaptor2d_Curve2d) myArc;
  Handle(Adaptor3d_Surface) mySurf;
  Standard_Real myMean;
  Contap_TFunction myType;
  gp_Dir myDir;
  Standard_Real myCosAng;
  gp_Pnt myEye;
  gp_Pnt solpt;
  TColgp_SequenceOfPnt seqpt;
  IntSurf_Quadric myQuad;


};


#include <Contap_ArcFunction.lxx>





#endif // _Contap_ArcFunction_HeaderFile
