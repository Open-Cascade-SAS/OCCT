// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#include <OpenGl_Context.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>
#include <OpenGl_Group.hxx>
#include <OpenGl_CView.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Text.hxx>
#include <OpenGl_Trihedron.hxx>
#include <OpenGl_Workspace.hxx>

#include <Standard_NotImplemented.hxx>

IMPLEMENT_STANDARD_HANDLE(OpenGl_GraphicDriver,Graphic3d_GraphicDriver)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_GraphicDriver,Graphic3d_GraphicDriver)

namespace
{
  // Global switch - shared by whole TKOpenGl module. To be removed.
  static Standard_Boolean TheToUseVbo = Standard_True;

  static const Handle(OpenGl_Context) TheNullGlCtx;
};

// Pour eviter de "mangler" MetaGraphicDriverFactory, le nom de la
// fonction qui cree un Graphic3d_GraphicDriver.
// En effet, ce nom est recherche par la methode DlSymb de la
// classe OSD_SharedLibrary dans la methode SetGraphicDriver de la
// classe Graphic3d_GraphicDevice
extern "C" {
#if defined(_MSC_VER) // disable MS VC++ warning on C-style function returning C++ object
  #pragma warning(push)
  #pragma warning(disable:4190)
#endif
  Standard_EXPORT Handle(Graphic3d_GraphicDriver) MetaGraphicDriverFactory (const Standard_CString theShrName)
  {
    Handle(OpenGl_GraphicDriver) aDriver = new OpenGl_GraphicDriver (theShrName);
    return aDriver;
  }
#if defined(_MSC_VER)
  #pragma warning(pop)
#endif
}

// =======================================================================
// function : OpenGl_GraphicDriver
// purpose  :
// =======================================================================
OpenGl_GraphicDriver::OpenGl_GraphicDriver (const Standard_CString theShrName)
: Graphic3d_GraphicDriver (theShrName),
  myMapOfView      (1, NCollection_BaseAllocator::CommonBaseAllocator()),
  myMapOfWS        (1, NCollection_BaseAllocator::CommonBaseAllocator()),
  myMapOfStructure (1, NCollection_BaseAllocator::CommonBaseAllocator()),
  myUserDrawCallback (NULL),
  myTempText (new OpenGl_Text())
{
  //
}

// =======================================================================
// function : UserDrawCallback
// purpose  :
// =======================================================================
OpenGl_GraphicDriver::OpenGl_UserDrawCallback_t& OpenGl_GraphicDriver::UserDrawCallback()
{
  return myUserDrawCallback;
}

// =======================================================================
// function : DefaultTextHeight
// purpose  :
// =======================================================================
Standard_ShortReal OpenGl_GraphicDriver::DefaultTextHeight() const
{
  return 16.;
}

// =======================================================================
// function : ToUseVBO
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_GraphicDriver::ToUseVBO()
{
  return TheToUseVbo;
}

// =======================================================================
// function : EnableVBO
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::EnableVBO (const Standard_Boolean theToTurnOn)
{
  TheToUseVbo = theToTurnOn;
}

// =======================================================================
// function : GetSharedContext
// purpose  :
// =======================================================================
const Handle(OpenGl_Context)& OpenGl_GraphicDriver::GetSharedContext() const
{
  if (myMapOfWS.IsEmpty())
  {
    return TheNullGlCtx;
  }

  NCollection_DataMap<Standard_Integer, Handle(OpenGl_Workspace)>::Iterator anIter (myMapOfWS);
  return anIter.Value()->GetGlContext();
}

// =======================================================================
// function : MemoryInfo
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_GraphicDriver::MemoryInfo (Standard_Size&           theFreeBytes,
                                                   TCollection_AsciiString& theInfo) const
{
  // this is extra work (for OpenGl_Context initialization)...
  OpenGl_Context aGlCtx;
  if (!aGlCtx.Init())
  {
    return Standard_False;
  }
  theFreeBytes = aGlCtx.AvailableMemory();
  theInfo      = aGlCtx.MemoryInfo();
  return !theInfo.IsEmpty();
}

// =======================================================================
// function : SetImmediateModeDrawToFront
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_GraphicDriver::SetImmediateModeDrawToFront (const Graphic3d_CView& theCView,
                                                                    const Standard_Boolean theDrawToFrontBuffer)
{
  if (theCView.ViewId == -1)
  {
    return Standard_False;
  }

  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL)
  {
    return aCView->WS->SetImmediateModeDrawToFront (theDrawToFrontBuffer);
  }
  return Standard_False;
}

