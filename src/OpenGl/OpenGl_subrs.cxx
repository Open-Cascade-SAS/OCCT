/***********************************************************************

FONCTION :
----------
File OpenGl_subrs :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
11-03-96 : FMN ; Correction warning compilation
01-04-96 : CAL ; Integration MINSK portage WNT
26-04-96 : FMN ; Correction warning compilation
10-06-96 : CAL ; Transparence dans call_subr_set_refl_props
17-07-96 : CAL ; Suppression de CALL_PHIGS_ELEM_HLHSR_ID
07-10-96 : FMN ; Suppression code inutile
21-08-97 : PCT ; ajout deuxieme passe pour les textures
23-12-97 : FMN ; Suppression TelBackInteriorStyle, TelBackInteriorStyleIndex
29-12-97 : FMN ; Ajout cas 4 pour call_subr_polygon_indices()
30-12-97 : FMN ; CTS18312: Correction back material
08-04-98 : FGU ; Ajout emission
30-11-98 : FMN ; S3819 : Textes toujours visibles
16-06-2000 : ATS,GG : G005 : Group of functions: call_subr_parray

************************************************************************/

#define IMP190602       /* GG Avoid memory leaks after creating a polyline
//                      with vertex colors primitive.
//                      Thanks to Ivan Fontaine (SAMTECH) for this improvment
*/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 


#include <OpenGl_tgl_all.hxx>

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>
#include <OpenGl_telem_inquire.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_tgl_elems.hxx>
#include <OpenGl_Memory.hxx>

void
call_subr_set_line_colr( Tfloat  r, Tfloat  g, Tfloat  b )
{
  CMN_KEY  key;
  TEL_COLOUR  col;

  col.rgb[0] = r,
  col.rgb[1] = g,
  col.rgb[2] = b;
  key.data.pdata = &col;
  TsmAddToStructure( TelPolylineColour, 1, &key );

  return;
}


void
call_subr_polyline( CALL_DEF_LISTPOINTS * alpoints )
{
  CMN_KEY  key[4];
  cmn_key  kp[4];

  kp[0] = &key[0], kp[1] = &key[1], kp[2] = &key[2], kp[3] = &key[3];
  key[0].id = NUM_LINES_ID;
  key[0].data.ldata = 1;
  key[1].id = VERTICES_ID;
  key[1].data.pdata = alpoints->UPoints.Points;
  key[2].id = BOUNDS_DATA_ID;
  key[2].data.pdata = &alpoints->NbPoints;
  key[3].id = NUM_VERTICES_ID;
  key[3].data.ldata = alpoints->NbPoints;
  TsmAddToStructure( TelPolyline, -4, kp );

  return;
}


void
call_subr_polyline_data( CALL_DEF_LISTPOINTS * alpoints )
{
  CMN_KEY  key[5];
  cmn_key  kp[5];
  tel_point points;
  tel_colour col;
  register   Tint  i;

  if( alpoints->TypePoints != 3 )
    return;  /* Coordinates and Vertex Colour not Specified */

  points = new TEL_POINT[alpoints->NbPoints];
  if( !points )
    return;

  col = new TEL_COLOUR[alpoints->NbPoints];
  if( !col )
  {
    delete[] points;
    return;
  }

  for( i = 0; i < alpoints->NbPoints; i++ )
  {
    points[i].xyz[0] = alpoints->UPoints.PointsC[i].Point.x;
    points[i].xyz[1] = alpoints->UPoints.PointsC[i].Point.y;
    points[i].xyz[2] = alpoints->UPoints.PointsC[i].Point.z;
    col[i].rgb[0] = alpoints->UPoints.PointsC[i].Color.r;
    col[i].rgb[1] = alpoints->UPoints.PointsC[i].Color.g;
    col[i].rgb[2] = alpoints->UPoints.PointsC[i].Color.b;
  }

  key[0].id = NUM_LINES_ID;
  key[0].data.ldata = 1;
  key[1].id = VERTEX_COLOUR_VALS_ID;
  key[1].data.pdata = col;
  key[2].id = VERTICES_ID;
  key[2].data.pdata = points;
  key[3].id = BOUNDS_DATA_ID;
  key[3].data.pdata = &alpoints->NbPoints;
  key[4].id = NUM_VERTICES_ID;
  key[4].data.ldata = alpoints->NbPoints;

  for( i = 0; i < 5; i++ )
    kp[i] = &key[i];

  TsmAddToStructure( TelPolyline, -5, kp );

#ifdef IMP190602
  delete[] points;
  delete col;
#endif

  return;
}



void
call_subr_remnameset( Tint num, Tint  *ns )
{
  CMN_KEY  key;

  key.id = num;
  key.data.pdata = ns;
  TsmAddToStructure( TelRemoveNameset, 1, &key );

  return;
}


void
call_subr_addnameset( Tint num, Tint  *ns )
{
  CMN_KEY  key;

  key.id = num;
  key.data.pdata = ns;
  TsmAddToStructure( TelAddNameset, 1, &key );

  return;
}

void
call_subr_set_back_int_colr( Tfloat r, Tfloat g, Tfloat b, Tfloat a )
{
  CMN_KEY  key;
  TEL_COLOUR  col;

  col.rgb[0] = r,
    col.rgb[1] = g,
    col.rgb[2] = b;
  key.data.pdata = &col;
  TsmAddToStructure( TelBackInteriorColour, 1, &key );

  return;
}



void
call_subr_set_int_colr( Tfloat r, Tfloat g, Tfloat b, Tfloat a )
{
  CMN_KEY  key;
  TEL_COLOUR  col;

  col.rgb[0] = r,
    col.rgb[1] = g,
    col.rgb[2] = b;
  key.data.pdata = &col;
  TsmAddToStructure( TelInteriorColour, 1, &key );

  return;
}

void
call_subr_set_edge_colr( Tfloat r, Tfloat g, Tfloat b )
{
  CMN_KEY  key;
  TEL_COLOUR  col;

  col.rgb[0] = r,
    col.rgb[1] = g,
    col.rgb[2] = b;
  key.data.pdata = &col;
  TsmAddToStructure( TelEdgeColour, 1, &key );

  return;
}


void
call_subr_set_refl_props( CALL_DEF_MATERIAL * material, Tint tag )
{
  CMN_KEY  key;
  TEL_SURF_PROP  surface;

  /* Par defaut tout a 0 */
  surface.amb  = ( float )0.0;
  surface.diff = ( float )0.0;
  surface.spec = ( float )0.0;
  surface.emsv = ( float )0.0;
  surface.isamb    = (int)0;
  surface.isdiff   = (int)0;
  surface.isspec   = (int)0;
  surface.isemsv   = (int)0;
  surface.isphysic = (int)0;

  /* Cas par cas pour l evaluation */
  if( material->IsAmbient )
  {
    surface.amb  = material->Ambient;
    surface.isamb = 1;
  }
  if( material->IsDiffuse )
  {
    surface.diff = material->Diffuse;
    surface.isdiff = 1;
  }
  if( material->IsSpecular )
  {
    surface.spec = material->Specular;
    surface.isspec = 1;
  }
  if( material->IsEmission )
  {
    surface.emsv = material->Emission;
    surface.isemsv = 1;
  }

  /* type de materiel */
  if( material->IsPhysic )
    surface.isphysic = 1;

  /* Couleur du materiel */

  /* Couleur eclairage ambient */
  surface.ambcol.rgb[0] = material->ColorAmb.r;
  surface.ambcol.rgb[1] = material->ColorAmb.g;
  surface.ambcol.rgb[2] = material->ColorAmb.b;

  /* Couleur eclairage diffus */
  surface.difcol.rgb[0] = material->ColorDif.r;
  surface.difcol.rgb[1] = material->ColorDif.g;
  surface.difcol.rgb[2] = material->ColorDif.b;

  /* Couleur eclairage speculaire */
  surface.speccol.rgb[0] = material->ColorSpec.r;
  surface.speccol.rgb[1] = material->ColorSpec.g;
  surface.speccol.rgb[2] = material->ColorSpec.b;

  /* Couleur d emission */
  surface.emscol.rgb[0] = material->ColorEms.r;
  surface.emscol.rgb[1] = material->ColorEms.g;
  surface.emscol.rgb[2] = material->ColorEms.b;

  surface.shine = ( float )128 * material->Shininess;
  surface.env_reflexion = material->EnvReflexion;

  /* Dans la couche C++ :
  * prop->trans = 0. => opaque
  * prop->trans = 1. => transparent
  * en OpenGl c'est le contraire.
  */
  surface.trans = 1.0F - material->Transparency;

  key.data.pdata = &surface;
  TsmAddToStructure( tag ? TelBackSurfaceAreaProperties :
  TelSurfaceAreaProperties, 1, &key );
  return;
}

void
call_subr_set_marker_colr( Tfloat r, Tfloat g, Tfloat b )
{
  CMN_KEY  key;
  TEL_COLOUR  col;

  col.rgb[0] = r,
    col.rgb[1] = g,
    col.rgb[2] = b;
  key.data.pdata = &col;
  TsmAddToStructure( TelPolymarkerColour, 1, &key );

  return;
}


TStatus
call_subr_get_exec_struct( Tint afather, Tint ason, Tint  *ind )
{
  tsm_node    node;
  Tint        i, num;

  if( TsmGetStructure( afather, &num, &node ) == TSuccess )
  {
    for( i = 0; i < num; i++, node = node->next )
    {
      if( node->elem.el == TelExecuteStructure )
        if( node->elem.data.ldata == ason )
        {
          *ind = (i+1);
          return TSuccess;
        }
    }
  }
  return TFailure;
}


void
call_subr_marker( CALL_DEF_MARKER * amarker )
{
  CMN_KEY  key;
  TEL_POINT  pt;

  key.id = 1;
  pt.xyz[0] = amarker->x,
    pt.xyz[1] = amarker->y,
    pt.xyz[2] = amarker->z;
  key.data.pdata = &pt;
  TsmAddToStructure( TelMarkerSet, 1, &key );

  return;
}


void
call_subr_marker_set( CALL_DEF_LISTMARKERS * almarkers )
{
  CMN_KEY  key;

  key.id = almarkers->NbMarkers;
  key.data.pdata = almarkers->Markers;
  TsmAddToStructure( TelMarkerSet, 1, &key );

  return;
}


