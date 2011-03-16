//--------------------------------------------------------------------
//
//  File Name : IGESGeom_CircularArc.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_CircularArc.ixx>
#include <gp_Dir2d.hxx>
#include <gp_GTrsf.hxx>


    IGESGeom_CircularArc::IGESGeom_CircularArc ()    {  }

    void IGESGeom_CircularArc::Init 
  (const Standard_Real aZT, const gp_XY&  aCenter,
   const gp_XY& aStart,     const gp_XY&  anEnd) 
{
  theZT     = aZT;
  theCenter = aCenter;
  theStart  = aStart;
  theEnd    = anEnd;
  InitTypeAndForm(100,0);
}


    gp_Pnt2d IGESGeom_CircularArc::Center () const
{
  gp_Pnt2d Center(theCenter);
  return Center;
}

    gp_Pnt IGESGeom_CircularArc::TransformedCenter () const
{
  gp_XYZ Center(theCenter.X(), theCenter.Y(), theZT);
  if (HasTransf()) Location().Transforms(Center);
  gp_Pnt transCenter(Center);
  return transCenter;
}

    gp_Pnt2d IGESGeom_CircularArc::StartPoint () const
{
  gp_Pnt2d Start(theStart);
  return Start;
}

    gp_Pnt IGESGeom_CircularArc::TransformedStartPoint () const
{
  gp_XYZ Start(theStart.X(), theStart.Y(), theZT);
  if (HasTransf()) Location().Transforms(Start);
  gp_Pnt transStart(Start);
  return transStart;
}

    Standard_Real IGESGeom_CircularArc::ZPlane () const
{
  return theZT;
}

    gp_Pnt2d IGESGeom_CircularArc::EndPoint () const
{
  gp_Pnt2d End(theEnd);
  return End;
}

    gp_Pnt IGESGeom_CircularArc::TransformedEndPoint () const
{
  gp_XYZ End(theEnd.X(), theEnd.Y(), theZT);
  if (HasTransf()) Location().Transforms(End);
  gp_Pnt transEnd(End);
  return transEnd;
}

    Standard_Real IGESGeom_CircularArc::Radius () const
{
  Standard_Real x1, y1, x2, y2;
  x1 = theStart.X();
  y1 = theStart.Y();
  x2 = theCenter.X();
  y2 = theCenter.Y();

  Standard_Real radius = Sqrt(Square(x2 - x1) + Square(y2 - y1));
  return radius;
}

    Standard_Real IGESGeom_CircularArc::Angle () const
{
  Standard_Real x1, y1, x2, y2, xc, yc;
  xc = theCenter.X();
  yc = theCenter.Y();
  x1 = theStart.X();
  y1 = theStart.Y();
  x2 = theEnd.X();
  y2 = theEnd.Y();
  gp_Dir2d dir1(x1-xc, y1-yc);   // After shifting the centre of
  // arc to the origin
  gp_Dir2d dir2(x2-xc, y2-yc);   // After shifting the centre of
  // arc to the origin
  Standard_Real t = dir1.Angle(dir2);
  return t + (t > 0 ? 0 : 2*PI);
}

    gp_Dir IGESGeom_CircularArc::Axis () const
{
  gp_Dir axis(0.0 , 0.0, 1.0);
  return axis;
}

    gp_Dir IGESGeom_CircularArc::TransformedAxis () const
{
  gp_XYZ axis(0.0 , 0.0, 1.0);
  if (!HasTransf()) return gp_Dir(axis);
  gp_GTrsf loc = Location();
  loc.SetTranslationPart (gp_XYZ(0.,0.,0.));
  loc.Transforms(axis);
  return gp_Dir(axis);
}

    Standard_Boolean IGESGeom_CircularArc::IsClosed () const
{
  return ((theStart.X() == theEnd.X()) && (theStart.Y() == theEnd.Y()));
}
