// Created on: 2017-03-28
// Created by: Sergey NIKONOV
// Copyright (c) 2000-2017 OPEN CASCADE SAS
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

#include <DDocStd.hxx>
#include <DBRep.hxx>
#include <FSD_File.hxx>
#include <FSD_CmpFile.hxx>
#include <FSD_BinaryFile.hxx>
#include <BRep_Builder.hxx>
#include <NCollection_Handle.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <Storage_BaseDriver.hxx>
#include <StdStorage.hxx>
#include <StdStorage_Data.hxx>
#include <StdStorage_HeaderData.hxx>
#include <StdStorage_RootData.hxx>
#include <StdStorage_TypeData.hxx>
#include <ShapePersistent_TopoDS.hxx>

//=======================================================================
//function : DDocStd_ShapeSchema_Write 
//=======================================================================

static Standard_Integer DDocStd_fsdwrite(Draw_Interpretor& theDI,
                                         Standard_Integer theArgNb,
                                         const char** theArgs)
{
  if (theArgNb < 3)
  {
    theDI << "Usage : fsdwrite shapes filename [gen | cmp | bin]\n";
    theDI << "        Arguments:\n";
    theDI << "        shapes   : list os shape names\n";
    theDI << "        filename : output file name\n";
    theDI << "        Storage driver:\n";
    theDI << "          gen : FSD_File driver (default)\n";
    theDI << "          cmp : FSD_CmpFile driver\n";
    theDI << "          bin : FSD_BinaryFile driver\n";
    return 1;
  }

  NCollection_Handle<Storage_BaseDriver> aFileDriver(new FSD_File);

  Standard_Boolean hasStorageDriver = Standard_False;
  Standard_Integer iArgN = theArgNb - 1;

  if (strncmp(theArgs[iArgN], "gen", 3) == 0)
  {
    aFileDriver = new FSD_File;
    hasStorageDriver = Standard_True;
  }
  else if (strncmp(theArgs[iArgN], "cmp", 3) == 0)
  {
    aFileDriver = new FSD_CmpFile;
    hasStorageDriver = Standard_True;
  }
  else if (strncmp(theArgs[iArgN], "bin", 3) == 0)
  {
    aFileDriver = new FSD_BinaryFile;
    hasStorageDriver = Standard_True;
  }

  if (hasStorageDriver) --iArgN;

  Storage_Error aStatus = aFileDriver->Open(theArgs[iArgN], Storage_VSWrite);
  if (aStatus != Storage_VSOk) {
    theDI << "Error : couldn't open file '" << "' for writing (" << aStatus << ")\n";
    return 1;
  }

  TopTools_SequenceOfShape aShapes;
  NCollection_DataMap<TCollection_AsciiString, Standard_Integer> aShapeNames;
  for (Standard_Integer i = 1; i < iArgN; ++i)
  {
    TopoDS_Shape aShape = DBRep::Get(theArgs[i]);
    if (aShape.IsNull())
    {
      theDI << "Error : null shape " << theArgs[i] << "\n";
      return 1;
    }
    aShapes.Append(aShape);
    if (aShapeNames.IsBound(theArgs[i]))
      aShapeNames.ChangeFind(theArgs[i]) += 1;
    else
      aShapeNames.Bind(theArgs[i], 1);
  }

  Handle(StdStorage_Data) aData = new StdStorage_Data;

  aData->HeaderData()->SetApplicationName(TCollection_ExtendedString("DDocStd_ShapeSchema_Write"));

  StdObjMgt_TransientPersistentMap aMap;
  for (Standard_Integer i = 1; i <= aShapes.Length(); ++i)
  {
    TopoDS_Shape aShape = aShapes.Value(i);

    Handle(ShapePersistent_TopoDS::HShape) aPShape =
      ShapePersistent_TopoDS::Translate(aShape, aMap, ShapePersistent_WithTriangle);
    if (aPShape.IsNull())
    {
      theDI << "Error : couldn't translate shape " << theArgs[i] << "\n";
      return 1;
    }
    
    TCollection_AsciiString aName = theArgs[i];
    if (aShapeNames.IsBound(aName))
    {
      Standard_Integer n = aShapeNames.Find(theArgs[i]);
      if (n > 1)
      {
        aName += "_";
        aName += n;
      }
    }

    Handle(StdStorage_Root) aRoot = new StdStorage_Root(aName, aPShape);
    aData->RootData()->AddRoot(aRoot);
  }

  Storage_Error anError = StdStorage::Write(*aFileDriver, aData);

  aFileDriver->Close();

  if (anError != Storage_VSOk)
  {
    theDI << "Error : " << anError << "\n";
    return 1;
  }

  return 0;
}

