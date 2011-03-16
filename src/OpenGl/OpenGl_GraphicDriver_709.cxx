
// File   OpenGl_GraphicDriver_709.cxx
// Created  Mardi 28 janvier 1997
// Author CAL
// Modified
//              27/08/97 ; PCT : ajout coordonnee texture

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

void OpenGl_GraphicDriver::QuadrangleMesh (const Graphic3d_CGroup& ACGroup,
                                           const Graphic3d_Array2OfVertex& ListVertex,
                                           const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Real X, Y, Z;

  Standard_Integer i, j, k;
  Standard_Integer LowerRow = ListVertex.LowerRow ();
  Standard_Integer UpperRow = ListVertex.UpperRow ();
  Standard_Integer LowerCol = ListVertex.LowerCol ();
  Standard_Integer UpperCol = ListVertex.UpperCol ();

  CALL_DEF_QUAD aquad;
  CALL_DEF_POINT *points;

  i = ListVertex.RowLength ();
  j = ListVertex.ColLength ();

  // Allocation dynamique
  points  = new CALL_DEF_POINT [i*j];

  aquad.NbPoints    = int (i*j);
  aquad.TypePoints  = 1;
  aquad.SizeRow   = int (i);
  aquad.SizeCol   = int (j);
  aquad.UPoints.Points  = points;

  // Parcours des sommets
  k = 0;
  for (i=LowerRow; i<=UpperRow; i++)
    for (j=LowerCol; j<=UpperCol; j++) {
      ListVertex (i, j).Coord (X, Y, Z);
      points[k].x = float (X);
      points[k].y = float (Y);
      points[k].z = float (Z);
      k++;
    }

    if (MyTraceLevel) {
      PrintFunction ("call_togl_quadrangle");
      PrintCGroup (MyCGroup, 1);
    }
    call_togl_quadrangle (&MyCGroup, &aquad);

    // Desallocation dynamique
    delete [] points;

}

void OpenGl_GraphicDriver::QuadrangleMesh (const Graphic3d_CGroup& ACGroup,
                                           const Graphic3d_Array2OfVertexN& ListVertex,
                                           const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Real X, Y, Z;
  Standard_Real DX, DY, DZ;

  Standard_Integer i, j, k;
  Standard_Integer LowerRow = ListVertex.LowerRow ();
  Standard_Integer UpperRow = ListVertex.UpperRow ();
  Standard_Integer LowerCol = ListVertex.LowerCol ();
  Standard_Integer UpperCol = ListVertex.UpperCol ();

  CALL_DEF_QUAD aquad;
  CALL_DEF_POINTN *points;

  i = ListVertex.RowLength ();
  j = ListVertex.ColLength ();

  // Allocation dynamique
  points  = new CALL_DEF_POINTN [i*j];

  aquad.NbPoints    = int (i*j);
  aquad.TypePoints  = 2;
  aquad.SizeRow   = int (i);
  aquad.SizeCol   = int (j);
  aquad.UPoints.PointsN = points;

  // Parcours des sommets
  k = 0;
  for (i=LowerRow; i<=UpperRow; i++)
    for (j=LowerCol; j<=UpperCol; j++) {
      ListVertex (i, j).Coord (X, Y, Z);
      points[k].Point.x = float (X);
      points[k].Point.y = float (Y);
      points[k].Point.z = float (Z);
      ListVertex (i, j).Normal (DX, DY, DZ);
      points[k].Normal.dx = float (DX);
      points[k].Normal.dy = float (DY);
      points[k].Normal.dz = float (DZ);
      k++;
    }

    if (MyTraceLevel) {
      PrintFunction ("call_togl_quadrangle");
      PrintCGroup (MyCGroup, 1);
    }
    call_togl_quadrangle (&MyCGroup, &aquad);

    // Desallocation dynamique
    delete [] points;

}


