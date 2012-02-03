// File:      OpenGl_GraphicDriver_4.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GraphicDriver.hxx>

#include <NCollection_DataMap.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_CView.hxx>

void OpenGl_GraphicDriver::ClearStructure (const Graphic3d_CStructure& theCStructure)
{
  OpenGl_Structure* aStructure = (OpenGl_Structure* )theCStructure.ptrStructure;
  if (aStructure == NULL)
    return;

  aStructure->Clear();
  InvalidateAllWorkspaces();
}

void OpenGl_GraphicDriver::ContextStructure (const Graphic3d_CStructure& theCStructure)
{
  OpenGl_Structure* aStructure = (OpenGl_Structure* )theCStructure.ptrStructure;
  if (aStructure == NULL)
    return;

  aStructure->SetTransformPersistence (theCStructure.TransformPersistence);

  if (theCStructure.ContextLine.IsDef)
    aStructure->SetAspectLine (theCStructure.ContextLine);

  if (theCStructure.ContextFillArea.IsDef)
    aStructure->SetAspectFace (theCStructure.ContextFillArea);

  if (theCStructure.ContextMarker.IsDef)
    aStructure->SetAspectMarker (theCStructure.ContextMarker);

  if (theCStructure.ContextText.IsDef)
    aStructure->SetAspectText (theCStructure.ContextText);

  InvalidateAllWorkspaces();
}

void OpenGl_GraphicDriver::Connect (const Graphic3d_CStructure& theFather,
                                    const Graphic3d_CStructure& theSon)
{
  OpenGl_Structure* aFather = (OpenGl_Structure* )theFather.ptrStructure;
  OpenGl_Structure* aSon = (OpenGl_Structure* )theSon.ptrStructure;
  if (aFather && aSon)
    aFather->Connect(aSon);
}

void OpenGl_GraphicDriver::Disconnect (const Graphic3d_CStructure& theFather,
                                       const Graphic3d_CStructure& theSon)
{
  OpenGl_Structure* aFather = (OpenGl_Structure* )theFather.ptrStructure;
  OpenGl_Structure* aSon = (OpenGl_Structure* )theSon.ptrStructure;
  if (aFather && aSon)
    aFather->Disconnect(aSon);
}

void OpenGl_GraphicDriver::DisplayStructure (const Graphic3d_CView&      theCView,
                                             const Graphic3d_CStructure& theCStructure,
                                             const Standard_Integer      thePriority)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  OpenGl_Structure* aStructure = (OpenGl_Structure* )theCStructure.ptrStructure;
  if (aCView == NULL || aStructure == NULL)
    return;

  aCView->View->DisplayStructure (aStructure, thePriority);
  aCView->WS->Invalidate();
}

void OpenGl_GraphicDriver::EraseStructure (const Graphic3d_CView&      theCView,
                                           const Graphic3d_CStructure& theCStructure)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  OpenGl_Structure* aStructure = (OpenGl_Structure* )theCStructure.ptrStructure;
  if (aCView == NULL || aStructure == NULL)
    return;

  aCView->View->EraseStructure (aStructure);
  aCView->WS->Invalidate();
}

void OpenGl_GraphicDriver::RemoveStructure (const Graphic3d_CStructure& theCStructure)
{
  if (!GetMapOfStructures().IsBound (theCStructure.Id))
    return;

  OpenGl_Structure* aStructure = OpenGl_GraphicDriver::GetMapOfStructures().Find (theCStructure.Id);
  OpenGl_GraphicDriver::GetMapOfStructures().UnBind (theCStructure.Id);
  delete aStructure;
  InvalidateAllWorkspaces();
}

void OpenGl_GraphicDriver::Structure (Graphic3d_CStructure& theCStructure)
{
  RemoveStructure (theCStructure);

  OpenGl_Structure* aStructure = new OpenGl_Structure();

  aStructure->SetDegenerateModel (theCStructure.ContextFillArea.DegenerationMode,
                                  theCStructure.ContextFillArea.SkipRatio);

  Standard_Integer aStatus = 0;
  if (theCStructure.highlight) aStatus |= OPENGL_NS_HIGHLIGHT;
  if (!theCStructure.visible)  aStatus |= OPENGL_NS_HIDE;
  if (theCStructure.pick)      aStatus |= OPENGL_NS_PICK;
  aStructure->SetNamedStatus (aStatus);

  theCStructure.ptrStructure = aStructure;
  OpenGl_GraphicDriver::GetMapOfStructures().Bind (theCStructure.Id, aStructure);
  InvalidateAllWorkspaces();
}
