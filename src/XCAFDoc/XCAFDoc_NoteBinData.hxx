// Created on: 2017-02-13
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

#ifndef _XCAFDoc_NoteBinData_HeaderFile
#define _XCAFDoc_NoteBinData_HeaderFile

#include <XCAFDoc_Note.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

class OSD_File;

class XCAFDoc_NoteBinData;
DEFINE_STANDARD_HANDLE(XCAFDoc_NoteBinData, XCAFDoc_Note)

class XCAFDoc_NoteBinData : public XCAFDoc_Note
{
public:

  DEFINE_STANDARD_RTTIEXT(XCAFDoc_NoteBinData, XCAFDoc_Note)

  Standard_EXPORT static const Standard_GUID& GetID();

  //! Finds a binary data attribute on the given label and returns it, if it is found
  Standard_EXPORT static Handle(XCAFDoc_NoteBinData) Get(const TDF_Label& theLabel);

  //! @name Set attribute functions.
  //! @{

  //! Create (if not exist) a binary note with data loaded from a binary file.
  //! \param [in] theLabel     - label to add the attribute.
  //! \param [in] theUserName  - the name of the user, who created the note.
  //! \param [in] theTimeStamp - creation timestamp of the note.
  //! \param [in] theTitle     - file title.
  //! \param [in] theMIMEtype  - MIME type of the file.
  //! \param [in] theFile      - input binary file.
  //! \return A handle to the attribute instance.
  Standard_EXPORT static Handle(XCAFDoc_NoteBinData) Set(const TDF_Label&                  theLabel,
                                                         const TCollection_ExtendedString& theUserName,
                                                         const TCollection_ExtendedString& theTimeStamp,
                                                         const TCollection_ExtendedString& theTitle,
                                                         const TCollection_AsciiString&    theMIMEtype,
                                                         OSD_File&                         theFile);

  //! Create (if not exist) a binary note byte data array.
  //! \param [in] theLabel     - label to add the attribute.
  //! \param [in] theUserName  - the name of the user, who created the note.
  //! \param [in] theTimeStamp - creation timestamp of the note.
  //! \param [in] theTitle     - data title.
  //! \param [in] theMIMEtype  - MIME type of data.
  //! \param [in] theData      - byte data array.
  //! \return A handle to the attribute instance.
  Standard_EXPORT static Handle(XCAFDoc_NoteBinData) Set(const TDF_Label&                     theLabel,
                                                         const TCollection_ExtendedString&    theUserName,
                                                         const TCollection_ExtendedString&    theTimeStamp,
                                                         const TCollection_ExtendedString&    theTitle,
                                                         const TCollection_AsciiString&       theMIMEtype,
                                                         const Handle(TColStd_HArray1OfByte)& theData);

  //! @}

  //! Creates an empty binary data note.
  Standard_EXPORT XCAFDoc_NoteBinData();

  //! @name Set attribute data functions.
  //! @{

  //! Sets title, MIME type and data from a binary file.
  //! \param [in] theTitle     - file title.
  //! \param [in] theMIMEtype  - MIME type of the file.
  //! \param [in] theFile      - input binary file.
  Standard_EXPORT Standard_Boolean Set(const TCollection_ExtendedString& theTitle,
                                       const TCollection_AsciiString&    theMIMEtype,
                                       OSD_File&                         theFile);

  //! Sets title, MIME type and data from a byte array.
  //! \param [in] theTitle     - data title.
  //! \param [in] theMIMEtype  - MIME type of data.
  //! \param [in] theData      - byte data array.
  Standard_EXPORT void Set(const TCollection_ExtendedString&    theTitle,
                           const TCollection_AsciiString&       theMIMEtype,
                           const Handle(TColStd_HArray1OfByte)& theData);

  //! @}

  //! Returns the note title.
  Standard_EXPORT const TCollection_ExtendedString& Title() const;

  //! Returns data MIME type.
  Standard_EXPORT const TCollection_AsciiString& MIMEtype() const;

  //! Size of data in bytes.
  Standard_EXPORT Standard_Integer Size() const;

  //! Returns byte data array.
  Standard_EXPORT const Handle(TColStd_HArray1OfByte)& Data() const;

public:

  // Overrides TDF_Attribute virtuals
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& theAttrFrom) Standard_OVERRIDE;
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       theAttrInto,
                             const Handle(TDF_RelocationTable)& theRT) const Standard_OVERRIDE;
  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& theOS) const Standard_OVERRIDE;

protected:

  TCollection_ExtendedString    myTitle;    ///< Note title.
  TCollection_AsciiString       myMIMEtype; ///< MIME type of data.
  Handle(TColStd_HArray1OfByte) myData;     ///< Byte data array.

};

#endif // _XCAFDoc_NoteBinData_HeaderFile
