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

#ifndef _Expr_SystemRelation_HeaderFile
#define _Expr_SystemRelation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Expr_SequenceOfGeneralRelation.hxx>
#include <Expr_GeneralRelation.hxx>
#include <Standard_Integer.hxx>
class Expr_GeneralExpression;
class Expr_NamedUnknown;
class TCollection_AsciiString;

class Expr_SystemRelation;
DEFINE_STANDARD_HANDLE(Expr_SystemRelation, Expr_GeneralRelation)

class Expr_SystemRelation : public Expr_GeneralRelation
{

public:
  //! Creates a system with one relation
  Standard_EXPORT Expr_SystemRelation(const Handle(Expr_GeneralRelation)& relation);

  //! Appends <relation> in the list of components of <me>.
  Standard_EXPORT void Add(const Handle(Expr_GeneralRelation)& relation);

  Standard_EXPORT void Remove(const Handle(Expr_GeneralRelation)& relation);

  //! Tests if <me> is linear between its NamedUnknowns.
  Standard_EXPORT Standard_Boolean IsLinear() const Standard_OVERRIDE;

  //! Returns the number of relations contained in <me>.
  Standard_EXPORT Standard_Integer NbOfSubRelations() const Standard_OVERRIDE;

  //! Returns the number of SingleRelations contained in
  //! <me>.
  Standard_EXPORT Standard_Integer NbOfSingleRelations() const Standard_OVERRIDE;

  //! Returns the relation denoted by <index> in <me>.
  //! An exception is raised if <index> is out of range.
  Standard_EXPORT Handle(Expr_GeneralRelation) SubRelation(const Standard_Integer index) const
    Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean IsSatisfied() const Standard_OVERRIDE;

  //! Returns a GeneralRelation after replacement of
  //! NamedUnknowns by an associated expression, and after
  //! values computation.
  Standard_EXPORT Handle(Expr_GeneralRelation) Simplified() const Standard_OVERRIDE;

  //! Replaces NamedUnknowns by associated expressions,
  //! and computes values in <me>.
  Standard_EXPORT void Simplify() Standard_OVERRIDE;

  //! Returns a copy of <me> having the same unknowns and functions.
  Standard_EXPORT Handle(Expr_GeneralRelation) Copy() const Standard_OVERRIDE;

  //! Tests if <me> contains <exp>.
  Standard_EXPORT Standard_Boolean
    Contains(const Handle(Expr_GeneralExpression)& exp) const Standard_OVERRIDE;

  //! Replaces all occurrences of <var> with <with> in <me>.
  Standard_EXPORT void Replace(const Handle(Expr_NamedUnknown)&      var,
                               const Handle(Expr_GeneralExpression)& with) Standard_OVERRIDE;

  //! returns a string representing <me> in a readable way.
  Standard_EXPORT TCollection_AsciiString String() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Expr_SystemRelation, Expr_GeneralRelation)

protected:
private:
  Expr_SequenceOfGeneralRelation myRelations;
};

#endif // _Expr_SystemRelation_HeaderFile
