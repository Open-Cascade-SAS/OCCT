//--------------------------------------------------------------------
//
//  File Name : IGESDimen_DiameterDimension.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_DiameterDimension.ixx>
#include <gp_XYZ.hxx>
#include <gp_GTrsf.hxx>


    IGESDimen_DiameterDimension::IGESDimen_DiameterDimension ()    {  }


    void  IGESDimen_DiameterDimension::Init
  (const Handle(IGESDimen_GeneralNote)& aNote,
   const Handle(IGESDimen_LeaderArrow)& aLeader,
   const Handle(IGESDimen_LeaderArrow)& anotherLeader,
   const gp_XY& aCenter)
{
  theNote         = aNote;
  theFirstLeader  = aLeader;
  theSecondLeader = anotherLeader;
  theCenter       = aCenter;
  InitTypeAndForm(206,0);
}

    Handle(IGESDimen_GeneralNote)  IGESDimen_DiameterDimension::Note () const 
{
  return theNote;
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_DiameterDimension::FirstLeader
  () const 
{
  return theFirstLeader;
}

    Standard_Boolean  IGESDimen_DiameterDimension::HasSecondLeader () const 
{
  return (! theSecondLeader.IsNull());
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_DiameterDimension::SecondLeader
  () const 
{
  return theSecondLeader;
}

    gp_Pnt2d  IGESDimen_DiameterDimension::Center () const 
{
  gp_Pnt2d center(theCenter);
  return center;
}

    gp_Pnt2d  IGESDimen_DiameterDimension::TransformedCenter () const 
{
  gp_XYZ center(theCenter.X(), theCenter.Y(), 0);
  if (HasTransf()) Location().Transforms(center);
  return gp_Pnt2d(center.X(), center.Y());
}
