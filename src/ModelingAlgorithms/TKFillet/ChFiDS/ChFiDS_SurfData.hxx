// Created on: 1993-11-26
// Created by: Isabelle GRIGNON
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

#ifndef _ChFiDS_SurfData_HeaderFile
#define _ChFiDS_SurfData_HeaderFile

#include <Standard.hxx>

#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Transient.hxx>

//! data structure for all information related to the
//! fillet and to 2 faces vis a vis
class ChFiDS_SurfData : public Standard_Transient
{

public:
  Standard_EXPORT ChFiDS_SurfData();

  Standard_EXPORT void Copy(const occ::handle<ChFiDS_SurfData>& Other);

  int IndexOfS1() const;

  int IndexOfS2() const;

  bool IsOnCurve1() const;

  bool IsOnCurve2() const;

  int IndexOfC1() const;

  int IndexOfC2() const;

  int Surf() const;

  TopAbs_Orientation Orientation() const;

  const ChFiDS_FaceInterference& InterferenceOnS1() const;

  const ChFiDS_FaceInterference& InterferenceOnS2() const;

  const ChFiDS_CommonPoint& VertexFirstOnS1() const;

  const ChFiDS_CommonPoint& VertexFirstOnS2() const;

  const ChFiDS_CommonPoint& VertexLastOnS1() const;

  const ChFiDS_CommonPoint& VertexLastOnS2() const;

  void ChangeIndexOfS1(const int Index);

  void ChangeIndexOfS2(const int Index);

  void ChangeSurf(const int Index);

  void SetIndexOfC1(const int Index);

  void SetIndexOfC2(const int Index);

  TopAbs_Orientation& ChangeOrientation();

  ChFiDS_FaceInterference& ChangeInterferenceOnS1();

  ChFiDS_FaceInterference& ChangeInterferenceOnS2();

  ChFiDS_CommonPoint& ChangeVertexFirstOnS1();

  ChFiDS_CommonPoint& ChangeVertexFirstOnS2();

  ChFiDS_CommonPoint& ChangeVertexLastOnS1();

  ChFiDS_CommonPoint& ChangeVertexLastOnS2();

  Standard_EXPORT const ChFiDS_FaceInterference& Interference(const int OnS) const;

  Standard_EXPORT ChFiDS_FaceInterference& ChangeInterference(const int OnS);

  Standard_EXPORT int Index(const int OfS) const;

  //! returns one of the four vertices whether First is true
  //! or wrong and OnS equals 1 or 2.
  Standard_EXPORT const ChFiDS_CommonPoint& Vertex(const bool First,
                                                   const int OnS) const;

  //! returns one of the four vertices whether First is true
  //! or wrong and OnS equals 1 or 2.
  Standard_EXPORT ChFiDS_CommonPoint& ChangeVertex(const bool First,
                                                   const int OnS);

  bool IsOnCurve(const int OnS) const;

  int IndexOfC(const int OnS) const;

  Standard_EXPORT double FirstSpineParam() const;

  Standard_EXPORT double LastSpineParam() const;

  Standard_EXPORT void FirstSpineParam(const double Par);

  Standard_EXPORT void LastSpineParam(const double Par);

  Standard_EXPORT double FirstExtensionValue() const;

  Standard_EXPORT double LastExtensionValue() const;

  Standard_EXPORT void FirstExtensionValue(const double Extend);

  Standard_EXPORT void LastExtensionValue(const double Extend);

  Standard_EXPORT occ::handle<Standard_Transient> Simul() const;

  Standard_EXPORT void SetSimul(const occ::handle<Standard_Transient>& S);

  Standard_EXPORT void ResetSimul();

  Standard_EXPORT gp_Pnt2d Get2dPoints(const bool First,
                                       const int OnS) const;

  Standard_EXPORT void Get2dPoints(gp_Pnt2d& P2df1,
                                   gp_Pnt2d& P2dl1,
                                   gp_Pnt2d& P2df2,
                                   gp_Pnt2d& P2dl2) const;

  Standard_EXPORT void Set2dPoints(const gp_Pnt2d& P2df1,
                                   const gp_Pnt2d& P2dl1,
                                   const gp_Pnt2d& P2df2,
                                   const gp_Pnt2d& P2dl2);

  bool TwistOnS1() const;

  bool TwistOnS2() const;

  void TwistOnS1(const bool T);

  void TwistOnS2(const bool T);

  DEFINE_STANDARD_RTTIEXT(ChFiDS_SurfData, Standard_Transient)

private:
  ChFiDS_CommonPoint         pfirstOnS1;
  ChFiDS_CommonPoint         plastOnS1;
  ChFiDS_CommonPoint         pfirstOnS2;
  ChFiDS_CommonPoint         plastOnS2;
  ChFiDS_FaceInterference    intf1;
  ChFiDS_FaceInterference    intf2;
  gp_Pnt2d                   p2df1;
  gp_Pnt2d                   p2dl1;
  gp_Pnt2d                   p2df2;
  gp_Pnt2d                   p2dl2;
  double              ufspine;
  double              ulspine;
  double              myfirstextend;
  double              mylastextend;
  occ::handle<Standard_Transient> simul;
  int           indexOfS1;
  int           indexOfC1;
  int           indexOfS2;
  int           indexOfC2;
  int           indexOfConge;
  bool           isoncurv1;
  bool           isoncurv2;
  bool           twistons1;
  bool           twistons2;
  TopAbs_Orientation         orientation;
};

#include <ChFiDS_SurfData.lxx>

#endif // _ChFiDS_SurfData_HeaderFile
