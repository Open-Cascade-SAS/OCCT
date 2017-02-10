// Created on: 2017-02-10
// Created by: Sergey NIKONOV
// Copyright (c) 2000-2017 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_NotesTool_HeaderFile
#define _XCAFDoc_NotesTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_LabelSequence.hxx>

class OSD_File;
class Standard_GUID;
class TCollection_AsciiString;
class TCollection_ExtendedString;
class TColStd_HArray1OfByte;
class TDF_RelocationTable;
class XCAFDoc_Note;
class XCAFDoc_AssemblyItemId;
class XCAFDoc_AssemblyItemRef;

class XCAFDoc_NotesTool;
DEFINE_STANDARD_HANDLE(XCAFDoc_NotesTool, TDF_Attribute)

//! A tool to annotate items in the hierarchical product structure.
//! There are two basic entities, which operates the notes tool: notes
//! and annotated items. A note is a user defined data structure derived
//! from \ref XCAFDoc_Note attribute that is attached to a separate label under
//! the notes hive. An annotated item is represented by \ref XCAFDoc_AssemblyItemRef
//! attribute attached to a separate label under the annotated items
//! hive. Notes are linked with annotated items by means of \ref XCAFDoc_GraphNode
//! attribute. Notes play parent roles and annotated items - child roles.
//!
//! ------------------------
//! | XCAFDoc_DocumentTool |
//! |          0:1         |
//! ------------------------
//!             |1
//! ------------------------
//! |  XCAFDoc_NotesTool   |
//! |         0:1:9        |
//! ------------------------
//!             |1
//!             |   -------------------     ---------------------------
//!             +___|      Notes      |-----|       XCAFDoc_Note      |
//!             |  1|     0:1:9:1     |1   *|         0:1:9:1:*       |
//!             |   -------------------     ---------------------------
//!             |                                        !*
//!             |                              { XCAFDoc_GraphNode }
//!             |                                       *!
//!             |   -------------------     ---------------------------
//!             +___| Annotated items |-----| XCAFDoc_AssemblyItemRef |
//!                1|     0:1:9:2     |1   *|         0:1:9:2:*       |
//!                 -------------------     ---------------------------
//!
//! A typical annotation procedure is illustrated by the code example below:
//! \code{.c++}
//! // Get the notes tool from a XCAF document
//! Handle(XCAFDoc_NotesTool) aNotesTool = XCAFDoc_DocumentTool::NotesTool(aDoc->Main());
//! // Create new comment note
//! Handle(XCAFDoc_Note) aNote = aNotesTool->CreateComment(aUserName, aTimestamp, aComment);
//! if (!aNote.IsNull()) {
//!   Handle(XCAFDoc_AssemblyItemRef) aRef = aNotesTool->AddNote(aNote->Label(), anAssemblyItemId);
//!   if (aRef.IsNull()) {
//!     // Process error...
//!   }
//! }
//! \endcode
class XCAFDoc_NotesTool : public TDF_Attribute
{
public:

  DEFINE_STANDARD_RTTIEXT(XCAFDoc_NotesTool, TDF_Attribute)

  Standard_EXPORT static const Standard_GUID& GetID();

  //! Create (if not exist) a notes tool from XCAFDoc on theLabel.
  Standard_EXPORT static Handle(XCAFDoc_NotesTool) Set(const TDF_Label& theLabel);

  //! Creates an empty notes tool.
  Standard_EXPORT XCAFDoc_NotesTool();

  //! Returns the label of the notes hive.
  Standard_EXPORT TDF_Label GetNotesLabel() const;

  //! Returns the label of the annotated items hive.
  Standard_EXPORT TDF_Label GetAnnotatedItemsLabel() const;

  //! Returns the number of labels in the notes hive.
  Standard_EXPORT Standard_Integer NbNotes() const;

  //! Returns the number of labels in the annotated items hive.
  Standard_EXPORT Standard_Integer NbAnnotatedItems() const;

  //! Returns all labels from the notes hive.
  //! The label sequence isn't cleared beforehand.
  //! \param [out] theNoteLabels - sequence of labels.
  Standard_EXPORT void GetNotes(TDF_LabelSequence& theNoteLabels) const;

