/***********************************************************************

FONCTION :
----------
File OpenGl_togl_unproject_raster :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
24-05-96 : CAL ; Creation

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/
#include <OpenGl_tgl_all.hxx>
#include <OpenGl_telem_util.hxx>

int EXPORT
call_togl_unproject_raster
(
 int wsid,
 int xm,
 int ym,
 int xM,
 int yM,
 int ixr,
 int iyr,
 float *x,
 float *y,
 float *z
 )
{
  TStatus result;
  Tint xr, yr;

  xr = ixr;
  /* 
  Patched by P.Dolbey: the window pixel height decreased by one 
  in order for yr to remain within valid coordinate range [0; Ym -1]
  where Ym means window pixel height.
  */
  yr = (yM-1)-ym-iyr;
  result = TelUnProjectionRaster (wsid, xr, yr, x, y, z);

  if (result == TSuccess)
    return (0);
  else
    return (1);
}

int EXPORT
call_togl_unproject_raster_with_ray
(
 int wsid,
 int xm,
 int ym,
 int xM,
 int yM,
 int ixr,
 int iyr,
 float *x,
 float *y,
 float *z,
 float *dx,
 float *dy,
 float *dz
 )
{
  TStatus result;
  Tint xr, yr;

  xr = ixr;
  yr = yM-ym-iyr;
  result = TelUnProjectionRasterWithRay (wsid, xr, yr, x, y, z, dx, dy, dz);

  if (result == TSuccess)
    return (0);
  else
    return (1);
}
