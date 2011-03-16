
// File   OpenGl_GraphicDriver_705.cxx
// Created  Mardi 28 janvier 1997
// Author CAL
// Modified
//              27/07/97 ; PCT : ajout coordonnee texture

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

void OpenGl_GraphicDriver::Polygon (const Graphic3d_CGroup& ACGroup,
                                    const Graphic3d_Array1OfVertex& ListVertex,
                                    const Graphic3d_TypeOfPolygon AType,
                                    const Standard_Boolean ) 
{
  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Integer i;
  Standard_Integer Lower  = ListVertex.Lower ();

  CALL_DEF_FACET afacet;

  i = ListVertex.Length ();

  afacet.TypeFacet  = int (AType);

  afacet.NormalIsDefined  = 0;
  afacet.ColorIsDefined = 0;

  afacet.NbPoints   = int (i);
  afacet.TypePoints = 1;
  afacet.UPoints.Points = (CALL_DEF_POINT *) &ListVertex (Lower);

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polygon");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polygon (&MyCGroup, &afacet);

}

void OpenGl_GraphicDriver::Polygon (const Graphic3d_CGroup& ACGroup,
                                    const Graphic3d_Array1OfVertex& ListVertex,
                                    const Graphic3d_Vector& Normal,
                                    const Graphic3d_TypeOfPolygon AType,
                                    const Standard_Boolean ) 
{
  Graphic3d_CGroup MyCGroup = ACGroup;


  Standard_Real X, Y, Z;

  Standard_Integer i;
  Standard_Integer Lower  = ListVertex.Lower ();

  CALL_DEF_FACET afacet;

  i = ListVertex.Length ();

  afacet.TypeFacet  = int (AType);

  afacet.NormalIsDefined  = 1;
  afacet.ColorIsDefined = 0;

  Normal.Coord (X, Y, Z);
  afacet.Normal.dx  = float (X);
  afacet.Normal.dy  = float (Y);
  afacet.Normal.dz  = float (Z);

  afacet.NbPoints   = int (i);
  afacet.TypePoints = 1;
  afacet.UPoints.Points = (CALL_DEF_POINT *) &ListVertex (Lower);

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polygon");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polygon (&MyCGroup, &afacet);

}

void OpenGl_GraphicDriver::Polygon (const Graphic3d_CGroup& ACGroup,
                                    const Graphic3d_Array1OfVertexN& ListVertex,
                                    const Graphic3d_TypeOfPolygon AType,
                                    const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Real X, Y, Z;
  Standard_Real DX, DY, DZ;

  Standard_Integer i, j;
  Standard_Integer Lower  = ListVertex.Lower ();
  Standard_Integer Upper  = ListVertex.Upper ();

  CALL_DEF_FACET afacet;
  CALL_DEF_POINTN *points;

  i = ListVertex.Length ();

  // Allocation dynamique
  points  = new CALL_DEF_POINTN [i];

  afacet.TypeFacet  = int (AType);

  afacet.NormalIsDefined  = 0;
  afacet.ColorIsDefined = 0;

  afacet.NbPoints   = int (i);
  afacet.TypePoints = 2;
  afacet.UPoints.PointsN  = points;

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
    PrintFunction ("call_togl_polygon");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polygon (&MyCGroup, &afacet);

  // Desallocation dynamique
  delete [] points;

}

void OpenGl_GraphicDriver::Polygon (const Graphic3d_CGroup& ACGroup,
                                    const Graphic3d_Array1OfVertexN& ListVertex,
                                    const Graphic3d_Vector& Normal,
                                    const Graphic3d_TypeOfPolygon AType,
                                    const Standard_Boolean ) 
{
  Graphic3d_CGroup MyCGroup = ACGroup;


  Standard_Real X, Y, Z;
  Standard_Real DX, DY, DZ;

  Standard_Integer i, j;
  Standard_Integer Lower  = ListVertex.Lower ();
  Standard_Integer Upper  = ListVertex.Upper ();

  CALL_DEF_FACET afacet;
  CALL_DEF_POINTN *points;

  i = ListVertex.Length ();

  // Allocation dynamique
  points  = new CALL_DEF_POINTN [i];

  afacet.TypeFacet  = int (AType);

  afacet.NormalIsDefined  = 1;
  afacet.ColorIsDefined = 0;

  Normal.Coord (DX, DY, DZ);
  afacet.Normal.dx  = float (DX);
  afacet.Normal.dy  = float (DY);
  afacet.Normal.dz  = float (DZ);

  afacet.NbPoints   = int (i);
  afacet.TypePoints = 2;
  afacet.UPoints.PointsN  = points;

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
    PrintFunction ("call_togl_polygon");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polygon (&MyCGroup, &afacet);

  // Desallocation dynamique
  delete [] points;

}


