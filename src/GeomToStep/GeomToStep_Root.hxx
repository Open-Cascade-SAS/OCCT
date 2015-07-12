// Created on: 1993-06-14
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _GeomToStep_Root_HeaderFile
#define _GeomToStep_Root_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>


//! This class implements the common services for
//! all classes of GeomToStep which report error.
class GeomToStep_Root 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Standard_Boolean IsDone() const;




protected:



  Standard_Boolean done;


private:





};







#endif // _GeomToStep_Root_HeaderFile