//=======================================================================
//function : DDocStd_ShapeSchema_Read 
//=======================================================================

static Standard_Integer DDocStd_fsdread(Draw_Interpretor& theDI, 
                                        Standard_Integer theArgNb, 
                                        const char** theArgs)
{
  if (theArgNb < 3)
  {
    theDI << "Usage : fsdread filename shape\n";
    theDI << "        Arguments:\n";
    theDI << "        filename : input file name\n";
    theDI << "        shape    : name of an output shape,\n";
    theDI << "                   root shapes will be put into a compound\n";
    theDI << "                   in case of multiple roots in the file\n";
    return 1;
  }

  Handle(StdStorage_Data) aData;
  Storage_Error anError = StdStorage::Read(TCollection_AsciiString(theArgs[1]), aData);
  if (anError != Storage_VSOk)
  {
    theDI << "Error : " << anError << "\n";
    return 1;
  }

  TopTools_SequenceOfShape aShapes;

  Handle(StdStorage_TypeData) aTypeData = aData->TypeData();
  Handle(StdStorage_RootData) aRootData = aData->RootData();
  Handle(StdStorage_HSequenceOfRoots) aRoots = aRootData->Roots();
  if (!aRoots.IsNull())
  {
    for (StdStorage_HSequenceOfRoots::Iterator anIt(*aRoots); anIt.More(); anIt.Next())
    {
      Handle(StdStorage_Root)& aRoot = anIt.ChangeValue();
      Handle(StdObjMgt_Persistent) aPObject = aRoot->Object();
      if (!aPObject.IsNull())
      {
        Handle(ShapePersistent_TopoDS::HShape) aHShape = Handle(ShapePersistent_TopoDS::HShape)::DownCast(aPObject);
        if (aHShape) // shapes are expected
        {
          TopoDS_Shape aShape = aHShape->Import();
          aShapes.Append(aShape);
        }
      }
    }
  }

  theDI << "Info : " << aTypeData->NumberOfTypes() << " type(s)\n";
  theDI << "       " << aRoots->Length() << " root(s)\n";
  theDI << "       " << aShapes.Length() << " shape(s) translated\n";

  if (aShapes.Length() > 1)
  {
    BRep_Builder aB;
    TopoDS_Compound aC;
    aB.MakeCompound(aC);
    for (Standard_Integer i = 1; i <= aShapes.Length(); ++i)
      aB.Add(aC, aShapes.Value(i));
    DBRep::Set(theArgs[2], aC);
  }
  else
    DBRep::Set(theArgs[2], aShapes.First());

  return 0;
}

//=======================================================================
//function : ShapeSchemaCommands
//purpose  : registers shape schema related commands in Draw interpreter
//=======================================================================

void DDocStd::ShapeSchemaCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  const char* g = "Shape persistence commands";

  theCommands.Add("fsdwrite",
    "fsdrite shape filename [driver]",
    __FILE__, DDocStd_fsdwrite, g);

  theCommands.Add("fsdread",
    "fsdread filename shape",
    __FILE__, DDocStd_fsdread, g);

}
