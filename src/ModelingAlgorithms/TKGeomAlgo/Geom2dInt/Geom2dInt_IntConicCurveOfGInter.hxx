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

#ifndef _Geom2dInt_IntConicCurveOfGInter_HeaderFile
#define _Geom2dInt_IntConicCurveOfGInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <IntCurve_IntConicConic.hxx>
#include <Geom2dInt_TheIntConicCurveOfGInter.hxx>
#include <IntRes2d_Intersection.hxx>
class Standard_ConstructionError;
class IntCurve_IConicTool;
class Adaptor2d_Curve2d;
class Geom2dInt_Geom2dCurveTool;
class Geom2dInt_TheIntConicCurveOfGInter;
class gp_Lin2d;
class IntRes2d_Domain;
class gp_Circ2d;
class gp_Elips2d;
class gp_Parab2d;
class gp_Hypr2d;

class Geom2dInt_IntConicCurveOfGInter : public IntRes2d_Intersection
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Standard_EXPORT Geom2dInt_IntConicCurveOfGInter();

  //! Intersection between a line and a parametric curve.
  Standard_EXPORT Geom2dInt_IntConicCurveOfGInter(const gp_Lin2d&          L,
                                                  const IntRes2d_Domain&   D1,
                                                  const Adaptor2d_Curve2d& PCurve,
                                                  const IntRes2d_Domain&   D2,
                                                  const Standard_Real      TolConf,
                                                  const Standard_Real      Tol);

  //! Intersection between a line and a parametric curve.
  Standard_EXPORT Geom2dInt_IntConicCurveOfGInter(const gp_Circ2d&         C,
                                                  const IntRes2d_Domain&   D1,
                                                  const Adaptor2d_Curve2d& PCurve,
                                                  const IntRes2d_Domain&   D2,
                                                  const Standard_Real      TolConf,
                                                  const Standard_Real      Tol);

  //! Intersection between an ellipse and a parametric curve.
  Standard_EXPORT Geom2dInt_IntConicCurveOfGInter(const gp_Elips2d&        E,
                                                  const IntRes2d_Domain&   D1,
                                                  const Adaptor2d_Curve2d& PCurve,
                                                  const IntRes2d_Domain&   D2,
                                                  const Standard_Real      TolConf,
                                                  const Standard_Real      Tol);

  //! Intersection between a parabola and a parametric curve.
  Standard_EXPORT Geom2dInt_IntConicCurveOfGInter(const gp_Parab2d&        Prb,
                                                  const IntRes2d_Domain&   D1,
                                                  const Adaptor2d_Curve2d& PCurve,
                                                  const IntRes2d_Domain&   D2,
                                                  const Standard_Real      TolConf,
                                                  const Standard_Real      Tol);

  //! Intersection between the main branch of an hyperbola
  //! and a parametric curve.
  Standard_EXPORT Geom2dInt_IntConicCurveOfGInter(const gp_Hypr2d&         H,
                                                  const IntRes2d_Domain&   D1,
                                                  const Adaptor2d_Curve2d& PCurve,
                                                  const IntRes2d_Domain&   D2,
                                                  const Standard_Real      TolConf,
                                                  const Standard_Real      Tol);

  //! Intersection between a line and a parametric curve.
  Standard_EXPORT void Perform(const gp_Lin2d&          L,
                               const IntRes2d_Domain&   D1,
                               const Adaptor2d_Curve2d& PCurve,
                               const IntRes2d_Domain&   D2,
                               const Standard_Real      TolConf,
                               const Standard_Real      Tol);

  //! Intersection between a line and a parametric curve.
  Standard_EXPORT void Perform(const gp_Circ2d&         C,
                               const IntRes2d_Domain&   D1,
                               const Adaptor2d_Curve2d& PCurve,
                               const IntRes2d_Domain&   D2,
                               const Standard_Real      TolConf,
                               const Standard_Real      Tol);

  //! Intersection between an ellipse and a parametric curve.
  Standard_EXPORT void Perform(const gp_Elips2d&        E,
                               const IntRes2d_Domain&   D1,
                               const Adaptor2d_Curve2d& PCurve,
                               const IntRes2d_Domain&   D2,
                               const Standard_Real      TolConf,
                               const Standard_Real      Tol);

  //! Intersection between a parabola and a parametric curve.
  Standard_EXPORT void Perform(const gp_Parab2d&        Prb,
                               const IntRes2d_Domain&   D1,
                               const Adaptor2d_Curve2d& PCurve,
                               const IntRes2d_Domain&   D2,
                               const Standard_Real      TolConf,
                               const Standard_Real      Tol);

  //! Intersection between the main branch of an hyperbola
  //! and a parametric curve.
  Standard_EXPORT void Perform(const gp_Hypr2d&         H,
                               const IntRes2d_Domain&   D1,
                               const Adaptor2d_Curve2d& PCurve,
                               const IntRes2d_Domain&   D2,
                               const Standard_Real      TolConf,
                               const Standard_Real      Tol);

protected:
private:
  Standard_EXPORT void InternalPerform(const gp_Lin2d&          Lin1,
                                       const IntRes2d_Domain&   D1,
                                       const Adaptor2d_Curve2d& PCurve,
                                       const IntRes2d_Domain&   D2,
                                       const Standard_Real      TolConf,
                                       const Standard_Real      Tol,
                                       const Standard_Boolean   Composite);

  Standard_EXPORT void InternalPerform(const gp_Circ2d&         Circ1,
                                       const IntRes2d_Domain&   D1,
                                       const Adaptor2d_Curve2d& PCurve,
                                       const IntRes2d_Domain&   D2,
                                       const Standard_Real      TolConf,
                                       const Standard_Real      Tol,
                                       const Standard_Boolean   Composite);

  Standard_EXPORT void InternalPerform(const gp_Elips2d&        Eli1,
                                       const IntRes2d_Domain&   D1,
                                       const Adaptor2d_Curve2d& PCurve,
                                       const IntRes2d_Domain&   D2,
                                       const Standard_Real      TolConf,
                                       const Standard_Real      Tol,
                                       const Standard_Boolean   Composite);

  Standard_EXPORT void InternalPerform(const gp_Parab2d&        Prb1,
                                       const IntRes2d_Domain&   D1,
                                       const Adaptor2d_Curve2d& PCurve,
                                       const IntRes2d_Domain&   D2,
                                       const Standard_Real      TolConf,
                                       const Standard_Real      Tol,
                                       const Standard_Boolean   Composite);

  Standard_EXPORT void InternalPerform(const gp_Hypr2d&         Hpr1,
                                       const IntRes2d_Domain&   D1,
                                       const Adaptor2d_Curve2d& PCurve,
                                       const IntRes2d_Domain&   D2,
                                       const Standard_Real      TolConf,
                                       const Standard_Real      Tol,
                                       const Standard_Boolean   Composite);

  Standard_Real                      param1inf;
  Standard_Real                      param1sup;
  Standard_Real                      param2inf;
  Standard_Real                      param2sup;
  IntCurve_IntConicConic             intconiconi;
  Geom2dInt_TheIntConicCurveOfGInter intconicurv;
};

#endif // _Geom2dInt_IntConicCurveOfGInter_HeaderFile
