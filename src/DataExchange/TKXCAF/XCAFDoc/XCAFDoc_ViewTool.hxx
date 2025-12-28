// Created on: 2016-10-19
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_ViewTool_HeaderFile
#define _XCAFDoc_ViewTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDataStd_GenericEmpty.hxx>
#include <Standard_Boolean.hxx>
#include <TDF_Label.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>

class TDF_Label;
class Standard_GUID;

//! Provides tools to store and retrieve Views
//! in and from TDocStd_Document
//! Each View contains parts XCAFDoc_View attribute
//! with all information about camera and view window.
//! Also each view contain information of displayed shapes and GDTs
//! as sets of shape and GDT labels.
class XCAFDoc_ViewTool : public TDataStd_GenericEmpty
{

public:
  Standard_EXPORT XCAFDoc_ViewTool();

  //! Creates (if not exist) ViewTool.
  Standard_EXPORT static occ::handle<XCAFDoc_ViewTool> Set(const TDF_Label& L);

  Standard_EXPORT static const Standard_GUID& GetID();

  //! Returns the label under which Views are stored
  Standard_EXPORT TDF_Label BaseLabel() const;

  //! Returns True if label belongs to a View table and
  //! is a View definition
  Standard_EXPORT bool IsView(const TDF_Label& theLabel) const;

  //! Returns a sequence of View labels currently stored
  //! in the View table
  Standard_EXPORT void GetViewLabels(NCollection_Sequence<TDF_Label>& theLabels) const;

  //! Sets a link with GUID
  Standard_EXPORT void SetView(const NCollection_Sequence<TDF_Label>& theShapes,
                               const NCollection_Sequence<TDF_Label>& theGDTs,
                               const NCollection_Sequence<TDF_Label>& theClippingPlanes,
                               const NCollection_Sequence<TDF_Label>& theNotes,
                               const NCollection_Sequence<TDF_Label>& theAnnotations,
                               const TDF_Label&                       theViewL) const;

  //! Sets a link with GUID
  Standard_EXPORT void SetView(const NCollection_Sequence<TDF_Label>& theShapes,
                               const NCollection_Sequence<TDF_Label>& theGDTs,
                               const NCollection_Sequence<TDF_Label>& theClippingPlanes,
                               const TDF_Label&                       theViewL) const;

  //! Sets a link with GUID
  Standard_EXPORT void SetView(const NCollection_Sequence<TDF_Label>& theShapes,
                               const NCollection_Sequence<TDF_Label>& theGDTs,
                               const TDF_Label&                       theViewL) const;

  //! Set Clipping planes to given View
  Standard_EXPORT void SetClippingPlanes(
    const NCollection_Sequence<TDF_Label>& theClippingPlaneLabels,
    const TDF_Label&                       theViewL) const;

  //! Remove View
  Standard_EXPORT void RemoveView(const TDF_Label& theViewL);

  //! Returns all View labels defined for label ShapeL
  Standard_EXPORT bool GetViewLabelsForShape(const TDF_Label&                 theShapeL,
                                             NCollection_Sequence<TDF_Label>& theViews) const;

  //! Returns all View labels defined for label GDTL
  Standard_EXPORT bool GetViewLabelsForGDT(const TDF_Label&                 theGDTL,
                                           NCollection_Sequence<TDF_Label>& theViews) const;

  //! Returns all View labels defined for label ClippingPlaneL
  Standard_EXPORT bool GetViewLabelsForClippingPlane(
    const TDF_Label&                 theClippingPlaneL,
    NCollection_Sequence<TDF_Label>& theViews) const;

  //! Returns all View labels defined for label NoteL
  Standard_EXPORT bool GetViewLabelsForNote(const TDF_Label&                 theNoteL,
                                            NCollection_Sequence<TDF_Label>& theViews) const;

  //! Returns all View labels defined for label AnnotationL
  Standard_EXPORT bool GetViewLabelsForAnnotation(const TDF_Label&                 theAnnotationL,
                                                  NCollection_Sequence<TDF_Label>& theViews) const;

  //! Adds a view definition to a View table and returns its label
  Standard_EXPORT TDF_Label AddView();

  //! Returns shape labels defined for label theViewL
  //! Returns False if the theViewL is not in View table
  Standard_EXPORT bool GetRefShapeLabel(const TDF_Label&                 theViewL,
                                        NCollection_Sequence<TDF_Label>& theShapeLabels) const;

  //! Returns GDT labels defined for label theViewL
  //! Returns False if the theViewL is not in View table
  Standard_EXPORT bool GetRefGDTLabel(const TDF_Label&                 theViewL,
                                      NCollection_Sequence<TDF_Label>& theGDTLabels) const;

  //! Returns ClippingPlane labels defined for label theViewL
  //! Returns False if the theViewL is not in View table
  Standard_EXPORT bool GetRefClippingPlaneLabel(
    const TDF_Label&                 theViewL,
    NCollection_Sequence<TDF_Label>& theClippingPlaneLabels) const;

  //! Returns Notes labels defined for label theViewL
  //! Returns False if the theViewL is not in View table
  Standard_EXPORT bool GetRefNoteLabel(const TDF_Label&                 theViewL,
                                       NCollection_Sequence<TDF_Label>& theNoteLabels) const;

  //! Returns Annotation labels defined for label theViewL
  //! Returns False if the theViewL is not in View table
  Standard_EXPORT bool GetRefAnnotationLabel(
    const TDF_Label&                 theViewL,
    NCollection_Sequence<TDF_Label>& theAnnotationLabels) const;

  //! Returns true if the given View is marked as locked
  Standard_EXPORT bool IsLocked(const TDF_Label& theViewL) const;

  //! Mark the given View as locked
  Standard_EXPORT void Lock(const TDF_Label& theViewL) const;

  //! Unlock the given View
  Standard_EXPORT void Unlock(const TDF_Label& theViewL) const;

  Standard_EXPORT const Standard_GUID& ID() const override;

  DEFINE_DERIVED_ATTRIBUTE(XCAFDoc_ViewTool, TDataStd_GenericEmpty)
};
#endif // _XCAFDoc_ViewTool_HeaderFile
