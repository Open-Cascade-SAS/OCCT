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

#ifndef _HeaderSection_HeaderFile
#define _HeaderSection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class HeaderSection_Protocol;
class HeaderSection_Protocol;
class HeaderSection_FileName;
class HeaderSection_FileDescription;
class HeaderSection_FileSchema;
class HeaderSection_HeaderRecognizer;



class HeaderSection 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates a Protocol
  Standard_EXPORT static Handle(HeaderSection_Protocol) Protocol();




protected:





private:




friend class HeaderSection_Protocol;
friend class HeaderSection_FileName;
friend class HeaderSection_FileDescription;
friend class HeaderSection_FileSchema;
friend class HeaderSection_HeaderRecognizer;

};







#endif // _HeaderSection_HeaderFile
