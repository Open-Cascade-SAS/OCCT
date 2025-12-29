// Created on: 1996-04-01
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

#ifndef _FairCurve_EnergyOfMVC_HeaderFile
#define _FairCurve_EnergyOfMVC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <FairCurve_BattenLaw.hxx>
#include <FairCurve_DistributionOfTension.hxx>
#include <FairCurve_DistributionOfSagging.hxx>
#include <FairCurve_DistributionOfJerk.hxx>
#include <FairCurve_AnalysisCode.hxx>
#include <FairCurve_Energy.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Pnt2d.hxx>
#include <math_Vector.hxx>

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! Energy Criterium to minimize in MinimalVariationCurve.
class FairCurve_EnergyOfMVC : public FairCurve_Energy
{
public:
  DEFINE_STANDARD_ALLOC

  //! Angles correspond to the Ox axis
  Standard_EXPORT FairCurve_EnergyOfMVC(const int                                         BSplOrder,
                                        const occ::handle<NCollection_HArray1<double>>&   FlatKnots,
                                        const occ::handle<NCollection_HArray1<gp_Pnt2d>>& Poles,
                                        const int                  ContrOrder1,
                                        const int                  ContrOrder2,
                                        const FairCurve_BattenLaw& Law,
                                        const double               PhysicalRatio,
                                        const double               LengthSliding,
                                        const bool                 FreeSliding = true,
                                        const double               Angle1      = 0,
                                        const double               Angle2      = 0,
                                        const double               Curvature1  = 0,
                                        const double               Curvature2  = 0);

  //! return the lengthSliding = P1P2 + Sliding
  double LengthSliding() const;

  //! return the status
  FairCurve_AnalysisCode Status() const;

  //! compute the variables <X> which correspond with the field <MyPoles>
  Standard_EXPORT bool Variable(math_Vector& X) const override;

protected:
  //! compute the poles which correspond with the variable X
  Standard_EXPORT void ComputePoles(const math_Vector& X) override;

  //! compute the energy in intermediate format
  Standard_EXPORT bool Compute(const int DerivativeOrder, math_Vector& Result) override;

private:
  double                          MyLengthSliding;
  double                          OriginalSliding;
  FairCurve_BattenLaw             MyBattenLaw;
  double                          MyPhysicalRatio;
  FairCurve_DistributionOfTension MyTension;
  FairCurve_DistributionOfSagging MySagging;
  FairCurve_DistributionOfJerk    MyJerk;
  FairCurve_AnalysisCode          MyStatus;
};

#include <FairCurve_EnergyOfMVC.lxx>

#endif // _FairCurve_EnergyOfMVC_HeaderFile
