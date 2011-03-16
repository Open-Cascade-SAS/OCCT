/***********************************************************************

FONCTION :
----------
File OpenGl_dotexturemap.c :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
19-08-97 : FMN ; ajout texture mapping

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/


#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

/*----------------------------------------------------------------------*/

static  TStatus  DoTextureMapMethodAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  DoTextureMapMethodDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  DoTextureMapMethodPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  DoTextureMapMethodInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
    DoTextureMapMethodDisplay,
    DoTextureMapMethodAdd,
    0,             /* Delete */
    DoTextureMapMethodPrint,
    DoTextureMapMethodInquire
};


MtblPtr
TelDoTextureMapInitClass( TelType *el )
{
  *el = TelDoTextureMap;
  return MtdTbl;
}

static  TStatus
DoTextureMapMethodAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}


static  TStatus
DoTextureMapMethodDisplay( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  CMN_KEY  key;

  key.id = TelDoTextureMap;
  key.data.ldata = d.ldata;
  TsmSetAttri( 1, &key );
  return TSuccess;
}


static  TStatus
DoTextureMapMethodPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  switch( data.ldata )
  {
  case TEL_SD_SHADING:
    fprintf( stdout, "TelDoTextureMap. Value = TEL_SD_SHADING\n" );
    break;

  case TEL_SD_TEXTURE:
    fprintf( stdout, "TelDoTextureMap. Value = TEL_SD_TEXTURE\n" );
    break;
  }

  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
DoTextureMapMethodInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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

        c = (tel_inq_content)k[i]->data.pdata;
        w = c->data;
        c->act_size = 0;
        w->idata = data.ldata;
        break;
      }
    }
  }

  return TSuccess;
}
