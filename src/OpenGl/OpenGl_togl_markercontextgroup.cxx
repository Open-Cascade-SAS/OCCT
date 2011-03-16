/***********************************************************************

FONCTION :
----------
File OpenGl_togl_markercontextgroup.c :


REMARQUES:
---------- 

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
03-03-98 : CAL ; Modification des structures CALL_DEF_GROUP et STRUCTURE

************************************************************************/


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

/*--------------------------------generate USERDEFINED marker--------begin-------*/
#ifdef WNT
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if defined(HAVE_MALLOC_H) || defined(WNT)
#include <malloc.h>
#endif

typedef struct
{
  int*      IdList;
  GLuint*   DisplayIdList;
  int       Quantity; 
} UserDefineMarkerStruct;

static UserDefineMarkerStruct UDMS_Marker = { NULL, NULL, 0 };

int EXPORT GenerateMarkerBitmap( int theId, unsigned int theWidth, unsigned int theHeight, unsigned char* theArray )
{
  int i = 0;
  int anIndex = -1;
  GLint w, h, size, k, l;

  for( ; i < UDMS_Marker.Quantity; i++ )
  {
    if( UDMS_Marker.IdList[i] == theId )
    {
      anIndex = i;
      glDeleteLists( UDMS_Marker.DisplayIdList[i], 1 ); 
      if ( glGetError() != GL_NO_ERROR ) 
        return -1;
      break;
    } 
  }
  if( anIndex == -1 )
  {
    anIndex = UDMS_Marker.Quantity;
    if( anIndex == 0 )
    {
      UDMS_Marker.IdList = (int*)malloc( sizeof( int ) );
      UDMS_Marker.DisplayIdList = (GLuint*)malloc( sizeof( GLuint ) );
    }
    else
    {
      UDMS_Marker.IdList = (int*) realloc( UDMS_Marker.IdList, (UDMS_Marker.Quantity + 1)*sizeof( int ) );
      UDMS_Marker.DisplayIdList = (GLuint*) realloc( UDMS_Marker.DisplayIdList, (UDMS_Marker.Quantity + 1)*sizeof( GLuint ) );
    }
    UDMS_Marker.Quantity++;    
  } 

  UDMS_Marker.IdList[anIndex] = theId;
  UDMS_Marker.DisplayIdList[anIndex] = glGenLists( 1 );
  glNewList( UDMS_Marker.DisplayIdList[anIndex], GL_COMPILE );

  w = ( GLsizei ) theWidth;
  h = ( GLsizei ) theHeight;
  glBitmap( w, h,
    ( float )theWidth / ( float )2.0,
    ( float )theHeight / ( float )2.0,
    ( float )30.0, ( float )30.0, 
    ( GLubyte* )theArray );

  glEndList();

  return 0;
}

GLuint EXPORT GetListIndex( int theId )
{
  int i = 0;
  for( ; i < UDMS_Marker.Quantity; i++ )
  {
    if( UDMS_Marker.IdList[i] == theId )
      return UDMS_Marker.DisplayIdList[i];
  }
  return -1;
}

/*--------------------------------generate USERDEFINED marker---------end--------*/


