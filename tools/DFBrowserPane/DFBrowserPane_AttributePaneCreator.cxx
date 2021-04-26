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

#include <inspector/DFBrowserPane_AttributePaneCreator.hxx>

#include <TDF_Attribute.hxx>
#include <TDF_Reference.hxx>

#include <TDataStd_TreeNode.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <TDataStd_ReferenceArray.hxx>

#include <TNaming_NamedShape.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_UsedShapes.hxx>

#include <inspector/DFBrowserPane_TDFReference.hxx>
#include <inspector/DFBrowserPane_TDataStdReferenceList.hxx>
#include <inspector/DFBrowserPane_TDataStdReferenceArray.hxx>
#include <inspector/DFBrowserPane_TDataStdTreeNode.hxx>
#include <inspector/DFBrowserPane_TNamingNamedShape.hxx>
#include <inspector/DFBrowserPane_TNamingNaming.hxx>
#include <inspector/DFBrowserPane_TNamingUsedShapes.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_AttributePaneAPI* DFBrowserPane_AttributePaneCreator::CreateAttributePane (Standard_CString theAttributeName)
{
  DFBrowserPane_AttributePaneAPI* aPane = 0;
  if (theAttributeName == STANDARD_TYPE (TDF_Reference)->Name())
    aPane = new DFBrowserPane_TDFReference();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_TreeNode)->Name())
    aPane = new DFBrowserPane_TDataStdTreeNode();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_ReferenceList)->Name())
    aPane = new DFBrowserPane_TDataStdReferenceList();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_ReferenceArray)->Name())
    aPane = new DFBrowserPane_TDataStdReferenceArray();
  else if (theAttributeName == STANDARD_TYPE (TNaming_NamedShape)->Name())
    aPane = new DFBrowserPane_TNamingNamedShape();
  else if (theAttributeName == STANDARD_TYPE (TNaming_Naming)->Name())
    aPane = new DFBrowserPane_TNamingNaming();
  else if (theAttributeName == STANDARD_TYPE (TNaming_UsedShapes)->Name())
    aPane = new DFBrowserPane_TNamingUsedShapes();

  return aPane;
}
