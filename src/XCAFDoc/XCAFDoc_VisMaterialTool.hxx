// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_VisMaterialTool_HeaderFile
#define _XCAFDoc_VisMaterialTool_HeaderFile

#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_LabelSequence.hxx>

class TopoDS_Shape;
class Quantity_ColorRGBA;
class XCAFDoc_ShapeTool;
class XCAFDoc_VisMaterial;

//! Provides tools to store and retrieve attributes (visualization materials) of TopoDS_Shape in and from TDocStd_Document.
//!
//! This attribute defines the list of visualization materials (XCAFDoc_VisMaterial) within the whole document.
//! Particular material is assigned to the shape through tree-nodes links.
//!
//! Visualization materials might co-exists with independent color attributes (XCAFDoc_ColorTool),
//! but beware to preserve consistency between them (it is better using one attribute type at once to avoid ambiguity).
//! Unlike color attributes, list of materials should be managed explicitly by application,
//! so that there is no tool eliminating material duplicates or removing unused materials.
//!
//! @sa XCAFDoc_VisMaterial
class XCAFDoc_VisMaterialTool : public TDF_Attribute
{
  DEFINE_STANDARD_RTTIEXT(XCAFDoc_VisMaterialTool, TDF_Attribute)
public:

  //! Creates (if not exist) ColorTool.
  Standard_EXPORT static Handle(XCAFDoc_VisMaterialTool) Set (const TDF_Label& L);

  Standard_EXPORT static const Standard_GUID& GetID();

public:
  //! Empty constructor.
  Standard_EXPORT XCAFDoc_VisMaterialTool();

  //! returns the label under which colors are stored
  Standard_EXPORT TDF_Label BaseLabel() const { return Label(); }

  //! Returns internal XCAFDoc_ShapeTool tool
  Standard_EXPORT const Handle(XCAFDoc_ShapeTool)& ShapeTool();

  //! Returns TRUE if Label belongs to a Material Table.
  Standard_Boolean IsMaterial (const TDF_Label& theLabel) const { return !GetMaterial (theLabel).IsNull(); }

  //! Returns Material defined by specified Label, or NULL if the label is not in Material Table.
  Standard_EXPORT Handle(XCAFDoc_VisMaterial) GetMaterial (const TDF_Label& theMatLabel) const;

  //! Adds Material definition to a Material Table and returns its Label.
  Standard_EXPORT TDF_Label AddMaterial (const Handle(XCAFDoc_VisMaterial)& theMat,
                                         const TCollection_AsciiString& theName) const;

  //! Adds Material definition to a Material Table and returns its Label.
  Standard_EXPORT TDF_Label AddMaterial(const TCollection_AsciiString& theName) const;

  //! Removes Material from the Material Table
  Standard_EXPORT void RemoveMaterial (const TDF_Label& theLabel) const;

  //! Returns a sequence of Materials currently stored in the Material Table.
  Standard_EXPORT void GetMaterials (TDF_LabelSequence& Labels) const;

  //! Sets new material to the shape.
  Standard_EXPORT void SetShapeMaterial (const TDF_Label& theShapeLabel,
                                         const TDF_Label& theMaterialLabel) const;

  //! Removes a link with GUID XCAFDoc::VisMaterialRefGUID() from shape label to material.
  Standard_EXPORT void UnSetShapeMaterial (const TDF_Label& theShapeLabel) const;

  //! Returns TRUE if label has a material assignment.
  Standard_EXPORT Standard_Boolean IsSetShapeMaterial (const TDF_Label& theLabel) const;

  //! Returns label with material assigned to shape label.
  //! @param theShapeLabel [in] shape label
  //! @param theMaterialLabel [out] material label
  //! @return FALSE if no material is assigned
  Standard_EXPORT static Standard_Boolean GetShapeMaterial (const TDF_Label& theShapeLabel, TDF_Label& theMaterialLabel);

  //! Returns material assigned to the shape label.
  Standard_EXPORT Handle(XCAFDoc_VisMaterial) GetShapeMaterial (const TDF_Label& theShapeLabel);

  //! Sets a link with GUID XCAFDoc::VisMaterialRefGUID() from shape label to material label.
  //! @param theShape [in] shape
  //! @param theMaterialLabel [in] material label
  //! @return FALSE if cannot find a label for shape
  Standard_EXPORT Standard_Boolean SetShapeMaterial (const TopoDS_Shape& theShape,
                                                     const TDF_Label& theMaterialLabel);

  //! Removes a link with GUID XCAFDoc::VisMaterialRefGUID() from shape label to material.
  //! @return TRUE if such link existed
  Standard_EXPORT Standard_Boolean UnSetShapeMaterial (const TopoDS_Shape& theShape);

  //! Returns TRUE if shape has a material assignment.
  Standard_EXPORT Standard_Boolean IsSetShapeMaterial (const TopoDS_Shape& theShape);

  //! Returns label with material assigned to shape.
  //! @param theShape [in] shape
  //! @param theMaterialLabel [out] material label
  //! @return FALSE if no material is assigned
  Standard_EXPORT Standard_Boolean GetShapeMaterial (const TopoDS_Shape& theShape, TDF_Label& theMaterialLabel);

  //! Returns material assigned to shape or NULL if not assigned.
  Standard_EXPORT Handle(XCAFDoc_VisMaterial) GetShapeMaterial (const TopoDS_Shape& theShape);

public:

  //! Returns GUID of this attribute type.
  virtual const Standard_GUID& ID() const Standard_OVERRIDE { return GetID(); }

  //! Does nothing.
  virtual void Restore (const Handle(TDF_Attribute)& ) Standard_OVERRIDE {}

  //! Creates new instance of this tool.
  virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE { return new XCAFDoc_VisMaterialTool(); }

  //! Does nothing.
  virtual void Paste (const Handle(TDF_Attribute)& ,
                      const Handle(TDF_RelocationTable)& ) const Standard_OVERRIDE {}

private:

  Handle(XCAFDoc_ShapeTool) myShapeTool;

};

DEFINE_STANDARD_HANDLE(XCAFDoc_VisMaterialTool, TDF_Attribute)

#endif // _XCAFDoc_VisMaterialTool_HeaderFile
