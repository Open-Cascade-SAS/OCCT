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

  
  //! Creates an iterator which will work on all the dictionary
  Standard_EXPORT STEPCAFControl_IteratorOfDictionaryOfExternFile(const Handle(STEPCAFControl_DictionaryOfExternFile)& acell);
  
  //! Creates an iterator which will consider only entries
  //! which name begin by the string given as basename (subpart)
  Standard_EXPORT STEPCAFControl_IteratorOfDictionaryOfExternFile(const Handle(STEPCAFControl_DictionaryOfExternFile)& acell, const Standard_CString basename);
  
  //! Creates an iterator which will consider only entries
  //! which name begin by the string given as basename (subpart)
  //! Same as above, but basename is String instead of CString
  Standard_EXPORT STEPCAFControl_IteratorOfDictionaryOfExternFile(const Handle(STEPCAFControl_DictionaryOfExternFile)& acell, const TCollection_AsciiString& basename);
  
  //! Allows to Start a new Iteration from beginning
  Standard_EXPORT void Start();
  
  //! Returns True if there are more entries to return
  Standard_EXPORT Standard_Boolean More();
  
  //! Go to the next entry
  //! (if there is not, Value will raise an exception)
  Standard_EXPORT void Next();
  
  //! Returns item value of current entry
  Standard_EXPORT const Handle(STEPCAFControl_ExternFile)& Value() const;
  
  //! Returns name of current entry
  Standard_EXPORT TCollection_AsciiString Name() const;




protected:





private:

  
  //! Appends a new value to the Iteration Stack
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
