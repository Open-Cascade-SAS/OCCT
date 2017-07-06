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

#ifndef _HLRAlgo_PolyInternalData_HeaderFile
#define _HLRAlgo_PolyInternalData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <HLRAlgo_HArray1OfTData.hxx>
#include <HLRAlgo_HArray1OfPISeg.hxx>
#include <HLRAlgo_HArray1OfPINod.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Real.hxx>
#include <HLRAlgo_Array1OfTData.hxx>
#include <HLRAlgo_Array1OfPISeg.hxx>
#include <HLRAlgo_Array1OfPINod.hxx>


class HLRAlgo_PolyInternalData;
DEFINE_STANDARD_HANDLE(HLRAlgo_PolyInternalData, Standard_Transient)

//! to Update OutLines.
class HLRAlgo_PolyInternalData : public Standard_Transient
{

public:

  
  Standard_EXPORT HLRAlgo_PolyInternalData(const Standard_Integer nbNod, const Standard_Integer nbTri);
  
  Standard_EXPORT void UpdateLinks (HLRAlgo_Array1OfTData*& TData, HLRAlgo_Array1OfPISeg*& PISeg, HLRAlgo_Array1OfPINod*& PINod);
  
  Standard_EXPORT Standard_Integer AddNode (HLRAlgo_PolyInternalNode::NodeData& Nod1RValues, HLRAlgo_PolyInternalNode::NodeData& Nod2RValues, HLRAlgo_Array1OfPINod*& PINod1, HLRAlgo_Array1OfPINod*& PINod2, const Standard_Real coef1, const Standard_Real X3, const Standard_Real Y3, const Standard_Real Z3);
  
  Standard_EXPORT void UpdateLinks (const Standard_Integer ip1, const Standard_Integer ip2, const Standard_Integer ip3, HLRAlgo_Array1OfTData*& TData1, HLRAlgo_Array1OfTData*& TData2, HLRAlgo_Array1OfPISeg*& PISeg1, HLRAlgo_Array1OfPISeg*& PISeg2, HLRAlgo_Array1OfPINod*& PINod1, HLRAlgo_Array1OfPINod*& PINod2);
  
  Standard_EXPORT void Dump() const;
  
  Standard_EXPORT void IncTData (HLRAlgo_Array1OfTData*& TData1, HLRAlgo_Array1OfTData*& TData2);
  
  Standard_EXPORT void IncPISeg (HLRAlgo_Array1OfPISeg*& PISeg1, HLRAlgo_Array1OfPISeg*& PISeg2);
  
  Standard_EXPORT void IncPINod (HLRAlgo_Array1OfPINod*& PINod1, HLRAlgo_Array1OfPINod*& PINod2);
  
    void DecTData();
  
    void DecPISeg();
  
    void DecPINod();
  
    Standard_Integer NbTData() const;
  
    Standard_Integer NbPISeg() const;
  
    Standard_Integer NbPINod() const;
  
    Standard_Boolean Planar() const;
  
    void Planar (const Standard_Boolean B);
  
    Standard_Boolean IntOutL() const;
  
    void IntOutL (const Standard_Boolean B);
  
    HLRAlgo_Array1OfTData& TData() const;
  
    HLRAlgo_Array1OfPISeg& PISeg() const;
  
    HLRAlgo_Array1OfPINod& PINod() const;




  DEFINE_STANDARD_RTTIEXT(HLRAlgo_PolyInternalData,Standard_Transient)

protected:




private:


  Standard_Integer myNbTData;
  Standard_Integer myNbPISeg;
  Standard_Integer myNbPINod;
  Standard_Integer myMxTData;
  Standard_Integer myMxPISeg;
  Standard_Integer myMxPINod;
  Standard_Boolean myIntOutL;
  Standard_Boolean myPlanar;
  Handle(HLRAlgo_HArray1OfTData) myTData;
  Handle(HLRAlgo_HArray1OfPISeg) myPISeg;
  Handle(HLRAlgo_HArray1OfPINod) myPINod;


};


#include <HLRAlgo_PolyInternalData.lxx>





#endif // _HLRAlgo_PolyInternalData_HeaderFile
