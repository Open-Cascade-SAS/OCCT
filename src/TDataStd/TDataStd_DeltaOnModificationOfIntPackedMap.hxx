// Created on: 2008-01-23
// Created by: Sergey ZARITCHNY
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _TDataStd_DeltaOnModificationOfIntPackedMap_HeaderFile
#define _TDataStd_DeltaOnModificationOfIntPackedMap_HeaderFile

#include <Standard.hxx>

#include <TDF_DeltaOnModification.hxx>
class TColStd_HPackedMapOfInteger;
class TDataStd_IntPackedMap;


class TDataStd_DeltaOnModificationOfIntPackedMap;
DEFINE_STANDARD_HANDLE(TDataStd_DeltaOnModificationOfIntPackedMap, TDF_DeltaOnModification)

//! This class provides default services for an
//! AttributeDelta on a MODIFICATION action.
class TDataStd_DeltaOnModificationOfIntPackedMap : public TDF_DeltaOnModification
{

public:

  
  //! Initializes a TDF_DeltaOnModification.
  Standard_EXPORT TDataStd_DeltaOnModificationOfIntPackedMap(const Handle(TDataStd_IntPackedMap)& Arr);
  
  //! Applies the delta to the attribute.
  Standard_EXPORT virtual void Apply() Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataStd_DeltaOnModificationOfIntPackedMap,TDF_DeltaOnModification)

protected:




private:


  Handle(TColStd_HPackedMapOfInteger) myAddition;
  Handle(TColStd_HPackedMapOfInteger) myDeletion;


};







#endif // _TDataStd_DeltaOnModificationOfIntPackedMap_HeaderFile
