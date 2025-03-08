// Created on: 1992-10-14
// Created by: Christophe MARION
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

#ifndef _HLRBRep_CInter_HeaderFile
#define _HLRBRep_CInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <IntCurve_IntConicConic.hxx>
#include <HLRBRep_TheIntConicCurveOfCInter.hxx>
#include <HLRBRep_TheIntPCurvePCurveOfCInter.hxx>
#include <IntRes2d_Intersection.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
class Standard_ConstructionError;
class HLRBRep_CurveTool;
class HLRBRep_TheProjPCurOfCInter;
class HLRBRep_TheCurveLocatorOfTheProjPCurOfCInter;
class HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter;
class HLRBRep_TheIntConicCurveOfCInter;
class HLRBRep_TheIntersectorOfTheIntConicCurveOfCInter;
class HLRBRep_IntConicCurveOfCInter;
class HLRBRep_TheIntPCurvePCurveOfCInter;
class HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter;
class HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter;
class HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter;
class IntRes2d_Domain;

class HLRBRep_CInter : public IntRes2d_Intersection
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  HLRBRep_CInter();

  //! Self Intersection of a curve
  HLRBRep_CInter(const Standard_Address& C, const Standard_Real TolConf, const Standard_Real Tol);

  //! Self Intersection of a curve with a domain.
  HLRBRep_CInter(const Standard_Address& C,
                 const IntRes2d_Domain&  D,
                 const Standard_Real     TolConf,
                 const Standard_Real     Tol);

  //! Intersection between 2 curves.
  HLRBRep_CInter(const Standard_Address& C1,
                 const Standard_Address& C2,
                 const Standard_Real     TolConf,
                 const Standard_Real     Tol);

  //! Intersection between 2 curves.
  HLRBRep_CInter(const Standard_Address& C1,
                 const IntRes2d_Domain&  D1,
                 const Standard_Address& C2,
                 const Standard_Real     TolConf,
                 const Standard_Real     Tol);

  //! Intersection between 2 curves.
  HLRBRep_CInter(const Standard_Address& C1,
                 const Standard_Address& C2,
                 const IntRes2d_Domain&  D2,
                 const Standard_Real     TolConf,
                 const Standard_Real     Tol);

  //! Intersection between 2 curves.
  HLRBRep_CInter(const Standard_Address& C1,
                 const IntRes2d_Domain&  D1,
                 const Standard_Address& C2,
                 const IntRes2d_Domain&  D2,
                 const Standard_Real     TolConf,
                 const Standard_Real     Tol);

  //! Intersection between 2 curves.
  Standard_EXPORT void Perform(const Standard_Address& C1,
                               const IntRes2d_Domain&  D1,
                               const Standard_Address& C2,
                               const IntRes2d_Domain&  D2,
                               const Standard_Real     TolConf,
                               const Standard_Real     Tol);

  //! Intersection between 2 curves.
  void Perform(const Standard_Address& C1,
               const Standard_Address& C2,
               const Standard_Real     TolConf,
               const Standard_Real     Tol);

  //! Intersection between 2 curves.
  Standard_EXPORT void Perform(const Standard_Address& C1,
                               const IntRes2d_Domain&  D1,
                               const Standard_Real     TolConf,
                               const Standard_Real     Tol);

  //! Intersection between 2 curves.
  Standard_EXPORT void Perform(const Standard_Address& C1,
                               const Standard_Real     TolConf,
                               const Standard_Real     Tol);

  //! Intersection between 2 curves.
  void Perform(const Standard_Address& C1,
               const IntRes2d_Domain&  D1,
               const Standard_Address& C2,
               const Standard_Real     TolConf,
               const Standard_Real     Tol);

  //! Intersection between 2 curves.
  void Perform(const Standard_Address& C1,
               const Standard_Address& C2,
               const IntRes2d_Domain&  D2,
               const Standard_Real     TolConf,
               const Standard_Real     Tol);

  //! Create a domain from a curve
  Standard_EXPORT IntRes2d_Domain ComputeDomain(const Standard_Address& C1,
                                                const Standard_Real     TolDomain) const;

  //! Set / get minimum number of points in polygon intersection.
  Standard_EXPORT void             SetMinNbSamples(const Standard_Integer theMinNbSamples);
  Standard_EXPORT Standard_Integer GetMinNbSamples() const;

