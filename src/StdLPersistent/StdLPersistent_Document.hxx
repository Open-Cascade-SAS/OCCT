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


#ifndef _StdLPersistent_Document_HeaderFile
#define _StdLPersistent_Document_HeaderFile

#include <StdObjMgt_Persistent.hxx>

class TDocStd_Document;
class StdLPersistent_Data;


class StdLPersistent_Document : public StdObjMgt_Persistent
{
public:
  //! Read persistent data from a file.
  Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);
  //! Read persistent data from a file.
  Standard_EXPORT virtual void Write (StdObjMgt_WriteData& theWriteData) const;
  //! Gets persistent child objects
  Standard_EXPORT virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent&) const;
  //! Returns persistent type name
  Standard_EXPORT virtual Standard_CString PName() const
    { return "PDocStd_Document"; }

  //! Import transient document from the persistent data.
  Standard_EXPORT virtual void ImportDocument
    (const Handle(TDocStd_Document)& theDocument) const;

private:
  Handle(StdLPersistent_Data) myData;
};

#endif
