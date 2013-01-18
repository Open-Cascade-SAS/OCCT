// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <OpenGl_GraphicDriver.hxx>

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

void OpenGl_GraphicDriver::UserDraw (const Graphic3d_CGroup&    theCGroup,
                                     const Graphic3d_CUserDraw& theUserDraw)
{
  if (theCGroup.ptrGroup != NULL
   && myUserDrawCallback != NULL)
  {
    OpenGl_Element* aUserDraw = myUserDrawCallback(&theUserDraw);
    if (aUserDraw != NULL)
    {
      ((OpenGl_Group* )theCGroup.ptrGroup)->AddElement (TelUserdraw, aUserDraw);
    }
  }
}
