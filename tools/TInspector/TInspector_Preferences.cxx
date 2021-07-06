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

#include <inspector/TInspector_Preferences.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <QTextStream>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : GetPreferences
// purpose :
// =======================================================================
void TInspector_Preferences::GetPreferences (const TCollection_AsciiString& thePluginName,
                                             TInspectorAPI_PreferencesDataMap& theItem)
{
  if (!myIsLoadedPreferences)
  {
    loadPreferences();
    myIsLoadedPreferences = Standard_True;
  }
  myLoadedPreferences.Find (thePluginName, theItem);
}

// =======================================================================
// function : StorePreferences
// purpose :
// =======================================================================
void TInspector_Preferences::StorePreferences()
{
  if (myLoadedPreferences.IsEmpty())
    return;

  QString aFileName = QString (GetDirectory().ToCString()) + QDir::separator() + PreferencesFileName();
  QFile aFile(aFileName);
  if (!aFile.open (QFile::WriteOnly))
    return;

  QDomDocument aDomDocument (documentKey());
  QDomComment aComment = aDomDocument.createComment("\nThis file is automatically created by TInspector application.\nChanges made in this file can be lost.\n");
  aDomDocument.appendChild (aComment);
  QDomElement aRootElement = aDomDocument.createElement (documentKey());
  aDomDocument.appendChild (aRootElement);

  for (NCollection_DataMap<TCollection_AsciiString, TInspectorAPI_PreferencesDataMap>::Iterator aPrefsIt (myLoadedPreferences);
    aPrefsIt.More(); aPrefsIt.Next())
  {
    QDomElement aPluginElement = aDomDocument.createElement (pluginKey());
    aPluginElement.setAttribute (nameKey(), aPrefsIt.Key().ToCString());
    aRootElement.appendChild (aPluginElement);

    const TInspectorAPI_PreferencesDataMap& aPluginMap = aPrefsIt.Value();
    for (TInspectorAPI_IteratorOfPreferencesDataMap aPluginPrefsIt (aPluginMap); aPluginPrefsIt.More(); aPluginPrefsIt.Next())
    {
      QDomElement aParameterElement = aDomDocument.createElement (parameterKey());
      aParameterElement.setAttribute (nameKey(), aPluginPrefsIt.Key().ToCString());
      aParameterElement.setAttribute (valueKey(), aPluginPrefsIt.Value().ToCString());
      aPluginElement.appendChild (aParameterElement);
    }
  }

  QTextStream aTextStream (&aFile);
  QStringList aDocumentStr = aDomDocument.toString().split ("\n");
  for (QStringList::ConstIterator aContentIt = aDocumentStr.begin(); aContentIt != aDocumentStr.end(); ++aContentIt)
  {
    aTextStream << *aContentIt << '\n';
  }
  aFile.close();
}

// =======================================================================
// function : RemovePreferences
// purpose :
// =======================================================================
void TInspector_Preferences::RemovePreferences()
{
  QString aFileName = QString (GetDirectory().ToCString()) + QDir::separator() + PreferencesFileName();
  QDir aDir (GetDirectory().ToCString());
  if (aDir.exists (aFileName))
    aDir.remove (aFileName);
  reset();
}

// =======================================================================
// function : loadPreferences
// purpose :
// =======================================================================
void TInspector_Preferences::loadPreferences()
{
  QString aFileName = QString (GetDirectory().ToCString()) + QDir::separator() + PreferencesFileName();
  QFile aFile (aFileName);
  if (!aFile.open (QFile::ReadOnly))
    return;

  QDomDocument aDomDocument;
  bool aResult = aDomDocument.setContent (&aFile);
  aFile.close();
  if (!aResult)
    return;

  QDomElement aRootElement = aDomDocument.documentElement();
  if (aRootElement.isNull() || aRootElement.tagName() != documentKey())
    return;

  QDomNode aPluginNode = aRootElement.firstChild();
  while (!aPluginNode.isNull())
  {
    if (aPluginNode.isElement())
    {
      QDomElement aPluginElement = aPluginNode.toElement();
      if (aPluginElement.tagName() == pluginKey() && aPluginElement.hasAttribute (nameKey()))
      {
        TInspectorAPI_PreferencesDataMap anItem;
        readPluginItem (aPluginElement, anItem);
        myLoadedPreferences.Bind (aPluginElement.attribute (nameKey()).toStdString().c_str(), anItem);
      }
    }
    aPluginNode = aPluginNode.nextSibling();
  }
}

// =======================================================================
// function : readPluginItem
// purpose :
// =======================================================================
void TInspector_Preferences::readPluginItem (const QDomElement thePluginElement, TInspectorAPI_PreferencesDataMap& theItem)
{
  QDomNode aParameterNode = thePluginElement.firstChild();
  while (!aParameterNode.isNull())
  {
    if (aParameterNode.isElement())
    {
      QDomElement aParameterElement = aParameterNode.toElement();
      if (aParameterElement.tagName() == parameterKey() &&
          aParameterElement.hasAttribute (nameKey()) && aParameterElement.hasAttribute (valueKey()))
      {
        theItem.Bind (aParameterElement.attribute (nameKey()).toStdString().c_str(),
                      aParameterElement.attribute (valueKey()).toStdString().c_str());
      }
    }
    aParameterNode = aParameterNode.nextSibling();
  }
}
