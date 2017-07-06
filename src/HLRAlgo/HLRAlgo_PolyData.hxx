// Created on: 1993-10-29
// Created by: Christophe MARION
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _HLRAlgo_PolyData_HeaderFile
#define _HLRAlgo_PolyData_HeaderFile

#include <HLRAlgo_BiPoint.hxx>
#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <HLRAlgo_HArray1OfTData.hxx>
#include <HLRAlgo_HArray1OfPHDat.hxx>
#include <Standard_Transient.hxx>
#include <TColgp_Array1OfXYZ.hxx>
#include <HLRAlgo_Array1OfTData.hxx>
#include <HLRAlgo_Array1OfPHDat.hxx>
#include <Standard_Boolean.hxx>

class HLRAlgo_EdgeStatus;

class HLRAlgo_PolyData;
DEFINE_STANDARD_HANDLE(HLRAlgo_PolyData, Standard_Transient)

//! Data structure of a set of Triangles.
class HLRAlgo_PolyData : public Standard_Transient
{

public:
  struct FaceIndices
  {
    Standard_Integer Index, Min, Max;
  };

  struct Triangle
  {
    gp_XY V1, V2, V3;
    Standard_Real Param, TolParam, TolAng, Tolerance;
  };

  struct Box
  {
    Standard_Real XMin, YMin, ZMin, XMax, YMax, ZMax;

    //! The default constructor.
    Box()
    {
    }

    //! The initializing constructor.
    Box(
        const Standard_Real& theXMin,
        const Standard_Real& theYMin,
        const Standard_Real& theZMin,
        const Standard_Real& theXMax,
        const Standard_Real& theYMax,
        const Standard_Real& theZMax) :
      XMin(theXMin),
      YMin(theYMin),
      ZMin(theZMin),
      XMax(theXMax),
      YMax(theYMax),
      ZMax(theZMax)
    {
    }
  };

  Standard_EXPORT HLRAlgo_PolyData();
  
  Standard_EXPORT void HNodes (const Handle(TColgp_HArray1OfXYZ)& HNodes);
  
  Standard_EXPORT void HTData (const Handle(HLRAlgo_HArray1OfTData)& HTData);
  
  Standard_EXPORT void HPHDat (const Handle(HLRAlgo_HArray1OfPHDat)& HPHDat);
  
    void FaceIndex (const Standard_Integer I);
  
    Standard_Integer FaceIndex() const;
  
    TColgp_Array1OfXYZ& Nodes() const;
  
    HLRAlgo_Array1OfTData& TData() const;
  
    HLRAlgo_Array1OfPHDat& PHDat() const;
  
  Standard_EXPORT void UpdateGlobalMinMax (Box& theBox);
  
    Standard_Boolean Hiding() const;
  
  //! process hiding between <Pt1> and <Pt2>.
  Standard_EXPORT void HideByPolyData (const HLRAlgo_BiPoint::PointsT& thePoints, Triangle& theTriangle, HLRAlgo_BiPoint::IndicesT& theIndices, const Standard_Boolean HidingShell, HLRAlgo_EdgeStatus& status);
  
  FaceIndices& Indices()
  {
    return myFaceIndices;
  }

  DEFINE_STANDARD_RTTIEXT(HLRAlgo_PolyData,Standard_Transient)

private:

  //! evident.
  void hideByOneTriangle (const HLRAlgo_BiPoint::PointsT& thePoints,
                          Triangle& theTriangle,
                          const Standard_Boolean Crossing,
                          const Standard_Boolean HideBefore,
                          const Standard_Integer TrFlags,
                          HLRAlgo_EdgeStatus& status);

  FaceIndices myFaceIndices;
  Handle(TColgp_HArray1OfXYZ) myHNodes;
  Handle(HLRAlgo_HArray1OfTData) myHTData;
  Handle(HLRAlgo_HArray1OfPHDat) myHPHDat;

};

#include <HLRAlgo_PolyData.lxx>

#endif // _HLRAlgo_PolyData_HeaderFile
