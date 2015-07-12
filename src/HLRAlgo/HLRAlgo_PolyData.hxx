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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <HLRAlgo_HArray1OfTData.hxx>
#include <HLRAlgo_HArray1OfPHDat.hxx>
#include <MMgt_TShared.hxx>
#include <TColgp_Array1OfXYZ.hxx>
#include <HLRAlgo_Array1OfTData.hxx>
#include <HLRAlgo_Array1OfPHDat.hxx>
#include <Standard_Address.hxx>
#include <Standard_Boolean.hxx>
class HLRAlgo_EdgeStatus;


class HLRAlgo_PolyData;
DEFINE_STANDARD_HANDLE(HLRAlgo_PolyData, MMgt_TShared)

//! Data structure of a set of Triangles.
class HLRAlgo_PolyData : public MMgt_TShared
{

public:

  
  Standard_EXPORT HLRAlgo_PolyData();
  
  Standard_EXPORT void HNodes (const Handle(TColgp_HArray1OfXYZ)& HNodes);
  
  Standard_EXPORT void HTData (const Handle(HLRAlgo_HArray1OfTData)& HTData);
  
  Standard_EXPORT void HPHDat (const Handle(HLRAlgo_HArray1OfPHDat)& HPHDat);
  
    void FaceIndex (const Standard_Integer I);
  
    Standard_Integer FaceIndex() const;
  
    TColgp_Array1OfXYZ& Nodes() const;
  
    HLRAlgo_Array1OfTData& TData() const;
  
    HLRAlgo_Array1OfPHDat& PHDat() const;
  
  Standard_EXPORT void UpdateGlobalMinMax (const Standard_Address ToTMinMax);
  
    Standard_Boolean Hiding() const;
  
  //! process hiding between <Pt1> and <Pt2>.
  Standard_EXPORT void HideByPolyData (const Standard_Address Coordinates, const Standard_Address RealPtr, const Standard_Address Indices, const Standard_Boolean HidingShell, HLRAlgo_EdgeStatus& status);
  
    Standard_Address Indices();




  DEFINE_STANDARD_RTTI(HLRAlgo_PolyData,MMgt_TShared)

protected:




private:

  
  //! evident.
  Standard_EXPORT void HideByOneTriangle (const Standard_Address Coordinates, const Standard_Address RealPtr, const Standard_Address BooleanPtr, const Standard_Address PlanPtr, HLRAlgo_EdgeStatus& status);

  Standard_Integer myIndices[3];
  Handle(TColgp_HArray1OfXYZ) myHNodes;
  Handle(HLRAlgo_HArray1OfTData) myHTData;
  Handle(HLRAlgo_HArray1OfPHDat) myHPHDat;


};


#include <HLRAlgo_PolyData.lxx>





#endif // _HLRAlgo_PolyData_HeaderFile
