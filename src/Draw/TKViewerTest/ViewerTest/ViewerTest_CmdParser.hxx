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

#include <Graphic3d_Vec3.hxx>

#include <map>
#include <vector>
#include <set>
#include <string>

class gp_Vec;
class gp_Pnt;

//! A key for a command line option used for a ViewerTest_CmdParser work
typedef std::size_t ViewerTest_CommandOptionKey;

//! A set of keys for command-line options
typedef std::set<ViewerTest_CommandOptionKey> ViewerTest_CommandOptionKeySet;

//! Command parser.
class ViewerTest_CmdParser
{
public:
  //! The key of the unnamed command option
  static const std::size_t THE_UNNAMED_COMMAND_OPTION_KEY;

  //! The key of the help command option
  static const std::size_t THE_HELP_COMMAND_OPTION_KEY;

  //! Initializes help option.
  //! @param theDescription the description of the command
  ViewerTest_CmdParser(const std::string& theDescription = std::string());

  //! Sets description for command.
  void SetDescription(const std::string& theDescription) { myDescription = theDescription; }

  //! Adds option to available option list. Several names may be provided if separated with '|'.
  //! @param theOptionNames the list of possible option names separated with '|'
  //! (the first name is main, the other names are aliases)
  //! @param theOptionDescription the description of the option
  //! @return an access key of the newly added option
  ViewerTest_CommandOptionKey AddOption(const std::string& theOptionNames,
                                        const std::string& theOptionDescription = std::string());

  //! Prints help message based on provided command and options descriptions.
  void PrintHelp() const;

  //! Parses argument list (skips the command name); assigns local arguments to each option.
  void Parse(Standard_Integer theArgsNb, const char* const* theArgVec);

  //! Gets an option name by its access key
  //! @param theOptionKey the access key of the option which name is to be found
  //! @retuan a name of the option with the given access key
  std::string GetOptionNameByKey(ViewerTest_CommandOptionKey theOptionKey) const;

  //! Gets a set of used options
  //! @return a set of used options
  ViewerTest_CommandOptionKeySet GetUsedOptions() const;

  //! Tests if there were no command line options provided
  //! @return true if no command line options were provided, or false otherwise
  bool HasNoOption() const;

  //! Tests if the unnamed command line option was provided
  //! @return true if the unnamed command line option was provided, or false otherwise
  bool HasUnnamedOption() const;

  //! Tests if only unnamed command line option was provided
  //! @return true if only unnamed command line option was provided, or false otherwise
  bool HasOnlyUnnamedOption() const;

  //! Checks if option was used with given minimal number of arguments.
  //! Prints error message if isFatal flag was set.
  //! @param theOptionName the name of the option to be checked
  //! @param theMandatoryArgsNb the number of mandatory arguments
  //! @param isFatal the flag that controls printing of an error message
  //! @return true if an option was set, or false otherwise
  bool HasOption(const std::string& theOptionName,
                 std::size_t        theMandatoryArgsNb = 0,
                 bool               isFatal            = Standard_False) const;

  //! Checks if option was used with given minimal number of arguments.
  //! Prints error message if isFatal flag was set.
  //! @param theOptionKey the access key of the option to be checked
  //! @param theMandatoryArgsNb the number of mandatory arguments
  //! @param isFatal the flag that controls printing of an error message
  //! @return true if an option was set, or false otherwise
  bool HasOption(ViewerTest_CommandOptionKey theOptionKey,
                 std::size_t                 theMandatoryArgsNb = 0,
                 bool                        isFatal            = Standard_False) const;

  //! Gets a number of option arguments
  //! @param theOptionName the name of the option
  //! @return a number of option arguments, or 0 if option was not used
  Standard_Integer GetNumberOfOptionArguments(const std::string& theOptionName) const;

  //! Gets a number of option arguments
  //! @param theOptionKey the access key of the option
  //! @return a number of option arguments, or 0 if option was not used
  Standard_Integer GetNumberOfOptionArguments(ViewerTest_CommandOptionKey theOptionKey) const;

