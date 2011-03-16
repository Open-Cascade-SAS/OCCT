//--------------------------------------------------------------------
//
//  File Name : IGESGeom_CompositeCurve.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_CompositeCurve.ixx>
#include <Standard_DimensionMismatch.hxx>


    IGESGeom_CompositeCurve::IGESGeom_CompositeCurve ()    {  }


    void  IGESGeom_CompositeCurve::Init
  (const Handle(IGESData_HArray1OfIGESEntity)& allEntities)
{
  if (!allEntities.IsNull() && allEntities->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESGeom_CompositeCurve : Init");
  theEntities = allEntities;
  InitTypeAndForm(102,0);
}

    Standard_Integer  IGESGeom_CompositeCurve::NbCurves () const
{
  return (theEntities.IsNull() ? 0 : theEntities->Length());
}

    Handle(IGESData_IGESEntity)  IGESGeom_CompositeCurve::Curve
  (const Standard_Integer Index) const
{
  return theEntities->Value(Index);
}