protected:
private:
  //! Intersection between 2 curves.
  Standard_EXPORT void InternalPerform(const Standard_Address& C1,
                                       const IntRes2d_Domain&  D1,
                                       const Standard_Address& C2,
                                       const IntRes2d_Domain&  D2,
                                       const Standard_Real     TolConf,
                                       const Standard_Real     Tol,
                                       const Standard_Boolean  Composite);

  //! Part of InternalCompositePerform function
  Standard_EXPORT void InternalCompositePerform_noRecurs(const Standard_Integer      NbInterC1,
                                                         const Standard_Address&     C1,
                                                         const Standard_Integer      NumInterC1,
                                                         const TColStd_Array1OfReal& Tab1,
                                                         const IntRes2d_Domain&      D1,
                                                         const Standard_Integer      NbInterC2,
                                                         const Standard_Address&     C2,
                                                         const Standard_Integer      NumInterC2,
                                                         const TColStd_Array1OfReal& Tab2,
                                                         const IntRes2d_Domain&      D2,
                                                         const Standard_Real         TolConf,
                                                         const Standard_Real         Tol);

  //! Intersection between 2 curves.
  Standard_EXPORT void InternalCompositePerform(const Standard_Address&     C1,
                                                const IntRes2d_Domain&      D1,
                                                const Standard_Integer      N1,
                                                const Standard_Integer      NB1,
                                                const TColStd_Array1OfReal& Tab1,
                                                const Standard_Address&     C2,
                                                const IntRes2d_Domain&      D2,
                                                const Standard_Integer      N2,
                                                const Standard_Integer      NB2,
                                                const TColStd_Array1OfReal& Tab2,
                                                const Standard_Real         TolConf,
                                                const Standard_Real         Tol,
                                                const Standard_Boolean      Composite);

  Standard_Real                      param1inf;
  Standard_Real                      param1sup;
  Standard_Real                      param2inf;
  Standard_Real                      param2sup;
  IntCurve_IntConicConic             intconiconi;
  HLRBRep_TheIntConicCurveOfCInter   intconicurv;
  HLRBRep_TheIntPCurvePCurveOfCInter intcurvcurv;
};

#define TheCurve Standard_Address
#define TheCurve_hxx <Standard_Address.hxx>
#define TheCurveTool HLRBRep_CurveTool
#define TheCurveTool_hxx <HLRBRep_CurveTool.hxx>
#define IntCurve_TheProjPCur HLRBRep_TheProjPCurOfCInter
#define IntCurve_TheProjPCur_hxx <HLRBRep_TheProjPCurOfCInter.hxx>
#define IntCurve_TheCurveLocatorOfTheProjPCur HLRBRep_TheCurveLocatorOfTheProjPCurOfCInter
#define IntCurve_TheCurveLocatorOfTheProjPCur_hxx                                                  \
  <HLRBRep_TheCurveLocatorOfTheProjPCurOfCInter.hxx>
#define IntCurve_TheLocateExtPCOfTheProjPCur HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter
#define IntCurve_TheLocateExtPCOfTheProjPCur_hxx <HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter.hxx>
#define IntCurve_TheCurveLocatorOfTheProjPCur HLRBRep_TheCurveLocatorOfTheProjPCurOfCInter
#define IntCurve_TheCurveLocatorOfTheProjPCur_hxx                                                  \
  <HLRBRep_TheCurveLocatorOfTheProjPCurOfCInter.hxx>
#define IntCurve_TheLocateExtPCOfTheProjPCur HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter
#define IntCurve_TheLocateExtPCOfTheProjPCur_hxx <HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter.hxx>
#define IntCurve_TheIntConicCurve HLRBRep_TheIntConicCurveOfCInter
#define IntCurve_TheIntConicCurve_hxx <HLRBRep_TheIntConicCurveOfCInter.hxx>
#define IntCurve_TheIntersectorOfTheIntConicCurve HLRBRep_TheIntersectorOfTheIntConicCurveOfCInter
#define IntCurve_TheIntersectorOfTheIntConicCurve_hxx                                              \
  <HLRBRep_TheIntersectorOfTheIntConicCurveOfCInter.hxx>
#define IntCurve_TheIntersectorOfTheIntConicCurve HLRBRep_TheIntersectorOfTheIntConicCurveOfCInter
#define IntCurve_TheIntersectorOfTheIntConicCurve_hxx                                              \
  <HLRBRep_TheIntersectorOfTheIntConicCurveOfCInter.hxx>
#define IntCurve_IntConicCurve HLRBRep_IntConicCurveOfCInter
#define IntCurve_IntConicCurve_hxx <HLRBRep_IntConicCurveOfCInter.hxx>
#define IntCurve_TheIntPCurvePCurve HLRBRep_TheIntPCurvePCurveOfCInter
#define IntCurve_TheIntPCurvePCurve_hxx <HLRBRep_TheIntPCurvePCurveOfCInter.hxx>
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve_hxx                                              \
  <HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter.hxx>
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve                                         \
  HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve_hxx                                     \
  <HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter.hxx>
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve                                        \
  HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve_hxx                                    \
  <HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter.hxx>
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve_hxx                                              \
  <HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter.hxx>
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve                                         \
  HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve_hxx                                     \
  <HLRBRep_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfCInter.hxx>
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve                                        \
  HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve_hxx                                    \
  <HLRBRep_ExactIntersectionPointOfTheIntPCurvePCurveOfCInter.hxx>
#define IntCurve_IntCurveCurveGen HLRBRep_CInter
#define IntCurve_IntCurveCurveGen_hxx <HLRBRep_CInter.hxx>

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

#endif // _HLRBRep_CInter_HeaderFile
