// Created on: 1999-06-18
// Created by: Sergei ZERTCHANINOV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeFix_FaceConnect_HeaderFile
#define _ShapeFix_FaceConnect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class TopoDS_Face;
class TopoDS_Shell;


//! Rebuilds connectivity between faces in shell
class ShapeFix_FaceConnect 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ShapeFix_FaceConnect();
  
  Standard_EXPORT Standard_Boolean Add (const TopoDS_Face& aFirst, const TopoDS_Face& aSecond);
  
  Standard_EXPORT TopoDS_Shell Build (const TopoDS_Shell& shell, const Standard_Real sewtoler, const Standard_Real fixtoler);
  
  //! Clears internal data structure
  Standard_EXPORT void Clear();




protected:





private:



  TopTools_DataMapOfShapeListOfShape myConnected;
  TopTools_DataMapOfShapeListOfShape myOriFreeEdges;
  TopTools_DataMapOfShapeListOfShape myResFreeEdges;
  TopTools_DataMapOfShapeListOfShape myResSharEdges;


};







#endif // _ShapeFix_FaceConnect_HeaderFile
