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
#include <TDF_Label.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_IndexedDataMap.hxx>

class XCAFDoc_ShapeTool;
class TDF_Label;
class Standard_GUID;
class TCollection_HAsciiString;

//! Attribute containing GD&T section of XCAF document.
//! Provide tools for GD&T section management.
class XCAFDoc_DimTolTool : public TDataStd_GenericEmpty
{

public:
  Standard_EXPORT XCAFDoc_DimTolTool();

  //! Creates (if not exist) DimTolTool attribute.
  Standard_EXPORT static occ::handle<XCAFDoc_DimTolTool> Set(const TDF_Label& L);

  //! Returns the standard GD&T tool GUID.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Returns the label under which GD&T table is stored.
  Standard_EXPORT TDF_Label BaseLabel() const;

  //! Returns internal XCAFDoc_ShapeTool tool
  Standard_EXPORT const occ::handle<XCAFDoc_ShapeTool>& ShapeTool();

  //! Returns True if the label belongs to a GD&T table and
  //! is a Dimension definition.
  Standard_EXPORT bool IsDimension(const TDF_Label& theLab) const;

  //! Returns a sequence of Dimension labels currently stored
  //! in the GD&T table.
  Standard_EXPORT void GetDimensionLabels(NCollection_Sequence<TDF_Label>& theLabels) const;

  //! Sets a dimension to sequences target labels.
  Standard_EXPORT void SetDimension(const NCollection_Sequence<TDF_Label>& theFirstLS,
                                    const NCollection_Sequence<TDF_Label>& theSecondLS,
                                    const TDF_Label&                       theDimL) const;

  //! Sets a dimension to target labels.
  Standard_EXPORT void SetDimension(const TDF_Label& theFirstL,
                                    const TDF_Label& theSecondL,
                                    const TDF_Label& theDimL) const;

  //! Sets a dimension to the target label.
  Standard_EXPORT void SetDimension(const TDF_Label& theL, const TDF_Label& theDimL) const;

  //! Returns all Dimension labels defined for theShapeL.
  Standard_EXPORT bool GetRefDimensionLabels(const TDF_Label&                 theShapeL,
                                             NCollection_Sequence<TDF_Label>& theDimensions) const;

  //! Adds a dimension definition to the GD&T table and returns its label.
  Standard_EXPORT TDF_Label AddDimension();

  //! Returns True if the label belongs to the GD&T table and is a dimension tolerance.
  Standard_EXPORT bool IsGeomTolerance(const TDF_Label& theLab) const;

  //! Returns a sequence of Tolerance labels currently stored in the GD&T table.
  Standard_EXPORT void GetGeomToleranceLabels(NCollection_Sequence<TDF_Label>& theLabels) const;

  //! Sets a geometry tolerance from theGeomTolL to theL label.
  //! Checks if theGeomTolL is a geometry tolerance definition first.
  Standard_EXPORT void SetGeomTolerance(const TDF_Label& theL, const TDF_Label& theGeomTolL) const;

  //! Sets a geometry tolerance from theGeomTolL to sequence of labels theL.
  //! Checks if theGeomTolL is a geometry tolerance definition first.
  Standard_EXPORT void SetGeomTolerance(const NCollection_Sequence<TDF_Label>& theL,
                                        const TDF_Label&                       theGeomTolL) const;

  //! Returns all GeomTolerance labels defined for theShapeL.
  Standard_EXPORT bool GetRefGeomToleranceLabels(const TDF_Label&                 theShapeL,
                                                 NCollection_Sequence<TDF_Label>& theDimTols) const;

  //! Adds a GeomTolerance definition to the GD&T table and returns its label.
  Standard_EXPORT TDF_Label AddGeomTolerance();

  //! Returns True if theLab belongs to the GD&T table and is a dmension tolerance.
  Standard_EXPORT bool IsDimTol(const TDF_Label& theLab) const;

