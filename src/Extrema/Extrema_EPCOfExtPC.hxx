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

#ifndef _Extrema_EPCOfExtPC_HeaderFile
#define _Extrema_EPCOfExtPC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Extrema_PCFOfEPCOfExtPC.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_TypeMismatch;
class Adaptor3d_Curve;
class Extrema_CurveTool;
class Extrema_POnCurv;
class gp_Pnt;
class gp_Vec;
class Extrema_PCFOfEPCOfExtPC;

class Extrema_EPCOfExtPC 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_EPCOfExtPC();
  
  //! It calculates all the distances.
  //! The function F(u)=distance(P,C(u)) has an extremum
  //! when g(u)=dF/du=0. The algorithm searchs all the
  //! zeros inside the definition range of the curve.
  //! NbU is used to locate the close points to
  //! find the zeros.
  //! Tol and TolU are used to decide to stop the
  //! iterations according to the following condition:
  //! if n is the number of iterations,
  //! abs(Un-Un-1) < TolU and abs(F(Un)-F(Un-1)) < Tol.
  Standard_EXPORT Extrema_EPCOfExtPC(const gp_Pnt& P, const Adaptor3d_Curve& C, const Standard_Integer NbU, const Standard_Real TolU, const Standard_Real TolF);
  
  //! It calculates all the distances.
  //! The function F(u)=distance(P,C(u)) has an extremum
  //! when g(u)=dF/du=0. The algorithm searchs all the
  //! zeros inside the definition range of the curve.
  //! NbU is used to locate the close points to
  //! find the zeros.
  //! Zeros are searched between umin and usup.
  //! Tol and TolU are used to decide to stop the
  //! iterations according to the following condition:
  //! if n is the number of iterations,
  //! abs(Un-Un-1) < TolU and abs(F(Un)-F(Un-1)) < Tol.
  Standard_EXPORT Extrema_EPCOfExtPC(const gp_Pnt& P, const Adaptor3d_Curve& C, const Standard_Integer NbU, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU, const Standard_Real TolF);
  
  //! sets the fields of the algorithm.
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Integer NbU, const Standard_Real TolU, const Standard_Real TolF);
  
  //! sets the fields of the algorithm.
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Integer NbU, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU, const Standard_Real TolF);
  
  //! sets the fields of the algorithm.
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C);
  
  //! sets the fields of the algorithm.
  Standard_EXPORT void Initialize (const Standard_Integer NbU, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU, const Standard_Real TolF);
  
  //! the algorithm is done with the point P.
  //! An exception is raised if the fields have not
  //! been initialized.
  Standard_EXPORT void Perform (const gp_Pnt& P);
  
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
  Standard_EXPORT const Extrema_POnCurv& Point (const Standard_Integer N) const;




protected:





private:



  Standard_Boolean myDone;
  Standard_Boolean myInit;
  Standard_Integer mynbsample;
  Standard_Real myumin;
  Standard_Real myusup;
  Standard_Real mytolu;
  Standard_Real mytolF;
  Extrema_PCFOfEPCOfExtPC myF;


};







#endif // _Extrema_EPCOfExtPC_HeaderFile
