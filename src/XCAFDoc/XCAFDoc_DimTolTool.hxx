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

#include <TDataStd_GenericEmpty.hxx>
#include <Standard_Boolean.hxx>
#include <TDF_LabelSequence.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_DataMap.hxx>

class XCAFDoc_ShapeTool;
class TDF_Label;
class Standard_GUID;
class TCollection_HAsciiString;
class TDF_Attribute;
class TDF_RelocationTable;


class XCAFDoc_DimTolTool;
DEFINE_STANDARD_HANDLE(XCAFDoc_DimTolTool, TDataStd_GenericEmpty)

//! Attribute containing GD&T section of XCAF document.
//! Provide tools for GD&T section management.
class XCAFDoc_DimTolTool : public TDataStd_GenericEmpty
{

public:

  
  Standard_EXPORT XCAFDoc_DimTolTool();
  
  //! Creates (if not exist) DimTolTool attribute.
  Standard_EXPORT static Handle(XCAFDoc_DimTolTool) Set (const TDF_Label& L);
  
  //! Returns the standard GD&T tool GUID.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Returns the label under which GD&T table is stored.
  Standard_EXPORT TDF_Label BaseLabel() const;
  
  //! Returns internal XCAFDoc_ShapeTool tool
  Standard_EXPORT const Handle(XCAFDoc_ShapeTool)& ShapeTool();

  //! Returns True if the label belongs to a GD&T table and
  //! is a Dimension definition.
  Standard_EXPORT Standard_Boolean IsDimension (const TDF_Label& theLab) const;
  
  //! Returns a sequence of Dimension labels currently stored
  //! in the GD&T table.
  Standard_EXPORT void GetDimensionLabels (TDF_LabelSequence& theLabels) const;
  
  //! Sets a dimension to sequences target labels.
  Standard_EXPORT void SetDimension (const TDF_LabelSequence& theFirstLS, 
                                     const TDF_LabelSequence& theSecondLS, 
                                     const TDF_Label& theDimL) const;

  //! Sets a dimension to target labels.
  Standard_EXPORT void SetDimension(const TDF_Label& theFirstL,
                                     const TDF_Label& theSecondL, 
                                     const TDF_Label& theDimL) const;
  
  //! Sets a dimension to the target label.
  Standard_EXPORT void SetDimension (const TDF_Label& theL, 
                                     const TDF_Label& theDimL) const;
  
  //! Returns all Dimension labels defined for theShapeL.
  Standard_EXPORT Standard_Boolean GetRefDimensionLabels (const TDF_Label& theShapeL, 
                                                          TDF_LabelSequence& theDimensions) const;
  
  //! Adds a dimension definition to the GD&T table and returns its label.
  Standard_EXPORT TDF_Label AddDimension() ;
  
  //! Returns True if the label belongs to the GD&T table and is a dimension tolerance.
  Standard_EXPORT Standard_Boolean IsGeomTolerance (const TDF_Label& theLab) const;
  
  //! Returns a sequence of Tolerance labels currently stored in the GD&T table.
  Standard_EXPORT void GetGeomToleranceLabels (TDF_LabelSequence& theLabels) const;
  
  //! Sets a geometry tolerance from theGeomTolL to theL label.
  //! Checks if theGeomTolL is a geometry tolerance definition first.
  Standard_EXPORT void SetGeomTolerance (const TDF_Label& theL, 
                                         const TDF_Label& theGeomTolL) const;

  //! Sets a geometry tolerance from theGeomTolL to sequence of labels theL.
  //! Checks if theGeomTolL is a geometry tolerance definition first.
  Standard_EXPORT void SetGeomTolerance(const TDF_LabelSequence& theL,
                                         const TDF_Label& theGeomTolL) const;
  
  //! Returns all GeomTolerance labels defined for theShapeL.
  Standard_EXPORT Standard_Boolean GetRefGeomToleranceLabels (const TDF_Label& theShapeL, 
                                                              TDF_LabelSequence& theDimTols) const;
  
  //! Adds a GeomTolerance definition to the GD&T table and returns its label.
  Standard_EXPORT TDF_Label AddGeomTolerance();
  
