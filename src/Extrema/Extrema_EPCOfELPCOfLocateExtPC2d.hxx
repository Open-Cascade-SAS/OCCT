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

#ifndef _Extrema_EPCOfELPCOfLocateExtPC2d_HeaderFile
#define _Extrema_EPCOfELPCOfLocateExtPC2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Extrema_PCFOfEPCOfELPCOfLocateExtPC2d.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_TypeMismatch;
class Adaptor2d_Curve2d;
class Extrema_Curve2dTool;
class Extrema_POnCurv2d;
class gp_Pnt2d;
class gp_Vec2d;
class Extrema_PCFOfEPCOfELPCOfLocateExtPC2d;

class Extrema_EPCOfELPCOfLocateExtPC2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_EPCOfELPCOfLocateExtPC2d();
  
  //! It calculates all the distances.
  //! The function F(u)=distance(P,C(u)) has an extremum
  //! when g(u)=dF/du=0. The algorithm searches all the
  //! zeros inside the definition range of the curve.
  //! NbU is used to locate the close points to
  //! find the zeros.
  //! Tol and TolU are used to decide to stop the
  //! iterations according to the following condition:
  //! if n is the number of iterations,
  //! abs(Un-Un-1) < TolU and abs(F(Un)-F(Un-1)) < Tol.
  Standard_EXPORT Extrema_EPCOfELPCOfLocateExtPC2d(const gp_Pnt2d& P, const Adaptor2d_Curve2d& C, const Standard_Integer NbU, const Standard_Real TolU, const Standard_Real TolF);
  
  //! It calculates all the distances.
  //! The function F(u)=distance(P,C(u)) has an extremum
  //! when g(u)=dF/du=0. The algorithm searches all the
  //! zeros inside the definition range of the curve.
  //! NbU is used to locate the close points to
  //! find the zeros.
  //! Zeros are searched between umin and usup.
  //! Tol and TolU are used to decide to stop the
  //! iterations according to the following condition:
  //! if n is the number of iterations,
  //! abs(Un-Un-1) < TolU and abs(F(Un)-F(Un-1)) < Tol.
  Standard_EXPORT Extrema_EPCOfELPCOfLocateExtPC2d(const gp_Pnt2d& P, const Adaptor2d_Curve2d& C, const Standard_Integer NbU, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU, const Standard_Real TolF);
  
  //! sets the fields of the algorithm.
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Integer NbU, const Standard_Real TolU, const Standard_Real TolF);
  
  //! sets the fields of the algorithm.
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Integer NbU, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU, const Standard_Real TolF);
  
  //! sets the fields of the algorithm.
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C);
  
  //! sets the fields of the algorithm.
  Standard_EXPORT void Initialize (const Standard_Integer NbU, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU, const Standard_Real TolF);
  
  //! the algorithm is done with the point P.
  //! An exception is raised if the fields have not
  //! been initialized.
  Standard_EXPORT void Perform (const gp_Pnt2d& P);
  
  //! True if the distances are found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of extremum distances.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns the value of the Nth extremum square distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  //! Returns True if the Nth extremum distance is a
  //! minimum.
  Standard_EXPORT Standard_Boolean IsMin (const Standard_Integer N) const;
  
  //! Returns the point of the Nth extremum distance.
  Standard_EXPORT const Extrema_POnCurv2d& Point (const Standard_Integer N) const;




protected:





private:



  Standard_Boolean myDone;
  Standard_Boolean myInit;
  Standard_Integer mynbsample;
  Standard_Real myumin;
  Standard_Real myusup;
  Standard_Real mytolu;
  Standard_Real mytolF;
  Extrema_PCFOfEPCOfELPCOfLocateExtPC2d myF;


};







#endif // _Extrema_EPCOfELPCOfLocateExtPC2d_HeaderFile
