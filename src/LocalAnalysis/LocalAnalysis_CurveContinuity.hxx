// Created on: 1996-08-09
// Created by: Herve LOUESSARD
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

#ifndef _LocalAnalysis_CurveContinuity_HeaderFile
#define _LocalAnalysis_CurveContinuity_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAbs_Shape.hxx>
#include <LocalAnalysis_StatusErrorType.hxx>
class Geom_Curve;
class GeomLProp_CLProps;



//! This class gives tools to check local continuity C0
//! C1 C2 G1 G2 between  two points situated on two curves
class LocalAnalysis_CurveContinuity 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! -u1 is the parameter of the point on Curv1
  //! -u2 is the  parameter of the point on  Curv2
  //! -Order is the required continuity:
  //! GeomAbs_C0    GeomAbs_C1  GeomAbs_C2
  //! GeomAbs_G1 GeomAbs_G2
  //!
  //! -EpsNul  is  used to  detect a  a vector with nul
  //! magnitude (in mm)
  //!
  //! -EpsC0 is used for C0  continuity to confuse two
  //! points (in mm)
  //!
  //! -EpsC1 is  an angular  tolerance in radians  used
  //! for C1 continuity  to compare the angle between
  //! the first derivatives
  //!
  //! -EpsC2 is an   angular tolerance in radians  used
  //! for C2  continuity to  compare the angle  between
  //! the second derivatives
  //!
  //! -EpsG1 is an  angular  tolerance in radians  used
  //! for G1  continuity to compare  the angle  between
  //! the tangents
  //!
  //! -EpsG2 is  an angular  tolerance in radians  used
  //! for  G2 continuity to  compare  the angle between
  //! the normals
  //!
  //! - percent  : percentage of  curvature variation (unitless)
  //! used for G2 continuity
  //!
  //! - Maxlen is the maximum length of Curv1 or Curv2 in
  //! meters used to detect nul curvature (in mm)
  //!
  //! the constructor computes the quantities  which are
  //! necessary to check the continuity in the following cases:
  //!
  //! case  C0
  //! --------
  //! - the distance between P1 and P2  with P1=Curv1 (u1)  and
  //! P2=Curv2(u2)
  //!
  //! case C1
  //! -------
  //!
  //! - the angle  between  the first derivatives
  //! dCurv1(u1)           dCurv2(u2)
  //! --------     and     ---------
  //! du                   du
  //!
  //! - the ratio   between  the magnitudes  of the first
  //! derivatives
  //!
  //! the angle value is between 0 and PI/2
  //!
  //! case  C2
  //! -------
  //! - the angle  between the second derivatives
  //! 2                   2
  //! d  Curv1(u1)       d Curv2(u2)
  //! ----------        ----------
  //! 2                   2
  //! du                  du
  //!
  //! the angle value is between 0 and PI/2
  //!
  //! - the ratio between the magnitudes of  the second
  //! derivatives
  //!
  //! case G1
  //! -------
  //! the angle between  the tangents at each point
  //!
  //! the angle value is between 0 and PI/2
  //!
  //! case G2
  //! -------
  //! -the angle between the normals at each point
  //!
  //! the angle value is between 0 and PI/2
  //!
  //! - the relative variation of curvature:
  //! |curvat1-curvat2|
  //! ------------------
  //! 1/2
  //! (curvat1*curvat2)
  //!
  //! where curvat1 is the curvature at the first point
  //! and curvat2 the curvature at the second point
  Standard_EXPORT LocalAnalysis_CurveContinuity(const Handle(Geom_Curve)& Curv1, const Standard_Real u1, const Handle(Geom_Curve)& Curv2, const Standard_Real u2, const GeomAbs_Shape Order, const Standard_Real EpsNul = 0.001, const Standard_Real EpsC0 = 0.001, const Standard_Real EpsC1 = 0.001, const Standard_Real EpsC2 = 0.001, const Standard_Real EpsG1 = 0.001, const Standard_Real EpsG2 = 0.001, const Standard_Real Percent = 0.01, const Standard_Real Maxlen = 10000);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT LocalAnalysis_StatusErrorType StatusError() const;
  
  Standard_EXPORT GeomAbs_Shape ContinuityStatus() const;
  
  Standard_EXPORT Standard_Real C0Value() const;
  
  Standard_EXPORT Standard_Real C1Angle() const;
  
  Standard_EXPORT Standard_Real C1Ratio() const;
  
  Standard_EXPORT Standard_Real C2Angle() const;
  
  Standard_EXPORT Standard_Real C2Ratio() const;
  
  Standard_EXPORT Standard_Real G1Angle() const;
  
  Standard_EXPORT Standard_Real G2Angle() const;
  
  Standard_EXPORT Standard_Real G2CurvatureVariation() const;
  
  Standard_EXPORT Standard_Boolean IsC0() const;
  
  Standard_EXPORT Standard_Boolean IsC1() const;
  
  Standard_EXPORT Standard_Boolean IsC2() const;
  
  Standard_EXPORT Standard_Boolean IsG1() const;
  
  Standard_EXPORT Standard_Boolean IsG2() const;




protected:





private:

  
  Standard_EXPORT void CurvC0 (GeomLProp_CLProps& Curv1, GeomLProp_CLProps& Curv2);
  
  Standard_EXPORT void CurvC1 (GeomLProp_CLProps& Curv1, GeomLProp_CLProps& Curv2);
  
  Standard_EXPORT void CurvC2 (GeomLProp_CLProps& Curv1, GeomLProp_CLProps& Curv2);
  
  Standard_EXPORT void CurvG1 (GeomLProp_CLProps& Curv1, GeomLProp_CLProps& Curv2);
  
  Standard_EXPORT void CurvG2 (GeomLProp_CLProps& Curv1, GeomLProp_CLProps& Curv2);


  Standard_Real myContC0;
  Standard_Real myContC1;
  Standard_Real myContC2;
  Standard_Real myContG1;
  Standard_Real myContG2;
  Standard_Real myCourbC1;
  Standard_Real myCourbC2;
  Standard_Real myG2Variation;
  Standard_Real myLambda1;
  Standard_Real myLambda2;
  GeomAbs_Shape myTypeCont;
  Standard_Real myepsnul;
  Standard_Real myepsC0;
  Standard_Real myepsC1;
  Standard_Real myepsC2;
  Standard_Real myepsG1;
  Standard_Real myepsG2;
  Standard_Real myMaxLon;
  Standard_Real myperce;
  Standard_Boolean myIsDone;
  LocalAnalysis_StatusErrorType myErrorStatus;


};







#endif // _LocalAnalysis_CurveContinuity_HeaderFile