  //! Returns True if theLab belongs to the GD&T table and is a dmension tolerance.
  Standard_EXPORT Standard_Boolean IsDimTol(const TDF_Label& theLab) const;
  
  //! Returns a sequence of D&GTs currently stored in the GD&T table.
  Standard_EXPORT void GetDimTolLabels (TDF_LabelSequence& Labels) const;
  
  //! Finds a dimension tolerance definition in the GD&T table 
  //! satisfying the specified kind, values, name and description
  //! and returns its label if found.
  //! Returns False if dimension tolerance is not found in DGTtable.
  Standard_EXPORT Standard_Boolean FindDimTol (const Standard_Integer theKind, 
                                               const Handle(TColStd_HArray1OfReal)& theVal, 
                                               const Handle(TCollection_HAsciiString)& theName, 
                                               const Handle(TCollection_HAsciiString)& theDescription, 
                                               TDF_Label& lab) const;
  
  //! Finds a dimension tolerance in the GD&T table 
  //! satisfying the specified kind, values, name and description
  //! and returns its label if found (or Null label else).
  Standard_EXPORT TDF_Label FindDimTol (const Standard_Integer theKind, 
                                        const Handle(TColStd_HArray1OfReal)& theVal, 
                                        const Handle(TCollection_HAsciiString)& theName, 
                                        const Handle(TCollection_HAsciiString)& theDescription) const;
  
  //! Adds a dimension tolerance definition with the specified
  //! kind, value, name and description to the GD&T table and returns its label.
  Standard_EXPORT TDF_Label AddDimTol (const Standard_Integer theKind, 
                                       const Handle(TColStd_HArray1OfReal)& theVal, 
                                       const Handle(TCollection_HAsciiString)& theName, 
                                       const Handle(TCollection_HAsciiString)& theDescription) const;
  
  //! Sets existing dimension tolerance to theL label.
  Standard_EXPORT void SetDimTol (const TDF_Label& theL, 
                                  const TDF_Label& theDimTolL) const;
  
  //! Creates a dimension tolerance and sets it to theL label.
  Standard_EXPORT TDF_Label SetDimTol (const TDF_Label& theL, 
                                       const Standard_Integer theKind, 
                                       const Handle(TColStd_HArray1OfReal)& theVal, 
                                       const Handle(TCollection_HAsciiString)& theName, 
                                       const Handle(TCollection_HAsciiString)& theDescription) const;
  
  //! Gets all shape labels referred by theL label of the GD&T table.
  //! Returns False if there are no shape labels added to the sequences.
  Standard_EXPORT Standard_Boolean GetRefShapeLabel (const TDF_Label& theL, 
                                                     TDF_LabelSequence& theShapeLFirst, 
                                                     TDF_LabelSequence& theShapeLSecond) const;
  
  //! Returns dimension tolerance assigned to theDimTolL label.
  //! Returns False if no such dimension tolerance is assigned.
  Standard_EXPORT Standard_Boolean GetDimTol (const TDF_Label& theDimTolL, 
                                              Standard_Integer& theKind, 
                                              Handle(TColStd_HArray1OfReal)& theVal, 
                                              Handle(TCollection_HAsciiString)& theName, 
                                              Handle(TCollection_HAsciiString)& theDescription) const;
  
  //! Returns True if label belongs to the GD&T table and
  //! is a Datum definition.
  Standard_EXPORT Standard_Boolean IsDatum (const TDF_Label& lab) const;
  
  //! Returns a sequence of Datums currently stored
  //! in the GD&T table.
  Standard_EXPORT void GetDatumLabels (TDF_LabelSequence& Labels) const;
  
  //! Finds a datum satisfying the specified name, description and
  //! identification and returns its label if found.
  Standard_EXPORT Standard_Boolean FindDatum (const Handle(TCollection_HAsciiString)& theName, 
                                              const Handle(TCollection_HAsciiString)& theDescription, 
                                              const Handle(TCollection_HAsciiString)& theIdentification, 
                                              TDF_Label& lab) const;
  
