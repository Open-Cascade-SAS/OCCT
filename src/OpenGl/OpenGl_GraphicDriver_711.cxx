// File   OpenGl_GraphicDriver_711.cxx
// Created  Mardi 28 janvier 1997
// Author CAL
//
// Modified
//              27/08/97 ; PCT : ajout coordonne texture

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


#include <OSD_Timer.hxx>

#include <OpenGl_tgl_funcs.hxx>

//-Aliases

//-Global data definitions

//-Methods, in order

void OpenGl_GraphicDriver::TriangleMesh (const Graphic3d_CGroup& ACGroup,
                                         const Graphic3d_Array1OfVertex& ListVertex,
                                         const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Integer Lower  = ListVertex.Lower ();

  CALL_DEF_TRIKE atrike;

  atrike.NbPoints   = int (ListVertex.Length ());
  atrike.TypePoints = 1;
  atrike.UPoints.Points = (CALL_DEF_POINT *) &ListVertex (Lower);

  if (MyTraceLevel) {
    PrintFunction ("call_togl_triangle");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_triangle (&MyCGroup, &atrike);

}

void OpenGl_GraphicDriver::TriangleMesh (const Graphic3d_CGroup& ACGroup,
                                         const Graphic3d_Array1OfVertexN& ListVertex,
                                         const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Real X, Y, Z;
  Standard_Real DX, DY, DZ;

  Standard_Integer i, j;
  Standard_Integer Lower  = ListVertex.Lower ();
  Standard_Integer Upper  = ListVertex.Upper ();

  CALL_DEF_TRIKE atrike;
  CALL_DEF_POINTN *points;

  i = ListVertex.Length ();

  // Allocation dynamique
  points  = new CALL_DEF_POINTN [i];

  atrike.NbPoints   = int (i);
  atrike.TypePoints = 2;
  atrike.UPoints.PointsN  = points;

  // Parcours des sommets
  for (j=0, i=Lower; i<=Upper; i++, j++) {
    ListVertex (i).Coord (X, Y, Z);
    points[j].Point.x = float (X);
    points[j].Point.y = float (Y);
    points[j].Point.z = float (Z);
    ListVertex (i).Normal (DX, DY, DZ);
    points[j].Normal.dx = float (DX);
    points[j].Normal.dy = float (DY);
    points[j].Normal.dz = float (DZ);
  }

  if (MyTraceLevel) {
    PrintFunction ("call_togl_triangle");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_triangle (&MyCGroup, &atrike);

  // Desallocation dynamique
  delete [] points;

}


void OpenGl_GraphicDriver::TriangleMesh(const Graphic3d_CGroup& ACGroup,
                                        const Graphic3d_Array1OfVertexNT& ListVertex,
                                        const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Real X, Y, Z;
  Standard_Real DX, DY, DZ;

  Standard_Integer i, j;
  Standard_Integer Lower  = ListVertex.Lower ();
  Standard_Integer Upper  = ListVertex.Upper ();

  CALL_DEF_TRIKE atrike;
  CALL_DEF_POINTNT *points;

  i = ListVertex.Length ();

  // Allocation dynamique
  points  = new CALL_DEF_POINTNT [i];

  atrike.NbPoints   = int (i);
  atrike.TypePoints = 5;
  atrike.UPoints.PointsNT = points;

  // Parcours des sommets
  for (j=0, i=Lower; i<=Upper; i++, j++) {
    ListVertex (i).Coord (X, Y, Z);
    points[j].Point.x = float (X);
    points[j].Point.y = float (Y);
    points[j].Point.z = float (Z);
    ListVertex (i).Normal (DX, DY, DZ);
    points[j].Normal.dx = float (DX);
    points[j].Normal.dy = float (DY);
    points[j].Normal.dz = float (DZ);
    ListVertex (i).TextureCoordinate(DX,DY);
    points[j].TextureCoord.tx = float(DX);
    points[j].TextureCoord.ty = float(DY);
  }

  if (MyTraceLevel) {
    PrintFunction ("call_togl_triangle");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_triangle (&MyCGroup, &atrike);

  // Desallocation dynamique
  delete [] points;

}

void OpenGl_GraphicDriver::TriangleSet (const Graphic3d_CGroup& ACGroup,
                                        const Graphic3d_Array1OfVertex& ListVertex,
                                        const Aspect_Array1OfEdge& ListEdge,
                                        const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  OSD_Timer theTimer;
  Standard_Real seconds, CPUtime;
  Standard_Integer minutes, hours;

  Standard_Integer i, j, k;
  Standard_Integer Lower, Upper;
  Standard_Integer OffSet;

  CALL_DEF_LISTPOINTS alpoints;
  CALL_DEF_LISTEDGES aledges;
  CALL_DEF_EDGE *edges;

  // Permettra une generalisation plus facile a une methode du style
  // PolygonSet (ListVertex, ListEdge, Bounds).
  int *integers;
  CALL_DEF_LISTINTEGERS albounds;

  theTimer.Reset();theTimer.Start();

  i = ListVertex.Length ();
  j = ListEdge.Length ();
  k = int (j/3);

  Lower = ListVertex.Lower ();
  Upper = ListVertex.Upper ();
  OffSet  = Lower;

  // Allocation dynamique
  edges = new CALL_DEF_EDGE [j];
  integers= new int [k];

#ifdef a
  alpoints.NbPoints = int (i);
#else
  alpoints.NbPoints = i;
#endif
  alpoints.TypePoints = 1;
  alpoints.UPoints.Points = (CALL_DEF_POINT *) &ListVertex (Lower);

#ifdef a
  aledges.NbEdges   = int (j);
#else
  aledges.NbEdges   = j;
#endif
  aledges.Edges   = edges;

#ifdef a
  albounds.NbIntegers = int (k);
#else
  albounds.NbIntegers = k;
#endif
  albounds.Integers = integers;

  Lower = ListEdge.Lower ();
  Upper = ListEdge.Upper ();

  // Parcours des aretes
  for (j=0, i=Lower; i<=Upper; i++, j++) {
#ifdef a
    edges[j].Index1 = int (ListEdge (i).FirstIndex ()-OffSet);
    edges[j].Index2 = int (ListEdge (i).LastIndex ()-OffSet);
    edges[j].Type = int (ListEdge (i).Type ());
#else
    edges[j].Index1 = ListEdge (i).FirstIndex ()-OffSet;
    edges[j].Index2 = ListEdge (i).LastIndex ()-OffSet;
    edges[j].Type = ListEdge (i).Type ();
#endif
  }

  // Parcours des limites (ici toutes les limites sont egales a 3.
  for (j=0; j<albounds.NbIntegers; j++) integers[j] = 3;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polygon_indices");
    PrintCGroup (MyCGroup, 1);
  }


  theTimer.Stop();
  theTimer.Show(seconds, minutes, hours, CPUtime);
#ifdef TRACE
  cout<<"opengl_driver general stuff in : sec: "<<seconds<< "; min: " << minutes <<endl;
#endif

  theTimer.Reset();theTimer.Start();
  call_togl_polygon_indices (&MyCGroup, &alpoints, &aledges, &albounds);
  theTimer.Stop();
  theTimer.Show(seconds, minutes, hours, CPUtime);
#ifdef TRACE
  cout<<"call_togl_polygon_indices in : sec: "<<seconds<< "; min: " << minutes <<endl;
#endif
  // Desallocation dynamique
  delete [] edges;
  delete [] integers;

}

void OpenGl_GraphicDriver::TriangleSet (const Graphic3d_CGroup& ACGroup,
                                        const Graphic3d_Array1OfVertexN& ListVertex,
                                        const Aspect_Array1OfEdge& ListEdge,
                                        const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Real X, Y, Z;
  Standard_Real DX, DY, DZ;

  Standard_Integer i, j, k;
  Standard_Integer Lower, Upper;
  Standard_Integer OffSet;

  CALL_DEF_LISTPOINTS alpoints;
  CALL_DEF_POINTN *points;
  CALL_DEF_LISTEDGES aledges;
  CALL_DEF_EDGE *edges;

  // Permettra une generalisation plus facile a une methode du style
  // PolygonSet (ListVertex, ListEdge, Bounds).
  int *integers;
  CALL_DEF_LISTINTEGERS albounds;

  i = ListVertex.Length ();
  j = ListEdge.Length ();
  k = int (j/3);

  Lower = ListVertex.Lower ();
  Upper = ListVertex.Upper ();
  OffSet  = Lower;

  // Allocation dynamique
  points  = new CALL_DEF_POINTN [i];
  edges = new CALL_DEF_EDGE [j];
  integers= new int [k];

  alpoints.NbPoints = int (i);
  alpoints.TypePoints = 2;
  alpoints.UPoints.PointsN= points;

  aledges.NbEdges   = int (j);
  aledges.Edges   = edges;

  albounds.NbIntegers = int (k);
  albounds.Integers = integers;

  // Parcours des sommets
  for (j=0, i=Lower; i<=Upper; i++, j++) {
    ListVertex (i).Coord (X, Y, Z);
    points[j].Point.x = float (X);
    points[j].Point.y = float (Y);
    points[j].Point.z = float (Z);
    ListVertex (i).Normal (DX, DY, DZ);
    points[j].Normal.dx = float (DX);
    points[j].Normal.dy = float (DY);
    points[j].Normal.dz = float (DZ);
  }

  Lower = ListEdge.Lower ();
  Upper = ListEdge.Upper ();

  // Parcours des aretes
  for (j=0, i=Lower; i<=Upper; i++, j++) {
    edges[j].Index1 = int (ListEdge (i).FirstIndex ()-OffSet);
    edges[j].Index2 = int (ListEdge (i).LastIndex ()-OffSet);
    edges[j].Type = int (ListEdge (i).Type ());
  }

  // Parcours des limites (ici toutes les limites sont egales a 3.
  for (j=0; j<albounds.NbIntegers; j++) integers[j] = 3;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polygon_indices");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polygon_indices (&MyCGroup, &alpoints, &aledges, &albounds);

  // Desallocation dynamique
  delete [] points;
  delete [] edges;
  delete [] integers;

}

void OpenGl_GraphicDriver::TriangleSet (const Graphic3d_CGroup& ACGroup,
                                        const Graphic3d_Array1OfVertexC& ListVertex,
                                        const Aspect_Array1OfEdge& ListEdge,
                                        const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Real X, Y, Z;
  Standard_Real R, G, B;

  Standard_Integer i, j, k;
  Standard_Integer Lower, Upper;
  Standard_Integer OffSet;

  CALL_DEF_LISTPOINTS alpoints;
  CALL_DEF_POINTC *points;
  CALL_DEF_LISTEDGES aledges;
  CALL_DEF_EDGE *edges;

  // Permettra une generalisation plus facile a une methode du style
  // PolygonSet (ListVertex, ListEdge, Bounds).
  int *integers;
  CALL_DEF_LISTINTEGERS albounds;

  i = ListVertex.Length ();
  j = ListEdge.Length ();
  k = int (j/3);

  Lower = ListVertex.Lower ();
  Upper = ListVertex.Upper ();
  OffSet  = Lower;

  // Allocation dynamique
  points  = new CALL_DEF_POINTC [i];
  edges = new CALL_DEF_EDGE [j];
  integers= new int [k];

  alpoints.NbPoints = int (i);
  alpoints.TypePoints = 3;
  alpoints.UPoints.PointsC= points;

  aledges.NbEdges   = int (j);
  aledges.Edges   = edges;

  albounds.NbIntegers = int (k);
  albounds.Integers = integers;

  // Parcours des sommets
  for (j=0, i=Lower; i<=Upper; i++, j++) {
    ListVertex (i).Coord (X, Y, Z);
    points[j].Point.x = float (X);
    points[j].Point.y = float (Y);
    points[j].Point.z = float (Z);
    (ListVertex (i).Color ()).Values (R, G, B, Quantity_TOC_RGB);
    points[j].Color.r = float (R);
    points[j].Color.g = float (G);
    points[j].Color.b = float (B);
  }

  Lower = ListEdge.Lower ();
  Upper = ListEdge.Upper ();

  // Parcours des aretes
  for (j=0, i=Lower; i<=Upper; i++, j++) {
    edges[j].Index1 = int (ListEdge (i).FirstIndex ()-OffSet);
    edges[j].Index2 = int (ListEdge (i).LastIndex ()-OffSet);
    edges[j].Type = int (ListEdge (i).Type ());
  }

  // Parcours des limites (ici toutes les limites sont egales a 3.
  for (j=0; j<albounds.NbIntegers; j++) integers[j] = 3;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polygon_indices");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polygon_indices (&MyCGroup, &alpoints, &aledges, &albounds);

  // Desallocation dynamique
  delete [] points;
  delete [] edges;
  delete [] integers;

}

void OpenGl_GraphicDriver::TriangleSet (const Graphic3d_CGroup& ACGroup,
                                        const Graphic3d_Array1OfVertexNC& ListVertex,
                                        const Aspect_Array1OfEdge& ListEdge,
                                        const Standard_Boolean )
{
  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Real X, Y, Z;
  Standard_Real R, G, B;
  Standard_Real DX, DY, DZ;

  Standard_Integer i, j, k;
  Standard_Integer Lower, Upper;
  Standard_Integer OffSet;

  CALL_DEF_LISTPOINTS alpoints;
  CALL_DEF_POINTNC *points;
  CALL_DEF_LISTEDGES aledges;
  CALL_DEF_EDGE *edges;

  // Permettra une generalisation plus facile a une methode du style
  // PolygonSet (ListVertex, ListEdge, Bounds).
  int *integers;
  CALL_DEF_LISTINTEGERS albounds;

  i = ListVertex.Length ();
  j = ListEdge.Length ();
  k = int (j/3);

  Lower = ListVertex.Lower ();
  Upper = ListVertex.Upper ();
  OffSet  = Lower;

  // Allocation dynamique
  points  = new CALL_DEF_POINTNC [i];
  edges = new CALL_DEF_EDGE [j];
  integers= new int [k];

  alpoints.NbPoints = int (i);
  alpoints.TypePoints = 4;
  alpoints.UPoints.PointsNC= points;

  aledges.NbEdges   = int (j);
  aledges.Edges   = edges;

  albounds.NbIntegers = int (k);
  albounds.Integers = integers;

  // Parcours des sommets
  for (j=0, i=Lower; i<=Upper; i++, j++) {
    ListVertex (i).Coord (X, Y, Z);
    points[j].Point.x = float (X);
    points[j].Point.y = float (Y);
    points[j].Point.z = float (Z);
    ListVertex (i).Normal (DX, DY, DZ);
    points[j].Normal.dx = float (DX);
    points[j].Normal.dy = float (DY);
    points[j].Normal.dz = float (DZ);
    (ListVertex (i).Color ()).Values (R, G, B, Quantity_TOC_RGB);
    points[j].Color.r = float (R);
    points[j].Color.g = float (G);
    points[j].Color.b = float (B);
  }

  Lower = ListEdge.Lower ();
  Upper = ListEdge.Upper ();

  // Parcours des aretes
  for (j=0, i=Lower; i<=Upper; i++, j++) {
    edges[j].Index1 = int (ListEdge (i).FirstIndex ()-OffSet);
    edges[j].Index2 = int (ListEdge (i).LastIndex ()-OffSet);
    edges[j].Type = int (ListEdge (i).Type ());
  }

  // Parcours des limites (ici toutes les limites sont egales a 3.
  for (j=0; j<albounds.NbIntegers; j++) integers[j] = 3;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polygon_indices");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polygon_indices (&MyCGroup, &alpoints, &aledges, &albounds);

  // Desallocation dynamique
  delete [] points;
  delete [] edges;
  delete [] integers;

}



void OpenGl_GraphicDriver::TriangleSet(const Graphic3d_CGroup& ACGroup,
                                       const Graphic3d_Array1OfVertexNT& ListVertex,
                                       const Aspect_Array1OfEdge& ListEdge,
                                       const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;
  Standard_Real X, Y, Z;
  Standard_Real DX, DY, DZ;

  Standard_Integer i, j, k;
  Standard_Integer Lower, Upper;
  Standard_Integer OffSet;

  CALL_DEF_LISTPOINTS alpoints;
  CALL_DEF_POINTNT *points;
  CALL_DEF_LISTEDGES aledges;
  CALL_DEF_EDGE *edges;

  // Permettra une generalisation plus facile a une methode du style
  // PolygonSet (ListVertex, ListEdge, Bounds).
  int *integers;
  CALL_DEF_LISTINTEGERS albounds;

  i = ListVertex.Length ();
  j = ListEdge.Length ();
  k = int (j/3);

  Lower = ListVertex.Lower ();
  Upper = ListVertex.Upper ();
  OffSet  = Lower;

  // Allocation dynamique
  points  = new CALL_DEF_POINTNT [i];
  edges = new CALL_DEF_EDGE [j];
  integers= new int [k];

  alpoints.NbPoints = int (i);
  alpoints.TypePoints = 5;
  alpoints.UPoints.PointsNT= points;

  aledges.NbEdges   = int (j);
  aledges.Edges   = edges;

  albounds.NbIntegers = int (k);
  albounds.Integers = integers;

  // Parcours des sommets
  for (j=0, i=Lower; i<=Upper; i++, j++) {
    ListVertex (i).Coord (X, Y, Z);
    points[j].Point.x = float (X);
    points[j].Point.y = float (Y);
    points[j].Point.z = float (Z);
    ListVertex (i).Normal (DX, DY, DZ);
    points[j].Normal.dx = float (DX);
    points[j].Normal.dy = float (DY);
    points[j].Normal.dz = float (DZ);
    ListVertex (i).TextureCoordinate(DX,DY);
    points[j].TextureCoord.tx = float(DX);
    points[j].TextureCoord.ty = float(DY);
  }

  Lower = ListEdge.Lower ();
  Upper = ListEdge.Upper ();

  // Parcours des aretes
  for (j=0, i=Lower; i<=Upper; i++, j++) {
    edges[j].Index1 = int (ListEdge (i).FirstIndex ()-OffSet);
    edges[j].Index2 = int (ListEdge (i).LastIndex ()-OffSet);
    edges[j].Type = int (ListEdge (i).Type ());
  }

  // Parcours des limites (ici toutes les limites sont egales a 3.
  for (j=0; j<albounds.NbIntegers; j++) integers[j] = 3;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polygon_indices");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polygon_indices (&MyCGroup, &alpoints, &aledges, &albounds);

  // Desallocation dynamique
  delete [] points;
  delete [] edges;
  delete [] integers;

}