void
call_subr_polygon( CALL_DEF_FACET * afacet )
{
  CMN_KEY      k[3];

  k[0].id = NUM_VERTICES_ID;
  k[0].data.ldata = afacet->NbPoints;

  k[1].id = SHAPE_FLAG_ID;
  k[1].data.ldata = TEL_SHAPE_UNKNOWN;

  switch (afacet->TypeFacet) {
        case 0 : /* TOP_UNKNOWN */
          k[1].data.ldata = TEL_SHAPE_UNKNOWN;
          break;
        case 1 : /* TOP_COMPLEX */
          k[1].data.ldata = TEL_SHAPE_COMPLEX;
          break;
        case 2 : /* TOP_CONCAVE */
          k[1].data.ldata = TEL_SHAPE_CONCAVE;
          break;
        case 3 : /* TOP_CONVEX */
          k[1].data.ldata = TEL_SHAPE_CONVEX;
          break;
  }

#if defined(__sgi) || defined(IRIX)
  /* Pb avec le tesselator sgi */
  k[1].data.ldata = TEL_SHAPE_CONVEX;
#endif

  k[2].id = VERTICES_ID;
  k[2].data.pdata = afacet->UPoints.Points;

  TsmAddToStructure( TelPolygon, 3, &k[0], &k[1], &k[2] );

  return;
}


void
call_subr_polygon_data( CALL_DEF_FACET * afacet )
{
  CMN_KEY      k[10];
  cmn_key      kp[10];
  Tint         nk = 0, i;
  TEL_COLOUR   col;
  void        *npt;
  TEL_POINT   *pts=0, *ptn=0;
  TEL_TEXTURE_COORD *ptt=0;

  k[nk].id = NUM_VERTICES_ID;
  k[nk].data.ldata = afacet->NbPoints;
  nk++;

  if( afacet->ColorIsDefined )
  {
    col.rgb[0] = afacet->Color.r;
    col.rgb[1] = afacet->Color.g;
    col.rgb[2] = afacet->Color.b;

    k[nk].id = FACET_COLOUR_VALS_ID;
    k[nk].data.pdata = &col;
    nk++;
  }

  if( afacet->NormalIsDefined )
  {
    npt = &afacet->Normal;
    k[nk].id = FNORMALS_ID;
    k[nk].data.pdata = npt;
    nk++;
  }

  switch (afacet->TypePoints)
  {
  case 1:
    {
      k[nk].id = SHAPE_FLAG_ID;
      k[nk].data.ldata = TEL_SHAPE_UNKNOWN;
      switch (afacet->TypeFacet) {
  case 0 : /* TOP_UNKNOWN */
    k[nk].data.ldata = TEL_SHAPE_UNKNOWN;
    break;
  case 1 : /* TOP_COMPLEX */
    k[nk].data.ldata = TEL_SHAPE_COMPLEX;
    break;
  case 2 : /* TOP_CONCAVE */
    k[nk].data.ldata = TEL_SHAPE_CONCAVE;
    break;
  case 3 : /* TOP_CONVEX */
    k[nk].data.ldata = TEL_SHAPE_CONVEX;
    break;
      }
#if defined(__sgi) || defined(IRIX)
      /* Pb avec le tesselator sgi */
      k[nk].data.ldata = TEL_SHAPE_CONVEX;
#endif
      nk++;
      k[nk].id = VERTICES_ID;
      k[nk].data.pdata = afacet->UPoints.Points;
      nk++;
      break;
    }
  case 2:
    {
      Tint i;

      k[nk].id = SHAPE_FLAG_ID;
      k[nk].data.ldata = TEL_SHAPE_UNKNOWN;
      switch (afacet->TypeFacet) {
  case 0 : /* TOP_UNKNOWN */
    k[nk].data.ldata = TEL_SHAPE_UNKNOWN;
    break;
  case 1 : /* TOP_COMPLEX */
    k[nk].data.ldata = TEL_SHAPE_COMPLEX;
    break;
  case 2 : /* TOP_CONCAVE */
    k[nk].data.ldata = TEL_SHAPE_CONCAVE;
    break;
  case 3 : /* TOP_CONVEX */
    k[nk].data.ldata = TEL_SHAPE_CONVEX;
    break;
      }
#if defined(__sgi) || defined(IRIX)
      /* Pb avec le tesselator sgi */
      k[nk].data.ldata = TEL_SHAPE_CONVEX;
#endif
      nk++;
      //cmn_memreserve( pts, afacet->NbPoints, 0 );
      pts = new TEL_POINT[afacet->NbPoints];
      k[nk].id = VERTICES_ID;
      k[nk].data.pdata = pts;
      nk++;
      //cmn_memreserve( ptn, afacet->NbPoints, 0 );
      ptn = new TEL_POINT[afacet->NbPoints];
      k[nk].id = VNORMALS_ID;
      k[nk].data.pdata = ptn;
      nk++;
      for( i = 0; i < afacet->NbPoints; i++ )
      {
        pts[i].xyz[0] = afacet->UPoints.PointsN[i].Point.x;
        pts[i].xyz[1] = afacet->UPoints.PointsN[i].Point.y;
        pts[i].xyz[2] = afacet->UPoints.PointsN[i].Point.z;

        ptn[i].xyz[0] = afacet->UPoints.PointsN[i].Normal.dx;
        ptn[i].xyz[1] = afacet->UPoints.PointsN[i].Normal.dy;
        ptn[i].xyz[2] = afacet->UPoints.PointsN[i].Normal.dz;
      }
      break;
    }
  case 5:
    {
      Tint i;

      k[nk].id = SHAPE_FLAG_ID;
      k[nk].data.ldata = TEL_SHAPE_UNKNOWN;
      switch (afacet->TypeFacet) {
  case 0 : /* TOP_UNKNOWN */
    k[nk].data.ldata = TEL_SHAPE_UNKNOWN;
    break;
  case 1 : /* TOP_COMPLEX */
    k[nk].data.ldata = TEL_SHAPE_COMPLEX;
    break;
  case 2 : /* TOP_CONCAVE */
    k[nk].data.ldata = TEL_SHAPE_CONCAVE;
    break;
  case 3 : /* TOP_CONVEX */
    k[nk].data.ldata = TEL_SHAPE_CONVEX;
    break;
      }
#if defined(__sgi) || defined(IRIX)
      /* Pb avec le tesselator sgi */
      k[nk].data.ldata = TEL_SHAPE_CONVEX;
#endif
      nk++;
      //cmn_memreserve( pts, afacet->NbPoints, 0 );
      pts = new TEL_POINT[afacet->NbPoints];
      k[nk].id = VERTICES_ID;
      k[nk].data.pdata = pts;
      nk++;
      //cmn_memreserve( ptn, afacet->NbPoints, 0 );
      ptn = new TEL_POINT[afacet->NbPoints];
      k[nk].id = VNORMALS_ID;
      k[nk].data.pdata = ptn;
      nk++;
      //cmn_memreserve( ptt, afacet->NbPoints, 0 );
      ptt = new TEL_TEXTURE_COORD[afacet->NbPoints];
      k[nk].id = VTEXTURECOORD_ID;
      k[nk].data.pdata = ptt;
      nk++;
      for( i = 0; i < afacet->NbPoints; i++ )
      {
        pts[i].xyz[0] = afacet->UPoints.PointsNT[i].Point.x;
        pts[i].xyz[1] = afacet->UPoints.PointsNT[i].Point.y;
        pts[i].xyz[2] = afacet->UPoints.PointsNT[i].Point.z;

        ptn[i].xyz[0] = afacet->UPoints.PointsNT[i].Normal.dx;
        ptn[i].xyz[1] = afacet->UPoints.PointsNT[i].Normal.dy;
        ptn[i].xyz[2] = afacet->UPoints.PointsNT[i].Normal.dz;

        ptt[i].xy[0] = afacet->UPoints.PointsNT[i].TextureCoord.tx;
        ptt[i].xy[1] = afacet->UPoints.PointsNT[i].TextureCoord.ty;
      }
      break;
    }
  }

  for( i = 0; i < nk; i++ )
    kp[i] = &k[i];

  TsmAddToStructure( TelPolygon, -nk, kp );

  if( pts )
  {
    //cmn_freemem( pts );
    delete[] pts;
    //cmn_freemem( ptn );
    delete[] ptn;
    //cmn_freemem( ptt );
    delete[] ptt;
  }

  return;
}


