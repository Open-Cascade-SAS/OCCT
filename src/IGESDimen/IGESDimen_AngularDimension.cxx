//--------------------------------------------------------------------
//
//  File Name : IGESDimen_AngularDimension.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_AngularDimension.ixx>
#include <gp_XYZ.hxx>
#include <gp_GTrsf.hxx>


    IGESDimen_AngularDimension::IGESDimen_AngularDimension ()    {  }

    void  IGESDimen_AngularDimension::Init
  (const Handle(IGESDimen_GeneralNote)& aNote, 
   const Handle(IGESDimen_WitnessLine)& aLine,
   const Handle(IGESDimen_WitnessLine)& anotherLine,
   const gp_XY& aVertex, const Standard_Real aRadius,
   const Handle(IGESDimen_LeaderArrow)& aLeader,
   const Handle(IGESDimen_LeaderArrow)& anotherLeader)
{
  theNote              = aNote;
  theFirstWitnessLine  = aLine;
  theSecondWitnessLine = anotherLine;
  theVertex            = aVertex;
  theRadius            = aRadius;
  theFirstLeader       = aLeader;
  theSecondLeader      = anotherLeader;
  InitTypeAndForm(202,0);
}

    Handle(IGESDimen_GeneralNote)  IGESDimen_AngularDimension::Note () const 
{
  return theNote;
}

    Standard_Boolean  IGESDimen_AngularDimension::HasFirstWitnessLine () const 
{
  return (! theFirstWitnessLine.IsNull());
}

    Handle(IGESDimen_WitnessLine)  IGESDimen_AngularDimension::FirstWitnessLine
  () const 
{
  return theFirstWitnessLine;
}

    Standard_Boolean  IGESDimen_AngularDimension::HasSecondWitnessLine () const 
{
  return (! theSecondWitnessLine.IsNull());
}

    Handle(IGESDimen_WitnessLine)  IGESDimen_AngularDimension::SecondWitnessLine
  () const 
{
  return theSecondWitnessLine;
}

    gp_Pnt2d  IGESDimen_AngularDimension::Vertex () const 
{
  gp_Pnt2d vertex(theVertex);
  return vertex;
}

    gp_Pnt2d  IGESDimen_AngularDimension::TransformedVertex () const 
{
  gp_XYZ point(theVertex.X(), theVertex.Y(), 0.0);
  if (HasTransf()) Location().Transforms(point);
  return gp_Pnt2d(point.X(), point.Y());
}

    Standard_Real  IGESDimen_AngularDimension::Radius () const 
{
  return theRadius;
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_AngularDimension::FirstLeader () const 
{
  return theFirstLeader;
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_AngularDimension::SecondLeader () const 
{
  return theSecondLeader;
}
