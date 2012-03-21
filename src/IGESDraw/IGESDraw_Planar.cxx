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

#include <IGESDraw_Planar.ixx>
#include <Standard_DimensionMismatch.hxx>


IGESDraw_Planar::IGESDraw_Planar ()    {  }


    void IGESDraw_Planar::Init
  (const Standard_Integer                        nbMats,
   const Handle(IGESGeom_TransformationMatrix)&  aTransformationMatrix,
   const Handle(IGESData_HArray1OfIGESEntity)&   allEntities)
{
  if (!allEntities.IsNull())
    if (allEntities->Lower() != 1)
      Standard_DimensionMismatch::Raise("IGESDraw_Planar : Init");
  theNbMatrices           = nbMats;
  theTransformationMatrix = aTransformationMatrix;
  theEntities             = allEntities;
  InitTypeAndForm(402,16);
}


    Standard_Integer IGESDraw_Planar::NbMatrices () const
{
  return theNbMatrices;
}

    Standard_Integer IGESDraw_Planar::NbEntities () const
{
  return ( theEntities.IsNull()? 0 : theEntities->Length() );
}

    Standard_Boolean IGESDraw_Planar::IsIdentityMatrix () const
{
  return ( theTransformationMatrix.IsNull() );
}

    Handle(IGESGeom_TransformationMatrix) IGESDraw_Planar::TransformMatrix () const
{
  return theTransformationMatrix;
}

    Handle(IGESData_IGESEntity) IGESDraw_Planar::Entity
  (const Standard_Integer EntityIndex) const
{
  return (theEntities->Value(EntityIndex));
}
