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

#include <NCollection_DataMap.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_CView.hxx>

void OpenGl_GraphicDriver::ClearStructure (const Graphic3d_CStructure& theCStructure)
{
  OpenGl_Structure* aStructure = (OpenGl_Structure* )theCStructure.ptrStructure;
  if (aStructure == NULL)
    return;

  aStructure->Clear (GetSharedContext());
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
    aStructure->SetAspectFace (GetSharedContext(), theCStructure.ContextFillArea);

  if (theCStructure.ContextMarker.IsDef)
    aStructure->SetAspectMarker (theCStructure.ContextMarker);

  if (theCStructure.ContextText.IsDef)
    aStructure->SetAspectText (theCStructure.ContextText);
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
}

void OpenGl_GraphicDriver::EraseStructure (const Graphic3d_CView&      theCView,
                                           const Graphic3d_CStructure& theCStructure)
{
  const OpenGl_CView* aCView = (const OpenGl_CView* )theCView.ptrView;
  OpenGl_Structure* aStructure = (OpenGl_Structure* )theCStructure.ptrStructure;
  if (aCView == NULL || aStructure == NULL)
    return;

  aCView->View->EraseStructure (aStructure);
}

void OpenGl_GraphicDriver::RemoveStructure (const Graphic3d_CStructure& theCStructure)
{
  if (!myMapOfStructure.IsBound (theCStructure.Id))
    return;

  OpenGl_Structure* aStructure = myMapOfStructure.Find (theCStructure.Id);
  myMapOfStructure.UnBind (theCStructure.Id);
  OpenGl_Element::Destroy (GetSharedContext(), aStructure);
}

void OpenGl_GraphicDriver::Structure (Graphic3d_CStructure& theCStructure)
{
  RemoveStructure (theCStructure);

  OpenGl_Structure* aStructure = new OpenGl_Structure();

  Standard_Integer aStatus = 0;
  if (theCStructure.highlight) aStatus |= OPENGL_NS_HIGHLIGHT;
  if (!theCStructure.visible)  aStatus |= OPENGL_NS_HIDE;
  aStructure->SetNamedStatus (aStatus);

  theCStructure.ptrStructure = aStructure;
  myMapOfStructure.Bind (theCStructure.Id, aStructure);
}

//=======================================================================
//function : ChangeZLayer
//purpose  :
//=======================================================================

void OpenGl_GraphicDriver::ChangeZLayer (const Graphic3d_CStructure& theCStructure,
                                         const Standard_Integer theLayer)
{
  if (!myMapOfStructure.IsBound (theCStructure.Id))
    return;

  OpenGl_Structure* aStructure = myMapOfStructure.Find (theCStructure.Id);

  aStructure->SetZLayer (theLayer);
}

//=======================================================================
//function : ChangeZLayer
//purpose  :
//=======================================================================

void OpenGl_GraphicDriver::ChangeZLayer (const Graphic3d_CStructure& theCStructure,
                                         const Graphic3d_CView& theCView,
                                         const Standard_Integer theNewLayerId)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)theCView.ptrView;

  if (!myMapOfStructure.IsBound (theCStructure.Id) || !aCView)
    return;

  OpenGl_Structure* aStructure = myMapOfStructure.Find (theCStructure.Id);

  aCView->View->ChangeZLayer (aStructure, theNewLayerId);
}

//=======================================================================
//function : GetZLayer
//purpose  :
//=======================================================================

Standard_Integer OpenGl_GraphicDriver::GetZLayer (const Graphic3d_CStructure& theCStructure) const
{
  if (!myMapOfStructure.IsBound (theCStructure.Id))
    return -1;

  OpenGl_Structure* aStructure = myMapOfStructure.Find (theCStructure.Id);

  return aStructure->GetZLayer();
}

//=======================================================================
//function : UnsetZLayer
//purpose  :
//=======================================================================

void OpenGl_GraphicDriver::UnsetZLayer (const Standard_Integer theLayerId)
{
  NCollection_DataMap<Standard_Integer, OpenGl_Structure*>::Iterator aStructIt (myMapOfStructure);
  for( ; aStructIt.More (); aStructIt.Next ())
  {
    OpenGl_Structure* aStruct = aStructIt.ChangeValue ();
    if (aStruct->GetZLayer () == theLayerId)
      aStruct->SetZLayer (0);
  }
}
