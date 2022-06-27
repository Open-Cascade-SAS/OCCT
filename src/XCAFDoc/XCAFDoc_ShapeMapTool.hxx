// Created on: 2003-08-29
// Created by: data exchange team
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

#ifndef _XCAFDoc_ShapeMapTool_HeaderFile
#define _XCAFDoc_ShapeMapTool_HeaderFile

#include <Standard.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
class Standard_GUID;
class TDF_Label;
class TopoDS_Shape;
class TDF_RelocationTable;


class XCAFDoc_ShapeMapTool;
DEFINE_STANDARD_HANDLE(XCAFDoc_ShapeMapTool, TDF_Attribute)

//! attribute containing map of sub shapes
class XCAFDoc_ShapeMapTool : public TDF_Attribute
{

public:

  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Create (if not exist) ShapeTool from XCAFDoc on <L>.
  Standard_EXPORT static Handle(XCAFDoc_ShapeMapTool) Set (const TDF_Label& L);
  
  //! Creates an empty tool
  Standard_EXPORT XCAFDoc_ShapeMapTool();
  
  //! Checks whether shape <sub> is subshape of shape stored on
  //! label shapeL
  Standard_EXPORT Standard_Boolean IsSubShape (const TopoDS_Shape& sub) const;
  
  //! Sets representation (TopoDS_Shape) for top-level shape
  Standard_EXPORT void SetShape (const TopoDS_Shape& S);
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT const TopTools_IndexedMapOfShape& GetMap() const;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XCAFDoc_ShapeMapTool,TDF_Attribute)

protected:




private:


  TopTools_IndexedMapOfShape myMap;


};







#endif // _XCAFDoc_ShapeMapTool_HeaderFile
