// Created on: 2014-12-25
// Created by: KULIKOVA Galina
// Copyright (c) 2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepAlgoAPI.ixx>

#include <stdio.h>
#include <TCollection_AsciiString.hxx>
#include <BRepTools.hxx>
#include <OSD_File.hxx>

//=======================================================================
//function : dumpOper
//purpose  : 
//=======================================================================
void BRepAlgoAPI::DumpOper(const Standard_CString theFilePath,
                           const TopoDS_Shape& theShape1,
                           const TopoDS_Shape& theShape2,
                           const TopoDS_Shape& theResult,
                           BOPAlgo_Operation theOperation,
                           Standard_Boolean isNonValidArgs)
{
  TCollection_AsciiString aPath(theFilePath);
  aPath += "/";
  Standard_Integer aNumOper = 1;
  Standard_Boolean isExist = Standard_True;
  TCollection_AsciiString aFileName;
 
  while(isExist)
  {
    aFileName = aPath + "BO_" + TCollection_AsciiString(aNumOper) +".tcl";
    OSD_File aScript(aFileName);
    isExist = aScript.Exists();
    if(isExist)
      aNumOper++;
  }

  FILE* afile = fopen(aFileName.ToCString(), "w+");
  if(!afile)
    return;
  if(isNonValidArgs)
    fprintf(afile,"%s\n","# Arguments are invalid");

  TCollection_AsciiString aName1;
  TCollection_AsciiString aName2;
  TCollection_AsciiString aNameRes;
  if(!theShape1.IsNull())
  {
    aName1 = aPath +
      "Arg1_" + TCollection_AsciiString(aNumOper) + ".brep";
    BRepTools::Write(theShape1, aName1.ToCString());
  }
  else
    fprintf(afile,"%s\n","# First argument is Null ");
   
  if(!theShape2.IsNull())
  {
    aName2 =  aPath +
      "Arg2_"+ TCollection_AsciiString(aNumOper) + ".brep";

    BRepTools::Write(theShape2, aName2.ToCString());
  }
  else
    fprintf(afile,"%s\n","# Second argument is Null ");
   
   if(!theResult.IsNull())
  {
    aNameRes =  aPath +
      "Result_"+ TCollection_AsciiString(aNumOper) + ".brep";

    BRepTools::Write(theResult, aNameRes.ToCString());
  }
  else
    fprintf(afile,"%s\n","# Result is Null ");
  
  fprintf(afile, "%s %s %s\n","restore",  aName1.ToCString(), "arg1");
  fprintf(afile, "%s %s %s\n","restore",  aName2.ToCString(), "arg2");;
  TCollection_AsciiString aBopString;
  switch (theOperation)
  {
    case BOPAlgo_COMMON : aBopString += "bcommon Res "; break;
    case BOPAlgo_FUSE   : aBopString += "bfuse Res "; break;
    case BOPAlgo_CUT    : 
    case BOPAlgo_CUT21  : aBopString += "bcut Res "; break;
    case BOPAlgo_SECTION : aBopString += "bsection Res "; break;
    default : break;
  };
  aBopString += ("arg1 arg2");
  if(theOperation == BOPAlgo_CUT21)
    aBopString += " 1";

  fprintf(afile, "%s\n",aBopString.ToCString());
  fclose(afile);
}
