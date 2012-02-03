// File:      OpenGl_GraphicDriver_713.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GraphicDriver.hxx>

#include <OpenGl_Callback.hxx>
#include <OpenGl_Group.hxx>
#include <OpenGl_PrimitiveArray.hxx>

#include <OpenGl_CView.hxx>

void OpenGl_GraphicDriver::SetDepthTestEnabled( const Graphic3d_CView& ACView, const Standard_Boolean isEnabled ) const
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->WS->UseDepthTest() = isEnabled;
}

Standard_Boolean OpenGl_GraphicDriver::IsDepthTestEnabled( const Graphic3d_CView& ACView ) const
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    return aCView->WS->UseDepthTest();
  return Standard_False;
}

void OpenGl_GraphicDriver::ReadDepths( const Graphic3d_CView& ACView,
                                      const Standard_Integer x,
                                      const Standard_Integer y,
                                      const Standard_Integer width,
                                      const Standard_Integer height,
                                      const Standard_Address buffer ) const
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->WS->ReadDepths(x, y, width, height, (float*) buffer);
}

void OpenGl_GraphicDriver::SetGLLightEnabled( const Graphic3d_CView& ACView, const Standard_Boolean isEnabled ) const
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->WS->UseGLLight() = isEnabled;
}

Standard_Boolean OpenGl_GraphicDriver::IsGLLightEnabled( const Graphic3d_CView& ACView ) const
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    return aCView->WS->UseGLLight();
  return Standard_False;
}

void OpenGl_GraphicDriver::PrimitiveArray( const Graphic3d_CGroup& ACGroup,
                                          const Graphic3d_PrimitiveArray& parray,
                                          const Standard_Boolean EvalMinMax )
{
  if ( ACGroup.ptrGroup && parray )
  {
    OpenGl_PrimitiveArray *aparray = new OpenGl_PrimitiveArray( (CALL_DEF_PARRAY *) parray );
    ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelParray, aparray );
  }
}

//=======================================================================
//function : RemovePrimitiveArray
//purpose  : Purpose: Clear visualization data in graphical driver and
//           stop displaying the primitives array of the graphical group
//           <theCGroup>. This method is internal and should be used by
//           Graphic3d_Group only.
//=======================================================================

void OpenGl_GraphicDriver::RemovePrimitiveArray (const Graphic3d_CGroup&         ACGroup,
                                                const Graphic3d_PrimitiveArray& thePArray)
{
  if ( ACGroup.ptrGroup && thePArray )
  {
    ((OpenGl_Group *)ACGroup.ptrGroup)->RemovePrimitiveArray( (CALL_DEF_PARRAY *) thePArray );
  }
}

static OpenGl_UserDrawCallback MyUserDrawCallback = NULL;

OpenGl_UserDrawCallback& UserDrawCallback ()
{
  return MyUserDrawCallback;
}

void OpenGl_GraphicDriver::UserDraw ( const Graphic3d_CGroup& ACGroup,
                                     const Graphic3d_CUserDraw& AUserDraw )
{
  if (ACGroup.ptrGroup && MyUserDrawCallback)
  {
    OpenGl_Element *auserdraw = (*MyUserDrawCallback)(&AUserDraw);

    if (auserdraw != 0)
      ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelUserdraw, auserdraw );
  }
}
