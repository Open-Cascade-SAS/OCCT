// Created on: 2004-01-09
// Created by: Sergey KUUL
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_DimTolTool_HeaderFile
#define _XCAFDoc_DimTolTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <TDF_LabelSequence.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopoDS_Shape.hxx>

class XCAFDoc_ShapeTool;
class TDF_Label;
class Standard_GUID;
class TCollection_HAsciiString;
class TDF_Attribute;
class TDF_RelocationTable;


class XCAFDoc_DimTolTool;
DEFINE_STANDARD_HANDLE(XCAFDoc_DimTolTool, TDF_Attribute)

//! Provides tools to store and retrieve attributes (colors)
//! of TopoDS_Shape in and from TDocStd_Document
//! A Document is intended to hold different
//! attributes of ONE shape and it's sub-shapes.
//! Attribute containing DimTol section of DECAF document.
//! Provide tools for management of DimTol section of document.
class XCAFDoc_DimTolTool : public TDF_Attribute
{

public:

  
  Standard_EXPORT XCAFDoc_DimTolTool();
  
  //! Creates (if not exist) DimTolTool.
  Standard_EXPORT static Handle(XCAFDoc_DimTolTool) Set (const TDF_Label& L);
  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! returns the label under which colors are stored
  Standard_EXPORT TDF_Label BaseLabel() const;
  
  //! Returns internal XCAFDoc_ShapeTool tool
  Standard_EXPORT const Handle(XCAFDoc_ShapeTool)& ShapeTool();

  //! Returns True if label belongs to a dimtoltable and
  //! is a Dimension definition
  Standard_EXPORT Standard_Boolean IsDimension (const TDF_Label& theLab) const;
  
  //! Returns a sequence of Dimensions labels currently stored
  //! in the DGTtable
  Standard_EXPORT void GetDimensionLabels (TDF_LabelSequence& theLabels) const;
  
  //! Sets a link with GUID
  Standard_EXPORT void SetDimension (const TDF_LabelSequence& theFirstLS, const TDF_LabelSequence& theSecondLS, const TDF_Label& theDimTolL) const;

  //! Sets a link with GUID
  Standard_EXPORT void SetDimension (const TDF_Label& theFirstL, const TDF_Label& theSecondL, const TDF_Label& theDimTolL) const;
  
  //! Sets a link with GUID
  Standard_EXPORT void SetDimension (const TDF_Label& theL, const TDF_Label& theDimTolL) const;
  
  //! Returns all Dimension labels defined for label ShapeL
  Standard_EXPORT Standard_Boolean GetRefDimensionLabels (const TDF_Label& theShapeL, TDF_LabelSequence& theDimensions) const;
  
  //! Adds a dimension definition to a DGTtable and returns its label
  Standard_EXPORT TDF_Label AddDimension() ;
  
  //! Returns True if label belongs to a dimtoltable and
  //! is a DimTol definition
  Standard_EXPORT Standard_Boolean IsGeomTolerance (const TDF_Label& theLab) const;
  
  //! Returns a sequence of Tolerance labels currently stored
  //! in the DGTtable
  Standard_EXPORT void GetGeomToleranceLabels (TDF_LabelSequence& theLabels) const;
  
  //! Sets a link with GUID
  Standard_EXPORT void SetGeomTolerance (const TDF_Label& theL, const TDF_Label& theDimTolL) const;

  //! Sets a link with GUID
  Standard_EXPORT void SetGeomTolerance (const TDF_LabelSequence& theL, const TDF_Label& theDimTolL) const;
  
  //! Returns all GeomTolerance labels defined for label ShapeL
  Standard_EXPORT Standard_Boolean GetRefGeomToleranceLabels (const TDF_Label& theShapeL, TDF_LabelSequence& theDimTols) const;
  
  //! Adds a GeomTolerance definition to a DGTtable and returns its label
  Standard_EXPORT TDF_Label AddGeomTolerance();
  
  //! Returns True if label belongs to a dimtoltable and
  //! is a DimTol definition
  Standard_EXPORT Standard_Boolean IsDimTol (const TDF_Label& lab) const;
  
  //! Returns a sequence of D&GTs currently stored
  //! in the DGTtable
  Standard_EXPORT void GetDimTolLabels (TDF_LabelSequence& Labels) const;
  
  //! Finds a dimtol definition in a DGTtable and returns
  //! its label if found
  //! Returns False if dimtol is not found in DGTtable
  Standard_EXPORT Standard_Boolean FindDimTol (const Standard_Integer kind, const Handle(TColStd_HArray1OfReal)& aVal, const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, TDF_Label& lab) const;
  
  //! Finds a dimtol definition in a DGTtable and returns
  //! its label if found (or Null label else)
  Standard_EXPORT TDF_Label FindDimTol (const Standard_Integer kind, const Handle(TColStd_HArray1OfReal)& aVal, const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription) const;
  
