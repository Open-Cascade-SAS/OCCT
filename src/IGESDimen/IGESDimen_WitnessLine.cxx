//--------------------------------------------------------------------
//
//  File Name : IGESDimen_WitnessLine.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_WitnessLine.ixx>
#include <Standard_DimensionMismatch.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <gp_GTrsf.hxx>


    IGESDimen_WitnessLine::IGESDimen_WitnessLine ()    {  }


    void  IGESDimen_WitnessLine::Init
  (const Standard_Integer dataType, const Standard_Real aDisp,
   const Handle(TColgp_HArray1OfXY)& dataPoints)
{
  if (dataPoints->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESDimen_WitnessLine : Init");
  theDatatype      = dataType;
  theZDisplacement = aDisp;
  theDataPoints    = dataPoints;
  InitTypeAndForm(106,40);
}


    Standard_Integer  IGESDimen_WitnessLine::Datatype () const 
{
  return theDatatype;
}

    Standard_Integer  IGESDimen_WitnessLine::NbPoints () const 
{
  return theDataPoints->Length();
}

    Standard_Real  IGESDimen_WitnessLine::ZDisplacement () const 
{
  return theZDisplacement;
}

    gp_Pnt IGESDimen_WitnessLine::Point (const Standard_Integer Index) const 
{
  gp_XY tempXY = theDataPoints->Value(Index);
  gp_Pnt point(tempXY.X(), tempXY.Y(), theZDisplacement);
  return point;
}

    gp_Pnt  IGESDimen_WitnessLine::TransformedPoint
  (const Standard_Integer Index) const 
{
  gp_XY point2d = theDataPoints->Value(Index);
  gp_XYZ point(point2d.X(), point2d.Y(), theZDisplacement);
  if (HasTransf()) Location().Transforms(point);
  return gp_Pnt(point);
}
