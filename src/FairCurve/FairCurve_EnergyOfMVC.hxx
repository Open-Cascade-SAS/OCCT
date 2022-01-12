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
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <math_Vector.hxx>

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! Energy Criterium to minimize in MinimalVariationCurve.
class FairCurve_EnergyOfMVC  : public FairCurve_Energy
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Angles corresspond to the Ox axis
  Standard_EXPORT FairCurve_EnergyOfMVC(const Standard_Integer BSplOrder, const Handle(TColStd_HArray1OfReal)& FlatKnots, const Handle(TColgp_HArray1OfPnt2d)& Poles, const Standard_Integer ContrOrder1, const Standard_Integer ContrOrder2, const FairCurve_BattenLaw& Law, const Standard_Real PhysicalRatio, const Standard_Real LengthSliding, const Standard_Boolean FreeSliding = Standard_True, const Standard_Real Angle1 = 0, const Standard_Real Angle2 = 0, const Standard_Real Curvature1 = 0, const Standard_Real Curvature2 = 0);
  
  //! return  the  lengthSliding = P1P2 + Sliding
    Standard_Real LengthSliding() const;
  
  //! return  the status
    FairCurve_AnalysisCode Status() const;
  
  //! compute the variables <X> which correspond with the field <MyPoles>
  Standard_EXPORT virtual Standard_Boolean Variable (math_Vector& X) const Standard_OVERRIDE;




protected:

  
  //! compute  the  poles which correspond with the variable X
  Standard_EXPORT virtual void ComputePoles (const math_Vector& X) Standard_OVERRIDE;
  
  //! compute the energy in intermediat format
  Standard_EXPORT virtual Standard_Boolean Compute (const Standard_Integer DerivativeOrder, math_Vector& Result) Standard_OVERRIDE;




private:



  Standard_Real MyLengthSliding;
  Standard_Real OriginalSliding;
  FairCurve_BattenLaw MyBattenLaw;
  Standard_Real MyPhysicalRatio;
  FairCurve_DistributionOfTension MyTension;
  FairCurve_DistributionOfSagging MySagging;
  FairCurve_DistributionOfJerk MyJerk;
  FairCurve_AnalysisCode MyStatus;


};


#include <FairCurve_EnergyOfMVC.lxx>





#endif // _FairCurve_EnergyOfMVC_HeaderFile
