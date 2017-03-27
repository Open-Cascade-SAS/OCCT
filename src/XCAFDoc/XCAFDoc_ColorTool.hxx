// Created on: 2000-05-11
// Created by: Edward AGAPOV
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

#ifndef _XCAFDoc_ColorTool_HeaderFile
#define _XCAFDoc_ColorTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <TDF_LabelSequence.hxx>
#include <XCAFDoc_ColorType.hxx>
class XCAFDoc_ShapeTool;
class TDF_Label;
class Standard_GUID;
class Quantity_Color;
class Quantity_ColorRGBA;
class TopoDS_Shape;
class TDF_Attribute;
class TDF_RelocationTable;


class XCAFDoc_ColorTool;
DEFINE_STANDARD_HANDLE(XCAFDoc_ColorTool, TDF_Attribute)

//! Provides tools to store and retrieve attributes (colors)
//! of TopoDS_Shape in and from TDocStd_Document
//! A Document is intended to hold different
//! attributes of ONE shape and it's sub-shapes
//! Provide tools for management of Colors section of document.
class XCAFDoc_ColorTool : public TDF_Attribute
{

public:

  
  Standard_EXPORT XCAFDoc_ColorTool();
  
  //! Creates (if not exist) ColorTool.
  Standard_EXPORT static Handle(XCAFDoc_ColorTool) Set (const TDF_Label& L);
  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! returns the label under which colors are stored
  Standard_EXPORT TDF_Label BaseLabel() const;
  
  //! Returns internal XCAFDoc_ShapeTool tool
  Standard_EXPORT const Handle(XCAFDoc_ShapeTool)& ShapeTool();
  
  //! Returns True if label belongs to a colortable and
  //! is a color definition
  Standard_EXPORT Standard_Boolean IsColor (const TDF_Label& lab) const;
  
  //! Returns color defined by label lab
  //! Returns False if the label is not in colortable
  //! or does not define a color
  Standard_EXPORT Standard_Boolean GetColor (const TDF_Label& lab, Quantity_Color& col) const;

  //! Returns color defined by label lab
  //! Returns False if the label is not in colortable
  //! or does not define a color
  Standard_EXPORT Standard_Boolean GetColor(const TDF_Label& lab, Quantity_ColorRGBA& col) const;
  
  //! Finds a color definition in a colortable and returns
  //! its label if found
  //! Returns False if color is not found in colortable
  Standard_EXPORT Standard_Boolean FindColor (const Quantity_Color& col, TDF_Label& lab) const;

  //! Finds a color definition in a colortable and returns
  //! its label if found
  //! Returns False if color is not found in colortable
  Standard_EXPORT Standard_Boolean FindColor(const Quantity_ColorRGBA& col, TDF_Label& lab) const;
  
  //! Finds a color definition in a colortable and returns
  //! its label if found (or Null label else)
  Standard_EXPORT TDF_Label FindColor (const Quantity_Color& col) const;

  //! Finds a color definition in a colortable and returns
  //! its label if found (or Null label else)
  Standard_EXPORT TDF_Label FindColor(const Quantity_ColorRGBA& col) const;
  
  //! Adds a color definition to a colortable and returns
  //! its label (returns existing label if the same color
  //! is already defined)
  Standard_EXPORT TDF_Label AddColor (const Quantity_Color& col) const;

  //! Adds a color definition to a colortable and returns
  //! its label (returns existing label if the same color
  //! is already defined)
  Standard_EXPORT TDF_Label AddColor(const Quantity_ColorRGBA& col) const;
  
  //! Removes color from the colortable
  Standard_EXPORT void RemoveColor (const TDF_Label& lab) const;
  
  //! Returns a sequence of colors currently stored
  //! in the colortable
  Standard_EXPORT void GetColors (TDF_LabelSequence& Labels) const;
  
  //! Sets a link with GUID defined by <type> (see
  //! XCAFDoc::ColorRefGUID()) from label <L> to color
  //! defined by <colorL>. Color of shape is defined following way
  //! in dependance with type of color.
  //! If type of color is XCAFDoc_ColorGen - then this color
  //! defines default color for surfaces and curves.
  //! If for shape color with types XCAFDoc_ColorSurf or XCAFDoc_ColorCurv is specified
  //! then such color overrides generic color.
  Standard_EXPORT void SetColor (const TDF_Label& L, const TDF_Label& colorL, const XCAFDoc_ColorType type) const;
  
  //! Sets a link with GUID defined by <type> (see
  //! XCAFDoc::ColorRefGUID()) from label <L> to color <Color>
  //! in the colortable
  //! Adds a color as necessary
  Standard_EXPORT void SetColor (const TDF_Label& L, const Quantity_Color& Color, const XCAFDoc_ColorType type) const;

  //! Sets a link with GUID defined by <type> (see
  //! XCAFDoc::ColorRefGUID()) from label <L> to color <Color>
  //! in the colortable
  //! Adds a color as necessary
  Standard_EXPORT void SetColor(const TDF_Label& L, const Quantity_ColorRGBA& Color, const XCAFDoc_ColorType type) const;
  
  //! Removes a link with GUID defined by <type> (see
  //! XCAFDoc::ColorRefGUID()) from label <L> to color
  Standard_EXPORT void UnSetColor (const TDF_Label& L, const XCAFDoc_ColorType type) const;
  
  //! Returns True if label <L> has a color assignment
  //! of the type <type>
  Standard_EXPORT Standard_Boolean IsSet (const TDF_Label& L, const XCAFDoc_ColorType type) const;
  
