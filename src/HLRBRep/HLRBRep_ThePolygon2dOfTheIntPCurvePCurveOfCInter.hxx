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

#ifndef _HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter_HeaderFile
#define _HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Standard_Boolean.hxx>
#include <Intf_Polygon2d.hxx>
class Standard_OutOfRange;
class HLRBRep_CurveTool;
class IntRes2d_Domain;
class Bnd_Box2d;
class gp_Pnt2d;



class HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter  : public Intf_Polygon2d
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Compute a polygon on the domain of the curve.
  Standard_EXPORT HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter(const Standard_Address& Curve, const Standard_Integer NbPnt, const IntRes2d_Domain& Domain, const Standard_Real Tol);
  
  //! The current polygon is modified if most
  //! of the  points of the  polygon  are
  //! outside  the  box  <OtherBox>.  In this
  //! situation, bounds are computed to build
  //! a polygon inside or near the OtherBox.
  Standard_EXPORT void ComputeWithBox (const Standard_Address& Curve, const Bnd_Box2d& OtherBox);
  
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

#define TheCurve Standard_Address
#define TheCurve_hxx <Standard_Address.hxx>
#define TheCurveTool HLRBRep_CurveTool
#define TheCurveTool_hxx <HLRBRep_CurveTool.hxx>
#define IntCurve_Polygon2dGen HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter
#define IntCurve_Polygon2dGen_hxx <HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter.hxx>

#include <IntCurve_Polygon2dGen.lxx>

#undef TheCurve
#undef TheCurve_hxx
#undef TheCurveTool
#undef TheCurveTool_hxx
#undef IntCurve_Polygon2dGen
#undef IntCurve_Polygon2dGen_hxx




#endif // _HLRBRep_ThePolygon2dOfTheIntPCurvePCurveOfCInter_HeaderFile