  //! Returns a sequence of D&GTs currently stored in the GD&T table.
  Standard_EXPORT void GetDimTolLabels(NCollection_Sequence<TDF_Label>& Labels) const;

  //! Finds a dimension tolerance definition in the GD&T table
  //! satisfying the specified kind, values, name and description
  //! and returns its label if found.
  //! Returns False if dimension tolerance is not found in DGTtable.
  Standard_EXPORT bool FindDimTol(const int                                       theKind,
                                  const occ::handle<NCollection_HArray1<double>>& theVal,
                                  const occ::handle<TCollection_HAsciiString>&    theName,
                                  const occ::handle<TCollection_HAsciiString>&    theDescription,
                                  TDF_Label&                                      lab) const;

  //! Finds a dimension tolerance in the GD&T table
  //! satisfying the specified kind, values, name and description
  //! and returns its label if found (or Null label else).
  Standard_EXPORT TDF_Label
    FindDimTol(const int                                       theKind,
               const occ::handle<NCollection_HArray1<double>>& theVal,
               const occ::handle<TCollection_HAsciiString>&    theName,
               const occ::handle<TCollection_HAsciiString>&    theDescription) const;

  //! Adds a dimension tolerance definition with the specified
  //! kind, value, name and description to the GD&T table and returns its label.
  Standard_EXPORT TDF_Label
    AddDimTol(const int                                       theKind,
              const occ::handle<NCollection_HArray1<double>>& theVal,
              const occ::handle<TCollection_HAsciiString>&    theName,
              const occ::handle<TCollection_HAsciiString>&    theDescription) const;

  //! Sets existing dimension tolerance to theL label.
  Standard_EXPORT void SetDimTol(const TDF_Label& theL, const TDF_Label& theDimTolL) const;

  //! Creates a dimension tolerance and sets it to theL label.
  Standard_EXPORT TDF_Label
    SetDimTol(const TDF_Label&                                theL,
              const int                                       theKind,
              const occ::handle<NCollection_HArray1<double>>& theVal,
              const occ::handle<TCollection_HAsciiString>&    theName,
              const occ::handle<TCollection_HAsciiString>&    theDescription) const;

  //! Gets all shape labels referred by theL label of the GD&T table.
  //! Returns False if there are no shape labels added to the sequences.
  Standard_EXPORT static bool GetRefShapeLabel(const TDF_Label&                 theL,
                                               NCollection_Sequence<TDF_Label>& theShapeLFirst,
                                               NCollection_Sequence<TDF_Label>& theShapeLSecond);

  //! Returns dimension tolerance assigned to theDimTolL label.
  //! Returns False if no such dimension tolerance is assigned.
  Standard_EXPORT bool GetDimTol(const TDF_Label&                          theDimTolL,
                                 int&                                      theKind,
                                 occ::handle<NCollection_HArray1<double>>& theVal,
                                 occ::handle<TCollection_HAsciiString>&    theName,
                                 occ::handle<TCollection_HAsciiString>&    theDescription) const;

  //! Returns True if label belongs to the GD&T table and
  //! is a Datum definition.
  Standard_EXPORT bool IsDatum(const TDF_Label& lab) const;

  //! Returns a sequence of Datums currently stored
  //! in the GD&T table.
  Standard_EXPORT void GetDatumLabels(NCollection_Sequence<TDF_Label>& Labels) const;

  //! Finds a datum satisfying the specified name, description and
  //! identification and returns its label if found.
  Standard_EXPORT bool FindDatum(const occ::handle<TCollection_HAsciiString>& theName,
                                 const occ::handle<TCollection_HAsciiString>& theDescription,
                                 const occ::handle<TCollection_HAsciiString>& theIdentification,
                                 TDF_Label&                                   lab) const;

  //! Adds a datum definition to the GD&T table and returns its label.
  Standard_EXPORT TDF_Label
    AddDatum(const occ::handle<TCollection_HAsciiString>& theName,
             const occ::handle<TCollection_HAsciiString>& theDescription,
             const occ::handle<TCollection_HAsciiString>& theIdentification) const;

