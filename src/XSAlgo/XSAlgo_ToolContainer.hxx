// Created on: 2000-01-19
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

#ifndef _XSAlgo_ToolContainer_HeaderFile
#define _XSAlgo_ToolContainer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MMgt_TShared.hxx>


class XSAlgo_ToolContainer;
DEFINE_STANDARD_HANDLE(XSAlgo_ToolContainer, MMgt_TShared)

//! Returns tools used by AlgoContainer
class XSAlgo_ToolContainer : public MMgt_TShared
{

public:

  
  //! Empty constructor
  Standard_EXPORT XSAlgo_ToolContainer();




  DEFINE_STANDARD_RTTI(XSAlgo_ToolContainer,MMgt_TShared)

protected:




private:




};







#endif // _XSAlgo_ToolContainer_HeaderFile
