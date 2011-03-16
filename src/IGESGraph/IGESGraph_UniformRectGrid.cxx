//--------------------------------------------------------------------
//
//  File Name : IGESGraph_UniformRectGrid.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGraph_UniformRectGrid.ixx>


    IGESGraph_UniformRectGrid::IGESGraph_UniformRectGrid ()    {  }

    void IGESGraph_UniformRectGrid::Init
  (const Standard_Integer nbProps,
   const Standard_Integer finite,
   const Standard_Integer line,
   const Standard_Integer weighted,
   const gp_XY&           aGridPoint,
   const gp_XY&           aGridSpacing,
   const Standard_Integer pointsX,
   const Standard_Integer pointsY)
{
  theNbPropertyValues = nbProps;
  isItFinite          = finite;
  isItLine            = line;
  isItWeighted        = weighted;
  theGridPoint        = aGridPoint;
  theGridSpacing      = aGridSpacing;
  theNbPointsX        = pointsX;
  theNbPointsY        = pointsY;
  InitTypeAndForm(406,22);
}

    Standard_Integer IGESGraph_UniformRectGrid::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Boolean IGESGraph_UniformRectGrid::IsFinite () const
{
  return (isItFinite == 1);
}

    Standard_Boolean IGESGraph_UniformRectGrid::IsLine () const
{
  return (isItLine == 1);
}

    Standard_Boolean IGESGraph_UniformRectGrid::IsWeighted () const
{
  return (isItWeighted == 0);
}

    gp_Pnt2d IGESGraph_UniformRectGrid::GridPoint () const
{
  return ( gp_Pnt2d(theGridPoint) );
}

    gp_Vec2d IGESGraph_UniformRectGrid::GridSpacing () const
{
  return ( gp_Vec2d(theGridSpacing) );
}

    Standard_Integer IGESGraph_UniformRectGrid::NbPointsX () const
{
  return theNbPointsX;
}

    Standard_Integer IGESGraph_UniformRectGrid::NbPointsY () const
{
  return theNbPointsY;
}
