// Created on: 1998-07-17
// Created by: Jean-Louis Frenkel
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _CDF_Timer_HeaderFile
#define _CDF_Timer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <OSD_Timer.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>



class CDF_Timer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT CDF_Timer();
  
  Standard_EXPORT void ShowAndRestart (const Standard_CString aMessage);
  
  Standard_EXPORT void ShowAndStop (const Standard_CString aMessage);
  
  Standard_EXPORT Standard_Boolean MustShow();




protected:





private:

  
  Standard_EXPORT void Show (const Standard_CString aMessage);


  OSD_Timer myTimer;


};







#endif // _CDF_Timer_HeaderFile