void OpenGl_GraphicDriver::Polygon(const Graphic3d_CGroup& ACGroup,
                                   const Graphic3d_Array1OfVertexNT& ListVertex,
                                   const Graphic3d_TypeOfPolygon AType,
                                   const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;


  Standard_Real X, Y, Z;
  Standard_Real DX, DY, DZ;
  Standard_Real TX, TY;

  Standard_Integer i, j;
  Standard_Integer Lower  = ListVertex.Lower ();
  Standard_Integer Upper  = ListVertex.Upper ();

  CALL_DEF_FACET afacet;
  CALL_DEF_POINTNT *points;

  i = ListVertex.Length ();

  // Allocation dynamique
  points  = new CALL_DEF_POINTNT [i];

  afacet.TypeFacet  = int (AType);

  afacet.NormalIsDefined  = 0;
  afacet.ColorIsDefined = 0;

  afacet.NbPoints   = int (i);
  afacet.TypePoints = 5;
  afacet.UPoints.PointsNT = points;

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
    ListVertex (i).TextureCoordinate(TX, TY);
    points[j].TextureCoord.tx = float(TX);
    points[j].TextureCoord.ty = float(TY);
  }

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polygon");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polygon (&MyCGroup, &afacet);

  // Desallocation dynamique
  delete [] points;

}


void OpenGl_GraphicDriver::PolygonHoles (const Graphic3d_CGroup& ACGroup,
                                         const TColStd_Array1OfInteger& Bounds,
                                         const Graphic3d_Array1OfVertex& ListVertex,
                                         const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Real X, Y, Z;

  Standard_Integer i, ii;
  Standard_Integer j;
  Standard_Integer k, kk;
  Standard_Integer nb_points;
  Standard_Integer begin_points;
  Standard_Integer end_points;
  Standard_Integer Lower, Upper;

  CALL_DEF_LISTFACETS alfacets;
  CALL_DEF_FACET *facets;
  CALL_DEF_POINT *points;

  i = ListVertex.Length ();
  j = Bounds.Length ();

  // Allocation dynamique
  points  = new CALL_DEF_POINT [i];
  facets  = new CALL_DEF_FACET [j];

  alfacets.NbFacets = int (j);
  alfacets.LFacets  = facets;

  begin_points  = ListVertex.Lower ();
  end_points  = ListVertex.Upper ();

  Lower = Bounds.Lower ();
  Upper = Bounds.Upper ();

  nb_points = 0;

  // Parcours des facettes
  for (k=0, kk=Lower; kk<=Upper; k++, kk++) {
    facets[k].TypeFacet   = 0; /* TOP_UNKNOWN */

    facets[k].NormalIsDefined = 0;
    facets[k].ColorIsDefined  = 0;

    facets[k].NbPoints    = int (Bounds.Value (kk));
    facets[k].TypePoints    = 1;
    facets[k].UPoints.Points  = (points + nb_points);

    // Parcours des sommets
    for (i=0, ii=begin_points;
      ((ii<=end_points) || (i==facets[k].NbPoints-1));
      i++, ii++) {
        ListVertex (ii).Coord (X, Y, Z);
        points[nb_points+i].x = float (X);
        points[nb_points+i].y = float (Y);
        points[nb_points+i].z = float (Z);
      }
      begin_points += facets[k].NbPoints;
      nb_points += facets[k].NbPoints;
  }

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polygon_holes");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polygon_holes (&MyCGroup, &alfacets);

  // Desallocation dynamique
  delete [] points;
  delete [] facets;

}

