/***********************************************************************

FONCTION :
----------
File OpenGl_textcontextgroup :


REMARQUES:
---------- 

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
03-09-97 : FMN ; Ajout traitement Begin/EndPrimitives()
03-03-98 : CAL ; Modification des structures CALL_DEF_GROUP et STRUCTURE
30-11-98 : FMN ; S4069 : Textes toujours visibles

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 


#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>
#include <OpenGl_context.hxx>
#include <OpenGl_tgl_utilgr.hxx>

#include <OSD_FontAspect.hxx>

/*----------------------------------------------------------------------*/

void EXPORT
call_togl_textcontextgroup
(
 CALL_DEF_GROUP * agroup,
 int noinsert
 )
{
  /* for the group */
  Tchar    *fontid=NULL;
  Tfloat    textr, textg, textb;
  Tfloat    textrs, textgs, textbs;
  Tfloat    textspace;
  Tfloat    textexpan;
  int       textstyle, textdisplaytype; 
  int       textzoomable;
  float     textangle;
  OSD_FontAspect textfontaspect;

  /* for the structure */
  Tchar    *Fontid=NULL;
  Tfloat    Textr, Textg, Textb;
  Tfloat    Textrs, Textgs, Textbs;
  Tfloat    Textspace;
  Tfloat    Textexpan;
  int       Textstyle, Textdisplaytype;
  int       Textzoomable;
  float     Textangle;
  OSD_FontAspect Textfontaspect;

  /*
  * Si le groupe est ouvert cela signifie que BeginPrimitives
  * a ete utilise.
  * Dans le cas des markers composes, il faut refermer temporairement
  * le groupe pour eviter des insertions d'attributs aleatoires.
  */
  if (agroup->IsOpen) call_togl_closegroup (agroup);

#ifdef TRACE

  cout << "TKOpenGl::OpenGl_togl_textcontextgroup"
    << "\t is defined : " << agroup->ContextText.IsDef << endl;
#endif
  if( agroup->ContextText.IsDef )
  {
    textr  = agroup->ContextText.Color.r;
    textg  = agroup->ContextText.Color.g;
    textb  = agroup->ContextText.Color.b;
    textrs = agroup->ContextText.ColorSubTitle.r;
    textgs = agroup->ContextText.ColorSubTitle.g;
    textbs = agroup->ContextText.ColorSubTitle.b;

    fontid = new Tchar[strlen((char*)agroup->ContextText.Font)+1];
    strcpy((char*)fontid,(char*)agroup->ContextText.Font);

    textspace = (Tfloat) agroup->ContextText.Space;
    textexpan = (Tfloat) agroup->ContextText.Expan;
    textstyle = (int) agroup->ContextText.Style;
    textdisplaytype = (int) agroup->ContextText.DisplayType;
    textzoomable = (int) agroup->ContextText.TextZoomable; 
    textangle = (float) agroup->ContextText.TextAngle; 
    textfontaspect = (OSD_FontAspect) agroup->ContextText.TextFontAspect;         

    Textr = agroup->Struct->ContextText.Color.r;
    Textg = agroup->Struct->ContextText.Color.g;
    Textb = agroup->Struct->ContextText.Color.b;

    Textrs = agroup->Struct->ContextText.ColorSubTitle.r;
    Textgs = agroup->Struct->ContextText.ColorSubTitle.g;
    Textbs = agroup->Struct->ContextText.ColorSubTitle.b;

    Fontid = new Tchar[strlen((char*)agroup->Struct->ContextText.Font)+1];
    strcpy((char*)Fontid,(char*)agroup->Struct->ContextText.Font);

    Textspace = (Tfloat) agroup->Struct->ContextText.Space;
    Textexpan = (Tfloat) agroup->Struct->ContextText.Expan;
    Textstyle = (int) agroup->Struct->ContextText.Style;
    Textdisplaytype = (int) agroup->Struct->ContextText.DisplayType;
    Textzoomable = (int) agroup->Struct->ContextText.TextZoomable;
    Textangle = (float) agroup->Struct->ContextText.TextAngle;
    Textfontaspect = (OSD_FontAspect) agroup->Struct->ContextText.TextFontAspect;

    if( noinsert )
    {
      if( agroup->ContextText.IsSet )
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
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTFILLAREA_SIZE );
      if( agroup->ContextMarker.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTMARKER_SIZE );
      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_text_zoomable( textzoomable );   
      call_func_set_text_angle( textangle );   
      call_func_set_text_fontaspect( textfontaspect );   

      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_text_font( fontid );
      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_char_space( textspace );
      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_char_expan( textexpan );
      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_subr_set_text_colr( textr, textg, textb );
      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_text_style( textstyle );
      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_text_display_type( textdisplaytype );
      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_subr_set_text_colr_subtitle( textrs, textgs, textbs );

      call_func_set_elem_ptr_label( agroup->LabelEnd );
      call_func_offset_elem_ptr(-call_util_context_group_place(agroup)-1);
      if( agroup->ContextLine.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTLINE_SIZE );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTFILLAREA_SIZE );
      if( agroup->ContextMarker.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTMARKER_SIZE );
      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );             
      call_func_set_text_zoomable( Textzoomable);
      call_func_set_text_angle( Textangle);
      call_func_set_text_fontaspect( Textfontaspect);   

      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_text_font( Fontid );
      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_char_space( Textspace );
      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_char_expan( Textexpan );
      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_subr_set_text_colr( Textr, Textg, Textb );
      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_text_style( Textstyle );
      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_text_display_type( Textdisplaytype );
      if( agroup->ContextText.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_subr_set_text_colr_subtitle( Textrs, Textgs, Textbs );
      call_func_close_struct();
    } /* no insert */

    if( noinsert == 0 )
    {
      call_func_set_edit_mode( CALL_PHIGS_EDIT_INSERT );
      call_func_open_struct( agroup->Struct->Id );

      call_func_set_elem_ptr( 0 );
      call_func_set_elem_ptr_label( agroup->LabelEnd );
      call_func_offset_elem_ptr(-call_util_context_group_place(agroup)-1);
      call_func_set_text_zoomable( textzoomable );  
      call_func_set_text_angle( textangle );  
      call_func_set_text_fontaspect( textfontaspect );  

      call_func_set_text_font( fontid );
      call_func_set_char_space( textspace );
      call_func_set_char_expan( textexpan );
      call_subr_set_text_colr( textr, textg, textb );
      call_func_set_text_style( textstyle );
      call_func_set_text_display_type( textdisplaytype );
      call_subr_set_text_colr_subtitle( textrs, textgs, textbs );

      if( agroup->ContextLine.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTLINE_SIZE );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTFILLAREA_SIZE );
      if( agroup->ContextMarker.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTMARKER_SIZE );
      if( !agroup->ContextText.IsSet )
      {
        call_func_set_text_zoomable( Textzoomable );
        call_func_set_text_angle( Textangle ); 
        call_func_set_text_fontaspect( Textfontaspect );     
        call_func_set_text_font( Fontid );
        call_func_set_char_space( Textspace );
        call_func_set_char_expan( Textexpan );
        call_subr_set_text_colr( Textr, Textg, Textb );
        call_func_set_text_style( Textstyle );
        call_func_set_text_display_type( Textdisplaytype );
        call_subr_set_text_colr_subtitle( Textrs, Textgs, Textbs );
      }
      call_func_close_struct();
    } /* insert */
  }
  /* Voir commentaire en debut de fonction */
  if (agroup->IsOpen) call_togl_opengroup (agroup);
  return;
}
