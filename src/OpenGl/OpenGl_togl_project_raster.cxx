/***********************************************************************

FONCTION :
----------
File OpenGl_togl_project_raster :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
24-05-96 : CAL ; Creation
12-06-98 : CAL ; Modification respecter l'origine top-left.

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/
#include <OpenGl_tgl_all.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_tsm_ws.hxx>

//For right copution of projection befor dumping to pixmap
int call_togl_adopt_to_rect( int wsid, int width, int height ) {
  CMN_KEY_DATA key;
  TStatus result;

  key.ldata = width;
  result = TsmSetWSAttri (wsid, WSWidth, &key);
  if (result != TSuccess)
    return 1;

  key.ldata = height;
  result = TsmSetWSAttri (wsid, WSHeight, &key);
  if (result != TSuccess)
    return 1;

  return 0;
}

int call_togl_project_raster
(
 int wsid,
 float x,
 float y,
 float z,
 int *ixr,
 int *iyr
 )
{
  TStatus result;
  float xr, yr;

  Tint h;
  CMN_KEY_DATA key;

  TsmGetWSAttri (wsid, WSHeight, &key);
  h = key.ldata;

  result = TelProjectionRaster (wsid, x, y, z, &xr, &yr);

  if (result == TSuccess) {
    *ixr = (int) xr;
    *iyr = h - (int) yr;
    return (0);
  }
  else
    return (1);
}
