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

#include <ShapeProcess_IteratorOfDictionaryOfOperator.hxx>

#include <ShapeProcess_DictionaryOfOperator.hxx>
#include <ShapeProcess_StackItemOfDictionaryOfOperator.hxx>
#include <Standard_NoSuchObject.hxx>
#include <ShapeProcess_Operator.hxx>
#include <TCollection_AsciiString.hxx>
 

#define TheItem Handle(ShapeProcess_Operator)
#define TheItem_hxx <ShapeProcess_Operator.hxx>
#define Dico_Iterator ShapeProcess_IteratorOfDictionaryOfOperator
#define Dico_Iterator_hxx <ShapeProcess_IteratorOfDictionaryOfOperator.hxx>
#define Dico_StackItem ShapeProcess_StackItemOfDictionaryOfOperator
#define Dico_StackItem_hxx <ShapeProcess_StackItemOfDictionaryOfOperator.hxx>
#define Handle_Dico_StackItem Handle(ShapeProcess_StackItemOfDictionaryOfOperator)
#define Dico_Dictionary ShapeProcess_DictionaryOfOperator
#define Dico_Dictionary_hxx <ShapeProcess_DictionaryOfOperator.hxx>
#define Handle_Dico_Dictionary Handle(ShapeProcess_DictionaryOfOperator)
#include <Dico_Iterator.gxx>

