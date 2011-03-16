#include <OpenGl_tgl_subrvis.hxx>
#include <OpenGl_tsm.hxx> 
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tgl_funcs.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <InterfaceGraphic_telem.hxx>

void EXPORT
call_togl_gradient_background
(
  int ws, int type,
  tel_colour tcolor1, tel_colour tcolor2
)
{
  call_subr_set_gradient_background ( ws, type, tcolor1, tcolor2 );
  return;
}


void EXPORT call_togl_set_gradient_type
(
  int ws, int type
)
{
  tsm_bg_gradient gradient;
  CMN_KEY_DATA  cmnData;
  CMN_KEY_DATA  cmnKey;

  /* check if gradient background is already created */
  TsmGetWSAttri( ws, WSBgGradient, &cmnData );
  gradient = (tsm_bg_gradient)cmnData.pdata;
  if ( gradient->type != 0 ) {
    switch ( type ) {
        case 0 :
          gradient->type = TSM_GT_NONE;
        case 1 :
          gradient->type = TSM_GT_HOR;
          break;
        case 2 :
          gradient->type = TSM_GT_VER;
          break;
        case 3 :
          gradient->type = TSM_GT_DIAG1;
          break;
        case 4 :
          gradient->type = TSM_GT_DIAG1;
          break;  
        case 5 :
          gradient->type = TSM_GT_CORNER1;
          break;
        case 6 :
          gradient->type = TSM_GT_CORNER2;
          break;
        case 7 :
          gradient->type = TSM_GT_CORNER3;
          break;
        case 8 :
          gradient->type = TSM_GT_CORNER4;
          break;
        default :
          gradient->type = TSM_GT_NONE;
          break;
    }

    /* setting flag to update changes */
    cmnKey.ldata = TNotDone;
    TsmSetWSAttri( ws, WSUpdateState, &cmnKey );

    /* storing background texture */
    cmnData.pdata = gradient;
    TsmSetWSAttri( ws, WSBgGradient, &cmnData );
  }
}
