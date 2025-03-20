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

#ifndef _StdLPersistent_Real_HeaderFile
#define _StdLPersistent_Real_HeaderFile

#include <StdObjMgt_Attribute.hxx>

#include <TDataStd_Real.hxx>

class StdLPersistent_Real : public StdObjMgt_Attribute<TDataStd_Real>
{
public:
  //! Empty constructor.
  StdLPersistent_Real()
      : myValue(0.0),
        myDimension(0)
  {
  }

  //! Read persistent data from a file.
  inline void Read(StdObjMgt_ReadData& theReadData) { theReadData >> myValue >> myDimension; }

  //! Write persistent data from a file.
  inline void Write(StdObjMgt_WriteData& theWriteData) const
  {
    theWriteData << myValue << myDimension;
  }

  //! Gets persistent child objects
  void PChildren(StdObjMgt_Persistent::SequenceOfPersistent&) const {}

  //! Returns persistent type name
  Standard_CString PName() const { return "PDataStd_Real"; }

  //! Import transient attribute from the persistent data.
  void Import(const Handle(TDataStd_Real)& theAttribute) const
  {
    theAttribute->Set(myValue);
    Standard_DISABLE_DEPRECATION_WARNINGS theAttribute->SetDimension(
      static_cast<TDataStd_RealEnum>(myDimension));
    Standard_ENABLE_DEPRECATION_WARNINGS theAttribute->SetID(TDataStd_Real::GetID());
  }

private:
  Standard_Real    myValue;
  Standard_Integer myDimension;
};

#endif
