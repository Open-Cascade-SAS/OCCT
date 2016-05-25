// Created on: 1992-06-04
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Geom2dInt_GInter_HeaderFile
#define _Geom2dInt_GInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <IntCurve_IntConicConic.hxx>
#include <Geom2dInt_TheIntConicCurveOfGInter.hxx>
#include <Geom2dInt_TheIntPCurvePCurveOfGInter.hxx>
#include <IntRes2d_Intersection.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
class Standard_ConstructionError;
class Adaptor2d_Curve2d;
class Geom2dInt_Geom2dCurveTool;
class Geom2dInt_TheProjPCurOfGInter;
class Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter;
class Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter;
class Geom2dInt_TheIntConicCurveOfGInter;
class Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter;
class Geom2dInt_IntConicCurveOfGInter;
class Geom2dInt_TheIntPCurvePCurveOfGInter;
class Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter;
class Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter;
class Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter;
class IntRes2d_Domain;



class Geom2dInt_GInter  : public IntRes2d_Intersection
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
    Geom2dInt_GInter();
  
  //! Self Intersection of a curve
    Geom2dInt_GInter(const Adaptor2d_Curve2d& C, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Self Intersection of a curve with a domain.
    Geom2dInt_GInter(const Adaptor2d_Curve2d& C, const IntRes2d_Domain& D, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between 2 curves.
    Geom2dInt_GInter(const Adaptor2d_Curve2d& C1, const Adaptor2d_Curve2d& C2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between 2 curves.
    Geom2dInt_GInter(const Adaptor2d_Curve2d& C1, const IntRes2d_Domain& D1, const Adaptor2d_Curve2d& C2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between 2 curves.
    Geom2dInt_GInter(const Adaptor2d_Curve2d& C1, const Adaptor2d_Curve2d& C2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between 2 curves.
    Geom2dInt_GInter(const Adaptor2d_Curve2d& C1, const IntRes2d_Domain& D1, const Adaptor2d_Curve2d& C2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between 2 curves.
  Standard_EXPORT void Perform (const Adaptor2d_Curve2d& C1, const IntRes2d_Domain& D1, const Adaptor2d_Curve2d& C2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between 2 curves.
    void Perform (const Adaptor2d_Curve2d& C1, const Adaptor2d_Curve2d& C2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between 2 curves.
  Standard_EXPORT void Perform (const Adaptor2d_Curve2d& C1, const IntRes2d_Domain& D1, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between 2 curves.
  Standard_EXPORT void Perform (const Adaptor2d_Curve2d& C1, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between 2 curves.
    void Perform (const Adaptor2d_Curve2d& C1, const IntRes2d_Domain& D1, const Adaptor2d_Curve2d& C2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between 2 curves.
    void Perform (const Adaptor2d_Curve2d& C1, const Adaptor2d_Curve2d& C2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Create a domain from a curve
  Standard_EXPORT IntRes2d_Domain ComputeDomain (const Adaptor2d_Curve2d& C1, const Standard_Real TolDomain) const;

  //! Set / get minimum number of points in polygon intersection.
  Standard_EXPORT void SetMinNbSamples (const Standard_Integer theMinNbSamples);
  Standard_EXPORT Standard_Integer GetMinNbSamples () const;


protected:





private:

  
  //! Intersection between 2 curves.
  Standard_EXPORT void InternalPerform (const Adaptor2d_Curve2d& C1, const IntRes2d_Domain& D1, const Adaptor2d_Curve2d& C2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Boolean Composite);
  
  //! Part of InternalCompositePerform function
  Standard_EXPORT void InternalCompositePerform_noRecurs (const Standard_Integer NbInterC1, const Adaptor2d_Curve2d& C1, const Standard_Integer NumInterC1, const TColStd_Array1OfReal& Tab1, const IntRes2d_Domain& D1, const Standard_Integer NbInterC2, const Adaptor2d_Curve2d& C2, const Standard_Integer NumInterC2, const TColStd_Array1OfReal& Tab2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol);
  
  //! Intersection between 2 curves.
  Standard_EXPORT void InternalCompositePerform (const Adaptor2d_Curve2d& C1, const IntRes2d_Domain& D1, const Standard_Integer N1, const Standard_Integer NB1, const TColStd_Array1OfReal& Tab1, const Adaptor2d_Curve2d& C2, const IntRes2d_Domain& D2, const Standard_Integer N2, const Standard_Integer NB2, const TColStd_Array1OfReal& Tab2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Boolean Composite);


  Standard_Real param1inf;
  Standard_Real param1sup;
  Standard_Real param2inf;
  Standard_Real param2sup;
  IntCurve_IntConicConic intconiconi;
  Geom2dInt_TheIntConicCurveOfGInter intconicurv;
  Geom2dInt_TheIntPCurvePCurveOfGInter intcurvcurv;


};

#define TheCurve Adaptor2d_Curve2d
#define TheCurve_hxx <Adaptor2d_Curve2d.hxx>
#define TheCurveTool Geom2dInt_Geom2dCurveTool
#define TheCurveTool_hxx <Geom2dInt_Geom2dCurveTool.hxx>
#define IntCurve_TheProjPCur Geom2dInt_TheProjPCurOfGInter
#define IntCurve_TheProjPCur_hxx <Geom2dInt_TheProjPCurOfGInter.hxx>
#define IntCurve_TheCurveLocatorOfTheProjPCur Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter
#define IntCurve_TheCurveLocatorOfTheProjPCur_hxx <Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter.hxx>
#define IntCurve_TheLocateExtPCOfTheProjPCur Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter
#define IntCurve_TheLocateExtPCOfTheProjPCur_hxx <Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter.hxx>
#define IntCurve_TheCurveLocatorOfTheProjPCur Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter
#define IntCurve_TheCurveLocatorOfTheProjPCur_hxx <Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter.hxx>
#define IntCurve_TheLocateExtPCOfTheProjPCur Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter
#define IntCurve_TheLocateExtPCOfTheProjPCur_hxx <Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter.hxx>
#define IntCurve_TheIntConicCurve Geom2dInt_TheIntConicCurveOfGInter
#define IntCurve_TheIntConicCurve_hxx <Geom2dInt_TheIntConicCurveOfGInter.hxx>
#define IntCurve_TheIntersectorOfTheIntConicCurve Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter
#define IntCurve_TheIntersectorOfTheIntConicCurve_hxx <Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter.hxx>
#define IntCurve_TheIntersectorOfTheIntConicCurve Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter
#define IntCurve_TheIntersectorOfTheIntConicCurve_hxx <Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter.hxx>
#define IntCurve_IntConicCurve Geom2dInt_IntConicCurveOfGInter
#define IntCurve_IntConicCurve_hxx <Geom2dInt_IntConicCurveOfGInter.hxx>
#define IntCurve_TheIntPCurvePCurve Geom2dInt_TheIntPCurvePCurveOfGInter
#define IntCurve_TheIntPCurvePCurve_hxx <Geom2dInt_TheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve_hxx <Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve_hxx <Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve_hxx <Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve_hxx <Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve_hxx <Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve_hxx <Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_IntCurveCurveGen Geom2dInt_GInter
#define IntCurve_IntCurveCurveGen_hxx <Geom2dInt_GInter.hxx>

#include <IntCurve_IntCurveCurveGen.lxx>

#undef TheCurve
#undef TheCurve_hxx
#undef TheCurveTool
#undef TheCurveTool_hxx
#undef IntCurve_TheProjPCur
#undef IntCurve_TheProjPCur_hxx
#undef IntCurve_TheCurveLocatorOfTheProjPCur
#undef IntCurve_TheCurveLocatorOfTheProjPCur_hxx
#undef IntCurve_TheLocateExtPCOfTheProjPCur
#undef IntCurve_TheLocateExtPCOfTheProjPCur_hxx
#undef IntCurve_TheCurveLocatorOfTheProjPCur
#undef IntCurve_TheCurveLocatorOfTheProjPCur_hxx
#undef IntCurve_TheLocateExtPCOfTheProjPCur
#undef IntCurve_TheLocateExtPCOfTheProjPCur_hxx
#undef IntCurve_TheIntConicCurve
#undef IntCurve_TheIntConicCurve_hxx
#undef IntCurve_TheIntersectorOfTheIntConicCurve
#undef IntCurve_TheIntersectorOfTheIntConicCurve_hxx
#undef IntCurve_TheIntersectorOfTheIntConicCurve
#undef IntCurve_TheIntersectorOfTheIntConicCurve_hxx
#undef IntCurve_IntConicCurve
#undef IntCurve_IntConicCurve_hxx
#undef IntCurve_TheIntPCurvePCurve
#undef IntCurve_TheIntPCurvePCurve_hxx
#undef IntCurve_ThePolygon2dOfTheIntPCurvePCurve
#undef IntCurve_ThePolygon2dOfTheIntPCurvePCurve_hxx
#undef IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve
#undef IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve_hxx
#undef IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve
#undef IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve_hxx
#undef IntCurve_ThePolygon2dOfTheIntPCurvePCurve
#undef IntCurve_ThePolygon2dOfTheIntPCurvePCurve_hxx
#undef IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve
#undef IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve_hxx
#undef IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve
#undef IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve_hxx
#undef IntCurve_IntCurveCurveGen
#undef IntCurve_IntCurveCurveGen_hxx




#endif // _Geom2dInt_GInter_HeaderFile
