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

#include <HLRAlgo_PolyData.hxx>
#include <HLRAlgo_BiPoint.hxx>
#include <NCollection_List.hxx>

class HLRAlgo_EdgeStatus;
class HLRAlgo_PolyShellData;

//! to remove Hidden lines on Triangulations.
class HLRAlgo_PolyAlgo : public Standard_Transient
{

public:
  Standard_EXPORT HLRAlgo_PolyAlgo();

  Standard_EXPORT void Init(const int theNbShells);

  const NCollection_Array1<occ::handle<HLRAlgo_PolyShellData>>& PolyShell() const { return myHShell; }

  NCollection_Array1<occ::handle<HLRAlgo_PolyShellData>>& ChangePolyShell() { return myHShell; }

  Standard_EXPORT void Clear();

  //! Prepare all the data to process the algo.
  Standard_EXPORT void Update();

  void InitHide()
  {
    myCurShell = 0;
    NextHide();
  }

  bool MoreHide() const { return myFound; }

  Standard_EXPORT void NextHide();

  //! process hiding between <Pt1> and <Pt2>.
  Standard_EXPORT HLRAlgo_BiPoint::PointsT& Hide(HLRAlgo_EdgeStatus& status,
                                                 int&   Index,
                                                 bool&   reg1,
                                                 bool&   regn,
                                                 bool&   outl,
                                                 bool&   intl);

  void InitShow()
  {
    myCurShell = 0;
    NextShow();
  }

  bool MoreShow() const { return myFound; }

  Standard_EXPORT void NextShow();

  //! process hiding between <Pt1> and <Pt2>.
  Standard_EXPORT HLRAlgo_BiPoint::PointsT& Show(int& Index,
                                                 bool& reg1,
                                                 bool& regn,
                                                 bool& outl,
                                                 bool& intl);

  DEFINE_STANDARD_RTTIEXT(HLRAlgo_PolyAlgo, Standard_Transient)

private:
  NCollection_Array1<occ::handle<HLRAlgo_PolyShellData>> myHShell;
  HLRAlgo_PolyData::Triangle                        myTriangle;
  NCollection_List<HLRAlgo_BiPoint>::Iterator                mySegListIt;
  int                                  myNbrShell;
  int                                  myCurShell;
  bool                                  myFound;
};

#endif // _HLRAlgo_PolyAlgo_HeaderFile
