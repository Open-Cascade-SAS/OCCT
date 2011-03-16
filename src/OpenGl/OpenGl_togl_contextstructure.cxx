/***********************************************************************

FONCTION :
----------
File OpenGl_contextstructure.c :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
05-08-97 : PCT ; Support texture mapping
23-12-97 : FMN ; Suppression TelBackInteriorStyle, TelBackInteriorStyleIndex
30-12-97 : FMN ; CTS18312: Correction back material
15-01-98 : FMN ; Ajout Hidden line
03-03-98 : CAL ; Modification des structures CALL_DEF_GROUP et STRUCTURE
08-04-98 : FGU ; Ajout Emission
30-11-98 : FMN ; S4069 : Textes toujours visibles
02.14.100 : JR : Warnings on WNT #include ,stdio.h>
22-03-04 : SAN ; OCC4895 High-level interface for controlling polygon offsets

************************************************************************/

#define OCC1174 /* SAV 08/01/03 : Added back face interior color management*/


/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <stdio.h>

#include <OpenGl_tgl_all.hxx>

#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>

#include <OSD_FontAspect.hxx>


void EXPORT
call_togl_contextstructure
(
 CALL_DEF_STRUCTURE * astructure
 )
{
  Tfloat liner, lineg, lineb;
  Tfloat fillr, fillg, fillb;
#ifdef OCC1174
  Tfloat fillBackr, fillBackg, fillBackb;
#endif
  Tint   linetype=0;
  Tint   intstyle=0;
  Tfloat linewidth;
  Tint   hatch=0;
  /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
  Tint   polyOffsetMode;
  Tfloat polyOffsetFactor, polyOffsetUnits;
  /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

  Tfloat markr, markg, markb;
  Tint   markertype=0;
  Tfloat markerscale;

  Tchar  *fontid=NULL;
  Tfloat textr, textg, textb;
  Tfloat textrs, textgs, textbs;
  Tfloat textspace;
  Tfloat textexpan;
  int    textstyle, textdisplaytype;
  Tint   textzoomable;
  Tfloat textangle;
  OSD_FontAspect textfontaspect;
  /* Flag de mise a jour des composants materiel */
  Tint lightbool = 0x0;
  CALL_DEF_TRANSFORM_PERSISTENCE transPers =  astructure->TransformPersistence;

  /* Destruction */
  if( astructure->ContextLine.IsDef == -1 )
  {
  }
  if( astructure->ContextFillArea.IsDef == -1 )
  {
  }
  if( astructure->ContextMarker.IsDef == -1 )
  {
  }
  if( astructure->ContextText.IsDef == -1 )
  {
  }
  call_func_open_struct( astructure->Id );
  call_func_set_elem_ptr( 0 );
  call_func_set_elem_ptr_label( Structure_LABTransPersistence );
  if( astructure->TransformPersistence.IsSet == 1 )
  {
    call_func_set_edit_mode( CALL_PHIGS_EDIT_REPLACE );
    call_func_offset_elem_ptr( 1 );
  }
  else
    call_func_set_edit_mode( CALL_PHIGS_EDIT_INSERT );
  /*call_func_set_transform_persistence( transPersFlag );*/
  call_func_set_transform_persistence( transPers.Flag, transPers.Point.x, transPers.Point.y, transPers.Point.z );
  call_func_close_struct();
  /* }*/
  /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
  if( astructure->ContextLine.IsDef == 1 )
  {
    liner = astructure->ContextLine.Color.r;
    lineg = astructure->ContextLine.Color.g;
    lineb = astructure->ContextLine.Color.b;

    switch( astructure->ContextLine.LineType )
    {
    case 0 : /* Aspect_TOL_SOLID */
      linetype = CALL_PHIGS_LINE_SOLID;
      break;
    case 1 : /* Aspect_TOL_DASH */
      linetype = CALL_PHIGS_LINE_DASH;
      break;
    case 2 : /* Aspect_TOL_DOT */
      linetype = CALL_PHIGS_LINE_DOT;
      break;
    case 3 : /* Aspect_TOL_DOTDASH */
      linetype = CALL_PHIGS_LINE_DASH_DOT;
      break;
    default :
      break;
    }

    linewidth = (Tfloat)astructure->ContextLine.Width;

    if( astructure->ContextLine.IsSet )
      call_func_set_edit_mode( CALL_PHIGS_EDIT_REPLACE );
    else
      call_func_set_edit_mode( CALL_PHIGS_EDIT_INSERT );
    call_func_open_struct( astructure->Id );
    call_func_set_elem_ptr( 0 );
    call_func_set_elem_ptr_label( Structure_LABContextLine );

    if( astructure->ContextLine.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_subr_set_line_colr( liner, lineg, lineb );
    if( astructure->ContextLine.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_linetype( linetype );
    if( astructure->ContextLine.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_linewidth( linewidth );
    call_func_close_struct();
  }

  if( astructure->ContextFillArea.IsDef == 1 )
  {
    switch( astructure->ContextFillArea.Style )
    {
    case 0 : /* Aspect_IS_EMPTY */
      intstyle = CALL_PHIGS_STYLE_EMPTY;
      break;
    case 1 : /* Aspect_IS_HOLLOW */
      intstyle = CALL_PHIGS_STYLE_HOLLOW;
      break;
    case 2 : /* Aspect_IS_HATCH */
      intstyle = CALL_PHIGS_STYLE_HATCH;
      break;
    case 3 : /* Aspect_IS_SOLID */
      intstyle = CALL_PHIGS_STYLE_SOLID;
      break;
    case 4 : /* Aspect_IS_HIDDENLINE */
      intstyle = CALL_PHIGS_STYLE_HIDDENLINE;
      break;
    default :
      break;
    }

    switch( astructure->ContextFillArea.Hatch )
    {
    case 0 : /* Aspect_HS_HORIZONTAL */
      hatch = CALL_PHIGS_HATCH_HORIZ;
      break;
    case 1 : /* Aspect_HS_HORIZONTAL_WIDE */
      hatch = CALL_PHIGS_HATCH_HORIZ_DBL;
      break;
    case 2 : /* Aspect_HS_VERTICAL */
      hatch = CALL_PHIGS_HATCH_VERT;
      break;
    case 3 : /* Aspect_HS_VERTICAL_WIDE */
      hatch = CALL_PHIGS_HATCH_VERT_DBL;
      break;
    case 4 : /* Aspect_HS_DIAGONAL_45 */
      hatch = CALL_PHIGS_HATCH_DIAG_45;
      break;
    case 5 : /* Aspect_HS_DIAGONAL_45_WIDE */
      hatch = CALL_PHIGS_HATCH_DIAG_45_DBL;
      break;
    case 6 : /* Aspect_HS_DIAGONAL_135 */
      hatch = CALL_PHIGS_HATCH_DIAG_135;
      break;
    case 7 : /* Aspect_HS_DIAGONAL_135_WIDE */
      hatch = CALL_PHIGS_HATCH_DIAG_135_DBL;
      break;
    case 8 : /* Aspect_HS_GRID */
      hatch = CALL_PHIGS_HATCH_GRID_R;
      break;
    case 9 : /* Aspect_HS_GRID_WIDE */
      hatch = CALL_PHIGS_HATCH_GRID_R_DBL;
      break;
    case 10 : /* Aspect_HS_GRID_DIAGONAL */
      hatch = CALL_PHIGS_HATCH_GRID_D;
      break;
    case 11 : /* Aspect_HS_GRID_DIAGONAL_WIDE */
      hatch = CALL_PHIGS_HATCH_GRID_D_DBL;
      break;
    default :
      break;
    }

    fillr = astructure->ContextFillArea.IntColor.r;
    fillg = astructure->ContextFillArea.IntColor.g;
    fillb = astructure->ContextFillArea.IntColor.b;

#ifdef OCC1174
    fillBackr = astructure->ContextFillArea.BackIntColor.r;
    fillBackg = astructure->ContextFillArea.BackIntColor.g;
    fillBackb = astructure->ContextFillArea.BackIntColor.b;
#endif

    liner = astructure->ContextFillArea.EdgeColor.r;
    lineg = astructure->ContextFillArea.EdgeColor.g;
    lineb = astructure->ContextFillArea.EdgeColor.b;

    /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
    polyOffsetMode   = astructure->ContextFillArea.PolygonOffsetMode;
    polyOffsetFactor = astructure->ContextFillArea.PolygonOffsetFactor;
    polyOffsetUnits  = astructure->ContextFillArea.PolygonOffsetUnits;
    /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

    switch( astructure->ContextFillArea.LineType )
    {
    case 0 : /* Aspect_TOL_SOLID */
      linetype = CALL_PHIGS_LINE_SOLID;
      break;
    case 1 : /* Aspect_TOL_DASH */
      linetype = CALL_PHIGS_LINE_DASH;
      break;
    case 2 : /* Aspect_TOL_DOT */
      linetype = CALL_PHIGS_LINE_DOT;
      break;
    case 3 : /* Aspect_TOL_DOTDASH */
      linetype = CALL_PHIGS_LINE_DASH_DOT;
      break;
    default :
      break;
    }

    linewidth = (Tfloat)astructure->ContextFillArea.Width;

    if( astructure->ContextFillArea.IsSet )
      call_func_set_edit_mode( CALL_PHIGS_EDIT_REPLACE );
    else
      call_func_set_edit_mode( CALL_PHIGS_EDIT_INSERT );
    call_func_open_struct( astructure->Id );
    call_func_set_elem_ptr( 0 );
    call_func_set_elem_ptr_label( Structure_LABContextFillArea );

    /* interior */
    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_int_style( intstyle );
    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_subr_set_int_colr( fillr, fillg, fillb, ( float )1.0 );
    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
#ifdef OCC1174
    call_subr_set_back_int_colr( fillBackr, fillBackg, fillBackb, ( float )1.0 );
#else
    call_subr_set_back_int_colr( fillr, fillg, fillb, ( float )1.0 );
#endif
    /* edge */
    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
    if( astructure->ContextFillArea.Edge )
      call_func_set_edge_flag( CALL_PHIGS_EDGE_ON );
    else
      call_func_set_edge_flag( CALL_PHIGS_EDGE_OFF );
    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_subr_set_edge_colr( liner, lineg, lineb );
    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_edge_type( linetype );
    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_edgewidth( linewidth );
    /* hatch */
    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
    if( intstyle == CALL_PHIGS_STYLE_HATCH )
      call_func_set_int_style_ind( hatch );
    else
      call_func_appl_data( 0 );
    /* front and back face */
    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
    if( astructure->ContextFillArea.Distinguish )
      call_func_set_face_disting_mode(CALL_PHIGS_DISTING_YES);
    else
      call_func_set_face_disting_mode(CALL_PHIGS_DISTING_NO);
    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
    if( astructure->ContextFillArea.BackFace )
      call_func_set_face_cull_mode(CALL_PHIGS_CULL_BACKFACE);
    else
      call_func_set_face_cull_mode( CALL_PHIGS_CULL_NONE );
    /* front face */
    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );

    /* Front face */
    /* Gestion Lumiere */
    if( astructure->ContextFillArea.Front.IsAmbient ) 
      lightbool = lightbool | AMBIENT_MASK;                

    if( astructure->ContextFillArea.Front.IsDiffuse )
      lightbool = lightbool | DIFFUSE_MASK;        

    if( astructure->ContextFillArea.Front.IsSpecular )
      lightbool = lightbool | SPECULAR_MASK;          

    if( astructure->ContextFillArea.Front.IsEmission )
      lightbool = lightbool | EMISSIVE_MASK;                                       

    if( !lightbool )
      call_func_set_refl_eqn( CALL_PHIGS_REFL_NONE );
    else
      call_func_set_refl_eqn( lightbool );

    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_subr_set_refl_props( &astructure->ContextFillArea.Front, 0 );

    /* back face */
    /* Gestion lumiere */
    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );

    if( astructure->ContextFillArea.Back.IsAmbient )
      lightbool = lightbool | AMBIENT_MASK;           

    if( astructure->ContextFillArea.Back.IsDiffuse )
      lightbool = lightbool | DIFFUSE_MASK;           

    if( astructure->ContextFillArea.Back.IsSpecular )
      lightbool = lightbool | SPECULAR_MASK;         

    if( astructure->ContextFillArea.Back.IsEmission )
      lightbool = lightbool | EMISSIVE_MASK;          

    if( !lightbool ) 
      call_func_set_back_refl_eqn( CALL_PHIGS_REFL_NONE );
    else
      call_func_set_back_refl_eqn( lightbool );

    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_subr_set_refl_props( &astructure->ContextFillArea.Back, 1 );

    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_do_texturemap(astructure->ContextFillArea.Texture.doTextureMap);

    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_texture_id(astructure->ContextFillArea.Texture.TexId);

    /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
    if( astructure->ContextFillArea.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_polygon_offset_params( polyOffsetMode, polyOffsetFactor, polyOffsetUnits );
    /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
    call_func_close_struct();
  }

  if( astructure->ContextMarker.IsDef == 1 )
  {
    markr = astructure->ContextMarker.Color.r;
    markg = astructure->ContextMarker.Color.g;
    markb = astructure->ContextMarker.Color.b;

    switch( astructure->ContextMarker.MarkerType )
    {
    case 0 : /* Aspect_TOM_POINT */
      markertype = CALL_PHIGS_MARKER_DOT;
      break;
    case 1 : /* Aspect_TOM_PLUS */
      markertype = CALL_PHIGS_MARKER_PLUS;
      break;
    case 2 : /* Aspect_TOM_STAR */
      markertype = CALL_PHIGS_MARKER_ASTERISK;
      break;
    case 3 : /* Aspect_TOM_O */
      markertype = CALL_PHIGS_MARKER_CIRCLE;
      break;
    case 4 : /* Aspect_TOM_X */
      markertype = CALL_PHIGS_MARKER_CROSS;
      break;
    case 5 : /* Aspect_TOM_USERDEFINED*/
      markertype = CALL_PHIGS_MARKER_USER_DEFINED;
      break;
    default :
      printf( "\n\tINVALID MARKER TYPE %d",
        astructure->ContextMarker.MarkerType );
      break;
    }

    markerscale = (Tfloat)astructure->ContextMarker.Scale;

    if( astructure->ContextMarker.IsSet )
      call_func_set_edit_mode( CALL_PHIGS_EDIT_REPLACE );
    else
      call_func_set_edit_mode( CALL_PHIGS_EDIT_INSERT );
    call_func_open_struct( astructure->Id );
    call_func_set_elem_ptr( 0 );
    call_func_set_elem_ptr_label( Structure_LABContextMarker );
    if( astructure->ContextMarker.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_subr_set_marker_colr( markr, markg, markb );
    if( astructure->ContextMarker.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_marker_type( markertype );
    if( astructure->ContextMarker.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_marker_size( markerscale );
    call_func_close_struct();
  }

#ifdef TRACE     
  cout << "TKOpenGl::OpenGl_togl_contextstructure"
    << "\t is defined : " << astructure->ContextText.IsDef << endl; 
#endif    
  if( astructure->ContextText.IsDef == 1 )
  {
    textr = astructure->ContextText.Color.r;
    textg = astructure->ContextText.Color.g;
    textb = astructure->ContextText.Color.b;

    textrs = astructure->ContextText.ColorSubTitle.r;
    textgs = astructure->ContextText.ColorSubTitle.g;
    textbs = astructure->ContextText.ColorSubTitle.b;

    fontid = new Tchar[strlen((char*)astructure->ContextText.Font)+1];
    strcpy((char*)fontid,(char*)astructure->ContextText.Font);

    textspace = (Tfloat)astructure->ContextText.Space;
    textexpan = (Tfloat)astructure->ContextText.Expan;
    textstyle = (int) astructure->ContextText.Style;
    textdisplaytype = (int) astructure->ContextText.DisplayType;    
    textzoomable = (int) astructure->ContextText.TextZoomable;
    textangle = (float) astructure->ContextText.TextAngle;
    textfontaspect = (OSD_FontAspect)astructure->ContextText.TextFontAspect;

    if( astructure->ContextText.IsSet )
      call_func_set_edit_mode( CALL_PHIGS_EDIT_REPLACE );
    else
      call_func_set_edit_mode( CALL_PHIGS_EDIT_INSERT );
    call_func_open_struct( astructure->Id );
    call_func_set_elem_ptr( 0 );
    call_func_set_elem_ptr_label( Structure_LABContextText );
    if( astructure->ContextText.IsSet )
      call_func_offset_elem_ptr( 1 );              
    call_func_set_text_zoomable( textzoomable );
    call_func_set_text_angle( textangle );
    call_func_set_text_fontaspect( textfontaspect );

    if( astructure->ContextText.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_text_font( fontid );
    if( astructure->ContextText.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_char_space( textspace );
    if( astructure->ContextText.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_char_expan( textexpan );
    if( astructure->ContextText.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_subr_set_text_colr( textr, textg, textb );
    if( astructure->ContextText.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_text_style( textstyle );
    if( astructure->ContextText.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_func_set_text_display_type( textdisplaytype );
    if( astructure->ContextText.IsSet )
      call_func_offset_elem_ptr( 1 );
    call_subr_set_text_colr_subtitle( textrs, textgs, textbs );

    call_func_close_struct();
  }
  return;
}
