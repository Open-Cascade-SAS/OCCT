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

#ifndef _ViewerTest_CmdParser_HeaderFile
#define _ViewerTest_CmdParser_HeaderFile

#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include <Standard.hxx>
#include <Graphic3d_Vec.hxx>
#include <gp_Vec.hxx>

//! Command parser.
class ViewerTest_CmdParser
{
public:

  //! Initializes default option.
  ViewerTest_CmdParser();

  //! Sets description for command.
  void AddDescription (const std::string& theDescription)
  {
    myDescription = theDescription;
  }

  //! Adds option to available option list. Several names may be provided if separated with '|'.
  void AddOption (const std::string& theOptionNames, const std::string& theOptionDescription = "");

  //! Prints help message based on provided command and options descriptions.
  void Help();

  //! Parses argument list; assignes local arguments to each option.
  void Parse (Standard_Integer  theArgsNb,
              const char**      theArgVec);

  //! Checks if option was set with given minimal argument number.
  //! Prints error message if isFatal flag was set.
  Standard_Boolean HasOption (const std::string& theOptionName,
                              Standard_Integer theMandatoryArgsNb = 0,
                              Standard_Boolean isFatal = Standard_False);

  //! Accesses local argument of option 'theOptionName' with index 'theArgumentIndex'.
  std::string Arg (const std::string& theOptionName, Standard_Integer theArgumentIndex);

  // Interprets arguments of option 'theOptionName' as float vector starting with index 'theArgumentIndex'.
  Graphic3d_Vec3 ArgVec3f (const std::string& theOptionName, const Standard_Integer theArgumentIndex = 0);

  // Interprets arguments of option 'theOptionName' as double vector starting with index 'theArgumentIndex'.
  Graphic3d_Vec3d ArgVec3d (const std::string& theOptionName, const Standard_Integer theArgumentIndex = 0);

  // Interprets arguments of option 'theOptionName' as gp vector starting with index 'theArgumentIndex'.
  gp_Vec ArgVec (const std::string& theOptionName, const Standard_Integer theArgumentIndex = 0);

  // Interprets arguments of option 'theOptionName' as gp vector starting with index 'theArgumentIndex'.
  gp_Pnt ArgPnt (const std::string& theOptionName, const Standard_Integer theArgumentIndex = 0);

  // Interprets arguments of option 'theOptionName' as double at index 'theArgumentIndex'.
  Standard_Real ArgDouble (const std::string& theOptionName, const Standard_Integer theArgumentIndex = 0);

  // Interprets arguments of option 'theOptionName' as float at index 'theArgumentIndex'.
  Standard_ShortReal ArgFloat (const std::string& theOptionName, const Standard_Integer theArgumentIndex = 0);

  // Interprets arguments of option 'theOptionName' as integer at index 'theArgumentIndex'.
  Standard_Integer ArgInt (const std::string& theOptionName, const Standard_Integer theArgumentIndex = 0);

  // Interprets arguments of option 'theOptionName' as boolean at index 'theArgumentIndex'.
  Standard_Boolean ArgBool (const std::string& theOptionName, const Standard_Integer theArgumentIndex = 0);

private:

  //! Object representing option state.
  struct ViewerTest_CmdOption
  {
    ViewerTest_CmdOption() : IsSet (Standard_False) {}

    std::string Name;
    std::string Description;
    Standard_Boolean IsSet;
    std::vector<std::string> Arguments;
  };

  //! Description of command.
  std::string myDescription;

  //! Map from all possible option names to option object indexes in myArgumentStorage.
  std::map<std::string, Standard_Integer> myArgumentLists;

  //! Container which stores option objects.
  std::vector<ViewerTest_CmdOption> myArgumentStorage;
};

#endif // _ViewerTest_CmdParser_HeaderFile
