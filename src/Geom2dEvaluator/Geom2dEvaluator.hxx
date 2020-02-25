// Created on: 1992-08-28
// Created by: Remi LEQUETTE
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

#ifndef _Geom2dEvaluator_HeaderFile
#define _Geom2dEvaluator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>

class gp_Pnt2d;
class gp_Vec2d;

//! The Geom2dEvaluator package provides  utilities for .
//! calculating value and derivatives of offset curve
//! using corresponding values of base curve

class Geom2dEvaluator
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Recalculate D1 values of base curve into D0 value of offset curve
  Standard_EXPORT static void CalculateD0(gp_Pnt2d& theValue,
                                          const gp_Vec2d& theD1, const Standard_Real theOffset);
  
  //! Recalculate D2 values of base curve into D1 values of offset curve
  Standard_EXPORT static  void CalculateD1(gp_Pnt2d& theValue,
                                           gp_Vec2d& theD1,
                                           const gp_Vec2d& theD2, const Standard_Real theOffset);

  
  //! Recalculate D3 values of base curve into D2 values of offset curve
  Standard_EXPORT static   void CalculateD2(gp_Pnt2d& theValue,
                                            gp_Vec2d& theD1,
                                            gp_Vec2d& theD2,
                                            const gp_Vec2d& theD3, const Standard_Boolean theIsDirChange, 
                                            const Standard_Real theOffset);

  
  //! Recalculate D3 values of base curve into D3 values of offset curve
  Standard_EXPORT static void CalculateD3(gp_Pnt2d& theValue,
                                          gp_Vec2d& theD1,
                                          gp_Vec2d& theD2,
                                          gp_Vec2d& theD3,
                                          const gp_Vec2d& theD4, const Standard_Boolean theIsDirChange,
                                          const Standard_Real theOffset); 

  
  //! Recalculate derivatives in the singular point
  //! Returns true if the direction of derivatives is changed
  Standard_EXPORT static Standard_Boolean AdjustDerivative(const Standard_Integer theMaxDerivative,
                                                           const Standard_Real theU,
                                                           gp_Vec2d& theD1,
                                                           gp_Vec2d& theD2,
                                                           gp_Vec2d& theD3,
                                                           gp_Vec2d& theD4);
  
  
protected:





private:

};


#endif // _Geom2dEvaluator_HeaderFile
