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
#include <Standard_Type.hxx>

#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Transient.hxx>



class ChFiDS_SurfData;
DEFINE_STANDARD_HANDLE(ChFiDS_SurfData, Standard_Transient)

//! data structure for all information related to  the
//! fillet and to 2 faces vis a vis
class ChFiDS_SurfData : public Standard_Transient
{

public:

  
  Standard_EXPORT ChFiDS_SurfData();
  
  Standard_EXPORT void Copy (const Handle(ChFiDS_SurfData)& Other);
  
    Standard_Integer IndexOfS1() const;
  
    Standard_Integer IndexOfS2() const;
  
    Standard_Boolean IsOnCurve1() const;
  
    Standard_Boolean IsOnCurve2() const;
  
    Standard_Integer IndexOfC1() const;
  
    Standard_Integer IndexOfC2() const;
  
    Standard_Integer Surf() const;
  
    TopAbs_Orientation Orientation() const;
  
    const ChFiDS_FaceInterference& InterferenceOnS1() const;
  
    const ChFiDS_FaceInterference& InterferenceOnS2() const;
  
    const ChFiDS_CommonPoint& VertexFirstOnS1() const;
  
    const ChFiDS_CommonPoint& VertexFirstOnS2() const;
  
    const ChFiDS_CommonPoint& VertexLastOnS1() const;
  
    const ChFiDS_CommonPoint& VertexLastOnS2() const;
  
    void ChangeIndexOfS1 (const Standard_Integer Index);
  
    void ChangeIndexOfS2 (const Standard_Integer Index);
  
    void ChangeSurf (const Standard_Integer Index);
  
    void SetIndexOfC1 (const Standard_Integer Index);
  
    void SetIndexOfC2 (const Standard_Integer Index);
  
    TopAbs_Orientation& ChangeOrientation();
  
    ChFiDS_FaceInterference& ChangeInterferenceOnS1();
  
    ChFiDS_FaceInterference& ChangeInterferenceOnS2();
  
    ChFiDS_CommonPoint& ChangeVertexFirstOnS1();
  
    ChFiDS_CommonPoint& ChangeVertexFirstOnS2();
  
    ChFiDS_CommonPoint& ChangeVertexLastOnS1();
  
    ChFiDS_CommonPoint& ChangeVertexLastOnS2();
  
  Standard_EXPORT const ChFiDS_FaceInterference& Interference (const Standard_Integer OnS) const;
  
  Standard_EXPORT ChFiDS_FaceInterference& ChangeInterference (const Standard_Integer OnS);
  
  Standard_EXPORT Standard_Integer Index (const Standard_Integer OfS) const;
  
  //! returns one of the four vertices  whether First is true
  //! or wrong and OnS equals 1 or 2.
  Standard_EXPORT const ChFiDS_CommonPoint& Vertex (const Standard_Boolean First, const Standard_Integer OnS) const;
  
  //! returns one of the four vertices  whether First is true
  //! or wrong and OnS equals 1 or 2.
  Standard_EXPORT ChFiDS_CommonPoint& ChangeVertex (const Standard_Boolean First, const Standard_Integer OnS);
  
    Standard_Boolean IsOnCurve (const Standard_Integer OnS) const;
  
    Standard_Integer IndexOfC (const Standard_Integer OnS) const;
  
  Standard_EXPORT Standard_Real FirstSpineParam() const;
  
  Standard_EXPORT Standard_Real LastSpineParam() const;
  
  Standard_EXPORT void FirstSpineParam (const Standard_Real Par);
  
  Standard_EXPORT void LastSpineParam (const Standard_Real Par);
  
  Standard_EXPORT Standard_Real FirstExtensionValue() const;
  
  Standard_EXPORT Standard_Real LastExtensionValue() const;
  
  Standard_EXPORT void FirstExtensionValue (const Standard_Real Extend);
  
  Standard_EXPORT void LastExtensionValue (const Standard_Real Extend);
  
  Standard_EXPORT Handle(Standard_Transient) Simul() const;
  
  Standard_EXPORT void SetSimul (const Handle(Standard_Transient)& S);
  
  Standard_EXPORT void ResetSimul();
  
  Standard_EXPORT gp_Pnt2d Get2dPoints (const Standard_Boolean First, const Standard_Integer OnS) const;
  
  Standard_EXPORT void Get2dPoints (gp_Pnt2d& P2df1, gp_Pnt2d& P2dl1, gp_Pnt2d& P2df2, gp_Pnt2d& P2dl2) const;
  
  Standard_EXPORT void Set2dPoints (const gp_Pnt2d& P2df1, const gp_Pnt2d& P2dl1, const gp_Pnt2d& P2df2, const gp_Pnt2d& P2dl2);
  
    Standard_Boolean TwistOnS1() const;
  
    Standard_Boolean TwistOnS2() const;
  
    void TwistOnS1 (const Standard_Boolean T);
  
    void TwistOnS2 (const Standard_Boolean T);




  DEFINE_STANDARD_RTTIEXT(ChFiDS_SurfData,Standard_Transient)

protected:




private:


  ChFiDS_CommonPoint pfirstOnS1;
  ChFiDS_CommonPoint plastOnS1;
  ChFiDS_CommonPoint pfirstOnS2;
  ChFiDS_CommonPoint plastOnS2;
  ChFiDS_FaceInterference intf1;
  ChFiDS_FaceInterference intf2;
  gp_Pnt2d p2df1;
  gp_Pnt2d p2dl1;
  gp_Pnt2d p2df2;
  gp_Pnt2d p2dl2;
  Standard_Real ufspine;
  Standard_Real ulspine;
  Standard_Real myfirstextend;
  Standard_Real mylastextend;
  Handle(Standard_Transient) simul;
  Standard_Integer indexOfS1;
  Standard_Integer indexOfC1;
  Standard_Integer indexOfS2;
  Standard_Integer indexOfC2;
  Standard_Integer indexOfConge;
  Standard_Boolean isoncurv1;
  Standard_Boolean isoncurv2;
  Standard_Boolean twistons1;
  Standard_Boolean twistons2;
  TopAbs_Orientation orientation;


};


#include <ChFiDS_SurfData.lxx>





#endif // _ChFiDS_SurfData_HeaderFile
