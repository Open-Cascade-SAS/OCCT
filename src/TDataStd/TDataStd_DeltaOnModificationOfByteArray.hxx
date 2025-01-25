// Created on: 2007-12-05
// Created by: Sergey ZARITCHNY
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef _TDataStd_DeltaOnModificationOfByteArray_HeaderFile
#define _TDataStd_DeltaOnModificationOfByteArray_HeaderFile

#include <Standard.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <Standard_Integer.hxx>
#include <TDF_DeltaOnModification.hxx>
class TDataStd_ByteArray;

class TDataStd_DeltaOnModificationOfByteArray;
DEFINE_STANDARD_HANDLE(TDataStd_DeltaOnModificationOfByteArray, TDF_DeltaOnModification)

//! This class provides default services for an
//! AttributeDelta on a MODIFICATION action.
class TDataStd_DeltaOnModificationOfByteArray : public TDF_DeltaOnModification
{

public:
  //! Initializes a TDF_DeltaOnModification.
  Standard_EXPORT TDataStd_DeltaOnModificationOfByteArray(const Handle(TDataStd_ByteArray)& Arr);

  //! Applies the delta to the attribute.
  Standard_EXPORT virtual void Apply() Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(TDataStd_DeltaOnModificationOfByteArray, TDF_DeltaOnModification)

protected:
private:
  Handle(TColStd_HArray1OfInteger) myIndxes;
  Handle(TColStd_HArray1OfByte)    myValues;
  Standard_Integer                 myUp1;
  Standard_Integer                 myUp2;
};

#endif // _TDataStd_DeltaOnModificationOfByteArray_HeaderFile
