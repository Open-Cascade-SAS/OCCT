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

#ifndef _Expr_SingleRelation_HeaderFile
#define _Expr_SingleRelation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Expr_GeneralRelation.hxx>
#include <Standard_Integer.hxx>
class Expr_GeneralExpression;
class Expr_NamedUnknown;

class Expr_SingleRelation;
DEFINE_STANDARD_HANDLE(Expr_SingleRelation, Expr_GeneralRelation)

class Expr_SingleRelation : public Expr_GeneralRelation
{

public:
  //! Defines the first member of the relation
  Standard_EXPORT void SetFirstMember(const Handle(Expr_GeneralExpression)& exp);

  //! Defines the second member of the relation
  Standard_EXPORT void SetSecondMember(const Handle(Expr_GeneralExpression)& exp);

  //! Returns the first member of the relation
  Standard_EXPORT Handle(Expr_GeneralExpression) FirstMember() const;

  //! Returns the second member of the relation
  Standard_EXPORT Handle(Expr_GeneralExpression) SecondMember() const;

  //! Tests if <me> is linear between its NamedUnknowns.
  Standard_EXPORT Standard_Boolean IsLinear() const Standard_OVERRIDE;

  //! Returns the number of relations contained in <me>.
  Standard_EXPORT Standard_Integer NbOfSubRelations() const Standard_OVERRIDE;

  //! Returns the number of SingleRelations contained in
  //! <me> (Always 1).
  Standard_EXPORT Standard_Integer NbOfSingleRelations() const Standard_OVERRIDE;

  //! Returns the relation denoted by <index> in <me>.
  //! An exception is raised if index is out of range.
  Standard_EXPORT Handle(Expr_GeneralRelation) SubRelation(const Standard_Integer index) const
    Standard_OVERRIDE;

  //! Tests if <me> contains <exp>.
  Standard_EXPORT Standard_Boolean
    Contains(const Handle(Expr_GeneralExpression)& exp) const Standard_OVERRIDE;

  //! Replaces all occurrences of <var> with <with> in <me>.
  Standard_EXPORT void Replace(const Handle(Expr_NamedUnknown)&      var,
                               const Handle(Expr_GeneralExpression)& with) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Expr_SingleRelation, Expr_GeneralRelation)

protected:
private:
  Handle(Expr_GeneralExpression) myFirstMember;
  Handle(Expr_GeneralExpression) mySecondMember;
};

#endif // _Expr_SingleRelation_HeaderFile
