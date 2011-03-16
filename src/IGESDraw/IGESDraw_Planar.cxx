//--------------------------------------------------------------------
//
//  File Name : IGESDraw_Planar.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
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
