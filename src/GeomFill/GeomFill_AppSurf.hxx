// Created on: 1993-09-28
// Created by: Bruno DUMORTIER
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

#ifndef _GeomFill_AppSurf_HeaderFile
#define _GeomFill_AppSurf_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColgp_SequenceOfArray1OfPnt2d.hxx>
#include <Approx_ParametrizationType.hxx>
#include <GeomAbs_Shape.hxx>
#include <AppBlend_Approx.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
class StdFail_NotDone;
class Standard_DomainError;
class Standard_OutOfRange;
class GeomFill_SectionGenerator;
class GeomFill_Line;

//! Approximate a  BSplineSurface passing by all the
//! curves described in the SectionGenerator

class GeomFill_AppSurf  : public AppBlend_Approx
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomFill_AppSurf();
  
  Standard_EXPORT GeomFill_AppSurf(const Standard_Integer Degmin, const Standard_Integer Degmax, const Standard_Real Tol3d, const Standard_Real Tol2d, const Standard_Integer NbIt, const Standard_Boolean KnownParameters = Standard_False);
  
  Standard_EXPORT void Init (const Standard_Integer Degmin, const Standard_Integer Degmax, const Standard_Real Tol3d, const Standard_Real Tol2d, const Standard_Integer NbIt, const Standard_Boolean KnownParameters = Standard_False);
  
  //! Define the type of parametrization used in the approximation
  Standard_EXPORT void SetParType (const Approx_ParametrizationType ParType);
  
  //! Define the Continuity used in the approximation
  Standard_EXPORT void SetContinuity (const GeomAbs_Shape C);
  
  //! define the Weights  associed to the criterium used in
  //! the  optimization.
  //!
  //! if Wi <= 0
  Standard_EXPORT void SetCriteriumWeight (const Standard_Real W1, const Standard_Real W2, const Standard_Real W3);
  
  //! returns the type of parametrization used in the approximation
  Standard_EXPORT Approx_ParametrizationType ParType() const;
  
  //! returns the Continuity used in the approximation
  Standard_EXPORT GeomAbs_Shape Continuity() const;
  
  //! returns the Weights (as percent) associed  to the criterium used in
  //! the  optimization.
  Standard_EXPORT void CriteriumWeight (Standard_Real& W1, Standard_Real& W2, Standard_Real& W3) const;
  
  Standard_EXPORT void Perform (const Handle(GeomFill_Line)& Lin, GeomFill_SectionGenerator& SecGen, const Standard_Boolean SpApprox = Standard_False);
  
  Standard_EXPORT void PerformSmoothing (const Handle(GeomFill_Line)& Lin, GeomFill_SectionGenerator& SecGen);
  
  Standard_EXPORT void Perform (const Handle(GeomFill_Line)& Lin, GeomFill_SectionGenerator& SecGen, const Standard_Integer NbMaxP);
  
    Standard_Boolean IsDone() const;
  
  Standard_EXPORT void SurfShape (Standard_Integer& UDegree, Standard_Integer& VDegree, Standard_Integer& NbUPoles, Standard_Integer& NbVPoles, Standard_Integer& NbUKnots, Standard_Integer& NbVKnots) const;
  
  Standard_EXPORT void Surface (TColgp_Array2OfPnt& TPoles, TColStd_Array2OfReal& TWeights, TColStd_Array1OfReal& TUKnots, TColStd_Array1OfReal& TVKnots, TColStd_Array1OfInteger& TUMults, TColStd_Array1OfInteger& TVMults) const;
  
    Standard_Integer UDegree() const;
  
    Standard_Integer VDegree() const;
  
    const TColgp_Array2OfPnt& SurfPoles() const;
  
    const TColStd_Array2OfReal& SurfWeights() const;
  
    const TColStd_Array1OfReal& SurfUKnots() const;
  
    const TColStd_Array1OfReal& SurfVKnots() const;
  
    const TColStd_Array1OfInteger& SurfUMults() const;
  
    const TColStd_Array1OfInteger& SurfVMults() const;
  
    Standard_Integer NbCurves2d() const;
  
  Standard_EXPORT void Curves2dShape (Standard_Integer& Degree, Standard_Integer& NbPoles, Standard_Integer& NbKnots) const;
  
  Standard_EXPORT void Curve2d (const Standard_Integer Index, TColgp_Array1OfPnt2d& TPoles, TColStd_Array1OfReal& TKnots, TColStd_Array1OfInteger& TMults) const;
  
    Standard_Integer Curves2dDegree() const;
  
    const TColgp_Array1OfPnt2d& Curve2dPoles (const Standard_Integer Index) const;
  
    const TColStd_Array1OfReal& Curves2dKnots() const;
  
    const TColStd_Array1OfInteger& Curves2dMults() const;
  
    void TolReached (Standard_Real& Tol3d, Standard_Real& Tol2d) const;
  
  Standard_EXPORT Standard_Real TolCurveOnSurf (const Standard_Integer Index) const;




protected:





private:

  
  Standard_EXPORT void InternalPerform (const Handle(GeomFill_Line)& Lin, GeomFill_SectionGenerator& SecGen, const Standard_Boolean SpApprox, const Standard_Boolean UseVariational);


  Standard_Boolean done;
  Standard_Integer dmin;
  Standard_Integer dmax;
  Standard_Real tol3d;
  Standard_Real tol2d;
  Standard_Integer nbit;
  Standard_Integer udeg;
  Standard_Integer vdeg;
  Standard_Boolean knownp;
  Handle(TColgp_HArray2OfPnt) tabPoles;
  Handle(TColStd_HArray2OfReal) tabWeights;
  Handle(TColStd_HArray1OfReal) tabUKnots;
  Handle(TColStd_HArray1OfReal) tabVKnots;
  Handle(TColStd_HArray1OfInteger) tabUMults;
  Handle(TColStd_HArray1OfInteger) tabVMults;
  TColgp_SequenceOfArray1OfPnt2d seqPoles2d;
  Standard_Real tol3dreached;
  Standard_Real tol2dreached;
  Approx_ParametrizationType paramtype;
  GeomAbs_Shape continuity;
  Standard_Real critweights[3];


};

#define TheSectionGenerator GeomFill_SectionGenerator
#define TheSectionGenerator_hxx <GeomFill_SectionGenerator.hxx>
#define Handle_TheLine Handle(GeomFill_Line)
#define TheLine GeomFill_Line
#define TheLine_hxx <GeomFill_Line.hxx>
#define AppBlend_AppSurf GeomFill_AppSurf
#define AppBlend_AppSurf_hxx <GeomFill_AppSurf.hxx>

#include <AppBlend_AppSurf.lxx>

#undef TheSectionGenerator
#undef TheSectionGenerator_hxx
#undef Handle_TheLine
#undef TheLine
#undef TheLine_hxx
#undef AppBlend_AppSurf
#undef AppBlend_AppSurf_hxx




#endif // _GeomFill_AppSurf_HeaderFile