// =======================================================================
// function : BeginAddMode
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_GraphicDriver::BeginAddMode (const Graphic3d_CView& theCView)
{
  if (theCView.ViewId == -1)
  {
    return Standard_False;
  }

  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL && aCView->WS->BeginAddMode())
  {
    myImmediateWS = aCView->WS;
    return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : EndAddMode
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::EndAddMode()
{
  if (!myImmediateWS.IsNull())
  {
    myImmediateWS->EndAddMode();
    myImmediateWS.Nullify();
  }
}

// =======================================================================
// function : BeginImmediatMode
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_GraphicDriver::BeginImmediatMode (const Graphic3d_CView& theCView,
                                                          const Aspect_CLayer2d& theCUnderLayer,
                                                          const Aspect_CLayer2d& theCOverLayer,
                                                          const Standard_Boolean theDoubleBuffer,
                                                          const Standard_Boolean theRetainMode)
{
  if (theCView.ViewId == -1)
  {
    return Standard_False;
  }

  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL && aCView->WS->BeginImmediatMode (theCView, theDoubleBuffer, theRetainMode))
  {
    myImmediateWS = aCView->WS;
    return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : ClearImmediatMode
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::ClearImmediatMode (const Graphic3d_CView& theCView,
                                              const Standard_Boolean theToFlush)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL)
  {
    aCView->WS->ClearImmediatMode (theCView, theToFlush);
  }
}

// =======================================================================
// function : DrawStructure
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::DrawStructure (const Graphic3d_CStructure& theCStructure)
{
  OpenGl_Structure* aStructure = (OpenGl_Structure* )theCStructure.ptrStructure;
  if (aStructure == NULL)
  {
    return;
  }

  if (!myImmediateWS.IsNull())
  {
    myImmediateWS->DrawStructure (aStructure);
  }
}

// =======================================================================
// function : EndImmediatMode
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::EndImmediatMode (const Standard_Integer )
{
  if (!myImmediateWS.IsNull())
  {
    myImmediateWS->EndImmediatMode();
    myImmediateWS.Nullify();
  }
}

// =======================================================================
// function : Print
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_GraphicDriver::Print (const Graphic3d_CView& theCView,
                                              const Aspect_CLayer2d& theCUnderLayer,
                                              const Aspect_CLayer2d& theCOverLayer,
                                              const Aspect_Handle    thePrintDC,
                                              const Standard_Boolean theToShowBackground,
                                              const Standard_CString theFilename,
                                              const Aspect_PrintAlgo thePrintAlgorithm,
                                              const Standard_Real    theScaleFactor) const
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView == NULL
   || !myPrintContext.IsNull())
  {
    return Standard_False;
  }

  Standard_Boolean isPrinted = Standard_False;
  myPrintContext = new OpenGl_PrinterContext();
#ifdef _WIN32
  isPrinted = aCView->WS->Print (myPrintContext,
                                 theCView,
                                 theCUnderLayer,
                                 theCOverLayer,
                                 thePrintDC,
                                 theToShowBackground,
                                 theFilename,
                                 thePrintAlgorithm,
                                 theScaleFactor);
#else
  Standard_NotImplemented::Raise ("OpenGl_GraphicDriver::Print is implemented only on Windows");
#endif
  myPrintContext.Nullify();
  return isPrinted;
}

// =======================================================================
// function : Text
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::Text (const Graphic3d_CGroup&                 theCGroup,
                                 const TCollection_ExtendedString&       theText,
                                 const Graphic3d_Vertex&                 thePoint,
                                 const Standard_Real                     theHeight,
                                 const Quantity_PlaneAngle               /*theAngle*/,
                                 const Graphic3d_TextPath                /*theTp*/,
                                 const Graphic3d_HorizontalTextAlignment theHta,
                                 const Graphic3d_VerticalTextAlignment   theVta,
                                 const Standard_Boolean                  /*theToEvalMinMax*/)
{
  if (theCGroup.ptrGroup == NULL)
  {
    return;
  }

  OpenGl_TextParam aParams;
  aParams.Height = int ((theHeight < 2.0) ? DefaultTextHeight() : theHeight);
  aParams.HAlign = theHta;
  aParams.VAlign = theVta;
  const OpenGl_Vec3 aPoint (thePoint.X(), thePoint.Y(), thePoint.Z());
  OpenGl_Text* aText = new OpenGl_Text (theText, aPoint, aParams);
  ((OpenGl_Group* )theCGroup.ptrGroup)->AddElement (TelText, aText);
}

// =======================================================================
// function : Text
// purpose  : Wrapper CString -> TCollection_ExtendedString
// =======================================================================
void OpenGl_GraphicDriver::Text (const Graphic3d_CGroup&                 theCGroup,
                                 const Standard_CString                  theText,
                                 const Graphic3d_Vertex&                 thePoint,
                                 const Standard_Real                     theHeight,
                                 const Quantity_PlaneAngle               theAngle,
                                 const Graphic3d_TextPath                theTp,
                                 const Graphic3d_HorizontalTextAlignment theHta,
                                 const Graphic3d_VerticalTextAlignment   theVta,
                                 const Standard_Boolean                  theToEvalMinMax)
{
  OpenGl_GraphicDriver::Text (theCGroup, TCollection_ExtendedString (theText),
                              thePoint, theHeight, theAngle, theTp, theHta, theVta, theToEvalMinMax);
}

