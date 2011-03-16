
#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_inquire.hxx>

TStatus
TelInqCurElemTypeSize( TelType *element, Tint *size )
{
  CMN_KEY  key;
  TSM_ELEM elem;
  TStatus  status;

  if( TsmGetCurElem( &elem ) == TFailure )
    return TFailure;

  *element = elem.el;
  key.id = INQ_GET_SIZE_ID;
  status = TsmSendMessage( elem.el, Inquire, elem.data, 1, &key );
  *size = key.data.ldata;

  return status;
}

TStatus
TelInqCurElemContent( Tint size, Tchar *buf, Tint *act_size, Teldata *data )
{
  CMN_KEY         key;
  TSM_ELEM        elem;
  TStatus         status;
  TEL_INQ_CONTENT content;

  if( TsmGetCurElem( &elem ) == TFailure )
    return TFailure;

  content.size     = size;
  content.buf      = buf;
  content.data     = data;

  key.data.pdata   = &content;

  key.id = INQ_GET_CONTENT_ID;
  status =  TsmSendMessage( elem.el, Inquire, elem.data, 1, &key );
  *act_size = content.act_size;

  return status;
}
