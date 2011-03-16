//static const char* sccsid = "@(#)Expr_SystemRelation.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_SystemRelation.cxx
// Created:	Thu Jun 13 14:01:02 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_SystemRelation.ixx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_DimensionMismatch.hxx>

Expr_SystemRelation::Expr_SystemRelation (const Handle(Expr_GeneralRelation)& relation)
{
  myRelations.Append(relation);
}

void Expr_SystemRelation::Add (const Handle(Expr_GeneralRelation)& relation)
{
  myRelations.Append(relation);
}

void Expr_SystemRelation::Remove (const Handle(Expr_GeneralRelation)& relation)
{
  Standard_Integer position    = 0;
  Standard_Boolean alreadyHere = Standard_False;

  for (Standard_Integer i = 1; i <=  myRelations.Length() && !alreadyHere; i++) {
    if (myRelations.Value(i) == relation) {
      alreadyHere = Standard_True;
      position    = i;
    }
  }

  if (alreadyHere) {
    Standard_NoSuchObject::Raise();
  }
  if (myRelations.Length() <= 1) {
    Standard_DimensionMismatch::Raise();
  }
  myRelations.Remove(position);
}

Standard_Boolean Expr_SystemRelation::IsLinear () const
{
  Standard_Integer len = myRelations.Length();
  for (Standard_Integer i = 1; i<= len; i++) {
    if (!myRelations(i)->IsLinear()) {
      return Standard_False;
    }
  }
  return Standard_True;
}

Standard_Boolean Expr_SystemRelation::Contains(const Handle(Expr_GeneralExpression)& exp) const
{
  for (Standard_Integer i=1; i<= myRelations.Length(); i++) {
    if (myRelations(i)->Contains(exp)) {
      return Standard_True;
    }
  }
  return Standard_False;
}

void Expr_SystemRelation::Replace(const Handle(Expr_NamedUnknown)& var, const Handle(Expr_GeneralExpression)& with)
{
  for (Standard_Integer i=1; i<= myRelations.Length(); i++) {
    myRelations(i)->Replace(var,with);
  }
}

Standard_Integer Expr_SystemRelation::NbOfSubRelations () const
{
  return myRelations.Length();
}

Handle(Expr_GeneralRelation) Expr_SystemRelation::SubRelation (const Standard_Integer index) const
{
  return myRelations(index);
}

Standard_Boolean Expr_SystemRelation::IsSatisfied () const
{
  Standard_Integer len = myRelations.Length();
  for (Standard_Integer i = 1; i<= len; i++) {
    if (!myRelations(i)->IsSatisfied()) {
      return Standard_False;
    }
  }
  return Standard_True;
}


Handle(Expr_GeneralRelation) Expr_SystemRelation::Simplified () const
{
  Standard_Integer len = myRelations.Length();
  Handle(Expr_GeneralRelation) rel;
  rel = myRelations(1);
  Handle(Expr_SystemRelation) result = new Expr_SystemRelation(rel->Simplified());
  for (Standard_Integer i = 2; i<= len; i++) {
    rel = myRelations(i);
    rel = rel->Simplified();
    result->Add(rel);
  }
  return result;
}
  

  
void Expr_SystemRelation::Simplify ()
{
  Standard_Integer len = myRelations.Length();
  Handle(Expr_GeneralRelation) rel;
  for (Standard_Integer i = 1; i<= len; i++) {
    rel = myRelations(i);
    rel->Simplify();
  }
}

Handle(Expr_GeneralRelation) Expr_SystemRelation::Copy () const
{
  Handle(Expr_SystemRelation) cop = new Expr_SystemRelation(myRelations(1)->Copy());
  Standard_Integer len = myRelations.Length();
  for (Standard_Integer i = 2; i<= len; i++) {
    cop->Add(myRelations(i)->Copy());
  }
  return cop;
}

Standard_Integer Expr_SystemRelation::NbOfSingleRelations() const
{
  Standard_Integer nbsing = 0;
  Standard_Integer nbrel = myRelations.Length();
  Handle(Expr_GeneralRelation) subrel;
  for (Standard_Integer i = 1; i<= nbrel ; i++) {
    subrel = myRelations(i);
    nbsing = nbsing + subrel->NbOfSingleRelations();
  }
  return nbsing;
}

TCollection_AsciiString Expr_SystemRelation::String() const
{
  Standard_Integer nbrel = myRelations.Length();
  Handle(Expr_GeneralRelation) subrel;
  TCollection_AsciiString res;
  for (Standard_Integer i = 1; i<= nbrel ; i++) {
    res += myRelations(i)->String();
    if (i != nbrel) {
      res += TCollection_AsciiString('\n');
    }
  }
  return res;
}
