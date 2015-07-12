// Created on: 2001-09-19
// Created by: admin of fao FACTORY
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _XmlLDrivers_NamespaceDef_HeaderFile
#define _XmlLDrivers_NamespaceDef_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_AsciiString.hxx>
class TCollection_AsciiString;



class XmlLDrivers_NamespaceDef 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT XmlLDrivers_NamespaceDef();
  
  Standard_EXPORT XmlLDrivers_NamespaceDef(const TCollection_AsciiString& thePrefix, const TCollection_AsciiString& theURI);
  
  Standard_EXPORT const TCollection_AsciiString& Prefix() const;
  
  Standard_EXPORT const TCollection_AsciiString& URI() const;




protected:





private:



  TCollection_AsciiString myPrefix;
  TCollection_AsciiString myURI;


};







#endif // _XmlLDrivers_NamespaceDef_HeaderFile