void
call_subr_polygon_indices( CALL_DEF_LISTPOINTS * alpoints,
                          CALL_DEF_LISTEDGES * aledges,
                          CALL_DEF_LISTINTEGERS * albounds )
{
  CMN_KEY      k[10];
  cmn_key      kp[10];
  Tint         nk = 0, i;
  tel_point    varr=0;
  tel_point    narr=0;
  tel_colour   carr=0;
  tel_texture_coord tarr=0;
  Tint        *edgvis=NULL, *bounds, *indices;
  Tint         num_indices;

  k[nk].id = NUM_VERTICES_ID;
  k[nk].data.ldata = alpoints->NbPoints;
  nk++;

  k[nk].id = NUM_FACETS_ID;
  k[nk].data.ldata = albounds->NbIntegers;
  nk++;

  k[nk].id = SHAPE_FLAG_ID;
  k[nk].data.ldata = TEL_SHAPE_UNKNOWN;
  nk++;

  if( aledges->NbEdges )
  {
    //cmn_memreserve( edgvis, aledges->NbEdges, 0 );
    edgvis = new Tint[aledges->NbEdges];
    if( !edgvis )
      return;
    for( i = 0; i < aledges->NbEdges; i++ )
      edgvis[i] = aledges->Edges[i].Type ? CALL_PHIGS_EDGE_OFF :
    CALL_PHIGS_EDGE_ON;
    k[nk].id = EDGE_DATA_ID;
    k[nk].data.pdata = edgvis;
    nk++;
  }

  bounds = new Tint[albounds->NbIntegers];
  if( !bounds )
  {
    delete[] edgvis;
    return;
  }
  for( i = 0, num_indices = 0; i < albounds->NbIntegers; i++ )
  {
    bounds[i] = albounds->Integers[i];
    num_indices += bounds[i];
  }
  k[nk].id = BOUNDS_DATA_ID;
  k[nk].data.pdata = bounds;
  nk++;

  indices = new Tint[num_indices];
  if( !indices )
  {
    delete[] edgvis;
    delete[] bounds;
    return;
  }
  for( i = 0; i < num_indices; i++ )
    indices[i] = aledges->Edges[i].Index1;
  k[nk].id = CONNECTIVITY_ID;
  k[nk].data.pdata = indices;
  nk++;

  switch( alpoints->TypePoints )
  {
  case 1 : /* Vertex Coordinates Specified */
    varr = new TEL_POINT[alpoints->NbPoints];
    if( !varr )
    {
      delete[] edgvis;
      delete[] bounds;
      delete[] indices;
      return;
    }
    for( i = 0; i < alpoints->NbPoints; i++ )
      varr[i].xyz[0] = alpoints->UPoints.Points[i].x,
      varr[i].xyz[1] = alpoints->UPoints.Points[i].y,
      varr[i].xyz[2] = alpoints->UPoints.Points[i].z;
    k[nk].id = VERTICES_ID;
    k[nk].data.pdata = varr;
    nk++;
    break;

  case 2 : /* Coordinates and Vertex Normal Specified */
    varr = new TEL_POINT[alpoints->NbPoints];
    if( !varr )
    {
      delete[] edgvis;
      delete[] bounds;
      delete[] indices;
      return;
    }
    narr = new TEL_POINT[alpoints->NbPoints];
    if( !narr )
    {
      delete[] edgvis;
      delete[] bounds;
      delete[] indices;
      delete[] varr;
      return;
    }
    for( i = 0; i < alpoints->NbPoints; i++ )
    {
      varr[i].xyz[0] = alpoints->UPoints.PointsN[i].Point.x,
        varr[i].xyz[1] = alpoints->UPoints.PointsN[i].Point.y,
        varr[i].xyz[2] = alpoints->UPoints.PointsN[i].Point.z;

      narr[i].xyz[0] = alpoints->UPoints.PointsN[i].Normal.dx,
        narr[i].xyz[1] = alpoints->UPoints.PointsN[i].Normal.dy,
        narr[i].xyz[2] = alpoints->UPoints.PointsN[i].Normal.dz;
    }
    k[nk].id = VERTICES_ID;
    k[nk].data.pdata = varr;
    nk++;
    k[nk].id = VNORMALS_ID;
    k[nk].data.pdata = narr;
    nk++;
    break;

  case 3 : /* Coordinates and Vertex Colour Specified */
    varr = new TEL_POINT[alpoints->NbPoints];
    if( !varr )
    {
      delete[] edgvis;
      delete[] bounds;
      delete[] indices;
      return;
    }
    carr = new TEL_COLOUR[alpoints->NbPoints];
    if( !carr )
    {
      delete[] edgvis;
      delete[] bounds;
      delete[] indices;
      delete[] varr;
      return;
    }
    for( i = 0; i < alpoints->NbPoints; i++ )
    {
      varr[i].xyz[0] = alpoints->UPoints.PointsC[i].Point.x,
        varr[i].xyz[1] = alpoints->UPoints.PointsC[i].Point.y,
        varr[i].xyz[2] = alpoints->UPoints.PointsC[i].Point.z;

      carr[i].rgb[0] = alpoints->UPoints.PointsC[i].Color.r,
        carr[i].rgb[1] = alpoints->UPoints.PointsC[i].Color.g,
        carr[i].rgb[2] = alpoints->UPoints.PointsC[i].Color.b;
    }
    k[nk].id = VERTICES_ID;
    k[nk].data.pdata = varr;
    nk++;
    k[nk].id = VERTEX_COLOUR_VALS_ID;
    k[nk].data.pdata = carr;
    nk++;
    break;

  case 4 : /* Coordinates and Vertex Colour and Vertex Normal Specified */
    varr = new TEL_POINT[alpoints->NbPoints];
    if( !varr )
    {
      delete[] edgvis;
      delete[] bounds;
      delete[] indices;
      return;
    }
    carr = new TEL_COLOUR[alpoints->NbPoints];
    if( !carr )
    {
      delete[] edgvis;
      delete[] bounds;
      delete[] indices;
      delete[] varr;
      return;
    }
    narr = new TEL_POINT[alpoints->NbPoints];
    if( !narr )
    {
      delete[] edgvis;
      delete[] bounds;
      delete[] indices;
      delete[] varr;
      delete[] carr;
      return;
    }
    for( i = 0; i < alpoints->NbPoints; i++ )
    {
      varr[i].xyz[0] = alpoints->UPoints.PointsNC[i].Point.x,
        varr[i].xyz[1] = alpoints->UPoints.PointsNC[i].Point.y,
        varr[i].xyz[2] = alpoints->UPoints.PointsNC[i].Point.z;

      carr[i].rgb[0] = alpoints->UPoints.PointsNC[i].Color.r,
        carr[i].rgb[1] = alpoints->UPoints.PointsNC[i].Color.g,
        carr[i].rgb[2] = alpoints->UPoints.PointsNC[i].Color.b;

      narr[i].xyz[0] = alpoints->UPoints.PointsNC[i].Normal.dx,
        narr[i].xyz[1] = alpoints->UPoints.PointsNC[i].Normal.dy,
        narr[i].xyz[2] = alpoints->UPoints.PointsNC[i].Normal.dz;
    }
    k[nk].id = VERTICES_ID;
    k[nk].data.pdata = varr;
    nk++;
    k[nk].id = VERTEX_COLOUR_VALS_ID;
    k[nk].data.pdata = carr;
    nk++;
    k[nk].id = VNORMALS_ID;
    k[nk].data.pdata = narr;
    nk++;
    break;

  case 5 : /* Coordinates and Vertex Normal and Texture Coordinates specified */
    varr = new TEL_POINT[alpoints->NbPoints];
    if( !varr )
    {
      delete[] edgvis;
      delete[] bounds;
      delete[] indices;
      return;
    }
    narr = new TEL_POINT[alpoints->NbPoints];
    if( !narr )
    {
      delete[] edgvis;
      delete[] bounds;
      delete[] indices;
      delete[] varr;
      return;
    }
    tarr = new TEL_TEXTURE_COORD[alpoints->NbPoints];

    if( !tarr )
    {
      delete[] edgvis;
      delete[] bounds;
      delete[] indices;
      delete[] varr;
      delete[] narr;
      return;
    }
    for( i = 0; i < alpoints->NbPoints; i++ )
    {
      varr[i].xyz[0] = alpoints->UPoints.PointsNT[i].Point.x,
        varr[i].xyz[1] = alpoints->UPoints.PointsNT[i].Point.y,
        varr[i].xyz[2] = alpoints->UPoints.PointsNT[i].Point.z;

      narr[i].xyz[0] = alpoints->UPoints.PointsNT[i].Normal.dx,
        narr[i].xyz[1] = alpoints->UPoints.PointsNT[i].Normal.dy,
        narr[i].xyz[2] = alpoints->UPoints.PointsNT[i].Normal.dz;

      tarr[i].xy[0]  = alpoints->UPoints.PointsNT[i].TextureCoord.tx,
        tarr[i].xy[1]  = alpoints->UPoints.PointsNT[i].TextureCoord.ty;
    }
    k[nk].id = VERTICES_ID;
    k[nk].data.pdata = varr;
    nk++;
    k[nk].id = VNORMALS_ID;
    k[nk].data.pdata = narr;
    nk++;
    k[nk].id = VTEXTURECOORD_ID;
    k[nk].data.pdata = tarr;
    nk++;
    break;
  }

  for( i = 0; i < nk; i++ )
    kp[i] = &k[i];


  TsmAddToStructure( TelPolygonIndices, -nk, kp );

  delete[] edgvis;
  delete[] bounds;
  delete[] indices;
  delete[] varr;
  if( tarr )
    delete[] tarr;
  if( narr )
    delete[] narr;
  if( carr )
    delete[] carr;

  return;
}


void
call_subr_polygon_set( CALL_DEF_LISTFACETS * alfacets )
{
  CMN_KEY   key[3];
  register  Tint  i, j, k;

  Tint  num_bounds, *bounds;
  tel_point          points;

  num_bounds = alfacets->NbFacets;
  bounds = new  Tint[num_bounds];
  if( !bounds )
    return;

  for( i = 0, j = 0; i < num_bounds; i++ )
    j += alfacets->LFacets[i].NbPoints;

  points = new TEL_POINT[j];
  if( !points )
  {
    delete[] bounds;
    return;
  }

  for( i = 0, k = 0; i < num_bounds; i++ )
    for( j = 0; j < alfacets->LFacets[i].NbPoints; j++ )
    {
      points[k].xyz[0] = alfacets->LFacets[i].UPoints.Points[j].x;
      points[k].xyz[1] = alfacets->LFacets[i].UPoints.Points[j].y;
      points[k].xyz[2] = alfacets->LFacets[i].UPoints.Points[j].z;
      k++;
    }

    key[0].id = NUM_FACETS_ID;
    key[0].data.ldata = num_bounds;
    key[1].id = BOUNDS_DATA_ID;
    key[1].data.pdata = bounds;
    key[2].id = VERTICES_ID;
    key[2].data.pdata = points;
    TsmAddToStructure( TelPolygonSet, 3, &key[0], &key[1], &key[2] );

    delete[] bounds;
    delete[] points;

    return;
}


