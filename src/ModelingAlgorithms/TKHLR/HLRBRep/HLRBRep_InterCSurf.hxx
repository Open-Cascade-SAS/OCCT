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

#ifndef _HLRBRep_InterCSurf_HeaderFile
#define _HLRBRep_InterCSurf_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <IntCurveSurface_Intersection.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

class Bnd_BoundSortBox;
class Bnd_Box;
class gp_Circ;
class gp_Elips;
class gp_Hypr;
class gp_Lin;
class gp_Parab;
class HLRBRep_LineTool;
class HLRBRep_Surface;
class HLRBRep_SurfaceTool;
class HLRBRep_TheCSFunctionOfInterCSurf;
class HLRBRep_TheExactInterCSurf;
class HLRBRep_TheInterferenceOfInterCSurf;
class HLRBRep_ThePolygonOfInterCSurf;
class HLRBRep_ThePolygonToolOfInterCSurf;
class HLRBRep_ThePolyhedronOfInterCSurf;
class HLRBRep_ThePolyhedronToolOfInterCSurf;
class HLRBRep_TheQuadCurvExactInterCSurf;
class HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf;
class IntAna_IntConicQuad;

class HLRBRep_InterCSurf : public IntCurveSurface_Intersection
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty Constructor
  Standard_EXPORT HLRBRep_InterCSurf();

  //! Compute the Intersection between the curve and the
  //! surface
  Standard_EXPORT void Perform(const gp_Lin& theCurve, HLRBRep_Surface* theSurface);

  //! Compute the Intersection between the curve and
  //! the surface. The Curve is already sampled and
  //! its polygon : <thePolygon> is given.
  Standard_EXPORT void Perform(const gp_Lin&                         theCurve,
                               const HLRBRep_ThePolygonOfInterCSurf& thePolygon,
                               HLRBRep_Surface*                      theSurface);

  //! Compute the Intersection between the curve and
  //! the surface. The Curve is already sampled and
  //! its polygon : <thePolygon> is given. The Surface is
  //! also sampled and <thePolyhedron> is given.
  Standard_EXPORT void Perform(const gp_Lin&                            theCurve,
                               const HLRBRep_ThePolygonOfInterCSurf&    thePolygon,
                               HLRBRep_Surface*                         theSurface,
                               const HLRBRep_ThePolyhedronOfInterCSurf& thePolyhedron);

  //! Compute the Intersection between the curve and
  //! the surface. The Curve is already sampled and
  //! its polygon : <thePolygon> is given. The Surface is
  //! also sampled and <thePolyhedron> is given.
  Standard_EXPORT void Perform(const gp_Lin&                            theCurve,
                               const HLRBRep_ThePolygonOfInterCSurf&    thePolygon,
                               HLRBRep_Surface*                         theSurface,
                               const HLRBRep_ThePolyhedronOfInterCSurf& thePolyhedron,
                               Bnd_BoundSortBox&                        theBndBSB);

  //! Compute the Intersection between the curve and
  //! the surface. The Surface is already sampled and
  //! its polyhedron : <thePolyhedron> is given.
  Standard_EXPORT void Perform(const gp_Lin&                            theCurve,
                               HLRBRep_Surface*                         theSurface,
                               const HLRBRep_ThePolyhedronOfInterCSurf& thePolyhedron);

