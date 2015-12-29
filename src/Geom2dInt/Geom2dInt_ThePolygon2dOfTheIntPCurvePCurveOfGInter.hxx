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

#ifndef _Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter_HeaderFile
#define _Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Standard_Boolean.hxx>
#include <Intf_Polygon2d.hxx>
class Standard_OutOfRange;
class Adaptor2d_Curve2d;
class Geom2dInt_Geom2dCurveTool;
class IntRes2d_Domain;
class Bnd_Box2d;
class gp_Pnt2d;



class Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter  : public Intf_Polygon2d
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Compute a polygon on the domain of the curve.
  Standard_EXPORT Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter(const Adaptor2d_Curve2d& Curve, const Standard_Integer NbPnt, const IntRes2d_Domain& Domain, const Standard_Real Tol);
  
  //! The current polygon is modified if most
  //! of the  points of the  polygon  are are
  //! outside  the  box  <OtherBox>.  In this
  //! situation, bounds are computed to build
  //! a polygon inside or near the OtherBox.
  Standard_EXPORT void ComputeWithBox (const Adaptor2d_Curve2d& Curve, const Bnd_Box2d& OtherBox);
  
    virtual Standard_Real DeflectionOverEstimation() const Standard_OVERRIDE;
  
    void SetDeflectionOverEstimation (const Standard_Real x);
  
    void Closed (const Standard_Boolean clos);
  
  //! Returns True if the polyline is closed.
    virtual Standard_Boolean Closed () const Standard_OVERRIDE { return ClosedPolygon; }
  
  //! Give the number of Segments in the polyline.
    virtual Standard_Integer NbSegments() const Standard_OVERRIDE;
  
  //! Returns the points of the segment <Index> in the Polygon.
  Standard_EXPORT virtual void Segment (const Standard_Integer theIndex, gp_Pnt2d& theBegin, gp_Pnt2d& theEnd) const Standard_OVERRIDE;
  
  //! Returns the parameter (On the curve)
  //! of the first point of the Polygon
    Standard_Real InfParameter() const;
  
  //! Returns the parameter (On the curve)
  //! of the last point of the Polygon
    Standard_Real SupParameter() const;
  
  Standard_EXPORT Standard_Boolean AutoIntersectionIsPossible() const;
  
  //! Give an approximation of the parameter on the curve
  //! according to the discretization of the Curve.
  Standard_EXPORT Standard_Real ApproxParamOnCurve (const Standard_Integer Index, const Standard_Real ParamOnLine) const;
  
    Standard_Integer CalculRegion (const Standard_Real x, const Standard_Real y, const Standard_Real x1, const Standard_Real x2, const Standard_Real y1, const Standard_Real y2) const;
  
  Standard_EXPORT void Dump() const;




protected:





private:



  Standard_Real TheDeflection;
  Standard_Integer NbPntIn;
  Standard_Integer TheMaxNbPoints;
  TColgp_Array1OfPnt2d ThePnts;
  TColStd_Array1OfReal TheParams;
  TColStd_Array1OfInteger TheIndex;
  Standard_Boolean ClosedPolygon;
  Standard_Real Binf;
  Standard_Real Bsup;


};

#define TheCurve Adaptor2d_Curve2d
#define TheCurve_hxx <Adaptor2d_Curve2d.hxx>
#define TheCurveTool Geom2dInt_Geom2dCurveTool
#define TheCurveTool_hxx <Geom2dInt_Geom2dCurveTool.hxx>
#define IntCurve_Polygon2dGen Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter
#define IntCurve_Polygon2dGen_hxx <Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter.hxx>

#include <IntCurve_Polygon2dGen.lxx>

#undef TheCurve
#undef TheCurve_hxx
#undef TheCurveTool
#undef TheCurveTool_hxx
#undef IntCurve_Polygon2dGen
#undef IntCurve_Polygon2dGen_hxx




#endif // _Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter_HeaderFile
