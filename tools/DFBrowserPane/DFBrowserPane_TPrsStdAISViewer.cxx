// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <inspector/DFBrowserPane_TPrsStdAISViewer.hxx>
#include <inspector/DFBrowserPane_Tools.hxx>

#include <AIS_InteractiveContext.hxx>
#include <TPrsStd_AISViewer.hxx>

#include <QVariant>

// =======================================================================
// function : 
// purpose :
// =======================================================================
void DFBrowserPane_TPrsStdAISViewer::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  Handle(TPrsStd_AISViewer) aViewerAttribute = Handle(TPrsStd_AISViewer)::DownCast (theAttribute);
  if (!aViewerAttribute)
    return;

  Handle(AIS_InteractiveContext) aContext = aViewerAttribute->GetInteractiveContext();
  TCollection_AsciiString aPointerInfo = !aContext.IsNull()
    ? DFBrowserPane_Tools::GetPointerInfo (aContext).ToCString() : "";

  theValues << "GetInteractiveContext" << aPointerInfo.ToCString();

}