  //! Accesses local argument of option 'theOptionName' with index 'theArgumentIndex'.
  //! @param theOptionName the name of the option which argument is to be accessed
  //! @param theArgumentIndex the index of an accessed argument
  //! @param theOptionArgument an argument of the option with the given name
  //! @return true if an access was successful, or false otherwise
  bool Arg(const std::string& theOptionName,
           Standard_Integer   theArgumentIndex,
           std::string&       theOptionArgument) const;

  //! Accesses a local argument with the index 'theArgumentIndex' of the option with the key
  //! 'theOptionKey'.
  //! @param theOptionKey the access key of the option which argument is to be accessed
  //! @param theArgumentIndex the index of an accessed argument
  //! @param theOptionArgument an argument of the option with the given key
  //! @return true if an access was successful, or false otherwise
  bool Arg(ViewerTest_CommandOptionKey theOptionKey,
           Standard_Integer            theArgumentIndex,
           std::string&                theOptionArgument) const;

  //! Accesses local argument of option 'theOptionName' with index 'theArgumentIndex'.
  //! @param theOptionName the name of the option which argument is to be accessed
  //! @param theArgumentIndex the index of an accessed argument
  //! @return an argument of the option with the given name
  std::string Arg(const std::string& theOptionName, Standard_Integer theArgumentIndex) const;

  //! Accesses a local argument with the index 'theArgumentIndex' of the option with the key
  //! 'theOptionKey'.
  //! @param theOptionKey the access key of the option which argument is to be accessed
  //! @param theArgumentIndex the index of an accessed argument
  //! @return an argument of the option with the given key
  std::string Arg(ViewerTest_CommandOptionKey theOptionKey,
                  Standard_Integer            theArgumentIndex) const;

  // Interprets arguments of option 'theOptionName' as float vector starting with index
  // 'theArgumentIndex'.
  Graphic3d_Vec3 ArgVec3f(const std::string&     theOptionName,
                          const Standard_Integer theArgumentIndex = 0) const;

  // Interprets arguments of option 'theOptionName' as double vector starting with index
  // 'theArgumentIndex'.
  Graphic3d_Vec3d ArgVec3d(const std::string&     theOptionName,
                           const Standard_Integer theArgumentIndex = 0) const;

  // Interprets arguments of option 'theOptionName' as gp vector starting with index
  // 'theArgumentIndex'.
  gp_Vec ArgVec(const std::string&     theOptionName,
                const Standard_Integer theArgumentIndex = 0) const;

  // Interprets arguments of option 'theOptionName' as gp vector starting with index
  // 'theArgumentIndex'.
  gp_Pnt ArgPnt(const std::string&     theOptionName,
                const Standard_Integer theArgumentIndex = 0) const;

  // Interprets arguments of option 'theOptionName' as double at index 'theArgumentIndex'.
  Standard_Real ArgDouble(const std::string&     theOptionName,
                          const Standard_Integer theArgumentIndex = 0) const;

  // Interprets arguments of option 'theOptionName' as float at index 'theArgumentIndex'.
  Standard_ShortReal ArgFloat(const std::string&     theOptionName,
                              const Standard_Integer theArgumentIndex = 0) const;

  // Interprets arguments of option 'theOptionName' as integer at index 'theArgumentIndex'.
  Standard_Integer ArgInt(const std::string&     theOptionName,
                          const Standard_Integer theArgumentIndex = 0) const;

  // Interprets arguments of option 'theOptionName' as boolean at index 'theArgumentIndex'.
  bool ArgBool(const std::string& theOptionName, const Standard_Integer theArgumentIndex = 0) const;

  //! Interprets arguments of the option 'theOptionName' with the index 'theArgumentIndex' as an
  //! RGB(A) color object.
  //! @tparam theColor the type of a resulting RGB(A) color object
  //! @param theOptionName the name of the option which arguments are to be interpreted
  //! @param theArgumentIndex the index of the first argument to be interpreted
  //! (will be promoted to the next argument after the block of interpreted arguments)
  //! @param theColor a color that is an interpretation of argument(s) of the option with the given
  //! name
  //! @return true if an interpretation was successful, or false otherwise
  template <typename TheColor>
  bool ArgColor(const std::string& theOptionName,
                Standard_Integer&  theArgumentIndex,
                TheColor&          theColor) const;

