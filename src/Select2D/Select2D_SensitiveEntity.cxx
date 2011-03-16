// Copyright: 	Matra-Datavision 1995
// File:	Select2D_SensitiveEntity.cxx
// Created:	Fri Mar 10 14:36:48 1995
// Author:	Mister rmi
//		<rmi>



#include <Select2D_SensitiveEntity.ixx>

Select2D_SensitiveEntity::
Select2D_SensitiveEntity(const Handle(SelectBasics_EntityOwner)& anOwner):
SelectBasics_SensitiveEntity(anOwner),myOwnTolerance(-1.)
{}


void Select2D_SensitiveEntity::Convert(const Handle(Select2D_Projector)& aProj)
{
   Standard_NotImplemented::Raise("cannot convert the 2D sensitiveEntity");
}

Standard_Boolean Select2D_SensitiveEntity::Is3D() const
{return Standard_False;}


Standard_Boolean Select2D_SensitiveEntity::Matches(const TColgp_Array1OfPnt2d& aPoly,
						   const Bnd_Box2d& aBox,
						   const Standard_Real aTol)
{
  return Standard_False;
}