  //! Returns all labels from the annotated items hive.
  //! The label sequence isn't cleared beforehand.
  //! \param [out] theNoteLabels - sequence of labels.
  Standard_EXPORT void GetAnnotatedItems(TDF_LabelSequence& theLabels) const;

  //! Checks if the given assembly item is annotated.
  //! \param [in] theItemId - assembly item ID.
  //! \return true if the item is annotated, otherwise - false.
  Standard_EXPORT Standard_Boolean IsAnnotatedItem(const XCAFDoc_AssemblyItemId& theItemId) const;

  //! Checks if the given labeled item is annotated.
  //! \param [in] theItemLabel - item label.
  //! \return true if the item is annotated, otherwise - false.
  Standard_EXPORT Standard_Boolean IsAnnotatedItem(const TDF_Label& theItemLabel) const;

  //! @name Find annotated item functions
  //! @{

  //! Finds a label of the given assembly item ID in the annotated items hive.
  //! \param [in] theItemId - assembly item ID.
  //! \return annotated item label if it is found, otherwise - null label.
  Standard_EXPORT TDF_Label FindAnnotatedItem(const XCAFDoc_AssemblyItemId& theItemId) const;

  //! Finds a label of the given labeled item in the annotated items hive.
  //! \param [in] theItemLabel - item label.
  //! \return annotated item label if it is found, otherwise - null label.
  Standard_EXPORT TDF_Label FindAnnotatedItem(const TDF_Label& theItemLabel) const;

  //! Finds a label of the given assembly item's attribute in the annotated items hive.
  //! \param [in] theItemId - assembly item ID.
  //! \param [in] theGUID   - assembly item's attribute GUID.
  //! \return annotated item label if it is found, otherwise - null label.
  Standard_EXPORT TDF_Label FindAnnotatedItemAttr(const XCAFDoc_AssemblyItemId& theItemId,
                                                  const Standard_GUID&          theGUID) const;

  //! Finds a label of the given labeled item's attribute in the annotated items hive.
  //! \param [in] theItemLabel - item label.
  //! \param [in] theGUID      - item's attribute GUID.
  //! \return annotated item label if it is found, otherwise - null label.
  Standard_EXPORT TDF_Label FindAnnotatedItemAttr(const TDF_Label&     theItemLabel,
                                                  const Standard_GUID& theGUID) const;

  //! Finds a label of the given assembly item's subshape in the annotated items hive.
  //! \param [in] theItemId        - assembly item ID.
  //! \param [in] theSubshapeIndex - assembly item's subshape index.
  //! \return annotated item label if it is found, otherwise - null label.
  Standard_EXPORT TDF_Label FindAnnotatedItemSubshape(const XCAFDoc_AssemblyItemId& theItemId,
                                                      Standard_Integer              theSubshapeIndex) const;

  //! Finds a label of the given labeled item's subshape in the annotated items hive.
  //! \param [in] theItemLabel     - item label.
  //! \param [in] theSubshapeIndex - labeled item's subshape index.
  //! \return annotated item label if it is found, otherwise - null label.
  Standard_EXPORT TDF_Label FindAnnotatedItemSubshape(const TDF_Label& theItemLabel,
                                                      Standard_Integer theSubshapeIndex) const;

  //! @}

  //! @name Note creation functions
  //! @{

  //! Create a new comment note.
  //! Creates a new label under the notes hive and attaches \ref XCAFDoc_NoteComment 
  //! attribute (derived ftom \ref XCAFDoc_Note).
  //! \param [in] theUserName  - the user associated with the note.
  //! \param [in] theTimeStamp - timestamp of the note.
  //! \param [in] theComment   - textual comment.
  //! \return a handle to the base note attribute.
  Standard_EXPORT Handle(XCAFDoc_Note) CreateComment(const TCollection_ExtendedString& theUserName,
                                                     const TCollection_ExtendedString& theTimeStamp,
                                                     const TCollection_ExtendedString& theComment);

