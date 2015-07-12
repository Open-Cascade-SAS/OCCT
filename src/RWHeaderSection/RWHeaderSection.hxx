// Created on: 1994-06-16
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _RWHeaderSection_HeaderFile
#define _RWHeaderSection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class RWHeaderSection_ReadWriteModule;
class RWHeaderSection_GeneralModule;
class RWHeaderSection_RWFileName;
class RWHeaderSection_RWFileDescription;
class RWHeaderSection_RWFileSchema;



class RWHeaderSection 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! enforced the initialisation of the  libraries
  Standard_EXPORT static void Init();




protected:





private:




friend class RWHeaderSection_ReadWriteModule;
friend class RWHeaderSection_GeneralModule;
friend class RWHeaderSection_RWFileName;
friend class RWHeaderSection_RWFileDescription;
friend class RWHeaderSection_RWFileSchema;

};







#endif // _RWHeaderSection_HeaderFile
