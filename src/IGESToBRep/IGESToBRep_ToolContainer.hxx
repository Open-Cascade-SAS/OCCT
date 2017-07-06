// Created on: 2000-02-07
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

#ifndef _IGESToBRep_ToolContainer_HeaderFile
#define _IGESToBRep_ToolContainer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class IGESToBRep_IGESBoundary;


class IGESToBRep_ToolContainer;
DEFINE_STANDARD_HANDLE(IGESToBRep_ToolContainer, Standard_Transient)


class IGESToBRep_ToolContainer : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT IGESToBRep_ToolContainer();
  
  //! Returns IGESToBRep_IGESBoundary
  Standard_EXPORT virtual Handle(IGESToBRep_IGESBoundary) IGESBoundary() const;




  DEFINE_STANDARD_RTTIEXT(IGESToBRep_ToolContainer,Standard_Transient)

protected:




private:




};







#endif // _IGESToBRep_ToolContainer_HeaderFile
