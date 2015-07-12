// Created on: 1992-07-28
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Dico_IteratorOfDictionaryOfTransient_HeaderFile
#define _Dico_IteratorOfDictionaryOfTransient_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
class Dico_DictionaryOfTransient;
class Dico_StackItemOfDictionaryOfTransient;
class Standard_NoSuchObject;
class Standard_Transient;
class TCollection_AsciiString;



class Dico_IteratorOfDictionaryOfTransient 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Dico_IteratorOfDictionaryOfTransient(const Handle(Dico_DictionaryOfTransient)& acell);
  
  Standard_EXPORT Dico_IteratorOfDictionaryOfTransient(const Handle(Dico_DictionaryOfTransient)& acell, const Standard_CString basename);
  
  Standard_EXPORT Dico_IteratorOfDictionaryOfTransient(const Handle(Dico_DictionaryOfTransient)& acell, const TCollection_AsciiString& basename);
  
  Standard_EXPORT void Start();
  
  Standard_EXPORT Standard_Boolean More();
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT const Handle(Standard_Transient)& Value() const;
  
  Standard_EXPORT TCollection_AsciiString Name() const;




protected:





private:

  
  Standard_EXPORT void AppendStack (const Handle(Dico_DictionaryOfTransient)& val);


  Handle(Dico_DictionaryOfTransient) thebase;
  TCollection_AsciiString thename;
  Handle(Dico_StackItemOfDictionaryOfTransient) thelast;
  Standard_Integer thenb;
  Standard_Boolean themore;
  Standard_Boolean theinit;
  Standard_Boolean thenext;


};







#endif // _Dico_IteratorOfDictionaryOfTransient_HeaderFile
