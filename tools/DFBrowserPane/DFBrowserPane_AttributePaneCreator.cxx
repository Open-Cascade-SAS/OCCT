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

// the types are defined in TDF library
#include <inspector/DFBrowserPane_TDFReference.hxx>
#include <inspector/DFBrowserPane_TDFTagSource.hxx>

#include <TDataStd_Current.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_Directory.hxx>
#include <TDataStd_NoteBook.hxx>
#include <TDataStd_Expression.hxx>
#include <TDataStd_Relation.hxx>
#include <TDataStd_Variable.hxx>
#include <TDataStd_Tick.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <TDataStd_IntegerList.hxx>
#include <TDataStd_RealList.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_BooleanList.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDataStd_NamedData.hxx>

#include <TDocStd_Modified.hxx>
#include <TDocStd_Owner.hxx>
#include <TDocStd_XLink.hxx>
#include <TDocStd_XLinkRoot.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <TPrsStd_AISPresentation.hxx>

#include <TNaming_NamedShape.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_UsedShapes.hxx>

#include <TFunction_Function.hxx>
#include <TFunction_GraphNode.hxx>
#include <TFunction_Scope.hxx>

// the types are defined in TDataStd.cdl
// Basic  attributes
#include <inspector/DFBrowserPane_TDataStdCurrent.hxx>
#include <inspector/DFBrowserPane_TDataStdName.hxx>
#include <inspector/DFBrowserPane_TDataStdComment.hxx>
#include <inspector/DFBrowserPane_TDataStdInteger.hxx>
#include <inspector/DFBrowserPane_TDataStdIntegerArray.hxx>
#include <inspector/DFBrowserPane_TDataStdReal.hxx>
#include <inspector/DFBrowserPane_TDataStdRealArray.hxx>
#include <inspector/DFBrowserPane_TDataStdExtStringArray.hxx>
#include <inspector/DFBrowserPane_TDataStdUAttribute.hxx>

#include <inspector/DFBrowserPane_TDataStdTreeNode.hxx>
#include <inspector/DFBrowserPane_TDataStdDirectory.hxx>
#include <inspector/DFBrowserPane_TDataStdNoteBook.hxx>
#include <inspector/DFBrowserPane_TDataStdExpression.hxx>
#include <inspector/DFBrowserPane_TDataStdRelation.hxx>
#include <inspector/DFBrowserPane_TDataStdVariable.hxx>

//Extension
#include <inspector/DFBrowserPane_TDataStdTick.hxx>
#include <inspector/DFBrowserPane_TDataStdAsciiString.hxx>
#include <inspector/DFBrowserPane_TDataStdIntPackedMap.hxx>
// Lists:
#include <inspector/DFBrowserPane_TDataStdIntegerList.hxx>
#include <inspector/DFBrowserPane_TDataStdRealList.hxx>
#include <inspector/DFBrowserPane_TDataStdExtStringList.hxx>
#include <inspector/DFBrowserPane_TDataStdBooleanList.hxx>
#include <inspector/DFBrowserPane_TDataStdReferenceList.hxx>

// Arrays:
#include <inspector/DFBrowserPane_TDataStdBooleanArray.hxx>
#include <inspector/DFBrowserPane_TDataStdReferenceArray.hxx>
#include <inspector/DFBrowserPane_TDataStdByteArray.hxx>
#include <inspector/DFBrowserPane_TDataStdNamedData.hxx>

// TDocStd attributes
#include <inspector/DFBrowserPane_TDocStdModified.hxx>
#include <inspector/DFBrowserPane_TDocStdOwner.hxx>
#include <inspector/DFBrowserPane_TDocStdXLink.hxx>
#include <inspector/DFBrowserPane_TDocStdXLinkRoot.hxx>

// TPrsStd attributes
#include <inspector/DFBrowserPane_TPrsStdAISViewer.hxx>
#include <inspector/DFBrowserPane_TPrsStdAISPresentation.hxx>

// TNaming attributes
#include <inspector/DFBrowserPane_TNamingNamedShape.hxx>
#include <inspector/DFBrowserPane_TNamingNaming.hxx>
#include <inspector/DFBrowserPane_TNamingUsedShapes.hxx>

