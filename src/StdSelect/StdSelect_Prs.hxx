// Created on: 1995-03-17
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StdSelect_Prs_HeaderFile
#define _StdSelect_Prs_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Prs3d_Presentation.hxx>
class Graphic3d_StructureManager;


class StdSelect_Prs;
DEFINE_STANDARD_HANDLE(StdSelect_Prs, Prs3d_Presentation)

//! allows entities owners to be hilighted
//! independantly from PresentableObjects
class StdSelect_Prs : public Prs3d_Presentation
{

public:

  
  Standard_EXPORT StdSelect_Prs(const Handle(Graphic3d_StructureManager)& aStructureManager);
  
    const Handle(Graphic3d_StructureManager)& Manager() const;




  DEFINE_STANDARD_RTTIEXT(StdSelect_Prs,Prs3d_Presentation)

protected:




private:


  Handle(Graphic3d_StructureManager) myManager;


};


#include <StdSelect_Prs.lxx>





#endif // _StdSelect_Prs_HeaderFile
