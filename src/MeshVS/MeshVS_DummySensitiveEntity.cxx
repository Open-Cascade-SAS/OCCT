// File:	MeshVS_DummySensitiveEntity.cxx
// Created:	Mon Sep 29 2003
// Author:	Alexander SOLOVYOV and Sergey LITONIN
// Copyright:	Open CASCADE 2003

#include <MeshVS_DummySensitiveEntity.ixx>

//================================================================
// Function : Constructor MeshVS_DummySensitiveEntity
// Purpose  :
//================================================================
MeshVS_DummySensitiveEntity::MeshVS_DummySensitiveEntity
  ( const Handle(SelectBasics_EntityOwner)& OwnerId )
: SelectBasics_SensitiveEntity( OwnerId )
{
}

//================================================================
// Function : Areas
// Purpose  :
//================================================================
void MeshVS_DummySensitiveEntity::Areas( SelectBasics_ListOfBox2d& )
{
}

//================================================================
// Function : Matches
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DummySensitiveEntity::Matches( const Standard_Real,
                                                       const Standard_Real,
                                                       const Standard_Real,
                                                       Standard_Real& )
{
  return Standard_False;
}

//================================================================
// Function : Matches
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DummySensitiveEntity::Matches( const Standard_Real,
                                                       const Standard_Real,
                                                       const Standard_Real,
                                                       const Standard_Real,
                                                       const Standard_Real )
{
  return Standard_False;
}

//================================================================
// Function : Matches
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DummySensitiveEntity::Matches( const TColgp_Array1OfPnt2d&,
                                                       const Bnd_Box2d&,
                                                       const Standard_Real )
{
  return Standard_False;
}

//================================================================
// Function : NeedsConversion
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DummySensitiveEntity::NeedsConversion() const
{
  return Standard_False;
}

//================================================================
// Function : Is3D
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DummySensitiveEntity::Is3D() const
{
  return Standard_True;
}

//================================================================
// Function : MaxBoxes
// Purpose  :
//================================================================
Standard_Integer MeshVS_DummySensitiveEntity::MaxBoxes() const
{
  return 0;
}