void
call_subr_quadrangle( CALL_DEF_QUAD * aquad )
{
  CMN_KEY       key[15];
  cmn_key       kp[15];
  Tint          nk = 0, i;
  tel_point     vertices, vnormals=0;
  tel_texture_coord vtexturecoord=0;

  /* key[nk].id = NUM_ROWS_ID; */
  key[nk].id = NUM_COLUMNS_ID;
  key[nk].data.ldata = aquad->SizeRow - 1;
  nk++;
  /* key[nk].id = NUM_COLUMNS_ID; */
  key[nk].id = NUM_ROWS_ID;
  key[nk].data.ldata = aquad->SizeCol - 1;
  nk++;

  //cmn_memreserve( vertices, aquad->NbPoints, 0 );
  vertices = new TEL_POINT[aquad->NbPoints];
  if( !vertices )
    return;

  switch( aquad->TypePoints )
  {
  case  1: /* Vertex Coordinates Specified */
    for( i = 0; i < aquad->NbPoints; i++ )
    {
      vertices[i].xyz[0] = aquad->UPoints.Points[i].x;
      vertices[i].xyz[1] = aquad->UPoints.Points[i].y;
      vertices[i].xyz[2] = aquad->UPoints.Points[i].z;
    }
    break;

  case  2: /* Coordinates and Vertex Normal Specified */
    //cmn_memreserve( vnormals, aquad->NbPoints, 0 );
    vnormals = new TEL_POINT[aquad->NbPoints];
    if( !vnormals )
    {
      //cmn_freemem( vertices );
      delete[] vertices;
      return;
    }
    for( i = 0; i < aquad->NbPoints; i++ )
    {
      vertices[i].xyz[0] = aquad->UPoints.PointsN[i].Point.x;
      vertices[i].xyz[1] = aquad->UPoints.PointsN[i].Point.y;
      vertices[i].xyz[2] = aquad->UPoints.PointsN[i].Point.z;
      vnormals[i].xyz[0] = aquad->UPoints.PointsN[i].Normal.dx;
      vnormals[i].xyz[1] = aquad->UPoints.PointsN[i].Normal.dy;
      vnormals[i].xyz[2] = aquad->UPoints.PointsN[i].Normal.dz;
    }
    key[nk].id = VNORMALS_ID;
    key[nk].data.pdata = vnormals;
    nk++;
    break;

  case  5: /* Coordinates and Vertex Normal and Texture Coordinate Specified */
    vnormals = new TEL_POINT[aquad->NbPoints];
    if( !vnormals )
    {
      delete[] vertices;
      return;
    }
    vtexturecoord = new TEL_TEXTURE_COORD[aquad->NbPoints];
    if (!vtexturecoord)
    {
      delete[] vnormals;
      delete[] vertices;
    }
    for( i = 0; i < aquad->NbPoints; i++ )
    {
      vertices[i].xyz[0] = aquad->UPoints.PointsNT[i].Point.x;
      vertices[i].xyz[1] = aquad->UPoints.PointsNT[i].Point.y;
      vertices[i].xyz[2] = aquad->UPoints.PointsNT[i].Point.z;
      vnormals[i].xyz[0] = aquad->UPoints.PointsNT[i].Normal.dx;
      vnormals[i].xyz[1] = aquad->UPoints.PointsNT[i].Normal.dy;
      vnormals[i].xyz[2] = aquad->UPoints.PointsNT[i].Normal.dz;
      vtexturecoord[i].xy[0] = aquad->UPoints.PointsNT[i].TextureCoord.tx;
      vtexturecoord[i].xy[1] = aquad->UPoints.PointsNT[i].TextureCoord.ty;
    }
    key[nk].id = VNORMALS_ID;
    key[nk].data.pdata = vnormals;
    nk++;
    key[nk].id = VTEXTURECOORD_ID;
    key[nk].data.pdata = vtexturecoord;
    nk++;
    break;
  }

  key[nk].id = VERTICES_ID;
  key[nk].data.pdata = vertices;
  nk++;

  for( i = 0; i < nk; i++ )
    kp[i] = &key[i];

  TsmAddToStructure( TelQuadrangle, -nk, kp );

  delete[] vertices;
  if (vtexturecoord)
    delete[] vtexturecoord;
  if( vnormals )
    delete[] vnormals;

  return;
}

void
call_subr_set_text_colr( Tfloat  r, Tfloat  g, Tfloat  b )
{
  CMN_KEY  key;
  TEL_COLOUR  col;

  col.rgb[0] = r,
    col.rgb[1] = g,
    col.rgb[2] = b;
  key.data.pdata = &col;
  TsmAddToStructure( TelTextColour, 1, &key );

  return;
}

void
call_subr_set_text_colr_subtitle( Tfloat  r, Tfloat  g, Tfloat  b )
{
  CMN_KEY  key;
  TEL_COLOUR  col;

  col.rgb[0] = r,
    col.rgb[1] = g,
    col.rgb[2] = b;
  key.data.pdata = &col;
  TsmAddToStructure( TelTextColourSubTitle, 1, &key );

  return;
}


void
call_subr_triangle( CALL_DEF_TRIKE * atrike )
{
  Tint    i;
  tel_point  vertices, vnormals=0;
  tel_texture_coord vtexturecoord=0;
  CMN_KEY    key[15];
  cmn_key    kp[15];
  Tint       nk = 0;

  vertices = new TEL_POINT[atrike->NbPoints];
  if( !vertices )
    return;

  switch( atrike->TypePoints )
  {
  case  1: /* Vertex Coordinates Specified */
    for( i = 0; i < atrike->NbPoints; i++ )
    {
      vertices[i].xyz[0] = atrike->UPoints.Points[i].x;
      vertices[i].xyz[1] = atrike->UPoints.Points[i].y;
      vertices[i].xyz[2] = atrike->UPoints.Points[i].z;
    }
    break;

  case  2: /* Coordinates and Vertex Normal Specified */
    vnormals = new TEL_POINT[atrike->NbPoints];
    if( !vnormals )
    {
      delete[] vertices; 
      return;
    }
    for( i = 0; i < atrike->NbPoints; i++ )
    {
      vertices[i].xyz[0] = atrike->UPoints.PointsN[i].Point.x;
      vertices[i].xyz[1] = atrike->UPoints.PointsN[i].Point.y;
      vertices[i].xyz[2] = atrike->UPoints.PointsN[i].Point.z;
      vnormals[i].xyz[0] = atrike->UPoints.PointsN[i].Normal.dx;
      vnormals[i].xyz[1] = atrike->UPoints.PointsN[i].Normal.dy;
      vnormals[i].xyz[2] = atrike->UPoints.PointsN[i].Normal.dz;
    }
    key[nk].id = VNORMALS_ID;
    key[nk].data.pdata = vnormals;
    nk++;
    break;
  case  5: /* Coordinates and Vertex Normal and Texture Coordinates Specified */
    vnormals = new TEL_POINT[atrike->NbPoints];
    if( !vnormals )
    {
      delete[] vertices;
      return;
    }
    vtexturecoord = new TEL_TEXTURE_COORD[atrike->NbPoints];
    if (!vtexturecoord)
    {
      delete[] vnormals;
      delete[] vertices;
    }
    for( i = 0; i < atrike->NbPoints; i++ )
    {
      vertices[i].xyz[0] = atrike->UPoints.PointsNT[i].Point.x;
      vertices[i].xyz[1] = atrike->UPoints.PointsNT[i].Point.y;
      vertices[i].xyz[2] = atrike->UPoints.PointsNT[i].Point.z;
      vnormals[i].xyz[0] = atrike->UPoints.PointsNT[i].Normal.dx;
      vnormals[i].xyz[1] = atrike->UPoints.PointsNT[i].Normal.dy;
      vnormals[i].xyz[2] = atrike->UPoints.PointsNT[i].Normal.dz;
      vtexturecoord[i].xy[0] = atrike->UPoints.PointsNT[i].TextureCoord.tx;
      vtexturecoord[i].xy[1] = atrike->UPoints.PointsNT[i].TextureCoord.ty;
    }
    key[nk].id = VNORMALS_ID;
    key[nk].data.pdata = vnormals;
    nk++;
    key[nk].id = VTEXTURECOORD_ID;
    key[nk].data.pdata = vtexturecoord;
    nk++;
    break;
  }

  key[nk].id = NUM_FACETS_ID;
  key[nk].data.ldata = atrike->NbPoints-2;
  nk++;

  key[nk].id = VERTICES_ID;
  key[nk].data.pdata = vertices;
  nk++;

  for( i = 0; i < nk; i++ )
    kp[i] = &key[i];

  TsmAddToStructure( TelTriangleMesh, -nk, kp );

  delete[] vertices;
  if( vnormals )
    delete[] vnormals;

  return;
}


void
call_subr_set_highl_rep( Tint ind )
{
  CMN_KEY  key;

  key.data.ldata = ind;
  TsmAddToStructure( TelHighlightIndex, 1, &key );

  return;
}


#define CALL_ 128

