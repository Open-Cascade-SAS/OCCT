// Created on: 1997-12-03
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _TNaming_DeltaOnRemoval_HeaderFile
#define _TNaming_DeltaOnRemoval_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_DeltaOnRemoval.hxx>
class TNaming_DeltaOnModification;
class TNaming_NamedShape;


class TNaming_DeltaOnRemoval;
DEFINE_STANDARD_HANDLE(TNaming_DeltaOnRemoval, TDF_DeltaOnRemoval)


class TNaming_DeltaOnRemoval : public TDF_DeltaOnRemoval
{

public:

  
  //! Initializes a TDF_DeltaOnModification.
  Standard_EXPORT TNaming_DeltaOnRemoval(const Handle(TNaming_NamedShape)& NS);
  
  //! Applies the delta to the attribute.
  Standard_EXPORT virtual void Apply() Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TNaming_DeltaOnRemoval,TDF_DeltaOnRemoval)

protected:




private:


  Handle(TNaming_DeltaOnModification) myDelta;


};







#endif // _TNaming_DeltaOnRemoval_HeaderFile
