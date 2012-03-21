// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESSolid_SolidAssembly.ixx>


IGESSolid_SolidAssembly::IGESSolid_SolidAssembly ()    {  }


    void  IGESSolid_SolidAssembly::Init
  (const Handle(IGESData_HArray1OfIGESEntity)& Items,
   const Handle(IGESGeom_HArray1OfTransformationMatrix)& Matrices)
{
  if (Items->Lower()  != 1 || Matrices->Lower() != 1 ||
      Items->Length() != Matrices->Length())
    Standard_DimensionError::Raise("IGESSolid_SolidAssembly : Init");

  theItems    = Items;
  theMatrices = Matrices;
  InitTypeAndForm(184,0);
}

    Standard_Boolean  IGESSolid_SolidAssembly::HasBrep () const
      {  return (FormNumber() == 1);  }

    void  IGESSolid_SolidAssembly::SetBrep (const Standard_Boolean hasbrep)
      {  InitTypeAndForm(184, (hasbrep ? 1 : 0));  }


    Standard_Integer  IGESSolid_SolidAssembly::NbItems () const
{
  return theItems->Length();
}

    Handle(IGESData_IGESEntity)  IGESSolid_SolidAssembly::Item
  (const Standard_Integer Index) const
{
  return theItems->Value(Index);
}

    Handle(IGESGeom_TransformationMatrix) IGESSolid_SolidAssembly::TransfMatrix
  (const Standard_Integer Index) const
{
  return theMatrices->Value(Index);
}
