// Created on: 2015-03-15
// Created by: Danila ULYANOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <ViewerTest_CmdParser.hxx>

#include <Draw.hxx>

#include <iostream>
#include <sstream>
#include <string>


//===============================================================================================
//function : ViewerTest_CmdParser
//purpose  :
//===============================================================================================
ViewerTest_CmdParser::ViewerTest_CmdParser()
{
  ViewerTest_CmdOption aDefaultOption;
  myArgumentStorage.push_back (aDefaultOption);
  myArgumentLists[""] = 0;
  myArgumentLists["help"] = 0;
}

//===============================================================================================
//function : AddOption
//purpose  :
//===============================================================================================
void ViewerTest_CmdParser::AddOption (const std::string& theOptionNames, const std::string& theOptionDescription)
{
  ViewerTest_CmdOption aNewOption;

  // extract option names
  std::vector<std::string> aNames;
  std::stringstream aStream (theOptionNames);
  std::string anItem;
  while (std::getline (aStream, anItem, '|'))
  {
    std::transform (anItem.begin(), anItem.end(), anItem.begin(), ::LowerCase);
    if (!anItem.empty())
    {
      aNames.push_back (anItem);
    }
  }

  aNewOption.Name        = aNames.front();
  aNewOption.Description = theOptionDescription;
  aNewOption.IsSet       = Standard_False;

  myArgumentStorage.push_back (aNewOption);

  std::vector<std::string>::const_iterator anIt = aNames.begin();
  for (; anIt != aNames.end(); ++anIt)
  {
    myArgumentLists[*anIt] = (Standard_Integer) myArgumentStorage.size() - 1;
  }
}

//===============================================================================================
//function : Help
//purpose  :
//===============================================================================================
void ViewerTest_CmdParser::Help()
{
  std::cout << myDescription << std::endl;

  std::vector<ViewerTest_CmdOption>::const_iterator anIt = myArgumentStorage.begin();
  for (++anIt; anIt != myArgumentStorage.end(); ++anIt)
  {
    std::cout << "\n    -" << (*anIt).Name << " : " << (*anIt).Description;
  }

  std::cout << std::endl;
}

//===============================================================================================
//function : Parse
//purpose  :
//===============================================================================================
void ViewerTest_CmdParser::Parse (Standard_Integer theArgsNb, const char** theArgVec)
{
  Standard_Integer aCurrentOption = 0;

  for (Standard_Integer anIter = 1; anIter < theArgsNb; ++anIter)
  {
    if (theArgVec[anIter][0] == '-' && !std::isdigit (theArgVec[anIter][1]))
    {
      std::string anOptionName (&theArgVec[anIter][1]);
      std::transform (anOptionName.begin(), anOptionName.end(), anOptionName.begin(), ::LowerCase);

      std::map<std::string, Standard_Integer>::iterator aMapIter = myArgumentLists.find (anOptionName);
      if (aMapIter != myArgumentLists.end())
      {
        aCurrentOption = aMapIter->second;
        myArgumentStorage[aCurrentOption].IsSet = true;
        myArgumentStorage[aCurrentOption].Arguments.clear();
      }
      else
      {
        std::cerr << "Error: unknown argument '" << theArgVec[anIter] << "'\n";
      }
    }
    else
    {
      myArgumentStorage[aCurrentOption].Arguments.push_back (theArgVec[anIter]);
    }
  }
}

//===============================================================================================
//function : HasOption
//purpose  :
//===============================================================================================
Standard_Boolean ViewerTest_CmdParser::HasOption (const std::string& theOptionName, Standard_Integer theMandatoryArgsNb /*= 0*/, Standard_Boolean isFatal /*= Standard_False*/)
{
  std::string aLowerName = theOptionName;
  std::transform (aLowerName.begin(), aLowerName.end(), aLowerName.begin(), ::LowerCase);

  Standard_Boolean aResult = Standard_False;
  std::map<std::string, Standard_Integer>::iterator aMapIter = myArgumentLists.find (aLowerName);
  if (aMapIter != myArgumentLists.end())
  {
    Standard_Integer anOption = aMapIter->second;
    aResult = myArgumentStorage[anOption].Arguments.size() >= static_cast<size_t> (theMandatoryArgsNb);
    if (isFatal && !aResult && myArgumentStorage[anOption].IsSet)
    {
      std::cerr << "Error: wrong syntax at argument '" << theOptionName << "'\n";
    }

    aResult &= myArgumentStorage[anOption].IsSet;
  }

  return aResult;
}