  //! Adds a datum definition to the GD&T table and returns its label.
  Standard_EXPORT TDF_Label AddDatum (const Handle(TCollection_HAsciiString)& theName, 
                                      const Handle(TCollection_HAsciiString)& theDescription, 
                                      const Handle(TCollection_HAsciiString)& theIdentification) const;

  //! Adds a datum definition to the GD&T table and returns its label.
  Standard_EXPORT TDF_Label AddDatum() ;
  
  //! Sets a datum to the sequence of shape labels.
  Standard_EXPORT void SetDatum (const TDF_LabelSequence& theShapeLabels, 
                                 const TDF_Label& theDatumL) const;
  
  //! Sets a datum to theL label and binds it with theTolerL label.
  //! A datum with the specified name, description and identification
  //! is created if it isn't found in the GD&T table.
  Standard_EXPORT void SetDatum (const TDF_Label& theL, 
                                 const TDF_Label& theTolerL, 
                                 const Handle(TCollection_HAsciiString)& theName, 
                                 const Handle(TCollection_HAsciiString)& theDescription, 
                                 const Handle(TCollection_HAsciiString)& theIdentification) const;

  //! Sets a datum from theDatumL label to theToletL label.
  Standard_EXPORT void SetDatumToGeomTol (const TDF_Label& theDatumL, 
                                          const TDF_Label& theTolerL)  const;
  
  //! Returns datum assigned to theDatumL label.
  //! Returns False if no such datum is assigned.
  Standard_EXPORT Standard_Boolean GetDatum (const TDF_Label& theDatumL, 
                                             Handle(TCollection_HAsciiString)& theName, 
                                             Handle(TCollection_HAsciiString)& theDescription, 
                                             Handle(TCollection_HAsciiString)& theIdentification) const;
  
  //! Returns all Datum labels defined for theDimTolL label.
  Standard_EXPORT Standard_Boolean GetDatumOfTolerLabels (const TDF_Label& theDimTolL, 
                                                          TDF_LabelSequence& theDatums) const;

  //! Returns all Datum labels with XCAFDimTolObjects_DatumObject defined for label theDimTolL.
  Standard_EXPORT Standard_Boolean GetDatumWithObjectOfTolerLabels (const TDF_Label& theDimTolL, 
                                                                    TDF_LabelSequence& theDatums) const;

  //! Returns all GeomToleranses labels defined for theDatumL label.
  Standard_EXPORT Standard_Boolean GetTolerOfDatumLabels (const TDF_Label& theDatumL, 
                                                          TDF_LabelSequence& theTols)  const;

  //! Returns Datum label defined for theShapeL label.
  Standard_EXPORT Standard_Boolean GetRefDatumLabel (const TDF_Label& theShapeL, 
                                                     TDF_LabelSequence& theDatum)  const;

  //! Returns true if the given GDT is marked as locked.
  Standard_EXPORT Standard_Boolean IsLocked(const TDF_Label& theViewL) const;

  //! Mark the given GDT as locked.
  Standard_EXPORT void Lock(const TDF_Label& theViewL) const;

  //! fill the map GDT label -> shape presentation 
  Standard_EXPORT void GetGDTPresentations(NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape, TDF_LabelMapHasher>& theGDTLabelToShape) const;

  //! Set shape presentation for GDT labels according to given map (theGDTLabelToPrs)
  //! theGDTLabelToPrsName map is an additional argument, can be used to set presentation names.
  //! If label is not in the theGDTLabelToPrsName map, the presentation name will be empty
  Standard_EXPORT void SetGDTPresentations(NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape, TDF_LabelMapHasher>& theGDTLabelToPrs);

  //! Unlock the given GDT.
  Standard_EXPORT void Unlock(const TDF_Label& theViewL) const;
    
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;


  DEFINE_DERIVED_ATTRIBUTE(XCAFDoc_DimTolTool,TDataStd_GenericEmpty)


private:


  Handle(XCAFDoc_ShapeTool) myShapeTool;


};







#endif // _XCAFDoc_DimTolTool_HeaderFile
