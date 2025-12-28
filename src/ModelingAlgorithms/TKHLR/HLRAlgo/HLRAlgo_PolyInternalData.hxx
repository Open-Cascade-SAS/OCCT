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
#include <Standard_Integer.hxx>
#include <HLRAlgo_TriangleData.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <HLRAlgo_PolyInternalSegment.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <HLRAlgo_PolyInternalNode.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

//! to Update OutLines.
class HLRAlgo_PolyInternalData : public Standard_Transient
{

public:
  Standard_EXPORT HLRAlgo_PolyInternalData(const int nbNod,
                                           const int nbTri);

  Standard_EXPORT void UpdateLinks(NCollection_Array1<HLRAlgo_TriangleData>& theTData,
                                   NCollection_Array1<HLRAlgo_PolyInternalSegment>& thePISeg,
                                   NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>& thePINod);

  Standard_EXPORT int AddNode(HLRAlgo_PolyInternalNode::NodeData& theNod1RValues,
                                           HLRAlgo_PolyInternalNode::NodeData& theNod2RValues,
                                           NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*&             thePINod1,
                                           NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*&             thePINod2,
                                           const double                 theCoef1,
                                           const double                 theX3,
                                           const double                 theY3,
                                           const double                 theZ3);

  Standard_EXPORT void UpdateLinks(const int  theIp1,
                                   const int  theIp2,
                                   const int  theIp3,
                                   NCollection_Array1<HLRAlgo_TriangleData>*& theTData1,
                                   NCollection_Array1<HLRAlgo_TriangleData>*& theTData2,
                                   NCollection_Array1<HLRAlgo_PolyInternalSegment>*& thePISeg1,
                                   NCollection_Array1<HLRAlgo_PolyInternalSegment>*& thePISeg2,
                                   NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*& thePINod1,
                                   NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*& thePINod2);

  Standard_EXPORT void Dump() const;

  Standard_EXPORT void IncTData(NCollection_Array1<HLRAlgo_TriangleData>*& TData1, NCollection_Array1<HLRAlgo_TriangleData>*& TData2);

  Standard_EXPORT void IncPISeg(NCollection_Array1<HLRAlgo_PolyInternalSegment>*& PISeg1, NCollection_Array1<HLRAlgo_PolyInternalSegment>*& PISeg2);

  Standard_EXPORT void IncPINod(NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*& thePINod1,
                                NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*& thePINod2);

  void DecTData();

  void DecPISeg();

  void DecPINod();

  int NbTData() const;

  int NbPISeg() const;

  int NbPINod() const;

  bool Planar() const;

  void Planar(const bool B);

  bool IntOutL() const;

  void IntOutL(const bool B);

  NCollection_Array1<HLRAlgo_TriangleData>& TData() const;

  NCollection_Array1<HLRAlgo_PolyInternalSegment>& PISeg() const;

  NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>& PINod() const;

  DEFINE_STANDARD_RTTIEXT(HLRAlgo_PolyInternalData, Standard_Transient)

private:
  int               myNbTData;
  int               myNbPISeg;
  int               myNbPINod;
  int               myMxTData;
  int               myMxPISeg;
  int               myMxPINod;
  bool               myIntOutL;
  bool               myPlanar;
  occ::handle<NCollection_HArray1<HLRAlgo_TriangleData>> myTData;
  occ::handle<NCollection_HArray1<HLRAlgo_PolyInternalSegment>> myPISeg;
  occ::handle<NCollection_HArray1<occ::handle<HLRAlgo_PolyInternalNode>>> myPINod;
};

#include <HLRAlgo_PolyInternalData.lxx>

#endif // _HLRAlgo_PolyInternalData_HeaderFile
