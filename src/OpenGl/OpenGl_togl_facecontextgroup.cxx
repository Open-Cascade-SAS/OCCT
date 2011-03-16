/***********************************************************************

FONCTION :
----------
File OpenGl_facecontextgroup :


REMARQUES:
---------- 

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
03-09-97 : FMN ; Ajout traitement Begin/EndPrimitives()
05-08-97 : PCT ; ajout texture mapping
23-12-97 : FMN ; Suppression TelBackInteriorStyle, TelBackInteriorStyleIndex
30-12-97 : FMN ; CTS18312: Correction back material
15-01-98 : FMN ; Ajout Hidden line
03-03-98 : CAL ; Modification des structures CALL_DEF_GROUP et STRUCTURE
08-04-98 : FGU ; Ajout emission 
22-03-04 : SAN ; OCC4895 High-level interface for controlling polygon offsets

************************************************************************/

#define OCC1174 /* SAV 08/01/03 : Added back face interior color management */

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_context.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>
#include <OpenGl_tgl_utilgr.hxx>

/*----------------------------------------------------------------------*/

void EXPORT
call_togl_facecontextgroup
(
 CALL_DEF_GROUP * agroup,
 int noinsert
 )
{
  /* for the group */
  Tfloat      liner, lineg, lineb;
  Tint        linetype=0;
  Tfloat      linewidth;
  Tfloat      fillr, fillg, fillb;
#ifdef OCC1174
  Tfloat      fillBackr, fillBackg, fillBackb;
#endif
  Tint        intstyle=0;
  Tint        hatch=0;
  /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
  Tint        polyOffsetMode;
  Tfloat      polyOffsetFactor, polyOffsetUnits;
  /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

  /* for the structure */
  Tfloat      Liner, Lineg, Lineb;
  Tint        Linetype=0;
  Tfloat      Linewidth;
  Tfloat      Fillr, Fillg, Fillb;
#ifdef OCC1174
  Tfloat      FillBackr, FillBackg, FillBackb;
#endif
  Tint        Intstyle=0;
  Tint        Hatch=0;
  /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
  Tint        PolyOffsetMode;
  Tfloat      PolyOffsetFactor, PolyOffsetUnits;
  /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

  /* Flag sur les composants lumineux du materiel */
  Tint lightbool = 0x0;

  /*
  * Si le groupe est ouvert cela signifie que BeginPrimitives
  * a ete utilise.
  * Dans le cas des markers composes, il faut refermer temporairement
  * le groupe pour eviter des insertions d'attributs aleatoires.
  */
  if (agroup->IsOpen) call_togl_closegroup (agroup);

  if( agroup->ContextFillArea.IsDef )
  {
    /* recuperation du contexte du group */
    /*   pour les primitives fill area   */

    switch( agroup->ContextFillArea.Style )
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

    switch( agroup->ContextFillArea.Hatch )
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

    fillr         = agroup->ContextFillArea.IntColor.r;
    fillg         = agroup->ContextFillArea.IntColor.g;
    fillb         = agroup->ContextFillArea.IntColor.b;

#ifdef OCC1174
    fillBackr     = agroup->ContextFillArea.BackIntColor.r;
    fillBackg     = agroup->ContextFillArea.BackIntColor.g;
    fillBackb     = agroup->ContextFillArea.BackIntColor.b;
#endif

    liner         = agroup->ContextFillArea.EdgeColor.r;
    lineg         = agroup->ContextFillArea.EdgeColor.g;
    lineb         = agroup->ContextFillArea.EdgeColor.b;

    switch( agroup->ContextFillArea.LineType )
    {
    case 0 : /* Aspect_TOL_SOLID */
      linetype         = CALL_PHIGS_LINE_SOLID;
      break;
    case 1 : /* Aspect_DASH */
      linetype         = CALL_PHIGS_LINE_DASH;
      break;
    case 2 : /* Aspect_TOL_DOT */
      linetype         = CALL_PHIGS_LINE_DOT;
      break;
    case 3 : /* Aspect_TOL_DOTDASH */
      linetype         = CALL_PHIGS_LINE_DASH_DOT;
      break;
    default :
      break;
    }

    linewidth         =( Tfloat ) agroup->ContextFillArea.Width;

    /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
    polyOffsetMode   = agroup->ContextFillArea.PolygonOffsetMode;
    polyOffsetFactor = agroup->ContextFillArea.PolygonOffsetFactor;
    polyOffsetUnits  = agroup->ContextFillArea.PolygonOffsetUnits;
    /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

    switch( agroup->Struct->ContextFillArea.Style )
    {
    case 0 : /* Aspect_IS_EMPTY */
      Intstyle = CALL_PHIGS_STYLE_EMPTY;
      break;
    case 1 : /* Aspect_IS_HOLLOW */
      Intstyle = CALL_PHIGS_STYLE_HOLLOW;
      break;
    case 2 : /* Aspect_IS_HATCH */
      Intstyle = CALL_PHIGS_STYLE_HATCH;
      break;
    case 3 : /* Aspect_IS_SOLID */
      Intstyle = CALL_PHIGS_STYLE_SOLID;
      break;
    case 4 : /* Aspect_IS_HIDDENLINE */
      intstyle = CALL_PHIGS_STYLE_HIDDENLINE;
      break;
    default :
      break;
    }

    switch( agroup->Struct->ContextFillArea.Hatch )
    {
    case 0 : /* Aspect_HS_HORIZONTAL */
      Hatch = CALL_PHIGS_HATCH_HORIZ;
      break;
    case 1 : /* Aspect_HS_HORIZONTAL_WIDE */
      Hatch = CALL_PHIGS_HATCH_HORIZ_DBL;
      break;
    case 2 : /* Aspect_HS_VERTICAL */
      Hatch = CALL_PHIGS_HATCH_VERT;
      break;
    case 3 : /* Aspect_HS_VERTICAL_WIDE */
      Hatch = CALL_PHIGS_HATCH_VERT_DBL;
      break;
    case 4 : /* Aspect_HS_DIAGONAL_45 */
      Hatch = CALL_PHIGS_HATCH_DIAG_45;
      break;
    case 5 : /* Aspect_HS_DIAGONAL_45_WIDE */
      Hatch = CALL_PHIGS_HATCH_DIAG_45_DBL;
      break;
    case 6 : /* Aspect_HS_DIAGONAL_135 */
      Hatch = CALL_PHIGS_HATCH_DIAG_135;
      break;
    case 7 : /* Aspect_HS_DIAGONAL_135_WIDE */
      Hatch = CALL_PHIGS_HATCH_DIAG_135_DBL;
      break;
    case 8 : /* Aspect_HS_GRID */
      Hatch = CALL_PHIGS_HATCH_GRID_R;
      break;
    case 9 : /* Aspect_HS_GRID_WIDE */
      Hatch = CALL_PHIGS_HATCH_GRID_R_DBL;
      break;
    case 10 : /* Aspect_HS_GRID_DIAGONAL */
      Hatch = CALL_PHIGS_HATCH_GRID_D;
      break;
    case 11 : /* Aspect_HS_GRID_DIAGONAL_WIDE */
      Hatch = CALL_PHIGS_HATCH_GRID_D_DBL;
      break;
    default :
      break;
    }

    Fillr         = agroup->Struct->ContextFillArea.IntColor.r;
    Fillg         = agroup->Struct->ContextFillArea.IntColor.g;
    Fillb         = agroup->Struct->ContextFillArea.IntColor.b;

#ifdef OCC1174
    FillBackr     = agroup->Struct->ContextFillArea.BackIntColor.r;
    FillBackg     = agroup->Struct->ContextFillArea.BackIntColor.g;
    FillBackb     = agroup->Struct->ContextFillArea.BackIntColor.b;
#endif

    Liner         = agroup->Struct->ContextFillArea.EdgeColor.r;
    Lineg         = agroup->Struct->ContextFillArea.EdgeColor.g;
    Lineb         = agroup->Struct->ContextFillArea.EdgeColor.b;

    switch( agroup->Struct->ContextFillArea.LineType )
    {
    case 0 : /* Aspect_TOL_SOLID */
      Linetype         = CALL_PHIGS_LINE_SOLID;
      break;
    case 1 : /* Aspect_DASH */
      Linetype         = CALL_PHIGS_LINE_DASH;
      break;
    case 2 : /* Aspect_TOL_DOT */
      Linetype         = CALL_PHIGS_LINE_DOT;
      break;
    case 3 : /* Aspect_TOL_DOTDASH */
      Linetype         = CALL_PHIGS_LINE_DASH_DOT;
      break;
    default :
      break;
    }

    Linewidth         =( Tfloat ) agroup->Struct->ContextFillArea.Width;

    /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
    PolyOffsetMode   = agroup->Struct->ContextFillArea.PolygonOffsetMode;
    PolyOffsetFactor = agroup->Struct->ContextFillArea.PolygonOffsetFactor;
    PolyOffsetUnits  = agroup->Struct->ContextFillArea.PolygonOffsetUnits;
    /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

    if( noinsert == 1 )
    {
      if( agroup->ContextFillArea.IsSet )
        call_func_set_edit_mode( CALL_PHIGS_EDIT_REPLACE );
      else
        call_func_set_edit_mode( CALL_PHIGS_EDIT_INSERT );
      call_func_open_struct( agroup->Struct->Id );

      call_func_set_elem_ptr( 0 );
      call_func_set_elem_ptr_label( agroup->LabelBegin );
      if( agroup->PickId.IsSet )
        call_func_offset_elem_ptr( 1 );
      if( agroup->ContextLine.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTLINE_SIZE );
      /* interior */
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_int_style( intstyle );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_subr_set_int_colr( fillr, fillg, fillb, ( float )1.0 );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
#ifdef OCC1174
      call_subr_set_back_int_colr( fillBackr, fillBackg, fillBackb, ( float )1.0 );
#else
      call_subr_set_back_int_colr( fillr, fillg, fillb, ( float )1.0 );
#endif
      /* edge */
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      if( agroup->ContextFillArea.Edge )
        call_func_set_edge_flag( CALL_PHIGS_EDGE_ON );
      else
        call_func_set_edge_flag( CALL_PHIGS_EDGE_OFF );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_subr_set_edge_colr( liner, lineg, lineb );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_edge_type( linetype );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_edgewidth( linewidth );
      /* hatch */
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      if( intstyle == CALL_PHIGS_STYLE_HATCH )
        call_func_set_int_style_ind( hatch );
      else
        call_func_appl_data( 0 );

      /* front and back face */
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      if( agroup->ContextFillArea.Distinguish )
        call_func_set_face_disting_mode( CALL_PHIGS_DISTING_YES );
      else
        call_func_set_face_disting_mode( CALL_PHIGS_DISTING_NO );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      if( agroup->ContextFillArea.BackFace )
        call_func_set_face_cull_mode( CALL_PHIGS_CULL_BACKFACE );
      else
        call_func_set_face_cull_mode( CALL_PHIGS_CULL_NONE );

      /* front face */
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );

      if( agroup->ContextFillArea.Front.IsAmbient )
        lightbool = lightbool | AMBIENT_MASK;        

      if( agroup->ContextFillArea.Front.IsDiffuse )
        lightbool = lightbool | DIFFUSE_MASK;           

      if( agroup->ContextFillArea.Front.IsSpecular )
        lightbool = lightbool | SPECULAR_MASK;

      if( agroup->ContextFillArea.Front.IsEmission )
        lightbool = lightbool | EMISSIVE_MASK;

      if( !lightbool )
        call_func_set_refl_eqn( CALL_PHIGS_REFL_NONE );
      else
        call_func_set_refl_eqn( lightbool );

      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );

      call_subr_set_refl_props( &agroup->ContextFillArea.Front, 0 );

      /* back face */
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );

      if( agroup->ContextFillArea.Back.IsAmbient )
        lightbool = lightbool | AMBIENT_MASK;

      if( agroup->ContextFillArea.Back.IsDiffuse )
        lightbool = lightbool | DIFFUSE_MASK;

      if( agroup->ContextFillArea.Back.IsSpecular )
        lightbool = lightbool | SPECULAR_MASK;

      if( agroup->ContextFillArea.Back.IsEmission )
        lightbool = lightbool | EMISSIVE_MASK;

      if( !lightbool )     
        call_func_set_back_refl_eqn( CALL_PHIGS_REFL_NONE );
      else
        call_func_set_back_refl_eqn( lightbool );

      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_subr_set_refl_props( &agroup->ContextFillArea.Back, 1 );

      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_do_texturemap(agroup->ContextFillArea.Texture.doTextureMap);

      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_texture_id(agroup->ContextFillArea.Texture.TexId);

      /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_polygon_offset_params( polyOffsetMode, polyOffsetFactor, polyOffsetUnits );
      /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

      call_func_set_elem_ptr_label( agroup->LabelEnd );
      call_func_offset_elem_ptr( -call_util_context_group_place( agroup )
        - 1 );
      /* interior */
      if( agroup->ContextLine.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTLINE_SIZE );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_int_style( Intstyle );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_subr_set_int_colr( Fillr, Fillg, Fillb, ( float )1.0 );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
