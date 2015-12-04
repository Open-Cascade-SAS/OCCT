// Created on: 2007-10-30
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

#ifndef _TDataStd_DeltaOnModificationOfRealArray_HeaderFile
#define _TDataStd_DeltaOnModificationOfRealArray_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Integer.hxx>
#include <TDF_DeltaOnModification.hxx>
class TDataStd_RealArray;


class TDataStd_DeltaOnModificationOfRealArray;
DEFINE_STANDARD_HANDLE(TDataStd_DeltaOnModificationOfRealArray, TDF_DeltaOnModification)

//! This class provides default services for an
//! AttributeDelta on a MODIFICATION action
class TDataStd_DeltaOnModificationOfRealArray : public TDF_DeltaOnModification
{

public:

  
  //! Initializes a TDF_DeltaOnModification.
  Standard_EXPORT TDataStd_DeltaOnModificationOfRealArray(const Handle(TDataStd_RealArray)& Arr);
  
  //! Applies the delta to the attribute.
  Standard_EXPORT virtual void Apply() Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataStd_DeltaOnModificationOfRealArray,TDF_DeltaOnModification)

protected:




private:


  Handle(TColStd_HArray1OfInteger) myIndxes;
  Handle(TColStd_HArray1OfReal) myValues;
  Standard_Integer myUp1;
  Standard_Integer myUp2;


};







#endif // _TDataStd_DeltaOnModificationOfRealArray_HeaderFile
