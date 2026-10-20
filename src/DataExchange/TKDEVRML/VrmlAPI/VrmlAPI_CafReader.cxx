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

#include <VrmlAPI_CafReader.hxx>

#include <DE_ValidationUtils.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <VrmlData_ImageTexture.hxx>
#include <VrmlData_Material.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopLoc_Location.hxx>
#include <XCAFDoc_VisMaterial.hxx>
#include <XCAFDoc_VisMaterialCommon.hxx>
#include <OSD_Path.hxx>
#include <Message.hxx>
#include <TopoDS_Iterator.hxx>
#include <VrmlData_Scene.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VrmlAPI_CafReader, Standard_Transient)

namespace
{
//=================================================================================================

static TCollection_AsciiString getVrmlErrorName(VrmlData_ErrorStatus theStatus)
{
  switch (theStatus)
  {
    case VrmlData_StatusOK:
      return "";
    case VrmlData_EmptyData:
      return "EmptyData";
    case VrmlData_UnrecoverableError:
      return "UnrecoverableError";
    case VrmlData_GeneralError:
      return "GeneralError";
    case VrmlData_EndOfFile:
      return "EndOfFile";
    case VrmlData_NotVrmlFile:
      return "NotVrmlFile";
    case VrmlData_CannotOpenFile:
      return "CannotOpenFile";
    case VrmlData_VrmlFormatError:
      return "VrmlFormatError";
    case VrmlData_NumericInputError:
      return "NumericInputError";
    case VrmlData_IrrelevantNumber:
      return "IrrelevantNumber";
    case VrmlData_BooleanInputError:
      return "BooleanInputError";
    case VrmlData_StringInputError:
      return "StringInputError";
    case VrmlData_NodeNameUnknown:
      return "NodeNameUnknown";
    case VrmlData_NonPositiveSize:
      return "NonPositiveSize";
    case VrmlData_ReadUnknownNode:
      return "ReadUnknownNode";
    case VrmlData_NonSupportedFeature:
      return "NonSupportedFeature";
    case VrmlData_OutputStreamUndefined:
      return "OutputStreamUndefined";
    case VrmlData_NotImplemented:
      return "NotImplemented";
  }
  return "UNKNOWN";
}

//=================================================================================================

static void performMeshSubshape(
  NCollection_DataMap<TopoDS_Shape, RWMesh_NodeAttributes, TopTools_ShapeMapHasher>& theAttribMap,
  const NCollection_DataMap<occ::handle<TopoDS_TShape>, occ::handle<VrmlData_Appearance>>&
                      theShapeAppMap,
  const TopoDS_Shape& theShape,
  NCollection_FlatDataMap<occ::handle<VrmlData_Appearance>, occ::handle<XCAFDoc_VisMaterial>>&
    theMaterials)
{
  occ::handle<VrmlData_Appearance> anAppearance;
  if (theShapeAppMap.Find(theShape.TShape(), anAppearance) && !anAppearance.IsNull())
  {
    occ::handle<XCAFDoc_VisMaterial>& aMaterial = theMaterials.TryEmplaced(anAppearance);
    if (aMaterial.IsNull())
    {
      XCAFDoc_VisMaterialCommon aCommon;
      aCommon.IsDefined                             = true;
      const occ::handle<VrmlData_Material>& aSource = anAppearance->Material();
      if (!aSource.IsNull())
      {
        aCommon.AmbientColor  = Quantity_Color(aSource->DiffuseColor().Rgb()
                                              * static_cast<float>(aSource->AmbientIntensity()));
        aCommon.DiffuseColor  = aSource->DiffuseColor();
        aCommon.SpecularColor = aSource->SpecularColor();
        aCommon.EmissiveColor = aSource->EmissiveColor();
        aCommon.Shininess     = static_cast<float>(aSource->Shininess());
        aCommon.Transparency  = static_cast<float>(aSource->Transparency());
      }
      const occ::handle<VrmlData_ImageTexture> aTexture =
        occ::down_cast<VrmlData_ImageTexture>(anAppearance->Texture());
      if (!aTexture.IsNull())
      {
        aCommon.DiffuseTexture = aTexture->Image();
      }
      aMaterial = new XCAFDoc_VisMaterial();
      aMaterial->SetCommonMaterial(aCommon);
      if (!aSource.IsNull() && aSource->Name() != nullptr && *aSource->Name() != '\0')
      {
        aMaterial->SetRawName(new TCollection_HAsciiString(aSource->Name()));
      }
    }
    RWMesh_NodeAttributes& aFaceAttribs = theAttribMap.TryEmplaced(theShape);
    aFaceAttribs.Style.SetMaterial(aMaterial);
    if (!anAppearance->Material().IsNull())
    {
      aFaceAttribs.Style.SetColorSurf(anAppearance->Material()->DiffuseColor());
    }
  }

  for (TopoDS_Iterator aSubShapeIter(theShape, true, false); aSubShapeIter.More();
       aSubShapeIter.Next())
  {
    performMeshSubshape(theAttribMap, theShapeAppMap, aSubShapeIter.Value(), theMaterials);
  }
}
} // namespace