#ifdef OCC1174
      call_subr_set_back_int_colr( FillBackr, FillBackg, FillBackb, ( float )1.0 );
#else
      call_subr_set_back_int_colr( Fillr, Fillg, Fillb, ( float )1.0 );
#endif
      /* edge */
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      if( agroup->ContextFillArea.Edge )
        call_func_set_edge_flag( CALL_PHIGS_EDGE_ON );
      else
        call_func_set_edge_flag( CALL_PHIGS_EDGE_OFF );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_subr_set_edge_colr( Liner, Lineg, Lineb );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_edge_type( Linetype );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_edgewidth( Linewidth );
      /* hatch */
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      if( Intstyle == CALL_PHIGS_STYLE_HATCH )
        call_func_set_int_style_ind( Hatch );
      else
        call_func_appl_data( 0 );
      /* front and back face */
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      if( agroup->Struct->ContextFillArea.Distinguish )
        call_func_set_face_disting_mode( CALL_PHIGS_DISTING_YES );
      else
        call_func_set_face_disting_mode( CALL_PHIGS_DISTING_NO );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      if( agroup->Struct->ContextFillArea.BackFace )
        call_func_set_face_cull_mode( CALL_PHIGS_CULL_BACKFACE );
      else
        call_func_set_face_cull_mode( CALL_PHIGS_CULL_NONE );

      /* front face */
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );

      if( agroup->Struct->ContextFillArea.Front.IsAmbient )
        lightbool = lightbool | AMBIENT_MASK;          

      if( agroup->Struct->ContextFillArea.Front.IsDiffuse )
        lightbool = lightbool | DIFFUSE_MASK;

      if( agroup->Struct->ContextFillArea.Front.IsSpecular )
        lightbool = lightbool | SPECULAR_MASK;

      if( agroup->Struct->ContextFillArea.Front.IsEmission )
        lightbool = lightbool | EMISSIVE_MASK;

      if( !lightbool )    
        call_func_set_refl_eqn( CALL_PHIGS_REFL_NONE );
      else
        call_func_set_refl_eqn( lightbool );  

      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );         
      call_subr_set_refl_props( &agroup->Struct->ContextFillArea.Front, 0 );

      /* back face */
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );

      if( agroup->Struct->ContextFillArea.Back.IsAmbient )
        lightbool = lightbool | AMBIENT_MASK;          

      if( agroup->Struct->ContextFillArea.Back.IsDiffuse )
        lightbool = lightbool | DIFFUSE_MASK;           

      if( agroup->Struct->ContextFillArea.Back.IsSpecular )
        lightbool = lightbool | SPECULAR_MASK;         

      if( agroup->Struct->ContextFillArea.Back.IsEmission )
        lightbool = lightbool | EMISSIVE_MASK;

      if( !lightbool )
        call_func_set_back_refl_eqn( CALL_PHIGS_REFL_NONE );
      else
        call_func_set_back_refl_eqn( lightbool );  

      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_subr_set_refl_props( &agroup->Struct->ContextFillArea.Back, 1 );

      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_do_texturemap(agroup->Struct->ContextFillArea.Texture.doTextureMap);

      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_texture_id(agroup->Struct->ContextFillArea.Texture.TexId);

      /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_polygon_offset_params( PolyOffsetMode, PolyOffsetFactor, PolyOffsetUnits );
      /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

      /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
      /*call_func_offset_elem_ptr( 1 );*/
      /*call_func_set_transform_persistence( TransPers );*/
      /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */

      call_func_close_struct();
    } /* no insert */

    if( noinsert == 0 )
    {
      call_func_set_edit_mode( CALL_PHIGS_EDIT_INSERT );
      call_func_open_struct( agroup->Struct->Id );

      call_func_set_elem_ptr( 0 );
      call_func_set_elem_ptr_label( agroup->LabelEnd );
      call_func_offset_elem_ptr( -call_util_context_group_place( agroup )
        - 1 );
      call_func_set_int_style( intstyle );
      call_subr_set_int_colr( fillr, fillg, fillb, ( float )1.0 );
#ifdef OCC1174
      call_subr_set_back_int_colr( fillBackr, fillBackg, fillBackb, ( float )1.0 );
#else
      call_subr_set_back_int_colr( fillr, fillg, fillb, ( float )1.0 );
#endif
      if( agroup->ContextFillArea.Edge )
        call_func_set_edge_flag( CALL_PHIGS_EDGE_ON );
      else
        call_func_set_edge_flag( CALL_PHIGS_EDGE_OFF );
      call_subr_set_edge_colr( liner, lineg, lineb );
      call_func_set_edge_type( linetype );
      call_func_set_edgewidth( linewidth );
      if( intstyle == CALL_PHIGS_STYLE_HATCH )
        call_func_set_int_style_ind( hatch );
      else
        call_func_appl_data( 0 );
      if( agroup->ContextFillArea.Distinguish )
        call_func_set_face_disting_mode( CALL_PHIGS_DISTING_YES );
      else
        call_func_set_face_disting_mode( CALL_PHIGS_DISTING_NO );
      if( agroup->ContextFillArea.BackFace )
        call_func_set_face_cull_mode( CALL_PHIGS_CULL_BACKFACE );
      else
        call_func_set_face_cull_mode( CALL_PHIGS_CULL_NONE );

      /* front face */
      if( agroup->ContextFillArea.Front.IsAmbient )
        lightbool = lightbool | AMBIENT_MASK;          

      if( agroup->ContextFillArea.Front.IsDiffuse )
        lightbool = lightbool | DIFFUSE_MASK;         

      if( agroup->ContextFillArea.Front.IsSpecular )
        lightbool = lightbool | SPECULAR_MASK;        

      if( agroup->ContextFillArea.Front.IsEmission )      
        lightbool = lightbool | EMISSIVE_MASK;              

      if( !lightbool )
        call_func_set_refl_eqn( CALL_PHIGS_REFL_NONE );
      else
        call_func_set_refl_eqn( lightbool );

      call_subr_set_refl_props( &agroup->ContextFillArea.Front, 0 );

      /* back face */
      if( agroup->ContextFillArea.Back.IsAmbient )
        lightbool = lightbool | AMBIENT_MASK;          

      if( agroup->ContextFillArea.Back.IsDiffuse )
        lightbool = lightbool | DIFFUSE_MASK;

      if( agroup->ContextFillArea.Back.IsSpecular )
        lightbool = lightbool | SPECULAR_MASK;

      if( agroup->ContextFillArea.Back.IsEmission )
        lightbool = lightbool | EMISSIVE_MASK;

      if( !lightbool )
        call_func_set_back_refl_eqn( CALL_PHIGS_REFL_NONE );
      else
        call_func_set_back_refl_eqn( lightbool );

      call_subr_set_refl_props( &agroup->ContextFillArea.Back, 1 );

      call_func_set_do_texturemap(agroup->ContextFillArea.Texture.doTextureMap);

      call_func_set_texture_id(agroup->ContextFillArea.Texture.TexId);

      /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
      call_func_set_polygon_offset_params( polyOffsetMode, polyOffsetFactor, polyOffsetUnits );
      /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */


      if( agroup->ContextLine.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTLINE_SIZE );
      if( !agroup->ContextFillArea.IsSet )
      {
        call_func_set_int_style( Intstyle );
        call_subr_set_int_colr( Fillr, Fillg, Fillb, ( float )1.0 );
#ifdef OCC1174
        call_subr_set_back_int_colr( FillBackr, FillBackg, FillBackb, ( float )1.0 );
#else
        call_subr_set_back_int_colr( Fillr, Fillg, Fillb, ( float )1.0 );
#endif
        if( agroup->Struct->ContextFillArea.Edge )
          call_func_set_edge_flag( CALL_PHIGS_EDGE_ON );
        else
          call_func_set_edge_flag( CALL_PHIGS_EDGE_OFF );
        call_subr_set_edge_colr( Liner, Lineg, Lineb );
        call_func_set_edge_type( Linetype );
        call_func_set_edgewidth( Linewidth );
        if( Intstyle == CALL_PHIGS_STYLE_HATCH )
          call_func_set_int_style_ind( Hatch );
        else
          call_func_appl_data( 0 );
        if( agroup->Struct->ContextFillArea.Distinguish )
          call_func_set_face_disting_mode( CALL_PHIGS_DISTING_YES );
        else
          call_func_set_face_disting_mode( CALL_PHIGS_DISTING_NO );
        if( agroup->Struct->ContextFillArea.BackFace )
          call_func_set_face_cull_mode( CALL_PHIGS_CULL_BACKFACE );
        else
          call_func_set_face_cull_mode( CALL_PHIGS_CULL_NONE );

        /* front face */
        if( agroup->Struct->ContextFillArea.Front.IsAmbient )
          lightbool = lightbool | AMBIENT_MASK;

        if( agroup->Struct->ContextFillArea.Front.IsDiffuse )
          lightbool = lightbool | DIFFUSE_MASK;

        if( agroup->Struct->ContextFillArea.Front.IsSpecular )
          lightbool = lightbool | SPECULAR_MASK;

        if( agroup->Struct->ContextFillArea.Front.IsEmission )
          lightbool = lightbool | EMISSIVE_MASK;

        if( !lightbool )
          call_func_set_refl_eqn( CALL_PHIGS_REFL_NONE );
        else
          call_func_set_refl_eqn( lightbool );

        call_subr_set_refl_props( &agroup->Struct->ContextFillArea.Front, 0 );

        /* back face */
        if( agroup->Struct->ContextFillArea.Back.IsAmbient )
          lightbool = lightbool | AMBIENT_MASK;

        if( agroup->Struct->ContextFillArea.Back.IsDiffuse )
          lightbool = lightbool | DIFFUSE_MASK;

        if( agroup->Struct->ContextFillArea.Back.IsSpecular )
          lightbool = lightbool | SPECULAR_MASK;

        if( agroup->Struct->ContextFillArea.Back.IsEmission )
          lightbool = lightbool | EMISSIVE_MASK;

        if( !lightbool )
          call_func_set_back_refl_eqn( CALL_PHIGS_REFL_NONE );
        else
          call_func_set_back_refl_eqn( lightbool );

        call_subr_set_refl_props(&agroup->Struct->ContextFillArea.Back, 1);

        call_func_set_do_texturemap(agroup->Struct->ContextFillArea.Texture.doTextureMap);

        call_func_set_texture_id(agroup->Struct->ContextFillArea.Texture.TexId);

        /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
        call_func_set_polygon_offset_params( PolyOffsetMode, PolyOffsetFactor, PolyOffsetUnits );
        /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
      }

      call_func_close_struct();
    } /* insert */

  }
  /* Voir commentaire en debut de fonction */
  if (agroup->IsOpen) call_togl_opengroup (agroup);
  return;
}