protected:
  //! Compute the Intersection between the curve and the
  //! surface
  Standard_EXPORT void Perform(const gp_Lin&    theCurve,
                               HLRBRep_Surface* theSurface,
                               const double     theU0,
                               const double     theV0,
                               const double     theU1,
                               const double     theV1);

  Standard_EXPORT void InternalPerformCurveQuadric(const gp_Lin&    theCurve,
                                                   HLRBRep_Surface* theSurface);

  Standard_EXPORT void InternalPerform(const gp_Lin&                            theCurve,
                                       const HLRBRep_ThePolygonOfInterCSurf&    thePolygon,
                                       HLRBRep_Surface*                         theSurface,
                                       const HLRBRep_ThePolyhedronOfInterCSurf& thePolyhedron,
                                       const double                             theU1,
                                       const double                             theV1,
                                       const double                             theU2,
                                       const double                             theV2);

  Standard_EXPORT void InternalPerform(const gp_Lin&                            theCurve,
                                       const HLRBRep_ThePolygonOfInterCSurf&    thePolygon,
                                       HLRBRep_Surface*                         theSurface,
                                       const HLRBRep_ThePolyhedronOfInterCSurf& thePolyhedron,
                                       const double                             theU1,
                                       const double                             theV1,
                                       const double                             theU2,
                                       const double                             theV2,
                                       Bnd_BoundSortBox&                        theBSB);

  Standard_EXPORT void InternalPerform(const gp_Lin&                         theCurve,
                                       const HLRBRep_ThePolygonOfInterCSurf& thePolygon,
                                       HLRBRep_Surface*                      theSurface,
                                       const double                          theU1,
                                       const double                          theV1,
                                       const double                          theU2,
                                       const double                          theV2);

  Standard_EXPORT void PerformConicSurf(const gp_Lin&    theLine,
                                        const gp_Lin&    theCurve,
                                        HLRBRep_Surface* theSurface,
                                        const double     theU1,
                                        const double     theV1,
                                        const double     theU2,
                                        const double     theV2);

  Standard_EXPORT void PerformConicSurf(const gp_Circ&   theCircle,
                                        const gp_Lin&    theCurve,
                                        HLRBRep_Surface* theSurface,
                                        const double     theU1,
                                        const double     theV1,
                                        const double     theU2,
                                        const double     theV2);

  Standard_EXPORT void PerformConicSurf(const gp_Elips&  theEllipse,
                                        const gp_Lin&    theCurve,
                                        HLRBRep_Surface* theSurface,
                                        const double     theU1,
                                        const double     theV1,
                                        const double     theU2,
                                        const double     theV2);

  Standard_EXPORT void PerformConicSurf(const gp_Parab&  theParab,
                                        const gp_Lin&    theCurve,
                                        HLRBRep_Surface* theSurface,
                                        const double     theU1,
                                        const double     theV1,
                                        const double     theU2,
                                        const double     theV2);

  Standard_EXPORT void PerformConicSurf(const gp_Hypr&   theHyper,
                                        const gp_Lin&    theCurve,
                                        HLRBRep_Surface* theSurface,
                                        const double     theU1,
                                        const double     theV1,
                                        const double     theU2,
                                        const double     theV2);

  Standard_EXPORT void AppendIntAna(const gp_Lin&              theCurve,
                                    HLRBRep_Surface*           theSurface,
                                    const IntAna_IntConicQuad& theInterAna);

  Standard_EXPORT void AppendPoint(const gp_Lin&    theCurve,
                                   const double     theW,
                                   HLRBRep_Surface* theSurface,
                                   const double     theU,
                                   const double     theV);

  Standard_EXPORT void AppendSegment(const gp_Lin&    theCurve,
                                     const double     theU0,
                                     const double     theU1,
                                     HLRBRep_Surface* theSurface);

private:
  Standard_EXPORT void DoSurface(HLRBRep_Surface*    theSurface,
                                 const double        theU0,
                                 const double        theU1,
                                 const double        theV0,
                                 const double        theV1,
                                 TColgp_Array2OfPnt& thePntsOnSurface,
                                 Bnd_Box&            theBoxSurface,
                                 double&             theGap);

  Standard_EXPORT void DoNewBounds(HLRBRep_Surface*            theSurface,
                                   const double                theU0,
                                   const double                theU1,
                                   const double                theV0,
                                   const double                theV1,
                                   const TColgp_Array2OfPnt&   thePntsOnSurface,
                                   const TColStd_Array1OfReal& theX,
                                   const TColStd_Array1OfReal& theY,
                                   const TColStd_Array1OfReal& theZ,
                                   TColStd_Array1OfReal&       theBounds);
};

#endif // _HLRBRep_InterCSurf_HeaderFile
