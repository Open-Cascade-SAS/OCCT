// Copyright (c) 2022 OPEN CASCADE SAS
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

#include <DE_ConfigurationNode.hxx>

#include <DE_Provider.hxx>
#include <DE_Wrapper.hxx>
#include <DE_ConfigurationContext.hxx>
#include <Message.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DE_ConfigurationNode, Standard_Transient)

//=======================================================================
// function : DE_ConfigurationNode
// purpose  :
//=======================================================================
DE_ConfigurationNode::DE_ConfigurationNode() :
  myIsEnabled(Standard_True)
{}

//=======================================================================
// function : DE_ConfigurationNode
// purpose  :
//=======================================================================
DE_ConfigurationNode::DE_ConfigurationNode(const Handle(DE_ConfigurationNode)& theConfigurationNode)
{
  GlobalParameters = theConfigurationNode->GlobalParameters;
  myIsEnabled = theConfigurationNode->IsEnabled();
}

//=======================================================================
// function : Load
// purpose  :
//=======================================================================
bool DE_ConfigurationNode::Load(const TCollection_AsciiString& theResourcePath)
{
  Handle(DE_ConfigurationContext) aResource = new DE_ConfigurationContext();
  aResource->LoadFile(theResourcePath);
  return Load(aResource);
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
bool DE_ConfigurationNode::Save(const TCollection_AsciiString& theResourcePath) const
{
  OSD_Path aPath = theResourcePath;
  OSD_File aFile(aPath);
  OSD_Protection aProt;
  {
    try
    {
      OCC_CATCH_SIGNALS
        aFile.Build(OSD_ReadWrite, aProt);
    }
    catch (Standard_Failure const&)
    {
      Message::SendFail() << "Error: Configuration writing process was stopped. Can't build file.";
      return false;
    }
  }
  if (aFile.Failed())
  {
    Message::SendFail() << "Error: Configuration writing process was stopped. Can't build file.";
    return false;
  }
  TCollection_AsciiString aResConfiguration = Save();
  aFile.Write(aResConfiguration, aResConfiguration.Length());
  aFile.Close();
  return true;
}

//=======================================================================
// function : UpdateLoad
// purpose  :
//=======================================================================
bool DE_ConfigurationNode::UpdateLoad(const Standard_Boolean theToImport,
                                      const Standard_Boolean theToKeep)
{
  (void)theToImport;
  (void)theToKeep;
  return true;
}

//=======================================================================
// function : IsImportSupported
// purpose  :
//=======================================================================
bool DE_ConfigurationNode::IsImportSupported() const
{
  return false;
}

//=======================================================================
// function : IsExportSupported
// purpose  :
//=======================================================================
bool DE_ConfigurationNode::IsExportSupported() const
{
  return false;
}

//=======================================================================
// function : CheckForSupport
// purpose  :
//=======================================================================
bool DE_ConfigurationNode::CheckExtension(const TCollection_AsciiString& theExtension) const
{
  TCollection_AsciiString anExtension(theExtension);
  if (anExtension.IsEmpty())
  {
    return false;
  }
  if (anExtension.Value(1) == '.')
  {
    anExtension.Remove(1);
  }
  const TColStd_ListOfAsciiString& anExtensions = GetExtensions();
  for (TColStd_ListOfAsciiString::Iterator anIter(anExtensions);
       anIter.More(); anIter.Next())
  {
    if (TCollection_AsciiString::IsSameString(anIter.Value(), anExtension, Standard_False))
    {
      return true;
    }
  }
  return false;
}

//=======================================================================
// function : CheckForSupport
// purpose  :
//=======================================================================
bool DE_ConfigurationNode::CheckContent(const Handle(NCollection_Buffer)& theBuffer) const
{
  (void)theBuffer;
  return false;
}
