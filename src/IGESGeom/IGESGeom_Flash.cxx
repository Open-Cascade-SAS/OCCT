//--------------------------------------------------------------------
//
//  File Name : IGESGeom_Flash.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_Flash.ixx>
#include <Standard_OutOfRange.hxx>
#include <gp_GTrsf.hxx>


    IGESGeom_Flash::IGESGeom_Flash ()     {  }


    void IGESGeom_Flash::Init
  (const gp_XY&                       aPoint,
   const Standard_Real                aDim1,
   const Standard_Real                aDim2,
   const Standard_Real                aRotation,
   const Handle(IGESData_IGESEntity)& aReference)
{
  thePoint     = aPoint;
  theDim1      = aDim1;
  theDim2      = aDim2;
  theRotation  = aRotation;
  theReference = aReference;
  InitTypeAndForm(125,FormNumber());
// FormNumber : 0-4, Shape of the Flash
}

    void  IGESGeom_Flash::SetFormNumber (const Standard_Integer form)
{
  if (form < 0 || form > 4) Standard_OutOfRange::Raise
    ("IGESGeom_Flash : SetFormNumber");
  InitTypeAndForm(125,form);
}


    gp_Pnt2d IGESGeom_Flash::ReferencePoint () const
{
  return ( gp_Pnt2d(thePoint) );
}

    gp_Pnt IGESGeom_Flash::TransformedReferencePoint () const
{
  gp_XYZ Point(thePoint.X(), thePoint.Y(), 0.0);
  if (HasTransf()) Location().Transforms(Point);
  return gp_Pnt(Point);
}

    Standard_Real IGESGeom_Flash::Dimension1 () const
{
  return theDim1;
}

    Standard_Real IGESGeom_Flash::Dimension2 () const
{
  return theDim2;
}

    Standard_Real IGESGeom_Flash::Rotation () const
{
  return theRotation;
}

    Standard_Boolean IGESGeom_Flash::HasReferenceEntity () const
{
  return (! theReference.IsNull() );
}

    Handle(IGESData_IGESEntity) IGESGeom_Flash::ReferenceEntity () const
{
  return theReference;
}
