// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _ShapeAnalysis_CanonicalRecognition_HeaderFile
#define _ShapeAnalysis_CanonicalRecognition_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomConvert_ConvType.hxx>
#include <TColStd_Array1OfReal.hxx>

class gp_Pln;
class gp_Cone;
class gp_Cylinder;
class gp_Sphere;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class Geom_Curve;
class Geom_Surface;

//! This class provides operators for analysis surfaces and curves of shapes
//! in order to find out more simple geometry entities, which could replace
//! existing complex (for example, BSpline) geometry objects with given tolerance.
class ShapeAnalysis_CanonicalRecognition
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT ShapeAnalysis_CanonicalRecognition();

  //! constructor with shape initialisation
  Standard_EXPORT ShapeAnalysis_CanonicalRecognition(const TopoDS_Shape& theShape);

  //! Sets shape
  Standard_EXPORT void SetShape(const TopoDS_Shape& theShape);

  //! Returns input shape
  const TopoDS_Shape& GetShape() const { return myShape; }

  //! Returns deviation between input geometry entity and analytical entity
  Standard_Real GetGap() const { return myGap; }

  //! Returns status of operation.
  //! Current meaning of possible values of status:
  //! -1 - algorithm is not initalazed by shape
  //!  0 - no errors
  //!  1 - error during any operation (usually - because of wrong input data)
  //! Any operation (calling any methods like IsPlane(...), ...) can be performed
  //! when current staue is equal 0.
  //! If after any operation status != 0, it is necessary to set it 0 by method ClearStatus()
  //! before calling other operation.
  Standard_Integer GetStatus() const { return myStatus; }

  //! Returns status to be equal 0.
  void ClearStatus() { myStatus = 0; }

  //! Returns true if the underlined surface can be represent by plane with tolerance theTol
  //! and sets in thePln the result plane.
  Standard_EXPORT Standard_Boolean IsPlane(const Standard_Real theTol, gp_Pln& thePln);

  //! Returns true if the underlined surface can be represent by cylindrical one with tolerance
  //! theTol and sets in theCyl the result cylinrical surface.
  Standard_EXPORT Standard_Boolean IsCylinder(const Standard_Real theTol, gp_Cylinder& theCyl);

  //! Returns true if the underlined surface can be represent by conical one with tolerance theTol
  //! and sets in theCone the result conical surface.
  Standard_EXPORT Standard_Boolean IsCone(const Standard_Real theTol, gp_Cone& theCone);

  //! Returns true if the underlined surface can be represent by spherical one with tolerance theTol
  //! and sets in theSphere the result spherical surface.
  Standard_EXPORT Standard_Boolean IsSphere(const Standard_Real theTol, gp_Sphere& theSphere);

  //! Returns true if the underlined curve can be represent by line with tolerance theTol
  //! and sets in theLin the result line.
  Standard_EXPORT Standard_Boolean IsLine(const Standard_Real theTol, gp_Lin& theLin);

  //! Returns true if the underlined curve can be represent by circle with tolerance theTol
  //! and sets in theCirc the result circle.
  Standard_EXPORT Standard_Boolean IsCircle(const Standard_Real theTol, gp_Circ& theCirc);

  //! Returns true if the underlined curve can be represent by ellipse with tolerance theTol
  //! and sets in theCirc the result ellipse.
  Standard_EXPORT Standard_Boolean IsEllipse(const Standard_Real theTol, gp_Elips& theElips);

private:
  Standard_Boolean IsElementarySurf(const GeomAbs_SurfaceType theTarget,
                                    const Standard_Real       theTol,
                                    gp_Ax3&                   thePos,
                                    TColStd_Array1OfReal&     theParams);

  Standard_Boolean IsConic(const GeomAbs_CurveType theTarget,
                           const Standard_Real     theTol,
                           gp_Ax2&                 thePos,
                           TColStd_Array1OfReal&   theParams);

  static Handle(Geom_Surface) GetSurface(const TopoDS_Face&         theFace,
                                         const Standard_Real        theTol,
                                         const GeomConvert_ConvType theType,
                                         const GeomAbs_SurfaceType  theTarget,
                                         Standard_Real&             theGap,
                                         Standard_Integer&          theStatus);

  static Handle(Geom_Surface) GetSurface(const TopoDS_Shell&        theShell,
                                         const Standard_Real        theTol,
                                         const GeomConvert_ConvType theType,
                                         const GeomAbs_SurfaceType  theTarget,
                                         Standard_Real&             theGap,
                                         Standard_Integer&          theStatus);

  static Handle(Geom_Surface) GetSurface(const TopoDS_Edge&         theEdge,
                                         const Standard_Real        theTol,
                                         const GeomConvert_ConvType theType,
                                         const GeomAbs_SurfaceType  theTarget,
                                         gp_Ax3&                    thePos,
                                         TColStd_Array1OfReal&      theParams,
                                         Standard_Real&             theGap,
                                         Standard_Integer&          theStatus);

  static Handle(Geom_Surface) GetSurface(const TopoDS_Wire&         theWire,
                                         const Standard_Real        theTol,
                                         const GeomConvert_ConvType theType,
                                         const GeomAbs_SurfaceType  theTarget,
                                         gp_Ax3&                    thePos,
                                         TColStd_Array1OfReal&      theParams,
                                         Standard_Real&             theGap,
                                         Standard_Integer&          theStatus);

  static Handle(Geom_Curve) GetCurve(const TopoDS_Edge&         theEdge,
                                     const Standard_Real        theTol,
                                     const GeomConvert_ConvType theType,
                                     const GeomAbs_CurveType    theTarget,
                                     Standard_Real&             theGap,
                                     Standard_Integer&          theStatus);

  static Standard_Boolean GetSurfaceByLS(const TopoDS_Wire&        theWire,
                                         const Standard_Real       theTol,
                                         const GeomAbs_SurfaceType theTarget,
                                         gp_Ax3&                   thePos,
                                         TColStd_Array1OfReal&     theParams,
                                         Standard_Real&            theGap,
                                         Standard_Integer&         theStatus);

  void Init(const TopoDS_Shape& theShape);

private:
  TopoDS_Shape     myShape;
  TopAbs_ShapeEnum mySType;
  Standard_Real    myGap;
  Standard_Integer myStatus;
};

#endif // _ShapeAnalysis_CanonicalRecognition_HeaderFile
