// Created on: 1999-10-25
// Created by: Sergey ZARITCHNY <szy@philipox.nnov.matra-dtv.fr>
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

#ifndef _QANewBRepNaming_Loader_HeaderFile
#define _QANewBRepNaming_Loader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_MapOfShape.hxx>
class BRepBuilderAPI_MakeShape;
class TopoDS_Shape;
class TNaming_Builder;
class TDF_Label;



class QANewBRepNaming_Loader 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Load in   the naming data-structure   the  shape
  //! generated  from FACE,  EDGE, VERTEX,..., after the
  //! MakeShape   operation.  <ShapeIn>  is  the initial
  //! shape.   <GeneratedFrom>   defines  the   kind  of
  //! shape generation    to    record  in   the  naming
  //! data-structure. The <builder> is used to store the
  //! set of evolutions in the data-framework of TDF.
  Standard_EXPORT static void LoadGeneratedShapes (BRepBuilderAPI_MakeShape& MakeShape, const TopoDS_Shape& ShapeIn, const TopAbs_ShapeEnum GeneratedFrom, TNaming_Builder& Buider);
  
  //! Load in the naming data-structure the shape
  //! modified from FACE, EDGE, VERTEX,...,
  //! after the MakeShape operation.
  //! <ShapeIn> is the initial shape.
  //! <ModifiedFrom> defines the kind of shape modification
  //! to record in the naming data-structure.
  //! The <builder> is used to store the set of evolutions
  //! in the data-framework of TDF.
  Standard_EXPORT static void LoadModifiedShapes (BRepBuilderAPI_MakeShape& MakeShape, const TopoDS_Shape& ShapeIn, const TopAbs_ShapeEnum ModifiedFrom, TNaming_Builder& Buider, const Standard_Boolean theBool = Standard_False);
  
  //! Load in the naming data-structure the shape
  //! deleted after the MakeShape operation.
  //! <ShapeIn> is the initial shape.
  //! <KindOfDeletedShape> defines the kind of
  //! deletion to record in the naming data-structure.
  //! The <builder> is used to store the set of evolutions
  //! in the data-framework of TDF.
  Standard_EXPORT static void LoadDeletedShapes (BRepBuilderAPI_MakeShape& MakeShape, const TopoDS_Shape& ShapeIn, const TopAbs_ShapeEnum KindOfDeletedShape, TNaming_Builder& Buider);
  
  //! The same as LoadGeneratedShapes plus performs orientation of
  //! loaded shapes according orientation of SubShapes
  Standard_EXPORT static void LoadAndOrientGeneratedShapes (BRepBuilderAPI_MakeShape& MakeShape, const TopoDS_Shape& ShapeIn, const TopAbs_ShapeEnum GeneratedFrom, TNaming_Builder& Buider, const TopTools_DataMapOfShapeShape& SubShapesOfResult);
  
  //! The same as LoadModifiedShapes plus performs orientation of
  //! loaded shapes according orientation of SubShapes
  Standard_EXPORT static void LoadAndOrientModifiedShapes (BRepBuilderAPI_MakeShape& MakeShape, const TopoDS_Shape& ShapeIn, const TopAbs_ShapeEnum ModifiedFrom, TNaming_Builder& Buider, const TopTools_DataMapOfShapeShape& SubShapesOfResult);
  
  Standard_EXPORT static void ModifyPart (const TopoDS_Shape& PartShape, const TopoDS_Shape& Primitive, const TDF_Label& Label);
  
  Standard_EXPORT static Standard_Boolean HasDangleShapes (const TopoDS_Shape& ShapeIn);
  
  Standard_EXPORT static void LoadGeneratedDangleShapes (const TopoDS_Shape& ShapeIn, const TopAbs_ShapeEnum GeneratedFrom, TNaming_Builder& GenBuider);
  
  Standard_EXPORT static void LoadGeneratedDangleShapes (const TopoDS_Shape& ShapeIn, const TopAbs_ShapeEnum GeneratedFrom, const TopTools_MapOfShape& OnlyThese, TNaming_Builder& GenBuider);
  
  Standard_EXPORT static void LoadModifiedDangleShapes (BRepBuilderAPI_MakeShape& MakeShape, const TopoDS_Shape& ShapeIn, const TopAbs_ShapeEnum GeneratedFrom, TNaming_Builder& GenBuider);
  
  Standard_EXPORT static void LoadDeletedDangleShapes (BRepBuilderAPI_MakeShape& MakeShape, const TopoDS_Shape& ShapeIn, const TopAbs_ShapeEnum ShapeType, TNaming_Builder& DelBuider);
  
  Standard_EXPORT static void LoadDangleShapes (const TopoDS_Shape& theShape, const TDF_Label& theLabelGenerator);
  
  Standard_EXPORT static void LoadDangleShapes (const TopoDS_Shape& theShape, const TopoDS_Shape& ignoredShape, const TDF_Label& theLabelGenerator);
  
  //! Returns dangle sub shapes Generator - Dangle.
  Standard_EXPORT static Standard_Boolean GetDangleShapes (const TopoDS_Shape& ShapeIn, const TopAbs_ShapeEnum GeneratedFrom, TopTools_DataMapOfShapeShape& Dangles);
  
  //! Returns dangle sub shapes.
  Standard_EXPORT static Standard_Boolean GetDangleShapes (const TopoDS_Shape& ShapeIn, const TopAbs_ShapeEnum GeneratedFrom, TopTools_MapOfShape& Dangles);
  
  Standard_EXPORT static Standard_Boolean IsDangle (const TopoDS_Shape& theDangle, const TopoDS_Shape& theShape);




protected:





private:





};







#endif // _QANewBRepNaming_Loader_HeaderFile
