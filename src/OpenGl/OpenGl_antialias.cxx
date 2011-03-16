/***********************************************************************

FONCTION :
----------
File OpenGl_antialias :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
20-02-96 : FMN ; Suppression code inutile:
01-04-96 : CAL ; Integration MINSK portage WNT

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

static  TStatus  AntiAliasingAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  AntiAliasingDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  AntiAliasingPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  AntiAliasingInquire( TSM_ELEM_DATA, Tint, cmn_key* );

/*----------------------------------------------------------------------*/

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
    0,             /* PickTraverse */
    AntiAliasingDisplay,
    AntiAliasingAdd,
    0,             /* Delete */
    AntiAliasingPrint,
    AntiAliasingInquire
};

/*----------------------------------------------------------------------*/

MtblPtr
TelAntiAliasingInitClass( TelType *el )
{
  *el = TelAntiAliasing;
  return MtdTbl;
}

/*----------------------------------------------------------------------*/

static  TStatus
AntiAliasingAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static  TStatus
AntiAliasingDisplay( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  CMN_KEY  key;

  key.id = TelAntiAliasing;
  key.data.ldata = d.ldata;
  TsmSetAttri( 1, &key );
  return TSuccess;
}

/*----------------------------------------------------------------------*/

static  TStatus
AntiAliasingPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  if( data.ldata == TOn )
    fprintf( stdout, "TelAntiAliasing. Value = ON\n" );
  else
    fprintf( stdout, "TelAntiAliasing. Value = OFF\n" );
  fprintf( stdout, "\n" );

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static TStatus
AntiAliasingInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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

/*----------------------------------------------------------------------*/
