// Copyright (c) 2017-2018 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_Note_HeaderFile
#define _XCAFDoc_Note_HeaderFile

#include <Standard.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDF_DerivedAttribute.hxx>
#include <TDF_Label.hxx>
#include <NCollection_Sequence.hxx>
#include <XCAFNoteObjects_NoteObject.hxx>

class TDF_RelocationTable;

//! A base note attribute.
//! Any note contains the name of the user created the note
//! and the creation timestamp.
class XCAFDoc_Note : public TDF_Attribute
{
public:
  DEFINE_STANDARD_RTTIEXT(XCAFDoc_Note, TDF_Attribute)

  //! Checks if the given label represents a note.
  Standard_EXPORT static bool IsMine(const TDF_Label& theLabel);

  //! Finds a reference attribute on the given label and returns it, if it is found
  Standard_EXPORT static occ::handle<XCAFDoc_Note> Get(const TDF_Label& theLabel);

  //! Sets the user name and the timestamp of the note.
  //! \param[in]  theUserName  - the user associated with the note.
  //! \param[in]  theTimeStamp - timestamp of the note.
  //! \return A handle to the attribute instance.
  Standard_EXPORT void Set(const TCollection_ExtendedString& theUserName,
                           const TCollection_ExtendedString& theTimeStamp);

  //! Returns the user name, who created the note.
  const TCollection_ExtendedString& UserName() const { return myUserName; }

  //! Returns the timestamp of the note.
  const TCollection_ExtendedString& TimeStamp() const { return myTimeStamp; }

  //! Checks if the note isn't linked to annotated items.
  Standard_EXPORT bool IsOrphan() const;

  //! Returns auxiliary data object
  Standard_EXPORT occ::handle<XCAFNoteObjects_NoteObject> GetObject() const;

  //! Updates auxiliary data
  Standard_EXPORT void SetObject(const occ::handle<XCAFNoteObjects_NoteObject>& theObject);

public:
  // Overrides TDF_Attribute virtuals
  Standard_EXPORT void              Restore(const occ::handle<TDF_Attribute>& theAttrFrom) override;
  Standard_EXPORT void              Paste(const occ::handle<TDF_Attribute>&       theAttrInto,
                                          const occ::handle<TDF_RelocationTable>& theRT) const override;
  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& theOS) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

protected:
  //! Creates an empty note.
  Standard_EXPORT XCAFDoc_Note();

private:
  TCollection_ExtendedString myUserName;  ///< Name of the user, who created the note.
  TCollection_ExtendedString myTimeStamp; ///< Timestamp, when the note was created.
};

#endif // _XCAFDoc_Note_HeaderFile
