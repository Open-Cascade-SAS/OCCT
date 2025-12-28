// Created on: 1996-04-16
// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AdvApp2Var_Context_HeaderFile
#define _AdvApp2Var_Context_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

//! contains all the parameters for approximation
//! (tolerancy, computing option, ...)
class AdvApp2Var_Context
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT AdvApp2Var_Context();

  Standard_EXPORT AdvApp2Var_Context(const int                                       ifav,
                                     const int                                       iu,
                                     const int                                       iv,
                                     const int                                       nlimu,
                                     const int                                       nlimv,
                                     const int                                       iprecis,
                                     const int                                       nb1Dss,
                                     const int                                       nb2Dss,
                                     const int                                       nb3Dss,
                                     const occ::handle<NCollection_HArray1<double>>& tol1D,
                                     const occ::handle<NCollection_HArray1<double>>& tol2D,
                                     const occ::handle<NCollection_HArray1<double>>& tol3D,
                                     const occ::handle<NCollection_HArray2<double>>& tof1D,
                                     const occ::handle<NCollection_HArray2<double>>& tof2D,
                                     const occ::handle<NCollection_HArray2<double>>& tof3D);

  Standard_EXPORT int TotalDimension() const;

  Standard_EXPORT int TotalNumberSSP() const;

  Standard_EXPORT int FavorIso() const;

  Standard_EXPORT int UOrder() const;

  Standard_EXPORT int VOrder() const;

  Standard_EXPORT int ULimit() const;

  Standard_EXPORT int VLimit() const;

  Standard_EXPORT int UJacDeg() const;

  Standard_EXPORT int VJacDeg() const;

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> UJacMax() const;

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> VJacMax() const;

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> URoots() const;

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> VRoots() const;

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> UGauss() const;

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> VGauss() const;

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> IToler() const;

  Standard_EXPORT occ::handle<NCollection_HArray2<double>> FToler() const;

  Standard_EXPORT occ::handle<NCollection_HArray2<double>> CToler() const;

private:
  int                                      myFav;
  int                                      myOrdU;
  int                                      myOrdV;
  int                                      myLimU;
  int                                      myLimV;
  int                                      myNb1DSS;
  int                                      myNb2DSS;
  int                                      myNb3DSS;
  int                                      myNbURoot;
  int                                      myNbVRoot;
  int                                      myJDegU;
  int                                      myJDegV;
  occ::handle<NCollection_HArray1<double>> myJMaxU;
  occ::handle<NCollection_HArray1<double>> myJMaxV;
  occ::handle<NCollection_HArray1<double>> myURoots;
  occ::handle<NCollection_HArray1<double>> myVRoots;
  occ::handle<NCollection_HArray1<double>> myUGauss;
  occ::handle<NCollection_HArray1<double>> myVGauss;
  occ::handle<NCollection_HArray1<double>> myInternalTol;
  occ::handle<NCollection_HArray2<double>> myFrontierTol;
  occ::handle<NCollection_HArray2<double>> myCuttingTol;
};

#endif // _AdvApp2Var_Context_HeaderFile
