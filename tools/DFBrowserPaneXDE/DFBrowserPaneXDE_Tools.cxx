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

#include <inspector/DFBrowserPaneXDE_Tools.hxx>

#include <inspector/DFBrowserPane_AttributePaneAPI.hxx>

#include <TCollection_AsciiString.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>

#include <Standard_GUID.hxx>

#include <XCAFDoc_Area.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <XCAFDoc_Color.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_Location.hxx>
#include <XCAFDoc_Material.hxx>
#include <XCAFDoc_Volume.hxx>

namespace DFBrowserPaneXDE_Tools
{

  // =======================================================================
  // function : IsXDEApplication
  // purpose :
  // =======================================================================
  bool IsXDEApplication (const Handle(TDocStd_Application)& theApplication)
  {
    bool isXDEApp = false;

    Handle(TDocStd_Document) aDocument;
    Standard_Integer aNbDoc = theApplication->NbDocuments();
    if (aNbDoc == 0)
      return isXDEApp;

    theApplication->GetDocument (1, aDocument);
    if (aDocument.IsNull())
      return isXDEApp;

    for (TDF_ChildIterator aLabelsIt (aDocument->Main().Root()); aLabelsIt.More() && !isXDEApp; aLabelsIt.Next())
    {
      const TDF_Label aLabel = aLabelsIt.Value();
      if (aLabel.IsNull())
        break;
      Handle(TDF_Attribute) anAttribute;
      aLabel.FindAttribute (XCAFDoc_DocumentTool::GetID(), anAttribute);
      isXDEApp = !anAttribute.IsNull();
    }
    return isXDEApp;
  }
}
