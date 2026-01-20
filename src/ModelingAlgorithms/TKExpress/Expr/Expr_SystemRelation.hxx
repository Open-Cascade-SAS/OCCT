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

#include <Expr_GeneralRelation.hxx>
#include <NCollection_Sequence.hxx>
#include <Expr_GeneralRelation.hxx>
#include <Standard_Integer.hxx>
class Expr_GeneralExpression;
class Expr_NamedUnknown;
class TCollection_AsciiString;

class Expr_SystemRelation : public Expr_GeneralRelation
{

public:
  //! Creates a system with one relation
  Standard_EXPORT Expr_SystemRelation(const occ::handle<Expr_GeneralRelation>& relation);

  //! Appends <relation> in the list of components of <me>.
  Standard_EXPORT void Add(const occ::handle<Expr_GeneralRelation>& relation);

  Standard_EXPORT void Remove(const occ::handle<Expr_GeneralRelation>& relation);

  //! Tests if <me> is linear between its NamedUnknowns.
  Standard_EXPORT bool IsLinear() const override;

  //! Returns the number of relations contained in <me>.
  Standard_EXPORT int NbOfSubRelations() const override;

  //! Returns the number of SingleRelations contained in
  //! <me>.
  Standard_EXPORT int NbOfSingleRelations() const override;

  //! Returns the relation denoted by <index> in <me>.
  //! An exception is raised if <index> is out of range.
  Standard_EXPORT occ::handle<Expr_GeneralRelation> SubRelation(const int index) const
    override;

  Standard_EXPORT bool IsSatisfied() const override;

  //! Returns a GeneralRelation after replacement of
  //! NamedUnknowns by an associated expression, and after
  //! values computation.
  Standard_EXPORT occ::handle<Expr_GeneralRelation> Simplified() const override;

  //! Replaces NamedUnknowns by associated expressions,
  //! and computes values in <me>.
  Standard_EXPORT void Simplify() override;

  //! Returns a copy of <me> having the same unknowns and functions.
  Standard_EXPORT occ::handle<Expr_GeneralRelation> Copy() const override;

  //! Tests if <me> contains <exp>.
  Standard_EXPORT bool
    Contains(const occ::handle<Expr_GeneralExpression>& exp) const override;

  //! Replaces all occurrences of <var> with <with> in <me>.
  Standard_EXPORT void Replace(const occ::handle<Expr_NamedUnknown>&      var,
                               const occ::handle<Expr_GeneralExpression>& with) override;

  //! returns a string representing <me> in a readable way.
  Standard_EXPORT TCollection_AsciiString String() const override;

  DEFINE_STANDARD_RTTIEXT(Expr_SystemRelation, Expr_GeneralRelation)

private:
  NCollection_Sequence<occ::handle<Expr_GeneralRelation>> myRelations;
};

#endif // _Expr_SystemRelation_HeaderFile
