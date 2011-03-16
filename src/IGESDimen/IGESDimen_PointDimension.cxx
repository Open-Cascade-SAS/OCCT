//--------------------------------------------------------------------
//
//  File Name : IGESDimen_PointDimension.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_PointDimension.ixx>
#include <Interface_Macros.hxx>

    IGESDimen_PointDimension::IGESDimen_PointDimension ()    {  }


    void IGESDimen_PointDimension::Init
  (const Handle(IGESDimen_GeneralNote)& aNote,
   const Handle(IGESDimen_LeaderArrow)& anArrow,
   const Handle(IGESData_IGESEntity)&   aGeom)
{
  theNote   = aNote;
  theLeader = anArrow;
  theGeom   = aGeom;
  InitTypeAndForm(220,0);
}


    Handle(IGESDimen_GeneralNote) IGESDimen_PointDimension::Note () const
{
  return theNote;
}

    Handle(IGESDimen_LeaderArrow) IGESDimen_PointDimension::LeaderArrow () const
{
  return theLeader;
}

    Handle(IGESGeom_CircularArc) IGESDimen_PointDimension::CircularArc () const
{
  return GetCasted(IGESGeom_CircularArc, theGeom);
}

    Handle(IGESGeom_CompositeCurve) IGESDimen_PointDimension::CompositeCurve () const
{
  return GetCasted(IGESGeom_CompositeCurve, theGeom);
}

    Handle(IGESData_IGESEntity) IGESDimen_PointDimension::Geom () const
{
  return theGeom;
}

    Standard_Integer IGESDimen_PointDimension::GeomCase () const
{
  if (theGeom.IsNull())                     return 0;
  else if (theGeom->TypeNumber() == 100)    return 1;
  else if (theGeom->TypeNumber() == 102)    return 2;
  else                                      return 3;
}