//=================================================================================================

bool VrmlAPI_CafReader::performMesh(std::istream&                  theStream,
                                    const TCollection_AsciiString& theFile,
                                    const Message_ProgressRange&   theProgress,
                                    const bool                     theToProbe)
{
  (void)theProgress;
  if (!theStream.good())
  {
    Message::SendFail() << "Error in VrmlAPI_CafReader: file '" << theFile << "' is not found";
    return false;
  }
  if (theToProbe)
  {
    Message::SendFail() << "Error in VrmlAPI_CafReader: theToProbe parameter isn't supported";
    return false; // unsupported
  }

  // determine file location to load associated files
  TCollection_AsciiString aFolder;
  TCollection_AsciiString aFileName;
  OSD_Path::FolderAndFileFromPath(theFile, aFolder, aFileName);

  TCollection_AsciiString aResourceName = theFile;
  if (myResourceStreams != nullptr && !theFile.IsEmpty()
      && DE_ValidationUtils::FindReadStream(*myResourceStreams, "", theFile, aResourceName)
           != &theStream)
  {
    Message::SendFail() << "Error in VrmlAPI_CafReader: invalid admitted root '" << theFile << "'";
    return false;
  }
  VrmlData_Scene aScene;
  aScene.SetLinearScale(CoordinateSystemConverter().LengthUnitFactor());
  aScene.SetResourceStreams(myResourceStreams, aResourceName);
  aScene.SetVrmlDir(aFolder);
  aScene << theStream;

  NCollection_DataMap<occ::handle<TopoDS_TShape>, occ::handle<VrmlData_Appearance>> aShapeAppMap;
  TopoDS_Shape                                                                      aShape;
  if (aScene.Status() == VrmlData_StatusOK)
  {
    aShape = aScene.GetShape(aShapeAppMap);
  }
  if (aScene.Status() != VrmlData_StatusOK || aShape.IsNull())
  {
    Message::SendFail() << "Error in VrmlAPI_CafReader: "
                        << getVrmlErrorName(aShape.IsNull() && aScene.Status() == VrmlData_StatusOK
                                              ? VrmlData_EmptyData
                                              : aScene.Status())
                        << " occurred at line " << aScene.GetLineError()
                        << "\nwhile reading VRML file '" << theFile << "'";
    return false;
  }

  aShape.Move(TopLoc_Location(CoordinateSystemConverter().CoordinateTransformation()));
  NCollection_FlatDataMap<occ::handle<VrmlData_Appearance>, occ::handle<XCAFDoc_VisMaterial>>
    aMaterials;
  performMeshSubshape(myAttribMap, aShapeAppMap, aShape, aMaterials);
  myRootShapes.Append(aShape);
  return true;
}
