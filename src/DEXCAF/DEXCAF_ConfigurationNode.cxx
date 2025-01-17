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

#include <DEXCAF_ConfigurationNode.hxx>

#include <DEXCAF_Provider.hxx>
#include <DE_ConfigurationContext.hxx>
#include <DE_PluginHolder.hxx>
#include <NCollection_Buffer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEXCAF_ConfigurationNode, DE_ConfigurationNode)

namespace
{
static const TCollection_AsciiString& THE_CONFIGURATION_SCOPE()
{
  static const TCollection_AsciiString aScope = "provider";
  return aScope;
}

// Wrapper to auto-load DE component
DE_PluginHolder<DEXCAF_ConfigurationNode> THE_OCCT_XCAF_COMPONENT_PLUGIN;
} // namespace

//=================================================================================================

DEXCAF_ConfigurationNode::DEXCAF_ConfigurationNode()
    : DE_ConfigurationNode()
{
}

//=================================================================================================

DEXCAF_ConfigurationNode::DEXCAF_ConfigurationNode(const Handle(DEXCAF_ConfigurationNode)& theNode)
    : DE_ConfigurationNode(theNode)
{
  InternalParameters = theNode->InternalParameters;
}

//=================================================================================================

bool DEXCAF_ConfigurationNode::Load(const Handle(DE_ConfigurationContext)& theResource)
{
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor();

  InternalParameters.ReadAppendMode =
    (PCDM_ReaderFilter::AppendMode)theResource->IntegerVal("read.append.mode",
                                                           InternalParameters.ReadAppendMode,
                                                           aScope);
  theResource->GetStringSeq("read.skip.values", InternalParameters.ReadSkipValues, aScope);
  theResource->GetStringSeq("read.values", InternalParameters.ReadValues, aScope);

  return true;
}

//=================================================================================================

TCollection_AsciiString DEXCAF_ConfigurationNode::Save() const
{
  TCollection_AsciiString aResult;
  aResult += "!*****************************************************************************\n";
  aResult =
    aResult + "!Configuration Node " + " Vendor: " + GetVendor() + " Format: " + GetFormat() + "\n";
  TCollection_AsciiString aScope =
    THE_CONFIGURATION_SCOPE() + "." + GetFormat() + "." + GetVendor() + ".";

  aResult += "!\n";
  aResult += "!Read parameters:\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Don't allow append (when the value  = 0, it is the default value), ";
  aResult += "keeps existing attributes, reads only new ones(when the value = 1), ";
  aResult += "overwrites the existing attributes by the loaded ones(when the value = 2)\n";
  aResult += "!Default value: 0. Available values: 0, 1, 2\n";
  aResult += aScope + "read.append.mode :\t " + InternalParameters.ReadAppendMode + "\n";
  aResult += "!\n";

  aResult += "!\n";
  aResult += "!Overwrites the existing attributes by the loaded ones";
  aResult += "!Default value: empty. Available values: {sequence<string>}\n";
  aResult += aScope + "read.skip.values :\t ";
  for (TColStd_ListOfAsciiString::Iterator anIt(InternalParameters.ReadSkipValues); anIt.More();
       anIt.Next())
  {
    aResult += anIt.Value() + " ";
  }
  aResult += "\n!\n";

  aResult += "!\n";
  aResult += "!1) Adds sub-tree path like \"0:2\"";
  aResult += "2) Adds attribute to read by typename. Disables the skipped attributes added. (there "
             "shouldn't be '0' after -read)\n";
  aResult += "!Default value: empty. Available values: {sequence<string>}\n";
  aResult += aScope + "read.values :\t ";
  for (TColStd_ListOfAsciiString::Iterator anIt(InternalParameters.ReadValues); anIt.More();
       anIt.Next())
  {
    aResult += anIt.Value() + " ";
  }
  aResult += "\n!\n";

  aResult += "!*****************************************************************************\n";
  return aResult;
}

//=================================================================================================

Handle(DE_ConfigurationNode) DEXCAF_ConfigurationNode::Copy() const
{
  return new DEXCAF_ConfigurationNode(*this);
}

//=================================================================================================

Handle(DE_Provider) DEXCAF_ConfigurationNode::BuildProvider()
{
  return new DEXCAF_Provider(this);
}

//=================================================================================================

bool DEXCAF_ConfigurationNode::IsImportSupported() const
{
  return true;
}

//=================================================================================================

bool DEXCAF_ConfigurationNode::IsExportSupported() const
{
  return true;
}

//=================================================================================================

TCollection_AsciiString DEXCAF_ConfigurationNode::GetFormat() const
{
  return TCollection_AsciiString("XCAF");
}

//=================================================================================================

TCollection_AsciiString DEXCAF_ConfigurationNode::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=================================================================================================

TColStd_ListOfAsciiString DEXCAF_ConfigurationNode::GetExtensions() const
{
  TColStd_ListOfAsciiString anExt;
  anExt.Append("xbf");
  return anExt;
}

//=================================================================================================

bool DEXCAF_ConfigurationNode::CheckContent(const Handle(NCollection_Buffer)& theBuffer) const
{
  if (theBuffer.IsNull() || theBuffer->Size() < 8)
  {
    return false;
  }
  const char* aBytes = (const char*)theBuffer->Data();
  if (!::strncmp(aBytes, "BINFILE", 7))
  {
    return true;
  }
  return false;
}
