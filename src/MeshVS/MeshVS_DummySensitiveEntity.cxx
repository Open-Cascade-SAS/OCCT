// Created on: 2003-09-29
// Created by: Alexander SOLOVYOV and Sergey LITONIN
// Copyright (c) 2003-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
