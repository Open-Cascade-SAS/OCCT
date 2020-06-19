// Created on: 2003-03-05
// Created by: Sergey KUUL
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_MaterialTool_HeaderFile
#define _XCAFDoc_MaterialTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDataStd_GenericEmpty.hxx>
#include <Standard_Boolean.hxx>
#include <TDF_LabelSequence.hxx>
#include <Standard_Real.hxx>
class XCAFDoc_ShapeTool;
class TDF_Label;
class Standard_GUID;
class TCollection_HAsciiString;
class TDF_Attribute;
class TDF_RelocationTable;


class XCAFDoc_MaterialTool;
DEFINE_STANDARD_HANDLE(XCAFDoc_MaterialTool, TDataStd_GenericEmpty)

//! Provides tools to store and retrieve attributes (materials)
//! of TopoDS_Shape in and from TDocStd_Document
//! A Document is intended to hold different
//! attributes of ONE shape and it's sub-shapes
//! Provide tools for management of Materialss section of document.
class XCAFDoc_MaterialTool : public TDataStd_GenericEmpty
{

public:

  
  Standard_EXPORT XCAFDoc_MaterialTool();
  
  //! Creates (if not exist) MaterialTool.
  Standard_EXPORT static Handle(XCAFDoc_MaterialTool) Set (const TDF_Label& L);
  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! returns the label under which colors are stored
  Standard_EXPORT TDF_Label BaseLabel() const;
  
  //! Returns internal XCAFDoc_ShapeTool tool
  Standard_EXPORT const Handle(XCAFDoc_ShapeTool)& ShapeTool();
  
  //! Returns True if label belongs to a material table and
  //! is a Material definition
  Standard_EXPORT Standard_Boolean IsMaterial (const TDF_Label& lab) const;
  
  //! Returns a sequence of materials currently stored
  //! in the material table
  Standard_EXPORT void GetMaterialLabels (TDF_LabelSequence& Labels) const;
  
  //! Adds a Material definition to a table and returns its label
  Standard_EXPORT TDF_Label AddMaterial (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Standard_Real aDensity, const Handle(TCollection_HAsciiString)& aDensName, const Handle(TCollection_HAsciiString)& aDensValType) const;
  
  //! Sets a link with GUID
  Standard_EXPORT void SetMaterial (const TDF_Label& L, const TDF_Label& MatL) const;
  
  //! Sets a link with GUID
  //! Adds a Material as necessary
  Standard_EXPORT void SetMaterial (const TDF_Label& L, const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Standard_Real aDensity, const Handle(TCollection_HAsciiString)& aDensName, const Handle(TCollection_HAsciiString)& aDensValType) const;
  
  //! Returns Material assigned to <MatL>
  //! Returns False if no such Material is assigned
  Standard_EXPORT Standard_Boolean GetMaterial (const TDF_Label& MatL, Handle(TCollection_HAsciiString)& aName, Handle(TCollection_HAsciiString)& aDescription, Standard_Real& aDensity, Handle(TCollection_HAsciiString)& aDensName, Handle(TCollection_HAsciiString)& aDensValType) const;
  
  //! Find referred material and return density from it
  //! if no material --> return 0
  Standard_EXPORT static Standard_Real GetDensityForShape (const TDF_Label& ShapeL);
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;


  DEFINE_DERIVED_ATTRIBUTE(XCAFDoc_MaterialTool,TDataStd_GenericEmpty)


private:


  Handle(XCAFDoc_ShapeTool) myShapeTool;


};







#endif // _XCAFDoc_MaterialTool_HeaderFile
