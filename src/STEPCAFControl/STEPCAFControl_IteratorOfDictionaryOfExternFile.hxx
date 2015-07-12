// Created on: 2000-04-09
// Created by: Sergey MOZOKHIN
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

#ifndef _STEPCAFControl_IteratorOfDictionaryOfExternFile_HeaderFile
#define _STEPCAFControl_IteratorOfDictionaryOfExternFile_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
class STEPCAFControl_DictionaryOfExternFile;
class STEPCAFControl_StackItemOfDictionaryOfExternFile;
class Standard_NoSuchObject;
class STEPCAFControl_ExternFile;
class TCollection_AsciiString;



class STEPCAFControl_IteratorOfDictionaryOfExternFile 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT STEPCAFControl_IteratorOfDictionaryOfExternFile(const Handle(STEPCAFControl_DictionaryOfExternFile)& acell);
  
  Standard_EXPORT STEPCAFControl_IteratorOfDictionaryOfExternFile(const Handle(STEPCAFControl_DictionaryOfExternFile)& acell, const Standard_CString basename);
  
  Standard_EXPORT STEPCAFControl_IteratorOfDictionaryOfExternFile(const Handle(STEPCAFControl_DictionaryOfExternFile)& acell, const TCollection_AsciiString& basename);
  
  Standard_EXPORT void Start();
  
  Standard_EXPORT Standard_Boolean More();
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT const Handle(STEPCAFControl_ExternFile)& Value() const;
  
  Standard_EXPORT TCollection_AsciiString Name() const;




protected:





private:

  
  Standard_EXPORT void AppendStack (const Handle(STEPCAFControl_DictionaryOfExternFile)& val);


  Handle(STEPCAFControl_DictionaryOfExternFile) thebase;
  TCollection_AsciiString thename;
  Handle(STEPCAFControl_StackItemOfDictionaryOfExternFile) thelast;
  Standard_Integer thenb;
  Standard_Boolean themore;
  Standard_Boolean theinit;
  Standard_Boolean thenext;


};







#endif // _STEPCAFControl_IteratorOfDictionaryOfExternFile_HeaderFile
