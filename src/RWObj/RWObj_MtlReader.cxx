// Author: Kirill Gavrilov
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

#include <RWObj_MtlReader.hxx>

#include <RWObj_Tools.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD_File.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_Path.hxx>

namespace
{
  //! Try to find a new location of the file relative to specified folder from absolute path.
  //! @param theAbsolutePath original absolute file path
  //! @param theNewFoler     the new folder to look for the file
  //! @param theRelativePath result file path relative to theNewFoler
  //! @return true if relative file has been found
  static bool findRelativePath (const TCollection_AsciiString& theAbsolutePath,
                                const TCollection_AsciiString& theNewFoler,
                                TCollection_AsciiString& theRelativePath)
  {
    TCollection_AsciiString aNewFoler = (theNewFoler.EndsWith ("\\") || theNewFoler.EndsWith ("/"))
                                      ?  theNewFoler
                                      : (theNewFoler + "/");

    TCollection_AsciiString aRelPath;
    TCollection_AsciiString aPath = theAbsolutePath;
    for (;;)
    {
      TCollection_AsciiString aFolder, aFileName;
      OSD_Path::FolderAndFileFromPath (aPath, aFolder, aFileName);
      if (aFolder.IsEmpty()
       || aFileName.IsEmpty())
      {
        return false;
      }

      if (aRelPath.IsEmpty())
      {
        aRelPath = aFileName;
      }
      else
      {
        aRelPath = aFileName + "/" + aRelPath;
      }

      if (OSD_File (aNewFoler + aRelPath).Exists())
      {
        theRelativePath = aRelPath;
        return true;
      }

      aPath = aFolder;
      for (; aPath.Length() >= 2;)
      {
        if (aPath.Value (aPath.Length()) == '/'
         || aPath.Value (aPath.Length()) == '\\')
        {
          aPath = aPath.SubString (1, aPath.Length() - 1);
          continue;
        }
        break;
      }
    }
  }
}

// =======================================================================
// function : RWObj_MtlReader
// purpose  :
// =======================================================================
RWObj_MtlReader::RWObj_MtlReader (NCollection_DataMap<TCollection_AsciiString, RWObj_Material>& theMaterials)
: myFile (NULL),
  myMaterials (&theMaterials),
  myNbLines (0)
{
  //
}

// =======================================================================
// function : ~RWObj_MtlReader
// purpose  :
// =======================================================================
RWObj_MtlReader::~RWObj_MtlReader()
{
  if (myFile != NULL)
  {
    ::fclose (myFile);
  }
}

