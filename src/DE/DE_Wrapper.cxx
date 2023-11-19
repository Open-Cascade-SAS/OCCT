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

#include <DE_Wrapper.hxx>

#include <DE_ConfigurationContext.hxx>
#include <DE_ConfigurationNode.hxx>
#include <DE_Provider.hxx>
#include <Message_ProgressRange.hxx>
#include <NCollection_Buffer.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <OSD_FileSystem.hxx>
#include <OSD_Protection.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DE_Wrapper, Standard_Transient)

namespace
{
  static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
  {
    static const TCollection_AsciiString aScope ("global");
    return aScope;
  }

  static Handle(DE_Wrapper)& THE_GLOBAL_CONFIGURATION()
  {
    static Handle(DE_Wrapper) aConf = new DE_Wrapper();
    return aConf;
  }
}

//=======================================================================
// function : DE_Wrapper
// purpose  :
//=======================================================================
DE_Wrapper::DE_Wrapper()
  :myKeepUpdates(Standard_False)
{}

//=======================================================================
// function : DE_Wrapper
// purpose  :
//=======================================================================
DE_Wrapper::DE_Wrapper(const Handle(DE_Wrapper)& theWrapper)
  : DE_Wrapper()
{
  if (theWrapper.IsNull())
  {
    return;
  }
  GlobalParameters = theWrapper->GlobalParameters;
  for (DE_ConfigurationFormatMap::Iterator aFormatIter(theWrapper->Nodes());
       aFormatIter.More(); aFormatIter.Next())
  {
    for (DE_ConfigurationVendorMap::Iterator aVendorIter(aFormatIter.Value());
         aVendorIter.More(); aVendorIter.Next())
    {
      Bind(aVendorIter.Value());
    }
  }
  theWrapper->myKeepUpdates = myKeepUpdates;
}

//=======================================================================
// function : GlobalWrapper
// purpose  :
//=======================================================================
const Handle(DE_Wrapper)& DE_Wrapper::GlobalWrapper()
{
  return THE_GLOBAL_CONFIGURATION();
}

//=======================================================================
// function : SetGlobalWrapper
// purpose  :
//=======================================================================
void DE_Wrapper::SetGlobalWrapper(const Handle(DE_Wrapper)& theWrapper)
{
  if (!theWrapper.IsNull())
  {
    THE_GLOBAL_CONFIGURATION() = theWrapper;
  }
}

