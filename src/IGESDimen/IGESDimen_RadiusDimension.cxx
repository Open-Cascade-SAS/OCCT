//--------------------------------------------------------------------
//
//  File Name : IGESDimen_RadiusDimension.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_RadiusDimension.ixx>
#include <gp_GTrsf.hxx>


    IGESDimen_RadiusDimension::IGESDimen_RadiusDimension ()    {  }


    void IGESDimen_RadiusDimension::Init
  (const Handle(IGESDimen_GeneralNote)& aNote,
   const Handle(IGESDimen_LeaderArrow)& anArrow,
   const gp_XY& arcCenter,
   const Handle(IGESDimen_LeaderArrow)& anotherArrow)
{
  theNote        = aNote;
  theLeaderArrow = anArrow;
  theCenter      = arcCenter;
  theLeader2     = anotherArrow;
  if (!anotherArrow.IsNull()) InitTypeAndForm(222, 1);  // 1 admet aussi Null
  else InitTypeAndForm(222,FormNumber());
}

    void IGESDimen_RadiusDimension::InitForm (const Standard_Integer form)
{
  InitTypeAndForm(222,form);
}

    Handle(IGESDimen_GeneralNote) IGESDimen_RadiusDimension::Note () const
{
  return theNote;
}

    Handle(IGESDimen_LeaderArrow) IGESDimen_RadiusDimension::Leader () const
{
  return theLeaderArrow;
}

    Standard_Boolean IGESDimen_RadiusDimension::HasLeader2 () const
{
  return (!theLeader2.IsNull());
}

    gp_Pnt2d IGESDimen_RadiusDimension::Center () const
{
  gp_Pnt2d g(theCenter);
  return g;
}

    gp_Pnt IGESDimen_RadiusDimension::TransformedCenter () const
{
  gp_XYZ tmpXYZ(theCenter.X(), theCenter.Y(), theLeaderArrow->ZDepth());
  if (HasTransf()) Location().Transforms(tmpXYZ);
  return gp_Pnt(tmpXYZ);
}

    Handle(IGESDimen_LeaderArrow) IGESDimen_RadiusDimension::Leader2 () const
{
  return theLeader2;
}
