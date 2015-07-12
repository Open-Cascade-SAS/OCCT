// Created on: 2001-11-26
// Created by: Sergey ZARITCHNY
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _QANewBRepNaming_Gluing_HeaderFile
#define _QANewBRepNaming_Gluing_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TDF_LabelMap.hxx>
#include <QANewBRepNaming_TopNaming.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
class Standard_NullObject;
class TDF_Label;
class QANewModTopOpe_Glue;
class TopoDS_Shape;


//! Loads a result of Gluing operation in  Data Framework
class QANewBRepNaming_Gluing  : public QANewBRepNaming_TopNaming
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT QANewBRepNaming_Gluing();
  
  Standard_EXPORT QANewBRepNaming_Gluing(const TDF_Label& theResultLabel);
  
  Standard_EXPORT void Init (const TDF_Label& theResultLabel);
  
  //! Loads a Gluing in a data framework
  Standard_EXPORT void Load (QANewModTopOpe_Glue& theMkGluing);
  
  Standard_EXPORT TDF_Label Content() const;
  
  Standard_EXPORT void SetContext (const TopoDS_Shape& theObject, const TopoDS_Shape& theTool);
  
  Standard_EXPORT void SetLog (const TDF_LabelMap& theLog);




protected:





private:

  
  //! If the shape is a compound the method
  //! returns the underlying shape type.
  Standard_EXPORT TopAbs_ShapeEnum ShapeType (const TopoDS_Shape& theShape) const;
  
  //! If the shape is a compound the method
  //! returns the underlying shape.
  Standard_EXPORT TopoDS_Shape GetShape (const TopoDS_Shape& theShape) const;
  
  //! A default implementation for naming of modified shapes of the operation
  Standard_EXPORT void LoadModifiedShapes (QANewModTopOpe_Glue& theMakeShape) const;
  
  //! A default implementation for naming of modified shapes of the operation
  Standard_EXPORT void LoadDeletedShapes (QANewModTopOpe_Glue& theMakeShape) const;
  
  //! Loads the content of the result.
  Standard_EXPORT void LoadContent (QANewModTopOpe_Glue& theMakeShape) const;
  
  //! Loads the result.
  Standard_EXPORT void LoadResult (QANewModTopOpe_Glue& theMakeShape) const;
  
  //! Returns true if the result is not the same as the object shape.
  Standard_EXPORT Standard_Boolean IsResultChanged (QANewModTopOpe_Glue& theMakeShape) const;
  
  //! Loads the deletion of the degenerated edges.
  Standard_EXPORT void LoadSectionEdges (QANewModTopOpe_Glue& theMakeShape) const;
  
  Standard_EXPORT void AddToTheUnique (const TopoDS_Shape& theUnique, const TopoDS_Shape& theIdentifier);
  
  //! Reset myShared map - shapes, which are modified by both object and tool.
  Standard_EXPORT void RecomputeUnique (QANewModTopOpe_Glue& theMakeShape);
  
  Standard_EXPORT void LoadSourceShapes (TopTools_DataMapOfShapeInteger& theSources) const;
  
  //! A default implementation for naming of generated  shapes of the operation
  Standard_EXPORT void LoadUniqueShapes (QANewModTopOpe_Glue& theMakeShape, const TopTools_DataMapOfShapeInteger& theSources);


  TopTools_IndexedDataMapOfShapeListOfShape myUnique;
  TopoDS_Shape myContext;
  TDF_LabelMap myLog;


};







#endif // _QANewBRepNaming_Gluing_HeaderFile
