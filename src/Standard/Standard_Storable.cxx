// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#include <Standard_Storable.ixx>

void Standard_Storable::Delete()
{}

//========================================================================
Standard_Integer Standard_Storable::HashCode(const Standard_Integer Upper)const
{
  return ::HashCode((Standard_Address) this, Upper, STANDARD_TYPE(Standard_Storable));
}

//========================================================================
Standard_Boolean Standard_Storable::IsEqual(const Standard_Storable& ) const
{
  return Standard_True;
}

//========================================================================
Standard_Boolean Standard_Storable::IsSimilar(const Standard_Storable& ) const
{
  return Standard_True;
}

//========================================================================
void Standard_Storable::ShallowDump(Standard_OStream& ) const
{
//  ::ShallowDump((Standard_Address) this, STANDARD_TYPE(Standard_Storable), S);
}