  //! Adds a dimtol definition to a DGTtable and returns its label
  Standard_EXPORT TDF_Label AddDimTol (const Standard_Integer kind, const Handle(TColStd_HArray1OfReal)& aVal, const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription) const;
  
  //! Sets a link with GUID
  Standard_EXPORT void SetDimTol (const TDF_Label& L, const TDF_Label& DimTolL) const;
  
  //! Sets a link with GUID
  //! Adds a DimTol as necessary
  Standard_EXPORT TDF_Label SetDimTol (const TDF_Label& L, const Standard_Integer kind, const Handle(TColStd_HArray1OfReal)& aVal, const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription) const;
  
  //! Returns ShapeL defined for label DimTolL
  //! Returns False if the DimTolL is not in DGTtable
  Standard_EXPORT Standard_Boolean GetRefShapeLabel (const TDF_Label& DimTolL, TDF_LabelSequence& ShapeLFirst, TDF_LabelSequence& ShapeLSecond) const;
  
  //! Returns all DimTol labels defined for label ShapeL
  Standard_EXPORT Standard_Boolean GetRefDGTLabels (const TDF_Label& ShapeL, TDF_LabelSequence& DimTols) const;
  
  //! Returns dimtol assigned to <DimTolL>
  //! Returns False if no such dimtol is assigned
  Standard_EXPORT Standard_Boolean GetDimTol (const TDF_Label& DimTolL, Standard_Integer& kind, Handle(TColStd_HArray1OfReal)& aVal, Handle(TCollection_HAsciiString)& aName, Handle(TCollection_HAsciiString)& aDescription) const;
  
  //! Returns True if label belongs to a dimtoltable and
  //! is a Datum definition
  Standard_EXPORT Standard_Boolean IsDatum (const TDF_Label& lab) const;
  
  //! Returns a sequence of Datumss currently stored
  //! in the DGTtable
  Standard_EXPORT void GetDatumLabels (TDF_LabelSequence& Labels) const;
  
  //! Finds a datum and returns its label if found
  Standard_EXPORT Standard_Boolean FindDatum (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Handle(TCollection_HAsciiString)& anIdentification, TDF_Label& lab) const;
  
  //! Adds a datum definition to a DGTtable and returns its label
  Standard_EXPORT TDF_Label AddDatum (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Handle(TCollection_HAsciiString)& anIdentification) const;

  //! Adds a datum definition to a DGTtable and returns its label
  Standard_EXPORT TDF_Label AddDatum() ;
  
  //! Sets a link with GUID
  Standard_EXPORT void SetDatum (const TDF_LabelSequence& theShapeLabels, const TDF_Label& theDatumL) const;

  //! Sets a link with GUID for Datum
  //! Sets connection between Datum and Tolerance
  Standard_EXPORT void SetDatumToGeomTol (const TDF_Label& theL, const TDF_Label& theTolerL)  const;
  
  //! Sets a link with GUID for Datum
  //! Adds a Datum as necessary
  //! Sets connection between Datum and Tolerance
  Standard_EXPORT void SetDatum (const TDF_Label& L, const TDF_Label& TolerL, const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Handle(TCollection_HAsciiString)& anIdentification) const;
  
  //! Returns datum assigned to <DatumL>
  //! Returns False if no such datum is assigned
  Standard_EXPORT Standard_Boolean GetDatum (const TDF_Label& DatumL, Handle(TCollection_HAsciiString)& aName, Handle(TCollection_HAsciiString)& aDescription, Handle(TCollection_HAsciiString)& anIdentification) const;
  
  //! Returns all Datum labels defined for label DimTolL
  Standard_EXPORT Standard_Boolean GetDatumOfTolerLabels (const TDF_Label& DimTolL, TDF_LabelSequence& Datums) const;

  //! Returns all Datum labels with XCAFDimTolObjects_DatumObject defined for label DimTolL
  Standard_EXPORT Standard_Boolean GetDatumWithObjectOfTolerLabels (const TDF_Label& DimTolL, TDF_LabelSequence& Datums) const;

  //! Returns all GeomToleranses labels defined for label DatumL
  Standard_EXPORT   Standard_Boolean GetTolerOfDatumLabels (const TDF_Label& theDatumL, TDF_LabelSequence& theTols)  const;

  //! Returns Datum label defined for label ShapeL
  Standard_EXPORT   Standard_Boolean GetRefDatumLabel (const TDF_Label& theShapeL, TDF_LabelSequence& theDatum)  const;

  //! Returns true if the given GDT is marked as locked
  Standard_EXPORT Standard_Boolean IsLocked(const TDF_Label& theViewL) const;

  //! Mark the given GDT as locked
  Standard_EXPORT void Lock(const TDF_Label& theViewL) const;

  //! Unlock the given GDT
  Standard_EXPORT void Unlock(const TDF_Label& theViewL) const;
    
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XCAFDoc_DimTolTool,TDF_Attribute)

protected:




private:


  Handle(XCAFDoc_ShapeTool) myShapeTool;


};







#endif // _XCAFDoc_DimTolTool_HeaderFile
