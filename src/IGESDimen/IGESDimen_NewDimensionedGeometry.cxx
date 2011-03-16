//--------------------------------------------------------------------
//
//  File Name : IGESDimen_NewDimensionedGeometry.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_NewDimensionedGeometry.ixx>
#include <IGESData_TransfEntity.hxx>
#include <gp_GTrsf.hxx>


    IGESDimen_NewDimensionedGeometry::IGESDimen_NewDimensionedGeometry ()   { }


    void  IGESDimen_NewDimensionedGeometry::Init
  (const Standard_Integer nbDimens,
   const Handle(IGESData_IGESEntity)& aDimen,
   const Standard_Integer anOrientation, const Standard_Real anAngle,
   const Handle(IGESData_HArray1OfIGESEntity)& allEntities,
   const Handle(TColStd_HArray1OfInteger)& allLocations,
   const Handle(TColgp_HArray1OfXYZ)& allPoints)
{
  Standard_Integer num = allEntities->Length();
  if (allEntities->Lower()  != 1 ||
      allLocations->Lower() != 1 || allLocations->Length() != num ||
      allPoints->Lower()    != 1 || allPoints->Length()    != num )
    Standard_DimensionMismatch::Raise
      ("IGESDimen_NewDimensionedGeometry: Init");
  theNbDimensions             = nbDimens;
  theDimensionEntity          = aDimen;
  theDimensionOrientationFlag = anOrientation;
  theAngleValue               = anAngle;
  theGeometryEntities         = allEntities;
  theDimensionLocationFlags   = allLocations;
  thePoints                   = allPoints;
  InitTypeAndForm(402,21);
}


    Standard_Integer  IGESDimen_NewDimensionedGeometry::NbDimensions () const
{
  return theNbDimensions;
}

    Standard_Integer  IGESDimen_NewDimensionedGeometry::NbGeometries () const
{
  return theGeometryEntities->Length();
}

    Handle(IGESData_IGESEntity)  IGESDimen_NewDimensionedGeometry::DimensionEntity
  ()const
{
  return theDimensionEntity;
}

    Standard_Integer  IGESDimen_NewDimensionedGeometry::DimensionOrientationFlag
  () const
{
  return theDimensionOrientationFlag;
}

    Standard_Real  IGESDimen_NewDimensionedGeometry::AngleValue () const
{
  return theAngleValue;
}

    Handle(IGESData_IGESEntity)  IGESDimen_NewDimensionedGeometry::GeometryEntity
  (const Standard_Integer Index) const
{
  return theGeometryEntities->Value(Index);
}

    Standard_Integer  IGESDimen_NewDimensionedGeometry::DimensionLocationFlag
  (const Standard_Integer Index) const
{
  return theDimensionLocationFlags->Value(Index);
}

    gp_Pnt  IGESDimen_NewDimensionedGeometry::Point
  (const Standard_Integer Index) const
{
  return gp_Pnt(thePoints->Value(Index));
}

    gp_Pnt  IGESDimen_NewDimensionedGeometry::TransformedPoint
  (const Standard_Integer Index) const
{
  gp_XYZ point = thePoints->Value(Index);
  if (HasTransf()) Location().Transforms(point);
  return gp_Pnt(point);
}