// =======================================================================
// function : Text
// purpose  : Wrapper CString -> TCollection_ExtendedString
// =======================================================================
void OpenGl_GraphicDriver::Text (const Graphic3d_CGroup& theCGroup,
                                 const Standard_CString  theText,
                                 const Graphic3d_Vertex& thePoint,
                                 const Standard_Real     theHeight,
                                 const Standard_Boolean  theToEvalMinMax)
{
  OpenGl_GraphicDriver::Text (theCGroup, TCollection_ExtendedString (theText), thePoint, theHeight, theToEvalMinMax);
}

// =======================================================================
// function : Text
// purpose  : Wrapper with default values
// =======================================================================
void OpenGl_GraphicDriver::Text (const Graphic3d_CGroup&           theCGroup,
                                 const TCollection_ExtendedString& theText,
                                 const Graphic3d_Vertex&           thePoint,
                                 const Standard_Real               theHeight,
                                 const Standard_Boolean            theToEvalMinMax)
{
  OpenGl_GraphicDriver::Text (theCGroup,
                              theText, thePoint, theHeight, 0.0,
                              Graphic3d_TP_RIGHT, Graphic3d_HTA_LEFT, Graphic3d_VTA_BOTTOM,
                              theToEvalMinMax);
}

// =======================================================================
// function : ZBufferTriedronSetup
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::ZBufferTriedronSetup (const Quantity_NameOfColor theXColor,
                                                 const Quantity_NameOfColor theYColor,
                                                 const Quantity_NameOfColor theZColor,
                                                 const Standard_Real        theSizeRatio,
                                                 const Standard_Real        theAxisDiametr,
                                                 const Standard_Integer     theNbFacettes)
{
  OpenGl_Trihedron::Setup (theXColor, theYColor, theZColor, theSizeRatio, theAxisDiametr, theNbFacettes);
}

// =======================================================================
// function : TriedronDisplay
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::TriedronDisplay (const Graphic3d_CView&              theCView,
                                            const Aspect_TypeOfTriedronPosition thePosition,
                                            const Quantity_NameOfColor          theColor,
                                            const Standard_Real                 theScale,
                                            const Standard_Boolean              theAsWireframe)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL)
  {
    aCView->View->TriedronDisplay (aCView->WS->GetGlContext(), thePosition, theColor, theScale, theAsWireframe);
  }
}

// =======================================================================
// function : TriedronErase
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::TriedronErase (const Graphic3d_CView& theCView)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL)
  {
    aCView->View->TriedronErase (aCView->WS->GetGlContext());
  }
}

// =======================================================================
// function : TriedronEcho
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::TriedronEcho (const Graphic3d_CView& ,
                                         const Aspect_TypeOfTriedronEcho )
{
  // do nothing
}

// =======================================================================
// function : Environment
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::Environment (const Graphic3d_CView& theCView)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView == NULL)
  {
    return;
  }

  aCView->View->SetTextureEnv    (aCView->WS->GetGlContext(), theCView.Context.TextureEnv);
  aCView->View->SetSurfaceDetail ((Visual3d_TypeOfSurfaceDetail)theCView.Context.SurfaceDetail);
}

// =======================================================================
// function : BackgroundImage
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::BackgroundImage (const Standard_CString  theFileName,
                                            const Graphic3d_CView&  theCView,
                                            const Aspect_FillMethod theFillStyle)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL)
  {
    aCView->View->CreateBackgroundTexture (theFileName, theFillStyle);
  }
}

// =======================================================================
// function : SetBgImageStyle
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::SetBgImageStyle (const Graphic3d_CView&  theCView,
                                            const Aspect_FillMethod theFillStyle)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL)
  {
    aCView->View->SetBackgroundTextureStyle (theFillStyle);
  }
}

// =======================================================================
// function : SetBgGradientStyle
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::SetBgGradientStyle (const Graphic3d_CView&          theCView,
                                               const Aspect_GradientFillMethod theFillType)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL)
  {
    aCView->View->SetBackgroundGradientType (theFillType);
  }
}

// =======================================================================
// function : GraduatedTrihedronDisplay
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::GraduatedTrihedronDisplay (const Graphic3d_CView&               theCView,
                                                      const Graphic3d_CGraduatedTrihedron& theCubic)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL)
  {
    aCView->View->GraduatedTrihedronDisplay (aCView->WS->GetGlContext(), theCubic);
  }
}

// =======================================================================
// function : GraduatedTrihedronErase
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::GraduatedTrihedronErase (const Graphic3d_CView& theCView)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL)
  {
    aCView->View->GraduatedTrihedronErase (aCView->WS->GetGlContext());
  }
}

// =======================================================================
// function : GraduatedTrihedronMinMaxValues
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::GraduatedTrihedronMinMaxValues (const Standard_ShortReal theMinX,
                                                           const Standard_ShortReal theMinY,
                                                           const Standard_ShortReal theMinZ,
                                                           const Standard_ShortReal theMaxX,
                                                           const Standard_ShortReal theMaxY,
                                                           const Standard_ShortReal theMaxZ)
{
  OpenGl_GraduatedTrihedron::SetMinMax (theMinX, theMinY, theMinZ, theMaxX, theMaxY, theMaxZ);
}
