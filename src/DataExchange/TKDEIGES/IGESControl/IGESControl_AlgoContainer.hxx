// Created on: 2000-02-08
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _IGESControl_AlgoContainer_HeaderFile
#define _IGESControl_AlgoContainer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESToBRep_AlgoContainer.hxx>

class IGESControl_AlgoContainer;
DEFINE_STANDARD_HANDLE(IGESControl_AlgoContainer, IGESToBRep_AlgoContainer)

class IGESControl_AlgoContainer : public IGESToBRep_AlgoContainer
{

public:
  //! Empty constructor
  Standard_EXPORT IGESControl_AlgoContainer();

  DEFINE_STANDARD_RTTIEXT(IGESControl_AlgoContainer, IGESToBRep_AlgoContainer)

protected:
private:
};

#endif // _IGESControl_AlgoContainer_HeaderFile
