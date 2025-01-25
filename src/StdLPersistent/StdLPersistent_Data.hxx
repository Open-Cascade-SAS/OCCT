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

#ifndef _StdLPersistent_Data_HeaderFile
#define _StdLPersistent_Data_HeaderFile

#include <StdLPersistent_HArray1.hxx>

class TDF_Data;

class StdLPersistent_Data : public StdObjMgt_Persistent
{
public:
  //! Empty constructor.
  StdLPersistent_Data()
      : myVersion(0)
  {
  }

  //! Read persistent data from a file.
  Standard_EXPORT virtual void Read(StdObjMgt_ReadData& theReadData);
  //! Write persistent data to a file.
  Standard_EXPORT virtual void Write(StdObjMgt_WriteData& theWriteData) const;

  //! Gets persistent child objects
  virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
  {
    theChildren.Append(myLabels);
    theChildren.Append(myAttributes);
  }

  //! Returns persistent type name
  virtual Standard_CString PName() const { return "PDF_Data"; }

  //! Import transient data from the persistent data.
  Standard_EXPORT Handle(TDF_Data) Import() const;

private:
  class Parser;

private:
  Standard_Integer                           myVersion;
  Handle(StdLPersistent_HArray1::Integer)    myLabels;
  Handle(StdLPersistent_HArray1::Persistent) myAttributes;
};

#endif
