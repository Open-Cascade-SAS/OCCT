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
#include <TDataXtd_ConstraintEnum.hxx>


class StdPersistent_DataXtd_Constraint
  : public StdObjMgt_Attribute<TDataXtd_Constraint>
{
public:
  //! Read persistent data from a file.
  inline void Read (StdObjMgt_ReadData& theReadData)
  {
    theReadData >> myType >> myGeometries >> myValue
                >> myIsReversed >> myIsInverted >> myIsVerified >> myPlane;
  }

  //! Import transient attribuite from the persistent data.
  void Import (const Handle(TDataXtd_Constraint)& theAttribute) const;

private:
  Enum<TDataXtd_ConstraintEnum> myType;
  Reference<StdLPersistent_HArray1::Persistent> myGeometries;
  Reference<>                   myValue;
  Value<Standard_Boolean>       myIsReversed;
  Value<Standard_Boolean>       myIsInverted;
  Value<Standard_Boolean>       myIsVerified;
  Reference<>                   myPlane;
};

#endif
