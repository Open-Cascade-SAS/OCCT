// Created on: 1999-06-22
// Created by: data exchange team
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

#ifndef _ShapeProcessAPI_ApplySequence_HeaderFile
#define _ShapeProcessAPI_ApplySequence_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_DataMapOfShapeShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
class ShapeProcess_ShapeContext;
class TopoDS_Shape;


//! Applies one of the sequence read from resource file.
class ShapeProcessAPI_ApplySequence 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an object and loads resource file and sequence of
  //! operators given by their names.
  Standard_EXPORT ShapeProcessAPI_ApplySequence(const Standard_CString rscName, const Standard_CString seqName = "");
  
  //! Returns object for managing resource file and sequence of
  //! operators.
  Standard_EXPORT Handle(ShapeProcess_ShapeContext)& Context();
  
  //! Performs sequence of operators stored in myRsc.
  //! If <fillmap> is True adds history "shape-shape" into myMap
  //! for shape and its subshapes until level <until> (included).
  //! If <until> is TopAbs_SHAPE,  all the subshapes are considered.
  Standard_EXPORT TopoDS_Shape PrepareShape (const TopoDS_Shape& shape, const Standard_Boolean fillmap = Standard_False, const TopAbs_ShapeEnum until = TopAbs_SHAPE);
  
  //! Clears myMap with accumulated history.
  Standard_EXPORT void ClearMap();
  
  //! Returns myMap with accumulated history.
  Standard_EXPORT const TopTools_DataMapOfShapeShape& Map() const;
  
  //! Prints result of preparation onto the messenger of the context.
  //! Note that results can be accumulated from previous preparations
  //! it method ClearMap was not called before PrepareShape.
  Standard_EXPORT void PrintPreparationResult() const;




protected:





private:



  Handle(ShapeProcess_ShapeContext) myContext;
  TopTools_DataMapOfShapeShape myMap;
  TCollection_AsciiString mySeq;


};







#endif // _ShapeProcessAPI_ApplySequence_HeaderFile
