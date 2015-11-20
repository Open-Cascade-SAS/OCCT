// Created on: 1991-02-26
// Created by: Isabelle GRIGNON
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

#ifndef _Extrema_CCLocFOfLocECC2d_HeaderFile
#define _Extrema_CCLocFOfLocECC2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <Standard_Real.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Extrema_SequenceOfPOnCurv2d.hxx>
#include <Standard_Integer.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>
class Standard_OutOfRange;
class Adaptor2d_Curve2d;
class Extrema_Curve2dTool;
class Extrema_POnCurv2d;
class gp_Pnt2d;
class gp_Vec2d;
class math_Matrix;



class Extrema_CCLocFOfLocECC2d  : public math_FunctionSetWithDerivatives
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_CCLocFOfLocECC2d(const Standard_Real thetol = 1.0e-10);
  
  Standard_EXPORT Extrema_CCLocFOfLocECC2d(const Adaptor2d_Curve2d& C1, const Adaptor2d_Curve2d& C2, const Standard_Real thetol = 1.0e-10);
  
  Standard_EXPORT void SetCurve (const Standard_Integer theRank, const Adaptor2d_Curve2d& C1);
  
    void SetTolerance (const Standard_Real theTol);
  
    virtual Standard_Integer NbVariables() const Standard_OVERRIDE;
  
    virtual Standard_Integer NbEquations() const Standard_OVERRIDE;
  
  //! Calculate Fi(U,V).
  Standard_EXPORT virtual Standard_Boolean Value (const math_Vector& UV, math_Vector& F) Standard_OVERRIDE;
  
  //! Calculate Fi'(U,V).
  Standard_EXPORT Standard_Boolean Derivatives (const math_Vector& UV, math_Matrix& DF) Standard_OVERRIDE;
  
  //! Calculate Fi(U,V) and Fi'(U,V).
  Standard_EXPORT Standard_Boolean Values (const math_Vector& UV, math_Vector& F, math_Matrix& DF) Standard_OVERRIDE;
  
  //! Save the found extremum.
  Standard_EXPORT virtual Standard_Integer GetStateNumber() Standard_OVERRIDE;
  
  //! Return the number of found extrema.
    Standard_Integer NbExt() const;
  
  //! Return the value of the Nth distance.
    Standard_Real SquareDistance (const Standard_Integer N) const;
  
  //! Return the points of the Nth extreme distance.
  Standard_EXPORT void Points (const Standard_Integer N, Extrema_POnCurv2d& P1, Extrema_POnCurv2d& P2) const;
  
  //! Returns a pointer to the curve specified in the constructor
  //! or in SetCurve() method.
    Standard_Address CurvePtr (const Standard_Integer theRank) const;
  
  //! Returns a tolerance specified in the constructor
  //! or in SetTolerance() method.
    Standard_Real Tolerance() const;
  
  //! Determines of boundaries of subinterval for find of root.
  Standard_EXPORT void SubIntervalInitialize (const math_Vector& theUfirst, const math_Vector& theUlast);
  
  //! Computes a Tol value. If 1st derivative of curve
  //! |D1|<Tol, it is considered D1=0.
  Standard_EXPORT Standard_Real SearchOfTolerance (const Standard_Address C);




protected:





private:



  Standard_Address myC1;
  Standard_Address myC2;
  Standard_Real myTol;
  Standard_Real myU;
  Standard_Real myV;
  gp_Pnt2d myP1;
  gp_Pnt2d myP2;
  gp_Vec2d myDu;
  gp_Vec2d myDv;
  TColStd_SequenceOfReal mySqDist;
  Extrema_SequenceOfPOnCurv2d myPoints;
  Standard_Real myTolC1;
  Standard_Real myTolC2;
  Standard_Integer myMaxDerivOrderC1;
  Standard_Integer myMaxDerivOrderC2;
  Standard_Real myUinfium;
  Standard_Real myUsupremum;
  Standard_Real myVinfium;
  Standard_Real myVsupremum;


};

#define Curve1 Adaptor2d_Curve2d
#define Curve1_hxx <Adaptor2d_Curve2d.hxx>
#define Tool1 Extrema_Curve2dTool
#define Tool1_hxx <Extrema_Curve2dTool.hxx>
#define Curve2 Adaptor2d_Curve2d
#define Curve2_hxx <Adaptor2d_Curve2d.hxx>
#define Tool2 Extrema_Curve2dTool
#define Tool2_hxx <Extrema_Curve2dTool.hxx>
#define POnC Extrema_POnCurv2d
#define POnC_hxx <Extrema_POnCurv2d.hxx>
#define Pnt gp_Pnt2d
#define Pnt_hxx <gp_Pnt2d.hxx>
#define Vec gp_Vec2d
#define Vec_hxx <gp_Vec2d.hxx>
#define Extrema_SeqPOnC Extrema_SequenceOfPOnCurv2d
#define Extrema_SeqPOnC_hxx <Extrema_SequenceOfPOnCurv2d.hxx>
#define Extrema_FuncExtCC Extrema_CCLocFOfLocECC2d
#define Extrema_FuncExtCC_hxx <Extrema_CCLocFOfLocECC2d.hxx>

#include <Extrema_FuncExtCC.lxx>

#undef Curve1
#undef Curve1_hxx
#undef Tool1
#undef Tool1_hxx
#undef Curve2
#undef Curve2_hxx
#undef Tool2
#undef Tool2_hxx
#undef POnC
#undef POnC_hxx
#undef Pnt
#undef Pnt_hxx
#undef Vec
#undef Vec_hxx
#undef Extrema_SeqPOnC
#undef Extrema_SeqPOnC_hxx
#undef Extrema_FuncExtCC
#undef Extrema_FuncExtCC_hxx




#endif // _Extrema_CCLocFOfLocECC2d_HeaderFile
