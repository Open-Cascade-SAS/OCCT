/*      08-04-98 : FGU ; Ajout emission  */

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
#include <OpenGl_tgl_funcs.hxx>

static  TStatus  InteriorReflectanceEquationAdd( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  InteriorReflectanceEquationDisplay( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  InteriorReflectanceEquationPrint( TSM_ELEM_DATA, Tint, cmn_key* );
static  TStatus  InteriorReflectanceEquationInquire( TSM_ELEM_DATA, Tint, cmn_key* );

static  TStatus  (*MtdTbl[])( TSM_ELEM_DATA, Tint, cmn_key* ) =
{
  0,             /* PickTraverse */
  InteriorReflectanceEquationDisplay,
  InteriorReflectanceEquationAdd,
  0,             /* Delete */
  InteriorReflectanceEquationPrint,
  InteriorReflectanceEquationInquire
};


MtblPtr
TelInteriorReflectanceEquationInitClass( TelType *el )
{
  *el = TelInteriorReflectanceEquation;
  return MtdTbl;
}

static  TStatus
InteriorReflectanceEquationAdd( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  ((tsm_elem_data)(d.pdata))->ldata = k[0]->data.ldata;

  return TSuccess;
}


static  TStatus
InteriorReflectanceEquationDisplay( TSM_ELEM_DATA d, Tint n, cmn_key *k )
{
  CMN_KEY  key;

  key.id = TelInteriorReflectanceEquation;
  key.data.ldata = d.ldata;
  TsmSetAttri( 1, &key );
  return TSuccess;
}


static  TStatus
InteriorReflectanceEquationPrint( TSM_ELEM_DATA data, Tint n, cmn_key *k )
{
  switch( data.ldata )
  {
  case CALL_PHIGS_REFL_NONE:
    fprintf( stdout, "TelInteriorReflectanceEquation. Value = NONE\n" );
    break;
  default :
    fprintf( stdout, "TelInteriorReflectanceEquation. Value = %d\n", data.ldata  );
    break;

    /* case TelLModelAmbient:
    fprintf( stdout, "TelInteriorReflectanceEquation. Value = \
    GL_AMBIENT\n" );
    break;

    case TelLModelDiffuse:
    fprintf( stdout, "TelInteriorReflectanceEquation. Value = \
    GL_AMBIENT_AND_DIFFUSE\n" );
    break;

    case TelLModelSpecular:
    fprintf( stdout, "TelInteriorReflectanceEquation. Value = \
    GL_SPECULAR\n" );
    break;*/
  }

  fprintf( stdout, "\n" );

  return TSuccess;
}


static TStatus
InteriorReflectanceEquationInquire( TSM_ELEM_DATA data, Tint n, cmn_key *k )
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
