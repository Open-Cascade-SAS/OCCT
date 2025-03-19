// Created on: 1998-09-30
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <AIS_InteractiveContext.hxx>
#include <Standard_Type.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TPrsStd_AISViewer, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TPrsStd_AISViewer::GetID()
{
  static Standard_GUID TPrsStd_AISViewerID("04fb4d05-5690-11d1-8940-080009dc3333");
  return TPrsStd_AISViewerID;
}

//=================================================================================================

Handle(TPrsStd_AISViewer) TPrsStd_AISViewer::New(const TDF_Label&          acces,
                                                 const Handle(V3d_Viewer)& viewer)
{
  Handle(TPrsStd_AISViewer) V;
  if (acces.Root().FindAttribute(TPrsStd_AISViewer::GetID(), V))
  {
    throw Standard_DomainError("already done");
  }
  V = new TPrsStd_AISViewer();
  V->SetInteractiveContext(new AIS_InteractiveContext(viewer));
  acces.Root().AddAttribute(V);

  // put TPrsStd_AISPresentation attribute at all labels containing
  // TDataXtd_Presentation attribute
  for (TDF_ChildIterator anIt(acces.Root(), Standard_True); anIt.More(); anIt.Next())
  {
    Handle(TDataXtd_Presentation) aPrsData;
    if (anIt.Value().FindAttribute(TDataXtd_Presentation::GetID(), aPrsData))
    {
      TPrsStd_AISPresentation::Set(anIt.Value(), aPrsData->GetDriverGUID());
    }
  }

  return V;
}

//=================================================================================================

Handle(TPrsStd_AISViewer) TPrsStd_AISViewer::New(const TDF_Label&                      acces,
                                                 const Handle(AIS_InteractiveContext)& IC)
{
  Handle(TPrsStd_AISViewer) V;
  if (acces.Root().FindAttribute(TPrsStd_AISViewer::GetID(), V))
  {
    throw Standard_DomainError("already done");
  }
  V = new TPrsStd_AISViewer();
  V->SetInteractiveContext(IC);
  acces.Root().AddAttribute(V);

  // put TPrsStd_AISPresentation attribute at all labels containing
  // TDataXtd_Presentation attribute
  for (TDF_ChildIterator anIt(acces.Root(), Standard_True); anIt.More(); anIt.Next())
  {
    Handle(TDataXtd_Presentation) aPrsData;
    if (anIt.Value().FindAttribute(TDataXtd_Presentation::GetID(), aPrsData))
    {
      TPrsStd_AISPresentation::Set(anIt.Value(), aPrsData->GetDriverGUID());
    }
  }

  return V;
}

//=================================================================================================

Standard_Boolean TPrsStd_AISViewer::Find(const TDF_Label& acces, Handle(TPrsStd_AISViewer)& V)
{
  return (acces.Root().FindAttribute(TPrsStd_AISViewer::GetID(), V));
}

//=================================================================================================

Standard_Boolean TPrsStd_AISViewer::Find(const TDF_Label& acces, Handle(AIS_InteractiveContext)& IC)
{
  Handle(TPrsStd_AISViewer) V;
  if (acces.Root().FindAttribute(TPrsStd_AISViewer::GetID(), V))
  {
    IC = V->GetInteractiveContext();
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean TPrsStd_AISViewer::Find(const TDF_Label& acces, Handle(V3d_Viewer)& VIEW)
{
  Handle(TPrsStd_AISViewer) V;
  if (acces.Root().FindAttribute(TPrsStd_AISViewer::GetID(), V))
  {
    VIEW = V->GetInteractiveContext()->CurrentViewer();
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean TPrsStd_AISViewer::Has(const TDF_Label& acces)
{
  return (acces.Root().IsAttribute(TPrsStd_AISViewer::GetID()));
}

//=================================================================================================

void TPrsStd_AISViewer::Update(const TDF_Label& acces)
{
  Handle(TPrsStd_AISViewer) viewer;
  if (Find(acces, viewer))
    viewer->Update();
}

//=================================================================================================

TPrsStd_AISViewer::TPrsStd_AISViewer() {}

//=================================================================================================

void TPrsStd_AISViewer::Update() const
{
  myInteractiveContext->UpdateCurrentViewer();
}

//=================================================================================================

void TPrsStd_AISViewer::SetInteractiveContext(const Handle(AIS_InteractiveContext)& ctx)
{
  myInteractiveContext = ctx;
}

//=================================================================================================

Handle(AIS_InteractiveContext) TPrsStd_AISViewer::GetInteractiveContext() const
{
  return myInteractiveContext;
}

//=================================================================================================

const Standard_GUID& TPrsStd_AISViewer::ID() const
{
  return GetID();
}

//=================================================================================================

void TPrsStd_AISViewer::Restore(const Handle(TDF_Attribute)&) {}

//=================================================================================================

Handle(TDF_Attribute) TPrsStd_AISViewer::NewEmpty() const
{
  return new TPrsStd_AISViewer();
}

//=================================================================================================

void TPrsStd_AISViewer::Paste(const Handle(TDF_Attribute)&,
                              const Handle(TDF_RelocationTable)&) const
{
}

//=================================================================================================

void TPrsStd_AISViewer::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myInteractiveContext.get())
}
