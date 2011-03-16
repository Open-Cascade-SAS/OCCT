/***********************************************************************

FONCTION :
----------
File OpenGl_linecontextgroup :


REMARQUES:
---------- 

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
03-09-97 : FMN ; Ajout traitement Begin/EndPrimitives()
03-03-98 : CAL ; Modification des structures CALL_DEF_GROUP et STRUCTURE

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
#include <OpenGl_tgl_utilgr.hxx>

/*----------------------------------------------------------------------*/

void EXPORT
call_togl_linecontextgroup
(
 CALL_DEF_GROUP * agroup,
 int noinsert
 )
{
  /* for the group */
  Tfloat  liner, lineg, lineb;
  Tint    linetype=0;
  Tfloat  linewidth;

  /* for the structure */
  Tfloat  Liner, Lineg, Lineb;
  Tint    Linetype=0;
  Tfloat  Linewidth;

  /*
  * Si le groupe est ouvert cela signifie que BeginPrimitives
  * a ete utilise.
  * Dans le cas des markers composes, il faut refermer temporairement
  * le groupe pour eviter des insertions d'attributs aleatoires.
  */
  if (agroup->IsOpen) call_togl_closegroup (agroup);

  if( agroup->ContextLine.IsDef )
  {
    liner = agroup->ContextLine.Color.r;
    lineg = agroup->ContextLine.Color.g;
    lineb = agroup->ContextLine.Color.b;

    switch( agroup->ContextLine.LineType )
    {
    case 0 : /* Aspect_TOL_SOLID */
      linetype = CALL_PHIGS_LINE_SOLID;
      break;
    case 1 : /* Aspect_DASH */
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
    linewidth = (Tfloat)agroup->ContextLine.Width;

    Liner = agroup->Struct->ContextLine.Color.r;
    Lineg = agroup->Struct->ContextLine.Color.g;
    Lineb = agroup->Struct->ContextLine.Color.b;

    switch( agroup->Struct->ContextLine.LineType )
    {
    case 0 : /* Aspect_TOL_SOLID */
      Linetype = CALL_PHIGS_LINE_SOLID;
      break;
    case 1 : /* Aspect_DASH */
      Linetype = CALL_PHIGS_LINE_DASH;
      break;
    case 2 : /* Aspect_TOL_DOT */
      Linetype = CALL_PHIGS_LINE_DOT;
      break;
    case 3 : /* Aspect_TOL_DOTDASH */
      Linetype = CALL_PHIGS_LINE_DASH_DOT;
      break;
    default :
      break;
    }
    Linewidth = (Tfloat)agroup->Struct->ContextLine.Width;

    /*TransPers = agroup->TransformPersistenceFlag;*/

    if( noinsert )
    {
      if( agroup->ContextLine.IsSet )
        call_func_set_edit_mode( CALL_PHIGS_EDIT_REPLACE );
      else
        call_func_set_edit_mode( CALL_PHIGS_EDIT_INSERT );
      call_func_open_struct( agroup->Struct->Id );

      call_func_set_elem_ptr( 0 );
      call_func_set_elem_ptr_label( agroup->LabelBegin );
      if( agroup->PickId.IsSet )
        call_func_offset_elem_ptr( 1 );
      if( agroup->ContextLine.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_subr_set_line_colr( liner, lineg, lineb );
      if( agroup->ContextLine.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_linetype( linetype );
      if( agroup->ContextLine.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_linewidth( linewidth );

      call_func_set_elem_ptr_label( agroup->LabelEnd );
      call_func_offset_elem_ptr( -call_util_context_group_place( agroup )
        - 1 );
      if( agroup->ContextLine.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_subr_set_line_colr( Liner, Lineg, Lineb );
      if( agroup->ContextLine.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_linetype( Linetype );
      if( agroup->ContextLine.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_linewidth( Linewidth );
      call_func_close_struct(  );
    } /* no insert */

    if( noinsert == 0 )
    {
      call_func_set_edit_mode( CALL_PHIGS_EDIT_INSERT );
      call_func_open_struct( agroup->Struct->Id );

      call_func_set_elem_ptr( 0 );
      call_func_set_elem_ptr_label( agroup->LabelEnd );
      call_func_offset_elem_ptr( -call_util_context_group_place( agroup )
        - 1 );
      call_subr_set_line_colr( liner, lineg, lineb );
      call_func_set_linetype( linetype );
      call_func_set_linewidth( linewidth );

      if( !agroup->ContextLine.IsSet )
      {
        call_subr_set_line_colr( Liner, Lineg, Lineb );
        call_func_set_linetype( Linetype );
        call_func_set_linewidth( Linewidth );
      }
      call_func_close_struct();
    } /* insert */
  }
  /* Voir commentaire en debut de fonction */
  if (agroup->IsOpen) call_togl_opengroup (agroup);
  return;
}