void
call_subr_structure_exploration( Tint             Id,
                                Tint             LabelBegin,
                                Tint             LabelEnd,
                                Tint             elem_num,
                                Tint            *elem_type,
                                CALL_DEF_POINT  *pt,
                                CALL_DEF_NORMAL *npt,
                                CALL_DEF_COLOR  *cpt,
                                CALL_DEF_NORMAL *nfa )
{
#define CALL_MAX_ELEMS_PRINT 1000000
  Tfloat  r=0,  g=0,  b=0;
  Tfloat  x=0,  y=0,  z=0;
  Tfloat nx=0, ny=0, nz=0;
  Tfloat dx=0, dy=0, dz=0;

  static Tint Initialisation = 1;

  Tchar  *buffer=NULL;
  Tint    actual_size; /* OUT content size */
  Tint    err=0;       /* OUT error indicator */
  TelType type;        /* OUT element type */
  Tint    size;        /* OUT element size */
  Tint    ws;          /* OUT workstation */

  Tint      index;        /* element pointer value */
  Tint      cur_index;    /* OUT element pointer value */
  Teldata   data;         /* OUT data record */

  register int i, j;
  Tint     MAXPOINTS;
  Tint     MAXAREAS;

  static Tint idata, jdata, kdata;

  Tint printall;      /* print all structure elements or not */
  Tint dump;          /* print or not */

  static Tint IdCur;  /* current structure id
                      (for exploration step by step) */
  static Tint ElCur;  /* current structure element number
                      (for exploration step by step) */
  static Tint IndCur; /* current index point in the structure element
                      (for exploration step by step) */
  static Tint JndCur; /* current index element in the structure element
                      (for exploration step by step) */
  static Tint EndExplo;/* flag when the exploration of the current
                       element is done.
                       (for exploration step by step) */

  if( Initialisation )
  {
    Initialisation =  0;
    IdCur          = -1;
    ElCur          = -1;
    IndCur         = -1;
    JndCur         =  0;
  }

  dump      = 0;
  printall  = 1;
  MAXPOINTS = CALL_MAX_ELEMS_PRINT;
  MAXAREAS  = CALL_MAX_ELEMS_PRINT;

  if( (LabelBegin == 0) && (LabelEnd == 0) )
  {
    dump     = 1;
    printall = 0;
  }

  if( elem_num )
  {
    index    = elem_num;
    dump     = 1;
    printall = 1;

    npt->dx = npt->dy = npt->dz = nx = ny = nz = ( float )0.0;
    nfa->dx = nfa->dy = nfa->dz = dx = dy = dz = ( float )0.0;
    pt->x = pt->y = pt->z = x = y = z = ( float )0.0;
    cpt->r = cpt->g = cpt->b = r = g = b = ( float )0.0;

    if ( (Id != IdCur) || (ElCur != elem_num) || EndExplo )
    {
      IdCur  = Id;
      ElCur  = elem_num;
      IndCur = -1;
      JndCur = 0;
    }
    else
    {
      IndCur ++;
    }
  }
  else
  {
    index  = 1;
    IdCur  = -1;
    ElCur  = -1;
    IndCur = -1;
    JndCur = 0;
  }

  call_func_open_struct (Id);

  while( index )
  {
    call_func_set_elem_ptr( index );
    if( call_func_inq_elem_ptr( &cur_index ) == TFailure )
      err = 1;
    else
      err = 0;

    /* it's the end */
    if( ( index != cur_index ) || err )
    {
      index = 0;
      break;
    }
    if( TelInqCurElemTypeSize( &type, &size ) == TFailure )
      err = 1;
    else
      err = 0;

    if( err || (type == CALL_PHIGS_ELEM_NIL) )
    {
      index = 0;
      break;
    }

    if( elem_num )
    {
      EndExplo   = 0;
      *elem_type = type;
    }

    if( type == CALL_PHIGS_ELEM_LABEL )
    {
      buffer = (Tchar *)malloc( CALL_ );
      TelInqCurElemContent( CALL_, buffer, &actual_size, &data );

      if( data.idata == LabelBegin )
        dump = 1;
      if( ( LabelBegin == 0 ) && ( LabelEnd == 0 ) )
        dump = 1;
      if( dump )
      {
        if( elem_num )
        {
          if( IndCur >= 1 )
          {
            EndExplo = 1;
          }
          else
          {
            x = ( float )data.idata;
            y = ( float )0.0;
            z = ( float )0.0;
          }
        }
        else
        {
          printf( "\tELEM_LABEL\n" );
          printf( "\t\tLABEL %d\n", data.idata );
        }
      }

      if( data.idata == LabelEnd )
        dump = 0;

      free( buffer );
    }

    if( dump )
    {
      switch (type)
      {
      case CALL_PHIGS_ELEM_NIL :
        {
          if( elem_num )
          {
            if (IndCur >= 0)
            {
              EndExplo = 1;
            }
            else
            {
              x = y = z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_NIL\n");
            printf("\t\tNo data involved\n");
          }
          break;
        }
      case CALL_PHIGS_ELEM_POLYMARKER3 :
        {
          i = size;
          buffer = new Tchar[i];
          TelInqCurElemContent( i, buffer, &actual_size, &data );
          idata = size/sizeof( TEL_POINT );
          if( elem_num )
          {
            if( (IndCur > -1) && (IndCur < idata) )
            {
              i = IndCur;
              x = data.pts3[i].xyz[0];
              y = data.pts3[i].xyz[1];
              z = data.pts3[i].xyz[2];
            }
            if (IndCur >= idata)
              EndExplo = 1;
          }
          else
          {
            printf("\tELEM_POLYMARKER3\n");
            printf("\t\tPOLYMARKER 3 nb pts %d\n", idata);
            if (printall)
            {
              for (i=0; i<idata && i<MAXPOINTS; i++)
                printf( "\t\tpoint %d : %f %f %f\n", i,
                data.pts3[i].xyz[0],
                data.pts3[i].xyz[1],
                data.pts3[i].xyz[2] );
            }
            else
            {
              i = 0;
              printf( "\t\tfirst point %d : %f %f %f\n", i,
                data.pts3[i].xyz[0],
                data.pts3[i].xyz[1],
                data.pts3[i].xyz[2] );
              i = idata - 1;
              printf("\t\tlast point %d : %f %f %f\n", i,
                data.pts3[i].xyz[0],
                data.pts3[i].xyz[1],
                data.pts3[i].xyz[2]);
            }
          }
          delete[] buffer;
          break;
        }
      case CALL_PHIGS_ELEM_FILL_AREA_SET3 :
        {
          i = size;
          buffer = new Tchar[i];
          TelInqCurElemContent( i, buffer, &actual_size, &data );
          jdata = data.fillareaset3data.num_bounds;
          if( elem_num )
          {
            if ((IndCur == 0) && (JndCur == 0))
              kdata = 0;
            if (JndCur >= jdata)
              EndExplo = 1;
            else
            {
              if (IndCur == 0)
              {
                idata = data.fillareaset3data.bounds[JndCur];
              }
              else
              {
                if ((IndCur > -1) && (IndCur < idata))
                {
                  x =
                    data.fillareaset3data.points[kdata+IndCur].xyz[0];
                  y =
                    data.fillareaset3data.points[kdata+IndCur].xyz[1];
                  z =
                    data.fillareaset3data.points[kdata+IndCur].xyz[2];

                  if(data.fillareaset3data.vrtflag ==
                    CALL_PHIGS_VERT_COORD_NORMAL)
                  {
                    nx = data.fillareaset3data.vnormals[IndCur].xyz[0];
                    ny = data.fillareaset3data.vnormals[IndCur].xyz[1];
                    nz = data.fillareaset3data.vnormals[IndCur].xyz[2];
                  }
                  if(data.fillareaset3data.vrtflag ==
                    CALL_PHIGS_VERT_COORD_COLOUR)
                  {
                    r =
                      data.fillareaset3data.colours[kdata+IndCur].rgb[0];
                    g =
                      data.fillareaset3data.colours[kdata+IndCur].rgb[1];
                    b =
                      data.fillareaset3data.colours[kdata+IndCur].rgb[2];
                  }
                  if (data.fillareaset3data.vrtflag ==
                    CALL_PHIGS_VERT_COORD_COLOUR_NORMAL)
                  {
                    nx = data.fillareaset3data.vnormals[IndCur].xyz[0];
                    ny = data.fillareaset3data.vnormals[IndCur].xyz[1];
                    nz = data.fillareaset3data.vnormals[IndCur].xyz[2];
                    r =
                      data.fillareaset3data.colours[kdata+IndCur].rgb[0];
                    g =
                      data.fillareaset3data.colours[kdata+IndCur].rgb[1];
                    b =
                      data.fillareaset3data.colours[kdata+IndCur].rgb[2];
                  }
                }
              }
              if (IndCur >= idata)
              {
                kdata += data.fillareaset3data.bounds[JndCur];
                JndCur ++;
                IndCur  = 0;
              }
            }
          }
          else
          {
            printf("\tELEM_FILL_AREA_SET3_DATA\n");
            printf("\t\tSET OF FILL AREA SET 3 WITH DATA\n");
            kdata = 0;
            printf("\t\tnb of faces %d\n", jdata);
            printf("\t\tfflag %d eflag %d vflag %d colr model RGB\n",
              data.fillareaset3data.faflag,
              data.fillareaset3data.edgflag,
              data.fillareaset3data.vrtflag );
            kdata = 0;
            for (j=0; j<jdata && j<MAXAREAS; j++)
            {
              idata = data.fillareaset3data.bounds[j];
              printf("\t\tface %d : %d vertices\n", j, idata);
              if (data.fillareaset3data.faflag ==
                CALL_PHIGS_FACET_NORMAL)
                printf("\t\t\tface normal %f %f %f\n",
                data.fillareaset3data.gnormal[j].xyz[0],
                data.fillareaset3data.gnormal[j].xyz[1],
                data.fillareaset3data.gnormal[j].xyz[2]);
              if (data.fillareaset3data.faflag ==
                CALL_PHIGS_FACET_COLOUR_NORMAL)
                printf("\t\t\tnormal %f %f %f\n",
                data.fillareaset3data.gnormal[j].xyz[0],
                data.fillareaset3data.gnormal[j].xyz[1],
                data.fillareaset3data.gnormal[j].xyz[2]);
              if (printall)
                for (i=0; i<idata && i<MAXPOINTS; i++)
                  printf("\t\t\tvertex %d : %f %f %f\n", i,
                  data.fillareaset3data.points[kdata+i].xyz[0],
                  data.fillareaset3data.points[kdata+i].xyz[1],
                  data.fillareaset3data.points[kdata+i].xyz[2]);
              else
              {
                i = 0;
                printf("\t\tfirst vertex %d : %f %f %f\n", i,
                  data.fillareaset3data.points[kdata+i].xyz[0],
                  data.fillareaset3data.points[kdata+i].xyz[1],
                  data.fillareaset3data.points[kdata+i].xyz[2]);

                i = idata - 1;
                printf("\t\tlast vertex %d : %f %f %f\n", i,
                  data.fillareaset3data.points[kdata+i].xyz[0],
                  data.fillareaset3data.points[kdata+i].xyz[1],
                  data.fillareaset3data.points[kdata+i].xyz[2]);
              }
              kdata += data.fillareaset3data.bounds[j];
            }
            delete[] buffer;
            break;
          }
        }
      case CALL_PHIGS_ELEM_FILL_AREA3 :
        {
          i = size;
          buffer = new Tchar[i];
          TelInqCurElemContent( i, buffer, &actual_size, &data );
          idata = data.fillarea3data.num_points;

          if( elem_num )
          {
            if( (IndCur > -1) && (IndCur < idata) )
            {
              i = IndCur;
              x = data.fillarea3data.points[i].xyz[0];
              y = data.fillarea3data.points[i].xyz[1];
              z = data.fillarea3data.points[i].xyz[2];
            }
            if( IndCur >= idata )
              EndExplo = 1;
          }
          else
          {
            printf( "\tELEM_FILL_AREA3\n" );
            printf( "\t\tFILL AREA 3 nb pts %d\n", idata );
            if( printall )
            {
              for( i=0; i<idata && i<MAXPOINTS; i++ )
                printf( "\t\tpoint %d : %f %f %f\n", i,
                data.fillarea3data.points[i].xyz[0],
                data.fillarea3data.points[i].xyz[1],
                data.fillarea3data.points[i].xyz[2] );
            }
            else
            {
              i = 0;
              printf("\t\tfirst point %d : %f %f %f\n", i,
                data.fillarea3data.points[i].xyz[0],
                data.fillarea3data.points[i].xyz[1],
                data.fillarea3data.points[i].xyz[2] );
              i = idata - 1;
              printf("\t\tlast point %d : %f %f %f\n", i,
                data.fillarea3data.points[i].xyz[0],
                data.fillarea3data.points[i].xyz[1],
                data.fillarea3data.points[i].xyz[2] );
            }
          }
          delete[] buffer;
          break;
        }
      case CALL_PHIGS_ELEM_ANNO_TEXT_REL3 :
        {
          buffer = new Tchar[size];
          TelInqCurElemContent( size, buffer, &actual_size, &data );

          if( elem_num )
          {
            if (IndCur == 0)
            {
              x = data.atext3.ref_pt.xyz[0];
              y = data.atext3.ref_pt.xyz[1];
              z = data.atext3.ref_pt.xyz[2];
            }
            if (IndCur == 1)
            {
              x = data.atext3.anno.xyz[0];
              y = data.atext3.anno.xyz[1];
              z = data.atext3.anno.xyz[2];
            }
            if( IndCur >= 2 )
              EndExplo = 1;
          }
          else
          {
            printf( "\tELEM_ANNO_TEXT_REL3\n" );
            printf( "\t\tANNOTATION TEXT RELATIVE 3 %s\n",
              data.atext3.string );
            printf( "\t\treference point %f %f %f\n",
              data.atext3.ref_pt.xyz[0],
              data.atext3.ref_pt.xyz[1],
              data.atext3.ref_pt.xyz[2] );
            printf( "\t\toffset %f %f %f\n",
              data.atext3.anno.xyz[0],
              data.atext3.anno.xyz[1],
              data.atext3.anno.xyz[2] );
          }
          delete[] buffer;
          break;
        }
      case CALL_PHIGS_ELEM_LINETYPE :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = ( float )data.idata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else {
            printf("\tELEM_LINETYPE\n");
            printf("\t\tLINETYPE %d\n", data.idata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_LINEWIDTH :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = data.fdata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf( "\tELEM_LINEWIDTH\n" );
            printf( "\t\tLINEWIDTH SCALE FACTOR %f\n", data.fdata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_MARKER_TYPE :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if( IndCur >= 1 )
              EndExplo = 1;
            else
            {
              x = ( float )data.idata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_MARKER_TYPE\n");
            printf("\t\tMARKER TYPE %d\n", data.idata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_MARKER_SIZE :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = data.fdata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_MARKER_SIZE\n");
            printf("\t\tMARKER SIZE SCALE FACTOR %f\n", data.fdata);
          }
          break;
        }
#if 0
      case CALL_PHIGS_ELEM_TEXT_FONT : /* DECPHIGS */
        buffer = new Tchar[CALL_];
        TelInqCurElemContent (CALL_, buffer, &actual_size, &data);
        if( elem_num )
          if (IndCur >= 1)
            EndExplo        = 1;
          else {
            x        = data.idata;
            y        = 0.0;
            z        = 0.0;
          }
        else {
          printf("\tELEM_TEXT_FONT\n");
          printf("\t\tTEXT FONT %d\n", data.idata);
        }
        delete[] buffer;
        break;
#endif
      case CALL_PHIGS_ELEM_CHAR_EXPAN :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = data.fdata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_CHAR_EXPAN\n");
            printf("\t\tCHARACTER EXPANSION FACTOR %f\n",data.fdata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_CHAR_SPACE :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = data.fdata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_CHAR_SPACE\n");
            printf("\t\tCHARACTER SPACING %f\n", data.fdata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_ANNO_CHAR_HT :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = data.fdata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_ANNO_CHAR_HT\n");
            printf("\t\tANNOTATION TEXT CHARACTER HEIGHT %f\n",
              data.fdata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_INT_STYLE :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = ( float )data.interior_style;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_INT_STYLE\n");
            printf("\t\tINTERIOR STYLE %d\n", data.interior_style);
          }
          break;
        }
      case CALL_PHIGS_ELEM_INT_STYLE_IND :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = ( float )data.idata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_INT_STYLE_IND\n");
            printf("\t\tINTERIOR STYLE INDEX %d\n", data.idata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_EDGE_FLAG :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = ( float )data.edge_flag;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_EDGE_FLAG\n");
            printf("\t\tEDGE FLAG %d\n", data.edge_flag);
          }
          break;
        }
      case CALL_PHIGS_ELEM_EDGETYPE :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = ( float )data.idata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_EDGETYPE\n");
            printf("\t\tEDGETYPE %d\n", data.idata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_EDGEWIDTH :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = data.fdata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_EDGEWIDTH\n");
            printf("\t\tEDGEWIDTH SCALE FACTOR %f\n", data.fdata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_ADD_NAMES_SET :
        {
          buffer = new Tchar[size];
          TelInqCurElemContent( size, buffer, &actual_size, &data );
          idata = data.name_set.number;
          if( elem_num )
          {
            if( (IndCur > -1) && (IndCur < idata) )
            {
              i = IndCur;
              x = ( float )data.name_set.integers[i];
              y = ( float )0.0;
              z = ( float )0.0;
            }
            if( IndCur >= idata )
              EndExplo = 1;
          }
          else
          {
            printf("\tELEM_ADD_NAMES_SET\n");
            printf("\t\tADD NAMES TO SET\n");
            for( i=0; i<idata; i++ )
              printf("\t\tnames %d : %d\n",
              i, data.name_set.integers[i]);
          }
          delete[] buffer;
          break;
        }
      case CALL_PHIGS_ELEM_REMOVE_NAMES_SET :
        {
          buffer = new Tchar[size];
          TelInqCurElemContent( size, buffer, &actual_size, &data );
          idata = data.name_set.number;
          if( elem_num )
          {
            if( (IndCur > -1) && (IndCur < idata) )
            {
              i = IndCur;
              x = ( float )data.name_set.integers[i];
              y = ( float )0.0;
              z = ( float )0.0;
            }
            if (IndCur >= idata)
              EndExplo = 1;
          }
          else
          {
            printf("\tELEM_REMOVE_NAMES_SET\n");
            printf("\t\tREMOVE NAMES FROM SET\n");
            for( i=0; i<idata; i++ )
              printf("\t\tnames %d : %d\n",
              i, data.name_set.integers[i]);
          }
          delete[] buffer;
          break;
        }
      case CALL_PHIGS_ELEM_LOCAL_MODEL_TRAN3 :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 0)
              EndExplo = 1;
            else
            {
              x = y = z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_LOCAL_MODEL_TRAN3\n");
            printf("\t\tLOCAL MODELLING TRANSFORMATION 3 comp %d\n",
              data.local_xform_3.compose_type);
            for( i=0; i<4; i++ )
            {
              printf("\t\t\t");
              for( j=0; j<4; j++ )
                printf(" %f", data.local_xform_3.mat3[i][j]);
              printf("\n");
            }
          }
          break;
        }
      case CALL_PHIGS_ELEM_VIEW_IND :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = ( float )data.idata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_VIEW_IND\n");
            printf("\t\tVIEW INDEX %d\n", data.idata);

            if(! call_subr_inquirews (Id, &ws))
              printf("\t\tERROR : no view definition\n");
            else
            {
              printf("\t\tview structure in %d workstation\n", ws);
              printf("\t\t**********************\n");
              printf("\t\tdefinition in the ws %d\n", ws);
              printf("\t\t**********************\n");
              TelPrintViewRepresentation( ws, Id );
            }
          }
          break;
        }
      case CALL_PHIGS_ELEM_EXEC_STRUCT :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = ( float )data.idata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_EXEC_STRUCT\n");
            printf("\t\tEXECUTE STRUCTURE %d\n", data.idata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_APPL_DATA :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 0)
              EndExplo = 1;
            else
            {
              x = y = z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_APPL_DATA\n");
            printf("\t\tcontent not exploited\n");
          }
          break;
        }
      case CALL_PHIGS_ELEM_PICK_ID :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = ( float )data.idata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_PICK_ID\n");
            printf("\t\tPICK ID %d\n", data.idata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_POLYLINE_SET3_DATA : /* DECPHIGS */
        {
          buffer = new Tchar[size];
          TelInqCurElemContent( size, buffer, &actual_size, &data);
          jdata = data.lineset3data.num_bounds;

          if( elem_num )
          {
            if( ( IndCur == -1 ) && ( JndCur == 0 ) )
              kdata = 0;

            if( JndCur >= jdata )
              EndExplo = 1;
            else
            {
              if (IndCur == 0)
              {
                idata = data.lineset3data.bounds[JndCur];
              }
              else
              {
                if ( ( IndCur > -1 ) && ( IndCur < idata ) )
                {
                  x = data.lineset3data.points[kdata+IndCur].xyz[0];
                  y = data.lineset3data.points[kdata+IndCur].xyz[1];
                  z = data.lineset3data.points[kdata+IndCur].xyz[2];

                  if( data.lineset3data.colours )
                  {
                    r =
                      data.lineset3data.colours[kdata+IndCur].rgb[0];
                    g =
                      data.lineset3data.colours[kdata+IndCur].rgb[1];
                    b =
                      data.lineset3data.colours[kdata+IndCur].rgb[2];
                  }
                }

                if( IndCur >= idata )
                {
                  kdata += data.lineset3data.bounds[JndCur];
                  JndCur ++;
                  IndCur = 0;
                }
              }
            }
          }
          else
          {
            printf( "\tELEM_POLYLINE_SET3_DATA\n" );
            kdata = 0;
            printf( "\t\tPOLYLINE SET 3 WITH DATA\n" );
            printf( "\t\tnb of lines %d\n", jdata );
            printf( "\t\tvflag %d colr model RGB\n",
              data.lineset3data.vrtflag );
            for( j = 0; j < jdata && j < MAXAREAS; j++ )
            {
              idata = data.lineset3data.bounds[j];
              printf( "\t\tface %d : %d vertices\n", j, idata );
              if( printall )
              {
                for( i = 0; i < idata && i < MAXPOINTS; i++ )
                  printf( "\t\t\tvertex %d : %f %f %f\n", i,
                  data.lineset3data.points[kdata+i].xyz[0],
                  data.lineset3data.points[kdata+i].xyz[1],
                  data.lineset3data.points[kdata+i].xyz[2] );
              }
              else
              {
                i = 0;
                printf( "\t\tfirst vertex %d : %f %f %f\n", i,
                  data.lineset3data.points[kdata+i].xyz[0],
                  data.lineset3data.points[kdata+i].xyz[1],
                  data.lineset3data.points[kdata+i].xyz[2] );

                i = idata - 1;
                printf( "\t\tlast vertex %d : %f %f %f\n", i,
                  data.lineset3data.points[kdata+i].xyz[0],
                  data.lineset3data.points[kdata+i].xyz[1],
                  data.lineset3data.points[kdata+i].xyz[2] );
              }
              kdata += data.lineset3data.bounds[j];
            }
          }
          delete[] buffer;
          break;
        }
      case CALL_PHIGS_ELEM_TRI_STRIP3_DATA :
        {
          buffer = new Tchar[size];
          TelInqCurElemContent( size, buffer, &actual_size, &data );
          idata = data.trianglestrip3data.num_facets + 2;

          if( elem_num )
          {
            if (IndCur >= idata)
              EndExplo = 1;
            else
            {
              x = data.trianglestrip3data.points[IndCur].xyz[0];
              y = data.trianglestrip3data.points[IndCur].xyz[1];
              z = data.trianglestrip3data.points[IndCur].xyz[2];

              if( data.trianglestrip3data.vrtflag ==
                CALL_PHIGS_VERT_COORD_COLOUR )
              {
                r = data.trianglestrip3data.colours[IndCur].rgb[0];
                g = data.trianglestrip3data.colours[IndCur].rgb[1];
                b = data.trianglestrip3data.colours[IndCur].rgb[2];
              }
              else if( data.trianglestrip3data.vrtflag ==
                CALL_PHIGS_VERT_COORD_NORMAL )
              {
                nx =
                  data.trianglestrip3data.vnormals[IndCur].xyz[0];
                ny =
                  data.trianglestrip3data.vnormals[IndCur].xyz[1];
                nz =
                  data.trianglestrip3data.vnormals[IndCur].xyz[2];
              }
              else if( data.trianglestrip3data.vrtflag ==
                CALL_PHIGS_VERT_COORD_COLOUR_NORMAL )
              {
                nx =
                  data.trianglestrip3data.vnormals[IndCur].xyz[0];
                ny =
                  data.trianglestrip3data.vnormals[IndCur].xyz[1];
                nz =
                  data.trianglestrip3data.vnormals[IndCur].xyz[2];
                r = data.trianglestrip3data.colours[IndCur].rgb[0];
                g = data.trianglestrip3data.colours[IndCur].rgb[1];
                b = data.trianglestrip3data.colours[IndCur].rgb[2];
              }
            }
          }
          else
          {
            printf("\tELEM_TRI_STRIP3_DATA\n");
            printf("\t\tTRIANGLE STRIP 3 WITH DATA\n");
            printf("\t\tnb of vertices %d\n", idata);
            printf("\t\tfflag %d vflag %d colr model RGB\n",
              data.trianglestrip3data.fctflag,
              data.trianglestrip3data.vrtflag);
            if( printall )
              for( i=0; i<idata && i<MAXPOINTS; i++ )
                printf("\t\t\tvertex %d : %f %f %f\n", i,
                data.trianglestrip3data.points[i].xyz[0],
                data.trianglestrip3data.points[i].xyz[1],
                data.trianglestrip3data.points[i].xyz[2] );
            else
            {
              i = 0;
              printf("\t\tfirst vertex %d : %f %f %f\n", i,
                data.trianglestrip3data.points[i].xyz[0],
                data.trianglestrip3data.points[i].xyz[1],
                data.trianglestrip3data.points[i].xyz[2]);

              i = idata - 1;
              printf("\t\tlast vertex %d : %f %f %f\n", i,
                data.trianglestrip3data.points[i].xyz[0],
                data.trianglestrip3data.points[i].xyz[1],
                data.trianglestrip3data.points[i].xyz[2]);
            }
          }
          delete buffer;
          break;
        }
      case CALL_PHIGS_ELEM_QUAD_MESH3_DATA :
        {
          buffer = new Tchar[size];
          TelInqCurElemContent( size, buffer, &actual_size, &data);
          idata = (data.quadmesh3data.dim.x_dim+1) *
            (data.quadmesh3data.dim.y_dim+1);

          if( elem_num )
          {
            if (IndCur >= idata)
              EndExplo = 1;
            else
            {
              x = data.quadmesh3data.points[IndCur].xyz[0];
              y = data.quadmesh3data.points[IndCur].xyz[1];
              z = data.quadmesh3data.points[IndCur].xyz[2];

              if( data.quadmesh3data.vrtflag ==
                CALL_PHIGS_VERT_COORD_COLOUR )
              {
                r = data.quadmesh3data.colours[IndCur].rgb[0];
                g = data.quadmesh3data.colours[IndCur].rgb[1];
                b = data.quadmesh3data.colours[IndCur].rgb[2];
              }
              else if( data.quadmesh3data.vrtflag ==
                CALL_PHIGS_VERT_COORD_NORMAL )
              {
                nx = data.quadmesh3data.vnormals[IndCur].xyz[0];
                ny = data.quadmesh3data.vnormals[IndCur].xyz[1];
                nz = data.quadmesh3data.vnormals[IndCur].xyz[2];
              }
              else if( data.quadmesh3data.vrtflag ==
                CALL_PHIGS_VERT_COORD_COLOUR_NORMAL )
              {
                nx = data.quadmesh3data.vnormals[IndCur].xyz[0];
                ny = data.quadmesh3data.vnormals[IndCur].xyz[1];
                nz = data.quadmesh3data.vnormals[IndCur].xyz[2];
                r = data.quadmesh3data.colours[IndCur].rgb[0];
                g = data.quadmesh3data.colours[IndCur].rgb[1];
                b = data.quadmesh3data.colours[IndCur].rgb[2];
              }
            }
          }
          else
          {
            printf("\tELEM_QUAD_MESH3_DATA\n");
            printf("\t\tQUADRILATERAL MESH 3 WITH DATA\n");
            printf("\t\tdimensions %d %d\n",
              data.quadmesh3data.dim.x_dim,
              data.quadmesh3data.dim.y_dim);
            printf("\t\tnb of vertices %d\n", idata);
            printf("\t\tfflag %d vflag %d colr model RGB\n",
              data.quadmesh3data.fctflag,
              data.quadmesh3data.vrtflag);
            if( printall )
            {
              for( i=0; i<idata && i<MAXPOINTS; i++ )
                printf("\t\t\tvertex %d : %f %f %f\n", i,
                data.quadmesh3data.points[i].xyz[0],
                data.quadmesh3data.points[i].xyz[1],
                data.quadmesh3data.points[i].xyz[2]);
            }
            else
            {
              i = 0;
              printf("\t\tfirst vertex %d : %f %f %f\n", i,
                data.quadmesh3data.points[i].xyz[0],
                data.quadmesh3data.points[i].xyz[1],
                data.quadmesh3data.points[i].xyz[2]);

              i = idata - 1;
              printf("\t\tlast vertex %d : %f %f %f\n", i,
                data.quadmesh3data.points[i].xyz[0],
                data.quadmesh3data.points[i].xyz[1],
                data.quadmesh3data.points[i].xyz[2]);
            }
          }
          delete[] buffer;
          break;
        }
      case CALL_PHIGS_ELEM_TEXT_COLR :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = data.gnl_colour.rgb[0];
              y = data.gnl_colour.rgb[1];
              z = data.gnl_colour.rgb[2];
            }
          }
          else
          {
            printf("\tELEM_TEXT_COLR\n");
            printf("\t\tTEXT COLOUR r %f g %f b %f\n",
              data.gnl_colour.rgb[0],
              data.gnl_colour.rgb[1],
              data.gnl_colour.rgb[2] );
          }
          break;
        }
      case CALL_PHIGS_ELEM_MARKER_COLR :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = data.gnl_colour.rgb[0];
              y = data.gnl_colour.rgb[1];
              z = data.gnl_colour.rgb[2];
            }
          }
          else
          {
            printf("\tELEM_MARKER_COLR\n");
            printf("\t\tPOLYMARKER COLOUR r %f g %f b %f\n",
              data.gnl_colour.rgb[0],
              data.gnl_colour.rgb[1],
              data.gnl_colour.rgb[2] );
          }
          break;
        }
      case CALL_PHIGS_ELEM_EDGE_COLR :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = data.gnl_colour.rgb[0];
              y = data.gnl_colour.rgb[1];
              z = data.gnl_colour.rgb[2];
            }
          }
          else
          {
            printf("\tELEM_EDGE_COLR\n");
            printf("\t\tEDGE COLOUR r %f g %f b %f\n",
              data.gnl_colour.rgb[0],
              data.gnl_colour.rgb[1],
              data.gnl_colour.rgb[2]);
          }
          break;
        }
      case CALL_PHIGS_ELEM_LINE_COLR :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = data.gnl_colour.rgb[0];
              y = data.gnl_colour.rgb[1];
              z = data.gnl_colour.rgb[2];
            }
          }
          else
          {
            printf("\tELEM_LINE_COLR\n");
            printf("\t\tPOLYLINE COLOUR r %f g %f b %f\n",
              data.gnl_colour.rgb[0],
              data.gnl_colour.rgb[1],
              data.gnl_colour.rgb[2]);
          }
          break;
        }
      case CALL_PHIGS_ELEM_INT_COLR :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = data.gnl_colour.rgb[0];
              y = data.gnl_colour.rgb[1];
              z = data.gnl_colour.rgb[2];
            }
          }
          else
          {
            printf("\tELEM_INT_COLR\n");
            printf("\t\tINTERIOR COLOUR r %f g %f b %f\n",
              data.gnl_colour.rgb[0],
              data.gnl_colour.rgb[1],
              data.gnl_colour.rgb[2]);
          }
          break;
        }
      case CALL_PHIGS_ELEM_BACK_INT_COLR :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = data.gnl_colour.rgb[0];
              y = data.gnl_colour.rgb[1];
              z = data.gnl_colour.rgb[2];
            }
          }
          else
          {
            printf("\tELEM_BACK_INT_COLR\n");
            printf("\t\tBACK INTERIOR COLOUR r %f g %f b %f\n",
              data.gnl_colour.rgb[0],
              data.gnl_colour.rgb[1],
              data.gnl_colour.rgb[2]);
          }
          break;
        }
      case CALL_PHIGS_ELEM_REFL_PROPS :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur == 0)
            {
              x = data.surf_prop.amb;
              y = data.surf_prop.diff;
              z = data.surf_prop.trans;
            }
            if (IndCur == 1)
            {
              x = data.surf_prop.speccol.rgb[0];
              y = data.surf_prop.speccol.rgb[1];
              z = data.surf_prop.speccol.rgb[2];
            }
            if (IndCur == 2)
            {
              x = data.surf_prop.spec;
              y = data.surf_prop.shine;
              z = ( float )0.0;
            }
            if (IndCur >= 3)
              EndExplo = 1;
          }
          else
          {
            printf("\tELEM_REFL_PROPS\n");
            printf("\t\tREFLECTANCE PROPERTIES\n");
            printf("\t\tamb coef %f diff coef %f trans coef %f\n",
              data.surf_prop.amb,
              data.surf_prop.diff,
              data.surf_prop.trans);
            printf("\t\tspecular color r %f g %f b %f\n",
              data.surf_prop.speccol.rgb[0],
              data.surf_prop.speccol.rgb[1],
              data.surf_prop.speccol.rgb[2]);
            printf("\t\tspecular coef %f exp %f\n",
              data.surf_prop.spec,
              data.surf_prop.shine);
          }
          break;
        }
      case CALL_PHIGS_ELEM_BACK_REFL_PROPS :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur == 0) {
              x = data.surf_prop.amb;
              y = data.surf_prop.diff;
              z = data.surf_prop.trans;
            }
            if (IndCur == 1) {
              x = data.surf_prop.speccol.rgb[0];
              y = data.surf_prop.speccol.rgb[1];
              z = data.surf_prop.speccol.rgb[2];
            }
            if (IndCur == 2) {
              x = data.surf_prop.spec;
              y = data.surf_prop.shine;
              z = ( float )0.0;
            }
            if (IndCur >= 3)
              EndExplo = 1;
          }
          else
          {
            printf("\tELEM_BACK_REFL_PROPS\n");
            printf("\t\tBACK REFLECTANCE PROPERTIES\n");
            printf("\t\tamb coef %f diff coef %f trans coef %f\n",
              data.surf_prop.amb,
              data.surf_prop.diff,
              data.surf_prop.trans);
            printf("\t\tspecular color r %f g %f b %f\n",
              data.surf_prop.speccol.rgb[0],
              data.surf_prop.speccol.rgb[1],
              data.surf_prop.speccol.rgb[2]);
            printf("\t\tspecular coef %f exp %f\n",
              data.surf_prop.spec,
              data.surf_prop.shine);
          }
          break;
        }
      case CALL_PHIGS_ELEM_INT_SHAD_METH :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = ( float )data.idata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_INT_SHAD_METH\n");
            printf("\t\tINTERIOR SHADING METHOD %d\n", data.idata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_INT_REFL_EQN :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = ( float )data.idata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_INT_REFL_EQN\n");
            printf("\t\tREFLECTANCE EQUATION %d\n", data.idata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_BACK_INT_REFL_EQN :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = ( float )data.idata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_BACK_INT_REFL_EQN\n");
            printf("\t\tBACK INTERIOR REFLECTANCE EQUATION %d\n",
              data.idata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_FACE_DISTING_MODE :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = ( float )data.idata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_FACE_DISTING_MODE\n");
            printf("\t\tFACE DISTINGUISHING MODE %d\n", data.idata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_FACE_CULL_MODE :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = ( float )data.idata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_FACE_CULL_MODE\n");
            printf("\t\tFACE CULLING MODE %d\n", data.idata);
          }
          break;
        }
      case CALL_PHIGS_ELEM_LIGHT_SRC_STATE :
        {
          buffer = new Tchar[size];
          TelInqCurElemContent( size, buffer, &actual_size, &data );
          idata        = data.light_source_state.on.number;
          jdata        = data.light_source_state.off.number;
          if( elem_num )
          {
            if ( (IndCur >= idata) && (IndCur >= jdata) )
              EndExplo = 1;
            else
            {
              if (IndCur < idata)
                x = ( float )data.light_source_state.on.integers[IndCur];
              if (IndCur < jdata)
                nx = ( float )data.light_source_state.off.integers[IndCur];
            }
          }
          else
          {
            printf("\tELEM_LIGHT_SRC_STATE\n");
            for (i=0; i<idata; i++)
              printf("\t\tactive light %d\n",
              data.light_source_state.on.integers[i]);
            for (j=0; j<jdata; j++)
              printf("\t\tinactive light %d\n",
              data.light_source_state.off.integers[j]);
          }
          delete[] buffer;
          break;
        }
      case CALL_PHIGS_ELEM_DCUE_IND :
        {
          TelInqCurElemContent( 0, buffer, &actual_size, &data );
          if( elem_num )
          {
            if (IndCur >= 1)
              EndExplo = 1;
            else
            {
              x = ( float )data.idata;
              y = ( float )0.0;
              z = ( float )0.0;
            }
          }
          else
          {
            printf("\tELEM_DCUE_IND\n");
            printf("\t\tDEPTH CUE INDEX %d\n", data.idata);
          }
          break;
        }
      default:
        break;
      }
    }

    if( elem_num )
    {
      index   = 0;
      pt->x   = x;
      pt->y   = y;
      pt->z   = z;
      npt->dx = nx;
      npt->dy = ny;
      npt->dz = nz;
      cpt->r  = r;
      cpt->g  = g;
      cpt->b  = b;
      nfa->dx = dx;
      nfa->dy = dy;
      nfa->dz = dz;

      if( EndExplo )
        *elem_type = 0;
    }
    else
      index ++;
  }        /* while (index) */

  call_func_close_struct ();
}

