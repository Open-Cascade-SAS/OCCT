/***********************************************************************

FONCTION :
----------
File OpenGl_texture.c :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
05-08-97 : PCT ; Support texture mapping

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>
#include <OpenGl_Memory.hxx>

static  TStatus  TextureIdMethodAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextureIdMethodDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextureIdMethodPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  TextureIdMethodInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  TextureIdMethodDisplay,
  TextureIdMethodAdd,
  0,             /* Delete */
  TextureIdMethodPrint,
  TextureIdMethodInquire
};


MtblPtr
TelTextureIdInitClass( TelType *el )
{
  *el = TelTextureId;
  return MtdTbl;
}

static  TStatus
TextureIdMethodAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}


static  TStatus
TextureIdMethodDisplay( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  CMN_KEY  key;

  key.id = TelTextureId;
  key.data.ldata = d.ldata;
  TsmSetAttri( 1, &key );
  return TSuccess;
}


static  TStatus
TextureIdMethodPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{

  return TSuccess;
}


static TStatus
TextureIdMethodInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  Tint i;

  for( i = 0; i < n; i++ )
  {
    switch( k[i]->id )
    {
    case INQ_GET_SIZE_ID:
      {
        k[i]->data.ldata = sizeof( Tint );
        break;
      }
    case INQ_GET_CONTENT_ID:
      {
        TEL_INQ_CONTENT *c;
        Teldata         *w;

        c = (TEL_INQ_CONTENT*)k[i]->data.pdata;
        w = c->data;
        c->act_size = 0;
        w->idata = data.ldata;
        break;
      }
    }
  }

  return TSuccess;
}