//===============================================================================================
//function : Arg
//purpose  :
//===============================================================================================
std::string ViewerTest_CmdParser::Arg (const std::string& theOptionName, Standard_Integer theArgumentIndex)
{
  std::string aLowerName = theOptionName;
  std::transform (aLowerName.begin(), aLowerName.end(), aLowerName.begin(), ::LowerCase);

  std::map<std::string, Standard_Integer>::iterator aMapIter = myArgumentLists.find (aLowerName);
  if (aMapIter != myArgumentLists.end())
  {
    Standard_Integer anOption = aMapIter->second;
    if (myArgumentStorage[anOption].Arguments.size() > static_cast<size_t> (theArgumentIndex))
    {
      return myArgumentStorage[anOption].Arguments[theArgumentIndex];
    }
    else
    {
      std::cerr << "Error: wrong syntax at argument '" << aLowerName << "'\n";
    }
  }

  return "";
}

//===============================================================================================
//function : ArgVec3f
//purpose  :
//===============================================================================================
Graphic3d_Vec3 ViewerTest_CmdParser::ArgVec3f (const std::string& theOptionName, Standard_Integer theArgumentIndex)
{
  return Graphic3d_Vec3 (static_cast<Standard_ShortReal> (Draw::Atof (Arg (theOptionName, theArgumentIndex    ).c_str())),
                         static_cast<Standard_ShortReal> (Draw::Atof (Arg (theOptionName, theArgumentIndex + 1).c_str())),
                         static_cast<Standard_ShortReal> (Draw::Atof (Arg (theOptionName, theArgumentIndex + 2).c_str())));
}

//===============================================================================================
//function : ArgVec3d
//purpose  :
//===============================================================================================
Graphic3d_Vec3d ViewerTest_CmdParser::ArgVec3d (const std::string& theOptionName, Standard_Integer theArgumentIndex)
{
  return Graphic3d_Vec3d ( Draw::Atof (Arg (theOptionName, theArgumentIndex    ).c_str()),
                           Draw::Atof (Arg (theOptionName, theArgumentIndex + 1).c_str()),
                           Draw::Atof (Arg (theOptionName, theArgumentIndex + 2).c_str()));
}

//===============================================================================================
//function : ArgVec
//purpose  :
//===============================================================================================
gp_Vec ViewerTest_CmdParser::ArgVec (const std::string& theOptionName, Standard_Integer theArgumentIndex)
{
  return gp_Vec ( Draw::Atof (Arg (theOptionName, theArgumentIndex    ).c_str()),
                  Draw::Atof (Arg (theOptionName, theArgumentIndex + 1).c_str()),
                  Draw::Atof (Arg (theOptionName, theArgumentIndex + 2).c_str()));
}

//===============================================================================================
//function : ArgPnt
//purpose  :
//===============================================================================================
gp_Pnt ViewerTest_CmdParser::ArgPnt (const std::string& theOptionName, Standard_Integer theArgumentIndex)
{
  return gp_Pnt ( Draw::Atof (Arg (theOptionName, theArgumentIndex    ).c_str()),
                  Draw::Atof (Arg (theOptionName, theArgumentIndex + 1).c_str()),
                  Draw::Atof (Arg (theOptionName, theArgumentIndex + 2).c_str()));
}

//===============================================================================================
//function : ArgDouble
//purpose  :
//===============================================================================================
Standard_Real ViewerTest_CmdParser::ArgDouble (const std::string& theOptionName, Standard_Integer theArgumentIndex)
{
  return Draw::Atof (Arg (theOptionName, theArgumentIndex).c_str());
}

//===============================================================================================
//function : ArgFloat
//purpose  :
//===============================================================================================
Standard_ShortReal ViewerTest_CmdParser::ArgFloat (const std::string& theOptionName, Standard_Integer theArgumentIndex)
{
  return static_cast<Standard_ShortReal> (Draw::Atof (Arg (theOptionName, theArgumentIndex).c_str()));
}

//===============================================================================================
//function : ArgInt
//purpose  :
//===============================================================================================
Standard_Integer ViewerTest_CmdParser::ArgInt (const std::string& theOptionName, const Standard_Integer theArgumentIndex)
{
  return static_cast<Standard_Integer> (Draw::Atoi (Arg (theOptionName, theArgumentIndex).c_str()));
}

//===============================================================================================
//function : ArgBool
//purpose  :
//===============================================================================================
Standard_Boolean ViewerTest_CmdParser::ArgBool (const std::string& theOptionName, const Standard_Integer theArgumentIndex)
{
  return Draw::Atoi (Arg (theOptionName, theArgumentIndex).c_str()) != 0;
}
