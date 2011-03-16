/***********************************************************************

FONCTION :
----------
File OpenGl_vind :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
20-02-96 : FMN ; Suppression code inutile:
01-04-96 : CAL ; Integration MINSK portage WNT
05-01-98 : FMN ; Suppression WNT inutile

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_telem_inquire.hxx>
#include <OpenGl_telem_attri.hxx>

/*----------------------------------------------------------------------*/
/*
* Fonctions statiques
*/

static  TStatus  ViewIndexDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  ViewIndexAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  ViewIndexPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  ViewIndexInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  ViewIndexDisplay,             /* PickTraverse */
    ViewIndexDisplay,
    ViewIndexAdd,
    0,             /* Delete */
    ViewIndexPrint,
    ViewIndexInquire
};

/*----------------------------------------------------------------------*/

MtblPtr
TelViewIndexInitClass( TelType *el )
{
  *el = TelViewIndex;
  return MtdTbl;
}

/*----------------------------------------------------------------------*/

static  TStatus
ViewIndexAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static  TStatus
ViewIndexDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY   kk;

  kk.id = TelViewIndex;
  kk.data.ldata = data.ldata;
  TsmSetAttri( 1, &kk );
  if( k[0]->id == TOn )
    TelSetViewIndex( TglActiveWs, data.ldata );
  return  TSuccess;
}

/*----------------------------------------------------------------------*/

static  TStatus
ViewIndexPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  fprintf( stdout, "TelViewIndex. Value = %d\n", data.ldata);
  fprintf( stdout, "\n" );

  return TSuccess;
}

/*----------------------------------------------------------------------*/

static TStatus
ViewIndexInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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

        c = (TEL_INQ_CONTENT*)(k[i]->data.pdata);
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
