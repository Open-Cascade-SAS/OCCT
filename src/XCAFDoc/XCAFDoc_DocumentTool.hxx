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

#include <TDataStd_GenericEmpty.hxx>
#include <Standard_Boolean.hxx>
#include <UnitsMethods_LengthUnit.hxx>

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
class XCAFDoc_VisMaterialTool;

class XCAFDoc_DocumentTool;
DEFINE_STANDARD_HANDLE(XCAFDoc_DocumentTool, TDataStd_GenericEmpty)

//! Defines sections structure of an XDE document.
//! attribute marking CAF document as being DECAF document.
//! Creates the sections structure of the document.
class XCAFDoc_DocumentTool : public TDataStd_GenericEmpty
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
  Standard_EXPORT static Handle(XCAFDoc_DocumentTool) Set(
    const TDF_Label&       L,
    const Standard_Boolean IsAcces = Standard_True);

  Standard_EXPORT static Standard_Boolean IsXCAFDocument(const Handle(TDocStd_Document)& Doc);

  //! Returns label where the DocumentTool attribute is or
  //! 0.1 if DocumentTool is not yet set.
  Standard_EXPORT static TDF_Label DocLabel(const TDF_Label& acces);

  //! Returns sub-label of DocLabel() with tag 1.
  Standard_EXPORT static TDF_Label ShapesLabel(const TDF_Label& acces);

  //! Returns sub-label of DocLabel() with tag 2.
  Standard_EXPORT static TDF_Label ColorsLabel(const TDF_Label& acces);

  //! Returns sub-label of DocLabel() with tag 3.
  Standard_EXPORT static TDF_Label LayersLabel(const TDF_Label& acces);

  //! Returns sub-label of DocLabel() with tag 4.
  Standard_EXPORT static TDF_Label DGTsLabel(const TDF_Label& acces);

  //! Returns sub-label of DocLabel() with tag 5.
  Standard_EXPORT static TDF_Label MaterialsLabel(const TDF_Label& acces);

  //! Returns sub-label of DocLabel() with tag 7.
  Standard_EXPORT static TDF_Label ViewsLabel(const TDF_Label& acces);

  //! Returns sub-label of DocLabel() with tag 8.
  Standard_EXPORT static TDF_Label ClippingPlanesLabel(const TDF_Label& acces);

  //! Returns sub-label of DocLabel() with tag 9.
  Standard_EXPORT static TDF_Label NotesLabel(const TDF_Label& acces);

  //! Returns sub-label of DocLabel() with tag 10.
  Standard_EXPORT static TDF_Label VisMaterialLabel(const TDF_Label& theLabel);

  //! Creates (if it does not exist) ShapeTool attribute on ShapesLabel().
  Standard_EXPORT static Handle(XCAFDoc_ShapeTool) ShapeTool(const TDF_Label& acces);

  //! Checks for the ShapeTool attribute on the label's document
  //! Returns TRUE if Tool exists, ELSE if it has not been created
  Standard_EXPORT static Standard_Boolean CheckShapeTool(const TDF_Label& theAcces);

  //! Creates (if it does not exist) ColorTool attribute on ColorsLabel().
  Standard_EXPORT static Handle(XCAFDoc_ColorTool) ColorTool(const TDF_Label& acces);

  //! Checks for the ColorTool attribute on the label's document
  //! Returns TRUE if Tool exists, ELSE if it has not been created
  Standard_EXPORT static Standard_Boolean CheckColorTool(const TDF_Label& theAcces);

  //! Creates (if it does not exist) XCAFDoc_VisMaterialTool attribute on VisMaterialLabel().
  //! Should not be confused with MaterialTool() defining physical/manufacturing materials.
  Standard_EXPORT static Handle(XCAFDoc_VisMaterialTool) VisMaterialTool(const TDF_Label& theLabel);

  //! Checks for the VisMaterialTool attribute on the label's document
  //! Returns TRUE if Tool exists, ELSE if it has not been created
  Standard_EXPORT static Standard_Boolean CheckVisMaterialTool(const TDF_Label& theAcces);

  //! Creates (if it does not exist) LayerTool attribute on LayersLabel().
  Standard_EXPORT static Handle(XCAFDoc_LayerTool) LayerTool(const TDF_Label& acces);

  //! Checks for the LayerTool attribute on the label's document
  //! Returns TRUE if Tool exists, ELSE if it has not been created
  Standard_EXPORT static Standard_Boolean CheckLayerTool(const TDF_Label& theAcces);

  //! Creates (if it does not exist) DimTolTool attribute on DGTsLabel().
  Standard_EXPORT static Handle(XCAFDoc_DimTolTool) DimTolTool(const TDF_Label& acces);

  //! Checks for the DimTolTool attribute on the label's document
  //! Returns TRUE if Tool exists, ELSE if it has not been created
  Standard_EXPORT static Standard_Boolean CheckDimTolTool(const TDF_Label& theAcces);

  //! Creates (if it does not exist) DimTolTool attribute on DGTsLabel().
  Standard_EXPORT static Handle(XCAFDoc_MaterialTool) MaterialTool(const TDF_Label& acces);

  //! Checks for the MaterialTool attribute on the label's document
  //! Returns TRUE if Tool exists, ELSE if it has not been created
  Standard_EXPORT static Standard_Boolean CheckMaterialTool(const TDF_Label& theAcces);

  //! Creates (if it does not exist) ViewTool attribute on ViewsLabel().
  Standard_EXPORT static Handle(XCAFDoc_ViewTool) ViewTool(const TDF_Label& acces);

  //! Checks for the ViewTool attribute on the label's document
  //! Returns TRUE if Tool exists, ELSE if it has not been created
  Standard_EXPORT static Standard_Boolean CheckViewTool(const TDF_Label& theAcces);

  //! Creates (if it does not exist) ClippingPlaneTool attribute on ClippingPlanesLabel().
  Standard_EXPORT static Handle(XCAFDoc_ClippingPlaneTool) ClippingPlaneTool(
    const TDF_Label& acces);

  //! Checks for the ClippingPlaneTool attribute on the label's document
  //! Returns TRUE if Tool exists, ELSE if it has not been created
  Standard_EXPORT static Standard_Boolean CheckClippingPlaneTool(const TDF_Label& theAcces);

  //! Creates (if it does not exist) NotesTool attribute on NotesLabel().
  Standard_EXPORT static Handle(XCAFDoc_NotesTool) NotesTool(const TDF_Label& acces);

  //! Checks for the NotesTool attribute on the label's document
  //! Returns TRUE if Tool exists, ELSE if it has not been created
  Standard_EXPORT static Standard_Boolean CheckNotesTool(const TDF_Label& theAcces);

  //! Returns value of current internal unit for the document
  //! converted to base unit type.
  Standard_EXPORT static Standard_Boolean GetLengthUnit(const Handle(TDocStd_Document)& theDoc,
                                                        Standard_Real&                  theResut,
                                                        const UnitsMethods_LengthUnit theBaseUnit);

  //! Returns value of current internal unit for the document in meter
  Standard_EXPORT static Standard_Boolean GetLengthUnit(const Handle(TDocStd_Document)& theDoc,
                                                        Standard_Real&                  theResut);

  //! Sets value of current internal unit to the document in meter
  Standard_EXPORT static void SetLengthUnit(const Handle(TDocStd_Document)& theDoc,
                                            const Standard_Real             theUnitValue);

  //! Sets value of current internal unit to the document
  //! @param theUnitValue must be represented in the base unit type
  Standard_EXPORT static void SetLengthUnit(const Handle(TDocStd_Document)& theDoc,
                                            const Standard_Real             theUnitValue,
                                            const UnitsMethods_LengthUnit   theBaseUnit);

public:
  Standard_EXPORT XCAFDoc_DocumentTool();

  //! to be called when reading this attribute from file
  Standard_EXPORT void Init() const;

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  //! To init this derived attribute after the attribute restore using the base restore-methods
  Standard_EXPORT Standard_Boolean AfterRetrieval(const Standard_Boolean forceIt = Standard_False)
    Standard_OVERRIDE;

  DEFINE_DERIVED_ATTRIBUTE(XCAFDoc_DocumentTool, TDataStd_GenericEmpty)
};

#endif // _XCAFDoc_DocumentTool_HeaderFile
