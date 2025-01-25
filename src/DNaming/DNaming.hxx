// Created on: 1997-01-08
// Created by: VAUTHIER Jean-Claude
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _DNaming_HeaderFile
#define _DNaming_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Draw_Interpretor.hxx>
class TDataStd_Real;
class TFunction_Function;
class TDataStd_Integer;
class TDataStd_Name;
class TNaming_NamedShape;
class gp_Ax1;
class TDataStd_UAttribute;
class TopoDS_Shape;
class BRepBuilderAPI_MakeShape;
class TNaming_Builder;
class TDF_Label;
class BRepAlgoAPI_BooleanOperation;
class TDF_Data;
class TCollection_AsciiString;

class DNaming
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static Handle(TDataStd_Real) GetReal(
    const Handle(TFunction_Function)& theFunction,
    const Standard_Integer            thePosition);

  Standard_EXPORT static Handle(TDataStd_Integer) GetInteger(
    const Handle(TFunction_Function)& theFunction,
    const Standard_Integer            thePosition);

  Standard_EXPORT static Handle(TDataStd_Name) GetString(
    const Handle(TFunction_Function)& theFunction,
    const Standard_Integer            thePosition);

  Standard_EXPORT static Standard_Boolean ComputeAxis(const Handle(TNaming_NamedShape)& theNS,
                                                      gp_Ax1&                           theAx1);

  Standard_EXPORT static Handle(TNaming_NamedShape) GetFunctionResult(
    const Handle(TFunction_Function)& theFunction);

  Standard_EXPORT static Handle(TDataStd_UAttribute) GetObjectArg(
    const Handle(TFunction_Function)& theFunction,
    const Standard_Integer            thePosition);

  Standard_EXPORT static void SetObjectArg(const Handle(TFunction_Function)&  theFunction,
                                           const Standard_Integer             thePosition,
                                           const Handle(TDataStd_UAttribute)& theNewValue);

  Standard_EXPORT static Handle(TNaming_NamedShape) GetObjectValue(
    const Handle(TDataStd_UAttribute)& theObject);

  Standard_EXPORT static Handle(TFunction_Function) GetLastFunction(
    const Handle(TDataStd_UAttribute)& theObject);

  Standard_EXPORT static Handle(TFunction_Function) GetFirstFunction(
    const Handle(TDataStd_UAttribute)& theObject);

  Standard_EXPORT static Handle(TFunction_Function) GetPrevFunction(
    const Handle(TFunction_Function)& theFunction);

  Standard_EXPORT static Handle(TDataStd_UAttribute) GetObjectFromFunction(
    const Handle(TFunction_Function)& theFunction);

  Standard_EXPORT static Standard_Boolean IsAttachment(
    const Handle(TDataStd_UAttribute)& theObject);

  Standard_EXPORT static Handle(TNaming_NamedShape) GetAttachmentsContext(
    const Handle(TDataStd_UAttribute)& theObject);

  Standard_EXPORT static Standard_Boolean ComputeSweepDir(const TopoDS_Shape& theShape,
                                                          gp_Ax1&             theAxis);

  Standard_EXPORT static void LoadAndOrientModifiedShapes(
    BRepBuilderAPI_MakeShape&           MakeShape,
    const TopoDS_Shape&                 ShapeIn,
    const TopAbs_ShapeEnum              GeneratedFrom,
    TNaming_Builder&                    Builder,
    const TopTools_DataMapOfShapeShape& SubShapesOfResult);

  Standard_EXPORT static void LoadAndOrientGeneratedShapes(
    BRepBuilderAPI_MakeShape&           MakeShape,
    const TopoDS_Shape&                 ShapeIn,
    const TopAbs_ShapeEnum              GeneratedFrom,
    TNaming_Builder&                    Builder,
    const TopTools_DataMapOfShapeShape& SubShapesOfResult);

  Standard_EXPORT static void LoadDeletedShapes(BRepBuilderAPI_MakeShape& MakeShape,
                                                const TopoDS_Shape&       ShapeIn,
                                                const TopAbs_ShapeEnum    KindOfDeletedShape,
                                                TNaming_Builder&          Builder);

  Standard_EXPORT static void LoadResult(const TDF_Label&              theLabel,
                                         BRepAlgoAPI_BooleanOperation& MS);

  Standard_EXPORT static TopoDS_Shape CurrentShape(const Standard_CString  ShapeEntry,
                                                   const Handle(TDF_Data)& Data);

  Standard_EXPORT static void GetShape(const Standard_CString  ShapeEntry,
                                       const Handle(TDF_Data)& Data,
                                       TopTools_ListOfShape&   Shapes);

  //! theStatus = 0  Not  found,
  //! theStatus = 1  One  shape,
  //! theStatus = 2  More than one shape.
  Standard_EXPORT static TCollection_AsciiString GetEntry(const TopoDS_Shape&     Shape,
                                                          const Handle(TDF_Data)& Data,
                                                          Standard_Integer&       theStatus);

  //! Loads the Shape to DF
  Standard_EXPORT static void LoadImportedShape(const TDF_Label&    theResultLabel,
                                                const TopoDS_Shape& theShape);

  //! Reloads sub-shapes of the Shape to DF
  Standard_EXPORT static void LoadPrime(const TDF_Label&    theResultLabel,
                                        const TopoDS_Shape& theShape);

  Standard_EXPORT static void AllCommands(Draw_Interpretor& DI);

  //! commands relatives to NamedShape
  Standard_EXPORT static void BasicCommands(Draw_Interpretor& DI);

  Standard_EXPORT static void ToolsCommands(Draw_Interpretor& DI);

  //! commands relatives to Naming
  Standard_EXPORT static void SelectionCommands(Draw_Interpretor& DI);

  //! commands for  testing Naming
  Standard_EXPORT static void ModelingCommands(Draw_Interpretor& DI);
};

#endif // _DNaming_HeaderFile
