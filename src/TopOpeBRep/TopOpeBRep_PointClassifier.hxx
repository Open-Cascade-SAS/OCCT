// Created on: 1995-12-07
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _TopOpeBRep_PointClassifier_HeaderFile
#define _TopOpeBRep_PointClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRep_DataMapOfTopolTool.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Real.hxx>
class BRepTopAdaptor_TopolTool;
class BRepAdaptor_HSurface;
class TopoDS_Face;
class gp_Pnt2d;



class TopOpeBRep_PointClassifier 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRep_PointClassifier();
  
  Standard_EXPORT void Init();
  
  Standard_EXPORT void Load (const TopoDS_Face& F);
  
  //! compute position of point <P> regarding with the face <F>.
  Standard_EXPORT TopAbs_State Classify (const TopoDS_Face& F, const gp_Pnt2d& P, const Standard_Real Tol);
  
  Standard_EXPORT TopAbs_State State() const;




protected:





private:



  Handle(BRepTopAdaptor_TopolTool) myTopolTool;
  Handle(BRepAdaptor_HSurface) myHSurface;
  TopOpeBRep_DataMapOfTopolTool myTopolToolMap;
  TopAbs_State myState;


};







#endif // _TopOpeBRep_PointClassifier_HeaderFile
