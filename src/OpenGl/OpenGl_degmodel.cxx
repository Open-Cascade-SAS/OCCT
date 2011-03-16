#include <OpenGl_tgl_all.hxx>
#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>
#include <OpenGl_degeneration.hxx>

#include <stdio.h>

static TStatus DegenerateModelAdd     ( TSM_ELEM_DATA, Tint, cmn_key* );
static TStatus DegenerateModelDelete  ( TSM_ELEM_DATA, Tint, cmn_key* );
static TStatus DegenerateModelPrint   ( TSM_ELEM_DATA, Tint, cmn_key* );
static TStatus DegenerateModelInquire ( TSM_ELEM_DATA, Tint, cmn_key* );

static TStatus ( *MtdTbl[] ) ( TSM_ELEM_DATA, Tint, cmn_key* ) = {

  NULL,                   /* PickTraverse    */
  NULL,                   /* DisplayTraverse */
  DegenerateModelAdd,     /* Add             */
  DegenerateModelDelete,  /* Delete          */
  DegenerateModelPrint,   /* Print           */
  DegenerateModelInquire  /* Inquire         */

};


MtblPtr TelDegenerateModelInitClass ( TelType* el ) {

  *el = TelDegenerationMode;

  return MtdTbl;

}  /* end TelDegenerateModelInitClass */

static TStatus DegenerateModelAdd ( TSM_ELEM_DATA d, Tint n, cmn_key* k ) {

  PDEGENERATION p = new DEGENERATION();
  //( PDEGENERATION )cmn_getmem (  1, sizeof( DEGENERATION ), 1  );
  //cmn_memcpy<DEGENERATION> (  p, k[ 0 ] -> data.pdata, 1  );
  memcpy( p, k[ 0 ] -> data.pdata, sizeof(DEGENERATION) );

  ( ( tsm_elem_data )( d.pdata )  ) -> pdata = p;

  return TSuccess;

}  /* end DegenerateModelAdd */

static TStatus DegenerateModelDelete  ( TSM_ELEM_DATA d, Tint n, cmn_key* k ) {

  //cmn_freemem ( d.pdata );
  delete d.pdata;

  return TSuccess;

}  /* end DegenerateModelDelete */

static TStatus DegenerateModelPrint ( TSM_ELEM_DATA data, Tint n, cmn_key* k ) {

  char* ptr;

  switch ( data.ldata ) {

  case 0:

    ptr = "TelDegenerateModel. Value = NONE\n";

    break;

  case 1:

    ptr = "TelDegenerateModel. Value = TINY\n";

    break;

  case 2:

    ptr = "TelDegenerateModel. Value = WIREFRAME\n";

    break;

  case 3:

    ptr = "TelDegenerateModel. Value = MARKER\n";

    break;

  case 4:

    ptr = "TelDegenerateModel. Value = BBOX\n";

    break;

  case 5:

    ptr = "TelDegenerateModel. Value = AUTO\n";

  default:

    ptr = "TelDegenerateModel. Value = <invalid>\n";

  }  /* end switch */

  fprintf ( stdout, ptr );

  return TSuccess;

}  /* end DegenerateModelPrint */


static TStatus DegenerateModelInquire ( TSM_ELEM_DATA data, Tint n, cmn_key* k ) {

  Tint i;

  for ( i = 0; i < n; ++i )

    switch ( k[ i ] -> id ) {

   case INQ_GET_SIZE_ID:

     k[ i ] -> data.ldata = sizeof( Tint );

     break;

   case INQ_GET_CONTENT_ID: {

     TEL_INQ_CONTENT* c;
     Teldata*         w;

     c = (tel_inq_content)k[i]->data.pdata;
     w = c -> data;
     c -> act_size = 0;
     w -> idata    = data.ldata;

                            } break;

    }  /* end switch */

    return TSuccess;

}  /* end DegenerateModelInquire */