// =======================================================================
// function : Read
// purpose  :
// =======================================================================
bool RWObj_MtlReader::Read (const TCollection_AsciiString& theFolder,
                            const TCollection_AsciiString& theFile)
{
  myPath = theFolder + theFile;
  myFile = OSD_OpenFile (myPath.ToCString(), "rb");
  if (myFile == NULL)
  {
    Message::Send (TCollection_AsciiString ("OBJ material file '") + myPath + "' is not found!", Message_Warning);
    return Standard_False;
  }

  char aLine[256] = {};
  TCollection_AsciiString aMatName;
  RWObj_Material aMat;
  const Standard_Integer aNbMatOld = myMaterials->Extent();
  bool hasAspect = false;
  for (; ::feof (myFile) == 0 && ::fgets (aLine, 255, myFile) != NULL; )
  {
    ++myNbLines;

    const char* aPos = aLine;

    // skip spaces
    for (; IsSpace(*aPos);)
    {
      ++aPos;
    }

    if (*aPos == '#'
     || *aPos == '\n'
     || *aPos == '\0')
    {
      continue;
    }

    if (::memcmp (aPos, "newmtl", 6) == 0)
    {
      aPos += 7;
      if (!aMatName.IsEmpty())
      {
        if (hasAspect)
        {
          aMat.Name = aMatName;
        }
        else
        {
          // reset incomplete material definition
          aMat = RWObj_Material();
        }
        myMaterials->Bind (aMatName, aMat);
        hasAspect = false;
      }

      aMatName = TCollection_AsciiString(aPos);
      aMat = RWObj_Material();
      if (!RWObj_Tools::ReadName (aPos, aMatName))
      {
        Message::SendWarning (TCollection_AsciiString("Empty OBJ material at line ") + myNbLines + " in file " + myPath);
      }
    }
    else if (::memcmp (aPos, "Ka", 2) == 0
          && IsSpace (aPos[2]))
    {
      aPos += 3;
      char* aNext = NULL;
      Graphic3d_Vec3 aColor;
      RWObj_Tools::ReadVec3 (aPos, aNext, aColor);
      aPos = aNext;
      if (validateColor (aColor))
      {
        aMat.AmbientColor = Quantity_Color (aColor.r(), aColor.g(), aColor.b(), Quantity_TOC_sRGB);
        hasAspect = true;
      }
    }
    else if (::memcmp (aPos, "Kd", 2) == 0
          && IsSpace (aPos[2]))
    {
      aPos += 3;
      char* aNext = NULL;
      Graphic3d_Vec3 aColor;
      RWObj_Tools::ReadVec3 (aPos, aNext, aColor);
      aPos = aNext;
      if (validateColor (aColor))
      {
        aMat.DiffuseColor = Quantity_Color (aColor.r(), aColor.g(), aColor.b(), Quantity_TOC_sRGB);
        hasAspect = true;
      }
    }
    else if (::memcmp (aPos, "Ks", 2) == 0
          && IsSpace (aPos[2]))
    {
      aPos += 3;
      char* aNext = NULL;
      Graphic3d_Vec3 aColor;
      RWObj_Tools::ReadVec3 (aPos, aNext, aColor);
      aPos = aNext;
      if (validateColor (aColor))
      {
        aMat.SpecularColor = Quantity_Color (aColor.r(), aColor.g(), aColor.b(), Quantity_TOC_sRGB);
        hasAspect = true;
      }
    }
    else if (::memcmp (aPos, "Ns", 2) == 0
          && IsSpace (aPos[2]))
    {
      aPos += 3;
      char* aNext = NULL;
      double aSpecular = Strtod (aPos, &aNext);
      aPos = aNext;
      if (aSpecular >= 0.0)
      {
        aMat.Shininess = (float )Min (aSpecular / 1000.0, 1.0);
        hasAspect = true;
      }
    }
    else if (::memcmp (aPos, "Tr", 2) == 0
          && IsSpace (aPos[2]))
    {
      aPos += 3;
      char* aNext = NULL;
      double aTransp = Strtod (aPos, &aNext);
      aPos = aNext;
      if (validateScalar (aTransp)
       && aTransp <= 0.99)
      {
        aMat.Transparency = (float )aTransp;
        hasAspect = true;
      }
    }
    else if (*aPos == 'd' && IsSpace (aPos[1]))
    {
      // dissolve
      aPos += 2;
      char* aNext = NULL;
      double anAlpha = Strtod (aPos, &aNext);
      aPos = aNext;
      if (validateScalar (anAlpha)
       && anAlpha >= 0.01)
      {
        aMat.Transparency = float(1.0 - anAlpha);
        hasAspect = true;
      }
    }
    else if (::memcmp (aPos, "map_Kd", 6) == 0
          && IsSpace (aPos[6]))
    {
      aPos += 7;
      if (RWObj_Tools::ReadName (aPos, aMat.DiffuseTexture))
      {
        processTexturePath (aMat.DiffuseTexture, theFolder);
        hasAspect = true;
      }
    }
    else if (::memcmp (aPos, "map_Ks", 6) == 0
          && IsSpace (aPos[6]))
    {
      aPos += 7;
      if (RWObj_Tools::ReadName (aPos, aMat.SpecularTexture))
      {
        processTexturePath (aMat.SpecularTexture, theFolder);
        hasAspect = true;
      }
    }
    else if (::memcmp (aPos, "map_Bump", 8) == 0
          && IsSpace (aPos[8]))
    {
      aPos += 9;
      if (RWObj_Tools::ReadName (aPos, aMat.BumpTexture))
      {
        processTexturePath (aMat.BumpTexture, theFolder);
        hasAspect = true;
      }
    }
    /*else if (::memcmp (aPos, "illum", 5) == 0)
    {
      aPos += 6;
      char* aNext = NULL;
      const int aModel = strtol (aPos, &aNext, 10);
      aPos = aNext;
      if (aModel < 0 || aModel > 10)
      {
        // unknown model
      }
    }*/
  }

  if (!aMatName.IsEmpty())
  {
    if (hasAspect)
    {
      aMat.Name = aMatName;
    }
    else
    {
      // reset incomplete material definition
      aMat = RWObj_Material();
    }
    myMaterials->Bind (aMatName, aMat);
  }

  return myMaterials->Extent() != aNbMatOld;
}

// =======================================================================
// function : processTexturePath
// purpose  :
// =======================================================================
void RWObj_MtlReader::processTexturePath (TCollection_AsciiString& theTexturePath,
                                          const TCollection_AsciiString& theFolder)
{
  if (OSD_Path::IsAbsolutePath (theTexturePath.ToCString()))
  {
    Message::SendWarning (TCollection_AsciiString("OBJ file specifies absolute path to the texture image file which may be inaccessible on another device\n")
                        + theTexturePath);
    if (!OSD_File (theTexturePath).Exists())
    {
      // workaround absolute filenames - try to find the same file at the OBJ file location
      TCollection_AsciiString aRelativePath;
      if (findRelativePath (theTexturePath, theFolder, aRelativePath))
      {
        theTexturePath = theFolder + aRelativePath;
      }
    }
  }
  else
  {
    theTexturePath = theFolder + theTexturePath;
  }
}

// =======================================================================
// function : validateScalar
// purpose  :
// =======================================================================
bool RWObj_MtlReader::validateScalar (const Standard_Real theValue)
{
  if (theValue < 0.0
   || theValue > 1.0)
  {
    Message::SendWarning (TCollection_AsciiString("Invalid scalar in OBJ material at line ") + myNbLines + " in file " + myPath);
    return false;
  }
  return true;
}

// =======================================================================
// function : validateColor
// purpose  :
// =======================================================================
bool RWObj_MtlReader::validateColor (const Graphic3d_Vec3& theVec)
{
  if (theVec.r() < 0.0f || theVec.r() > 1.0f
   || theVec.g() < 0.0f || theVec.g() > 1.0f
   || theVec.b() < 0.0f || theVec.b() > 1.0f)
  {
    Message::SendWarning (TCollection_AsciiString("Invalid color in OBJ material at line ") + myNbLines + " in file " + myPath);
    return false;
  }
  return true;
}
