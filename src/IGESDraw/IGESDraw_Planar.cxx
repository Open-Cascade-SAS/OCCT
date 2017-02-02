// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESData_IGESEntity.hxx>
#include <IGESDraw_Planar.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_Planar,IGESData_IGESEntity)

IGESDraw_Planar::IGESDraw_Planar ()    {  }


    void IGESDraw_Planar::Init
  (const Standard_Integer                        nbMats,
   const Handle(IGESGeom_TransformationMatrix)&  aTransformationMatrix,
   const Handle(IGESData_HArray1OfIGESEntity)&   allEntities)
{
  if (!allEntities.IsNull())
    if (allEntities->Lower() != 1)
      throw Standard_DimensionMismatch("IGESDraw_Planar : Init");
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
