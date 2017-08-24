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

#include <inspector/DFBrowserPaneXDE_AttributeCommonPane.hxx>

#include <inspector/DFBrowserPane_ItemRole.hxx>
#include <inspector/DFBrowserPaneXDE_XDEDRAW.hxx>

#include <TDataStd_TreeNode.hxx>
#include <TDF_Reference.hxx>
#include <TNaming_NamedShape.hxx>
#include <XCAFDoc_Volume.hxx>
#include <XCAFDoc_Area.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <TDataStd_UAttribute.hxx>
#include <XCAFDoc_Color.hxx>
#include <XCAFDoc_DimTol.hxx>
#include <XCAFDoc_Material.hxx>
#include <XCAFDoc_GraphNode.hxx>

#include <set>

#include <QStringList>

static std::set<Standard_CString> AttributeTypes;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
DFBrowserPaneXDE_AttributeCommonPane::DFBrowserPaneXDE_AttributeCommonPane (DFBrowserPane_AttributePaneAPI* theStandardPane)
: DFBrowserPane_AttributePane()
{
  myStandardPane = dynamic_cast<DFBrowserPane_AttributePane*> (theStandardPane);
}

// =======================================================================
// function : ProcessAttribute
// purpose :
// =======================================================================
bool DFBrowserPaneXDE_AttributeCommonPane::ProcessAttribute (const Standard_CString& theAttributeType)
{
  if (AttributeTypes.empty())
  {
    AttributeTypes.insert (STANDARD_TYPE (TDataStd_TreeNode)->Name());
    AttributeTypes.insert (STANDARD_TYPE (TDF_Reference)->Name());
    AttributeTypes.insert (STANDARD_TYPE (TNaming_NamedShape)->Name());
    AttributeTypes.insert (STANDARD_TYPE (XCAFDoc_Volume)->Name());
    AttributeTypes.insert (STANDARD_TYPE (XCAFDoc_Area)->Name());
    AttributeTypes.insert (STANDARD_TYPE (XCAFDoc_Centroid)->Name());
    AttributeTypes.insert (STANDARD_TYPE (TDataStd_UAttribute)->Name());
    AttributeTypes.insert (STANDARD_TYPE (XCAFDoc_Color)->Name());
    AttributeTypes.insert (STANDARD_TYPE (XCAFDoc_DimTol)->Name());
    AttributeTypes.insert (STANDARD_TYPE (XCAFDoc_Material)->Name());
    AttributeTypes.insert (STANDARD_TYPE (XCAFDoc_GraphNode)->Name());
  }
  return AttributeTypes.find (theAttributeType) != AttributeTypes.end();
}

// =======================================================================
// function : CreateWidget
// purpose :
// =======================================================================
QWidget* DFBrowserPaneXDE_AttributeCommonPane::CreateWidget (QWidget* theParent)
{
  return myStandardPane ? myStandardPane->CreateWidget (theParent) : 0;
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void DFBrowserPaneXDE_AttributeCommonPane::Init (const Handle(TDF_Attribute)& theAttribute)
{
  if (myStandardPane)
    myStandardPane->Init (theAttribute);
}

// =======================================================================
// function : GetSelectionModels
// purpose :
// =======================================================================
std::list<QItemSelectionModel*> DFBrowserPaneXDE_AttributeCommonPane::GetSelectionModels()
{
  return myStandardPane ? myStandardPane->GetSelectionModels() : std::list<QItemSelectionModel*>();
}

// =======================================================================
// function : GetValues
// purpose :
// =======================================================================
void DFBrowserPaneXDE_AttributeCommonPane::GetValues (const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  if (myStandardPane)
    myStandardPane->GetValues (theAttribute, theValues);
}

// =======================================================================
// function : GetAttributeInfo
// purpose :
// =======================================================================
QVariant DFBrowserPaneXDE_AttributeCommonPane::GetAttributeInfo (const Handle(TDF_Attribute)& theAttribute,
                                                                 int theRole, int theColumnId)
{
  QVariant aValue;
  if (myStandardPane)
  {
    if (theRole == DFBrowserPane_ItemRole_ShortInfo)
    {
      QList<QVariant> aValues;
      GetShortAttributeInfo (theAttribute, aValues);
      QStringList anInfoList;
      for (QList<QVariant>::const_iterator aValuesIt = aValues.begin(); aValuesIt != aValues.end(); aValuesIt++)
        anInfoList.append (aValuesIt->toString());
      aValue = anInfoList.join (", ");
    }
    else
      aValue = myStandardPane->GetAttributeInfo (theAttribute, theRole, theColumnId);
  }
  else
    aValue = DFBrowserPane_AttributePane::GetAttributeInfoByType (theAttribute->DynamicType()->Name(),
                                                                  theRole, theColumnId);
  return aValue;
}

// =======================================================================
// function : GetShortAttributeInfo
// purpose :
// =======================================================================
void DFBrowserPaneXDE_AttributeCommonPane::GetShortAttributeInfo (
                          const Handle(TDF_Attribute)& theAttribute, QList<QVariant>& theValues)
{
  theValues.append (DFBrowserPaneXDE_XDEDRAW::GetAttributeInfo (theAttribute).ToCString());
}

// =======================================================================
// function : GetSelectionKind
// purpose :
// =======================================================================
int DFBrowserPaneXDE_AttributeCommonPane::GetSelectionKind (QItemSelectionModel* theModel)
{
  if (myStandardPane)
    return myStandardPane->GetSelectionKind (theModel);

  return DFBrowserPane_AttributePane::GetSelectionKind (theModel);
}

// =======================================================================
// function : GetSelectionParameters
// purpose :
// =======================================================================
void DFBrowserPaneXDE_AttributeCommonPane::GetSelectionParameters (QItemSelectionModel* theModel,
                                    NCollection_List<Handle(Standard_Transient)>& theParameters)
{
  if (myStandardPane)
    myStandardPane->GetSelectionParameters (theModel, theParameters);

  DFBrowserPane_AttributePane::GetSelectionParameters (theModel, theParameters);
}

// =======================================================================
// function : GetPresentation
// purpose :
// =======================================================================
Handle(Standard_Transient) DFBrowserPaneXDE_AttributeCommonPane::GetPresentation (
                                           const Handle(TDF_Attribute)& theAttribute)
{
  Handle(Standard_Transient) anIO;
  if (myStandardPane)
    anIO = myStandardPane->GetPresentation (theAttribute);
  return anIO;
}

// =======================================================================
// function : GetReferences
// purpose :
// =======================================================================
void DFBrowserPaneXDE_AttributeCommonPane::GetReferences (
                                       const Handle(TDF_Attribute)& theAttribute,
                                       NCollection_List<TDF_Label>& theRefLabels,
                                       Handle(Standard_Transient)& theRefPresentation)
{
  Handle(Standard_Transient) anIO;
  if (myStandardPane)
    myStandardPane->GetReferences (theAttribute, theRefLabels, theRefPresentation);
}

// =======================================================================
// function : GetAttributeReferences
// purpose :
// =======================================================================
void DFBrowserPaneXDE_AttributeCommonPane::GetAttributeReferences (
                                       const Handle(TDF_Attribute)& theAttribute,
                                       NCollection_List<Handle(TDF_Attribute)>& theRefAttributes,
                                       Handle(Standard_Transient)& theRefPresentation)
{
  Handle(Standard_Transient) anIO;
  if (myStandardPane)
    myStandardPane->GetAttributeReferences (theAttribute, theRefAttributes, theRefPresentation);
}