Tint
call_subr_inquirews( Tint struct_id, Tint *ws_id )
{
  Tint act_size;

  TsmGetWSPosted( struct_id, 1, ws_id, &act_size );

  if( act_size )
    return 1;
  else
    return 0;
}


void
call_subr_polygon_holes( CALL_DEF_LISTFACETS * alfacets )
{
  register int i, j, k;
  int go;
  CMN_KEY k1, k2, k3;

  Tint      *bounds;
  TEL_POINT *points;

  bounds = new Tint[alfacets->NbFacets];

  j = 0;
  for( i=0; i<alfacets->NbFacets; i++ )
    j += alfacets->LFacets[i].NbPoints;

  points = new TEL_POINT[j];

  for( i = 0; i < alfacets->NbFacets; i++ )
    bounds[i] = alfacets->LFacets[i].NbPoints;

  k = 0;
  for (i=0; i<alfacets->NbFacets; i++)
  {
    for (j=0; j<alfacets->LFacets[i].NbPoints; j++)
    {
      points[k].xyz[0] = alfacets->LFacets[i].UPoints.Points[j].x;
      points[k].xyz[1] = alfacets->LFacets[i].UPoints.Points[j].y;
      points[k].xyz[2] = alfacets->LFacets[i].UPoints.Points[j].z;
      k++;
    }
  }

  go = 1;
  if( bounds == 0 )
    go = 0;
  if( points == 0 )
    go = 0;

  if( go )
  {
    k1.id = NUM_FACETS_ID;
    k1.data.ldata = alfacets->NbFacets;
    k2.id = BOUNDS_DATA_ID;
    k2.data.pdata = bounds;
    k3.id = VERTICES_ID;
    k3.data.pdata = points;

    TsmAddToStructure( TelPolygonHoles, 3, &k1, &k2, &k3 );
  }

  if( bounds != 0 )
    delete[] bounds;
  if( points != 0 )
    delete[] points;
}

