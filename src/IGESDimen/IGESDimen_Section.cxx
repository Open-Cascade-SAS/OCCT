//--------------------------------------------------------------------
//
//  File Name : IGESDimen_Section.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_Section.ixx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <gp_GTrsf.hxx>


    IGESDimen_Section::IGESDimen_Section ()    {  }


    void  IGESDimen_Section::Init
  (const Standard_Integer dataType, const Standard_Real aDisp,
   const Handle(TColgp_HArray1OfXY)& dataPoints)
{
  if (dataPoints->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESDimen_Section : Init");
  theDatatype      = dataType;
  theZDisplacement = aDisp;
  theDataPoints    = dataPoints;
  InitTypeAndForm(106,FormNumber());
//  FormNumber  precises the type of Hatches  (31-38)
}


    void  IGESDimen_Section::SetFormNumber (const Standard_Integer form)
{
  if (form < 31 || form > 38) Standard_OutOfRange::Raise
    ("IGESDimen_Section : SetFormNumber");
  InitTypeAndForm(106,form);
}


    Standard_Integer  IGESDimen_Section::Datatype () const 
{
  return theDatatype;
}

    Standard_Integer  IGESDimen_Section::NbPoints () const 
{
  return theDataPoints->Length();
}

    Standard_Real  IGESDimen_Section::ZDisplacement () const 
{
  return theZDisplacement;
}

    gp_Pnt IGESDimen_Section::Point (const Standard_Integer Index) const 
{
  gp_XY tempXY = theDataPoints->Value(Index);
  gp_Pnt point(tempXY.X(), tempXY.Y(), theZDisplacement);
  return point;
}

    gp_Pnt  IGESDimen_Section::TransformedPoint (const Standard_Integer Index) const 
{
  gp_XY point2d = theDataPoints->Value(Index);
  gp_XYZ point(point2d.X(), point2d.Y(), theZDisplacement);
  if (HasTransf()) Location().Transforms(point);
  return gp_Pnt(point);
}
