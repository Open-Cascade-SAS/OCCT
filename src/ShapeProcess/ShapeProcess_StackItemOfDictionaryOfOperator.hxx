// Created on: 2000-08-21
// Created by: Andrey BETENEV
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

#ifndef _ShapeProcess_StackItemOfDictionaryOfOperator_HeaderFile
#define _ShapeProcess_StackItemOfDictionaryOfOperator_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MMgt_TShared.hxx>
class ShapeProcess_DictionaryOfOperator;
class ShapeProcess_Operator;
class ShapeProcess_IteratorOfDictionaryOfOperator;


class ShapeProcess_StackItemOfDictionaryOfOperator;
DEFINE_STANDARD_HANDLE(ShapeProcess_StackItemOfDictionaryOfOperator, MMgt_TShared)


class ShapeProcess_StackItemOfDictionaryOfOperator : public MMgt_TShared
{

public:

  
  //! Creates a StackItem with no Previous one
  Standard_EXPORT ShapeProcess_StackItemOfDictionaryOfOperator();
  
  //! Creates a StackItem with a Previous one
  Standard_EXPORT ShapeProcess_StackItemOfDictionaryOfOperator(const Handle(ShapeProcess_StackItemOfDictionaryOfOperator)& previous);
  
  //! Returns the Previous Item (is Null if no Previous defined)
  Standard_EXPORT Handle(ShapeProcess_StackItemOfDictionaryOfOperator) Previous() const;
  
  //! Returns the Dictionary Cell corresponding to an Item
  Standard_EXPORT Handle(ShapeProcess_DictionaryOfOperator) Value() const;
  
  //! Sets a new Dictionary Cell as Value of an Item
  Standard_EXPORT void SetValue (const Handle(ShapeProcess_DictionaryOfOperator)& cval);




  DEFINE_STANDARD_RTTI(ShapeProcess_StackItemOfDictionaryOfOperator,MMgt_TShared)

protected:




private:


  Handle(ShapeProcess_DictionaryOfOperator) thevalue;
  Handle(ShapeProcess_StackItemOfDictionaryOfOperator) theprev;


};







#endif // _ShapeProcess_StackItemOfDictionaryOfOperator_HeaderFile
