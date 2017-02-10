// Created on: 2000-08-30
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_DocumentTool_HeaderFile
#define _XCAFDoc_DocumentTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
class Standard_GUID;
class TDF_Label;
class TDocStd_Document;
class XCAFDoc_ShapeTool;
class XCAFDoc_ColorTool;
class XCAFDoc_ClippingPlaneTool;
class XCAFDoc_LayerTool;
class XCAFDoc_DimTolTool;
class XCAFDoc_MaterialTool;
class XCAFDoc_NotesTool;
class XCAFDoc_ViewTool;
class TDF_Attribute;
class TDF_RelocationTable;


class XCAFDoc_DocumentTool;
DEFINE_STANDARD_HANDLE(XCAFDoc_DocumentTool, TDF_Attribute)

//! Defines sections structure of an XDE document.
//! attribute marking CAF document as being DECAF document.
//! Creates the sections structure of the document.
class XCAFDoc_DocumentTool : public TDF_Attribute
{

public:

  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Create (if not exist) DocumentTool attribute
  //! on 0.1 label if <IsAcces> is true, else
  //! on <L> label.
  //! This label will be returned by DocLabel();
  //! If the attribute is already set it won't be reset on
  //! <L> even if <IsAcces> is false.
  //! ColorTool and ShapeTool attributes are also set by this method.
  Standard_EXPORT static Handle(XCAFDoc_DocumentTool) Set (const TDF_Label& L, const Standard_Boolean IsAcces = Standard_True);
  
  Standard_EXPORT static Standard_Boolean IsXCAFDocument (const Handle(TDocStd_Document)& Doc);
  
  //! Returns label where the DocumentTool attribute is or
  //! 0.1 if DocumentTool is not yet set.
  Standard_EXPORT static TDF_Label DocLabel (const TDF_Label& acces);
  
  //! Returns sub-label of DocLabel() with tag 1.
  Standard_EXPORT static TDF_Label ShapesLabel (const TDF_Label& acces);
  
  //! Returns sub-label of DocLabel() with tag 2.
  Standard_EXPORT static TDF_Label ColorsLabel (const TDF_Label& acces);
  
  //! Returns sub-label of DocLabel() with tag 3.
  Standard_EXPORT static TDF_Label LayersLabel (const TDF_Label& acces);
  
  //! Returns sub-label of DocLabel() with tag 4.
  Standard_EXPORT static TDF_Label DGTsLabel (const TDF_Label& acces);
  
  //! Returns sub-label of DocLabel() with tag 5.
  Standard_EXPORT static TDF_Label MaterialsLabel (const TDF_Label& acces);

  //! Returns sub-label of DocLabel() with tag 7.
  Standard_EXPORT static TDF_Label ViewsLabel(const TDF_Label& acces);
  
  //! Returns sub-label of DocLabel() with tag 8.
  Standard_EXPORT static TDF_Label ClippingPlanesLabel(const TDF_Label& acces);

  //! Returns sub-label of DocLabel() with tag 9.
  Standard_EXPORT static TDF_Label NotesLabel(const TDF_Label& acces);

  //! Creates (if it does not exist) ShapeTool attribute on ShapesLabel().
  Standard_EXPORT static Handle(XCAFDoc_ShapeTool) ShapeTool (const TDF_Label& acces);
  
  //! Creates (if it does not exist) ColorTool attribute on ColorsLabel().
  Standard_EXPORT static Handle(XCAFDoc_ColorTool) ColorTool (const TDF_Label& acces);
  
  //! Creates (if it does not exist) LayerTool attribute on LayersLabel().
  Standard_EXPORT static Handle(XCAFDoc_LayerTool) LayerTool (const TDF_Label& acces);
  
  //! Creates (if it does not exist) DimTolTool attribute on DGTsLabel().
  Standard_EXPORT static Handle(XCAFDoc_DimTolTool) DimTolTool (const TDF_Label& acces);
  
  //! Creates (if it does not exist) DimTolTool attribute on DGTsLabel().
  Standard_EXPORT static Handle(XCAFDoc_MaterialTool) MaterialTool (const TDF_Label& acces);

  //! Creates (if it does not exist) ViewTool attribute on ViewsLabel().
  Standard_EXPORT static Handle(XCAFDoc_ViewTool) ViewTool(const TDF_Label& acces);
  
  //! Creates (if it does not exist) ClippingPlaneTool attribute on ClippingPlanesLabel().
  Standard_EXPORT static Handle(XCAFDoc_ClippingPlaneTool) ClippingPlaneTool(const TDF_Label& acces);

  //! Creates (if it does not exist) NotesTool attribute on NotesLabel().
  Standard_EXPORT static Handle(XCAFDoc_NotesTool) NotesTool(const TDF_Label& acces);

  Standard_EXPORT XCAFDoc_DocumentTool();
  
  //! to be called when reading this attribute from file
  Standard_EXPORT void Init() const;
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
 
  DEFINE_STANDARD_RTTIEXT(XCAFDoc_DocumentTool,TDF_Attribute)

protected:




private:




};







#endif // _XCAFDoc_DocumentTool_HeaderFile
