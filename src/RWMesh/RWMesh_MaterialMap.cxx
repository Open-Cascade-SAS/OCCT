// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#include <RWMesh_MaterialMap.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWMesh_MaterialMap, Standard_Transient)

// =======================================================================
// function : RWMesh_MaterialMap
// purpose  :
// =======================================================================
RWMesh_MaterialMap::RWMesh_MaterialMap (const TCollection_AsciiString& theFile)
: myFileName (theFile),
  myKeyPrefix ("mat_"),
  myNbMaterials (0),
  myIsFailed (false),
  myMatNameAsKey (true)
{
  TCollection_AsciiString aFileName, aFileExt;
  OSD_Path::FolderAndFileFromPath (theFile, myFolder, aFileName);
  OSD_Path::FileNameAndExtension (aFileName, myShortFileNameBase, aFileExt);
}

// =======================================================================
// function : ~RWMesh_MaterialMap
// purpose  :
// =======================================================================
RWMesh_MaterialMap::~RWMesh_MaterialMap()
{
  //
}

// =======================================================================
// function : AddMaterial
// purpose  :
// =======================================================================
TCollection_AsciiString RWMesh_MaterialMap::AddMaterial (const XCAFPrs_Style& theStyle)
{
  if (myStyles.IsBound1 (theStyle))
  {
    return myStyles.Find1 (theStyle);
  }

  TCollection_AsciiString aMatKey, aMatName, aMatNameSuffix;
  int  aCounter    = 0;
  int* aCounterPtr = &myNbMaterials;
  if (myMatNameAsKey)
  {
    if (!theStyle.Material().IsNull()
     && !theStyle.Material()->IsEmpty())
    {
      aCounterPtr = &aCounter;
      Handle(TDataStd_Name) aNodeName;
      if (!theStyle.Material()->Label().IsNull()
       &&  theStyle.Material()->Label().FindAttribute (TDataStd_Name::GetID(), aNodeName))
      {
        aMatName = aNodeName->Get();
      }
      else
      {
        aMatName = "mat";
      }
      aMatNameSuffix = aMatName;
    }
    else
    {
      ++myNbMaterials;
      aMatNameSuffix = myKeyPrefix;
      aMatName = aMatNameSuffix + myNbMaterials;
    }
    aMatKey = aMatName;
  }
  else
  {
    aMatKey        = myNbMaterials++; // starts from 0
    aMatNameSuffix = myKeyPrefix;
    aMatName       = aMatNameSuffix + aMatKey;
  }

  for (;; ++(*aCounterPtr))
  {
    if (myStyles.IsBound2 (aMatKey))
    {
      if (myMatNameAsKey)
      {
        aMatName = aMatNameSuffix + (*aCounterPtr);
        aMatKey  = aMatName;
      }
      else
      {
        aMatKey  = *aCounterPtr;
        aMatName = aMatNameSuffix + aMatKey;
      }
      continue;
    }
    break;
  }

  myStyles.Bind (theStyle, aMatKey);
  DefineMaterial (theStyle, aMatKey, aMatName);
  return aMatKey;
}

// =======================================================================
// function : copyFileTo
// purpose  :
// =======================================================================
bool RWMesh_MaterialMap::copyFileTo (const TCollection_AsciiString& theFileSrc,
                                     const TCollection_AsciiString& theFileDst)
{
  if (theFileSrc.IsEmpty()
   || theFileDst.IsEmpty())
  {
    return false;
  }
  else if (theFileSrc == theFileDst)
  {
    return true;
  }

  try
  {
    OSD_Path aSrcPath (theFileSrc);
    OSD_Path aDstPath (theFileDst);
    OSD_File aFileSrc (aSrcPath);
    if (!aFileSrc.Exists())
    {
      Message::SendFail (TCollection_AsciiString("Failed to copy file - source file '") + theFileSrc + "' does not exist");
      return false;
    }
    aFileSrc.Copy (aDstPath);
    return !aFileSrc.Failed();
  }
  catch (Standard_Failure const& theException)
  {
    Message::SendFail (TCollection_AsciiString("Failed to copy file\n") + theException.GetMessageString());
    return false;
  }
}

// =======================================================================
// function : CopyTexture
// purpose  :
// =======================================================================
bool RWMesh_MaterialMap::CopyTexture (TCollection_AsciiString& theResTexture,
                                      const Handle(Image_Texture)& theTexture,
                                      const TCollection_AsciiString& theKey)
{
  CreateTextureFolder();

  TCollection_AsciiString aTexFileName;
  TCollection_AsciiString aTextureSrc = theTexture->FilePath();
  if (!aTextureSrc.IsEmpty()
    && theTexture->FileOffset() <= 0
    && theTexture->FileLength() <= 0)
  {
    TCollection_AsciiString aSrcTexFolder;
    OSD_Path::FolderAndFileFromPath (aTextureSrc, aSrcTexFolder, aTexFileName);
    const TCollection_AsciiString aResTexFile = myTexFolder + aTexFileName;
    theResTexture = myTexFolderShort + aTexFileName;
    return copyFileTo (aTextureSrc, aResTexFile);
  }

  TCollection_AsciiString anExt = theTexture->ProbeImageFileFormat();
  if (anExt.IsEmpty())
  {
    anExt = "bin";
  }
  aTexFileName = theKey + "." + anExt;

  const TCollection_AsciiString aResTexFile = myTexFolder + aTexFileName;
  theResTexture = myTexFolderShort + aTexFileName;
  return theTexture->WriteImage (aResTexFile);
}

// =======================================================================
// function : CreateTextureFolder
// purpose  :
// =======================================================================
bool RWMesh_MaterialMap::CreateTextureFolder()
{
  if (!myTexFolder.IsEmpty())
  {
    return true;
  }

  myTexFolderShort = myShortFileNameBase + "_textures/";
  myTexFolder      = myFolder + "/" + myTexFolderShort;
  OSD_Path aTexFolderPath (myTexFolder);
  OSD_Directory aTexDir (aTexFolderPath);
  if (aTexDir.Exists())
  {
    return true;
  }

  OSD_Path aResFolderPath (myFolder);
  OSD_Directory aResDir (aResFolderPath);
  if (!aResDir.Exists())
  {
    return false;
  }
  const OSD_Protection aParentProt = aResDir.Protection();
  OSD_Protection aProt = aParentProt;
  if (aProt.User() == OSD_None)
  {
    aProt.SetUser (OSD_RWXD);
  }
  if (aProt.System() == OSD_None)
  {
    aProt.SetSystem (OSD_RWXD);
  }

  aTexDir.Build (aProt);
  if (aTexDir.Failed())
  {
    // fallback to the same folder as output model file
    myTexFolder = myFolder;
    myTexFolderShort.Clear();
    return true;
  }
  return true;
}
