// Created on: 1991-09-18
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Expr_UnknownIterator_HeaderFile
#define _Expr_UnknownIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Expr_MapOfNamedUnknown.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Expr_GeneralExpression;
class Expr_NamedUnknown;


//! Describes an iterator on NamedUnknowns contained
//! in any GeneralExpression.
class Expr_UnknownIterator 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Expr_UnknownIterator(const Handle(Expr_GeneralExpression)& exp);
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT Handle(Expr_NamedUnknown) Value() const;




protected:





private:

  
  Standard_EXPORT void Perform (const Handle(Expr_GeneralExpression)& exp);


  Expr_MapOfNamedUnknown myMap;
  Standard_Integer myCurrent;


};







#endif // _Expr_UnknownIterator_HeaderFile
