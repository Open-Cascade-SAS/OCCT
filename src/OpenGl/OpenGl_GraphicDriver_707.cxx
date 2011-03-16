
// File   OpenGl_GraphicDriver_707.cxx
// Created  Mardi 28 janvier 1997
// Author CAL

//-Copyright  MatraDatavision 1997

//-Version  

//-Design Declaration des variables specifiques aux Drivers

//-Warning  Un driver encapsule les Pex et OpenGl drivers

//-References 

//-Language C++ 2.0

//-Declarations

// for the class
#include <OpenGl_GraphicDriver.jxx>

#include <Aspect_DriverDefinitionError.hxx>

#include <OpenGl_tgl_funcs.hxx>

//-Aliases

//-Global data definitions

//-Methods, in order

void OpenGl_GraphicDriver::Polyline (const Graphic3d_CGroup& ACGroup,
                                     const Graphic3d_Array1OfVertex& ListVertex,
                                     const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Integer i;
  Standard_Integer Lower  = ListVertex.Lower ();

  CALL_DEF_LISTPOINTS alpoints;

  i = ListVertex.Length ();

  alpoints.NbPoints = int (i);
  alpoints.TypePoints = 1;
  alpoints.UPoints.Points = (CALL_DEF_POINT *) &ListVertex (Lower);

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polyline");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polyline (&MyCGroup, &alpoints);

}

void OpenGl_GraphicDriver::Polyline (const Graphic3d_CGroup& ACGroup,
                                     const Graphic3d_Array1OfVertexC& ListVertex,
                                     const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;


  Standard_Real X, Y, Z;
  Standard_Real R, G, B;

  Standard_Integer i, j;
  Standard_Integer Lower  = ListVertex.Lower ();
  Standard_Integer Upper  = ListVertex.Upper ();

  CALL_DEF_LISTPOINTS alpoints;
  CALL_DEF_POINTC *points;

  i = ListVertex.Length ();

  // Allocation dynamique
  points  = new CALL_DEF_POINTC [i];

  alpoints.NbPoints = int (i);
  alpoints.TypePoints = 3;
  alpoints.UPoints.PointsC= points;

  Lower = ListVertex.Lower ();
  Upper = ListVertex.Upper ();

  // Parcours des sommets
  for (j=0, i=Lower; i<=Upper; i++, j++) {
    ListVertex (i).Coord (X, Y, Z);
    (ListVertex (i).Color ()).Values (R, G, B, Quantity_TOC_RGB);
    points[j].Color.r = float (R);
    points[j].Color.g = float (G);
    points[j].Color.b = float (B);
    points[j].Point.x = float (X);
    points[j].Point.y = float (Y);
    points[j].Point.z = float (Z);
  }

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polyline");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polyline (&MyCGroup, &alpoints);

  // Desallocation dynamique
  delete [] points;

}

void OpenGl_GraphicDriver::Polyline (const Graphic3d_CGroup& ACGroup,
                                     const Standard_Real X1,
                                     const Standard_Real Y1,
                                     const Standard_Real Z1,
                                     const Standard_Real X2,
                                     const Standard_Real Y2,
                                     const Standard_Real Z2,
                                     const Standard_Boolean )
{

  Graphic3d_CGroup TheGroup = ACGroup;

  CALL_DEF_LISTPOINTS alpoints;

  CALL_DEF_POINT *points;

  // Allocation dynamique
  points  = new CALL_DEF_POINT [2];

  alpoints.NbPoints = 2;
  alpoints.TypePoints = 1;
  alpoints.UPoints.Points = points;

  points[0].x = float (X1);
  points[0].y = float (Y1);
  points[0].z = float (Z1);
  points[1].x = float (X2);
  points[1].y = float (Y2);
  points[1].z = float (Z2);

  call_togl_polyline (&TheGroup, &alpoints);

  // Desallocation dynamique
  delete [] points;

}
