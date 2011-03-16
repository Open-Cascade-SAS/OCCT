// File         OpenGl_GraphicDriver_713.cxx
// Created      22-10-01
// Author       SAV

// SAV 09/07/02 merged with OpenGl_GraphicDriver_713.cxx created 16/06/2000 by ATS,SPK,GG : G005
//              implementation of PARRAY method

#include <OpenGl_GraphicDriver.jxx>

#include <Aspect_DriverDefinitionError.hxx>


#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_telem_util.hxx>


#define  BUC61044    /* 25/10/01 SAV ; added functionality to control gl depth testing
from higher API */
#define  BUC61045    /* 25/10/01 SAV ; added functionality to control gl lighting 
from higher API */


void OpenGl_GraphicDriver::SetDepthTestEnabled( const Graphic3d_CView& view,
                                               const Standard_Boolean isEnabled ) const
{

#ifdef BUC61044
  Graphic3d_CView MyCView = view;
  call_togl_depthtest( &MyCView, isEnabled );
#endif

}

Standard_Boolean OpenGl_GraphicDriver
::IsDepthTestEnabled( const Graphic3d_CView& view ) const
{
#ifdef BUC61044
  Graphic3d_CView MyCView = view;
  if ( call_togl_isdepthtest( &MyCView ) )
    return Standard_True;
#endif
}

void OpenGl_GraphicDriver::ReadDepths( const Graphic3d_CView& view,
                                       const Standard_Integer x,
                                       const Standard_Integer y,
                                       const Standard_Integer width,
                                       const Standard_Integer height,
                                       const Standard_Address buffer ) const
{
  TelReadDepths (view.WsId, x, y, width, height, (float*) buffer);
}

void OpenGl_GraphicDriver::SetGLLightEnabled( const Graphic3d_CView& view,
                                             const Standard_Boolean isEnabled ) const
{
#ifdef BUC61045
  Graphic3d_CView MyCView = view;
  call_togl_gllight( &MyCView, isEnabled );
#endif
}

Standard_Boolean OpenGl_GraphicDriver
::IsGLLightEnabled( const Graphic3d_CView& view ) const
{
#ifdef BUC61045
  Graphic3d_CView MyCView = view;
  if ( call_togl_isgllight( &MyCView ) )
    return Standard_True;
#endif
}

void OpenGl_GraphicDriver :: PrimitiveArray( const Graphic3d_CGroup& ACGroup,
                                             const Graphic3d_PrimitiveArray& parray,
                                                 const Standard_Boolean EvalMinMax )
{
  Graphic3d_CGroup MyCGroup = ACGroup;

  if( parray ) call_togl_parray (&MyCGroup,parray);
}


void OpenGl_GraphicDriver :: UserDraw ( const Graphic3d_CGroup& ACGroup,
                                        const Graphic3d_CUserDraw& AUserDraw )
{
  Graphic3d_CGroup MyCGroup = ACGroup;
  Graphic3d_CUserDraw MyUserDraw = AUserDraw;

  call_togl_userdraw (&MyCGroup,&MyUserDraw);
}

extern int VBOenabled;

void OpenGl_GraphicDriver :: EnableVBO( const Standard_Boolean flag )
{
  VBOenabled = flag;
}
