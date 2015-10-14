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

#ifndef _Dico_StackItemOfDictionaryOfInteger_HeaderFile
#define _Dico_StackItemOfDictionaryOfInteger_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MMgt_TShared.hxx>
#include <Standard_Integer.hxx>
class Dico_DictionaryOfInteger;
class Dico_IteratorOfDictionaryOfInteger;


class Dico_StackItemOfDictionaryOfInteger;
DEFINE_STANDARD_HANDLE(Dico_StackItemOfDictionaryOfInteger, MMgt_TShared)


class Dico_StackItemOfDictionaryOfInteger : public MMgt_TShared
{

public:

  
  //! Creates a StackItem with no Previous one
  Standard_EXPORT Dico_StackItemOfDictionaryOfInteger();
  
  //! Creates a StackItem with a Previous one
  Standard_EXPORT Dico_StackItemOfDictionaryOfInteger(const Handle(Dico_StackItemOfDictionaryOfInteger)& previous);
  
  //! Returns the Previous Item (is Null if no Previous defined)
  Standard_EXPORT Handle(Dico_StackItemOfDictionaryOfInteger) Previous() const;
  
  //! Returns the Dictionary Cell corresponding to an Item
  Standard_EXPORT Handle(Dico_DictionaryOfInteger) Value() const;
  
  //! Sets a new Dictionary Cell as Value of an Item
  Standard_EXPORT void SetValue (const Handle(Dico_DictionaryOfInteger)& cval);




  DEFINE_STANDARD_RTTI(Dico_StackItemOfDictionaryOfInteger,MMgt_TShared)

protected:




private:


  Handle(Dico_DictionaryOfInteger) thevalue;
  Handle(Dico_StackItemOfDictionaryOfInteger) theprev;


};







#endif // _Dico_StackItemOfDictionaryOfInteger_HeaderFile
