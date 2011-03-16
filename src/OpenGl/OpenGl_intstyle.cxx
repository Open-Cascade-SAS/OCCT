/***********************************************************************

FONCTION :
----------
Fichier OpenGl_intstyle.c


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
??-??-?? : PCT ; creation
15-01-98 : FMN ; Ajout Hidden line

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

/*----------------------------------------------------------------------*/

static  TStatus  InteriorStyleDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  InteriorStyleAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  InteriorStylePrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  InteriorStyleInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  InteriorStyleDisplay,
  InteriorStyleAdd,
  0,             /* Delete */
  InteriorStylePrint,
  InteriorStyleInquire
};


MtblPtr
TelInteriorStyleInitClass( TelType *el )
{
  *el = TelInteriorStyle;
  return MtdTbl;
}

static  TStatus
InteriorStyleAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}


static  TStatus
InteriorStyleDisplay( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  CMN_KEY     key;

  key.id = TelInteriorStyle;
  key.data.ldata = data.ldata;
  TsmSetAttri( 1, &key );

  return TSuccess;
}


static  TStatus
InteriorStylePrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  switch( data.ldata )
  {
  case TSM_SOLID:
    fprintf( stdout, "TelInteriorStyle. Value = SOLID\n" );
    break;

  case TSM_HOLLOW:
    fprintf( stdout, "TelInteriorStyle. Value = HOLLOW\n" );
    break;

  case TSM_EMPTY:
    fprintf( stdout, "TelInteriorStyle. Value = EMPTY\n" );
    break;

  case TSM_HATCH:
    fprintf( stdout, "TelInteriorStyle. Value = HATCH\n" );
    break;

  case TSM_POINT:
    fprintf( stdout, "TelInteriorStyle. Value = POINT\n" );
    break;

  case TSM_HIDDENLINE:
    fprintf( stdout, "TelInteriorStyle. Value = HIDDENLINE\n" );
    break;


  }
  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
InteriorStyleInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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
