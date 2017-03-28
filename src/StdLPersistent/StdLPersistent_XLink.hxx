// Copyright (c) 2015 OPEN CASCADE SAS
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


#ifndef _StdLPersistent_XLink_HeaderFile
#define _StdLPersistent_XLink_HeaderFile

#include <StdObjMgt_Attribute.hxx>
#include <StdLPersistent_HString.hxx>

#include <TDocStd_XLink.hxx>


class StdLPersistent_XLink : public StdObjMgt_Attribute<TDocStd_XLink>
{
public:
  //! Read persistent data from a file.
  inline void Read (StdObjMgt_ReadData& theReadData)
    { theReadData >> myDocEntry >> myLabEntry; }
  //! Write persistent data to a file.
  inline void Write (StdObjMgt_WriteData& theWriteData) const
    { theWriteData << myDocEntry << myLabEntry; }
  //! Gets persistent child objects
  inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
  {
    theChildren.Append(myDocEntry);
    theChildren.Append(myLabEntry);
  }
  //! Returns persistent type name
  inline Standard_CString PName() const { return "PDocStd_XLink"; }

  //! Import transient attribuite from the persistent data.
  void Import (const Handle(TDocStd_XLink)& theAttribute) const
  {
    if (myDocEntry)
      theAttribute->DocumentEntry (myDocEntry->Value()->String());

    if (myLabEntry)
      theAttribute->LabelEntry    (myLabEntry->Value()->String());
  }

private:
  Handle(StdLPersistent_HString::Ascii) myDocEntry;
  Handle(StdLPersistent_HString::Ascii) myLabEntry;
};

#endif
