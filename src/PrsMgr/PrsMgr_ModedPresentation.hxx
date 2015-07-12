// Created on: 1995-01-30
// Created by: Mister rmi
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

#ifndef _PrsMgr_ModedPresentation_HeaderFile
#define _PrsMgr_ModedPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class PrsMgr_Presentation;



class PrsMgr_ModedPresentation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT PrsMgr_ModedPresentation();
  
  Standard_EXPORT PrsMgr_ModedPresentation(const Handle(PrsMgr_Presentation)& thePrs, const Standard_Integer theMode);
  
  Standard_EXPORT const Handle(PrsMgr_Presentation)& Presentation() const;
  
  Standard_EXPORT Standard_Integer Mode() const;




protected:





private:



  Handle(PrsMgr_Presentation) myPresentation;
  Standard_Integer myMode;


};







#endif // _PrsMgr_ModedPresentation_HeaderFile
