// Copyright (c) 2023 OPEN CASCADE SAS
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

#include <XSDRAWDE.hxx>

#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <DE_ConfigurationContext.hxx>
#include <DE_Provider.hxx>
#include <DE_Wrapper.hxx>
#include <DEBREP_ConfigurationNode.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Message.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TopoDS_Shape.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>

//=================================================================================================

static Standard_Integer DumpConfiguration(Draw_Interpretor& theDI,
                                          Standard_Integer  theNbArgs,
                                          const char**      theArgVec)
{
  Handle(DE_Wrapper)        aConf = DE_Wrapper::GlobalWrapper();
  TCollection_AsciiString   aPath;
  Standard_Boolean          aIsRecursive    = Standard_True;
  Standard_Boolean          isHandleFormat  = Standard_False;
  Standard_Boolean          isHandleVendors = Standard_False;
  TColStd_ListOfAsciiString aFormats;
  TColStd_ListOfAsciiString aVendors;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if ((anArg == "-path") && (anArgIter + 1 < theNbArgs))
    {
      ++anArgIter;
      aPath = theArgVec[anArgIter];
    }
    else if ((anArg == "-recursive") && (anArgIter + 1 < theNbArgs)
             && Draw::ParseOnOff(theArgVec[anArgIter + 1], aIsRecursive))
    {
      ++anArgIter;
    }
    else if (anArg == "-format")
    {
      isHandleFormat  = Standard_True;
      isHandleVendors = Standard_False;
    }
    else if (anArg == "-vendor")
    {
      isHandleFormat  = Standard_False;
      isHandleVendors = Standard_True;
    }
    else if (isHandleFormat)
    {
      aFormats.Append(theArgVec[anArgIter]);
    }
    else if (isHandleVendors)
    {
      aVendors.Append(theArgVec[anArgIter]);
    }
    else if (!isHandleFormat && !isHandleVendors)
    {
      theDI << "Syntax error at argument '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }
  Standard_Boolean aStat = Standard_True;
  if (!aPath.IsEmpty())
  {
    aStat = aConf->Save(aPath, aIsRecursive, aFormats, aVendors);
  }
  else
  {
    theDI << aConf->Save(aIsRecursive, aFormats, aVendors) << "\n";
  }
  if (!aStat)
  {
    return 1;
  }
  return 0;
}

//=================================================================================================

static Standard_Integer CompareConfiguration(Draw_Interpretor& theDI,
                                             Standard_Integer  theNbArgs,
                                             const char**      theArgVec)
{
  if (theNbArgs > 5)
  {
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }
  Handle(DE_ConfigurationContext) aResourceFirst = new DE_ConfigurationContext();
  if (!aResourceFirst->Load(theArgVec[1]))
  {
    theDI << "Error: Can't load first configuration\n";
    return 1;
  }
  Handle(DE_ConfigurationContext) aResourceSecond = new DE_ConfigurationContext();
  if (!aResourceSecond->Load(theArgVec[2]))
  {
    theDI << "Error: Can't load second configuration\n";
    return 1;
  }
  const DE_ResourceMap& aResourceMapFirst  = aResourceFirst->GetInternalMap();
  const DE_ResourceMap& aResourceMapSecond = aResourceSecond->GetInternalMap();
  Standard_Integer      anDiffers          = 0;
  for (DE_ResourceMap::Iterator anOrigIt(aResourceMapFirst); anOrigIt.More(); anOrigIt.Next())
  {
    const TCollection_AsciiString& anOrigValue = anOrigIt.Value();
    const TCollection_AsciiString& anOrigKey   = anOrigIt.Key();
    TCollection_AsciiString        aCompValue;
    if (!aResourceMapSecond.Find(anOrigKey, aCompValue))
    {
      Message::SendWarning() << "Second configuration don't have the next scope : " << anOrigKey;
      anDiffers++;
    }
    if (!aCompValue.IsEqual(anOrigValue))
    {
      Message::SendWarning() << "Configurations have differs value with the next scope :"
                             << anOrigKey << " First value : " << anOrigValue
                             << " Second value : " << aCompValue;
      anDiffers++;
    }
  }
  TCollection_AsciiString aMessage;
  if (aResourceMapFirst.Extent() != aResourceMapSecond.Extent() || anDiffers > 0)
  {
    theDI << "Error: Configurations are not same : " << " Differs count : " << anDiffers
          << " Count of first's scopes : " << aResourceMapFirst.Extent()
          << " Count of second's scopes : " << aResourceMapSecond.Extent() << "\n";
    return 1;
  }
  return 0;
}

//=================================================================================================

static Standard_Integer LoadConfiguration(Draw_Interpretor& theDI,
                                          Standard_Integer  theNbArgs,
                                          const char**      theArgVec)
{
  if (theNbArgs > 4)
  {
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }
  Handle(DE_Wrapper)      aConf        = DE_Wrapper::GlobalWrapper();
  TCollection_AsciiString aString      = theArgVec[1];
  Standard_Boolean        aIsRecursive = Standard_True;
  if (theNbArgs == 4)
  {
    TCollection_AsciiString anArg = theArgVec[2];
    anArg.LowerCase();
    if (!(anArg == "-recursive") || !Draw::ParseOnOff(theArgVec[3], aIsRecursive))
    {
      theDI << "Syntax error at argument '" << theArgVec[3] << "'\n";
      return 1;
    }
  }
  if (!aConf->Load(aString, aIsRecursive))
  {
    theDI << "Error: configuration is incorrect\n";
    return 1;
  }
  return 0;
}

//=================================================================================================

static Standard_Integer ReadFile(Draw_Interpretor& theDI,
                                 Standard_Integer  theNbArgs,
                                 const char**      theArgVec)
{
  if (theNbArgs > 6)
  {
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }
  TCollection_AsciiString     aDocShapeName;
  TCollection_AsciiString     aFilePath;
  Handle(TDocStd_Document)    aDoc;
  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
  TCollection_AsciiString     aConfString;
  Standard_Boolean            isNoDoc = (TCollection_AsciiString(theArgVec[0]) == "readfile");
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if ((anArg == "-conf") && (anArgIter + 1 < theNbArgs))
    {
      ++anArgIter;
      aConfString = theArgVec[anArgIter];
    }
    else if (aDocShapeName.IsEmpty())
    {
      aDocShapeName             = theArgVec[anArgIter];
      Standard_CString aNameVar = aDocShapeName.ToCString();
      if (!isNoDoc)
      {
        DDocStd::GetDocument(aNameVar, aDoc, Standard_False);
      }
    }
    else if (aFilePath.IsEmpty())
    {
      aFilePath = theArgVec[anArgIter];
    }
    else
    {
      theDI << "Syntax error at argument '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }
  if (aDocShapeName.IsEmpty() || aFilePath.IsEmpty())
  {
    theDI << "Syntax error: wrong number of arguments\n";
    return 1;
  }
  if (aDoc.IsNull() && !isNoDoc)
  {
    anApp->NewDocument(TCollection_ExtendedString("BinXCAF"), aDoc);
    Handle(DDocStd_DrawDocument) aDrawDoc = new DDocStd_DrawDocument(aDoc);
    TDataStd_Name::Set(aDoc->GetData()->Root(), theArgVec[1]);
    Draw::Set(theArgVec[1], aDrawDoc);
  }

  Handle(DE_Wrapper)            aConf = DE_Wrapper::GlobalWrapper()->Copy();
  Handle(XSControl_WorkSession) aWS   = XSDRAW::Session();
  Standard_Boolean              aStat = Standard_True;
  if (!aConfString.IsEmpty())
  {
    aStat = aConf->Load(aConfString);
  }
  if (aStat)
  {
    TopoDS_Shape aShape;
    aStat = isNoDoc ? aConf->Read(aFilePath, aShape, aWS) : aConf->Read(aFilePath, aDoc, aWS);
    if (isNoDoc && aStat)
    {
      DBRep::Set(aDocShapeName.ToCString(), aShape);
    }
  }
  if (!aStat)
  {
    return 1;
  }
  XSDRAW::CollectActiveWorkSessions(aFilePath);
  return 0;
}

//=================================================================================================

static Standard_Integer WriteFile(Draw_Interpretor& theDI,
                                  Standard_Integer  theNbArgs,
                                  const char**      theArgVec)
{
  if (theNbArgs > 6)
  {
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }
  TCollection_AsciiString  aDocShapeName;
  TCollection_AsciiString  aFilePath;
  Handle(TDocStd_Document) aDoc;
  TCollection_AsciiString  aConfString;
  Standard_Boolean         isNoDoc = (TCollection_AsciiString(theArgVec[0]) == "writefile");
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if ((anArg == "-conf") && (anArgIter + 1 < theNbArgs))
    {
      ++anArgIter;
      aConfString = theArgVec[anArgIter];
    }
    else if (aDocShapeName.IsEmpty())
    {
      aDocShapeName             = theArgVec[anArgIter];
      Standard_CString aNameVar = aDocShapeName.ToCString();
      if (!isNoDoc)
      {
        DDocStd::GetDocument(aNameVar, aDoc, Standard_False);
      }
    }
    else if (aFilePath.IsEmpty())
    {
      aFilePath = theArgVec[anArgIter];
    }
    else
    {
      theDI << "Syntax error at argument '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }
  if (aDocShapeName.IsEmpty() || aFilePath.IsEmpty())
  {
    theDI << "Syntax error: wrong number of arguments\n";
    return 1;
  }
  if (aDoc.IsNull() && !isNoDoc)
  {
    theDI << "Error: incorrect document\n";
    return 1;
  }
  Handle(DE_Wrapper)            aConf = DE_Wrapper::GlobalWrapper()->Copy();
  Handle(XSControl_WorkSession) aWS   = XSDRAW::Session();
  Standard_Boolean              aStat = Standard_True;
  if (!aConfString.IsEmpty())
  {
    aStat = aConf->Load(aConfString);
  }
  if (aStat)
  {
    if (isNoDoc)
    {
      TopoDS_Shape aShape = DBRep::Get(aDocShapeName);
      if (aShape.IsNull())
      {
        theDI << "Error: incorrect shape " << aDocShapeName << "\n";
        return 1;
      }
      aStat = aConf->Write(aFilePath, aShape, aWS);
    }
    else
    {
      aStat = aConf->Write(aFilePath, aDoc, aWS);
    }
  }
  if (!aStat)
  {
    return 1;
  }
  XSDRAW::CollectActiveWorkSessions(aFilePath);
  return 0;
}

//=================================================================================================

void XSDRAWDE::Factory(Draw_Interpretor& theDI)
{
  static Standard_Boolean aIsActivated = Standard_False;
  if (aIsActivated)
  {
    return;
  }
  aIsActivated = Standard_True;

  Standard_CString aGroup = "XDE translation commands";
  theDI.Add("DumpConfiguration",
            "DumpConfiguration [-path <path>] [-recursive {on|off}] [-format fmt1 fmt2 ...] "
            "[-vendor vend1 vend2 ...]\n"
            "\n\t\t: Dump special resource generated from global configuration."
            "\n\t\t:   '-path' - save resource configuration to the file"
            "\n\t\t:   '-recursive' - flag to generate a resource from providers. Default is On. "
            "Off disables other options"
            "\n\t\t:   '-format' - flag to generate a resource for chosen formats. If list is "
            "empty, generate it for all"
            "\n\t\t:   '-vendor' - flag to generate a resource for chosen vendors. If list is "
            "empty, generate it for all",
            __FILE__,
            DumpConfiguration,
            aGroup);
  theDI.Add(
    "LoadConfiguration",
    "LoadConfiguration conf [-recursive {on|off}]\n"
    "\n\t\t:   'conf' - path to the resource file or string value in the special format"
    "\n\t\t:   '-recursive' - flag to generate a resource for all providers. Default is true"
    "\n\t\t: Configure global configuration according special resource",
    __FILE__,
    LoadConfiguration,
    aGroup);
  theDI.Add(
    "CompareConfiguration",
    "CompareConfiguration conf1 conf2\n"
    "\n\t\t:   'conf1' - path to the first resource file or string value in the special format"
    "\n\t\t:   'conf2' - path to the second resource file or string value in the special format"
    "\n\t\t: Compare two configurations",
    __FILE__,
    CompareConfiguration,
    aGroup);
  theDI.Add("ReadFile",
            "ReadFile docName filePath [-conf <value|path>]\n"
            "\n\t\t: Read CAD file to document with registered format's providers. Use global "
            "configuration by default.",
            __FILE__,
            ReadFile,
            aGroup);
  theDI.Add("readfile",
            "readfile shapeName filePath [-conf <value|path>]\n"
            "\n\t\t: Read CAD file to shape with registered format's providers. Use global "
            "configuration by default.",
            __FILE__,
            ReadFile,
            aGroup);
  theDI.Add("WriteFile",
            "WriteFile docName filePath [-conf <value|path>]\n"
            "\n\t\t: Write CAD file to document with registered format's providers. Use global "
            "configuration by default.",
            __FILE__,
            WriteFile,
            aGroup);
  theDI.Add("writefile",
            "writefile shapeName filePath [-conf <value|path>]\n"
            "\n\t\t: Write CAD file to shape with registered format's providers. Use global "
            "configuration by default.",
            __FILE__,
            WriteFile,
            aGroup);

  // Load XSDRAW session for pilot activation
  XSDRAW::LoadDraw(theDI);

  // Workaround to force load TKDECascade lib
  DEBREP_ConfigurationNode aTmpObj;
  (void)aTmpObj;
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWDE)