//=======================================================================
// function : GlobalLoadMutex
// purpose  :
//=======================================================================
Standard_Mutex& DE_Wrapper::GlobalLoadMutex()
{
  static Standard_Mutex THE_GLOBAL_LOAD_MUTEX;
  return THE_GLOBAL_LOAD_MUTEX;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::Read(const TCollection_AsciiString& thePath,
                                  const Handle(TDocStd_Document)& theDocument,
                                  Handle(XSControl_WorkSession)& theWS,
                                  const Message_ProgressRange& theProgress)
{
  if (theDocument.IsNull())
  {
    return Standard_False;
  }
  if (theWS.IsNull())
  {
    return Read(thePath, theDocument, theProgress);
  }
  Handle(DE_Provider) aProvider;
  if (!FindProvider(thePath, Standard_True, aProvider))
  {
    return Standard_False;
  }
  return aProvider->Read(thePath, theDocument, theWS, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::Write(const TCollection_AsciiString& thePath,
                                   const Handle(TDocStd_Document)& theDocument,
                                   Handle(XSControl_WorkSession)& theWS,
                                   const Message_ProgressRange& theProgress)
{
  if (theDocument.IsNull())
  {
    return Standard_False;
  }
  if (theWS.IsNull())
  {
    return Write(thePath, theDocument, theProgress);
  }
  Handle(DE_Provider) aProvider;
  if (!FindProvider(thePath, Standard_False, aProvider))
  {
    return Standard_False;
  }
  return aProvider->Write(thePath, theDocument, theWS, theProgress);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::Read(const TCollection_AsciiString& thePath,
                                  const Handle(TDocStd_Document)& theDocument,
                                  const Message_ProgressRange& theProgress)
{
  if (theDocument.IsNull())
  {
    return Standard_False;
  }
  Handle(DE_Provider) aProvider;
  if (!FindProvider(thePath, Standard_True, aProvider))
  {
    return Standard_False;
  }
  return aProvider->Read(thePath, theDocument, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::Write(const TCollection_AsciiString& thePath,
                                   const Handle(TDocStd_Document)& theDocument,
                                   const Message_ProgressRange& theProgress)
{
  if (theDocument.IsNull())
  {
    return Standard_False;
  }
  Handle(DE_Provider) aProvider;
  if (!FindProvider(thePath, Standard_False, aProvider))
  {
    return Standard_False;
  }
  return aProvider->Write(thePath, theDocument, theProgress);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::Read(const TCollection_AsciiString& thePath,
                                  TopoDS_Shape& theShape,
                                  Handle(XSControl_WorkSession)& theWS,
                                  const Message_ProgressRange& theProgress)
{
  if (theWS.IsNull())
  {
    return Read(thePath, theShape, theProgress);
  }
  Handle(DE_Provider) aProvider;
  if (!FindProvider(thePath, Standard_True, aProvider))
  {
    return Standard_False;
  }
  return aProvider->Read(thePath, theShape, theWS, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::Write(const TCollection_AsciiString& thePath,
                                   const TopoDS_Shape& theShape,
                                   Handle(XSControl_WorkSession)& theWS,
                                   const Message_ProgressRange& theProgress)
{
  if (theWS.IsNull())
  {
    return Write(thePath, theShape, theProgress);
  }
  Handle(DE_Provider) aProvider;
  if (!FindProvider(thePath, Standard_False, aProvider))
  {
    return Standard_False;
  }
  return aProvider->Write(thePath, theShape, theWS, theProgress);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::Read(const TCollection_AsciiString& thePath,
                                  TopoDS_Shape& theShape,
                                  const Message_ProgressRange& theProgress)
{

  Handle(DE_Provider) aProvider;
  if (!FindProvider(thePath, Standard_True, aProvider))
  {
    return Standard_False;
  }
  return aProvider->Read(thePath, theShape, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::Write(const TCollection_AsciiString& thePath,
                                   const TopoDS_Shape& theShape,
                                   const Message_ProgressRange& theProgress)
{
  Handle(DE_Provider) aProvider;
  if (!FindProvider(thePath, Standard_False, aProvider))
  {
    return Standard_False;
  }
  return aProvider->Write(thePath, theShape, theProgress);
}

//=======================================================================
// function : Load
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::Load(const TCollection_AsciiString& theResource,
                                  const Standard_Boolean theIsRecursive)
{
  Handle(DE_ConfigurationContext) aResource = new DE_ConfigurationContext();
  aResource->Load(theResource);
  return Load(aResource, theIsRecursive);
}

//=======================================================================
// function : Load
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::Load(const Handle(DE_ConfigurationContext)& theResource,
                                  const Standard_Boolean theIsRecursive)
{
  GlobalParameters.LengthUnit = theResource->RealVal("general.length.unit", GlobalParameters.LengthUnit, THE_CONFIGURATION_SCOPE());
  if (theIsRecursive)
  {
    for (DE_ConfigurationFormatMap::Iterator aFormatIter(myConfiguration);
         aFormatIter.More(); aFormatIter.Next())
    {
      for (DE_ConfigurationVendorMap::Iterator aVendorIter(aFormatIter.Value());
           aVendorIter.More(); aVendorIter.Next())
      {
        aVendorIter.Value()->Load(theResource);
      }
    }
    sort(theResource);
  }
  return Standard_True;
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::Save(const TCollection_AsciiString& theResourcePath,
                                  const Standard_Boolean theIsRecursive,
                                  const TColStd_ListOfAsciiString& theFormats,
                                  const TColStd_ListOfAsciiString& theVendors)
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
      return Standard_False;
    }
  }
  if (aFile.Failed())
  {
    return Standard_False;
  }
  TCollection_AsciiString aResConfiguration = Save(theIsRecursive, theFormats, theVendors);
  aFile.Write(aResConfiguration, aResConfiguration.Length());
  aFile.Close();
  return Standard_True;
}

//=======================================================================
// function : Save
// purpose  :
//=======================================================================
TCollection_AsciiString DE_Wrapper::Save(const Standard_Boolean theIsRecursive,
                                         const TColStd_ListOfAsciiString& theFormats,
                                         const TColStd_ListOfAsciiString& theVendors)
{
  TCollection_AsciiString aResult;
  aResult += "!Description of the config file for DE toolkit\n";
  aResult += "!*****************************************************************************\n";
  aResult += "!\n";
  aResult += "!Format of the file is compliant with the standard Open CASCADE resource files\n";
  aResult += "!Each key defines a sequence of either further keys.\n";
  aResult += "!Keys can be nested down to an arbitrary level.\n";
  aResult += "!\n";
  aResult += "!*****************************************************************************\n";
  aResult += "!DE_Wrapper\n";
  aResult += "!Priority vendor list. For every CAD format set indexed list of vendors\n";
  for (DE_ConfigurationFormatMap::Iterator aFormatIter(myConfiguration);
       aFormatIter.More(); aFormatIter.Next())
  {
    const TCollection_AsciiString& aFormat = aFormatIter.Key();
    aResult += THE_CONFIGURATION_SCOPE() + '.' + "priority" + '.' + aFormat + " :\t ";
    for (DE_ConfigurationVendorMap::Iterator aVendorIter(aFormatIter.Value());
         aVendorIter.More(); aVendorIter.Next())
    {
      const TCollection_AsciiString& aVendorName = aVendorIter.Value()->GetVendor();
      aResult += aVendorName + " ";
    }
    aResult += "\n";
  }
  aResult += "!Global parameters. Used for all providers\n";
  aResult += "!Length scale unit value. Should be more the 0. Default value: 1.0(MM)\n";
  aResult += THE_CONFIGURATION_SCOPE() + ".general.length.unit :\t " + GlobalParameters.LengthUnit + "\n";
  if (theIsRecursive)
  {
    for (DE_ConfigurationFormatMap::Iterator aFormatIter(myConfiguration);
         aFormatIter.More(); aFormatIter.Next())
    {
      if (!theFormats.IsEmpty() && !theFormats.Contains(aFormatIter.Key()))
      {
        continue;
      }
      for (DE_ConfigurationVendorMap::Iterator aVendorIter(aFormatIter.Value());
           aVendorIter.More(); aVendorIter.Next())
      {
        if (!theVendors.IsEmpty() && !theVendors.Contains(aVendorIter.Key()))
        {
          continue;
        }
        aResult += "!\n";
        aResult += aVendorIter.Value()->Save();
        aResult += "!\n";
      }
    }
  }
  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=======================================================================
// function : Bind
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::Bind(const Handle(DE_ConfigurationNode)& theNode)
{
  if (theNode.IsNull())
  {
    return Standard_False;
  }
  const TCollection_AsciiString aFileFormat = theNode->GetFormat();
  const TCollection_AsciiString aVendorName = theNode->GetVendor();
  DE_ConfigurationVendorMap* aVendorMap = myConfiguration.ChangeSeek(aFileFormat);
  if (aVendorMap == NULL)
  {
    DE_ConfigurationVendorMap aTmpVendorMap;
    aVendorMap = myConfiguration.Bound(aFileFormat, aTmpVendorMap);
  }
  return aVendorMap->Add(aVendorName, theNode) > 0;
}

//=======================================================================
// function : UnBind
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::UnBind(const Handle(DE_ConfigurationNode)& theNode)
{
  if (theNode.IsNull())
  {
    return false;
  }
  const TCollection_AsciiString aFileFormat = theNode->GetFormat();
  const TCollection_AsciiString aVendorName = theNode->GetVendor();
  DE_ConfigurationVendorMap* aVendorMap = myConfiguration.ChangeSeek(aFileFormat);
  if (aVendorMap == NULL)
  {
    return false;
  }
  const auto aPrevSize = aVendorMap->Size();
  aVendorMap->RemoveKey(aVendorName);
  return aVendorMap->Size() != aPrevSize;
}

//=======================================================================
// function : Find
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::Find(const TCollection_AsciiString& theFormat,
                                  const TCollection_AsciiString& theVendor,
                                  Handle(DE_ConfigurationNode)& theNode) const
{
  const DE_ConfigurationVendorMap* aVendorMap = myConfiguration.Seek(theFormat);
  return aVendorMap != nullptr && aVendorMap->FindFromKey(theVendor, theNode);
}

//=======================================================================
// function : ChangePriority
// purpose  :
//=======================================================================
void DE_Wrapper::ChangePriority(const TCollection_AsciiString& theFormat,
                                const TColStd_ListOfAsciiString& theVendorPriority,
                                const Standard_Boolean theToDisable)
{
  DE_ConfigurationVendorMap aVendorMap;
  if (!myConfiguration.Find(theFormat, aVendorMap))
  {
    return;
  }
  DE_ConfigurationVendorMap aNewVendorMap;
  // Sets according to the input priority
  for (TColStd_ListOfAsciiString::Iterator aPriorIter(theVendorPriority);
       aPriorIter.More(); aPriorIter.Next())
  {
    const TCollection_AsciiString& aVendorName = aPriorIter.Value();
    Handle(DE_ConfigurationNode) aNode;
    if (aVendorMap.FindFromKey(aVendorName, aNode))
    {
      aNode->SetEnabled(Standard_True);
      aNewVendorMap.Add(aVendorName, aNode);
    }
  }
  // Sets not used elements
  for (DE_ConfigurationVendorMap::Iterator aVendorIter(aVendorMap);
       aVendorIter.More(); aVendorIter.Next())
  {
    const TCollection_AsciiString& aVendorName = aVendorIter.Key();
    if (!theVendorPriority.Contains(aVendorName))
    {
      const Handle(DE_ConfigurationNode)& aNode = aVendorIter.Value();
      if (theToDisable)
      {
        aNode->SetEnabled(Standard_False);
      }
      aNewVendorMap.Add(aVendorName, aNode);
    }
  }
  myConfiguration.Bind(theFormat, aNewVendorMap);
}

//=======================================================================
// function : ChangePriority
// purpose  :
//=======================================================================
void DE_Wrapper::ChangePriority(const TColStd_ListOfAsciiString& theVendorPriority,
                                const Standard_Boolean theToDisable)
{
  for (DE_ConfigurationFormatMap::Iterator aFormatIter(myConfiguration);
       aFormatIter.More(); aFormatIter.Next())
  {
    ChangePriority(aFormatIter.Key(), theVendorPriority, theToDisable);
  }
}

//=======================================================================
// function : Nodes
// purpose  :
//=======================================================================
const DE_ConfigurationFormatMap& DE_Wrapper::Nodes() const
{
  return myConfiguration;
}

//=======================================================================
// function : Copy
// purpose  :
//=======================================================================
Handle(DE_Wrapper) DE_Wrapper::Copy() const
{
  return new DE_Wrapper(*this);
}

//=======================================================================
// function : FindProvider
// purpose  :
//=======================================================================
Standard_Boolean DE_Wrapper::FindProvider(const TCollection_AsciiString& thePath,
                                          const Standard_Boolean theToImport,
                                          Handle(DE_Provider)& theProvider) const
{
  Handle(NCollection_Buffer) aBuffer;
  if (theToImport)
  {
    const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
    std::shared_ptr<std::istream> aStream = aFileSystem->OpenIStream(thePath, std::ios::in | std::ios::binary);
    if (aStream.get() != nullptr)
    {
      aBuffer = new NCollection_Buffer(NCollection_BaseAllocator::CommonBaseAllocator(), 2048);
      aStream->read((char*)aBuffer->ChangeData(), 2048);
      aBuffer->ChangeData()[2047] = '\0';
    }
  }
  OSD_Path aPath(thePath);
  const TCollection_AsciiString anExtr = aPath.Extension();
  for (DE_ConfigurationFormatMap::Iterator aFormatIter(myConfiguration);
       aFormatIter.More(); aFormatIter.Next())
  {
    for (DE_ConfigurationVendorMap::Iterator aVendorIter(aFormatIter.Value());
         aVendorIter.More(); aVendorIter.Next())
    {
      const Handle(DE_ConfigurationNode)& aNode = aVendorIter.Value();
      if (aNode->IsEnabled() &&
          ((theToImport && aNode->IsImportSupported()) ||
          (!theToImport && aNode->IsExportSupported())) &&
          (aNode->CheckExtension(anExtr) ||
          (theToImport && aNode->CheckContent(aBuffer))) &&
          aNode->UpdateLoad(theToImport, myKeepUpdates))
      {
        theProvider = aNode->BuildProvider();
        aNode->GlobalParameters = GlobalParameters;
        return Standard_True;
      }
    }
  }
  return Standard_False;
}

//=======================================================================
// function : UpdateLoad
// purpose  :
//=======================================================================
Standard_EXPORT void DE_Wrapper::UpdateLoad(const Standard_Boolean theToForceUpdate) const
{
  for (DE_ConfigurationFormatMap::Iterator aFormatIter(myConfiguration);
       aFormatIter.More(); aFormatIter.Next())
  {
    for (DE_ConfigurationVendorMap::Iterator aVendorIter(aFormatIter.Value());
         aVendorIter.More(); aVendorIter.Next())
    {
      const Handle(DE_ConfigurationNode)& aNode = aVendorIter.Value();
      aNode->UpdateLoad(Standard_True, Standard_True);
      aNode->UpdateLoad(Standard_False, Standard_True);
      if (!theToForceUpdate)
        continue;
      aNode->SetEnabled(aNode->IsExportSupported() || aNode->IsImportSupported());
    }
  }
}

//=======================================================================
// function : sort
// purpose  :
//=======================================================================
void DE_Wrapper::sort(const Handle(DE_ConfigurationContext)& theResource)
{
  const TCollection_AsciiString aScope(THE_CONFIGURATION_SCOPE() + '.' + "priority");
  for (DE_ConfigurationFormatMap::Iterator aFormatIter(myConfiguration);
       aFormatIter.More(); aFormatIter.Next())
  {
    TColStd_ListOfAsciiString aVendorPriority;
    if (!theResource->GetStringSeq(aFormatIter.Key(), aVendorPriority, aScope))
    {
      continue;
    }
    ChangePriority(aFormatIter.Key(), aVendorPriority, Standard_True);
  }
}
