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

#ifndef _Geom2dInt_TheIntConicCurveOfGInter_HeaderFile
#define _Geom2dInt_TheIntConicCurveOfGInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <IntRes2d_Intersection.hxx>
class IntCurve_IConicTool;
class Adaptor2d_Curve2d;
class Geom2dInt_Geom2dCurveTool;
class Geom2dInt_TheProjPCurOfGInter;
class Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter;
class Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter;
class gp_Lin2d;
class IntRes2d_Domain;
class gp_Circ2d;
class gp_Elips2d;
class gp_Parab2d;
class gp_Hypr2d;

class Geom2dInt_TheIntConicCurveOfGInter : public IntRes2d_Intersection
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Geom2dInt_TheIntConicCurveOfGInter();

  //! Intersection between a line and a parametric curve.
  Geom2dInt_TheIntConicCurveOfGInter(const gp_Lin2d&          L,
                                     const IntRes2d_Domain&   D1,
                                     const Adaptor2d_Curve2d& PCurve,
                                     const IntRes2d_Domain&   D2,
                                     const Standard_Real      TolConf,
                                     const Standard_Real      Tol);

  //! Intersection between a line and a parametric curve.
  Standard_EXPORT Geom2dInt_TheIntConicCurveOfGInter(const gp_Circ2d&         C,
                                                     const IntRes2d_Domain&   D1,
                                                     const Adaptor2d_Curve2d& PCurve,
                                                     const IntRes2d_Domain&   D2,
                                                     const Standard_Real      TolConf,
                                                     const Standard_Real      Tol);

  //! Intersection between an ellipse and a parametric curve.
  Standard_EXPORT Geom2dInt_TheIntConicCurveOfGInter(const gp_Elips2d&        E,
                                                     const IntRes2d_Domain&   D1,
                                                     const Adaptor2d_Curve2d& PCurve,
                                                     const IntRes2d_Domain&   D2,
                                                     const Standard_Real      TolConf,
                                                     const Standard_Real      Tol);

  //! Intersection between a parabola and a parametric curve.
  Standard_EXPORT Geom2dInt_TheIntConicCurveOfGInter(const gp_Parab2d&        Prb,
                                                     const IntRes2d_Domain&   D1,
                                                     const Adaptor2d_Curve2d& PCurve,
                                                     const IntRes2d_Domain&   D2,
                                                     const Standard_Real      TolConf,
                                                     const Standard_Real      Tol);

  //! Intersection between the main branch of an hyperbola
  //! and a parametric curve.
  Standard_EXPORT Geom2dInt_TheIntConicCurveOfGInter(const gp_Hypr2d&         H,
                                                     const IntRes2d_Domain&   D1,
                                                     const Adaptor2d_Curve2d& PCurve,
                                                     const IntRes2d_Domain&   D2,
                                                     const Standard_Real      TolConf,
                                                     const Standard_Real      Tol);

  //! Intersection between a line and a parametric curve.
  void Perform(const gp_Lin2d&          L,
               const IntRes2d_Domain&   D1,
               const Adaptor2d_Curve2d& PCurve,
               const IntRes2d_Domain&   D2,
               const Standard_Real      TolConf,
               const Standard_Real      Tol);

  //! Intersection between a line and a parametric curve.
  void Perform(const gp_Circ2d&         C,
               const IntRes2d_Domain&   D1,
               const Adaptor2d_Curve2d& PCurve,
               const IntRes2d_Domain&   D2,
               const Standard_Real      TolConf,
               const Standard_Real      Tol);

  //! Intersection between an ellipse and a parametric curve.
  void Perform(const gp_Elips2d&        E,
               const IntRes2d_Domain&   D1,
               const Adaptor2d_Curve2d& PCurve,
               const IntRes2d_Domain&   D2,
               const Standard_Real      TolConf,
               const Standard_Real      Tol);

  //! Intersection between a parabola and a parametric curve.
  void Perform(const gp_Parab2d&        Prb,
               const IntRes2d_Domain&   D1,
               const Adaptor2d_Curve2d& PCurve,
               const IntRes2d_Domain&   D2,
               const Standard_Real      TolConf,
               const Standard_Real      Tol);

  //! Intersection between the main branch of an hyperbola
  //! and a parametric curve.
  void Perform(const gp_Hypr2d&         H,
               const IntRes2d_Domain&   D1,
               const Adaptor2d_Curve2d& PCurve,
               const IntRes2d_Domain&   D2,
               const Standard_Real      TolConf,
               const Standard_Real      Tol);

protected:
private:
  //! Intersection between a conic fom gp
  //! and a parametric curve.
  void Perform(const IntCurve_IConicTool& ICurve,
               const IntRes2d_Domain&     D1,
               const Adaptor2d_Curve2d&   PCurve,
               const IntRes2d_Domain&     D2,
               const Standard_Real        TolConf,
               const Standard_Real        Tol);
};

#define TheImpTool IntCurve_IConicTool
#define TheImpTool_hxx <IntCurve_IConicTool.hxx>
#define ThePCurve Adaptor2d_Curve2d
#define ThePCurve_hxx <Adaptor2d_Curve2d.hxx>
#define ThePCurveTool Geom2dInt_Geom2dCurveTool
#define ThePCurveTool_hxx <Geom2dInt_Geom2dCurveTool.hxx>
#define TheProjPCur Geom2dInt_TheProjPCurOfGInter
#define TheProjPCur_hxx <Geom2dInt_TheProjPCurOfGInter.hxx>
#define IntCurve_TheIntersector Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter
#define IntCurve_TheIntersector_hxx <Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter.hxx>
#define IntCurve_MyImpParToolOfTheIntersector                                                      \
  Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter
#define IntCurve_MyImpParToolOfTheIntersector_hxx                                                  \
  <Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter.hxx>
#define IntCurve_MyImpParToolOfTheIntersector                                                      \
  Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter
#define IntCurve_MyImpParToolOfTheIntersector_hxx                                                  \
  <Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter.hxx>
#define IntCurve_IntConicCurveGen Geom2dInt_TheIntConicCurveOfGInter
#define IntCurve_IntConicCurveGen_hxx <Geom2dInt_TheIntConicCurveOfGInter.hxx>

#include <IntCurve_IntConicCurveGen.lxx>

#undef TheImpTool
#undef TheImpTool_hxx
#undef ThePCurve
#undef ThePCurve_hxx
#undef ThePCurveTool
#undef ThePCurveTool_hxx
#undef TheProjPCur
#undef TheProjPCur_hxx
#undef IntCurve_TheIntersector
#undef IntCurve_TheIntersector_hxx
#undef IntCurve_MyImpParToolOfTheIntersector
#undef IntCurve_MyImpParToolOfTheIntersector_hxx
#undef IntCurve_MyImpParToolOfTheIntersector
#undef IntCurve_MyImpParToolOfTheIntersector_hxx
#undef IntCurve_IntConicCurveGen
#undef IntCurve_IntConicCurveGen_hxx

#endif // _Geom2dInt_TheIntConicCurveOfGInter_HeaderFile
