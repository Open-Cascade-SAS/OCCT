// Created on: 1993-04-07
// Created by: Laurent BUCHARD
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

#ifndef _IntCurveSurface_TheExactHInter_HeaderFile
#define _IntCurveSurface_TheExactHInter_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <IntCurveSurface_TheCSFunctionOfHInter.hxx>

class Adaptor3d_HSurfaceTool;
class IntCurveSurface_TheHCurveTool;
class IntCurveSurface_TheCSFunctionOfHInter;
class math_FunctionSetRoot;

class IntCurveSurface_TheExactHInter
{
public:
  DEFINE_STANDARD_ALLOC

  //! compute the solution point with the close point
  //! MarginCoef is the coefficient for extension of UV bounds.
  //! Ex., UFirst -= MarginCoef*(ULast-UFirst)
  Standard_EXPORT IntCurveSurface_TheExactHInter(const double                          U,
                                                 const double                          V,
                                                 const double                          W,
                                                 const IntCurveSurface_TheCSFunctionOfHInter& F,
                                                 const double TolTangency,
                                                 const double MarginCoef = 0.0);

  //! initialize the parameters to compute the solution
  Standard_EXPORT IntCurveSurface_TheExactHInter(const IntCurveSurface_TheCSFunctionOfHInter& F,
                                                 const double TolTangency);

  //! compute the solution
  //! it's possible to write to optimize:
  //! IntImp_IntCS inter(S1,C1,Toltangency)
  //! math_FunctionSetRoot rsnld(Inter.function())
  //! while ...{
  //! u=...
  //! v=...
  //! w=...
  //! inter.Perform(u,v,w,rsnld)
  //! }
  //! or
  //! IntImp_IntCS inter(Toltangency)
  //! inter.SetSurface(S);
  //! math_FunctionSetRoot rsnld(Inter.function())
  //! while ...{
  //! C=...
  //! inter.SetCurve(C);
  //! u=...
  //! v=...
  //! w=...
  //! inter.Perform(u,v,w,rsnld)
  //! }
  Standard_EXPORT void Perform(const double   U,
                               const double   V,
                               const double   W,
                               math_FunctionSetRoot& Rsnld,
                               const double   u0,
                               const double   v0,
                               const double   u1,
                               const double   v1,
                               const double   w0,
                               const double   w1);

  //! Returns TRUE if the creation completed without failure.
  Standard_EXPORT bool IsDone() const;

  Standard_EXPORT bool IsEmpty() const;

  //! returns the intersection point
  //! The exception NotDone is raised if IsDone is false.
  //! The exception DomainError is raised if IsEmpty is true.
  Standard_EXPORT const gp_Pnt& Point() const;

  Standard_EXPORT double ParameterOnCurve() const;

  Standard_EXPORT void ParameterOnSurface(double& U, double& V) const;

  //! return the math function which
  //! is used to compute the intersection
  Standard_EXPORT IntCurveSurface_TheCSFunctionOfHInter& Function();

private:
  bool                      done;
  bool                      empty;
  IntCurveSurface_TheCSFunctionOfHInter myFunction;
  double                         w;
  double                         u;
  double                         v;
  double                         tol;
};

#endif // _IntCurveSurface_TheExactHInter_HeaderFile
