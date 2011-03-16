//--------------------------------------------------------------------
//
//  File Name : IGESDraw_View.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDraw_View.ixx>
#include <gp_GTrsf.hxx>


    IGESDraw_View::IGESDraw_View ()    {  }


// This class inherits from IGESData_ViewKindEntity

    void IGESDraw_View::Init
  (const Standard_Integer        aViewNum,
   const Standard_Real           aScale,
   const Handle(IGESGeom_Plane)& aLeftPlane,
   const Handle(IGESGeom_Plane)& aTopPlane,
   const Handle(IGESGeom_Plane)& aRightPlane,
   const Handle(IGESGeom_Plane)& aBottomPlane,
   const Handle(IGESGeom_Plane)& aBackPlane,
   const Handle(IGESGeom_Plane)& aFrontPlane)
{
  theViewNumber  = aViewNum;
  theScaleFactor = aScale;
  theLeftPlane   = aLeftPlane;
  theTopPlane    = aTopPlane;
  theRightPlane  = aRightPlane;
  theBottomPlane = aBottomPlane;
  theBackPlane   = aBackPlane;
  theFrontPlane  = aFrontPlane;
  InitTypeAndForm(410,0);
}

    Standard_Boolean IGESDraw_View::IsSingle () const
{
  return Standard_True;
  // Redefined to return TRUE
}

    Standard_Integer IGESDraw_View::NbViews () const
{  return 1;  }

    Handle(IGESData_ViewKindEntity)  IGESDraw_View::ViewItem
  (const Standard_Integer) const
{  return Handle(IGESData_ViewKindEntity)::DownCast (This());  }


    Standard_Integer IGESDraw_View::ViewNumber () const
{
  return theViewNumber;
}

    Standard_Real IGESDraw_View::ScaleFactor () const
{
  return theScaleFactor;
}

    Standard_Boolean IGESDraw_View::HasLeftPlane () const
{
  return  (! theLeftPlane.IsNull());
}

    Handle(IGESGeom_Plane) IGESDraw_View::LeftPlane () const
{
  return  theLeftPlane;
}

    Standard_Boolean IGESDraw_View::HasTopPlane () const
{
  return  (! theTopPlane.IsNull());
}

    Handle(IGESGeom_Plane) IGESDraw_View::TopPlane () const
{
  return  theTopPlane;
}

    Standard_Boolean IGESDraw_View::HasRightPlane () const
{
  return  (! theRightPlane.IsNull());
}

    Handle(IGESGeom_Plane) IGESDraw_View::RightPlane () const
{
  return  theRightPlane;
}

    Standard_Boolean IGESDraw_View::HasBottomPlane () const
{
  return  (! theBottomPlane.IsNull());
}

    Handle(IGESGeom_Plane) IGESDraw_View::BottomPlane () const
{
  return  theBottomPlane;
}

    Standard_Boolean IGESDraw_View::HasBackPlane () const
{
  return  (! theBackPlane.IsNull());
}

    Handle(IGESGeom_Plane) IGESDraw_View::BackPlane () const
{
  return  theBackPlane;
}

    Standard_Boolean IGESDraw_View::HasFrontPlane () const
{
  return  (! theFrontPlane.IsNull());
}

    Handle(IGESGeom_Plane) IGESDraw_View::FrontPlane () const
{
  return  theFrontPlane;
}

    Handle(IGESData_TransfEntity) IGESDraw_View::ViewMatrix () const
{
  return (Transf());
}

    gp_XYZ IGESDraw_View::ModelToView (const gp_XYZ& coords) const
{
  gp_XYZ tempCoords = coords;
  Location().Transforms(tempCoords);
  return (tempCoords);
}
