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
#include <DE_ValidationUtils.hxx>
#include <Message_ProgressRange.hxx>
#include <NCollection_Buffer.hxx>
#include <OSD_File.hxx>
#include <OSD_FileSystem.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DE_Wrapper, Standard_Transient)

namespace
{
static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
{
  static const TCollection_AsciiString aScope("global");
  return aScope;
}

static occ::handle<DE_Wrapper>& THE_GLOBAL_CONFIGURATION()
{
  static occ::handle<DE_Wrapper> aConf = new DE_Wrapper();
  return aConf;
}
} // namespace

//=================================================================================================

DE_Wrapper::DE_Wrapper()
    : myKeepUpdates(false)
{
}

//=================================================================================================

DE_Wrapper::DE_Wrapper(const occ::handle<DE_Wrapper>& theWrapper)
    : DE_Wrapper()
{
  if (theWrapper.IsNull())
  {
    return;
  }
  GlobalParameters = theWrapper->GlobalParameters;
  for (NCollection_DataMap<TCollection_AsciiString, NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>>::Iterator aFormatIter(theWrapper->Nodes()); aFormatIter.More();
       aFormatIter.Next())
  {
    for (NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>::Iterator aVendorIter(aFormatIter.Value()); aVendorIter.More();
         aVendorIter.Next())
    {
      Bind(aVendorIter.Value());
    }
  }
  theWrapper->myKeepUpdates = myKeepUpdates;
}

//=================================================================================================

const occ::handle<DE_Wrapper>& DE_Wrapper::GlobalWrapper()
{
  return THE_GLOBAL_CONFIGURATION();
}

//=================================================================================================

void DE_Wrapper::SetGlobalWrapper(const occ::handle<DE_Wrapper>& theWrapper)
{
  if (!theWrapper.IsNull())
  {
    THE_GLOBAL_CONFIGURATION() = theWrapper;
  }
}

//=================================================================================================

std::mutex& DE_Wrapper::GlobalLoadMutex()
{
  static std::mutex THE_GLOBAL_LOAD_MUTEX;
  return THE_GLOBAL_LOAD_MUTEX;
}

//=================================================================================================

