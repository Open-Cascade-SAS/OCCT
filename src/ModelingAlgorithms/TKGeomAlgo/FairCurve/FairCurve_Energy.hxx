// Created on: 1996-03-06
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _FairCurve_Energy_HeaderFile
#define _FairCurve_Energy_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <gp_XY.hxx>
#include <math_MultipleVarFunctionWithHessian.hxx>
#include <Standard_Real.hxx>
class math_Matrix;
class gp_Pnt2d;

//! necessary methodes to compute the energy of an FairCurve.
class FairCurve_Energy : public math_MultipleVarFunctionWithHessian
{
public:
  DEFINE_STANDARD_ALLOC

  //! returns the number of variables of the energy.
  int NbVariables() const override;

  //! computes the values of the Energys E for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Value(const math_Vector& X, double& E) override;

  //! computes the gradient <G> of the energys for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Gradient(const math_Vector& X, math_Vector& G) override;

  //! computes the Energy <E> and the gradient <G> of the
  //! energy for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Values(const math_Vector& X, double& E, math_Vector& G) override;

  //! computes the Energy <E>, the gradient <G> and the
  //! Hessian <H> of the energy for the variable <X>.
  //! Returns True if the computation was done
  //! successfully, False otherwise.
  Standard_EXPORT bool Values(const math_Vector& X,
                                      double&            E,
                                      math_Vector&       G,
                                      math_Matrix&       H) override;

  //! compute the variables <X> which correspond with the field <MyPoles>
  Standard_EXPORT virtual bool Variable(math_Vector& X) const;

  //! return the poles
  const occ::handle<NCollection_HArray1<gp_Pnt2d>>& Poles() const;

protected:
  //! Angles correspond to the Ox axis
  //! ConstrOrder1(2) can be equal to 0, 1 or 2
  Standard_EXPORT FairCurve_Energy(const occ::handle<NCollection_HArray1<gp_Pnt2d>>& Poles,
                                   const int                                         ConstrOrder1,
                                   const int                                         ConstrOrder2,
                                   const bool   WithAuxValue = false,
                                   const double Angle1       = 0,
                                   const double Angle2       = 0,
                                   const int    Degree       = 2,
                                   const double Curvature1   = 0,
                                   const double Curvature2   = 0);

  //! It is use internally to make the Gradient Vector <G>
  Standard_EXPORT void Gradient1(const math_Vector& TheVector, math_Vector& G);

  //! It is use internally to make the Hessian Matrix <H>
  Standard_EXPORT void Hessian1(const math_Vector& TheVector, math_Matrix& H);

  //! compute the poles which correspond with the variable X
  Standard_EXPORT virtual void ComputePoles(const math_Vector& X);

  int Indice(const int i, const int j) const;

  //! compute the pole which depend of variables and G1 constraint
  Standard_EXPORT void ComputePolesG1(const int       Side,
                                      const double    Lambda,
                                      const gp_Pnt2d& P1,
                                      gp_Pnt2d&       P2) const;

  //! compute the pole which depend of variables and G2 constraint
  Standard_EXPORT void ComputePolesG2(const int       Side,
                                      const double    Lambda,
                                      const double    Rho,
                                      const gp_Pnt2d& P1,
                                      gp_Pnt2d&       P2) const;

  //! compute the energy (and derivatives) in intermediate format
  Standard_EXPORT virtual bool Compute(const int DerivativeOrder, math_Vector& Result) = 0;

  occ::handle<NCollection_HArray1<gp_Pnt2d>> MyPoles;
  int                                        MyContrOrder1;
  int                                        MyContrOrder2;
  bool                                       MyWithAuxValue;
  int                                        MyNbVar;

private:
  int                       MyNbValues;
  NCollection_Array1<gp_XY> MyLinearForm;
  NCollection_Array1<gp_XY> MyQuadForm;
  math_Vector               MyGradient;
  math_Vector               MyHessian;
};

#include <FairCurve_Energy.lxx>

#endif // _FairCurve_Energy_HeaderFile
