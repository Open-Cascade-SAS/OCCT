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

#ifndef _Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter_HeaderFile
#define _Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <IntRes2d_Intersection.hxx>
#include <TColStd_Array1OfReal.hxx>
class Standard_ConstructionError;
class IntCurve_IConicTool;
class Adaptor2d_Curve2d;
class Geom2dInt_Geom2dCurveTool;
class Geom2dInt_TheProjPCurOfGInter;
class Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter;
class IntRes2d_Domain;
class gp_Pnt2d;

class Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter : public IntRes2d_Intersection
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Standard_EXPORT Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter();

  //! Intersection between an implicit curve and
  //! a parametrised curve.
  //! The exception ConstructionError is raised if the domain
  //! of the parametrised curve does not verify HasFirstPoint
  //! and HasLastPoint return True.
  Standard_EXPORT Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter(
    const IntCurve_IConicTool& ITool,
    const IntRes2d_Domain&     Dom1,
    const Adaptor2d_Curve2d&   PCurve,
    const IntRes2d_Domain&     Dom2,
    const Standard_Real        TolConf,
    const Standard_Real        Tol);

  //! Intersection between an implicit curve and
  //! a parametrised curve.
  //! The exception ConstructionError is raised if the domain
  //! of the parametrised curve does not verify HasFirstPoint
  //! and HasLastPoint return True.
  Standard_EXPORT void Perform(const IntCurve_IConicTool& ITool,
                               const IntRes2d_Domain&     Dom1,
                               const Adaptor2d_Curve2d&   PCurve,
                               const IntRes2d_Domain&     Dom2,
                               const Standard_Real        TolConf,
                               const Standard_Real        Tol);

  Standard_EXPORT Standard_Real FindU(const Standard_Real        parameter,
                                      gp_Pnt2d&                  point,
                                      const Adaptor2d_Curve2d&   TheParCurev,
                                      const IntCurve_IConicTool& TheImpTool) const;

  Standard_EXPORT Standard_Real FindV(const Standard_Real        parameter,
                                      gp_Pnt2d&                  point,
                                      const IntCurve_IConicTool& TheImpTool,
                                      const Adaptor2d_Curve2d&   ParCurve,
                                      const IntRes2d_Domain&     TheParCurveDomain,
                                      const Standard_Real        V0,
                                      const Standard_Real        V1,
                                      const Standard_Real        Tolerance) const;

  Standard_EXPORT void And_Domaine_Objet1_Intersections(const IntCurve_IConicTool& TheImpTool,
                                                        const Adaptor2d_Curve2d&   TheParCurve,
                                                        const IntRes2d_Domain& TheImpCurveDomain,
                                                        const IntRes2d_Domain& TheParCurveDomain,
                                                        Standard_Integer&      NbResultats,
                                                        TColStd_Array1OfReal&  Inter2_And_Domain2,
                                                        TColStd_Array1OfReal&  Inter1,
                                                        TColStd_Array1OfReal&  Resultat1,
                                                        TColStd_Array1OfReal&  Resultat2,
                                                        const Standard_Real    EpsNul) const;

protected:
private:
};

#endif // _Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter_HeaderFile