void OpenGl_GraphicDriver::PolygonHoles (const Graphic3d_CGroup& ACGroup,
                                         const TColStd_Array1OfInteger& Bounds,
                                         const Graphic3d_Array1OfVertex& ListVertex,
                                         const Graphic3d_Vector& Normal,
                                         const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Real X, Y, Z;
  Standard_Real NX, NY, NZ;

  Standard_Integer i, ii;
  Standard_Integer j;
  Standard_Integer k, kk;
  Standard_Integer nb_points;
  Standard_Integer begin_points;
  Standard_Integer end_points;
  Standard_Integer Lower, Upper;

  CALL_DEF_LISTFACETS alfacets;
  CALL_DEF_FACET *facets;
  CALL_DEF_POINT *points;

  i = ListVertex.Length ();
  j = Bounds.Length ();

  // Allocation dynamique
  points  = new CALL_DEF_POINT [i];
  facets  = new CALL_DEF_FACET [j];

  alfacets.NbFacets = int (j);
  alfacets.LFacets  = facets;

  begin_points  = ListVertex.Lower ();
  end_points  = ListVertex.Upper ();

  Lower = Bounds.Lower ();
  Upper = Bounds.Upper ();

  nb_points = 0;

  Normal.Coord (NX, NY, NZ);

  // Parcours des facettes
  for (k=0, kk=Lower; kk<=Upper; k++, kk++) {
    facets[k].TypeFacet   = 0; /* TOP_UNKNOWN */

    facets[k].ColorIsDefined  = 0;
    facets[k].NormalIsDefined = 1;
    facets[k].Normal.dx   = float (NX);
    facets[k].Normal.dy   = float (NY);
    facets[k].Normal.dz   = float (NZ);

    facets[k].NbPoints    = int (Bounds.Value (kk));
    facets[k].TypePoints    = 1;
    facets[k].UPoints.Points  = (points + nb_points);

    // Parcours des sommets
    for (i=0, ii=begin_points;
      ((ii<=end_points) || (i==facets[k].NbPoints-1));
      i++, ii++) {
        ListVertex (ii).Coord (X, Y, Z);
        points[nb_points+i].x = float (X);
        points[nb_points+i].y = float (Y);
        points[nb_points+i].z = float (Z);
      }
      begin_points += facets[k].NbPoints;
      nb_points += facets[k].NbPoints;
  }

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polygon_holes");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polygon_holes (&MyCGroup, &alfacets);

  // Desallocation dynamique
  delete [] points;
  delete [] facets;

}

void OpenGl_GraphicDriver::PolygonHoles (const Graphic3d_CGroup& ACGroup,
                                         const TColStd_Array1OfInteger& Bounds,
                                         const Graphic3d_Array1OfVertexN& ListVertex,
                                         const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Real X, Y, Z;
  Standard_Real DX, DY, DZ;

  Standard_Integer i, ii;
  Standard_Integer j;
  Standard_Integer k, kk;
  Standard_Integer nb_points;
  Standard_Integer begin_points;
  Standard_Integer end_points;
  Standard_Integer Lower, Upper;

  CALL_DEF_LISTFACETS alfacets;
  CALL_DEF_FACET *facets;
  CALL_DEF_POINTN *points;

  i = ListVertex.Length ();
  j = Bounds.Length ();

  // Allocation dynamique
  points  = new CALL_DEF_POINTN [i];
  facets  = new CALL_DEF_FACET [j];

  alfacets.NbFacets = int (j);
  alfacets.LFacets  = facets;

  begin_points  = ListVertex.Lower ();
  end_points  = ListVertex.Upper ();

  Lower = Bounds.Lower ();
  Upper = Bounds.Upper ();

  nb_points = 0;

  // Parcours des facettes
  for (k=0, kk=Lower; kk<=Upper; k++, kk++) {
    facets[k].TypeFacet   = 0; /* TOP_UNKNOWN */

    facets[k].NormalIsDefined = 0;
    facets[k].ColorIsDefined  = 0;

    facets[k].NbPoints    = int (Bounds.Value (kk));
    facets[k].TypePoints    = 2;
    facets[k].UPoints.PointsN = (points + nb_points);

    // Parcours des sommets
    for (i=0, ii=begin_points;
      ((ii<=end_points) || (i==facets[k].NbPoints-1));
      i++, ii++) {
        ListVertex (ii).Coord (X, Y, Z);
        points[nb_points+i].Point.x = float (X);
        points[nb_points+i].Point.y = float (Y);
        points[nb_points+i].Point.z = float (Z);
        ListVertex (ii).Normal (DX, DY, DZ);
        points[nb_points+i].Normal.dx = float (DX);
        points[nb_points+i].Normal.dy = float (DY);
        points[nb_points+i].Normal.dz = float (DZ);
      }
      begin_points += facets[k].NbPoints;
      nb_points += facets[k].NbPoints;
  }

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polygon_holes");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polygon_holes (&MyCGroup, &alfacets);

  // Desallocation dynamique
  delete [] points;
  delete [] facets;

}

