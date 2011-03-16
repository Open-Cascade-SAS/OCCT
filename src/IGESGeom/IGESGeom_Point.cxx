//--------------------------------------------------------------------
//
//  File Name : IGESGeom_Point.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_Point.ixx>
#include <gp_GTrsf.hxx>


    IGESGeom_Point::IGESGeom_Point ()     {  }


    void IGESGeom_Point::Init
  (const gp_XYZ& aPoint, 
   const Handle(IGESBasic_SubfigureDef)& aSymbol)
{
  thePoint  = aPoint;
  theSymbol = aSymbol;
  InitTypeAndForm(116,0);
}

    gp_Pnt IGESGeom_Point::Value () const
{
  return gp_Pnt(thePoint);
}

    gp_Pnt IGESGeom_Point::TransformedValue () const
{
  gp_XYZ Val = thePoint;
  if (HasTransf()) Location().Transforms(Val);
  gp_Pnt transVal(Val);
  return transVal;
}

    Standard_Boolean IGESGeom_Point::HasDisplaySymbol () const
{
  return (!theSymbol.IsNull());
}

    Handle(IGESBasic_SubfigureDef) IGESGeom_Point::DisplaySymbol () const
{
  return theSymbol;
}
