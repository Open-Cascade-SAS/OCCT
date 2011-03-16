//--------------------------------------------------------------------
//
//  File Name : IGESGeom_OffsetSurface.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_OffsetSurface.ixx>
#include <gp_GTrsf.hxx>


    IGESGeom_OffsetSurface::IGESGeom_OffsetSurface ()     {  }


    void IGESGeom_OffsetSurface::Init
  (const gp_XYZ&                      anIndicator,
   const Standard_Real                aDistance,
   const Handle(IGESData_IGESEntity)& aSurface)
{
  theIndicator = anIndicator;
  theDistance  = aDistance;
  theSurface   = aSurface;
  InitTypeAndForm(140,0);
}

    gp_Vec IGESGeom_OffsetSurface::OffsetIndicator () const
{
  return gp_Vec(theIndicator);
}

    gp_Vec IGESGeom_OffsetSurface::TransformedOffsetIndicator () const
{
  if (!HasTransf()) return gp_Vec(theIndicator);
  gp_XYZ temp(theIndicator);
  gp_GTrsf loc = Location();
  loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
  loc.Transforms(temp);
  return gp_Vec(temp);
}

    Standard_Real IGESGeom_OffsetSurface::Distance () const
{
  return theDistance;
}

    Handle(IGESData_IGESEntity) IGESGeom_OffsetSurface::Surface () const
{
  return theSurface;
}