  //! Create a new 'balloon' note.
  //! Creates a new label under the notes hive and attaches \ref XCAFDoc_NoteBalloon 
  //! attribute (derived ftom \ref XCAFDoc_Note).
  //! \param [in] theUserName  - the user associated with the note.
  //! \param [in] theTimeStamp - timestamp of the note.
  //! \param [in] theComment   - textual comment.
  //! \return a handle to the base note attribute.
  Standard_EXPORT Handle(XCAFDoc_Note) CreateBalloon(const TCollection_ExtendedString& theUserName,
                                                     const TCollection_ExtendedString& theTimeStamp,
                                                     const TCollection_ExtendedString& theComment);

  //! Create a new note with data loaded from a binary file.
  //! Creates a new label under the notes hive and attaches \ref XCAFDoc_NoteComment 
  //! attribute (derived ftom \ref XCAFDoc_Note).
  //! \param [in] theUserName  - the user associated with the note.
  //! \param [in] theTimeStamp - timestamp of the note.
  //! \param [in] theTitle     - file title.
  //! \param [in] theMIMEtype  - MIME type of the file.
  //! \param [in] theFile      - input binary file.
  //! \return a handle to the base note attribute.
  Standard_EXPORT Handle(XCAFDoc_Note) CreateBinData(const TCollection_ExtendedString& theUserName,
                                                     const TCollection_ExtendedString& theTimeStamp,
                                                     const TCollection_ExtendedString& theTitle,
                                                     const TCollection_AsciiString&    theMIMEtype,
                                                     OSD_File&                         theFile);

  //! Create a new note with data loaded from a byte data array.
  //! Creates a new label under the notes hive and attaches \ref XCAFDoc_NoteComment 
  //! attribute (derived ftom \ref XCAFDoc_Note).
  //! \param [in] theUserName  - the user associated with the note.
  //! \param [in] theTimeStamp - timestamp of the note.
  //! \param [in] theTitle     - data title.
  //! \param [in] theMIMEtype  - MIME type of the file.
  //! \param [in] theData      - byte data array.
  //! \return a handle to the base note attribute.
  Standard_EXPORT Handle(XCAFDoc_Note) CreateBinData(const TCollection_ExtendedString&    theUserName,
                                                     const TCollection_ExtendedString&    theTimeStamp,
                                                     const TCollection_ExtendedString&    theTitle,
                                                     const TCollection_AsciiString&       theMIMEtype,
                                                     const Handle(TColStd_HArray1OfByte)& theData);

  //! @}

  //! @name Get notes from annotated items functions
  //! @{