  //! Interprets arguments of the option with the key 'theOptionKey' as an RGB(A) color object.
  //! @tparam theColor the type of a resulting RGB(A) color object
  //! @param theOptionKey the access key of the option which arguments are to be interpreted
  //! @param theArgumentIndex the index of the first argument to be interpreted
  //! (will be promoted to the next argument after the block of interpreted arguments)
  //! @param theColor a color that is an interpretation of argument(s) of the option with the given
  //! name
  //! @return true if an interpretation was successful, or false otherwise
  template <typename TheColor>
  bool ArgColor(ViewerTest_CommandOptionKey theOptionKey,
                Standard_Integer&           theArgumentIndex,
                TheColor&                   theColor) const;

private:
  //! A list of aliases to a command option name
  typedef std::vector<std::string> OptionAliases;

  //! A map from all possible option names to option access keys
  typedef std::map<std::string, ViewerTest_CommandOptionKey> OptionMap;

  //! A map from keys of used options to their indices in the storage
  typedef std::map<ViewerTest_CommandOptionKey, std::size_t> UsedOptionMap;

  //! A list of command option arguments
  typedef std::vector<std::string> OptionArguments;

  //! A storage of arguments of different command options
  typedef std::vector<OptionArguments> OptionArgumentsStorage;

  //! A full description of a command option
  struct CommandOption
  {
    std::string   Name;        //!< A command option name
    OptionAliases Aliases;     //!< A list of aliases to a command option name
    std::string   Description; //!< A text description of a command option
  };

  // A storage of command options descriptions
  typedef std::vector<CommandOption> CommandOptionStorage;

  // A list of raw string arguments
  typedef std::vector<const char*> RawStringArguments;

  //! Description of command.
  std::string myDescription;

  //! Container which stores option objects.
  std::vector<CommandOption> myOptionStorage;

  //! Map from all possible option names to option access keys (that are indices in myOptionStorage)
  OptionMap myOptionMap;

  //! Map from keys of used options to their indices in the option arguments storage
  UsedOptionMap myUsedOptionMap;

  //! Container which stores the arguments of all used options
  OptionArgumentsStorage myOptionArgumentStorage;

  //! Gets an access key of the option
  //! @param theOptionName the name of the option which key is to be found
  //! @param theOptionKey an access key of the option with the given name
  //! @return true if the given option was found, or false otherwise
  bool findOptionKey(const std::string&           theOptionName,
                     ViewerTest_CommandOptionKey& theOptionKey) const;

  //! Gets an index of an option that was used
  //! @param theOptionKey the access key of the used option which index is to be found
  //! @param theUsedOptionIndex an index of the used option with the given access key
  //! @return true if the given option was not found or not used, or false otherwise
  bool findUsedOptionIndex(ViewerTest_CommandOptionKey theOptionKey,
                           std::size_t&                theUsedOptionIndex) const;

  //! Gets an index of an option that was used
  //! @param theOptionName the name of the used option which index is to be found
  //! @param theUsedOptionIndex an index of the used option with the given name
  //! @return true if the given option was not found or not used, or false otherwise
  bool findUsedOptionIndex(const std::string& theOptionName, std::size_t& theUsedOptionIndex) const;

  //! Adds the option that is used in the passed command line parameters
  //! @param theNewUsedOptionKey the access key of the adding option
  //! @return an index of a newly added option
  std::size_t addUsedOption(ViewerTest_CommandOptionKey theNewUsedOptionKey);

  //! Gets an index of an option that was used
  //! @param theOptionName the name of the used option which index is to be found
  //! @param theUsedOptionIndex an index of the used option with the given name
  //! @return true if the given option was not found or not used, or false otherwise
  RawStringArguments getRawStringArguments(std::size_t theUsedOptionIndex) const;
};

#endif // _ViewerTest_CmdParser_HeaderFile
