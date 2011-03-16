//--------------------------------------------------------------------
//
//  File Name : IGESGeom_TabulatedCylinder.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_TabulatedCylinder.ixx>
#include <gp_GTrsf.hxx>


    IGESGeom_TabulatedCylinder::IGESGeom_TabulatedCylinder ()    {  }


    void IGESGeom_TabulatedCylinder::Init
  (const Handle(IGESData_IGESEntity)& aDirectrix,
   const gp_XYZ&                      anEnd)
{
  theDirectrix = aDirectrix;
  theEnd       = anEnd;
  InitTypeAndForm(122,0);
}

    Handle(IGESData_IGESEntity) IGESGeom_TabulatedCylinder::Directrix () const 
{
  return theDirectrix;
}

    gp_Pnt IGESGeom_TabulatedCylinder::EndPoint () const
{
  return ( gp_Pnt(theEnd) );
}

    gp_Pnt IGESGeom_TabulatedCylinder::TransformedEndPoint () const
{
  gp_XYZ EndPoint = theEnd;
  if (HasTransf()) Location().Transforms(EndPoint);
  return ( gp_Pnt(EndPoint) );
}