// TFunction attributes
#include <inspector/DFBrowserPane_TFunctionFunction.hxx>
#include <inspector/DFBrowserPane_TFunctionGraphNode.hxx>
#include <inspector/DFBrowserPane_TFunctionScope.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPane_AttributePaneAPI* DFBrowserPane_AttributePaneCreator::CreateAttributePane (
                                                                        const Standard_CString& theAttributeName)
{
  DFBrowserPane_AttributePaneAPI* aPane = 0;
  if (theAttributeName == STANDARD_TYPE (TDF_Reference)->Name())
    aPane = new DFBrowserPane_TDFReference();
  else if (theAttributeName == STANDARD_TYPE (TDF_TagSource)->Name())
    aPane = new DFBrowserPane_TDFTagSource();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_Current)->Name()) // Basic  attributes
    aPane = new DFBrowserPane_TDataStdCurrent();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_Name)->Name())
    aPane = new DFBrowserPane_TDataStdName();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_Comment)->Name())
    aPane = new DFBrowserPane_TDataStdComment();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_Integer)->Name())
    aPane = new DFBrowserPane_TDataStdInteger();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_IntegerArray)->Name())
    aPane = new DFBrowserPane_TDataStdIntegerArray();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_Real)->Name())
    aPane = new DFBrowserPane_TDataStdReal();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_RealArray)->Name())
    aPane = new DFBrowserPane_TDataStdRealArray();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_ExtStringArray)->Name())
    aPane = new DFBrowserPane_TDataStdExtStringArray();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_UAttribute)->Name())
    aPane = new DFBrowserPane_TDataStdUAttribute();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_TreeNode)->Name()) // Attributes for organization
    aPane = new DFBrowserPane_TDataStdTreeNode();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_Directory)->Name())
    aPane = new DFBrowserPane_TDataStdDirectory();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_NoteBook)->Name()) // Other attributes
    aPane = new DFBrowserPane_TDataStdNoteBook();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_Expression)->Name())
    aPane = new DFBrowserPane_TDataStdExpression();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_Relation)->Name())
    aPane = new DFBrowserPane_TDataStdRelation();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_Variable)->Name())
    aPane = new DFBrowserPane_TDataStdVariable();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_Tick)->Name()) //Extension
    aPane = new DFBrowserPane_TDataStdTick();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_AsciiString)->Name())
    aPane = new DFBrowserPane_TDataStdAsciiString();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_IntPackedMap)->Name())
    aPane = new DFBrowserPane_TDataStdIntPackedMap();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_IntegerList)->Name()) // Lists
    aPane = new DFBrowserPane_TDataStdIntegerList();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_RealList)->Name())
    aPane = new DFBrowserPane_TDataStdRealList();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_ExtStringList)->Name())
    aPane = new DFBrowserPane_TDataStdExtStringList();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_BooleanList)->Name())
    aPane = new DFBrowserPane_TDataStdBooleanList();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_ReferenceList)->Name())
    aPane = new DFBrowserPane_TDataStdReferenceList();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_BooleanArray)->Name()) // Arrays:
    aPane = new DFBrowserPane_TDataStdBooleanArray();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_ReferenceArray)->Name())
    aPane = new DFBrowserPane_TDataStdReferenceArray();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_ByteArray)->Name())
    aPane = new DFBrowserPane_TDataStdByteArray();
  else if (theAttributeName == STANDARD_TYPE (TDataStd_NamedData)->Name())
    aPane = new DFBrowserPane_TDataStdNamedData();
  else if (theAttributeName == STANDARD_TYPE (TDocStd_Modified)->Name()) // TDocStd attributes
    aPane = new DFBrowserPane_TDocStdModified();
  else if (theAttributeName == STANDARD_TYPE (TDocStd_Owner)->Name())
    aPane = new DFBrowserPane_TDocStdOwner();
  else if (theAttributeName == STANDARD_TYPE (TDocStd_XLink)->Name())
    aPane = new DFBrowserPane_TDocStdXLink();
  else if (theAttributeName == STANDARD_TYPE (TDocStd_XLinkRoot)->Name())
    aPane = new DFBrowserPane_TDocStdXLinkRoot();
  else if (theAttributeName == STANDARD_TYPE (TPrsStd_AISViewer)->Name()) // TPrsStd attributes
    aPane = new DFBrowserPane_TPrsStdAISViewer();
  else if (theAttributeName == STANDARD_TYPE (TPrsStd_AISPresentation)->Name())
    aPane = new DFBrowserPane_TPrsStdAISPresentation();
  else if (theAttributeName == STANDARD_TYPE (TNaming_NamedShape)->Name()) // TNaming attributes
    aPane = new DFBrowserPane_TNamingNamedShape();
  else if (theAttributeName == STANDARD_TYPE (TNaming_Naming)->Name())
    aPane = new DFBrowserPane_TNamingNaming();
  else if (theAttributeName == STANDARD_TYPE (TNaming_UsedShapes)->Name())
    aPane = new DFBrowserPane_TNamingUsedShapes();
  else if (theAttributeName == STANDARD_TYPE (TFunction_Function)->Name()) // TFunction attributes
    aPane = new DFBrowserPane_TFunctionFunction();
  else if (theAttributeName == STANDARD_TYPE (TFunction_GraphNode)->Name())
    aPane = new DFBrowserPane_TFunctionGraphNode();
  else if (theAttributeName == STANDARD_TYPE (TFunction_Scope)->Name())
    aPane = new DFBrowserPane_TFunctionScope();

  return aPane;
}
