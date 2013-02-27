// Created on: 2012-12-25
// Created by: KULIKOVA Galina
// Copyright (c) 2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
