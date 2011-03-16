//--------------------------------------------------------------------
//
//  File Name : IGESGeom_Line.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_Line.ixx>
#include <gp_GTrsf.hxx>


    IGESGeom_Line::IGESGeom_Line ()    {  }


    void IGESGeom_Line::Init
  (const gp_XYZ& aStart, const gp_XYZ& anEnd)
{
  theStart = aStart;
  theEnd   = anEnd;
  InitTypeAndForm(110,0);
}

    Standard_Integer  IGESGeom_Line::Infinite () const
      {  return FormNumber();  }

    void  IGESGeom_Line::SetInfinite (const Standard_Integer status)
      {  if (status >= 0 && status <= 2) InitTypeAndForm(110,status);  }


    gp_Pnt IGESGeom_Line::StartPoint () const
{
  gp_Pnt start(theStart);
  return start;
}

    gp_Pnt IGESGeom_Line::TransformedStartPoint () const
{
  gp_XYZ Start = theStart;
  if (HasTransf()) Location().Transforms(Start);
  gp_Pnt transStart(Start);
  return transStart;
}

    gp_Pnt IGESGeom_Line::EndPoint () const
{
  gp_Pnt end(theEnd);
  return end;
}

    gp_Pnt IGESGeom_Line::TransformedEndPoint () const
{
  gp_XYZ End = theEnd;
  if (HasTransf()) Location().Transforms(End);
  gp_Pnt transEnd(End);
  return transEnd;
}
