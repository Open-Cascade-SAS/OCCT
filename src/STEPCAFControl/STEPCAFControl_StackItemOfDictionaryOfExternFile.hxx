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

#ifndef _STEPCAFControl_StackItemOfDictionaryOfExternFile_HeaderFile
#define _STEPCAFControl_StackItemOfDictionaryOfExternFile_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MMgt_TShared.hxx>
class STEPCAFControl_DictionaryOfExternFile;
class STEPCAFControl_ExternFile;
class STEPCAFControl_IteratorOfDictionaryOfExternFile;


class STEPCAFControl_StackItemOfDictionaryOfExternFile;
DEFINE_STANDARD_HANDLE(STEPCAFControl_StackItemOfDictionaryOfExternFile, MMgt_TShared)


class STEPCAFControl_StackItemOfDictionaryOfExternFile : public MMgt_TShared
{

public:

  
  //! Creates a StackItem with no Previous one
  Standard_EXPORT STEPCAFControl_StackItemOfDictionaryOfExternFile();
  
  //! Creates a StackItem with a Previous one
  Standard_EXPORT STEPCAFControl_StackItemOfDictionaryOfExternFile(const Handle(STEPCAFControl_StackItemOfDictionaryOfExternFile)& previous);
  
  //! Returns the Previous Item (is Null if no Previous defined)
  Standard_EXPORT Handle(STEPCAFControl_StackItemOfDictionaryOfExternFile) Previous() const;
  
  //! Returns the Dictionary Cell corresponding to an Item
  Standard_EXPORT Handle(STEPCAFControl_DictionaryOfExternFile) Value() const;
  
  //! Sets a new Dictionary Cell as Value of an Item
  Standard_EXPORT void SetValue (const Handle(STEPCAFControl_DictionaryOfExternFile)& cval);




  DEFINE_STANDARD_RTTI(STEPCAFControl_StackItemOfDictionaryOfExternFile,MMgt_TShared)

protected:




private:


  Handle(STEPCAFControl_DictionaryOfExternFile) thevalue;
  Handle(STEPCAFControl_StackItemOfDictionaryOfExternFile) theprev;


};







#endif // _STEPCAFControl_StackItemOfDictionaryOfExternFile_HeaderFile
