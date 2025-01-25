// Created on: 1997-03-19
// Created by: Yves FRICAUD
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

#ifndef _TNaming_Name_HeaderFile
#define _TNaming_Name_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TNaming_NameType.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TNaming_ListOfNamedShape.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <TDF_Label.hxx>
#include <TopAbs_Orientation.hxx>
#include <TDF_LabelMap.hxx>
class TNaming_NamedShape;
class TDF_RelocationTable;

//! store the arguments of Naming.
class TNaming_Name
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TNaming_Name();

  Standard_EXPORT void Type(const TNaming_NameType aType);

  Standard_EXPORT void ShapeType(const TopAbs_ShapeEnum aType);

  Standard_EXPORT void Shape(const TopoDS_Shape& theShape);

  Standard_EXPORT void Append(const Handle(TNaming_NamedShape)& arg);

  Standard_EXPORT void StopNamedShape(const Handle(TNaming_NamedShape)& arg);

  Standard_EXPORT void Index(const Standard_Integer I);

  Standard_EXPORT void ContextLabel(const TDF_Label& theLab);

  Standard_EXPORT void Orientation(const TopAbs_Orientation theOrientation);

  Standard_EXPORT TNaming_NameType Type() const;

  Standard_EXPORT TopAbs_ShapeEnum ShapeType() const;

  Standard_EXPORT TopoDS_Shape Shape() const;

  Standard_EXPORT const TNaming_ListOfNamedShape& Arguments() const;

  Standard_EXPORT Handle(TNaming_NamedShape) StopNamedShape() const;

  Standard_EXPORT Standard_Integer Index() const;

  Standard_EXPORT const TDF_Label& ContextLabel() const;

  TopAbs_Orientation Orientation() const { return myOrientation; }

  Standard_EXPORT Standard_Boolean Solve(const TDF_Label& aLab, const TDF_LabelMap& Valid) const;

  Standard_EXPORT void Paste(TNaming_Name& into, const Handle(TDF_RelocationTable)& RT) const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:
  TNaming_NameType           myType;
  TopAbs_ShapeEnum           myShapeType;
  TNaming_ListOfNamedShape   myArgs;
  Handle(TNaming_NamedShape) myStop;
  Standard_Integer           myIndex;
  TopoDS_Shape               myShape;
  TDF_Label                  myContextLabel;
  TopAbs_Orientation         myOrientation;
};

#endif // _TNaming_Name_HeaderFile
