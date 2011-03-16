
#include <OpenGl_tgl_all.hxx>

#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>

void EXPORT
call_togl_boundarybox
(
 CALL_DEF_STRUCTURE * astructure,
 int create
 )
{
#define CALL_MAX_BOUNDBOXSIZE 16

  /* for the group */
  Tfloat liner, lineg, lineb;
  Tint linetype;
  Tfloat linewidth;

  /* for the structure */
  Tfloat Liner, Lineg, Lineb;
  Tint Linetype=0;
  Tfloat Linewidth;

  Tfloat Xm, Ym, Zm, XM, YM, ZM;

  CALL_DEF_LISTPOINTS alpoints;
  CALL_DEF_POINT points[CALL_MAX_BOUNDBOXSIZE];

  if( create )
  {
    Xm = astructure->BoundBox.Pmin.x;
    Ym = astructure->BoundBox.Pmin.y;
    Zm = astructure->BoundBox.Pmin.z;
    XM = astructure->BoundBox.Pmax.x;
    YM = astructure->BoundBox.Pmax.y;
    ZM = astructure->BoundBox.Pmax.z;
    call_func_set_edit_mode( CALL_PHIGS_EDIT_INSERT );
    call_func_open_struct( astructure->Id );
    call_func_set_elem_ptr( 0 );
    call_func_set_elem_ptr_label( Structure_LABHighlight );

    liner = astructure->BoundBox.Color.r;
    lineg = astructure->BoundBox.Color.g;
    lineb = astructure->BoundBox.Color.b;
    linetype = CALL_PHIGS_LINE_SOLID;
    linewidth = ( float )1.0;
    call_subr_set_line_colr( liner, lineg, lineb );
    call_func_set_linetype( linetype );
    call_func_set_linewidth( linewidth );

    alpoints.NbPoints = CALL_MAX_BOUNDBOXSIZE;
    alpoints.TypePoints = 1;
    alpoints.UPoints.Points = points;

    points[ 0].x = Xm; points[ 0].y = Ym; points[ 0].z = Zm;
    points[ 1].x = Xm; points[ 1].y = Ym; points[ 1].z = ZM;
    points[ 2].x = Xm; points[ 2].y = YM; points[ 2].z = ZM;
    points[ 3].x = Xm; points[ 3].y = YM; points[ 3].z = Zm;
    points[ 4].x = Xm; points[ 4].y = Ym; points[ 4].z = Zm;
    points[ 5].x = XM; points[ 5].y = Ym; points[ 5].z = Zm;
    points[ 6].x = XM; points[ 6].y = Ym; points[ 6].z = ZM;
    points[ 7].x = XM; points[ 7].y = YM; points[ 7].z = ZM;
    points[ 8].x = XM; points[ 8].y = YM; points[ 8].z = Zm;
    points[ 9].x = XM; points[ 9].y = Ym; points[ 9].z = Zm;
    points[10].x = XM; points[10].y = YM; points[10].z = Zm;
    points[11].x = Xm; points[11].y = YM; points[11].z = Zm;
    points[12].x = Xm; points[12].y = YM; points[12].z = ZM;
    points[13].x = XM; points[13].y = YM; points[13].z = ZM;
    points[14].x = XM; points[14].y = Ym; points[14].z = ZM;
    points[15].x = Xm; points[15].y = Ym; points[15].z = ZM;

    call_subr_polyline( &alpoints );

    Liner = astructure->ContextLine.Color.r;
    Lineg = astructure->ContextLine.Color.g;
    Lineb = astructure->ContextLine.Color.b;

    switch( astructure->ContextLine.LineType )
    {
    case 0 : /* Aspect_TOL_SOLID */
      Linetype = CALL_PHIGS_LINE_SOLID;
      break;
    case 1 : /* Aspect_TOL_DASH */
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

    Linewidth = (Tfloat)astructure->ContextLine.Width;
    call_subr_set_line_colr( Liner, Lineg, Lineb );
    call_func_set_linetype( Linetype );
    call_func_set_linewidth( Linewidth );
    call_func_close_struct();
  }
  else
  {
    call_func_set_edit_mode( CALL_PHIGS_EDIT_REPLACE );
    call_func_open_struct( astructure->Id );
    call_func_set_elem_ptr( 0 );
    call_func_set_elem_ptr_label( Structure_LABHighlight );
    call_func_offset_elem_ptr( 1 );
    call_func_del_elem(); /* boundary box linecolor */
    call_func_offset_elem_ptr( 1 );
    call_func_del_elem(); /* boundary box linetype */
    call_func_offset_elem_ptr( 1 );
    call_func_del_elem(); /* boundary box linewidth */
    call_func_offset_elem_ptr( 1 );
    call_func_del_elem(); /* boundary box definition */
    call_func_offset_elem_ptr( 1 );
    call_func_del_elem(); /* structure linecolor */
    call_func_offset_elem_ptr( 1 );
    call_func_del_elem(); /* structure linetype */
    call_func_offset_elem_ptr( 1 );
    call_func_del_elem(); /* structure linewidth */
    call_func_close_struct();
  }
  return;
}