  //! Gets all note labels of the assembly item.
  //! Notes linked to item's subshapes or attributes aren't
  //! taken into account. The label sequence isn't cleared beforehand.
  //! \param [in] theItemId      - assembly item ID.
  //! \param [out] theNoteLabels - sequence of labels.
  //! \return number of added labels.
  Standard_EXPORT Standard_Integer GetNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                            TDF_LabelSequence&            theNoteLabels) const;

  //! Gets all note labels of the labeled item.
  //! Notes linked to item's attributes aren't
  //! taken into account. The label sequence isn't cleared beforehand.
  //! \param [in] theItemLabel   - item label.
  //! \param [out] theNoteLabels - sequence of labels.
  //! \return number of added labels.
  Standard_EXPORT Standard_Integer GetNotes(const TDF_Label&   theItemLabel,
                                            TDF_LabelSequence& theNoteLabels) const;

  //! Gets all note labels of the assembly item's attribute.
  //! Notes linked to the item itself or to item's subshapes
  //! aren't taken into account. The label sequence isn't cleared beforehand.
  //! \param [in] theItemId      - assembly item ID.
  //! \param [in] theGUID        - assembly item's attribute GUID.
  //! \param [out] theNoteLabels - sequence of labels.
  //! \return number of added labels.
  Standard_EXPORT Standard_Integer GetAttrNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                                const Standard_GUID&          theGUID,
                                                TDF_LabelSequence&            theNoteLabels) const;

  //! Gets all note labels of the labeled item's attribute.
  //! Notes linked to the item itself or to item's subshapes
  //! aren't taken into account. The label sequence isn't cleared beforehand.
  //! \param [in] theItemLabel   - item label.
  //! \param [in] theGUID        - item's attribute GUID.
  //! \param [out] theNoteLabels - sequence of labels.
  //! \return number of added labels.
  Standard_EXPORT Standard_Integer GetAttrNotes(const TDF_Label&     theItemLabel,
                                                const Standard_GUID& theGUID,
                                                TDF_LabelSequence&   theNoteLabels) const;

  //! Gets all note labels of the annotated item.
  //! Notes linked to the item itself or to item's attributes
  //! taken into account. The label sequence isn't cleared beforehand.
  //! \param [in] theItemId        - assembly item ID.
  //! \param [in] theSubshapeIndex - assembly item's subshape index.
  //! \param [out] theNoteLabels   - sequence of labels.
  //! \return number of added labels.
  Standard_EXPORT Standard_Integer GetSubshapeNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                                    Standard_Integer              theSubshapeIndex,
                                                    TDF_LabelSequence&            theNoteLabels) const;

  //! @}

  //! @name Annotation functions
  //! @{

  //! Adds the given note to the assembly item.
  //! \param [in] theNoteLabel - note label.
  //! \param [in] theItemId    - assembly item ID.
  //! \return a handle to the assembly reference attribute.
  Standard_EXPORT Handle(XCAFDoc_AssemblyItemRef) AddNote(const TDF_Label&              theNoteLabel,
                                                          const XCAFDoc_AssemblyItemId& theItemId);

  //! Adds the given note to the labeled item.
  //! \param [in] theNoteLabel - note label.
  //! \param [in] theItemLabel - item label.
  //! \return a handle to the assembly reference attribute.
  Standard_EXPORT Handle(XCAFDoc_AssemblyItemRef) AddNote(const TDF_Label& theNoteLabel,
                                                          const TDF_Label& theItemLabel);

  //! Adds the given note to the assembly item's attribute.
  //! \param [in] theNoteLabel - note label.
  //! \param [in] theItemId    - assembly item ID.
  //! \param [in] theGUID      - assembly item's attribute GUID.
  //! \return a handle to the assembly reference attribute.
  Standard_EXPORT Handle(XCAFDoc_AssemblyItemRef) AddNoteToAttr(const TDF_Label&              theNoteLabel,
                                                                const XCAFDoc_AssemblyItemId& theItemId,
                                                                const Standard_GUID&          theGUID);

  //! Adds the given note to the labeled item's attribute.
  //! \param [in] theNoteLabel - note label.
  //! \param [in] theItemLabel - item label.
  //! \param [in] theGUID      - assembly item's attribute GUID.
  //! \return a handle to the assembly reference attribute.
  Standard_EXPORT Handle(XCAFDoc_AssemblyItemRef) AddNoteToAttr(const TDF_Label&     theNoteLabel,
                                                                const TDF_Label&     theItemLabel,
                                                                const Standard_GUID& theGUID);

  //! Adds the given note to the assembly item's subshape.
  //! \param [in] theNoteLabel     - note label.
  //! \param [in] theItemId        - assembly item ID.
  //! \param [in] theSubshapeIndex - assembly item's subshape index.
  //! \return a handle to the assembly reference attribute.
  Standard_EXPORT Handle(XCAFDoc_AssemblyItemRef) AddNoteToSubshape(const TDF_Label&              theNoteLabel,
                                                                    const XCAFDoc_AssemblyItemId& theItemId,
                                                                    Standard_Integer              theSubshapeIndex);

  //! Adds the given note to the labeled item's subshape.
  //! \param [in] theNoteLabel     - note label.
  //! \param [in] theItemLabel     - item label.
  //! \param [in] theSubshapeIndex - assembly item's subshape index.
  //! \return a handle to the assembly reference attribute.
  Standard_EXPORT Handle(XCAFDoc_AssemblyItemRef) AddNoteToSubshape(const TDF_Label& theNoteLabel,
                                                                    const TDF_Label& theItemLabel,
                                                                    Standard_Integer theSubshapeIndex);

  //! @}

  //! @name Remove annotation functions
  //! @{

  //! Removes the given note from the assembly item.
  //! \param [in] theNoteLabel   - note label.
  //! \param [in] theItemId      - assembly item ID.
  //! \param [in] theDelIfOrphan - deletes the note from the notes hive 
  //!                              if there are no more assembly items
  //!                              linked with the note.
  //! \return true if the note is removed, otherwise - false.
  Standard_EXPORT Standard_Boolean RemoveNote(const TDF_Label&              theNoteLabel,
                                              const XCAFDoc_AssemblyItemId& theItemId,
                                              Standard_Boolean              theDelIfOrphan = Standard_False);

  //! Removes the given note from the labeled item.
  //! \param [in] theNoteLabel   - note label.
  //! \param [in] theItemLabel   - item label.
  //! \param [in] theDelIfOrphan - deletes the note from the notes hive 
  //!                              if there are no more labeled items
  //!                              linked with the note.
  //! \return true if the note is removed, otherwise - false.
  Standard_EXPORT Standard_Boolean RemoveNote(const TDF_Label& theNoteLabel,
                                              const TDF_Label& theItemLabel,
                                              Standard_Boolean theDelIfOrphan = Standard_False);

  //! Removes the given note from the assembly item's subshape.
  //! \param [in] theNoteLabel     - note label.
  //! \param [in] theItemId        - assembly item ID.
  //! \param [in] theSubshapeIndex - assembly item's subshape index.
  //! \param [in] theDelIfOrphan   - deletes the note from the notes hive 
  //!                                if there are no more assembly item's
  //!                                subshape linked with the note.
  //! \return true if the note is removed, otherwise - false.
  Standard_EXPORT Standard_Boolean RemoveSubshapeNote(const TDF_Label&              theNoteLabel,
                                                      const XCAFDoc_AssemblyItemId& theItemId,
                                                      Standard_Integer              theSubshapeIndex,
                                                      Standard_Boolean              theDelIfOrphan = Standard_False);

  //! Removes the given note from the labeled item's subshape.
  //! \param [in] theNoteLabel     - note label.
  //! \param [in] theItemLabel     - item label.
  //! \param [in] theSubshapeIndex - labeled item's subshape index.
  //! \param [in] theDelIfOrphan   - deletes the note from the notes hive 
  //!                                if there are no more assembly item's
  //!                                subshape linked with the note.
  //! \return true if the note is removed, otherwise - false.
  Standard_EXPORT Standard_Boolean RemoveSubshapeNote(const TDF_Label& theNoteLabel,
                                                      const TDF_Label& theItemLabel,
                                                      Standard_Integer theSubshapeIndex,
                                                      Standard_Boolean theDelIfOrphan = Standard_False);

  //! Removes a note from the assembly item's attribute.
  //! \param [in] theNoteLabel   - note label.
  //! \param [in] theItemId      - assembly item ID.
  //! \param [in] theGUID        - assembly item's attribute GUID.
  //! \param [in] theDelIfOrphan - deletes the note from the notes hive 
  //!                              if there are no more assembly item's
  //!                              attribute linked with the note.
  //! \return true if the note is removed, otherwise - false.
  Standard_EXPORT Standard_Boolean RemoveAttrNote(const TDF_Label&              theNoteLabel,
                                                  const XCAFDoc_AssemblyItemId& theItemId,
                                                  const Standard_GUID&          theGUID,
                                                  Standard_Boolean              theDelIfOrphan = Standard_False);

  //! Removes a note from the labeled item's attribute.
  //! \param [in] theNoteLabel   - note label.
  //! \param [in] theItemLabel   - item label.
  //! \param [in] theGUID        - labeled item's attribute GUID.
  //! \param [in] theDelIfOrphan - deletes the note from the notes hive 
  //!                              if there are no more assembly item's
  //!                              attribute linked with the note.
  //! \return true if the note is removed, otherwise - false.
  Standard_EXPORT Standard_Boolean RemoveAttrNote(const TDF_Label&     theNoteLabel,
                                                  const TDF_Label&     theItemLabel,
                                                  const Standard_GUID& theGUID,
                                                  Standard_Boolean     theDelIfOrphan = Standard_False);

  //! Removes all notes from the assembly item.
  //! \param [in] theItemId      - assembly item ID.
  //! \param [in] theDelIfOrphan - deletes removed notes from the notes
  //!                              hive if there are no more annotated items
  //!                              linked with the notes.
  //! \return true if the notes are removed, otherwise - false.
  Standard_EXPORT Standard_Boolean RemoveAllNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                                  Standard_Boolean              theDelIfOrphan = Standard_False);

  //! Removes all notes from the labeled item.
  //! \param [in] theItemLabel   - item label.
  //! \param [in] theDelIfOrphan - deletes removed notes from the notes
  //!                              hive if there are no more annotated items
  //!                              linked with the notes.
  //! \return true if the notes are removed, otherwise - false.
  Standard_EXPORT Standard_Boolean RemoveAllNotes(const TDF_Label& theItemLabel,
                                                  Standard_Boolean theDelIfOrphan = Standard_False);

  //! Removes all notes from the assembly item's subshape.
  //! \param [in] theItemId        - assembly item ID.
  //! \param [in] theSubshapeIndex - assembly item's subshape index.
  //! \param [in] theDelIfOrphan   - deletes removed notes from the notes
  //!                                hive if there are no more annotated items
  //!                                linked with the notes.
  //! \return true if the notes are removed, otherwise - false.
  Standard_EXPORT Standard_Boolean RemoveAllSubshapeNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                                          Standard_Integer              theSubshapeIndex,
                                                          Standard_Boolean              theDelIfOrphan = Standard_False);

  //! Removes all notes from the assembly item's attribute.
  //! \param [in] theItemId      - assembly item ID.
  //! \param [in] theGUID        - assembly item's attribute GUID.
  //! \param [in] theDelIfOrphan - deletes removed notes from the notes
  //!                              hive if there are no more annotated items
  //!                              linked with the notes.
  //! \return true if the notes are removed, otherwise - false.
  Standard_EXPORT Standard_Boolean RemoveAllAttrNotes(const XCAFDoc_AssemblyItemId& theItemId,
                                                      const Standard_GUID&          theGUID,
                                                      Standard_Boolean              theDelIfOrphan = Standard_False);

  //! Removes all notes from the labeled item's attribute.
  //! \param [in] theItemLabel   - item label.
  //! \param [in] theGUID        - labeled item's attribute GUID.
  //! \param [in] theDelIfOrphan - deletes removed notes from the notes
  //!                              hive if there are no more annotated items
  //!                              linked with the notes.
  //! \return true if the notes are removed, otherwise - false.
  Standard_EXPORT Standard_Boolean RemoveAllAttrNotes(const TDF_Label&     theItemLabel,
                                                      const Standard_GUID& theGUID,
                                                      Standard_Boolean     theDelIfOrphan = Standard_False);

  //! @}

  //! @name Delete note functions
  //! @{

  //! Deletes the given note.
  //! Removes all links with items annotated by the note.
  //! \param [in] theNoteLabel - note label.
  //! \return true if the note is deleted, otherwise - false.
  Standard_EXPORT Standard_Boolean DeleteNote(const TDF_Label& theNoteLabel);

  //! Deletes the given notes.
  //! Removes all links with items annotated by the notes.
  //! \param [in] theNoteLabels - note label sequence.
  //! \return number of deleted notes.
  Standard_EXPORT Standard_Integer DeleteNotes(TDF_LabelSequence& theNoteLabels);

  //! Deletes all notes.
  //! Clears all annotations.
  //! \return number of deleted notes.
  Standard_EXPORT Standard_Integer DeleteAllNotes();

  //! @}

  //! @name Orphan annotated items functions
  //! @{

  //! Returns number of notes that aren't linked to annotated items.
  Standard_EXPORT Standard_Integer NbOrphanNotes() const;

  //! Returns note labels that aren't linked to annotated items.
  //! The label sequence isn't cleared beforehand.
  //! \param [out] theNoteLabels - sequence of labels.
  Standard_EXPORT void GetOrphanNotes(TDF_LabelSequence& theNoteLabels) const;
  
  //! Deletes all notes that aren't linked to annotated items.
  //! \return number of deleted notes.
  Standard_EXPORT Standard_Integer DeleteOrphanNotes();

  //! @}

public:

  // Overrides TDF_Attribute virtuals
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theAttrFrom) Standard_OVERRIDE;
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theAttrInto, 
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;
  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& theOS) const Standard_OVERRIDE;

};

#endif // _XCAFDoc_NotesTool_HeaderFile
