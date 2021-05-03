// Created on: 1992-02-18
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

#ifndef _HLRAlgo_PolyAlgo_HeaderFile
#define _HLRAlgo_PolyAlgo_HeaderFile

#include <NCollection_Array1.hxx>
#include <HLRAlgo_PolyData.hxx>
#include <HLRAlgo_ListIteratorOfListOfBPoint.hxx>

class HLRAlgo_EdgeStatus;
class HLRAlgo_PolyShellData;

class HLRAlgo_PolyAlgo;
DEFINE_STANDARD_HANDLE(HLRAlgo_PolyAlgo, Standard_Transient)

//! to remove Hidden lines on Triangulations.
class HLRAlgo_PolyAlgo : public Standard_Transient
{

public:

  Standard_EXPORT HLRAlgo_PolyAlgo();

  Standard_EXPORT void Init (const Standard_Integer theNbShells);

  const NCollection_Array1<Handle(HLRAlgo_PolyShellData)>& PolyShell() const { return myHShell; }

  NCollection_Array1<Handle(HLRAlgo_PolyShellData)>& ChangePolyShell() { return myHShell; }

  Standard_EXPORT void Clear();

  //! Prepare all the data to process the algo.
  Standard_EXPORT void Update();

  void InitHide()
  {
    myCurShell = 0;
    NextHide();
  }

  Standard_Boolean MoreHide() const { return myFound; }

  Standard_EXPORT void NextHide();

  //! process hiding between <Pt1> and <Pt2>.
  Standard_EXPORT HLRAlgo_BiPoint::PointsT& Hide (HLRAlgo_EdgeStatus& status,
                                                  Standard_Integer& Index,
                                                  Standard_Boolean& reg1,
                                                  Standard_Boolean& regn,
                                                  Standard_Boolean& outl,
                                                  Standard_Boolean& intl);

  void InitShow()
  {
    myCurShell = 0;
    NextShow();
  }

  Standard_Boolean MoreShow() const { return myFound; }

  Standard_EXPORT void NextShow();

  //! process hiding between <Pt1> and <Pt2>.
  Standard_EXPORT HLRAlgo_BiPoint::PointsT& Show (Standard_Integer& Index,
                                                  Standard_Boolean& reg1,
                                                  Standard_Boolean& regn,
                                                  Standard_Boolean& outl,
                                                  Standard_Boolean& intl);

  DEFINE_STANDARD_RTTIEXT(HLRAlgo_PolyAlgo,Standard_Transient)

private:

  NCollection_Array1<Handle(HLRAlgo_PolyShellData)> myHShell;
  HLRAlgo_PolyData::Triangle myTriangle;
  HLRAlgo_ListIteratorOfListOfBPoint mySegListIt;
  Standard_Integer myNbrShell;
  Standard_Integer myCurShell;
  Standard_Boolean myFound;

};

#endif // _HLRAlgo_PolyAlgo_HeaderFile
