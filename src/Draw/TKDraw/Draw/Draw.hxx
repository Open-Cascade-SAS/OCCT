// Created on: 1991-04-24
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Draw_HeaderFile
#define _Draw_HeaderFile

#include <Draw_Interpretor.hxx>
#include <NCollection_Map.hxx>
#include <Quantity_ColorRGBA.hxx>
#include <Standard_Handle.hxx>

class Draw_Drawable3D;
class Draw_ProgressIndicator;

//! MAQUETTE DESSIN MODELISATION
class Draw
{
public:
  DEFINE_STANDARD_ALLOC

  //! (Re)Load a Draw Harness plugin.
  //! @param[in][out] theDI    Tcl interpreter to append loaded commands
  //! @param[in] theKey  plugin code name to be resolved in resource file
  //! @param[in] theResourceFileName    description file name
  //! @param[in] theDefaultsDirectory   default folder for looking description file
  //! @param[in] theUserDefaultsDirectory  user folder for looking description file
  //! @param[in] theIsVerbose  print verbose messages
  Standard_EXPORT static void Load(Draw_Interpretor&              theDI,
                                   const TCollection_AsciiString& theKey,
                                   const TCollection_AsciiString& theResourceFileName,
                                   const TCollection_AsciiString& theDefaultsDirectory,
                                   const TCollection_AsciiString& theUserDefaultsDirectory,
                                   const bool         theIsVerbose = false);

public: //! @name Tcl variables management tools
  //! Sets a variable. Display it if <Disp> is true.
  Standard_EXPORT static void Set(const char*         Name,
                                  const occ::handle<Draw_Drawable3D>& D,
                                  const bool         Disp);

  //! Sets a variable, a null handle clear the
  //! vartiable. Automatic display is context driven.
  Standard_EXPORT static void Set(const char* Name, const occ::handle<Draw_Drawable3D>& D);

  //! Sets a numeric variable.
  Standard_EXPORT static void Set(const char* Name, const double val);

  //! Returns main DRAW interpreter.
  Standard_EXPORT static Draw_Interpretor& GetInterpretor();

  //! Returns a variable value.
  //! The name "." does a graphic selection; in this case theName will be is overwritten with the
  //! name of the variable.
  static occ::handle<Draw_Drawable3D> Get(const char*& theName)
  {
    return getDrawable(theName, true);
  }

  //! Returns a variable value.
  static occ::handle<Draw_Drawable3D> GetExisting(const char*& theName)
  {
    const char* aName = theName;
    return getDrawable(aName, false);
  }

  //! Gets a numeric variable. Returns True if the
  //! variable exist.
  Standard_EXPORT static bool Get(const char* Name, double& val);

  //! Sets a TCL string variable
  Standard_EXPORT static void Set(const char* Name, const char* val);

  //! Returns a map of Draw_Drawable3D variables.
  Standard_EXPORT static const NCollection_Map<occ::handle<Draw_Drawable3D>>& Drawables();

public: //! @name argument parsing tools
  //! Converts numeric expression, that can involve DRAW
  //! variables, to real value.
  Standard_EXPORT static double Atof(const char* Name);

  //! Converts the numeric expression, that can involve DRAW variables, to a real value
  //! @param theExpressionString the strings that contains the expression involving DRAW variables
  //! to be parsed
  //! @param theParsedRealValue a real value that is a result of parsing
  //! @return true if parsing was successful, or false otherwise
  Standard_EXPORT static bool ParseReal(const char* theExpressionString,
                                        double&         theParsedRealValue);

  //! Converts numeric expression, that can involve DRAW
  //! variables, to integer value.
  //! Implemented as cast of Atof() to integer.
  Standard_EXPORT static int Atoi(const char* Name);

  //! Converts the numeric expression, that can involve DRAW variables, to an integer value
  //! @param theExpressionString the strings that contains the expression involving DRAW variables
  //! to be parsed
  //! @param theParsedIntegerValue an integer value that is a result of parsing
  //! @return true if parsing was successful, or false otherwise
  Standard_EXPORT static bool ParseInteger(const char* theExpressionString,
                                           int&      theParsedIntegerValue);

  //! Parses RGB(A) color argument(s) specified within theArgVec[0], theArgVec[1], theArgVec[2] and
  //! theArgVec[3]. Handles either color specified by name (single argument) or by RGB(A) components
  //! (3-4 arguments) in range 0..1. The result is stored in theColor on success.
  //!
  //! Usage code sample for command argument in form "cmd -color {ColorName|R G B [A]|ColorHex}":
  //! @code
  //!   for (int anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  //!   {
  //!     TCollection_AsciiString aParam (theArgVec[anArgIter]);
  //!     aParam.LowerCase();
  //!     if (aParam == "-color")
  //!     {
  //!       Quantity_ColorRGBA aColor;
  //!       int aNbParsed = Draw::ParseColor (theArgNb  - anArgIter - 1,
  //!                                                      theArgVec + anArgIter + 1, aColor);
  //!       anArgIter += aNbParsed;
  //!       if (aNbParsed == 0) { std::cerr << "Syntax error at '" << aParam << "'"; return 1; }
  //!       // process color
  //!     }
  //!   }
  //! @endcode
  //!
  //! @param[in] theArgNb   number of available arguments in theArgVec (array limits)
  //! @param[in] theArgVec  argument list
  //! @param[out] theColor  retrieved color
  //! @return number of handled arguments (1, 2, 3 or 4) or 0 on syntax error
  static int ParseColor(const int   theArgNb,
                                     const char* const* const theArgVec,
                                     Quantity_ColorRGBA&      theColor)
  {
    return parseColor(theArgNb, theArgVec, theColor, true);
  }

