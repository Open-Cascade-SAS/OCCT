// Created on: 1999-09-10
// Created by: Andrey BETENEV
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

#ifndef _STEPConstruct_Styles_HeaderFile
#define _STEPConstruct_Styles_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_SequenceOfTransient.hxx>
#include <STEPConstruct_Tool.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <STEPConstruct_DataMapOfAsciiStringTransient.hxx>
#include <STEPConstruct_DataMapOfPointTransient.hxx>
class XSControl_WorkSession;
class StepVisual_StyledItem;
class StepRepr_RepresentationItem;
class StepVisual_PresentationStyleAssignment;
class TopoDS_Shape;
class StepRepr_RepresentationContext;
class StepVisual_MechanicalDesignGeometricPresentationRepresentation;
class StepShape_ContextDependentShapeRepresentation;
class StepRepr_ProductDefinitionShape;
class StepVisual_Colour;
class Quantity_Color;


//! Provides a mechanism for reading and writing shape styles
//! (such as color) to and from the STEP file
//! This tool maintains a list of styles, either taking them
//! from STEP model (reading), or filling it by calls to
//! AddStyle or directly (writing).
//! Some methods deal with general structures of styles and
//! presentations in STEP, but there are methods which deal
//! with particular implementation of colors (as described in RP)
class STEPConstruct_Styles  : public STEPConstruct_Tool
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an empty tool
  Standard_EXPORT STEPConstruct_Styles();
  
  //! Creates a tool and initializes it
  Standard_EXPORT STEPConstruct_Styles(const Handle(XSControl_WorkSession)& WS);
  
  //! Initializes tool; returns True if succeeded
  Standard_EXPORT Standard_Boolean Init (const Handle(XSControl_WorkSession)& WS);
  
  //! Returns number of defined styles
  Standard_EXPORT Standard_Integer NbStyles() const;
  
  //! Returns style with given index
  Standard_EXPORT Handle(StepVisual_StyledItem) Style (const Standard_Integer i) const;

  //! Returns number of override styles
  Standard_EXPORT Standard_Integer NbRootStyles() const;
  
  //! Returns override style with given index
  Standard_EXPORT Handle(StepVisual_StyledItem) RootStyle (const Standard_Integer i) const;
  
  //! Clears all defined styles and PSA sequence
  Standard_EXPORT void ClearStyles();
  
  //! Adds a style to a sequence
  Standard_EXPORT void AddStyle (const Handle(StepVisual_StyledItem)& style);
  
  //! Create a style linking giving PSA to the item, and add it to the
  //! sequence of stored styles. If Override is not Null, then
  //! the resulting style will be of the subtype OverridingStyledItem.
  Standard_EXPORT Handle(StepVisual_StyledItem) AddStyle (const Handle(StepRepr_RepresentationItem)& item, const Handle(StepVisual_PresentationStyleAssignment)& PSA, const Handle(StepVisual_StyledItem)& Override);
  
  //! Create a style linking giving PSA to the Shape, and add it to the
  //! sequence of stored styles. If Override is not Null, then
  //! the resulting style will be of the subtype OverridingStyledItem.
  //! The Sape is used to find corresponding STEP entity by call to
  //! STEPConstruct::FindEntity(), then previous method is called.
  Standard_EXPORT Handle(StepVisual_StyledItem) AddStyle (const TopoDS_Shape& Shape, const Handle(StepVisual_PresentationStyleAssignment)& PSA, const Handle(StepVisual_StyledItem)& Override);
  
  //! Create MDGPR, fill it with all the styles previously defined,
  //! and add it to the model
  Standard_EXPORT Standard_Boolean CreateMDGPR (const Handle(StepRepr_RepresentationContext)& Context,
                                                Handle(StepVisual_MechanicalDesignGeometricPresentationRepresentation)& MDGPR,
                                                Handle(StepData_StepModel)& theStepModel);
  
  //! Create MDGPR, fill it with all the styles previously defined,
  //! and add it to the model
  //! IMPORTANT: <initPDS> must be null when use for NAUO colors
  //! <initPDS> initialised only for SHUO case.
  Standard_EXPORT Standard_Boolean CreateNAUOSRD (const Handle(StepRepr_RepresentationContext)& Context, const Handle(StepShape_ContextDependentShapeRepresentation)& CDSR, const Handle(StepRepr_ProductDefinitionShape)& initPDS);
  
  //! Searches the STEP model for the RepresentationContext in which
  //! given shape is defined. This context (if found) can be used
  //! then in call to CreateMDGPR()
  Standard_EXPORT Handle(StepRepr_RepresentationContext) FindContext (const TopoDS_Shape& Shape) const;
  
  //! Searches the STEP model for the MDGPR or DM entities
  //! (which bring styles) and fills sequence of styles
  Standard_EXPORT Standard_Boolean LoadStyles();
  
  //! Searches the STEP model for the INISIBILITY entities
  //! (which bring styles) and fills out sequence of styles
  Standard_EXPORT Standard_Boolean LoadInvisStyles (Handle(TColStd_HSequenceOfTransient)& InvSyles) const;
  
  //! Create a PresentationStyleAssignment entity which defines
  //! two colors (for filling surfaces and curves)
  //! if isForNAUO true then returns PresentationStyleByContext
  Standard_EXPORT Handle(StepVisual_PresentationStyleAssignment) MakeColorPSA (const Handle(StepRepr_RepresentationItem)& item, const Handle(StepVisual_Colour)& SurfCol, const Handle(StepVisual_Colour)& CurveCol, const Handle(StepVisual_Colour) &RenderCol, const Standard_Real RenderTransp, const Standard_Boolean isForNAUO = Standard_False) const;
  
  //! Returns a PresentationStyleAssignment entity which defines
  //! surface and curve colors as Col. This PSA is either created
  //! or taken from internal map where all PSAs created by this
  //! method are remembered.
  Standard_EXPORT Handle(StepVisual_PresentationStyleAssignment) GetColorPSA (const Handle(StepRepr_RepresentationItem)& item, const Handle(StepVisual_Colour)& Col);
  
  //! Extract color definitions from the style entity
  //! For each type of color supported, result can be either
  //! NULL if it is not defined by that style, or last
  //! definition (if they are 1 or more)
  Standard_EXPORT Standard_Boolean GetColors (const Handle(StepVisual_StyledItem)& style, Handle(StepVisual_Colour)& SurfCol, Handle(StepVisual_Colour)& BoundCol, Handle(StepVisual_Colour)& CurveCol, Handle(StepVisual_Colour)& RenderCol, Standard_Real& RenderTransp, Standard_Boolean& IsComponent) const;
  
  //! Create STEP color entity by given Quantity_Color
  //! The analysis is performed for whether the color corresponds to
  //! one of standard colors predefined in STEP. In that case,
  //! PredefinedColour entity is created instead of RGBColour
  Standard_EXPORT static Handle(StepVisual_Colour) EncodeColor (const Quantity_Color& Col);
  
  //! Create STEP color entity by given Quantity_Color
  //! The analysis is performed for whether the color corresponds to
  //! one of standard colors predefined in STEP. In that case,
  //! PredefinedColour entity is created instead of RGBColour
  Standard_EXPORT static Handle(StepVisual_Colour) EncodeColor (const Quantity_Color& Col, STEPConstruct_DataMapOfAsciiStringTransient& DPDCs, STEPConstruct_DataMapOfPointTransient& ColRGBs);
  
  //! Decodes STEP color and fills the Quantity_Color.
  //! Returns True if OK or False if color is not recognized
  Standard_EXPORT static Standard_Boolean DecodeColor (const Handle(StepVisual_Colour)& Colour, Quantity_Color& Col);




protected:





private:



  TColStd_IndexedDataMapOfTransientTransient myMapOfStyles;
  TColStd_IndexedMapOfTransient myStyles;
  TColStd_IndexedMapOfTransient myRootStyles;
  TColStd_SequenceOfTransient myPSA;


};







#endif // _STEPConstruct_Styles_HeaderFile
