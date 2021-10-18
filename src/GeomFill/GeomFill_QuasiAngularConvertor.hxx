// Created on: 1997-08-06
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _GeomFill_QuasiAngularConvertor_HeaderFile
#define _GeomFill_QuasiAngularConvertor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Standard_Real.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfVec.hxx>
class gp_Pnt;
class gp_Vec;


//! To convert circular section in QuasiAngular Bezier
//! form
class GeomFill_QuasiAngularConvertor 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomFill_QuasiAngularConvertor();
  
  //! say if <me> is Initialized
  Standard_EXPORT Standard_Boolean Initialized() const;
  
  Standard_EXPORT void Init();
  
  Standard_EXPORT void Section (const gp_Pnt& FirstPnt, const gp_Pnt& Center, const gp_Vec& Dir, const Standard_Real Angle, TColgp_Array1OfPnt& Poles, TColStd_Array1OfReal& Weights);
  
  Standard_EXPORT void Section (const gp_Pnt& FirstPnt, const gp_Vec& DFirstPnt, const gp_Pnt& Center, const gp_Vec& DCenter, const gp_Vec& Dir, const gp_Vec& DDir, const Standard_Real Angle, const Standard_Real DAngle, TColgp_Array1OfPnt& Poles, TColgp_Array1OfVec& DPoles, TColStd_Array1OfReal& Weights, TColStd_Array1OfReal& DWeights);
  
  Standard_EXPORT void Section (const gp_Pnt& FirstPnt, const gp_Vec& DFirstPnt, const gp_Vec& D2FirstPnt, const gp_Pnt& Center, const gp_Vec& DCenter, const gp_Vec& D2Center, const gp_Vec& Dir, const gp_Vec& DDir, const gp_Vec& D2Dir, const Standard_Real Angle, const Standard_Real DAngle, const Standard_Real D2Angle, TColgp_Array1OfPnt& Poles, TColgp_Array1OfVec& DPoles, TColgp_Array1OfVec& D2Poles, TColStd_Array1OfReal& Weights, TColStd_Array1OfReal& DWeights, TColStd_Array1OfReal& D2Weights);




protected:





private:



  Standard_Boolean myinit;
  math_Matrix B;
  math_Vector Px;
  math_Vector Py;
  math_Vector W;
  math_Vector Vx;
  math_Vector Vy;
  math_Vector Vw;


};







#endif // _GeomFill_QuasiAngularConvertor_HeaderFile