void OpenGl_GraphicDriver::PolygonHoles (const Graphic3d_CGroup& ACGroup,
                                         const TColStd_Array1OfInteger& Bounds,
                                         const Graphic3d_Array1OfVertexN& ListVertex,
                                         const Graphic3d_Vector& Normal,
                                         const Standard_Boolean ) 
{

  Graphic3d_CGroup MyCGroup = ACGroup;

  Standard_Real X, Y, Z;
  Standard_Real DX, DY, DZ;
  Standard_Real NX, NY, NZ;

  Standard_Integer i, ii;
  Standard_Integer j;
  Standard_Integer k, kk;
  Standard_Integer nb_points;
  Standard_Integer begin_points;
  Standard_Integer end_points;
  Standard_Integer Lower, Upper;

  CALL_DEF_LISTFACETS alfacets;
  CALL_DEF_FACET *facets;
  CALL_DEF_POINTN *points;

  i = ListVertex.Length ();
  j = Bounds.Length ();

  // Allocation dynamique
  points  = new CALL_DEF_POINTN [i];
  facets  = new CALL_DEF_FACET [j];

  alfacets.NbFacets = int (j);
  alfacets.LFacets  = facets;

  begin_points  = ListVertex.Lower ();
  end_points  = ListVertex.Upper ();

  Lower = Bounds.Lower ();
  Upper = Bounds.Upper ();

  nb_points = 0;

  Normal.Coord (NX, NY, NZ);

  // Parcours des facettes
  for (k=0, kk=Lower; kk<=Upper; k++, kk++) {
    facets[k].TypeFacet   = 0; /* TOP_UNKNOWN */

    facets[k].ColorIsDefined  = 0;
    facets[k].NormalIsDefined = 1;
    facets[k].Normal.dx   = float (NX);
    facets[k].Normal.dy   = float (NY);
    facets[k].Normal.dz   = float (NZ);

    facets[k].NbPoints    = int (Bounds.Value (kk));
    facets[k].TypePoints    = 2;
    facets[k].UPoints.PointsN = (points + nb_points);

    // Parcours des sommets
    for (i=0, ii=begin_points;
      ((ii<=end_points) || (i==facets[k].NbPoints-1));
      i++, ii++) {
        ListVertex (ii).Coord (X, Y, Z);
        points[nb_points+i].Point.x = float (X);
        points[nb_points+i].Point.y = float (Y);
        points[nb_points+i].Point.z = float (Z);
        ListVertex (ii).Normal (DX, DY, DZ);
        points[nb_points+i].Normal.dx = float (DX);
        points[nb_points+i].Normal.dy = float (DY);
        points[nb_points+i].Normal.dz = float (DZ);
      }
      begin_points += facets[k].NbPoints;
      nb_points += facets[k].NbPoints;
  }

  if (MyTraceLevel) {
    PrintFunction ("call_togl_polygon_holes");
    PrintCGroup (MyCGroup, 1);
  }
  call_togl_polygon_holes (&MyCGroup, &alfacets);

  // Desallocation dynamique
  delete [] points;
  delete [] facets;

}