void
call_subr_polygon_holes_data( CALL_DEF_LISTFACETS * alfacets )
{
  register Tint i, j, k, count=0;
  CMN_KEY key[10];
  cmn_key kk[10];
  int go;
  Tint num_vertices;
  Tint num_bounds, *bounds = 0;
  TEL_POINT *vertices = 0;
  TEL_POINT *vnormals = 0;

  for( i = 0; i < 10; i++ )
    kk[i] = &key[i];

  key[count].id = SHAPE_FLAG_ID;
  key[count].data.ldata = TEL_SHAPE_UNKNOWN;
  count++;

  key[count].id = NUM_FACETS_ID;
  key[count].data.ldata = alfacets->NbFacets;
  count++;

  num_bounds = alfacets->NbFacets;
  bounds = new Tint[num_bounds];

  key[count].id = BOUNDS_DATA_ID;
  key[count].data.pdata = bounds;
  count++;

  num_vertices = 0;
  for( i=0; i<num_bounds; i++ )
    num_vertices += alfacets->LFacets[i].NbPoints;

  k = 0;
  for( i=0; i<num_bounds; i++ )
  {
    bounds[i] = alfacets->LFacets[i].NbPoints;

    switch( alfacets->LFacets[i].TypePoints )
    {
    case 1 : /* Coordinates Specified */
      {
        if( vertices == 0 )
        {
          vertices = new TEL_POINT[num_vertices];

          key[count].id = VERTICES_ID;
          key[count].data.pdata = vertices;
          count++;
        }

        for( j=0; j<alfacets->LFacets[i].NbPoints; j++ )
        {
          vertices[k].xyz[0] = alfacets->LFacets[i].UPoints.Points[j].x;
          vertices[k].xyz[1] = alfacets->LFacets[i].UPoints.Points[j].y;
          vertices[k].xyz[2] = alfacets->LFacets[i].UPoints.Points[j].z;
          k++;
        }

        break;
      }

    case 2 : /* Coordinates and Vertex Normal Specified */
      {
        if( vertices == 0 )
        {
          vertices = new TEL_POINT[num_vertices];

          key[count].id = VERTICES_ID;
          key[count].data.pdata = vertices;
          count++;

          vnormals = new TEL_POINT[num_vertices];
          key[count].id = VNORMALS_ID;
          key[count].data.pdata = vnormals;
          count++;
        }

        for( j=0; j<alfacets->LFacets[i].NbPoints; j++ )
        {
          vertices[k].xyz[0] =
            alfacets->LFacets[i].UPoints.PointsN[j].Point.x;
          vertices[k].xyz[1] =
            alfacets->LFacets[i].UPoints.PointsN[j].Point.y;
          vertices[k].xyz[2] =
            alfacets->LFacets[i].UPoints.PointsN[j].Point.z;
          vnormals[k].xyz[0] =
            alfacets->LFacets[i].UPoints.PointsN[j].Normal.dx;
          vnormals[k].xyz[1] =
            alfacets->LFacets[i].UPoints.PointsN[j].Normal.dy;
          vnormals[k].xyz[2] =
            alfacets->LFacets[i].UPoints.PointsN[j].Normal.dz;
          k++;
        }

        break;
      }
    }
  }

  go = 1;
  if(bounds == 0)
    go = 0;
  if(vertices == 0)
    go = 0;
  if(vnormals == 0)
    go = 0;

  if(go)
  {
    TsmAddToStructure( TelPolygonHoles, -count, kk );
  }

  if(bounds)
    delete[] bounds;
  if(vertices)
    delete[] vertices;
  if(vnormals)
    delete[] vnormals;
}


void
call_subr_curve( CALL_DEF_LISTPOINTS * alpoints )
{
  CMN_KEY  key[3];
  cmn_key  kp[3];

  kp[0] = &key[0], kp[1] = &key[1], kp[2] = &key[2];

  key[0].id = CURVE_TYPE_ID;
  key[0].data.ldata = TelBezierCurve;

  key[1].id = CURVE_NUM_POINTS_ID;
  key[1].data.ldata = 20;

  key[2].id = CURVE_VERTICES_ID;
  key[2].data.pdata = alpoints->UPoints.Points;

  TsmAddToStructure( TelCurve, -3, kp );

  return;
}

/* *** PARRAY functions *** */

void call_subr_parray( CALL_DEF_PARRAY *parray )
{
  CMN_KEY      key;
  key.id = PARRAY_ID;
  key.data.pdata = (void *) parray;

  TsmAddToStructure( TelParray, 1, &key );
}

void call_subr_userdraw( CALL_DEF_USERDRAW *anobject )
{
  CMN_KEY  key;

  key.id = 1;
  key.data.pdata = anobject;

  TsmAddToStructure( TelUserdraw, 1, &key );
}
