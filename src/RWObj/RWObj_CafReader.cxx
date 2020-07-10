// Author: Kirill Gavrilov
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <RWObj_CafReader.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWObj_CafReader, RWMesh_CafReader)

//================================================================
// Function : Constructor
// Purpose  :
//================================================================
RWObj_CafReader::RWObj_CafReader()
: myIsSinglePrecision (Standard_False)
{
  //myCoordSysConverter.SetInputLengthUnit (-1.0); // length units are undefined within OBJ file
  // OBJ format does not define coordinate system (apart from mentioning that it is right-handed),
  // however most files are stored Y-up
  myCoordSysConverter.SetInputCoordinateSystem (RWMesh_CoordinateSystem_glTF);
}

//================================================================
// Function : BindNamedShape
// Purpose  :
//================================================================
void RWObj_CafReader::BindNamedShape (const TopoDS_Shape& theShape,
                                      const TCollection_AsciiString& theName,
                                      const RWObj_Material* theMaterial,
                                      const Standard_Boolean theIsRootShape)
{
  if (theShape.IsNull())
  {
    return;
  }

  RWMesh_NodeAttributes aShapeAttribs;
  aShapeAttribs.Name = theName;
  if (theMaterial != NULL)
  {
    // assign material and not color
    //aShapeAttribs.Style.SetColorSurf (Quantity_ColorRGBA (theMaterial->DiffuseColor, 1.0f - theMaterial->Transparency));

    Handle(XCAFDoc_VisMaterial) aMat = new XCAFDoc_VisMaterial();
    if (!myObjMaterialMap.Find (theMaterial->Name, aMat)) // material names are used as unique keys in OBJ
    {
      XCAFDoc_VisMaterialCommon aMatXde;
      aMatXde.IsDefined = true;
      aMatXde.AmbientColor    = theMaterial->AmbientColor;
      aMatXde.DiffuseColor    = theMaterial->DiffuseColor;
      aMatXde.SpecularColor   = theMaterial->SpecularColor;
      aMatXde.Shininess       = theMaterial->Shininess;
      aMatXde.Transparency    = theMaterial->Transparency;
      if (!theMaterial->DiffuseTexture.IsEmpty())
      {
        aMatXde.DiffuseTexture  = new Image_Texture (theMaterial->DiffuseTexture);
      }

      aMat = new XCAFDoc_VisMaterial();
      aMat->SetCommonMaterial (aMatXde);
      aMat->SetRawName (new TCollection_HAsciiString (theMaterial->Name));
      myObjMaterialMap.Bind (theMaterial->Name, aMat);
    }
    aShapeAttribs.Style.SetMaterial (aMat);
  }
  myAttribMap.Bind (theShape, aShapeAttribs);

  if (theIsRootShape)
  {
    myRootShapes.Append (theShape);
  }
}

//================================================================
// Function : createReaderContext
// Purpose  :
//================================================================
Handle(RWObj_TriangulationReader) RWObj_CafReader::createReaderContext()
{
  Handle(RWObj_TriangulationReader) aReader = new RWObj_TriangulationReader();
  return aReader;
}

//================================================================
// Function : performMesh
// Purpose  :
//================================================================
Standard_Boolean RWObj_CafReader::performMesh (const TCollection_AsciiString& theFile,
                                               const Message_ProgressRange& theProgress,
                                               const Standard_Boolean theToProbe)
{
  Handle(RWObj_TriangulationReader) aCtx = createReaderContext();
  aCtx->SetSinglePrecision (myIsSinglePrecision);
  aCtx->SetCreateShapes (Standard_True);
  aCtx->SetShapeReceiver (this);
  aCtx->SetTransformation (myCoordSysConverter);
  aCtx->SetMemoryLimit (myMemoryLimitMiB == -1 ? Standard_Size(-1) : Standard_Size(myMemoryLimitMiB * 1024 * 1024));
  Standard_Boolean isDone = Standard_False;
  if (theToProbe)
  {
    isDone = aCtx->Probe (theFile.ToCString(), theProgress);
  }
  else
  {
    isDone = aCtx->Read (theFile.ToCString(), theProgress);
  }
  if (!aCtx->FileComments().IsEmpty())
  {
    myMetadata.Add ("Comments", aCtx->FileComments());
  }
  for (NCollection_IndexedMap<TCollection_AsciiString>::Iterator aFileIter (aCtx->ExternalFiles()); aFileIter.More(); aFileIter.Next())
  {
    myExternalFiles.Add (aFileIter.Value());
  }
  return isDone;
}