void EXPORT
call_togl_markercontextgroup
(
 CALL_DEF_GROUP * agroup,
 int noinsert
 )
{
  /* for the group */
  Tfloat        markr, markg, markb;
  Tint        markertype;
  Tfloat        markerscale;

  /* for the structure */
  Tfloat        Markr, Markg, Markb;
  Tint        Markertype;
  Tfloat        Markerscale;

  /*
  * Si le groupe est ouvert cela signifie que BeginPrimitives
  * a ete utilise.
  * Dans le cas des markers composes, il faut refermer temporairement
  * le groupe pour eviter des insertions d'attributs aleatoires.
  */
  if (agroup->IsOpen) call_togl_closegroup (agroup);

  if( agroup->ContextMarker.IsDef )
  {
    markr = agroup->ContextMarker.Color.r;
    markg = agroup->ContextMarker.Color.g;
    markb = agroup->ContextMarker.Color.b;
    markertype = CALL_PHIGS_MARKER_CROSS;
    switch( agroup->ContextMarker.MarkerType )
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
    case 13 : /* Aspect_TOM_USERDEFINED */
      markertype = CALL_PHIGS_MARKER_USER_DEFINED;
      break;
    default :
      break;
    }
    markerscale = (Tfloat)agroup->ContextMarker.Scale;

    Markr = agroup->Struct->ContextMarker.Color.r;
    Markg = agroup->Struct->ContextMarker.Color.g;
    Markb = agroup->Struct->ContextMarker.Color.b;
    Markertype = CALL_PHIGS_MARKER_CROSS;
    switch( agroup->Struct->ContextMarker.MarkerType )
    {
    case 0 : /* Aspect_TOM_POINT */
      Markertype = CALL_PHIGS_MARKER_DOT;
      break;
    case 1 : /* Aspect_TOM_PLUS */
      Markertype = CALL_PHIGS_MARKER_PLUS;
      break;
    case 2 : /* Aspect_TOM_STAR */
      Markertype = CALL_PHIGS_MARKER_ASTERISK;
      break;
    case 3 : /* Aspect_TOM_O */
      Markertype = CALL_PHIGS_MARKER_CIRCLE;
      break;
    case 4 : /* Aspect_TOM_X */
      Markertype = CALL_PHIGS_MARKER_CROSS;
      break;
    case 13 : /* Aspect_TOM_USERDEFINED*/
      markertype = CALL_PHIGS_MARKER_USER_DEFINED;
      break;
    default :
      break;
    }
    Markerscale = (Tfloat)agroup->Struct->ContextMarker.Scale;

    if( noinsert )
    {
      if( agroup->ContextMarker.IsSet )
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
        call_func_offset_elem_ptr( 1 );
      call_subr_set_marker_colr( markr, markg, markb );
      if( agroup->ContextMarker.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_marker_type( markertype );
      if( agroup->ContextMarker.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_marker_size( markerscale );

      call_func_set_elem_ptr_label( agroup->LabelEnd );
      call_func_offset_elem_ptr(-call_util_context_group_place(agroup)-1);
      if( agroup->ContextLine.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTLINE_SIZE );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTFILLAREA_SIZE );
      if( agroup->ContextMarker.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_subr_set_marker_colr( Markr, Markg, Markb );
      if( agroup->ContextMarker.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_marker_type( Markertype );
      if( agroup->ContextMarker.IsSet )
        call_func_offset_elem_ptr( 1 );
      call_func_set_marker_size( Markerscale );

      call_func_close_struct();
    } /* no insert */

    if( noinsert == 0 )
    {
      call_func_set_edit_mode( CALL_PHIGS_EDIT_INSERT );
      call_func_open_struct( agroup->Struct->Id );

      call_func_set_elem_ptr( 0 );
      call_func_set_elem_ptr_label( agroup->LabelEnd );
      call_func_offset_elem_ptr(-call_util_context_group_place(agroup)-1);
      call_subr_set_marker_colr( markr, markg, markb );
      call_func_set_marker_type( markertype );
      call_func_set_marker_size( markerscale );

      if( agroup->ContextLine.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTLINE_SIZE );
      if( agroup->ContextFillArea.IsSet )
        call_func_offset_elem_ptr( CALL_DEF_CONTEXTFILLAREA_SIZE );
      if( !agroup->ContextMarker.IsSet )
      {
        call_subr_set_marker_colr( Markr, Markg, Markb );
        call_func_set_marker_type( Markertype );
        call_func_set_marker_size( Markerscale );
      }
      call_func_close_struct();
    } /* insert */
  }
  /* Voir commentaire en debut de fonction */
  if (agroup->IsOpen) call_togl_opengroup (agroup);
  return;
}