void OpenGl_GraphicDriver::QuadrangleMesh(const Graphic3d_CGroup& ACGroup,
                                          const Graphic3d_Array2OfVertexNT& ListVertex,
                                          const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Real X, Y, Z;
  Standard_Real DX, DY, DZ;

  Standard_Integer i, j, k;
  Standard_Integer LowerRow = ListVertex.LowerRow ();
  Standard_Integer UpperRow = ListVertex.UpperRow ();
  Standard_Integer LowerCol = ListVertex.LowerCol ();
  Standard_Integer UpperCol = ListVertex.UpperCol ();

  CALL_DEF_QUAD aquad;
  CALL_DEF_POINTNT *points;

  i = ListVertex.RowLength ();
  j = ListVertex.ColLength ();

  // Allocation dynamique
  points  = new CALL_DEF_POINTNT [i*j];

  aquad.NbPoints    = int (i*j);
  aquad.TypePoints  = 5;
  aquad.SizeRow   = int (i);
  aquad.SizeCol   = int (j);
  aquad.UPoints.PointsNT  = points;

  // Parcours des sommets
  k = 0;
  for (i=LowerRow; i<=UpperRow; i++)
    for (j=LowerCol; j<=UpperCol; j++) {
      ListVertex (i, j).Coord (X, Y, Z);
      points[k].Point.x = float (X);
      points[k].Point.y = float (Y);
      points[k].Point.z = float (Z);
      ListVertex (i, j).Normal (DX, DY, DZ);
      points[k].Normal.dx = float (DX);
      points[k].Normal.dy = float (DY);
      points[k].Normal.dz = float (DZ);
      ListVertex (i, j).TextureCoordinate(DX,DY);
      points[k].TextureCoord.tx = float(DX);
      points[k].TextureCoord.ty = float(DY);
      k++;
    }

    if (MyTraceLevel) {
      PrintFunction ("call_togl_quadrangle");
      PrintCGroup (MyCGroup, 1);
    }
    call_togl_quadrangle (&MyCGroup, &aquad);

    // Desallocation dynamique
    delete [] points;

}


void OpenGl_GraphicDriver::QuadrangleSet (const Graphic3d_CGroup& ACGroup,
                                          const Graphic3d_Array1OfVertex& ListVertex,
                                          const Aspect_Array1OfEdge& ListEdge,
                                          const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

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

  i = ListVertex.Length ();
  j = ListEdge.Length ();
  k = int (j/4);

  Lower = ListVertex.Lower ();
  Upper = ListVertex.Upper ();
  OffSet  = Lower;

  // Allocation dynamique
  edges = new CALL_DEF_EDGE [j];
  integers= new int [k];

  alpoints.NbPoints = int (i);
  alpoints.TypePoints = 1;
  alpoints.UPoints.Points = (CALL_DEF_POINT *) &ListVertex (Lower);

  aledges.NbEdges   = int (j);
  aledges.Edges   = edges;

  albounds.NbIntegers = int (k);
  albounds.Integers = integers;

  Lower = ListEdge.Lower ();
  Upper = ListEdge.Upper ();

  // Parcours des aretes
  for (j=0, i=Lower; i<=Upper; i++, j++) {
    edges[j].Index1 = int (ListEdge (i).FirstIndex ()-OffSet);
    edges[j].Index2 = int (ListEdge (i).LastIndex ()-OffSet);
    edges[j].Type = int (ListEdge (i).Type ());
  }

  // Parcours des limites (ici toutes les limites sont egales a 4.
  for (j=0; j<albounds.NbIntegers; j++) integers[j] = 4;

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polygon_indices");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polygon_indices (&MyCGroup, &alpoints, &aledges, &albounds);

  // Desallocation dynamique
  delete [] edges;
  delete [] integers;

}

void OpenGl_GraphicDriver::QuadrangleSet (const Graphic3d_CGroup& ACGroup,
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
  k = int (j/4);

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

  // Parcours des limites (ici toutes les limites sont egales a 4.
  for (j=0; j<albounds.NbIntegers; j++) integers[j] = 4;

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

void OpenGl_GraphicDriver::QuadrangleSet (const Graphic3d_CGroup& ACGroup,
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
  k = int (j/4);

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

  // Parcours des limites (ici toutes les limites sont egales a 4.
  for (j=0; j<albounds.NbIntegers; j++) integers[j] = 4;

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

void OpenGl_GraphicDriver::QuadrangleSet (const Graphic3d_CGroup& ACGroup,
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
  k = int (j/4);

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

  // Parcours des limites (ici toutes les limites sont egales a 4.
  for (j=0; j<albounds.NbIntegers; j++) integers[j] = 4;

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



void OpenGl_GraphicDriver::QuadrangleSet(const Graphic3d_CGroup& ACGroup,
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
  k = int (j/4);

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

  // Parcours des limites (ici toutes les limites sont egales a 4.
  for (j=0; j<albounds.NbIntegers; j++) integers[j] = 4;

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
