//--------------------------------------------------------------------
//
//  File Name : IGESDimen_DimensionedGeometry.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_DimensionedGeometry.ixx>
#include <Standard_DimensionMismatch.hxx>


    IGESDimen_DimensionedGeometry::IGESDimen_DimensionedGeometry ()    {  }


    void  IGESDimen_DimensionedGeometry::Init
  (const Standard_Integer nbDims, 
   const Handle(IGESData_IGESEntity)& aDimension, 
   const Handle(IGESData_HArray1OfIGESEntity)& entities)
{
  if (entities->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESDimen_DimensionedGeometry : Init");
  theNbDimensions     = nbDims;
  theDimension        = aDimension;
  theGeometryEntities = entities;
  InitTypeAndForm(402,13);
}


    Standard_Integer  IGESDimen_DimensionedGeometry::NbDimensions () const 
{
  return theNbDimensions;
}

    Standard_Integer  IGESDimen_DimensionedGeometry::NbGeometryEntities () const 
{
  return theGeometryEntities->Length();
}

    Handle(IGESData_IGESEntity)  IGESDimen_DimensionedGeometry::DimensionEntity
  () const
{
  return theDimension;
}

    Handle(IGESData_IGESEntity)  IGESDimen_DimensionedGeometry::GeometryEntity
  (const Standard_Integer num) const 
{
  return theGeometryEntities->Value(num);
}