  //! Parses RGB color argument(s).
  //! @param[in] theArgNb   number of available arguments in theArgVec (array limits)
  //! @param[in] theArgVec  argument list
  //! @param[out] theColor  retrieved color
  //! @return number of handled arguments (1 or 3) or 0 on syntax error.
  static int ParseColor(const int   theArgNb,
                                     const char* const* const theArgVec,
                                     Quantity_Color&          theColor)
  {
    Quantity_ColorRGBA     anRgba;
    const int aNbParsed = parseColor(theArgNb, theArgVec, anRgba, false);
    if (aNbParsed != 0)
    {
      theColor = anRgba.GetRGB();
    }
    return aNbParsed;
  }

  //! Parses boolean argument. Handles either flag specified by 0|1 or on|off.
  //!
  //! Usage code sample for command argument in form "cmd -usefeature [on|off|1|0]=on":
  //! @code
  //!   for (int anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  //!   {
  //!     TCollection_AsciiString aParam (theArgVec[anArgIter]);
  //!     aParam.LowerCase();
  //!     if (aParam == "-usefeature")
  //!     {
  //!       bool toUseFeature = true;
  //!       if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff (theArgVec[anArgIter + 1]))
  //!       {
  //!         ++anArgIter;
  //!       }
  //!       // process feature
  //!     }
  //!   }
  //! @endcode
  //!
  //! @param[in] theArg    argument value
  //! @param[out] theIsOn  decoded Boolean flag
  //! @return FALSE on syntax error
  Standard_EXPORT static bool ParseOnOff(const char*  theArg,
                                                     bool& theIsOn);

  //! Parses boolean argument at specified iterator position with optional on/off coming next.
  //!
  //! Usage code sample for command argument in form "cmd -usefeature [on|off|1|0]=on":
  //! @code
  //!   for (int anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  //!   {
  //!     if (strcasecmp (theArgVec[anArgIter], "-usefeature") == 0)
  //!     {
  //!       bool toUseFeature = Draw::ParseOnOffIterator (theNbArgs, theArgVec, anArgIter);
  //!       // process feature
  //!     }
  //!   }
  //! @endcode
  //!
  //! @param[in] theArgsNb  overall number of arguments
  //! @param[in] theArgVec  vector of arguments
  //! @param[in][out] theArgIter   argument position to parse
  //! @return flag value
  Standard_EXPORT static bool ParseOnOffIterator(int  theArgsNb,
                                                             const char**      theArgVec,
                                                             int& theArgIter);

  //! Parses boolean argument at specified iterator position with optional on/off coming next.
  //! Similar to ParseOnOffIterator() but also reverses returned value if argument name starts with
  //! "no" prefix. E.g. if nominal argument is "cmd -usefeature [on|off|1|0]=on", then
  //! "-nousefeature" argument will return FALSE.
  //! @param[in] theArgsNb  overall number of arguments
  //! @param[in] theArgVec  vector of arguments
  //! @param[in][out] theArgIter   argument position to parse
  //! @return flag value
  Standard_EXPORT static bool ParseOnOffNoIterator(int  theArgsNb,
                                                               const char**      theArgVec,
                                                               int& theArgIter);

public:
  //! Returns last graphic selection description.
  Standard_EXPORT static void LastPick(int& view,
                                       int& X,
                                       int& Y,
                                       int& button);

  //! Asks to repaint the screen after the current command.
  Standard_EXPORT static void Repaint();

  //! sets progress indicator
  Standard_EXPORT static void SetProgressBar(const occ::handle<Draw_ProgressIndicator>& theProgress);

  //! gets progress indicator
  Standard_EXPORT static occ::handle<Draw_ProgressIndicator> GetProgressBar();

public: //! @name methods loading standard command sets
  //! Defines all Draw commands
  Standard_EXPORT static void Commands(Draw_Interpretor& I);

  //! Defines Draw basic commands
  Standard_EXPORT static void BasicCommands(Draw_Interpretor& I);

  //! Defines Draw message commands
  Standard_EXPORT static void MessageCommands(Draw_Interpretor& I);

  //! Defines Draw variables handling commands.
  Standard_EXPORT static void VariableCommands(Draw_Interpretor& I);

  //! Defines Draw variables handling commands.
  Standard_EXPORT static void GraphicCommands(Draw_Interpretor& I);

  //! Defines Loads Draw plugins commands.
  Standard_EXPORT static void PloadCommands(Draw_Interpretor& I);

  //! Defines Draw unit commands
  Standard_EXPORT static void UnitCommands(Draw_Interpretor& I);

protected:
  //! Returns a variable value.
  //! @param[in][out] theName   variable name, or "." to activate picking
  //! @param[in] theToAllowPick  when TRUE, "." name will activate picking
  Standard_EXPORT static occ::handle<Draw_Drawable3D> getDrawable(const char*& theName,
                                                             bool  theToAllowPick);

  //! Parses RGB(A) color argument(s) specified within theArgVec[0], theArgVec[1], theArgVec[2] and
  //! theArgVec[3]. Handles either color specified by name (single argument) or by RGB(A) components
  //! (3-4 arguments) in range 0..1. The result is stored in theColor on success. Returns number of
  //! handled arguments (1, 2, 3 or 4) or 0 on syntax error.
  Standard_EXPORT static int parseColor(int    theArgNb,
                                                     const char* const*  theArgVec,
                                                     Quantity_ColorRGBA& theColor,
                                                     bool                theToParseAlpha);
};

#endif // _Draw_HeaderFile
