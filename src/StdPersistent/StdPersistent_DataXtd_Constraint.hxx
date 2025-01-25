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

#ifndef _StdPersistent_DataXtd_Constraint_HeaderFile
#define _StdPersistent_DataXtd_Constraint_HeaderFile

#include <StdObjMgt_Attribute.hxx>
#include <StdLPersistent_HArray1.hxx>

#include <TDataXtd_Constraint.hxx>

class StdPersistent_DataXtd_Constraint : public StdObjMgt_Attribute<TDataXtd_Constraint>
{
public:
  //! Read persistent data from a file.
  inline void Read(StdObjMgt_ReadData& theReadData)
  {
    theReadData >> myType >> myGeometries >> myValue >> myIsReversed >> myIsInverted >> myIsVerified
      >> myPlane;
  }

  //! Write persistent data to a file.
  inline void Write(StdObjMgt_WriteData& theWriteData) const
  {
    theWriteData << myType << myGeometries << myValue << myIsReversed << myIsInverted
                 << myIsVerified << myPlane;
  }

  //! Gets persistent child objects
  inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
  {
    theChildren.Append(myGeometries);
    theChildren.Append(myValue);
    theChildren.Append(myPlane);
  }

  //! Returns persistent type name
  inline Standard_CString PName() const { return "PDataXtd_Constraint"; }

  //! Import transient attribute from the persistent data.
  void Import(const Handle(TDataXtd_Constraint)& theAttribute) const;

private:
  Standard_Integer                           myType;
  Handle(StdLPersistent_HArray1::Persistent) myGeometries;
  Handle(StdObjMgt_Persistent)               myValue;
  Standard_Boolean                           myIsReversed;
  Standard_Boolean                           myIsInverted;
  Standard_Boolean                           myIsVerified;
  Handle(StdObjMgt_Persistent)               myPlane;
};

#endif
