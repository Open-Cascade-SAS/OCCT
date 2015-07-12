// Created on: 1993-11-17
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRep_VPointInterClassifier_HeaderFile
#define _TopOpeBRep_VPointInterClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepClass_FaceClassifier.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class TopoDS_Shape;
class TopOpeBRep_VPointInter;
class TopOpeBRep_PointClassifier;



class TopOpeBRep_VPointInterClassifier 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRep_VPointInterClassifier();
  
  //! compute position of VPoint <VP> regarding with face <F>.
  //! <ShapeIndex> (= 1,2) indicates which (u,v) point of <VP> is used.
  //! when state is ON, set VP.EdgeON() with the edge containing <VP>
  //! and associated parameter.
  //! returns state of VP on ShapeIndex.
  Standard_EXPORT TopAbs_State VPointPosition (const TopoDS_Shape& F, TopOpeBRep_VPointInter& VP, const Standard_Integer ShapeIndex, TopOpeBRep_PointClassifier& PC, const Standard_Boolean AssumeINON, const Standard_Real Tol);
  
  //! returns the edge containing the VPoint <VP> used in the
  //! last VPointPosition() call. Edge is defined if the state previously
  //! computed is ON, else Edge is a null shape.
  Standard_EXPORT const TopoDS_Shape& Edge() const;
  
  //! returns the parameter of the VPoint <VP> on Edge()
  Standard_EXPORT Standard_Real EdgeParameter() const;




protected:





private:



  BRepClass_FaceClassifier mySlowFaceClassifier;
  TopAbs_State myState;
  TopoDS_Shape myNullShape;


};







#endif // _TopOpeBRep_VPointInterClassifier_HeaderFile