  //! Adds a datum definition to the GD&T table and returns its label.
  Standard_EXPORT TDF_Label AddDatum();

  //! Sets a datum to the sequence of shape labels.
  Standard_EXPORT void SetDatum(const NCollection_Sequence<TDF_Label>& theShapeLabels,
                                const TDF_Label&                       theDatumL) const;

  //! Sets a datum to theL label and binds it with theTolerL label.
  //! A datum with the specified name, description and identification
  //! is created if it isn't found in the GD&T table.
  Standard_EXPORT void SetDatum(
    const TDF_Label&                             theL,
    const TDF_Label&                             theTolerL,
    const occ::handle<TCollection_HAsciiString>& theName,
    const occ::handle<TCollection_HAsciiString>& theDescription,
    const occ::handle<TCollection_HAsciiString>& theIdentification) const;

  //! Sets a datum from theDatumL label to theToletL label.
  Standard_EXPORT void SetDatumToGeomTol(const TDF_Label& theDatumL,
                                         const TDF_Label& theTolerL) const;

  //! Returns datum assigned to theDatumL label.
  //! Returns False if no such datum is assigned.
  Standard_EXPORT bool GetDatum(const TDF_Label&                       theDatumL,
                                occ::handle<TCollection_HAsciiString>& theName,
                                occ::handle<TCollection_HAsciiString>& theDescription,
                                occ::handle<TCollection_HAsciiString>& theIdentification) const;

  //! Returns all Datum labels defined for theDimTolL label.
  Standard_EXPORT static bool GetDatumOfTolerLabels(const TDF_Label&                 theDimTolL,
                                                    NCollection_Sequence<TDF_Label>& theDatums);

  //! Returns all Datum labels with XCAFDimTolObjects_DatumObject defined for label theDimTolL.
  Standard_EXPORT static bool GetDatumWithObjectOfTolerLabels(
    const TDF_Label&                 theDimTolL,
    NCollection_Sequence<TDF_Label>& theDatums);

  //! Returns all GeomToleranses labels defined for theDatumL label.
  Standard_EXPORT bool GetTolerOfDatumLabels(const TDF_Label&                 theDatumL,
                                             NCollection_Sequence<TDF_Label>& theTols) const;

  //! Returns Datum label defined for theShapeL label.
  Standard_EXPORT bool GetRefDatumLabel(const TDF_Label&                 theShapeL,
                                        NCollection_Sequence<TDF_Label>& theDatum) const;

  //! Returns true if the given GDT is marked as locked.
  Standard_EXPORT bool IsLocked(const TDF_Label& theViewL) const;

  //! Mark the given GDT as locked.
  Standard_EXPORT void Lock(const TDF_Label& theViewL) const;

  //! fill the map GDT label -> shape presentation
  Standard_EXPORT void GetGDTPresentations(
    NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape>& theGDTLabelToShape) const;

  //! Set shape presentation for GDT labels according to given map (theGDTLabelToPrs)
  //! theGDTLabelToPrsName map is an additional argument, can be used to set presentation names.
  //! If label is not in the theGDTLabelToPrsName map, the presentation name will be empty
  Standard_EXPORT void SetGDTPresentations(
    NCollection_IndexedDataMap<TDF_Label, TopoDS_Shape>& theGDTLabelToPrs);

  //! Unlock the given GDT.
  Standard_EXPORT void Unlock(const TDF_Label& theViewL) const;

  Standard_EXPORT const Standard_GUID& ID() const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_DERIVED_ATTRIBUTE(XCAFDoc_DimTolTool, TDataStd_GenericEmpty)

private:
  occ::handle<XCAFDoc_ShapeTool> myShapeTool;
};

#endif // _XCAFDoc_DimTolTool_HeaderFile