bool DE_Wrapper::Read(const TCollection_AsciiString&  thePath,
                                  const occ::handle<TDocStd_Document>& theDocument,
                                  occ::handle<XSControl_WorkSession>&  theWS,
                                  const Message_ProgressRange&    theProgress)
{
  if (theDocument.IsNull())
  {
    return false;
  }
  if (theWS.IsNull())
  {
    return Read(thePath, theDocument, theProgress);
  }
  occ::handle<DE_Provider> aProvider;
  if (!FindProvider(thePath, true, aProvider))
  {
    return false;
  }
  return aProvider->Read(thePath, theDocument, theWS, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Write(const TCollection_AsciiString&  thePath,
                                   const occ::handle<TDocStd_Document>& theDocument,
                                   occ::handle<XSControl_WorkSession>&  theWS,
                                   const Message_ProgressRange&    theProgress)
{
  if (theDocument.IsNull())
  {
    return false;
  }
  if (theWS.IsNull())
  {
    return Write(thePath, theDocument, theProgress);
  }
  occ::handle<DE_Provider> aProvider;
  if (!FindProvider(thePath, false, aProvider))
  {
    return false;
  }
  return aProvider->Write(thePath, theDocument, theWS, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Read(const TCollection_AsciiString&  thePath,
                                  const occ::handle<TDocStd_Document>& theDocument,
                                  const Message_ProgressRange&    theProgress)
{
  if (theDocument.IsNull())
  {
    return false;
  }
  occ::handle<DE_Provider> aProvider;
  if (!FindProvider(thePath, true, aProvider))
  {
    return false;
  }
  return aProvider->Read(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Write(const TCollection_AsciiString&  thePath,
                                   const occ::handle<TDocStd_Document>& theDocument,
                                   const Message_ProgressRange&    theProgress)
{
  if (theDocument.IsNull())
  {
    return false;
  }
  occ::handle<DE_Provider> aProvider;
  if (!FindProvider(thePath, false, aProvider))
  {
    return false;
  }
  return aProvider->Write(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Read(const TCollection_AsciiString& thePath,
                                  TopoDS_Shape&                  theShape,
                                  occ::handle<XSControl_WorkSession>& theWS,
                                  const Message_ProgressRange&   theProgress)
{
  if (theWS.IsNull())
  {
    return Read(thePath, theShape, theProgress);
  }
  occ::handle<DE_Provider> aProvider;
  if (!FindProvider(thePath, true, aProvider))
  {
    return false;
  }
  return aProvider->Read(thePath, theShape, theWS, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Write(const TCollection_AsciiString& thePath,
                                   const TopoDS_Shape&            theShape,
                                   occ::handle<XSControl_WorkSession>& theWS,
                                   const Message_ProgressRange&   theProgress)
{
  if (theWS.IsNull())
  {
    return Write(thePath, theShape, theProgress);
  }
  occ::handle<DE_Provider> aProvider;
  if (!FindProvider(thePath, false, aProvider))
  {
    return false;
  }
  return aProvider->Write(thePath, theShape, theWS, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Read(const TCollection_AsciiString& thePath,
                                  TopoDS_Shape&                  theShape,
                                  const Message_ProgressRange&   theProgress)
{

  occ::handle<DE_Provider> aProvider;
  if (!FindProvider(thePath, true, aProvider))
  {
    return false;
  }
  return aProvider->Read(thePath, theShape, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Write(const TCollection_AsciiString& thePath,
                                   const TopoDS_Shape&            theShape,
                                   const Message_ProgressRange&   theProgress)
{
  occ::handle<DE_Provider> aProvider;
  if (!FindProvider(thePath, false, aProvider))
  {
    return false;
  }
  return aProvider->Write(thePath, theShape, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Load(const TCollection_AsciiString& theResource,
                                  const bool         theIsRecursive)
{
  occ::handle<DE_ConfigurationContext> aResource = new DE_ConfigurationContext();
  aResource->Load(theResource);
  return Load(aResource, theIsRecursive);
}

//=================================================================================================

bool DE_Wrapper::Load(const occ::handle<DE_ConfigurationContext>& theResource,
                                  const bool                 theIsRecursive)
{
  GlobalParameters.LengthUnit = theResource->RealVal("general.length.unit",
                                                     GlobalParameters.LengthUnit,
                                                     THE_CONFIGURATION_SCOPE());
  GlobalParameters.SystemUnit = theResource->RealVal("general.system.unit",
                                                     GlobalParameters.SystemUnit,
                                                     THE_CONFIGURATION_SCOPE());
  if (theIsRecursive)
  {
    for (NCollection_DataMap<TCollection_AsciiString, NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>>::Iterator aFormatIter(myConfiguration); aFormatIter.More();
         aFormatIter.Next())
    {
      for (NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>::Iterator aVendorIter(aFormatIter.Value()); aVendorIter.More();
           aVendorIter.Next())
      {
        aVendorIter.Value()->Load(theResource);
      }
    }
    sort(theResource);
  }
  return true;
}

//=================================================================================================

bool DE_Wrapper::Save(const TCollection_AsciiString&   theResourcePath,
                                  const bool           theIsRecursive,
                                  const NCollection_List<TCollection_AsciiString>& theFormats,
                                  const NCollection_List<TCollection_AsciiString>& theVendors)
{
  OSD_Path       aPath = theResourcePath;
  OSD_File       aFile(aPath);
  OSD_Protection aProt;
  {
    try
    {
      OCC_CATCH_SIGNALS
      aFile.Build(OSD_ReadWrite, aProt);
    }
    catch (Standard_Failure const&)
    {
      return false;
    }
  }
  if (aFile.Failed())
  {
    return false;
  }
  TCollection_AsciiString aResConfiguration = Save(theIsRecursive, theFormats, theVendors);
  aFile.Write(aResConfiguration, aResConfiguration.Length());
  aFile.Close();
  return true;
}

//=================================================================================================

TCollection_AsciiString DE_Wrapper::Save(const bool           theIsRecursive,
                                         const NCollection_List<TCollection_AsciiString>& theFormats,
                                         const NCollection_List<TCollection_AsciiString>& theVendors)
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
  for (NCollection_DataMap<TCollection_AsciiString, NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>>::Iterator aFormatIter(myConfiguration); aFormatIter.More();
       aFormatIter.Next())
  {
    const TCollection_AsciiString& aFormat = aFormatIter.Key();
    aResult += THE_CONFIGURATION_SCOPE() + '.' + "priority" + '.' + aFormat + " :\t ";
    for (NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>::Iterator aVendorIter(aFormatIter.Value()); aVendorIter.More();
         aVendorIter.Next())
    {
      const TCollection_AsciiString& aVendorName = aVendorIter.Value()->GetVendor();
      aResult += aVendorName + " ";
    }
    aResult += "\n";
  }
  aResult += "!Global parameters. Used for all providers\n";
  aResult += "!Length scale unit value. Should be more than 0. Default value: 1.0(MM)\n";
  aResult +=
    THE_CONFIGURATION_SCOPE() + ".general.length.unit :\t " + GlobalParameters.LengthUnit + "\n";
  aResult += "!System unit value. Should be more than 0. Default value: 1.0(MM)\n";
  aResult +=
    THE_CONFIGURATION_SCOPE() + ".general.system.unit :\t " + GlobalParameters.SystemUnit + "\n";
  if (theIsRecursive)
  {
    for (NCollection_DataMap<TCollection_AsciiString, NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>>::Iterator aFormatIter(myConfiguration); aFormatIter.More();
         aFormatIter.Next())
    {
      if (!theFormats.IsEmpty() && !theFormats.Contains(aFormatIter.Key()))
      {
        continue;
      }
      for (NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>::Iterator aVendorIter(aFormatIter.Value()); aVendorIter.More();
           aVendorIter.Next())
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

//=================================================================================================

bool DE_Wrapper::Bind(const occ::handle<DE_ConfigurationNode>& theNode)
{
  if (theNode.IsNull())
  {
    return false;
  }
  const TCollection_AsciiString aFileFormat = theNode->GetFormat();
  const TCollection_AsciiString aVendorName = theNode->GetVendor();
  NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>*    aVendorMap  = myConfiguration.ChangeSeek(aFileFormat);
  if (aVendorMap == NULL)
  {
    NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>> aTmpVendorMap;
    aVendorMap = myConfiguration.Bound(aFileFormat, aTmpVendorMap);
  }
  return aVendorMap->Add(aVendorName, theNode) > 0;
}

//=================================================================================================

bool DE_Wrapper::UnBind(const occ::handle<DE_ConfigurationNode>& theNode)
{
  if (theNode.IsNull())
  {
    return false;
  }
  const TCollection_AsciiString aFileFormat = theNode->GetFormat();
  const TCollection_AsciiString aVendorName = theNode->GetVendor();
  NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>*    aVendorMap  = myConfiguration.ChangeSeek(aFileFormat);
  if (aVendorMap == NULL)
  {
    return false;
  }
  const auto aPrevSize = aVendorMap->Size();
  aVendorMap->RemoveKey(aVendorName);
  return aVendorMap->Size() != aPrevSize;
}

//=================================================================================================

bool DE_Wrapper::Find(const TCollection_AsciiString& theFormat,
                                  const TCollection_AsciiString& theVendor,
                                  occ::handle<DE_ConfigurationNode>&  theNode) const
{
  const NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>* aVendorMap = myConfiguration.Seek(theFormat);
  return aVendorMap != nullptr && aVendorMap->FindFromKey(theVendor, theNode);
}

//=================================================================================================

void DE_Wrapper::ChangePriority(const TCollection_AsciiString&   theFormat,
                                const NCollection_List<TCollection_AsciiString>& theVendorPriority,
                                const bool           theToDisable)
{
  NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>> aVendorMap;
  if (!myConfiguration.Find(theFormat, aVendorMap))
  {
    return;
  }
  NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>> aNewVendorMap;
  // Sets according to the input priority
  for (NCollection_List<TCollection_AsciiString>::Iterator aPriorIter(theVendorPriority); aPriorIter.More();
       aPriorIter.Next())
  {
    const TCollection_AsciiString& aVendorName = aPriorIter.Value();
    occ::handle<DE_ConfigurationNode>   aNode;
    if (aVendorMap.FindFromKey(aVendorName, aNode))
    {
      aNode->SetEnabled(true);
      aNewVendorMap.Add(aVendorName, aNode);
    }
  }
  // Sets not used elements
  for (NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>::Iterator aVendorIter(aVendorMap); aVendorIter.More();
       aVendorIter.Next())
  {
    const TCollection_AsciiString& aVendorName = aVendorIter.Key();
    if (!theVendorPriority.Contains(aVendorName))
    {
      const occ::handle<DE_ConfigurationNode>& aNode = aVendorIter.Value();
      if (theToDisable)
      {
        aNode->SetEnabled(false);
      }
      aNewVendorMap.Add(aVendorName, aNode);
    }
  }
  myConfiguration.Bind(theFormat, aNewVendorMap);
}

//=================================================================================================

void DE_Wrapper::ChangePriority(const NCollection_List<TCollection_AsciiString>& theVendorPriority,
                                const bool           theToDisable)
{
  for (NCollection_DataMap<TCollection_AsciiString, NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>>::Iterator aFormatIter(myConfiguration); aFormatIter.More();
       aFormatIter.Next())
  {
    ChangePriority(aFormatIter.Key(), theVendorPriority, theToDisable);
  }
}

//=================================================================================================

const NCollection_DataMap<TCollection_AsciiString, NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>>& DE_Wrapper::Nodes() const
{
  return myConfiguration;
}

//=================================================================================================

occ::handle<DE_Wrapper> DE_Wrapper::Copy() const
{
  return new DE_Wrapper(*this);
}

//=================================================================================================

bool DE_Wrapper::FindProvider(const TCollection_AsciiString& thePath,
                                          const bool         theToImport,
                                          occ::handle<DE_Provider>&           theProvider) const
{
  if (theToImport)
  {
    return FindReadProvider(thePath, true, theProvider);
  }
  else
  {
    return FindWriteProvider(thePath, theProvider);
  }
}

//=================================================================================================

bool DE_Wrapper::FindReadProvider(const TCollection_AsciiString& thePath,
                                              const bool         theCheckContent,
                                              occ::handle<DE_Provider>&           theProvider) const
{
  occ::handle<NCollection_Buffer> aBuffer;
  if (theCheckContent && !DE_ValidationUtils::CreateContentBuffer(thePath, aBuffer))
  {
    return false;
  }
  OSD_Path                      aPath(thePath);
  const TCollection_AsciiString anExtr = aPath.Extension();
  for (NCollection_DataMap<TCollection_AsciiString, NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>>::Iterator aFormatIter(myConfiguration); aFormatIter.More();
       aFormatIter.Next())
  {
    for (NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>::Iterator aVendorIter(aFormatIter.Value()); aVendorIter.More();
         aVendorIter.Next())
    {
      const occ::handle<DE_ConfigurationNode>& aNode = aVendorIter.Value();
      if (aNode->IsEnabled() && aNode->IsImportSupported()
          && (aNode->CheckExtension(anExtr) || (theCheckContent && aNode->CheckContent(aBuffer)))
          && aNode->UpdateLoad(true, myKeepUpdates))
      {
        theProvider             = aNode->BuildProvider();
        aNode->GlobalParameters = GlobalParameters;
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

bool DE_Wrapper::FindReadProvider(const TCollection_AsciiString& thePath,
                                              std::istream&                  theStream,
                                              occ::handle<DE_Provider>&           theProvider) const
{
  occ::handle<NCollection_Buffer> aBuffer;
  if (!DE_ValidationUtils::CreateContentBuffer(theStream, aBuffer))
  {
    return false;
  }

  OSD_Path                      aPath(thePath);
  const TCollection_AsciiString anExtr = aPath.Extension();
  for (NCollection_DataMap<TCollection_AsciiString, NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>>::Iterator aFormatIter(myConfiguration); aFormatIter.More();
       aFormatIter.Next())
  {
    for (NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>::Iterator aVendorIter(aFormatIter.Value()); aVendorIter.More();
         aVendorIter.Next())
    {
      const occ::handle<DE_ConfigurationNode>& aNode = aVendorIter.Value();
      if (aNode->IsEnabled() && aNode->IsImportSupported()
          && (aNode->CheckExtension(anExtr) || aNode->CheckContent(aBuffer))
          && aNode->UpdateLoad(true, myKeepUpdates))
      {
        theProvider             = aNode->BuildProvider();
        aNode->GlobalParameters = GlobalParameters;
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

bool DE_Wrapper::FindWriteProvider(const TCollection_AsciiString& thePath,
                                               occ::handle<DE_Provider>&           theProvider) const
{
  OSD_Path                      aPath(thePath);
  const TCollection_AsciiString anExtr = aPath.Extension();
  for (NCollection_DataMap<TCollection_AsciiString, NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>>::Iterator aFormatIter(myConfiguration); aFormatIter.More();
       aFormatIter.Next())
  {
    for (NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>::Iterator aVendorIter(aFormatIter.Value()); aVendorIter.More();
         aVendorIter.Next())
    {
      const occ::handle<DE_ConfigurationNode>& aNode = aVendorIter.Value();
      if (aNode->IsEnabled() && aNode->IsExportSupported() && aNode->CheckExtension(anExtr)
          && aNode->UpdateLoad(false, myKeepUpdates))
      {
        theProvider             = aNode->BuildProvider();
        aNode->GlobalParameters = GlobalParameters;
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

Standard_EXPORT void DE_Wrapper::UpdateLoad(const bool theToForceUpdate) const
{
  for (NCollection_DataMap<TCollection_AsciiString, NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>>::Iterator aFormatIter(myConfiguration); aFormatIter.More();
       aFormatIter.Next())
  {
    for (NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>::Iterator aVendorIter(aFormatIter.Value()); aVendorIter.More();
         aVendorIter.Next())
    {
      const occ::handle<DE_ConfigurationNode>& aNode = aVendorIter.Value();
      aNode->UpdateLoad(true, true);
      aNode->UpdateLoad(false, true);
      if (!theToForceUpdate)
        continue;
      aNode->SetEnabled(aNode->IsExportSupported() || aNode->IsImportSupported());
    }
  }
}

//=================================================================================================

void DE_Wrapper::sort(const occ::handle<DE_ConfigurationContext>& theResource)
{
  const TCollection_AsciiString aScope(THE_CONFIGURATION_SCOPE() + '.' + "priority");
  for (NCollection_DataMap<TCollection_AsciiString, NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<DE_ConfigurationNode>>>::Iterator aFormatIter(myConfiguration); aFormatIter.More();
       aFormatIter.Next())
  {
    NCollection_List<TCollection_AsciiString> aVendorPriority;
    if (!theResource->GetStringSeq(aFormatIter.Key(), aVendorPriority, aScope))
    {
      continue;
    }
    ChangePriority(aFormatIter.Key(), aVendorPriority, true);
  }
}

//=================================================================================================

bool DE_Wrapper::Read(DE_Provider::ReadStreamList&    theStreams,
                                  const occ::handle<TDocStd_Document>& theDocument,
                                  occ::handle<XSControl_WorkSession>&  theWS,
                                  const Message_ProgressRange&    theProgress)
{
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, "DE_Wrapper Read"))
  {
    return false;
  }

  const TCollection_AsciiString& aFirstKey = theStreams.First().Path;

  occ::handle<DE_Provider> aProvider;
  Standard_IStream&   aFirstStream = theStreams.First().Stream;
  if (!FindReadProvider(aFirstKey, aFirstStream, aProvider))
  {
    Message::SendFail() << "Error: DE_Wrapper cannot find provider for stream " << aFirstKey;
    return false;
  }

  if (!aProvider->GetNode()->IsStreamSupported())
  {
    Message::SendFail() << "Error: Provider " << aProvider->GetFormat() << " "
                        << aProvider->GetVendor() << " doesn't support stream operations";
    return false;
  }

  return aProvider->Read(theStreams, theDocument, theWS, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Write(DE_Provider::WriteStreamList&   theStreams,
                                   const occ::handle<TDocStd_Document>& theDocument,
                                   occ::handle<XSControl_WorkSession>&  theWS,
                                   const Message_ProgressRange&    theProgress)
{
  if (!DE_ValidationUtils::ValidateWriteStreamList(theStreams, "DE_Wrapper Write"))
  {
    return false;
  }

  const TCollection_AsciiString& aFirstKey = theStreams.First().Path;

  occ::handle<DE_Provider> aProvider;
  if (!FindWriteProvider(aFirstKey, aProvider))
  {
    Message::SendFail() << "Error: DE_Wrapper cannot find provider for stream " << aFirstKey;
    return false;
  }

  if (!aProvider->GetNode()->IsStreamSupported())
  {
    Message::SendFail() << "Error: Provider " << aProvider->GetFormat() << " "
                        << aProvider->GetVendor() << " doesn't support stream operations";
    return false;
  }

  return aProvider->Write(theStreams, theDocument, theWS, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Read(DE_Provider::ReadStreamList&    theStreams,
                                  const occ::handle<TDocStd_Document>& theDocument,
                                  const Message_ProgressRange&    theProgress)
{
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, "DE_Wrapper Read"))
  {
    return false;
  }

  const TCollection_AsciiString& aFirstKey = theStreams.First().Path;

  occ::handle<DE_Provider> aProvider;
  Standard_IStream&   aFirstStream = theStreams.First().Stream;
  if (!FindReadProvider(aFirstKey, aFirstStream, aProvider))
  {
    Message::SendFail() << "Error: DE_Wrapper cannot find provider for stream " << aFirstKey;
    return false;
  }

  if (!aProvider->GetNode()->IsStreamSupported())
  {
    Message::SendFail() << "Error: Provider " << aProvider->GetFormat() << " "
                        << aProvider->GetVendor() << " doesn't support stream operations";
    return false;
  }

  return aProvider->Read(theStreams, theDocument, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Write(DE_Provider::WriteStreamList&   theStreams,
                                   const occ::handle<TDocStd_Document>& theDocument,
                                   const Message_ProgressRange&    theProgress)
{
  if (!DE_ValidationUtils::ValidateWriteStreamList(theStreams, "DE_Wrapper Write"))
  {
    return false;
  }

  const TCollection_AsciiString& aFirstKey = theStreams.First().Path;

  occ::handle<DE_Provider> aProvider;
  if (!FindWriteProvider(aFirstKey, aProvider))
  {
    Message::SendFail() << "Error: DE_Wrapper cannot find provider for stream " << aFirstKey;
    return false;
  }

  if (!aProvider->GetNode()->IsStreamSupported())
  {
    Message::SendFail() << "Error: Provider " << aProvider->GetFormat() << " "
                        << aProvider->GetVendor() << " doesn't support stream operations";
    return false;
  }

  return aProvider->Write(theStreams, theDocument, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Read(DE_Provider::ReadStreamList&   theStreams,
                                  TopoDS_Shape&                  theShape,
                                  occ::handle<XSControl_WorkSession>& theWS,
                                  const Message_ProgressRange&   theProgress)
{
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, "DE_Wrapper Read"))
  {
    return false;
  }

  const TCollection_AsciiString& aFirstKey = theStreams.First().Path;

  occ::handle<DE_Provider> aProvider;
  Standard_IStream&   aFirstStream = theStreams.First().Stream;
  if (!FindReadProvider(aFirstKey, aFirstStream, aProvider))
  {
    Message::SendFail() << "Error: DE_Wrapper cannot find provider for stream " << aFirstKey;
    return false;
  }

  if (!aProvider->GetNode()->IsStreamSupported())
  {
    Message::SendFail() << "Error: Provider " << aProvider->GetFormat() << " "
                        << aProvider->GetVendor() << " doesn't support stream operations";
    return false;
  }

  return aProvider->Read(theStreams, theShape, theWS, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Write(DE_Provider::WriteStreamList&  theStreams,
                                   const TopoDS_Shape&            theShape,
                                   occ::handle<XSControl_WorkSession>& theWS,
                                   const Message_ProgressRange&   theProgress)
{
  if (!DE_ValidationUtils::ValidateWriteStreamList(theStreams, "DE_Wrapper Write"))
  {
    return false;
  }

  const TCollection_AsciiString& aFirstKey = theStreams.First().Path;

  occ::handle<DE_Provider> aProvider;
  if (!FindWriteProvider(aFirstKey, aProvider))
  {
    Message::SendFail() << "Error: DE_Wrapper cannot find provider for stream " << aFirstKey;
    return false;
  }

  if (!aProvider->GetNode()->IsStreamSupported())
  {
    Message::SendFail() << "Error: Provider " << aProvider->GetFormat() << " "
                        << aProvider->GetVendor() << " doesn't support stream operations";
    return false;
  }

  return aProvider->Write(theStreams, theShape, theWS, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Read(DE_Provider::ReadStreamList& theStreams,
                                  TopoDS_Shape&                theShape,
                                  const Message_ProgressRange& theProgress)
{
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, "DE_Wrapper Read"))
  {
    return false;
  }

  const TCollection_AsciiString& aFirstKey = theStreams.First().Path;

  occ::handle<DE_Provider> aProvider;
  Standard_IStream&   aFirstStream = theStreams.First().Stream;
  if (!FindReadProvider(aFirstKey, aFirstStream, aProvider))
  {
    Message::SendFail() << "Error: DE_Wrapper cannot find provider for stream " << aFirstKey;
    return false;
  }

  if (!aProvider->GetNode()->IsStreamSupported())
  {
    Message::SendFail() << "Error: Provider " << aProvider->GetFormat() << " "
                        << aProvider->GetVendor() << " doesn't support stream operations";
    return false;
  }

  return aProvider->Read(theStreams, theShape, theProgress);
}

//=================================================================================================

bool DE_Wrapper::Write(DE_Provider::WriteStreamList& theStreams,
                                   const TopoDS_Shape&           theShape,
                                   const Message_ProgressRange&  theProgress)
{
  if (!DE_ValidationUtils::ValidateWriteStreamList(theStreams, "DE_Wrapper Write"))
  {
    return false;
  }

  const TCollection_AsciiString& aFirstKey = theStreams.First().Path;

  occ::handle<DE_Provider> aProvider;
  if (!FindWriteProvider(aFirstKey, aProvider))
  {
    Message::SendFail() << "Error: DE_Wrapper cannot find provider for stream " << aFirstKey;
    return false;
  }

  if (!aProvider->GetNode()->IsStreamSupported())
  {
    Message::SendFail() << "Error: Provider " << aProvider->GetFormat() << " "
                        << aProvider->GetVendor() << " doesn't support stream operations";
    return false;
  }

  return aProvider->Write(theStreams, theShape, theProgress);
}
