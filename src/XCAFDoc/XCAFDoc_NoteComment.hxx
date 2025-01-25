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

#ifndef _XCAFDoc_NoteComment_HeaderFile
#define _XCAFDoc_NoteComment_HeaderFile

#include <XCAFDoc_Note.hxx>

//! A comment note attribute.
//! Contains a textual comment.
class XCAFDoc_NoteComment : public XCAFDoc_Note
{
public:
  DEFINE_STANDARD_RTTIEXT(XCAFDoc_NoteComment, XCAFDoc_Note)

  //! Returns default attribute GUID
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Finds a reference attribute on the given label and returns it, if it is found
  Standard_EXPORT static Handle(XCAFDoc_NoteComment) Get(const TDF_Label& theLabel);

  //! Create (if not exist) a comment note on the given label.
  //! \param[in]  theLabel     - note label.
  //! \param[in]  theUserName  - the name of the user, who created the note.
  //! \param[in]  theTimeStamp - creation timestamp of the note.
  //! \param[in]  theComment   - comment text.
  Standard_EXPORT static Handle(XCAFDoc_NoteComment) Set(
    const TDF_Label&                  theLabel,
    const TCollection_ExtendedString& theUserName,
    const TCollection_ExtendedString& theTimeStamp,
    const TCollection_ExtendedString& theComment);

  //! Creates an empty comment note.
  Standard_EXPORT XCAFDoc_NoteComment();

  //! Sets the comment text.
  Standard_EXPORT void Set(const TCollection_ExtendedString& theComment);

  //! Returns the comment text.
  const TCollection_ExtendedString& Comment() const { return myComment; }

public:
  // Overrides TDF_Attribute virtuals
  Standard_EXPORT const Standard_GUID&  ID() const Standard_OVERRIDE;
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theAttrFrom) Standard_OVERRIDE;
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theAttrInto,
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;
  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& theOS) const Standard_OVERRIDE;

protected:
  TCollection_ExtendedString myComment; ///< Comment text.
};

DEFINE_STANDARD_HANDLE(XCAFDoc_NoteComment, XCAFDoc_Note)

#endif // _XCAFDoc_NoteComment_HeaderFile
