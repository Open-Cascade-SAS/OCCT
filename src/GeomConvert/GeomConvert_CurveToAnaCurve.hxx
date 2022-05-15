// Created: 2001-05-21 
// 
// Copyright (c) 2001-2013 OPEN CASCADE SAS 
// 
// This file is part of commercial software by OPEN CASCADE SAS, 
// furnished in accordance with the terms and conditions of the contract 
// and with the inclusion of this copyright notice. 
// This file or any part thereof may not be provided or otherwise 
// made available to any third party. 
// 
// No ownership title to the software is transferred hereby. 
// 
// OPEN CASCADE SAS makes no representation or warranties with respect to the 
// performance of this software, and specifically disclaims any responsibility 
// for any damages, special or consequential, connected with its use. 

#ifndef _GeomConvert_CurveToAnaCurve_HeaderFile
#define _GeomConvert_CurveToAnaCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <GeomConvert_ConvType.hxx>
#include <GeomAbs_CurveType.hxx>

class Geom_Curve;
class Geom_Line;
class gp_Lin;
class gp_Pnt;
class gp_Circ;



class GeomConvert_CurveToAnaCurve 
{
public:

  DEFINE_STANDARD_ALLOC
 
  Standard_EXPORT GeomConvert_CurveToAnaCurve();
  
  Standard_EXPORT GeomConvert_CurveToAnaCurve(const Handle(Geom_Curve)& C);
  
  Standard_EXPORT void Init (const Handle(Geom_Curve)& C);
  
  //! Converts me to analytical if possible with given
  //! tolerance. The new first and last parameters are
  //! returned to newF, newL
  Standard_EXPORT Standard_Boolean ConvertToAnalytical (const Standard_Real theTol, Handle(Geom_Curve)& theResultCurve, const Standard_Real F, const Standard_Real L, Standard_Real& newF, Standard_Real& newL);
  
  Standard_EXPORT static Handle(Geom_Curve) ComputeCurve (const Handle(Geom_Curve)& curve, const Standard_Real tolerance, 
    const Standard_Real c1, const Standard_Real c2, Standard_Real& cf, Standard_Real& cl, 
    Standard_Real& theGap, const GeomConvert_ConvType theCurvType = GeomConvert_MinGap, const GeomAbs_CurveType theTarget = GeomAbs_Line);
  
  //! Tries to convert the given curve to circle with given
  //! tolerance. Returns NULL curve if conversion is
  //! not possible.
  Standard_EXPORT static Handle(Geom_Curve) ComputeCircle (const Handle(Geom_Curve)& curve, const Standard_Real tolerance, const Standard_Real c1, const Standard_Real c2, Standard_Real& cf, Standard_Real& cl, Standard_Real& Deviation);
  
  //! Tries to convert the given curve to ellipse with given
  //! tolerance. Returns NULL curve if conversion is
  //! not possible.
  Standard_EXPORT static Handle(Geom_Curve) ComputeEllipse (const Handle(Geom_Curve)& curve, const Standard_Real tolerance, const Standard_Real c1, const Standard_Real c2, Standard_Real& cf, Standard_Real& cl, Standard_Real& Deviation);
  
  //! Tries to convert the given curve to line with given
  //! tolerance. Returns NULL curve if conversion is
  //! not possible.
  Standard_EXPORT static Handle(Geom_Line) ComputeLine (const Handle(Geom_Curve)& curve, const Standard_Real tolerance, const Standard_Real c1, const Standard_Real c2, Standard_Real& cf, Standard_Real& cl, Standard_Real& Deviation);
  
  //! Returns true if the set of points is linear with given
  //! tolerance
  Standard_EXPORT static Standard_Boolean IsLinear (const TColgp_Array1OfPnt& aPoints, const Standard_Real tolerance, Standard_Real& Deviation);
   
  //! Creates line on two points.
  //! Resulting parameters returned
  Standard_EXPORT static gp_Lin GetLine(const gp_Pnt& P1, const gp_Pnt& P2, Standard_Real& cf, Standard_Real& cl);

  //! Creates circle on points. Returns true if OK.
  Standard_EXPORT static Standard_Boolean GetCircle(gp_Circ& Circ, const gp_Pnt& P0, const gp_Pnt& P1, const gp_Pnt& P2); 

  //! Returns maximal deviation of converted surface from the original
  //! one computed by last call to ConvertToAnalytical
  Standard_Real Gap() const
  {
    return myGap;
  }

  //! Returns conversion type 
  GeomConvert_ConvType GetConvType() const
  {
    return myConvType;
  }

  //! Sets type of convertion
  void SetConvType(const GeomConvert_ConvType theConvType)
  {
    myConvType = theConvType;
  }

  //! Returns target curve type 
  GeomAbs_CurveType GetTarget() const
  {
    return myTarget;
  }

  //! Sets target curve type
  void SetTarget(const GeomAbs_CurveType theTarget)
  {
    myTarget = theTarget;
  }


protected:





private:

  Handle(Geom_Curve) myCurve;
  Standard_Real myGap;
  GeomConvert_ConvType myConvType;
  GeomAbs_CurveType myTarget;

};



#endif // _GeomConvert_CurveToAnaCurve_HeaderFile
