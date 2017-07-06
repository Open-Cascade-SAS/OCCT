// Created on: 1991-01-14
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

#ifndef _Expr_GeneralRelation_HeaderFile
#define _Expr_GeneralRelation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Standard_OutOfRange;
class Standard_NumericError;
class Expr_GeneralExpression;
class Expr_NamedUnknown;
class TCollection_AsciiString;


class Expr_GeneralRelation;
DEFINE_STANDARD_HANDLE(Expr_GeneralRelation, Standard_Transient)

//! Defines the general purposes of any relation between
//! expressions.
class Expr_GeneralRelation : public Standard_Transient
{

public:

  
  //! Returns the current status of the relation
  Standard_EXPORT virtual Standard_Boolean IsSatisfied() const = 0;
  
  //! Tests if <me> is linear between its NamedUnknowns.
  Standard_EXPORT virtual Standard_Boolean IsLinear() const = 0;
  
  //! Returns a GeneralRelation after replacement of
  //! NamedUnknowns by an associated expression, and after
  //! values computation.
  Standard_EXPORT virtual Handle(Expr_GeneralRelation) Simplified() const = 0;
  
  //! Replaces NamedUnknowns by associated expressions,
  //! and computes values in <me>.
  Standard_EXPORT virtual void Simplify() = 0;
  
  //! Returns a copy of <me> having the same unknowns and
  //! functions.
  Standard_EXPORT virtual Handle(Expr_GeneralRelation) Copy() const = 0;
  
  //! Returns the number of relations contained in <me>.
  Standard_EXPORT virtual Standard_Integer NbOfSubRelations() const = 0;
  
  //! Returns the number of SingleRelations contained in
  //! <me>.
  Standard_EXPORT virtual Standard_Integer NbOfSingleRelations() const = 0;
  
  //! Returns the relation denoted by <index> in <me>.
  //! An exception is raised if <index> is out of range.
  Standard_EXPORT virtual Handle(Expr_GeneralRelation) SubRelation (const Standard_Integer index) const = 0;
  
  //! Tests if <exp> contains <var>.
  Standard_EXPORT virtual Standard_Boolean Contains (const Handle(Expr_GeneralExpression)& exp) const = 0;
  
  //! Replaces all occurences of <var> with <with> in <me>.
  Standard_EXPORT virtual void Replace (const Handle(Expr_NamedUnknown)& var, const Handle(Expr_GeneralExpression)& with) = 0;
  
  //! returns a string representing <me> in a readable way.
  Standard_EXPORT virtual TCollection_AsciiString String() const = 0;




  DEFINE_STANDARD_RTTIEXT(Expr_GeneralRelation,Standard_Transient)

protected:




private:




};







#endif // _Expr_GeneralRelation_HeaderFile
