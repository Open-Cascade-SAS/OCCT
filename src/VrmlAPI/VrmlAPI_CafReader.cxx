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

#include <OSD_FileSystem.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_Path.hxx>
#include <Message.hxx>
#include <Standard_CLocaleSentry.hxx>
#include <TopoDS_Iterator.hxx>
#include <VrmlData_Scene.hxx>

#include <memory>

IMPLEMENT_STANDARD_RTTIEXT(VrmlAPI_CafReader, Standard_Transient)

namespace
{
  //=======================================================================
  // function : getVrmlErrorName
  // purpose  :
  //=======================================================================
  static TCollection_AsciiString getVrmlErrorName(VrmlData_ErrorStatus theStatus)
  {
    switch (theStatus)
    {
      case VrmlData_StatusOK:              return "";
      case VrmlData_EmptyData:             return "EmptyData";
      case VrmlData_UnrecoverableError:    return "UnrecoverableError";
      case VrmlData_GeneralError:          return "GeneralError";
      case VrmlData_EndOfFile:             return "EndOfFile";
      case VrmlData_NotVrmlFile:           return "NotVrmlFile";
      case VrmlData_CannotOpenFile:        return "CannotOpenFile";
      case VrmlData_VrmlFormatError:       return "VrmlFormatError";
      case VrmlData_NumericInputError:     return "NumericInputError";
      case VrmlData_IrrelevantNumber:      return "IrrelevantNumber";
      case VrmlData_BooleanInputError:     return "BooleanInputError";
      case VrmlData_StringInputError:      return "StringInputError";
      case VrmlData_NodeNameUnknown:       return "NodeNameUnknown";
      case VrmlData_NonPositiveSize:       return "NonPositiveSize";
      case VrmlData_ReadUnknownNode:       return "ReadUnknownNode";
      case VrmlData_NonSupportedFeature:   return "NonSupportedFeature";
      case VrmlData_OutputStreamUndefined: return "OutputStreamUndefined";
      case VrmlData_NotImplemented:        return "NotImplemented";
    }
    return "UNKNOWN";
  }

  //=======================================================================
  // function : performMeshSubshape
  // purpose  :
  //=======================================================================
  static void performMeshSubshape(RWMesh_NodeAttributeMap& theAttribMap,
                                  const VrmlData_DataMapOfShapeAppearance& theShapeAppMap,
                                  const TopoDS_Shape& theShape)
  {
    Handle(VrmlData_Appearance) anAppearance;
    if (theShapeAppMap.Find(theShape.TShape(), anAppearance))
    {
      if (!anAppearance.IsNull()
          && !anAppearance->Material().IsNull())
      {
        RWMesh_NodeAttributes aFaceAttribs;
        theAttribMap.Find(theShape, aFaceAttribs);
        aFaceAttribs.Style.SetColorSurf(anAppearance->Material()->DiffuseColor());
        theAttribMap.Bind(theShape, aFaceAttribs);
      }
    }

    for (TopoDS_Iterator aSubShapeIter(theShape, true, false); aSubShapeIter.More(); aSubShapeIter.Next())
    {
      performMeshSubshape(theAttribMap, theShapeAppMap, aSubShapeIter.Value());
    }
  }
}

//=======================================================================
// function : performMesh
// purpose  :
//=======================================================================
bool VrmlAPI_CafReader::performMesh(std::istream& theStream,
                                    const TCollection_AsciiString& theFile,
                                    const Message_ProgressRange& theProgress,
                                    const Standard_Boolean theToProbe)
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

  VrmlData_Scene aScene;
  aScene.SetLinearScale(FileLengthUnit());
  aScene.SetVrmlDir(aFolder);
  aScene << theStream;

  VrmlData_DataMapOfShapeAppearance aShapeAppMap;
  TopoDS_Shape aShape = aScene.GetShape(aShapeAppMap);
  if (!aShape.IsNull())
  {
    performMeshSubshape(myAttribMap, aShapeAppMap, aShape);
    myRootShapes.Append(aShape);
  }
  if (aScene.Status() != VrmlData_StatusOK
      || aShape.IsNull())
  {
    Message::SendFail() << "Error in VrmlAPI_CafReader: " << getVrmlErrorName(aScene.Status())
                        << "occurred at line " << aScene.GetLineError()
                        << "\nwhile reading VRML file '" << theFile << "'";
    return false;
  }

  return true;
}