  //! Returns label with color assigned to <L> as <type>
  //! Returns False if no such color is assigned
  Standard_EXPORT static Standard_Boolean GetColor (const TDF_Label& L, const XCAFDoc_ColorType type, TDF_Label& colorL);
  
  //! Returns color assigned to <L> as <type>
  //! Returns False if no such color is assigned
  Standard_EXPORT Standard_Boolean GetColor (const TDF_Label& L, const XCAFDoc_ColorType type, Quantity_Color& color);

  //! Returns color assigned to <L> as <type>
  //! Returns False if no such color is assigned
  Standard_EXPORT Standard_Boolean GetColor(const TDF_Label& L, const XCAFDoc_ColorType type, Quantity_ColorRGBA& color);
  
  //! Sets a link with GUID defined by <type> (see
  //! XCAFDoc::ColorRefGUID()) from label <L> to color
  //! defined by <colorL>
  //! Returns False if cannot find a label for shape S
  Standard_EXPORT Standard_Boolean SetColor (const TopoDS_Shape& S, const TDF_Label& colorL, const XCAFDoc_ColorType type);
  
  //! Sets a link with GUID defined by <type> (see
  //! XCAFDoc::ColorRefGUID()) from label <L> to color <Color>
  //! in the colortable
  //! Adds a color as necessary
  //! Returns False if cannot find a label for shape S
  Standard_EXPORT Standard_Boolean SetColor (const TopoDS_Shape& S, const Quantity_Color& Color, const XCAFDoc_ColorType type);

  //! Sets a link with GUID defined by <type> (see
  //! XCAFDoc::ColorRefGUID()) from label <L> to color <Color>
  //! in the colortable
  //! Adds a color as necessary
  //! Returns False if cannot find a label for shape S
  Standard_EXPORT Standard_Boolean SetColor(const TopoDS_Shape& S, const Quantity_ColorRGBA& Color, const XCAFDoc_ColorType type);
  
  //! Removes a link with GUID defined by <type> (see
  //! XCAFDoc::ColorRefGUID()) from label <L> to color
  //! Returns True if such link existed
  Standard_EXPORT Standard_Boolean UnSetColor (const TopoDS_Shape& S, const XCAFDoc_ColorType type);
  
  //! Returns True if label <L> has a color assignment
  //! of the type <type>
  Standard_EXPORT Standard_Boolean IsSet (const TopoDS_Shape& S, const XCAFDoc_ColorType type);
  
  //! Returns label with color assigned to <L> as <type>
  //! Returns False if no such color is assigned
  Standard_EXPORT Standard_Boolean GetColor (const TopoDS_Shape& S, const XCAFDoc_ColorType type, TDF_Label& colorL);
  
  //! Returns color assigned to <L> as <type>
  //! Returns False if no such color is assigned
  Standard_EXPORT Standard_Boolean GetColor (const TopoDS_Shape& S, const XCAFDoc_ColorType type, Quantity_Color& color);

  //! Returns color assigned to <L> as <type>
  //! Returns False if no such color is assigned
  Standard_EXPORT Standard_Boolean GetColor(const TopoDS_Shape& S, const XCAFDoc_ColorType type, Quantity_ColorRGBA& color);
  
  //! Return TRUE if object on this label is visible, FALSE if invisible.
  Standard_EXPORT Standard_Boolean IsVisible (const TDF_Label& L) const;
  
  //! Set the visibility of object on label. Do nothing if there no any object.
  //! Set UAttribute with corresponding GUID.
  Standard_EXPORT void SetVisibility (const TDF_Label& shapeLabel, const Standard_Boolean isvisible = Standard_True);
  
  //! Sets the color of component that styled with SHUO structure
  //! Returns FALSE if no sush component found
  //! NOTE: create SHUO structeure if it is necessary and if <isCreateSHUO>
  Standard_EXPORT Standard_Boolean SetInstanceColor (const TopoDS_Shape& theShape, const XCAFDoc_ColorType type, const Quantity_Color& color, const Standard_Boolean isCreateSHUO = Standard_True);
  
  //! Sets the color of component that styled with SHUO structure
  //! Returns FALSE if no sush component found
  //! NOTE: create SHUO structeure if it is necessary and if <isCreateSHUO>
  Standard_EXPORT Standard_Boolean SetInstanceColor(const TopoDS_Shape& theShape, const XCAFDoc_ColorType type, const Quantity_ColorRGBA& color, const Standard_Boolean isCreateSHUO = Standard_True);

  //! Gets the color of component that styled with SHUO structure
  //! Returns FALSE if no sush component or color type
  Standard_EXPORT Standard_Boolean GetInstanceColor (const TopoDS_Shape& theShape, const XCAFDoc_ColorType type, Quantity_Color& color);

  //! Gets the color of component that styled with SHUO structure
  //! Returns FALSE if no sush component or color type
  Standard_EXPORT Standard_Boolean GetInstanceColor(const TopoDS_Shape& theShape, const XCAFDoc_ColorType type, Quantity_ColorRGBA& color);
  
  //! Gets the visibility status of component that styled with SHUO structure
  //! Returns FALSE if no sush component
  Standard_EXPORT Standard_Boolean IsInstanceVisible (const TopoDS_Shape& theShape);
  
  //! Reverses order in chains of TreeNodes (from Last to First) under
  //! each Color Label since we became to use function ::Prepend()
  //! instead of ::Append() in method SetColor() for acceleration
  Standard_EXPORT Standard_Boolean ReverseChainsOfTreeNodes();
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XCAFDoc_ColorTool,TDF_Attribute)

protected:




private:


  Handle(XCAFDoc_ShapeTool) myShapeTool;


};







#endif // _XCAFDoc_ColorTool_HeaderFile
