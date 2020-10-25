// Created on: 1998-09-01
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
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

#include <OpenGl_GlCore20.hxx>
#include <ViewerTest.hxx>

#include <AIS_AnimationCamera.hxx>
#include <AIS_AnimationObject.hxx>
#include <AIS_CameraFrustum.hxx>
#include <AIS_ColorScale.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_Manipulator.hxx>
#include <AIS_ViewCube.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_Grid.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <Graphic3d_CubeMapPacked.hxx>
#include <Graphic3d_CubeMapSeparate.hxx>
#include <Graphic3d_GraduatedTrihedron.hxx>
#include <Graphic3d_NameOfTextureEnv.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_TypeOfTextureFilter.hxx>
#include <Image_AlienPixMap.hxx>
#include <Image_Diff.hxx>
#include <Image_VideoRecorder.hxx>
#include <Message_ProgressScope.hxx>
#include <Message_ProgressRange.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_LocalArray.hxx>
#include <NCollection_Vector.hxx>
#include <OSD.hxx>
#include <OSD_Parallel.hxx>
#include <OSD_Timer.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Text.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_MapOfAsciiString.hxx>
#include <ViewerTest_AutoUpdater.hxx>
#include <ViewerTest_ContinuousRedrawer.hxx>
#include <ViewerTest_EventManager.hxx>
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_CmdParser.hxx>
#include <ViewerTest_V3dView.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_SpotLight.hxx>

#include <tcl.h>

#include <cstdlib>

#if defined(_WIN32)
  #include <WNT_WClass.hxx>
  #include <WNT_Window.hxx>
  #include <WNT_HIDSpaceMouse.hxx>
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  #include <Cocoa_Window.hxx>
#else
  #include <Xw_Window.hxx>
  #include <X11/Xlib.h> /* contains some dangerous #defines such as Status, True etc. */
  #include <X11/Xutil.h>
  #include <tk.h>
#endif

//==============================================================================
//  VIEWER GLOBAL VARIABLES
//==============================================================================

Standard_IMPORT Standard_Boolean Draw_VirtualWindows;
Standard_IMPORT Standard_Boolean Draw_Interprete (const char* theCommand);

Standard_EXPORT int ViewerMainLoop(Standard_Integer , const char** argv);
extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();

#if defined(_WIN32)
static Handle(WNT_Window)& VT_GetWindow() {
  static Handle(WNT_Window) WNTWin;
  return WNTWin;
}
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
static Handle(Cocoa_Window)& VT_GetWindow()
{
  static Handle(Cocoa_Window) aWindow;
  return aWindow;
}
extern void ViewerTest_SetCocoaEventManagerView (const Handle(Cocoa_Window)& theWindow);
extern void GetCocoaScreenResolution (Standard_Integer& theWidth, Standard_Integer& theHeight);

#else
static Handle(Xw_Window)& VT_GetWindow(){
  static Handle(Xw_Window) XWWin;
  return XWWin;
}

static void VProcessEvents(ClientData,int);
#endif

static Handle(Aspect_DisplayConnection)& GetDisplayConnection()
{
  static Handle(Aspect_DisplayConnection) aDisplayConnection;
  return aDisplayConnection;
}

static void SetDisplayConnection (const Handle(Aspect_DisplayConnection)& theDisplayConnection)
{
  GetDisplayConnection() = theDisplayConnection;
}

NCollection_DoubleMap <TCollection_AsciiString, Handle(V3d_View)> ViewerTest_myViews;
static NCollection_DoubleMap <TCollection_AsciiString, Handle(AIS_InteractiveContext)>  ViewerTest_myContexts;
static NCollection_DoubleMap <TCollection_AsciiString, Handle(Graphic3d_GraphicDriver)> ViewerTest_myDrivers;
static OpenGl_Caps ViewerTest_myDefaultCaps;

static void OSWindowSetup();

static struct
{
  Quantity_Color FlatColor;
  Quantity_Color GradientColor1;
  Quantity_Color GradientColor2;
  Aspect_GradientFillMethod FillMethod;
} ViewerTest_DefaultBackground = { Quantity_NOC_BLACK, Quantity_NOC_BLACK, Quantity_NOC_BLACK, Aspect_GFM_NONE };

//==============================================================================
//  EVENT GLOBAL VARIABLES
//==============================================================================

Standard_Boolean TheIsAnimating = Standard_False;

namespace
{

  //! Checks if some set is a subset of other set
  //! @tparam TheSuperSet the type of the superset
  //! @tparam TheSubSet the type of the subset
  //! @param theSuperSet the superset
  //! @param theSubSet the subset to be checked
  //! @return true if the superset includes subset, or false otherwise
  template <typename TheSuperSet, typename TheSubSet>
  static bool includes (const TheSuperSet& theSuperSet, const TheSubSet& theSubSet)
  {
    return std::includes (theSuperSet.begin(), theSuperSet.end(), theSubSet.begin(), theSubSet.end());
  }

  //! A variable set of keys for command-line options.
  //! It includes a set of mandatory keys and a set of all possible keys.
  class CommandOptionKeyVariableSet
  {
  public:
    //! Default constructor
    CommandOptionKeyVariableSet()
    {
    }

    //! Constructor
    //! @param theMandatoryKeySet the set of the mandatory option keys
    //! @param theAdditionalKeySet the set of additional options that could be omitted
    CommandOptionKeyVariableSet (
      const ViewerTest_CommandOptionKeySet& theMandatoryKeySet,
      const ViewerTest_CommandOptionKeySet& theAdditionalKeySet = ViewerTest_CommandOptionKeySet())
    : myMandatoryKeySet (theMandatoryKeySet)
    {
      std::set_union (theMandatoryKeySet.begin(),
                      theMandatoryKeySet.end(),
                      theAdditionalKeySet.begin(),
                      theAdditionalKeySet.end(),
                      std::inserter (myFullKeySet, myFullKeySet.begin()));
    }

    //! Checks if the set of option keys fits to the current variable set (it must contain all mandatory keys
    //! and be contained in the full key set)
    //! @param theCheckedKeySet the set of option keys to be checked
    bool IsInSet (const ViewerTest_CommandOptionKeySet& theCheckedKeySet) const
    {
      return includes (theCheckedKeySet, myMandatoryKeySet) && includes (myFullKeySet, theCheckedKeySet);
    }

  private:
    //! A set of mandatory command-line option keys
    ViewerTest_CommandOptionKeySet myMandatoryKeySet;

    //! A full set of command-line option keys (includes mandatory and additional option keys)
    ViewerTest_CommandOptionKeySet myFullKeySet;
  };

  //! Gets some code by its name
  //! @tparam TheCode the type of a code to be found
  //! @param theCodeNameMap the map from code names to codes
  //! @param theCodeName the name of a code to be found
  //! @param theCode the code to be found
  //! @return true if a code is found, or false otherwise
  template <typename TheCode>
  static bool getSomeCodeByName (const std::map<TCollection_AsciiString, TheCode>& theCodeNameMap,
                                 TCollection_AsciiString                           theCodeName,
                                 TheCode&                                          theCode)
  {
    theCodeName.LowerCase();
    const typename std::map<TCollection_AsciiString, TheCode>::const_iterator aCodeIterator = theCodeNameMap.find (
      theCodeName);
    if (aCodeIterator == theCodeNameMap.end())
    {
      return false;
    }
    theCode = aCodeIterator->second;
    return true;
  }

  // Defines possible commands related to background changing
  enum BackgroundCommand
  {
    BackgroundCommand_Main,              //!< The main command that manages other commands through options
    BackgroundCommand_Image,             //!< Sets an image as a background
    BackgroundCommand_ImageMode,         //!< Changes a background image mode
    BackgroundCommand_Gradient,          //!< Sets a gradient as a background
    BackgroundCommand_GradientMode,      //!< Changes a background gradient mode
    BackgroundCommand_Color,             //!< Fills background with a specified color
    BackgroundCommand_Default            //!< Sets the background default color or gradient
  };

  //! Map from background command names to its codes
  typedef std::map<TCollection_AsciiString, BackgroundCommand> BackgroundCommandNameMap;

  //! Creates a map from background command names to its codes
  //! @return a map from background command names to its codes
  static BackgroundCommandNameMap createBackgroundCommandNameMap()
  {
    BackgroundCommandNameMap aBackgroundCommandNameMap;
    aBackgroundCommandNameMap["vbackground"]      = BackgroundCommand_Main;
    aBackgroundCommandNameMap["vsetbg"]           = BackgroundCommand_Image;
    aBackgroundCommandNameMap["vsetbgmode"]       = BackgroundCommand_ImageMode;
    aBackgroundCommandNameMap["vsetgradientbg"]   = BackgroundCommand_Gradient;
    aBackgroundCommandNameMap["vsetgrbgmode"]     = BackgroundCommand_GradientMode;
    aBackgroundCommandNameMap["vsetcolorbg"]      = BackgroundCommand_Color;
    aBackgroundCommandNameMap["vsetdefaultbg"]    = BackgroundCommand_Default;
    return aBackgroundCommandNameMap;
  }

  //! Gets a background command by its name
  //! @param theBackgroundCommandName the name of the background command
  //! @param theBackgroundCommand the background command to be found
  //! @return true if a background command is found, or false otherwise
  static bool getBackgroundCommandByName (const TCollection_AsciiString& theBackgroundCommandName,
                                          BackgroundCommand&             theBackgroundCommand)
  {
    static const BackgroundCommandNameMap THE_BACKGROUND_COMMAND_NAME_MAP = createBackgroundCommandNameMap();
    return getSomeCodeByName (THE_BACKGROUND_COMMAND_NAME_MAP, theBackgroundCommandName, theBackgroundCommand);
  }

  //! Map from background image fill method names to its codes
  typedef std::map<TCollection_AsciiString, Aspect_FillMethod> BackgroundImageFillMethodNameMap;

  //! Creates a map from background image fill method names to its codes
  //! @return a map from background image fill method names to its codes
  static BackgroundImageFillMethodNameMap createBackgroundImageFillMethodNameMap()
  {
    BackgroundImageFillMethodNameMap aBackgroundImageFillMethodNameMap;
    aBackgroundImageFillMethodNameMap["none"]     = Aspect_FM_NONE;
    aBackgroundImageFillMethodNameMap["centered"] = Aspect_FM_CENTERED;
    aBackgroundImageFillMethodNameMap["tiled"]    = Aspect_FM_TILED;
    aBackgroundImageFillMethodNameMap["stretch"]  = Aspect_FM_STRETCH;
    return aBackgroundImageFillMethodNameMap;
  }

  //! Gets a background image fill method by its name
  //! @param theBackgroundImageFillMethodName the name of the background image fill method
  //! @param theBackgroundImageFillMethod the background image fill method to be found
  //! @return true if a background image fill method is found, or false otherwise
  static bool getBackgroundImageFillMethodByName (const TCollection_AsciiString& theBackgroundImageFillMethodName,
                                                  Aspect_FillMethod&             theBackgroundImageFillMethod)
  {
    static const BackgroundImageFillMethodNameMap THE_BACKGROUND_IMAGE_FILL_METHOD_NAME_MAP =
      createBackgroundImageFillMethodNameMap();
    return getSomeCodeByName (THE_BACKGROUND_IMAGE_FILL_METHOD_NAME_MAP,
                              theBackgroundImageFillMethodName,
                              theBackgroundImageFillMethod);
  }

  //! Map from background gradient fill method names to its codes
  typedef std::map<TCollection_AsciiString, Aspect_GradientFillMethod> BackgroundGradientFillMethodNameMap;

  //! Creates a map from background gradient fill method names to its codes
  //! @return a map from background gradient fill method names to its codes
  static BackgroundGradientFillMethodNameMap createBackgroundGradientFillMethodNameMap()
  {
    BackgroundGradientFillMethodNameMap aBackgroundGradientFillMethodNameMap;
    aBackgroundGradientFillMethodNameMap["none"]       = Aspect_GFM_NONE;
    aBackgroundGradientFillMethodNameMap["hor"]        = Aspect_GFM_HOR;
    aBackgroundGradientFillMethodNameMap["horizontal"] = Aspect_GFM_HOR;
    aBackgroundGradientFillMethodNameMap["ver"]        = Aspect_GFM_VER;
    aBackgroundGradientFillMethodNameMap["vertical"]   = Aspect_GFM_VER;
    aBackgroundGradientFillMethodNameMap["diag1"]      = Aspect_GFM_DIAG1;
    aBackgroundGradientFillMethodNameMap["diagonal1"]  = Aspect_GFM_DIAG1;
    aBackgroundGradientFillMethodNameMap["diag2"]      = Aspect_GFM_DIAG2;
    aBackgroundGradientFillMethodNameMap["diagonal2"]  = Aspect_GFM_DIAG2;
    aBackgroundGradientFillMethodNameMap["corner1"]    = Aspect_GFM_CORNER1;
    aBackgroundGradientFillMethodNameMap["corner2"]    = Aspect_GFM_CORNER2;
    aBackgroundGradientFillMethodNameMap["corner3"]    = Aspect_GFM_CORNER3;
    aBackgroundGradientFillMethodNameMap["corner4"]    = Aspect_GFM_CORNER4;
    return aBackgroundGradientFillMethodNameMap;
  }

  //! Gets a gradient fill method by its name
  //! @param theBackgroundGradientFillMethodName the name of the gradient fill method
  //! @param theBackgroundGradientFillMethod the gradient fill method to be found
  //! @return true if a gradient fill method is found, or false otherwise
  static bool getBackgroundGradientFillMethodByName (const TCollection_AsciiString& theBackgroundGradientFillMethodName,
                                                     Aspect_GradientFillMethod&     theBackgroundGradientFillMethod)
  {
    static const BackgroundGradientFillMethodNameMap THE_BACKGROUND_GRADIENT_FILL_METHOD_NAME_MAP =
      createBackgroundGradientFillMethodNameMap();
    return getSomeCodeByName (THE_BACKGROUND_GRADIENT_FILL_METHOD_NAME_MAP,
                              theBackgroundGradientFillMethodName,
                              theBackgroundGradientFillMethod);
  }

  //! Changes the background in accordance with passed command line options
  class BackgroundChanger
  {
  public:
    //! Constructor. Prepares the command parser
    BackgroundChanger()
    {
      prepareCommandParser();
    }

    //! Processes the command line and changes the background
    //! @param theDrawInterpretor the interpreter of the Draw Harness application
    //! @param theNumberOfCommandLineArguments the number of passed command line arguments
    //! @param theCommandLineArguments the array of command line arguments
    bool ProcessCommandLine (Draw_Interpretor&        theDrawInterpretor,
                             const Standard_Integer   theNumberOfCommandLineArguments,
                             const char* const* const theCommandLineArguments)
    {
      const char* const aBackgroundCommandName = theCommandLineArguments[0];
      BackgroundCommand aBackgroundCommand = BackgroundCommand_Main;
      if (!getBackgroundCommandByName (aBackgroundCommandName, aBackgroundCommand))
      {
        return false;
      }
      addCommandDescription (aBackgroundCommand);
      myCommandParser.Parse (theNumberOfCommandLineArguments, theCommandLineArguments);
      return processCommandOptions (aBackgroundCommandName, aBackgroundCommand, theDrawInterpretor);
    }

  private:
    //! The type of functions that are able to set gradient background filling
    typedef void SetGradientFunction (const Quantity_Color& /* theColor1 */,
                                      const Quantity_Color& /* theColor2 */,
                                      const Aspect_GradientFillMethod /* theGradientMode */);

    //! The type of functions that are able to fill a background with a specific color
    typedef void SetColorFunction (const Quantity_Color& /* theColor */);

    //! the command parser used to parse command line options and its arguments
    ViewerTest_CmdParser myCommandParser;

    //! the option key for the command that sets an image as a background
    ViewerTest_CommandOptionKey myImageOptionKey;

    //! the option key for the command that sets a background image fill type
    ViewerTest_CommandOptionKey myImageModeOptionKey;

    //! the option key for the command that sets a gradient filling for the background
    ViewerTest_CommandOptionKey myGradientOptionKey;

    //! the option key for the command that sets a background gradient filling method
    ViewerTest_CommandOptionKey myGradientModeOptionKey;

    //! the option key for the command that fills background with a specific color
    ViewerTest_CommandOptionKey myColorOptionKey;

    //! the option key for the command that sets default background gradient or color
    ViewerTest_CommandOptionKey myDefaultOptionKey;

    //! the option key for the command that sets an environment cubemap as a background
    ViewerTest_CommandOptionKey myCubeMapOptionKey;

    //! the option key for the command that defines order of tiles in one image packed cubemap
    ViewerTest_CommandOptionKey myCubeMapOrderOptionKey;

    //! the option key for the command that sets inversion of Z axis for background cubemap
    ViewerTest_CommandOptionKey myCubeMapInvertedZOptionKey;

    //! the option key for the command that allows skip IBL map generation
    ViewerTest_CommandOptionKey myCubeMapDoNotGenPBREnvOptionKey;

    //! the variable set of options that are allowed for the old scenario (without any option passed)
    CommandOptionKeyVariableSet myUnnamedOptionVariableSet;

    //! the variable set of options that are allowed for setting an environment cubemap as background
    CommandOptionKeyVariableSet myCubeMapOptionVariableSet;

    //! the variable set of options that are allowed for setting an image as a background
    CommandOptionKeyVariableSet myImageOptionVariableSet;

    //! the variable set of options that are allowed for setting a background image fill type
    CommandOptionKeyVariableSet myImageModeOptionVariableSet;

    //! the variable set of options that are allowed for setting a gradient filling for the background
    CommandOptionKeyVariableSet myGradientOptionVariableSet;

    //! the variable set of options that are allowed for setting a background gradient filling method
    CommandOptionKeyVariableSet myGradientModeOptionVariableSet;

    //! the variable set of options that are allowed for filling a background with a specific color
    CommandOptionKeyVariableSet myColorOptionVariableSet;

    //! the variable set of options that are allowed for setting a default background gradient
    CommandOptionKeyVariableSet myDefaultGradientOptionVariableSet;

    //! the variable set of options that are allowed for setting a default background color
    CommandOptionKeyVariableSet myDefaultColorOptionVariableSet;

    //! the variable set of options that are allowed for printing help
    CommandOptionKeyVariableSet myHelpOptionVariableSet;

    //! Adds options to command parser
    void addOptionsToCommandParser()
    {
      myImageOptionKey     = myCommandParser.AddOption ("imageFile|image|imgFile|img",
                                                    "filename of image used as background");
      myImageModeOptionKey = myCommandParser.AddOption (
        "imageMode|imgMode", "image fill type, should be one of CENTERED, TILED, STRETCH, NONE");
      myGradientOptionKey = myCommandParser.AddOption ("gradient|grad|gr",
                                                       "sets background gradient starting and ending colors");
      myGradientModeOptionKey =
        myCommandParser.AddOption ("gradientMode|gradMode|gradMd|grMode|grMd",
                                   "gradient fill method, should be one of NONE, HOR[IZONTAL], VER[TICAL], "
                                   "DIAG[ONAL]1, DIAG[ONAL]2, CORNER1, CORNER2, CORNER3, CORNER4");
      myColorOptionKey   = myCommandParser.AddOption ("color|col", "background color");
      myDefaultOptionKey = myCommandParser.AddOption ("default|def", "sets background default gradient or color");

      myCubeMapOptionKey           = myCommandParser.AddOption ("cubemap|cmap|cm", "background cubemap");
      myCubeMapOrderOptionKey      = myCommandParser.AddOption ("order|o", "order of sides in one image packed cubemap");
      myCubeMapInvertedZOptionKey = myCommandParser.AddOption (
        "invertedz|invz|iz", "whether Z axis is inverted or not during background cubemap rendering");
      myCubeMapDoNotGenPBREnvOptionKey = myCommandParser.AddOption ("nopbrenv", "whether IBL map generation should be skipped");
    }

    //! Creates option sets used to determine if a passed option set is valid or not
    void createOptionSets()
    {
      ViewerTest_CommandOptionKeySet anUnnamedOptionSet;
      anUnnamedOptionSet.insert (ViewerTest_CmdParser::THE_UNNAMED_COMMAND_OPTION_KEY);
      myUnnamedOptionVariableSet = CommandOptionKeyVariableSet (anUnnamedOptionSet);

      ViewerTest_CommandOptionKeySet aCubeMapOptionSet;
      aCubeMapOptionSet.insert (myCubeMapOptionKey);
      ViewerTest_CommandOptionKeySet aCubeMapAdditionalOptionKeySet;
      aCubeMapAdditionalOptionKeySet.insert (myCubeMapInvertedZOptionKey);
      aCubeMapAdditionalOptionKeySet.insert (myCubeMapDoNotGenPBREnvOptionKey);
      aCubeMapAdditionalOptionKeySet.insert (myCubeMapOrderOptionKey);
      myCubeMapOptionVariableSet     = CommandOptionKeyVariableSet (aCubeMapOptionSet, aCubeMapAdditionalOptionKeySet);

      ViewerTest_CommandOptionKeySet anImageOptionSet;
      anImageOptionSet.insert (myImageOptionKey);
      ViewerTest_CommandOptionKeySet anImageModeOptionSet;
      anImageModeOptionSet.insert (myImageModeOptionKey);
      myImageOptionVariableSet     = CommandOptionKeyVariableSet (anImageOptionSet, anImageModeOptionSet);
      myImageModeOptionVariableSet = CommandOptionKeyVariableSet (anImageModeOptionSet);

      ViewerTest_CommandOptionKeySet aGradientOptionSet;
      aGradientOptionSet.insert (myGradientOptionKey);
      ViewerTest_CommandOptionKeySet aGradientModeOptionSet;
      aGradientModeOptionSet.insert (myGradientModeOptionKey);
      myGradientOptionVariableSet     = CommandOptionKeyVariableSet (aGradientOptionSet, aGradientModeOptionSet);
      myGradientModeOptionVariableSet = CommandOptionKeyVariableSet (aGradientModeOptionSet);

      ViewerTest_CommandOptionKeySet aColorOptionSet;
      aColorOptionSet.insert (myColorOptionKey);
      myColorOptionVariableSet = CommandOptionKeyVariableSet (aColorOptionSet);

      aGradientOptionSet.insert (myDefaultOptionKey);
      myDefaultGradientOptionVariableSet = CommandOptionKeyVariableSet (aGradientOptionSet, aGradientModeOptionSet);
      aColorOptionSet.insert (myDefaultOptionKey);
      myDefaultColorOptionVariableSet = CommandOptionKeyVariableSet (aColorOptionSet);

      ViewerTest_CommandOptionKeySet aHelpOptionSet;
      aHelpOptionSet.insert (ViewerTest_CmdParser::THE_HELP_COMMAND_OPTION_KEY);
      myHelpOptionVariableSet = CommandOptionKeyVariableSet (aHelpOptionSet);
    }

    //! Prepares the command parser. Adds options and creates option sets used to determine
    //! if a passed option set is valid or not
    void prepareCommandParser()
    {
      addOptionsToCommandParser();
      createOptionSets();
    }

    //! Adds a command description to the command parser
    //! @param theBackgroundCommand the key of the command which description is added to the command parser
    void addCommandDescription (const BackgroundCommand theBackgroundCommand)
    {
      std::string aDescription;
      bool        isMainCommand = false;
      switch (theBackgroundCommand)
      {
        case BackgroundCommand_Main:
          aDescription  = "Command: vbackground (changes background or some background settings)";
          isMainCommand = true;
          break;
        case BackgroundCommand_Image:
          aDescription = "Command: vsetbg (loads image as a background)";
          break;
        case BackgroundCommand_ImageMode:
          aDescription = "Command: vsetbgmode (changes background fill type)";
          break;
        case BackgroundCommand_Gradient:
          aDescription = "Command: vsetgradientbg (mounts gradient background)";
          break;
        case BackgroundCommand_GradientMode:
          aDescription = "Command: vsetgradientbgmode (changes gradient background fill method)";
          break;
        case BackgroundCommand_Color:
          aDescription = "Command: vsetcolorbg (sets color background)";
          break;
        case BackgroundCommand_Default:
          aDescription = "Command: vsetdefaultbg (sets default viewer background gradient or fill color)";
          break;
        default:
          return;
      }
      if (!isMainCommand)
      {
        aDescription += "\nThis command is obsolete. Use vbackground instead.";
      }
      myCommandParser.SetDescription (aDescription);
    }

    //! Check if a viewer is needed to be initialized
    //! @param theBackgroundCommand the key of the command that changes the background
    //! @return true if processing was successful, or false otherwise
    bool checkViewerIsNeeded (const BackgroundCommand theBackgroundCommand) const
    {
      const bool                           isMain             = (theBackgroundCommand == BackgroundCommand_Main);
      const ViewerTest_CommandOptionKeySet aUsedOptions       = myCommandParser.GetUsedOptions();
      const bool                           aViewerIsNotNeeded =
        (theBackgroundCommand == BackgroundCommand_Default)
        || (myDefaultGradientOptionVariableSet.IsInSet (aUsedOptions) && isMain)
        || (myDefaultColorOptionVariableSet.IsInSet (aUsedOptions) && isMain)
        || myHelpOptionVariableSet.IsInSet (aUsedOptions);
      return !aViewerIsNotNeeded;
    }

    //! Check if a viewer is initialized
    //! @param theBackgroundCommandName the name of the command that changes the background
    //! @param theDrawInterpretor the interpreter of the Draw Harness application
    //! @return true if a viewer is initialized, or false otherwise
    static bool checkViewerIsInitialized (const char* const theBackgroundCommandName,
                                          Draw_Interpretor& theDrawInterpretor)
    {
      const Handle (AIS_InteractiveContext)& anAISContext = ViewerTest::GetAISContext();
      if (anAISContext.IsNull())
      {
        theDrawInterpretor << "Use 'vinit' command before executing '" << theBackgroundCommandName << "' command.\n";
        return false;
      }
      return true;
    }

    //! Processes command options
    //! @param theBackgroundCommandName the name of the command that changes the background
    //! @param theBackgroundCommand the key of the command that changes the background
    //! @param theDrawInterpretor the interpreter of the Draw Harness application
    //! @return true if processing was successful, or false otherwise
    bool processCommandOptions (const char* const       theBackgroundCommandName,
                                const BackgroundCommand theBackgroundCommand,
                                Draw_Interpretor&       theDrawInterpretor) const
    {
      if (myCommandParser.HasNoOption())
      {
        return printHelp (theBackgroundCommandName, theDrawInterpretor);
      }
      if (checkViewerIsNeeded (theBackgroundCommand)
          && !checkViewerIsInitialized (theBackgroundCommandName, theDrawInterpretor))
      {
        return false;
      }
      if (myCommandParser.HasOnlyUnnamedOption())
      {
        return processUnnamedOption (theBackgroundCommand);
      }
      return processNamedOptions (theBackgroundCommandName, theBackgroundCommand, theDrawInterpretor);
    }

    //! Processes the unnamed option
    //! @param theBackgroundCommand the key of the command that changes the background
    //! @return true if processing was successful, or false otherwise
    bool processUnnamedOption (const BackgroundCommand theBackgroundCommand) const
    {
      switch (theBackgroundCommand)
      {
        case BackgroundCommand_Main:
          return false;
        case BackgroundCommand_Image:
          return processImageUnnamedOption();
        case BackgroundCommand_ImageMode:
          return processImageModeUnnamedOption();
        case BackgroundCommand_Gradient:
          return processGradientUnnamedOption();
        case BackgroundCommand_GradientMode:
          return processGradientModeUnnamedOption();
        case BackgroundCommand_Color:
          return processColorUnnamedOption();
        case BackgroundCommand_Default:
          return processDefaultUnnamedOption();
        default:
          return false;
      }
    }

    //! Processes the image unnamed option
    //! @return true if processing was successful, or false otherwise
    bool processImageUnnamedOption() const
    {
      const std::size_t aNumberOfImageUnnamedOptionArguments = myCommandParser.GetNumberOfOptionArguments (
        ViewerTest_CmdParser::THE_UNNAMED_COMMAND_OPTION_KEY);
      if ((aNumberOfImageUnnamedOptionArguments != 1) && (aNumberOfImageUnnamedOptionArguments != 2))
      {
        return false;
      }
      std::string anImageFileName;
      if (!myCommandParser.Arg (ViewerTest_CmdParser::THE_UNNAMED_COMMAND_OPTION_KEY, 0, anImageFileName))
      {
        return false;
      }
      Aspect_FillMethod anImageMode = Aspect_FM_CENTERED;
      if (aNumberOfImageUnnamedOptionArguments == 2)
      {
        std::string anImageModeString;
        if (!myCommandParser.Arg (ViewerTest_CmdParser::THE_UNNAMED_COMMAND_OPTION_KEY, 1, anImageModeString))
        {
          return false;
        }
        if (!getBackgroundImageFillMethodByName (anImageModeString.c_str(), anImageMode))
        {
          return false;
        }
      }
      setImage (anImageFileName.c_str(), anImageMode);
      return true;
    }

    //! Processes the image mode unnamed option
    //! @return true if processing was successful, or false otherwise
    bool processImageModeUnnamedOption() const
    {
      return processImageModeOptionSet (ViewerTest_CmdParser::THE_UNNAMED_COMMAND_OPTION_KEY);
    }

    //! Processes the gradient unnamed option
    //! @param theSetGradient the function used to set a background gradient filling
    //! @return true if processing was successful, or false otherwise
    bool processGradientUnnamedOption (SetGradientFunction* const theSetGradient = setGradient) const
    {
      const Standard_Integer aNumberOfGradientUnnamedOptionArguments = myCommandParser.GetNumberOfOptionArguments (
        ViewerTest_CmdParser::THE_UNNAMED_COMMAND_OPTION_KEY);
      if (aNumberOfGradientUnnamedOptionArguments < 2)
      {
        return false;
      }

      Standard_Integer anArgumentIndex = 0;
      Quantity_Color   aColor1;
      if (!myCommandParser.ArgColor (ViewerTest_CmdParser::THE_UNNAMED_COMMAND_OPTION_KEY, anArgumentIndex, aColor1))
      {
        return false;
      }
      if (anArgumentIndex >= aNumberOfGradientUnnamedOptionArguments)
      {
        return false;
      }

      Quantity_Color aColor2;
      if (!myCommandParser.ArgColor (ViewerTest_CmdParser::THE_UNNAMED_COMMAND_OPTION_KEY, anArgumentIndex, aColor2))
      {
        return false;
      }
      if (anArgumentIndex > aNumberOfGradientUnnamedOptionArguments)
      {
        return false;
      }

      Aspect_GradientFillMethod aGradientMode = Aspect_GFM_HOR;
      if (anArgumentIndex == aNumberOfGradientUnnamedOptionArguments - 1)
      {
        std::string anGradientModeString;

        if (!myCommandParser.Arg (ViewerTest_CmdParser::THE_UNNAMED_COMMAND_OPTION_KEY,
                                  anArgumentIndex,
                                  anGradientModeString))
        {
          return false;
        }
        if (!getBackgroundGradientFillMethodByName (anGradientModeString.c_str(), aGradientMode))
        {
          return false;
        }
        ++anArgumentIndex;
      }
      if (anArgumentIndex != aNumberOfGradientUnnamedOptionArguments)
      {
        return false;
      }
      theSetGradient (aColor1, aColor2, aGradientMode);
      return true;
    }

    //! Processes the gradient mode unnamed option
    //! @return true if processing was successful, or false otherwise
    bool processGradientModeUnnamedOption() const
    {
      return processGradientModeOptionSet (ViewerTest_CmdParser::THE_UNNAMED_COMMAND_OPTION_KEY);
    }

    //! Processes the color unnamed option
    //! @param theSetColor the function used to set a background color
    //! @return true if processing was successful, or false otherwise
    bool processColorUnnamedOption (SetColorFunction* const theSetColor = setColor) const
    {
      return processColorOptionSet (ViewerTest_CmdParser::THE_UNNAMED_COMMAND_OPTION_KEY, theSetColor);
    }

    //! Processes the default back unnamed option
    //! @return true if processing was successful, or false otherwise
    bool processDefaultUnnamedOption() const
    {
      if (processGradientUnnamedOption (setDefaultGradient))
      {
        return true;
      }
      return processColorUnnamedOption (setDefaultColor);
    }

    //! Processes named options
    //! @param theBackgroundCommandName the name of the command that changes the background
    //! @param theBackgroundCommand the key of the command that changes the background
    //! @param theDrawInterpretor the interpreter of the Draw Harness application
    //! @return true if processing was successful, or false otherwise
    bool processNamedOptions (const char* const       theBackgroundCommandName,
                              const BackgroundCommand theBackgroundCommand,
                              Draw_Interpretor&       theDrawInterpretor) const
    {
      const bool                           isMain       = (theBackgroundCommand == BackgroundCommand_Main);
      const ViewerTest_CommandOptionKeySet aUsedOptions = myCommandParser.GetUsedOptions();
      if (myCubeMapOptionVariableSet.IsInSet (aUsedOptions) && isMain)
      {
        return processCubeMapOptionSet();
      }
      if (myImageOptionVariableSet.IsInSet (aUsedOptions)
          && (isMain || (theBackgroundCommand == BackgroundCommand_Image)))
      {
        return processImageOptionSet();
      }
      if (myImageModeOptionVariableSet.IsInSet (aUsedOptions)
          && (isMain || (theBackgroundCommand == BackgroundCommand_ImageMode)))
      {
        return processImageModeOptionSet();
      }
      if (myGradientOptionVariableSet.IsInSet (aUsedOptions)
          && (isMain || (theBackgroundCommand == BackgroundCommand_Gradient)))
      {
        return processGradientOptionSet();
      }
      if (myGradientModeOptionVariableSet.IsInSet (aUsedOptions)
          && (isMain || (theBackgroundCommand == BackgroundCommand_GradientMode)))
      {
        return processGradientModeOptionSet();
      }
      if (myColorOptionVariableSet.IsInSet (aUsedOptions)
          && (isMain || (theBackgroundCommand == BackgroundCommand_Color)))
      {
        return processColorOptionSet();
      }
      if ((myDefaultGradientOptionVariableSet.IsInSet (aUsedOptions) && isMain)
          || (myGradientOptionVariableSet.IsInSet (aUsedOptions)
              && (theBackgroundCommand == BackgroundCommand_Default)))
      {
        return processDefaultGradientOptionSet();
      }
      if ((myDefaultColorOptionVariableSet.IsInSet (aUsedOptions) && isMain)
          || (myColorOptionVariableSet.IsInSet (aUsedOptions) && (theBackgroundCommand == BackgroundCommand_Default)))
      {
        return processDefaultColorOptionSet();
      }
      if (myHelpOptionVariableSet.IsInSet (aUsedOptions))
      {
        return processHelpOptionSet (theBackgroundCommandName, theDrawInterpretor);
      }
      return false;
    }

    //! Process the cubemap option set in named and unnamed case.
    //! @return true if processing was successful, or false otherwise
    bool processCubeMapOptionSet() const
    {
      NCollection_Array1<TCollection_AsciiString> aFilePaths;

      if (!processCubeMapOptions (aFilePaths))
      {
        return false;
      }

      Graphic3d_CubeMapOrder anOrder = Graphic3d_CubeMapOrder::Default();

      if (myCommandParser.HasOption (myCubeMapOrderOptionKey))
      {
        if (!processCubeMapOrderOptions (anOrder))
        {
          return false;
        }
      }

      bool aZIsInverted = false;
      if (myCommandParser.HasOption (myCubeMapInvertedZOptionKey))
      {
        if (!processCubeMapInvertedZOptionSet())
        {
          return false;
        }
        aZIsInverted = true;
      }

      bool aToGenPBREnv = true;
      if (myCommandParser.HasOption (myCubeMapDoNotGenPBREnvOptionKey))
      {
        if (!processCubeMapDoNotGenPBREnvOptionSet())
        {
          return false;
        }
        aToGenPBREnv = false;
      }

      setCubeMap (aFilePaths, anOrder.Validated(), aZIsInverted, aToGenPBREnv);
      return true;
    }

    //! Processes the image option set
    //! @return true if processing was successful, or false otherwise
    bool processImageOptionSet() const
    {
      std::string anImageFileName;
      if (!processImageOption (anImageFileName))
      {
        return false;
      }
      Aspect_FillMethod anImageMode = Aspect_FM_CENTERED;
      if (myCommandParser.HasOption (myImageModeOptionKey) && !processImageModeOption (anImageMode))
      {
        return false;
      }
      setImage (anImageFileName.c_str(), anImageMode);
      return true;
    }

    //! Processes the image mode option set
    //! @return true if processing was successful, or false otherwise
    bool processImageModeOptionSet() const
    {
      return processImageModeOptionSet (myImageModeOptionKey);
    }

    //! Processes the image mode option set
    //! @param theImageModeOptionKey the key of the option that is interpreted as an image mode option
    //! @return true if processing was successful, or false otherwise
    bool processImageModeOptionSet (const ViewerTest_CommandOptionKey theImageModeOptionKey) const
    {
      Aspect_FillMethod anImageMode = Aspect_FM_NONE;
      if (!processImageModeOption (theImageModeOptionKey, anImageMode))
      {
        return false;
      }
      setImageMode (anImageMode);
      return true;
    }

    //! Processes the gradient option set
    //! @param theSetGradient the function used to set a background gradient filling
    //! @return true if processing was successful, or false otherwise
    bool processGradientOptionSet (SetGradientFunction* const theSetGradient = setGradient) const
    {
      Quantity_Color aColor1;
      Quantity_Color aColor2;
      if (!processGradientOption (aColor1, aColor2))
      {
        return false;
      }
      Aspect_GradientFillMethod aGradientMode = Aspect_GFM_HOR;
      if (myCommandParser.HasOption (myGradientModeOptionKey) && !processGradientModeOption (aGradientMode))
      {
        return false;
      }
      theSetGradient (aColor1, aColor2, aGradientMode);
      return true;
    }

    //! Processes the gradient mode option set
    //! @return true if processing was successful, or false otherwise
    bool processGradientModeOptionSet() const
    {
      return processGradientModeOptionSet (myGradientModeOptionKey);
    }

    //! Processes the gradient mode option set
    //! @param theGradientModeOptionKey the key of the option that is interpreted as a gradient mode option
    //! @return true if processing was successful, or false otherwise
    bool processGradientModeOptionSet (const ViewerTest_CommandOptionKey theGradientModeOptionKey) const
    {
      Aspect_GradientFillMethod aGradientMode = Aspect_GFM_NONE;
      if (!processGradientModeOption (theGradientModeOptionKey, aGradientMode))
      {
        return false;
      }
      setGradientMode (aGradientMode);
      return true;
    }

    //! Processes the color option set
    //! @param theSetColor the function used to set a background color
    //! @return true if processing was successful, or false otherwise
    bool processColorOptionSet (SetColorFunction* const theSetColor = setColor) const
    {
      return processColorOptionSet (myColorOptionKey, theSetColor);
    }

    //! Processes the default color option set
    //! @return true if processing was successful, or false otherwise
    bool processDefaultGradientOptionSet() const
    {
      return processGradientOptionSet (setDefaultGradient);
    }

    //! Processes the default gradient option set
    //! @return true if processing was successful, or false otherwise
    bool processDefaultColorOptionSet() const
    {
      return processColorOptionSet (setDefaultColor);
    }

    //! Processes the color option set
    //! @param theColorOptionKey the key of the option that is interpreted as a color option
    //! @param theSetColor the function used to set a background color
    //! @return true if processing was successful, or false otherwise
    bool processColorOptionSet (const ViewerTest_CommandOptionKey theColorOptionKey,
                                SetColorFunction* const           theSetColor = setColor) const
    {
      Quantity_Color aColor;
      if (!processColorOption (theColorOptionKey, aColor))
      {
        return false;
      }
      theSetColor (aColor);
      return true;
    }

    //! Processes the help option set
    //! @param theBackgroundCommandName the name of the command that changes the background
    //! @param theDrawInterpretor the interpreter of the Draw Harness application
    //! @return true if processing was successful, or false otherwise
    bool processHelpOptionSet (const char* const theBackgroundCommandName, Draw_Interpretor& theDrawInterpretor) const
    {
      const Standard_Integer aNumberOfHelpOptionArguments = myCommandParser.GetNumberOfOptionArguments (
        ViewerTest_CmdParser::THE_HELP_COMMAND_OPTION_KEY);
      if (aNumberOfHelpOptionArguments != 0)
      {
        return false;
      }
      return printHelp (theBackgroundCommandName, theDrawInterpretor);
    }

    //! Processes the cubemap option
    //! @param theFilePaths the array of filenames of cubemap sides
    //! @return true if processing was successful, or false otherwise
    bool processCubeMapOptions (NCollection_Array1<TCollection_AsciiString> &theFilePaths) const
    {
      const Standard_Integer aNumberOfCubeMapOptionArguments = myCommandParser.GetNumberOfOptionArguments (myCubeMapOptionKey);

      if (aNumberOfCubeMapOptionArguments != 1
       && aNumberOfCubeMapOptionArguments != 6)
      {
        return false;
      }

      theFilePaths.Resize(0, aNumberOfCubeMapOptionArguments - 1, Standard_False);

      for (int i = 0; i < aNumberOfCubeMapOptionArguments; ++i)
      {
        std::string aCubeMapFileName;
        if (!myCommandParser.Arg (myCubeMapOptionKey, i, aCubeMapFileName))
        {
          return false;
        }
        theFilePaths[i] = aCubeMapFileName.c_str();
      }

      return true;
    }

    //! Processes the inverted z cubemap option
    //! @return true if processing was successful, or false otherwise
    bool processCubeMapInvertedZOptionSet () const
    {
      const Standard_Integer aNumberOfCubeMapZInversionOptionArguments =
        myCommandParser.GetNumberOfOptionArguments (myCubeMapInvertedZOptionKey);

      if (aNumberOfCubeMapZInversionOptionArguments != 0)
      {
        return false;
      }

      return true;
    }

    //! Processes the option allowing to skip IBM maps generation
    //! @return true if processing was successful, or false otherwise
    bool processCubeMapDoNotGenPBREnvOptionSet() const
    {
      const Standard_Integer aNumberOfCubeMapDoNotGenPBREnvOptionArguments =
        myCommandParser.GetNumberOfOptionArguments(myCubeMapDoNotGenPBREnvOptionKey);

      if (aNumberOfCubeMapDoNotGenPBREnvOptionArguments != 0)
      {
        return false;
      }

      return true;
    }

    //! Processes the tiles order option
    //! @param theOrder the array of indexes if cubemap sides in tile grid
    //! @return true if processing was successful, or false otherwise
    bool processCubeMapOrderOptions (Graphic3d_CubeMapOrder& theOrder) const
    {
      const Standard_Integer aNumberOfCubeMapOrderOptionArguments = myCommandParser.GetNumberOfOptionArguments(
        myCubeMapOrderOptionKey);

      if (aNumberOfCubeMapOrderOptionArguments != 6)
      {
        return false;
      }


      for (unsigned int i = 0; i < 6; ++i)
      {
        std::string anOrderItem;
        if (!myCommandParser.Arg (myCubeMapOrderOptionKey, i, anOrderItem)) 
        {
          return false;
        }

        theOrder.Set (Graphic3d_CubeMapSide (i),
                      static_cast<unsigned char> (Draw::Atoi (anOrderItem.c_str())));
      }

      return theOrder.IsValid();
    }

    //! Processes the image option
    //! @param theImageFileName the filename of the image to be used as a background
    //! @return true if processing was successful, or false otherwise
    bool processImageOption (std::string& theImageFileName) const
    {
      const Standard_Integer aNumberOfImageOptionArguments = myCommandParser.GetNumberOfOptionArguments (
        myImageOptionKey);
      if (aNumberOfImageOptionArguments != 1)
      {
        return false;
      }
      std::string anImageFileName;
      if (!myCommandParser.Arg (myImageOptionKey, 0, anImageFileName))
      {
        return false;
      }
      theImageFileName = anImageFileName;
      return true;
    }

    //! Processes the image mode option
    //! @param theImageMode the fill type used for a background image
    //! @return true if processing was successful, or false otherwise
    bool processImageModeOption (Aspect_FillMethod& theImageMode) const
    {
      return processImageModeOption (myImageModeOptionKey, theImageMode);
    }

    //! Processes the image mode option
    //! @param theImageModeOptionKey the key of the option that is interpreted as an image mode option
    //! @param theImageMode the fill type used for a background image
    //! @return true if processing was successful, or false otherwise
    bool processImageModeOption (const ViewerTest_CommandOptionKey theImageModeOptionKey,
                                 Aspect_FillMethod&                theImageMode) const
    {
      return processModeOption (theImageModeOptionKey, getBackgroundImageFillMethodByName, theImageMode);
    }

    //! Processes the gradient option
    //! @param theColor1 the gradient starting color
    //! @param theColor2 the gradient ending color
    //! @return true if processing was successful, or false otherwise
    bool processGradientOption (Quantity_Color& theColor1, Quantity_Color& theColor2) const
    {
      Standard_Integer anArgumentIndex = 0;
      Quantity_Color   aColor1;
      if (!myCommandParser.ArgColor (myGradientOptionKey, anArgumentIndex, aColor1))
      {
        return false;
      }
      Quantity_Color aColor2;
      if (!myCommandParser.ArgColor (myGradientOptionKey, anArgumentIndex, aColor2))
      {
        return false;
      }
      const Standard_Integer aNumberOfGradientOptionArguments = myCommandParser.GetNumberOfOptionArguments (
        myGradientOptionKey);
      if (anArgumentIndex != aNumberOfGradientOptionArguments)
      {
        return false;
      }
      theColor1 = aColor1;
      theColor2 = aColor2;
      return true;
    }

    //! Processes the gradient mode option
    //! @param theGradientMode the fill method used for a background gradient filling
    //! @return true if processing was successful, or false otherwise
    bool processGradientModeOption (Aspect_GradientFillMethod& theGradientMode) const
    {
      return processGradientModeOption (myGradientModeOptionKey, theGradientMode);
    }

    //! Processes the gradient mode option
    //! @param theGradientModeOptionKey the key of the option that is interpreted as a gradient mode option
    //! @param theGradientMode the fill method used for a background gradient filling
    //! @return true if processing was successful, or false otherwise
    bool processGradientModeOption (const ViewerTest_CommandOptionKey theGradientModeOptionKey,
                                    Aspect_GradientFillMethod&        theGradientMode) const
    {
      return processModeOption (theGradientModeOptionKey, getBackgroundGradientFillMethodByName, theGradientMode);
    }

    //! Processes some mode option
    //! @tparam TheMode the type of a mode to be processed
    //! @param theModeOptionKey the key of the option that is interpreted as a mode option
    //! @param theMode a mode to be processed
    //! @return true if processing was successful, or false otherwise
    template <typename TheMode>
    bool processModeOption (const ViewerTest_CommandOptionKey theModeOptionKey,
                            bool (*const theGetModeByName) (const TCollection_AsciiString& /* theModeName */,
                                                            TheMode& /* theMode */),
                            TheMode& theMode) const
    {
      const Standard_Integer aNumberOfModeOptionArguments = myCommandParser.GetNumberOfOptionArguments (
        theModeOptionKey);
      if (aNumberOfModeOptionArguments != 1)
      {
        return false;
      }
      std::string aModeString;
      if (!myCommandParser.Arg (theModeOptionKey, 0, aModeString))
      {
        return false;
      }
      TheMode aMode = TheMode();
      if (!theGetModeByName (aModeString.c_str(), aMode))
      {
        return false;
      }
      theMode = aMode;
      return true;
    }

    //! Processes the color option
    //! @param theColor a color used for filling a background
    //! @return true if processing was successful, or false otherwise
    bool processColorOption (Quantity_Color& theColor) const
    {
      return processColorOption (myColorOptionKey, theColor);
    }

    //! Processes the color option
    //! @param theColorOptionKey the key of the option that is interpreted as a color option
    //! @param theColor a color used for filling a background
    //! @return true if processing was successful, or false otherwise
    bool processColorOption (const ViewerTest_CommandOptionKey theColorOptionKey, Quantity_Color& theColor) const
    {
      Standard_Integer anArgumentIndex = 0;
      Quantity_Color   aColor;
      if (!myCommandParser.ArgColor (theColorOptionKey, anArgumentIndex, aColor))
      {
        return false;
      }
      const Standard_Integer aNumberOfColorOptionArguments = myCommandParser.GetNumberOfOptionArguments (
        theColorOptionKey);
      if (anArgumentIndex != aNumberOfColorOptionArguments)
      {
        return false;
      }
      theColor = aColor;
      return true;
    }

    //! Prints helping message
    //! @param theBackgroundCommandName the name of the command that changes the background
    //! @param theDrawInterpretor the interpreter of the Draw Harness application
    //! @return true if printing was successful, or false otherwise
    static bool printHelp (const char* const theBackgroundCommandName, Draw_Interpretor& theDrawInterpretor)
    {
      return theDrawInterpretor.PrintHelp (theBackgroundCommandName) == TCL_OK;
    }

    //! Sets the cubemap as a background
    //! @param theFileNames the array of filenames of packed or multifile cubemap
    //! @param theOrder array of cubemap sides indexes mapping them from tiles in packed cubemap
    static void setCubeMap (const NCollection_Array1<TCollection_AsciiString>& theFileNames,
                            const Graphic3d_ValidatedCubeMapOrder              theOrder = Graphic3d_CubeMapOrder::Default(),
                            bool                                               theZIsInverted = false,
                            bool                                               theToGenPBREnv = true)
    {
      const Handle(V3d_View)& aCurrentView = ViewerTest::CurrentView();
      Handle(Graphic3d_CubeMap) aCubeMap;

      if (theFileNames.Size() == 1)
        aCubeMap = new Graphic3d_CubeMapPacked(theFileNames[0], theOrder);
      else
        aCubeMap = new Graphic3d_CubeMapSeparate(theFileNames);

      aCubeMap->SetZInversion (theZIsInverted);

      aCubeMap->GetParams()->SetFilter(Graphic3d_TOTF_BILINEAR);
      aCubeMap->GetParams()->SetRepeat(Standard_False);
      aCubeMap->GetParams()->SetTextureUnit(Graphic3d_TextureUnit_EnvMap);

      aCurrentView->SetBackgroundCubeMap (aCubeMap, theToGenPBREnv, Standard_True);
    }

    //! Sets the image as a background
    //! @param theImageFileName the filename of the image to be used as a background
    //! @param theImageMode the fill type used for a background image
    static void setImage (const Standard_CString theImageFileName, const Aspect_FillMethod theImageMode)
    {
      const Handle (V3d_View)& aCurrentView = ViewerTest::CurrentView();
      aCurrentView->SetBackgroundImage (theImageFileName, theImageMode, Standard_True);
    }

    //! Sets the fill type used for a background image
    //! @param theImageMode the fill type used for a background image
    static void setImageMode (const Aspect_FillMethod theImageMode)
    {
      const Handle (V3d_View)& aCurrentView = ViewerTest::CurrentView();
      aCurrentView->SetBgImageStyle (theImageMode, Standard_True);
    }

    //! Sets the gradient filling for a background
    //! @param theColor1 the gradient starting color
    //! @param theColor2 the gradient ending color
    //! @param theGradientMode the fill method used for a background gradient filling
    static void setGradient (const Quantity_Color&           theColor1,
                             const Quantity_Color&           theColor2,
                             const Aspect_GradientFillMethod theGradientMode)
    {
      const Handle (V3d_View)& aCurrentView = ViewerTest::CurrentView();
      aCurrentView->SetBgGradientColors (theColor1, theColor2, theGradientMode, Standard_True);
    }

    //! Sets the fill method used for a background gradient filling
    //! @param theGradientMode the fill method used for a background gradient filling
    static void setGradientMode (const Aspect_GradientFillMethod theGradientMode)
    {
      const Handle (V3d_View)& aCurrentView = ViewerTest::CurrentView();
      aCurrentView->SetBgGradientStyle (theGradientMode, Standard_True);
    }

    //! Sets the color used for filling a background
    //! @param theColor the color used for filling a background
    static void setColor (const Quantity_Color& theColor)
    {
      const Handle (V3d_View)& aCurrentView = ViewerTest::CurrentView();
      aCurrentView->SetBgGradientStyle (Aspect_GFM_NONE);
      aCurrentView->SetBackgroundColor (theColor);
      aCurrentView->Update();
    }

    //! Sets the gradient filling for a background in a default viewer
    //! @param theColor1 the gradient starting color
    //! @param theColor2 the gradient ending color
    //! @param theGradientMode the fill method used for a background gradient filling
    static void setDefaultGradient (const Quantity_Color&           theColor1,
                                    const Quantity_Color&           theColor2,
                                    const Aspect_GradientFillMethod theGradientMode)
    {
      ViewerTest_DefaultBackground.GradientColor1 = theColor1;
      ViewerTest_DefaultBackground.GradientColor2 = theColor2;
      ViewerTest_DefaultBackground.FillMethod     = theGradientMode;
      setDefaultGradient();
    }

    //! Sets the color used for filling a background in a default viewer
    //! @param theColor the color used for filling a background
    static void setDefaultColor (const Quantity_Color& theColor)
    {
      ViewerTest_DefaultBackground.GradientColor1 = Quantity_Color();
      ViewerTest_DefaultBackground.GradientColor2 = Quantity_Color();
      ViewerTest_DefaultBackground.FillMethod     = Aspect_GFM_NONE;
      ViewerTest_DefaultBackground.FlatColor      = theColor;
      setDefaultGradient();
      setDefaultColor();
    }

    //! Sets the gradient filling for a background in a default viewer.
    //! Gradient settings are taken from ViewerTest_DefaultBackground structure
    static void setDefaultGradient()
    {
      for (NCollection_DoubleMap<TCollection_AsciiString, Handle (AIS_InteractiveContext)>::Iterator
             anInteractiveContextIterator (ViewerTest_myContexts);
           anInteractiveContextIterator.More();
           anInteractiveContextIterator.Next())
      {
        const Handle (V3d_Viewer)& aViewer = anInteractiveContextIterator.Value()->CurrentViewer();
        aViewer->SetDefaultBgGradientColors (ViewerTest_DefaultBackground.GradientColor1,
                                             ViewerTest_DefaultBackground.GradientColor2,
                                             ViewerTest_DefaultBackground.FillMethod);
      }
    }

    //! Sets the color used for filling a background in a default viewer.
    //! The color value is taken from ViewerTest_DefaultBackground structure
    static void setDefaultColor()
    {
      for (NCollection_DoubleMap<TCollection_AsciiString, Handle (AIS_InteractiveContext)>::Iterator
             anInteractiveContextIterator (ViewerTest_myContexts);
           anInteractiveContextIterator.More();
           anInteractiveContextIterator.Next())
      {
        const Handle (V3d_Viewer)& aViewer = anInteractiveContextIterator.Value()->CurrentViewer();
        aViewer->SetDefaultBackgroundColor (ViewerTest_DefaultBackground.FlatColor);
      }
    }
  };

} // namespace

//==============================================================================

#ifdef _WIN32
static LRESULT WINAPI ViewerWindowProc(
                                       HWND hwnd,
                                       UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam );
static LRESULT WINAPI AdvViewerWindowProc(
  HWND hwnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam );
#endif


//==============================================================================
//function : WClass
//purpose  :
//==============================================================================

const Handle(WNT_WClass)& ViewerTest::WClass()
{
  static Handle(WNT_WClass) theWClass;
#if defined(_WIN32)
  if (theWClass.IsNull())
  {
    theWClass = new WNT_WClass ("GW3D_Class", (Standard_Address )AdvViewerWindowProc,
                                CS_VREDRAW | CS_HREDRAW, 0, 0,
                                ::LoadCursor (NULL, IDC_ARROW));
  }
#endif
  return theWClass;
}

//==============================================================================
//function : CreateName
//purpose  : Create numerical name for new object in theMap
//==============================================================================
template <typename ObjectType>
TCollection_AsciiString CreateName (const NCollection_DoubleMap <TCollection_AsciiString, ObjectType>& theObjectMap,
                                    const TCollection_AsciiString& theDefaultString)
{
  if (theObjectMap.IsEmpty())
    return theDefaultString + TCollection_AsciiString(1);

  Standard_Integer aNextKey = 1;
  Standard_Boolean isFound = Standard_False;
  while (!isFound)
  {
    TCollection_AsciiString aStringKey = theDefaultString + TCollection_AsciiString(aNextKey);
    // Look for objects with default names
    if (theObjectMap.IsBound1(aStringKey))
    {
      aNextKey++;
    }
    else
      isFound = Standard_True;
  }

  return theDefaultString + TCollection_AsciiString(aNextKey);
}

//==============================================================================
//structure : ViewerTest_Names
//purpose   : Allow to operate with full view name: driverName/viewerName/viewName
//==============================================================================
struct ViewerTest_Names
{
private:
  TCollection_AsciiString myDriverName;
  TCollection_AsciiString myViewerName;
  TCollection_AsciiString myViewName;

public:

  const TCollection_AsciiString& GetDriverName () const
  {
    return myDriverName;
  }
  void SetDriverName (const TCollection_AsciiString& theDriverName)
  {
    myDriverName = theDriverName;
  }
  const TCollection_AsciiString& GetViewerName () const
  {
    return myViewerName;
  }
  void SetViewerName (const TCollection_AsciiString& theViewerName)
  {
    myViewerName = theViewerName;
  }
  const TCollection_AsciiString& GetViewName () const
  {
    return myViewName;
  }
  void SetViewName (const TCollection_AsciiString& theViewName)
  {
    myViewName = theViewName;
  }

  //===========================================================================
  //function : Constructor for ViewerTest_Names
  //purpose  : Get view, viewer, driver names from custom string
  //===========================================================================

  ViewerTest_Names (const TCollection_AsciiString& theInputString)
  {
    TCollection_AsciiString aName(theInputString);
    if (theInputString.IsEmpty())
    {
      // Get current configuration
      if (ViewerTest_myDrivers.IsEmpty())
        myDriverName = CreateName<Handle(Graphic3d_GraphicDriver)>
          (ViewerTest_myDrivers, TCollection_AsciiString("Driver"));
      else
        myDriverName = ViewerTest_myDrivers.Find2
        (ViewerTest::GetAISContext()->CurrentViewer()->Driver());

      if(ViewerTest_myContexts.IsEmpty())
      {
        myViewerName = CreateName <Handle(AIS_InteractiveContext)>
          (ViewerTest_myContexts, TCollection_AsciiString (myDriverName + "/Viewer"));
      }
      else
      {
        myViewerName = ViewerTest_myContexts.Find2 (ViewerTest::GetAISContext());
      }

      myViewName = CreateName <Handle(V3d_View)> (ViewerTest_myViews, TCollection_AsciiString(myViewerName + "/View"));
    }
    else
    {
      // There is at least view name
      Standard_Integer aParserNumber = 0;
      for (Standard_Integer i = 0; i < 3; ++i)
      {
        Standard_Integer aParserPos = aName.SearchFromEnd("/");
        if(aParserPos != -1)
        {
          aParserNumber++;
          aName.Split(aParserPos-1);
        }
        else
          break;
      }
      if (aParserNumber == 0)
      {
        // Only view name
        if (!ViewerTest::GetAISContext().IsNull())
        {
          myDriverName = ViewerTest_myDrivers.Find2
          (ViewerTest::GetAISContext()->CurrentViewer()->Driver());
          myViewerName = ViewerTest_myContexts.Find2
          (ViewerTest::GetAISContext());
        }
        else
        {
          // There is no opened contexts here, need to create names for viewer and driver
          myDriverName = CreateName<Handle(Graphic3d_GraphicDriver)>
            (ViewerTest_myDrivers, TCollection_AsciiString("Driver"));

          myViewerName = CreateName <Handle(AIS_InteractiveContext)>
            (ViewerTest_myContexts, TCollection_AsciiString (myDriverName + "/Viewer"));
        }
        myViewName = TCollection_AsciiString(myViewerName + "/" + theInputString);
      }
      else if (aParserNumber == 1)
      {
        // Here is viewerName/viewName
        if (!ViewerTest::GetAISContext().IsNull())
          myDriverName = ViewerTest_myDrivers.Find2
          (ViewerTest::GetAISContext()->CurrentViewer()->Driver());
        else
        {
          // There is no opened contexts here, need to create name for driver
          myDriverName = CreateName<Handle(Graphic3d_GraphicDriver)>
            (ViewerTest_myDrivers, TCollection_AsciiString("Driver"));
        }
        myViewerName = TCollection_AsciiString(myDriverName + "/" + aName);

        myViewName = TCollection_AsciiString(myDriverName + "/" + theInputString);
      }
      else
      {
        //Here is driverName/viewerName/viewName
        myDriverName = TCollection_AsciiString(aName);

        TCollection_AsciiString aViewerName(theInputString);
        aViewerName.Split(aViewerName.SearchFromEnd("/") - 1);
        myViewerName = TCollection_AsciiString(aViewerName);

        myViewName = TCollection_AsciiString(theInputString);
      }
    }
  }
};

//==============================================================================
//function : FindContextByView
//purpose  : Find AIS_InteractiveContext by View
//==============================================================================

Handle(AIS_InteractiveContext) FindContextByView (const Handle(V3d_View)& theView)
{
  Handle(AIS_InteractiveContext) anAISContext;

  for (NCollection_DoubleMap<TCollection_AsciiString, Handle(AIS_InteractiveContext)>::Iterator
       anIter (ViewerTest_myContexts); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->CurrentViewer() == theView->Viewer())
       return anIter.Key2();
  }
  return anAISContext;
}

//==============================================================================
//function : IsWindowOverlapped
//purpose  : Check if theWindow overlapp another view
//==============================================================================

Standard_Boolean IsWindowOverlapped (const Standard_Integer thePxLeft,
                                     const Standard_Integer thePxTop,
                                     const Standard_Integer thePxRight,
                                     const Standard_Integer thePxBottom,
                                     TCollection_AsciiString& theViewId)
{
  for(NCollection_DoubleMap <TCollection_AsciiString, Handle(V3d_View)>::Iterator
      anIter(ViewerTest_myViews); anIter.More(); anIter.Next())
  {
    Standard_Integer aTop = 0,
      aLeft = 0,
      aRight = 0,
      aBottom = 0;
    anIter.Value()->Window()->Position(aLeft, aTop, aRight, aBottom);
    if ((thePxLeft >= aLeft && thePxLeft <= aRight && thePxTop >= aTop && thePxTop <= aBottom) ||
        (thePxLeft >= aLeft && thePxLeft <= aRight && thePxBottom >= aTop && thePxBottom <= aBottom) ||
        (thePxRight >= aLeft && thePxRight <= aRight && thePxTop >= aTop && thePxTop <= aBottom) ||
        (thePxRight >= aLeft && thePxRight <= aRight && thePxBottom >= aTop && thePxBottom <= aBottom))
    {
      theViewId = anIter.Key1();
      return Standard_True;
    }
  }
  return Standard_False;
}

// Workaround: to create and delete non-orthographic views outside ViewerTest
void ViewerTest::RemoveViewName (const TCollection_AsciiString& theName)
{
  ViewerTest_myViews.UnBind1 (theName);
}

void ViewerTest::InitViewName (const TCollection_AsciiString& theName,
                               const Handle(V3d_View)& theView)
{
  ViewerTest_myViews.Bind (theName, theView);
}

TCollection_AsciiString ViewerTest::GetCurrentViewName ()
{
  return ViewerTest_myViews.Find2( ViewerTest::CurrentView());
}

//==============================================================================
//function : ViewerInit
//purpose  : Create the window viewer and initialize all the global variable
//==============================================================================

TCollection_AsciiString ViewerTest::ViewerInit (const Standard_Integer thePxLeft,
                                                const Standard_Integer thePxTop,
                                                const Standard_Integer thePxWidth,
                                                const Standard_Integer thePxHeight,
                                                const TCollection_AsciiString& theViewName,
                                                const TCollection_AsciiString& theDisplayName,
                                                const Handle(V3d_View)& theViewToClone)
{
  // Default position and dimension of the viewer window.
  // Note that left top corner is set to be sufficiently small to have
  // window fit in the small screens (actual for remote desktops, see #23003).
  // The position corresponds to the window's client area, thus some
  // gap is added for window frame to be visible.
  Standard_Integer aPxLeft   = 20;
  Standard_Integer aPxTop    = 40;
  Standard_Integer aPxWidth  = 409;
  Standard_Integer aPxHeight = 409;
  Standard_Boolean toCreateViewer = Standard_False;
  if (!theViewToClone.IsNull())
  {
    theViewToClone->Window()->Size (aPxWidth, aPxHeight);
  }

  Handle(OpenGl_GraphicDriver) aGraphicDriver;
  ViewerTest_Names aViewNames(theViewName);
  if (ViewerTest_myViews.IsBound1 (aViewNames.GetViewName ()))
    aViewNames.SetViewName (aViewNames.GetViewerName() + "/" + CreateName<Handle(V3d_View)>(ViewerTest_myViews, "View"));

  if (thePxLeft != 0)
    aPxLeft = thePxLeft;
  if (thePxTop != 0)
    aPxTop = thePxTop;
  if (thePxWidth != 0)
    aPxWidth = thePxWidth;
  if (thePxHeight != 0)
    aPxHeight = thePxHeight;

  // Get graphic driver (create it or get from another view)
  const bool isNewDriver = !ViewerTest_myDrivers.IsBound1 (aViewNames.GetDriverName());
  if (isNewDriver)
  {
    // Get connection string
  #if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
    if (!theDisplayName.IsEmpty())
    {
      SetDisplayConnection (new Aspect_DisplayConnection (theDisplayName));
    }
    else
    {
      ::Display* aDispX = NULL;
      // create dedicated display connection instead of reusing Tk connection
      // so that to procede events independently through VProcessEvents()/ViewerMainLoop() callbacks
      /*Draw_Interpretor& aCommands = Draw::GetInterpretor();
      Tcl_Interp* aTclInterp = aCommands.Interp();
      Tk_Window aMainWindow = Tk_MainWindow (aTclInterp);
      aDispX = aMainWindow != NULL ? Tk_Display (aMainWindow) : NULL;*/
      SetDisplayConnection (new Aspect_DisplayConnection (aDispX));
    }
  #else
    (void)theDisplayName; // avoid warning on unused argument
    SetDisplayConnection (new Aspect_DisplayConnection ());
  #endif

    if (Draw_VirtualWindows)
    {
      // don't waste the time waiting for VSync when window is not displayed on the screen
      ViewerTest_myDefaultCaps.swapInterval = 0;
      // alternatively we can disable buffer swap at all, but this might be inappropriate for testing
      //ViewerTest_myDefaultCaps.buffersNoSwap = true;
    }
    aGraphicDriver = new OpenGl_GraphicDriver (GetDisplayConnection(), false);
    aGraphicDriver->ChangeOptions() = ViewerTest_myDefaultCaps;
    aGraphicDriver->InitContext();

    ViewerTest_myDrivers.Bind (aViewNames.GetDriverName(), aGraphicDriver);
    toCreateViewer = Standard_True;
  }
  else
  {
    aGraphicDriver = Handle(OpenGl_GraphicDriver)::DownCast (ViewerTest_myDrivers.Find1 (aViewNames.GetDriverName()));
  }

  //Dispose the window if input parameters are default
  if (!ViewerTest_myViews.IsEmpty() && thePxLeft == 0 && thePxTop == 0)
  {
    Standard_Integer aTop = 0,
                     aLeft = 0,
                     aRight = 0,
                     aBottom = 0,
                     aScreenWidth = 0,
                     aScreenHeight = 0;

    // Get screen resolution
#if defined(_WIN32) || defined(__WIN32__)
    RECT aWindowSize;
    GetClientRect(GetDesktopWindow(), &aWindowSize);
    aScreenHeight = aWindowSize.bottom;
    aScreenWidth = aWindowSize.right;
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
    GetCocoaScreenResolution (aScreenWidth, aScreenHeight);
#else
    Screen *aScreen = DefaultScreenOfDisplay(GetDisplayConnection()->GetDisplay());
    aScreenWidth = WidthOfScreen(aScreen);
    aScreenHeight = HeightOfScreen(aScreen);
#endif

    TCollection_AsciiString anOverlappedViewId("");

    while (IsWindowOverlapped (aPxLeft, aPxTop, aPxLeft + aPxWidth, aPxTop + aPxHeight, anOverlappedViewId))
    {
      ViewerTest_myViews.Find1(anOverlappedViewId)->Window()->Position (aLeft, aTop, aRight, aBottom);

      if (IsWindowOverlapped (aRight + 20, aPxTop, aRight + 20 + aPxWidth, aPxTop + aPxHeight, anOverlappedViewId)
        && aRight + 2*aPxWidth + 40 > aScreenWidth)
      {
        if (aBottom + aPxHeight + 40 > aScreenHeight)
        {
          aPxLeft = 20;
          aPxTop = 40;
          break;
        }
        aPxLeft = 20;
        aPxTop = aBottom + 40;
      }
      else
        aPxLeft = aRight + 20;
    }
  }

  // Get viewer name
  TCollection_AsciiString aTitle("3D View - ");
  aTitle = aTitle + aViewNames.GetViewName() + "(*)";

  // Change name of current active window
  if (const Handle(V3d_View)& aCurrentView = ViewerTest::CurrentView())
  {
    aCurrentView->Window()->SetTitle (TCollection_AsciiString ("3D View - ") + ViewerTest_myViews.Find2 (aCurrentView));
  }

  // Create viewer
  Handle(V3d_Viewer) a3DViewer;
  // If it's the single view, we first look for empty context
  if (ViewerTest_myViews.IsEmpty() && !ViewerTest_myContexts.IsEmpty())
  {
    NCollection_DoubleMap <TCollection_AsciiString, Handle(AIS_InteractiveContext)>::Iterator
      anIter(ViewerTest_myContexts);
    if (anIter.More())
      ViewerTest::SetAISContext (anIter.Value());
    a3DViewer = ViewerTest::GetAISContext()->CurrentViewer();
  }
  else if (ViewerTest_myContexts.IsBound1(aViewNames.GetViewerName()))
  {
    ViewerTest::SetAISContext(ViewerTest_myContexts.Find1(aViewNames.GetViewerName()));
    a3DViewer = ViewerTest::GetAISContext()->CurrentViewer();
  }
  else if (a3DViewer.IsNull())
  {
    toCreateViewer = Standard_True;
    a3DViewer = new V3d_Viewer(aGraphicDriver);
    a3DViewer->SetDefaultBackgroundColor (ViewerTest_DefaultBackground.FlatColor);
    a3DViewer->SetDefaultBgGradientColors (ViewerTest_DefaultBackground.GradientColor1,
                                           ViewerTest_DefaultBackground.GradientColor2,
                                           ViewerTest_DefaultBackground.FillMethod);
  }

  // AIS context setup
  if (ViewerTest::GetAISContext().IsNull() ||
      !(ViewerTest_myContexts.IsBound1(aViewNames.GetViewerName())))
  {
    Handle(AIS_InteractiveContext) aContext = new AIS_InteractiveContext (a3DViewer);
    ViewerTest::SetAISContext (aContext);
    ViewerTest_myContexts.Bind (aViewNames.GetViewerName(), ViewerTest::GetAISContext());
  }
  else
  {
    ViewerTest::ResetEventManager();
  }

  // Create window
#if defined(_WIN32)
  VT_GetWindow() = new WNT_Window (aTitle.ToCString(), WClass(),
                                    Draw_VirtualWindows ? WS_POPUP : WS_OVERLAPPEDWINDOW,
                                    aPxLeft, aPxTop,
                                    aPxWidth, aPxHeight,
                                    Quantity_NOC_BLACK);
  VT_GetWindow()->RegisterRawInputDevices (WNT_Window::RawInputMask_SpaceMouse);
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  VT_GetWindow() = new Cocoa_Window (aTitle.ToCString(),
                                     aPxLeft, aPxTop,
                                     aPxWidth, aPxHeight);
  ViewerTest_SetCocoaEventManagerView (VT_GetWindow());
#else
  VT_GetWindow() = new Xw_Window (aGraphicDriver->GetDisplayConnection(),
                                  aTitle.ToCString(),
                                  aPxLeft, aPxTop,
                                  aPxWidth, aPxHeight);
#endif
  VT_GetWindow()->SetVirtual (Draw_VirtualWindows);

  // View setup
  Handle(V3d_View) aView;
  if (!theViewToClone.IsNull())
  {
    aView = new ViewerTest_V3dView (a3DViewer, theViewToClone);
  }
  else
  {
    aView = new ViewerTest_V3dView (a3DViewer, a3DViewer->DefaultTypeOfView());
  }

  aView->SetWindow (VT_GetWindow());
  ViewerTest::GetAISContext()->RedrawImmediate (a3DViewer);

  ViewerTest::CurrentView(aView);
  ViewerTest_myViews.Bind (aViewNames.GetViewName(), aView);

  // Setup for X11 or NT
  OSWindowSetup();

  // Set parameters for V3d_View and V3d_Viewer
  const Handle (V3d_View) aV3dView = ViewerTest::CurrentView();
  aV3dView->SetComputedMode(Standard_False);

  a3DViewer->SetDefaultBackgroundColor(Quantity_NOC_BLACK);
  if (toCreateViewer)
  {
    a3DViewer->SetDefaultLights();
    a3DViewer->SetLightOn();
  }

#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  if (isNewDriver)
  {
    ::Display* aDispX = GetDisplayConnection()->GetDisplay();
    Tcl_CreateFileHandler (XConnectionNumber (aDispX), TCL_READABLE, VProcessEvents, (ClientData )aDispX);
  }
#endif

  VT_GetWindow()->Map();

  // Set the handle of created view in the event manager
  ViewerTest::ResetEventManager();

  ViewerTest::CurrentView()->Redraw();

  aView.Nullify();
  a3DViewer.Nullify();

  return aViewNames.GetViewName();
}

//==============================================================================
//function : RedrawAllViews
//purpose  : Redraw all created views
//==============================================================================
void ViewerTest::RedrawAllViews()
{
  NCollection_DoubleMap<TCollection_AsciiString, Handle(V3d_View)>::Iterator aViewIt(ViewerTest_myViews);
  for (; aViewIt.More(); aViewIt.Next())
  {
    const Handle(V3d_View)& aView = aViewIt.Key2();
    aView->Redraw();
  }
}

//==============================================================================
//function : Vinit
//purpose  : Create the window viewer and initialize all the global variable
//    Use Tk_CreateFileHandler on UNIX to catch the X11 Viewer event
//==============================================================================

static int VInit (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  TCollection_AsciiString aViewName, aDisplayName;
  Standard_Integer aPxLeft = 0, aPxTop = 0, aPxWidth = 0, aPxHeight = 0;
  Handle(V3d_View) aCopyFrom;
  TCollection_AsciiString aName, aValue;
  int is2dMode = -1;
  for (Standard_Integer anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    const TCollection_AsciiString anArg = theArgVec[anArgIt];
    TCollection_AsciiString anArgCase = anArg;
    anArgCase.LowerCase();
    if (anArgIt + 1 < theArgsNb
     && anArgCase == "-name")
    {
      aViewName = theArgVec[++anArgIt];
    }
    else if (anArgIt + 1 < theArgsNb
          && (anArgCase == "-left"
           || anArgCase == "-l"))
    {
      aPxLeft = Draw::Atoi (theArgVec[++anArgIt]);
    }
    else if (anArgIt + 1 < theArgsNb
          && (anArgCase == "-top"
           || anArgCase == "-t"))
    {
      aPxTop = Draw::Atoi (theArgVec[++anArgIt]);
    }
    else if (anArgIt + 1 < theArgsNb
          && (anArgCase == "-width"
           || anArgCase == "-w"))
    {
      aPxWidth = Draw::Atoi (theArgVec[++anArgIt]);
    }
    else if (anArgIt + 1 < theArgsNb
          && (anArgCase == "-height"
           || anArgCase == "-h"))
    {
      aPxHeight = Draw::Atoi (theArgVec[++anArgIt]);
    }
    else if (anArgCase == "-exitonclose")
    {
      ViewerTest_EventManager::ToExitOnCloseView() = true;
      if (anArgIt + 1 < theArgsNb
       && Draw::ParseOnOff (theArgVec[anArgIt + 1], ViewerTest_EventManager::ToExitOnCloseView()))
      {
        ++anArgIt;
      }
    }
    else if (anArgCase == "-closeonescape"
          || anArgCase == "-closeonesc")
    {
      ViewerTest_EventManager::ToCloseViewOnEscape() = true;
      if (anArgIt + 1 < theArgsNb
       && Draw::ParseOnOff (theArgVec[anArgIt + 1], ViewerTest_EventManager::ToCloseViewOnEscape()))
      {
        ++anArgIt;
      }
    }
    else if (anArgCase == "-2d_mode"
          || anArgCase == "-2dmode"
          || anArgCase == "-2d")
    {
      bool toEnable = true;
      if (anArgIt + 1 < theArgsNb
       && Draw::ParseOnOff (theArgVec[anArgIt + 1], toEnable))
      {
        ++anArgIt;
      }
      is2dMode = toEnable ? 1 : 0;
    }
    else if (anArgIt + 1 < theArgsNb
          && (anArgCase == "-disp"
           || anArgCase == "-display"))
    {
      aDisplayName = theArgVec[++anArgIt];
    }
    else if (!ViewerTest::CurrentView().IsNull()
          &&  aCopyFrom.IsNull()
          && (anArgCase == "-copy"
           || anArgCase == "-clone"
           || anArgCase == "-cloneactive"
           || anArgCase == "-cloneactiveview"))
    {
      aCopyFrom = ViewerTest::CurrentView();
    }
    // old syntax
    else if (ViewerTest::SplitParameter (anArg, aName, aValue))
    {
      aName.LowerCase();
      if (aName == "name")
      {
        aViewName = aValue;
      }
      else if (aName == "l"
            || aName == "left")
      {
        aPxLeft = aValue.IntegerValue();
      }
      else if (aName == "t"
            || aName == "top")
      {
        aPxTop = aValue.IntegerValue();
      }
      else if (aName == "disp"
            || aName == "display")
      {
        aDisplayName = aValue;
      }
      else if (aName == "w"
            || aName == "width")
      {
        aPxWidth = aValue.IntegerValue();
      }
      else if (aName == "h"
            || aName == "height")
      {
        aPxHeight = aValue.IntegerValue();
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown argument " << anArg;
        return 1;
      }
    }
    else if (aViewName.IsEmpty())
    {
      aViewName = anArg;
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument " << anArg;
      return 1;
    }
  }

#if defined(_WIN32) || (defined(__APPLE__) && !defined(MACOSX_USE_GLX))
  if (!aDisplayName.IsEmpty())
  {
    aDisplayName.Clear();
    Message::SendWarning() << "Warning: display parameter will be ignored.\n";
  }
#endif

  ViewerTest_Names aViewNames (aViewName);
  if (ViewerTest_myViews.IsBound1 (aViewNames.GetViewName()))
  {
    TCollection_AsciiString aCommand = TCollection_AsciiString ("vactivate ") + aViewNames.GetViewName();
    theDi.Eval (aCommand.ToCString());
    if (is2dMode != -1)
    {
      ViewerTest_V3dView::SetCurrentView2DMode (is2dMode == 1);
    }
    return 0;
  }

  TCollection_AsciiString aViewId = ViewerTest::ViewerInit (aPxLeft, aPxTop, aPxWidth, aPxHeight,
                                                            aViewName, aDisplayName, aCopyFrom);
  if (is2dMode != -1)
  {
    ViewerTest_V3dView::SetCurrentView2DMode (is2dMode == 1);
  }
  theDi << aViewId;
  return 0;
}

//! Parse HLR algo type.
static Standard_Boolean parseHlrAlgoType (const char* theName,
                                          Prs3d_TypeOfHLR& theType)
{
  TCollection_AsciiString aName (theName);
  aName.LowerCase();
  if (aName == "polyalgo")
  {
    theType = Prs3d_TOH_PolyAlgo;
  }
  else if (aName == "algo")
  {
    theType = Prs3d_TOH_Algo;
  }
  else
  {
    return Standard_False;
  }
  return Standard_True;
}

//==============================================================================
//function : VHLR
//purpose  : hidden lines removal algorithm
//==============================================================================

static int VHLR (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  const Handle(V3d_View) aView = ViewerTest::CurrentView();
  const Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Standard_Boolean hasHlrOnArg = Standard_False;
  Standard_Boolean hasShowHiddenArg = Standard_False;
  Standard_Boolean isHLROn = Standard_False;
  Standard_Boolean toShowHidden = aCtx->DefaultDrawer()->DrawHiddenLine();
  Prs3d_TypeOfHLR  aTypeOfHLR = Prs3d_TOH_NotSet;
  ViewerTest_AutoUpdater anUpdateTool (Handle(AIS_InteractiveContext)(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < argc; ++anArgIter)
  {
    TCollection_AsciiString anArg (argv[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      continue;
    }
    else if (anArg == "-showhidden"
          && anArgIter + 1 < argc
          && Draw::ParseOnOff (argv[anArgIter + 1], toShowHidden))
    {
      ++anArgIter;
      hasShowHiddenArg = Standard_True;
      continue;
    }
    else if ((anArg == "-type"
           || anArg == "-algo"
           || anArg == "-algotype")
          && anArgIter + 1 < argc
          && parseHlrAlgoType (argv[anArgIter + 1], aTypeOfHLR))
    {
      ++anArgIter;
      continue;
    }
    else if (!hasHlrOnArg
          && Draw::ParseOnOff (argv[anArgIter], isHLROn))
    {
      hasHlrOnArg = Standard_True;
      continue;
    }
    // old syntax
    else if (!hasShowHiddenArg
          && Draw::ParseOnOff(argv[anArgIter], toShowHidden))
    {
      hasShowHiddenArg = Standard_True;
      continue;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << argv[anArgIter] << "'";
      return 1;
    }
  }
  if (!hasHlrOnArg)
  {
    di << "HLR:        " << aView->ComputedMode() << "\n";
    di << "HiddenLine: " << aCtx->DefaultDrawer()->DrawHiddenLine() << "\n";
    di << "HlrAlgo:    ";
    switch (aCtx->DefaultDrawer()->TypeOfHLR())
    {
      case Prs3d_TOH_NotSet:   di << "NotSet\n";   break;
      case Prs3d_TOH_PolyAlgo: di << "PolyAlgo\n"; break;
      case Prs3d_TOH_Algo:     di << "Algo\n";     break;
    }
    anUpdateTool.Invalidate();
    return 0;
  }

  Standard_Boolean toRecompute = Standard_False;
  if (aTypeOfHLR != Prs3d_TOH_NotSet
   && aTypeOfHLR != aCtx->DefaultDrawer()->TypeOfHLR())
  {
    toRecompute = Standard_True;
    aCtx->DefaultDrawer()->SetTypeOfHLR (aTypeOfHLR);
  }
  if (toShowHidden != aCtx->DefaultDrawer()->DrawHiddenLine())
  {
    toRecompute = Standard_True;
    if (toShowHidden)
    {
      aCtx->DefaultDrawer()->EnableDrawHiddenLine();
    }
    else
    {
      aCtx->DefaultDrawer()->DisableDrawHiddenLine();
    }
  }

  // redisplay shapes
  if (aView->ComputedMode() && isHLROn && toRecompute)
  {
    AIS_ListOfInteractive aListOfShapes;
    aCtx->DisplayedObjects (aListOfShapes);
    for (AIS_ListIteratorOfListOfInteractive anIter (aListOfShapes); anIter.More(); anIter.Next())
    {
      if (Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(anIter.Value()))
      {
        aCtx->Redisplay (aShape, Standard_False);
      }
    }
  }

  aView->SetComputedMode (isHLROn);
  return 0;
}

//==============================================================================
//function : VHLRType
//purpose  : change type of using HLR algorithm
//==============================================================================

static int VHLRType (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  const Handle(V3d_View) aView = ViewerTest::CurrentView();
  const Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Prs3d_TypeOfHLR aTypeOfHLR = Prs3d_TOH_NotSet;
  ViewerTest_AutoUpdater anUpdateTool (Handle(AIS_InteractiveContext)(), aView);
  AIS_ListOfInteractive aListOfShapes;
  for (Standard_Integer anArgIter = 1; anArgIter < argc; ++anArgIter)
  {
    TCollection_AsciiString anArg (argv[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      continue;
    }
    else if ((anArg == "-type"
           || anArg == "-algo"
           || anArg == "-algotype")
          && anArgIter + 1 < argc
          && parseHlrAlgoType (argv[anArgIter + 1], aTypeOfHLR))
    {
      ++anArgIter;
      continue;
    }
    // old syntax
    else if (aTypeOfHLR == Prs3d_TOH_NotSet
          && parseHlrAlgoType (argv[anArgIter], aTypeOfHLR))
    {
      continue;
    }
    else
    {
      ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
      TCollection_AsciiString aName (argv[anArgIter]);
      if (!aMap.IsBound2 (aName))
      {
        Message::SendFail() << "Syntax error: Wrong shape name '" << aName << "'";
        return 1;
      }

      Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (aMap.Find2 (aName));
      if (aShape.IsNull())
      {
        Message::SendFail() << "Syntax error: '" << aName << "' is not a shape presentation";
        return 1;
      }
      aListOfShapes.Append (aShape);
      continue;
    }
  }
  if (aTypeOfHLR == Prs3d_TOH_NotSet)
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }

  const Standard_Boolean isGlobal = aListOfShapes.IsEmpty();
  if (isGlobal)
  {
    aCtx->DisplayedObjects (aListOfShapes);
    aCtx->DefaultDrawer()->SetTypeOfHLR (aTypeOfHLR);
  }

  for (AIS_ListIteratorOfListOfInteractive anIter(aListOfShapes); anIter.More(); anIter.Next())
  {
    Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(anIter.Value());
    if (aShape.IsNull())
    {
      continue;
    }

    const bool toUpdateShape = aShape->TypeOfHLR() != aTypeOfHLR
                            && aView->ComputedMode();
    if (!isGlobal
     || aShape->TypeOfHLR() != aTypeOfHLR)
    {
      aShape->SetTypeOfHLR (aTypeOfHLR);
    }
    if (toUpdateShape)
    {
      aCtx->Redisplay (aShape, Standard_False);
    }
  }
  return 0;
}

//==============================================================================
//function : FindViewIdByWindowHandle
//purpose  : Find theView Id in the map of views by window handle
//==============================================================================
#if defined(_WIN32) || (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
TCollection_AsciiString FindViewIdByWindowHandle (Aspect_Drawable theWindowHandle)
{
  for (NCollection_DoubleMap<TCollection_AsciiString, Handle(V3d_View)>::Iterator
       anIter(ViewerTest_myViews); anIter.More(); anIter.Next())
  {
    Aspect_Drawable aWindowHandle = anIter.Value()->Window()->NativeHandle();
    if (aWindowHandle == theWindowHandle)
      return anIter.Key1();
  }
  return TCollection_AsciiString("");
}
#endif

//! Make the view active
void ActivateView (const TCollection_AsciiString& theViewName,
                   Standard_Boolean theToUpdate = Standard_True)
{
  const Handle(V3d_View) aView = ViewerTest_myViews.Find1(theViewName);
  if (aView.IsNull())
  {
    return;
  }

  Handle(AIS_InteractiveContext) anAISContext = FindContextByView(aView);
  if (!anAISContext.IsNull())
  {
    if (const Handle(V3d_View)& aCurrentView = ViewerTest::CurrentView())
    {
      aCurrentView->Window()->SetTitle (TCollection_AsciiString ("3D View - ") + ViewerTest_myViews.Find2 (aCurrentView));
    }

    ViewerTest::CurrentView (aView);
    ViewerTest::SetAISContext (anAISContext);
    aView->Window()->SetTitle (TCollection_AsciiString("3D View - ") + theViewName + "(*)");
#if defined(_WIN32)
    VT_GetWindow() = Handle(WNT_Window)::DownCast(ViewerTest::CurrentView()->Window());
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
    VT_GetWindow() = Handle(Cocoa_Window)::DownCast(ViewerTest::CurrentView()->Window());
#else
    VT_GetWindow() = Handle(Xw_Window)::DownCast(ViewerTest::CurrentView()->Window());
#endif
    SetDisplayConnection(ViewerTest::CurrentView()->Viewer()->Driver()->GetDisplayConnection());
    if (theToUpdate)
    {
      ViewerTest::CurrentView()->Redraw();
    }
  }
}

//==============================================================================
//function : RemoveView
//purpose  :
//==============================================================================
void ViewerTest::RemoveView (const Handle(V3d_View)& theView,
                             const Standard_Boolean  theToRemoveContext)
{
  if (!ViewerTest_myViews.IsBound2 (theView))
  {
    return;
  }

  const TCollection_AsciiString aViewName = ViewerTest_myViews.Find2 (theView);
  RemoveView (aViewName, theToRemoveContext);
}

//==============================================================================
//function : RemoveView
//purpose  : Close and remove view from display, clear maps if neccessary
//==============================================================================
void ViewerTest::RemoveView (const TCollection_AsciiString& theViewName, const Standard_Boolean isContextRemoved)
{
  if (!ViewerTest_myViews.IsBound1(theViewName))
  {
    Message::SendFail() << "Wrong view name";
    return;
  }

  // Activate another view if it's active now
  if (ViewerTest_myViews.Find1(theViewName) == ViewerTest::CurrentView())
  {
    if (ViewerTest_myViews.Extent() > 1)
    {
      TCollection_AsciiString aNewViewName;
      for (NCollection_DoubleMap <TCollection_AsciiString, Handle(V3d_View)>::Iterator anIter (ViewerTest_myViews);
           anIter.More(); anIter.Next())
      {
        if (anIter.Key1() != theViewName)
        {
          aNewViewName = anIter.Key1();
          break;
        }
      }
      ActivateView (aNewViewName);
    }
    else
    {
      VT_GetWindow().Nullify();
      ViewerTest::CurrentView (Handle(V3d_View)());
      if (isContextRemoved)
      {
        Handle(AIS_InteractiveContext) anEmptyContext;
        ViewerTest::SetAISContext(anEmptyContext);
      }
    }
  }

  // Delete view
  Handle(V3d_View) aView = ViewerTest_myViews.Find1(theViewName);
  Handle(AIS_InteractiveContext) aCurrentContext = FindContextByView(aView);
  ViewerTest_ContinuousRedrawer& aRedrawer = ViewerTest_ContinuousRedrawer::Instance();
  aRedrawer.Stop (aView->Window());

  // Remove view resources
  ViewerTest_myViews.UnBind1(theViewName);
  aView->Window()->Unmap();
  aView->Remove();

#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  XFlush (GetDisplayConnection()->GetDisplay());
#endif

  // Keep context opened only if the closed view is last to avoid
  // unused empty contexts
  if (!aCurrentContext.IsNull())
  {
    // Check if there are more difined views in the viewer
    if ((isContextRemoved || ViewerTest_myContexts.Size() != 1)
     && aCurrentContext->CurrentViewer()->DefinedViews().IsEmpty())
    {
      // Remove driver if there is no viewers that use it
      Standard_Boolean isRemoveDriver = Standard_True;
      for(NCollection_DoubleMap<TCollection_AsciiString, Handle(AIS_InteractiveContext)>::Iterator
          anIter(ViewerTest_myContexts); anIter.More(); anIter.Next())
      {
        if (aCurrentContext != anIter.Key2() &&
          aCurrentContext->CurrentViewer()->Driver() == anIter.Value()->CurrentViewer()->Driver())
        {
          isRemoveDriver = Standard_False;
          break;
        }
      }

      aCurrentContext->RemoveAll (Standard_False);
      if(isRemoveDriver)
      {
        ViewerTest_myDrivers.UnBind2 (aCurrentContext->CurrentViewer()->Driver());
      #if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
        Tcl_DeleteFileHandler (XConnectionNumber (aCurrentContext->CurrentViewer()->Driver()->GetDisplayConnection()->GetDisplay()));
      #endif
      }

      ViewerTest_myContexts.UnBind2(aCurrentContext);
    }
  }
  Message::SendInfo() << "3D View - " << theViewName << " was deleted.\n";
  if (ViewerTest_EventManager::ToExitOnCloseView())
  {
    Draw_Interprete ("exit");
  }
}

//==============================================================================
//function : VClose
//purpose  : Remove the view defined by its name
//==============================================================================

static int VClose (Draw_Interpretor& /*theDi*/,
                   Standard_Integer  theArgsNb,
                   const char**      theArgVec)
{
  NCollection_List<TCollection_AsciiString> aViewList;
  if (theArgsNb > 1)
  {
    TCollection_AsciiString anArg (theArgVec[1]);
    anArg.UpperCase();
    if (anArg.IsEqual ("ALL")
     || anArg.IsEqual ("*"))
    {
      for (NCollection_DoubleMap<TCollection_AsciiString, Handle(V3d_View)>::Iterator anIter (ViewerTest_myViews);
           anIter.More(); anIter.Next())
      {
        aViewList.Append (anIter.Key1());
      }
      if (aViewList.IsEmpty())
      {
        std::cout << "No view to close\n";
        return 0;
      }
    }
    else
    {
      ViewerTest_Names aViewName (theArgVec[1]);
      if (!ViewerTest_myViews.IsBound1 (aViewName.GetViewName()))
      {
        Message::SendFail() << "Error: the view with name '" << theArgVec[1] << "' does not exist";
        return 1;
      }
      aViewList.Append (aViewName.GetViewName());
    }
  }
  else
  {
    // close active view
    if (ViewerTest::CurrentView().IsNull())
    {
      Message::SendFail ("Error: no active view");
      return 1;
    }
    aViewList.Append (ViewerTest_myViews.Find2 (ViewerTest::CurrentView()));
  }

  Standard_Boolean toRemoveContext = (theArgsNb != 3 || Draw::Atoi (theArgVec[2]) != 1);
  for (NCollection_List<TCollection_AsciiString>::Iterator anIter(aViewList);
       anIter.More(); anIter.Next())
  {
    ViewerTest::RemoveView (anIter.Value(), toRemoveContext);
  }

  return 0;
}

//==============================================================================
//function : VActivate
//purpose  : Activate the view defined by its ID
//==============================================================================

static int VActivate (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  if (theArgsNb == 1)
  {
    theDi.Eval("vviewlist");
    return 0;
  }

  TCollection_AsciiString aNameString;
  Standard_Boolean toUpdate = Standard_True;
  Standard_Boolean toActivate = Standard_True;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (toUpdate
     && anArg == "-noupdate")
    {
      toUpdate = Standard_False;
    }
    else if (toActivate
          && aNameString.IsEmpty()
          && anArg == "none")
    {
      ViewerTest::CurrentView()->Window()->SetTitle (TCollection_AsciiString ("3D View - ") + ViewerTest_myViews.Find2 (ViewerTest::CurrentView()));
      VT_GetWindow().Nullify();
      ViewerTest::CurrentView (Handle(V3d_View)());
      ViewerTest::ResetEventManager();
      theDi << theArgVec[0] << ": all views are inactive\n";
      toActivate = Standard_False;
    }
    else if (toActivate
          && aNameString.IsEmpty())
    {
      aNameString = theArgVec[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (!toActivate)
  {
    return 0;
  }
  else if (aNameString.IsEmpty())
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }

  // Check if this view exists in the viewer with the driver
  ViewerTest_Names aViewNames (aNameString);
  if (!ViewerTest_myViews.IsBound1(aViewNames.GetViewName()))
  {
    theDi << "Syntax error: wrong view name '" << aNameString << "'\n";
    return 1;
  }

  // Check if it is active already
  if (ViewerTest::CurrentView() == ViewerTest_myViews.Find1(aViewNames.GetViewName()))
  {
    theDi << theArgVec[0] << ": the view is active already\n";
    return 0;
  }

  ActivateView (aViewNames.GetViewName(), toUpdate);
  return 0;
}

//==============================================================================
//function : VViewList
//purpose  : Print current list of views per viewer and graphic driver ID
//           shared between viewers
//==============================================================================

static int VViewList (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  if (theArgsNb > 2)
  {
    theDi << theArgVec[0] << ": Wrong number of command arguments\n"
          << "Usage: " << theArgVec[0] << " name";
    return 1;
  }
  if (ViewerTest_myContexts.Size() < 1)
    return 0;

  Standard_Boolean isTreeView =
    (( theArgsNb==1 ) || ( strcasecmp( theArgVec[1], "long" ) != 0 ));

  if (isTreeView)
  {
    theDi << theArgVec[0] <<":\n";
  }

  for (NCollection_DoubleMap <TCollection_AsciiString, Handle(Graphic3d_GraphicDriver)>::Iterator aDriverIter (ViewerTest_myDrivers);
       aDriverIter.More(); aDriverIter.Next())
  {
    if (isTreeView)
      theDi << aDriverIter.Key1() << ":\n";

    for (NCollection_DoubleMap <TCollection_AsciiString, Handle(AIS_InteractiveContext)>::Iterator
      aContextIter(ViewerTest_myContexts); aContextIter.More(); aContextIter.Next())
    {
      if (aContextIter.Key1().Search(aDriverIter.Key1()) != -1)
      {
        if (isTreeView)
        {
          TCollection_AsciiString aContextName(aContextIter.Key1());
          theDi << " " << aContextName.Split(aDriverIter.Key1().Length() + 1) << ":\n";
        }

        for (NCollection_DoubleMap <TCollection_AsciiString, Handle(V3d_View)>::Iterator aViewIter (ViewerTest_myViews);
             aViewIter.More(); aViewIter.Next())
        {
          if (aViewIter.Key1().Search(aContextIter.Key1()) != -1)
          {
            TCollection_AsciiString aViewName(aViewIter.Key1());
            if (isTreeView)
            {
              if (aViewIter.Value() == ViewerTest::CurrentView())
                theDi << "  " << aViewName.Split(aContextIter.Key1().Length() + 1) << "(*)\n";
              else
                theDi << "  " << aViewName.Split(aContextIter.Key1().Length() + 1) << "\n";
            }
            else
            {
              theDi << aViewName << " ";
            }
          }
        }
      }
    }
  }
  return 0;
}

//==============================================================================
//function : GetMousePosition
//purpose  :
//==============================================================================
void ViewerTest::GetMousePosition (Standard_Integer& theX,
                                   Standard_Integer& theY)
{
  if (Handle(ViewerTest_EventManager) aViewCtrl = ViewerTest::CurrentEventManager())
  {
    theX = aViewCtrl->LastMousePosition().x();
    theY = aViewCtrl->LastMousePosition().y();
  }
}

//==============================================================================
//function : VViewProj
//purpose  : Switch view projection
//==============================================================================
static int VViewProj (Draw_Interpretor& ,
                      Standard_Integer theNbArgs,
                      const char** theArgVec)
{
  static Standard_Boolean isYup = Standard_False;
  const Handle(V3d_View)& aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  TCollection_AsciiString aCmdName (theArgVec[0]);
  Standard_Boolean isGeneralCmd = Standard_False;
  if (aCmdName == "vfront")
  {
    aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_Front : V3d_TypeOfOrientation_Zup_Front, isYup);
  }
  else if (aCmdName == "vback")
  {
    aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_Back : V3d_TypeOfOrientation_Zup_Back, isYup);
  }
  else if (aCmdName == "vtop")
  {
    aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_Top : V3d_TypeOfOrientation_Zup_Top, isYup);
  }
  else if (aCmdName == "vbottom")
  {
    aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_Bottom : V3d_TypeOfOrientation_Zup_Bottom, isYup);
  }
  else if (aCmdName == "vleft")
  {
    aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_Left : V3d_TypeOfOrientation_Zup_Left, isYup);
  }
  else if (aCmdName == "vright")
  {
    aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_Right : V3d_TypeOfOrientation_Zup_Right, isYup);
  }
  else if (aCmdName == "vaxo")
  {
    aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_AxoRight : V3d_TypeOfOrientation_Zup_AxoRight, isYup);
  }
  else
  {
    isGeneralCmd = Standard_True;
    for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
    {
      TCollection_AsciiString anArgCase (theArgVec[anArgIter]);
      anArgCase.LowerCase();
      if (anArgCase == "-zup")
      {
        isYup = Standard_False;
      }
      else if (anArgCase == "-yup")
      {
        isYup = Standard_True;
      }
      else if (anArgCase == "-front"
            || anArgCase == "front"
            || anArgCase == "-f"
            || anArgCase == "f")
      {
        aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_Front : V3d_TypeOfOrientation_Zup_Front, isYup);
      }
      else if (anArgCase == "-back"
            || anArgCase == "back"
            || anArgCase == "-b"
            || anArgCase == "b")
      {
        aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_Back : V3d_TypeOfOrientation_Zup_Back, isYup);
      }
      else if (anArgCase == "-top"
            || anArgCase == "top"
            || anArgCase == "-t"
            || anArgCase == "t")
      {
        aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_Top : V3d_TypeOfOrientation_Zup_Top, isYup);
      }
      else if (anArgCase == "-bottom"
            || anArgCase == "bottom"
            || anArgCase == "-bot"
            || anArgCase == "bot"
            || anArgCase == "-b"
            || anArgCase == "b")
      {
        aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_Bottom : V3d_TypeOfOrientation_Zup_Bottom, isYup);
      }
      else if (anArgCase == "-left"
            || anArgCase == "left"
            || anArgCase == "-l"
            || anArgCase == "l")
      {
        aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_Left : V3d_TypeOfOrientation_Zup_Left, isYup);
      }
      else if (anArgCase == "-right"
            || anArgCase == "right"
            || anArgCase == "-r"
            || anArgCase == "r")
      {
        aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_Right : V3d_TypeOfOrientation_Zup_Right, isYup);
      }
      else if (anArgCase == "-axoleft"
            || anArgCase == "-leftaxo"
            || anArgCase == "axoleft"
            || anArgCase == "leftaxo")
      {
        aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_AxoLeft : V3d_TypeOfOrientation_Zup_AxoLeft, isYup);
      }
      else if (anArgCase == "-axo"
            || anArgCase == "axo"
            || anArgCase == "-a"
            || anArgCase == "a"
            || anArgCase == "-axoright"
            || anArgCase == "-rightaxo"
            || anArgCase == "axoright"
            || anArgCase == "rightaxo")
      {
        aView->SetProj (isYup ? V3d_TypeOfOrientation_Yup_AxoRight : V3d_TypeOfOrientation_Zup_AxoRight, isYup);
      }
      else if (anArgCase == "+x")
      {
        aView->SetProj (V3d_Xpos, isYup);
      }
      else if (anArgCase == "-x")
      {
        aView->SetProj (V3d_Xneg, isYup);
      }
      else if (anArgCase == "+y")
      {
        aView->SetProj (V3d_Ypos, isYup);
      }
      else if (anArgCase == "-y")
      {
        aView->SetProj (V3d_Yneg, isYup);
      }
      else if (anArgCase == "+z")
      {
        aView->SetProj (V3d_Zpos, isYup);
      }
      else if (anArgCase == "-z")
      {
        aView->SetProj (V3d_Zneg, isYup);
      }
      else if (anArgCase == "+x+y+z")
      {
        aView->SetProj (V3d_XposYposZpos, isYup);
      }
      else if (anArgCase == "+x+y-z")
      {
        aView->SetProj (V3d_XposYposZneg, isYup);
      }
      else if (anArgCase == "+x-y+z")
      {
        aView->SetProj (V3d_XposYnegZpos, isYup);
      }
      else if (anArgCase == "+x-y-z")
      {
        aView->SetProj (V3d_XposYnegZneg, isYup);
      }
      else if (anArgCase == "-x+y+z")
      {
        aView->SetProj (V3d_XnegYposZpos, isYup);
      }
      else if (anArgCase == "-x+y-z")
      {
        aView->SetProj (V3d_XnegYposZneg, isYup);
      }
      else if (anArgCase == "-x-y+z")
      {
        aView->SetProj (V3d_XnegYnegZpos, isYup);
      }
      else if (anArgCase == "-x-y-z")
      {
        aView->SetProj (V3d_XnegYnegZneg, isYup);
      }
      else if (anArgCase == "+x+y")
      {
        aView->SetProj (V3d_XposYpos, isYup);
      }
      else if (anArgCase == "+x-y")
      {
        aView->SetProj (V3d_XposYneg, isYup);
      }
      else if (anArgCase == "-x+y")
      {
        aView->SetProj (V3d_XnegYpos, isYup);
      }
      else if (anArgCase == "-x-y")
      {
        aView->SetProj (V3d_XnegYneg, isYup);
      }
      else if (anArgCase == "+x+z")
      {
        aView->SetProj (V3d_XposZpos, isYup);
      }
      else if (anArgCase == "+x-z")
      {
        aView->SetProj (V3d_XposZneg, isYup);
      }
      else if (anArgCase == "-x+z")
      {
        aView->SetProj (V3d_XnegZpos, isYup);
      }
      else if (anArgCase == "-x-z")
      {
        aView->SetProj (V3d_XnegZneg, isYup);
      }
      else if (anArgCase == "+y+z")
      {
        aView->SetProj (V3d_YposZpos, isYup);
      }
      else if (anArgCase == "+y-z")
      {
        aView->SetProj (V3d_YposZneg, isYup);
      }
      else if (anArgCase == "-y+z")
      {
        aView->SetProj (V3d_YnegZpos, isYup);
      }
      else if (anArgCase == "-y-z")
      {
        aView->SetProj (V3d_YnegZneg, isYup);
      }
      else if (anArgIter + 1 < theNbArgs
            && anArgCase == "-frame"
            && TCollection_AsciiString (theArgVec[anArgIter + 1]).Length() == 4)
      {
        TCollection_AsciiString aFrameDef (theArgVec[++anArgIter]);
        aFrameDef.LowerCase();
        gp_Dir aRight, anUp;
        if (aFrameDef.Value (2) == aFrameDef.Value (4))
        {
          Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
          return 1;
        }

        if (aFrameDef.Value (2) == 'x')
        {
          aRight = aFrameDef.Value (1) == '+' ? gp::DX() : -gp::DX();
        }
        else if (aFrameDef.Value (2) == 'y')
        {
          aRight = aFrameDef.Value (1) == '+' ? gp::DY() : -gp::DY();
        }
        else if (aFrameDef.Value (2) == 'z')
        {
          aRight = aFrameDef.Value (1) == '+' ? gp::DZ() : -gp::DZ();
        }
        else
        {
          Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
          return 1;
        }

        if (aFrameDef.Value (4) == 'x')
        {
          anUp = aFrameDef.Value (3) == '+' ? gp::DX() : -gp::DX();
        }
        else if (aFrameDef.Value (4) == 'y')
        {
          anUp = aFrameDef.Value (3) == '+' ? gp::DY() : -gp::DY();
        }
        else if (aFrameDef.Value (4) == 'z')
        {
          anUp = aFrameDef.Value (3) == '+' ? gp::DZ() : -gp::DZ();
        }
        else
        {
          Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
          return 1;
        }

        const Handle(Graphic3d_Camera)& aCamera = aView->Camera();
        const gp_Pnt anOriginVCS = aCamera->ConvertWorld2View (gp::Origin());
        const gp_Dir aDir = anUp.Crossed (aRight);
        aCamera->SetCenter (gp_Pnt (0, 0, 0));
        aCamera->SetDirection (aDir);
        aCamera->SetUp (anUp);
        aCamera->OrthogonalizeUp();

        aView->Panning (anOriginVCS.X(), anOriginVCS.Y());
        aView->Update();
      }
      else
      {
        Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }
    }
  }

  if (!isGeneralCmd
    && theNbArgs != 1)
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }
  return 0;
}

//==============================================================================
//function : VHelp
//purpose  : Dsiplay help on viewer Keyboead and mouse commands
//Draw arg : No args
//==============================================================================

static int VHelp(Draw_Interpretor& di, Standard_Integer , const char** )
{
  di << "=========================\n";
  di << "F : FitAll\n";
  di << "T : TopView\n";
  di << "B : BottomView\n";
  di << "R : RightView\n";
  di << "L : LeftView\n";
  di << "Backspace : AxonometricView\n";

  di << "=========================\n";
  di << "W, S : Fly   forward/backward\n";
  di << "A, D : Slide left/right\n";
  di << "Q, E : Bank  left/right\n";
  di << "-, + : Change flying speed\n";
  di << "Arrows : look left/right/up/down\n";
  di << "Arrows+Shift : slide left/right/up/down\n";

  di << "=========================\n";
  di << "S + Ctrl : Shading\n";
  di << "W + Ctrl : Wireframe\n";
  di << "H : HiddenLineRemoval\n";
  di << "U : Unset display mode\n";
  di << "Delete : Remove selection from viewer\n";

  di << "=========================\n";
  di << "Selection mode \n";
  di << "0 : Shape\n";
  di << "1 : Vertex\n";
  di << "2 : Edge\n";
  di << "3 : Wire\n";
  di << "4 : Face\n";
  di << "5 : Shell\n";
  di << "6 : Solid\n";
  di << "7 : Compound\n";

  di << "=========================\n";
  di << "< : Hilight next detected\n";
  di << "> : Hilight previous detected\n";

  return 0;
}

#ifdef _WIN32

static LRESULT WINAPI AdvViewerWindowProc (HWND theWinHandle,
                                           UINT theMsg,
                                           WPARAM wParam,
                                           LPARAM lParam )
{
  if (ViewerTest_myViews.IsEmpty())
  {
    return ViewerWindowProc (theWinHandle, theMsg, wParam, lParam);
  }

  switch (theMsg)
  {
    case WM_CLOSE:
    {
      // Delete view from map of views
      ViewerTest::RemoveView (FindViewIdByWindowHandle (theWinHandle));
      return 0;
    }
    case WM_ACTIVATE:
    {
      if (LOWORD(wParam) == WA_CLICKACTIVE
       || LOWORD(wParam) == WA_ACTIVE
       || ViewerTest::CurrentView().IsNull())
      {
        // Activate inactive window
        if (VT_GetWindow().IsNull()
         || (HWND )VT_GetWindow()->HWindow() != theWinHandle)
        {
          ActivateView (FindViewIdByWindowHandle (theWinHandle));
        }
      }
      break;
    }
    default:
    {
      return ViewerWindowProc (theWinHandle, theMsg, wParam, lParam);
    }
  }
  return 0;
}

static LRESULT WINAPI ViewerWindowProc (HWND theWinHandle,
                                        UINT theMsg,
                                        WPARAM wParam,
                                        LPARAM lParam)
{
  const Handle(V3d_View)& aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    return DefWindowProcW (theWinHandle, theMsg, wParam, lParam);
  }

  switch (theMsg)
  {
    case WM_PAINT:
    {
      PAINTSTRUCT aPaint;
      BeginPaint(theWinHandle, &aPaint);
      EndPaint  (theWinHandle, &aPaint);
      ViewerTest::CurrentEventManager()->ProcessExpose();
      break;
    }
    case WM_SIZE:
    {
      ViewerTest::CurrentEventManager()->ProcessConfigure();
      break;
    }
    case WM_MOVE:
    case WM_MOVING:
    case WM_SIZING:
    {
      switch (aView->RenderingParams().StereoMode)
      {
        case Graphic3d_StereoMode_RowInterlaced:
        case Graphic3d_StereoMode_ColumnInterlaced:
        case Graphic3d_StereoMode_ChessBoard:
        {
          // track window moves to reverse stereo pair
          aView->MustBeResized();
          aView->Update();
          break;
        }
        default:
          break;
      }
      break;
    }
    case WM_KEYUP:
    case WM_KEYDOWN:
    {
      const Aspect_VKey aVKey = WNT_Window::VirtualKeyFromNative ((Standard_Integer )wParam);
      if (aVKey != Aspect_VKey_UNKNOWN)
      {
        const double aTimeStamp = ViewerTest::CurrentEventManager()->EventTime();
        if (theMsg == WM_KEYDOWN)
        {
          ViewerTest::CurrentEventManager()->KeyDown (aVKey, aTimeStamp);
        }
        else
        {
          ViewerTest::CurrentEventManager()->KeyUp (aVKey, aTimeStamp);
        }
        ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
      }
      break;
    }
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    {
      const Graphic3d_Vec2i aPos (LOWORD(lParam), HIWORD(lParam));
      const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (wParam);
      Aspect_VKeyMouse aButton = Aspect_VKeyMouse_NONE;
      switch (theMsg)
      {
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
          aButton = Aspect_VKeyMouse_LeftButton;
          break;
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:
          aButton = Aspect_VKeyMouse_MiddleButton;
          break;
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
          aButton = Aspect_VKeyMouse_RightButton;
          break;
      }
      if (theMsg == WM_LBUTTONDOWN
       || theMsg == WM_MBUTTONDOWN
       || theMsg == WM_RBUTTONDOWN)
      {
        if (aButton == Aspect_VKeyMouse_LeftButton)
        {
          TheIsAnimating = Standard_False;
        }

        SetFocus  (theWinHandle);
        SetCapture(theWinHandle);
        ViewerTest::CurrentEventManager()->PressMouseButton (aPos, aButton, aFlags, false);
      }
      else
      {
        ReleaseCapture();
        ViewerTest::CurrentEventManager()->ReleaseMouseButton (aPos, aButton, aFlags, false);
      }
      ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
      break;
    }
    case WM_MOUSEWHEEL:
    {
      const int aDelta = GET_WHEEL_DELTA_WPARAM (wParam);
      const Standard_Real aDeltaF = Standard_Real(aDelta) / Standard_Real(WHEEL_DELTA);
      const Aspect_VKeyFlags aFlags = WNT_Window::MouseKeyFlagsFromEvent (wParam);
      Graphic3d_Vec2i aPos (int(short(LOWORD(lParam))), int(short(HIWORD(lParam))));
      POINT aCursorPnt = { aPos.x(), aPos.y() };
      if (ScreenToClient (theWinHandle, &aCursorPnt))
      {
        aPos.SetValues (aCursorPnt.x, aCursorPnt.y);
      }

      ViewerTest::CurrentEventManager()->UpdateMouseScroll (Aspect_ScrollDelta (aPos, aDeltaF, aFlags));
      ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
      break;
    }
    case WM_MOUSEMOVE:
    {
      Graphic3d_Vec2i aPos (LOWORD(lParam), HIWORD(lParam));
      Aspect_VKeyMouse aButtons = WNT_Window::MouseButtonsFromEvent (wParam);
      Aspect_VKeyFlags aFlags   = WNT_Window::MouseKeyFlagsFromEvent(wParam);

      // don't make a slide-show from input events - fetch the actual mouse cursor position
      CURSORINFO aCursor;
      aCursor.cbSize = sizeof(aCursor);
      if (::GetCursorInfo (&aCursor) != FALSE)
      {
        POINT aCursorPnt = { aCursor.ptScreenPos.x, aCursor.ptScreenPos.y };
        if (ScreenToClient (theWinHandle, &aCursorPnt))
        {
          // as we override mouse position, we need overriding also mouse state
          aPos.SetValues (aCursorPnt.x, aCursorPnt.y);
          aButtons = WNT_Window::MouseButtonsAsync();
          aFlags   = WNT_Window::MouseKeyFlagsAsync();
        }
      }

      if (VT_GetWindow().IsNull()
      || (HWND )VT_GetWindow()->HWindow() != theWinHandle)
      {
        // mouse move events come also for inactive windows
        break;
      }

      ViewerTest::CurrentEventManager()->UpdateMousePosition (aPos, aButtons, aFlags, false);
      ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), aView, true);
      break;
    }
    case WM_INPUT:
    {
      UINT aSize = 0;
      ::GetRawInputData ((HRAWINPUT )lParam, RID_INPUT, NULL, &aSize, sizeof(RAWINPUTHEADER));
      NCollection_LocalArray<BYTE> aRawData (aSize);
      if (aSize == 0 || ::GetRawInputData ((HRAWINPUT )lParam, RID_INPUT, aRawData, &aSize, sizeof(RAWINPUTHEADER)) != aSize)
      {
        break;
      }

      const RAWINPUT* aRawInput = (RAWINPUT* )(BYTE* )aRawData;
      if (aRawInput->header.dwType != RIM_TYPEHID)
      {
        break;
      }

      RID_DEVICE_INFO aDevInfo;
      aDevInfo.cbSize = sizeof(RID_DEVICE_INFO);
      UINT aDevInfoSize = sizeof(RID_DEVICE_INFO);
      if (::GetRawInputDeviceInfoW (aRawInput->header.hDevice, RIDI_DEVICEINFO, &aDevInfo, &aDevInfoSize) != sizeof(RID_DEVICE_INFO)
        || (aDevInfo.hid.dwVendorId != WNT_HIDSpaceMouse::VENDOR_ID_LOGITECH
         && aDevInfo.hid.dwVendorId != WNT_HIDSpaceMouse::VENDOR_ID_3DCONNEXION))
      {
        break;
      }

      WNT_HIDSpaceMouse aSpaceData (aDevInfo.hid.dwProductId, aRawInput->data.hid.bRawData, aRawInput->data.hid.dwSizeHid);
      if (ViewerTest::CurrentEventManager()->Update3dMouse (aSpaceData)
      && !VT_GetWindow().IsNull())
      {
        VT_GetWindow()->InvalidateContent();
      }
      break;
    }
    default:
    {
      return DefWindowProcW (theWinHandle, theMsg, wParam, lParam);
    }
  }
  return 0L;
}

//==============================================================================
//function : ViewerMainLoop
//purpose  : Get a Event on the view and dispatch it
//==============================================================================

int ViewerMainLoop (Standard_Integer theNbArgs, const char** theArgVec)
{
  Handle(ViewerTest_EventManager) aViewCtrl = ViewerTest::CurrentEventManager();
  if (aViewCtrl.IsNull()
   || theNbArgs < 4)
  {
    return 0;
  }

  aViewCtrl->StartPickPoint (theArgVec[1], theArgVec[2], theArgVec[3]);

  std::cout << "Start picking\n";

  MSG aMsg;
  aMsg.wParam = 1;
  while (aViewCtrl->ToPickPoint())
  {
    // Wait for a VT_ProcessButton1Press() to toggle pick to 1 or 0
    if (GetMessageW (&aMsg, NULL, 0, 0))
    {
      TranslateMessage (&aMsg);
      DispatchMessageW (&aMsg);
    }
  }

  std::cout << "Picking done\n";
  return 0;
}

#elif !defined(__APPLE__) || defined(MACOSX_USE_GLX)

int min( int a, int b )
{
  if( a<b )
    return a;
  else
    return b;
}

int max( int a, int b )
{
  if( a>b )
    return a;
  else
    return b;
}

int ViewerMainLoop (Standard_Integer theNbArgs, const char** theArgVec)
{
  static XEvent aReport;
  const Standard_Boolean toPick = theNbArgs > 0;
  if (theNbArgs > 0)
  {
    if (ViewerTest::CurrentEventManager().IsNull())
    {
      return 0;
    }
    ViewerTest::CurrentEventManager()->StartPickPoint (theArgVec[1], theArgVec[2], theArgVec[3]);
  }

  Display* aDisplay = GetDisplayConnection()->GetDisplay();
  XNextEvent (aDisplay, &aReport);

  // Handle event for the chosen display connection
  switch (aReport.type)
  {
    case ClientMessage:
    {
      if ((Atom)aReport.xclient.data.l[0] == GetDisplayConnection()->GetAtom(Aspect_XA_DELETE_WINDOW))
      {
        // Close the window
        ViewerTest::RemoveView(FindViewIdByWindowHandle (aReport.xclient.window));
        return toPick ? 0 : 1;
      }
      break;
    }
    case FocusIn:
    {
      // Activate inactive view
      Window aWindow = !VT_GetWindow().IsNull() ? VT_GetWindow()->XWindow() : 0;
      if (aWindow != aReport.xfocus.window)
      {
        ActivateView (FindViewIdByWindowHandle (aReport.xfocus.window));
      }
      break;
    }
    case Expose:
    {
      Window anXWindow = !VT_GetWindow().IsNull() ? VT_GetWindow()->XWindow() : 0;
      if (anXWindow == aReport.xexpose.window)
      {
        ViewerTest::CurrentEventManager()->ProcessExpose();
      }

      // remove all the ExposureMask and process them at once
      for (int aNbMaxEvents = XPending (aDisplay); aNbMaxEvents > 0; --aNbMaxEvents)
      {
        if (!XCheckWindowEvent (aDisplay, anXWindow, ExposureMask, &aReport))
        {
          break;
        }
      }

      break;
    }
    case ConfigureNotify:
    {
      // remove all the StructureNotifyMask and process them at once
      Window anXWindow = !VT_GetWindow().IsNull() ? VT_GetWindow()->XWindow() : 0;
      for (int aNbMaxEvents = XPending (aDisplay); aNbMaxEvents > 0; --aNbMaxEvents)
      {
        if (!XCheckWindowEvent (aDisplay, anXWindow, StructureNotifyMask, &aReport))
        {
          break;
        }
      }

      if (anXWindow == aReport.xconfigure.window)
      {
        ViewerTest::CurrentEventManager()->ProcessConfigure();
      }
      break;
    }
    case KeyPress:
    case KeyRelease:
    {
      XKeyEvent*   aKeyEvent = (XKeyEvent* )&aReport;
      const KeySym aKeySym = XLookupKeysym (aKeyEvent, 0);
      const Aspect_VKey aVKey = Xw_Window::VirtualKeyFromNative (aKeySym);
      if (aVKey != Aspect_VKey_UNKNOWN)
      {
        const double aTimeStamp = ViewerTest::CurrentEventManager()->EventTime();
        if (aReport.type == KeyPress)
        {
          ViewerTest::CurrentEventManager()->KeyDown (aVKey, aTimeStamp);
        }
        else
        {
          ViewerTest::CurrentEventManager()->KeyUp (aVKey, aTimeStamp);
        }
        ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
      }
      break;
    }
    case ButtonPress:
    case ButtonRelease:
    {
      const Graphic3d_Vec2i aPos (aReport.xbutton.x, aReport.xbutton.y);
      Aspect_VKeyFlags aFlags = Aspect_VKeyFlags_NONE;
      Aspect_VKeyMouse aButton = Aspect_VKeyMouse_NONE;
      if (aReport.xbutton.button == Button1)
      {
        aButton = Aspect_VKeyMouse_LeftButton;
      }
      if (aReport.xbutton.button == Button2)
      {
        aButton = Aspect_VKeyMouse_MiddleButton;
      }
      if (aReport.xbutton.button == Button3)
      {
        aButton = Aspect_VKeyMouse_RightButton;
      }

      if (aReport.xbutton.state & ControlMask)
      {
        aFlags |= Aspect_VKeyFlags_CTRL;
      }
      if (aReport.xbutton.state & ShiftMask)
      {
        aFlags |= Aspect_VKeyFlags_SHIFT;
      }
      if (ViewerTest::CurrentEventManager()->Keys().IsKeyDown (Aspect_VKey_Alt))
      {
        aFlags |= Aspect_VKeyFlags_ALT;
      }

      if (aReport.xbutton.button == Button4
       || aReport.xbutton.button == Button5)
      {
        if (aReport.type != ButtonPress)
        {
          break;
        }

        const double aDeltaF = (aReport.xbutton.button == Button4 ? 1.0 : -1.0);
        ViewerTest::CurrentEventManager()->UpdateMouseScroll (Aspect_ScrollDelta (aPos, aDeltaF, aFlags));
      }
      else if (aReport.type == ButtonPress)
      {
        if (aButton == Aspect_VKeyMouse_LeftButton)
        {
          TheIsAnimating = Standard_False;
        }
        ViewerTest::CurrentEventManager()->PressMouseButton (aPos, aButton, aFlags, false);
      }
      else
      {
        ViewerTest::CurrentEventManager()->ReleaseMouseButton (aPos, aButton, aFlags, false);
      }
      ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
      break;
    }
    case MotionNotify:
    {
      Window anXWindow = !VT_GetWindow().IsNull() ? VT_GetWindow()->XWindow() : 0;
      if (anXWindow != aReport.xmotion.window)
      {
        break;
      }

      // remove all the ButtonMotionMask and process them at once
      for (int aNbMaxEvents = XPending (aDisplay); aNbMaxEvents > 0; --aNbMaxEvents)
      {
        if (!XCheckWindowEvent (aDisplay, anXWindow, ButtonMotionMask | PointerMotionMask, &aReport))
        {
          break;
        }
      }

      Graphic3d_Vec2i aPos (aReport.xmotion.x, aReport.xmotion.y);
      Aspect_VKeyMouse aButtons = Aspect_VKeyMouse_NONE;
      Aspect_VKeyFlags aFlags   = Aspect_VKeyFlags_NONE;
      if ((aReport.xmotion.state & Button1Mask) != 0)
      {
        aButtons |= Aspect_VKeyMouse_LeftButton;
      }
      else if ((aReport.xmotion.state & Button2Mask) != 0)
      {
        aButtons |= Aspect_VKeyMouse_MiddleButton;
      }
      else if ((aReport.xmotion.state & Button3Mask) != 0)
      {
        aButtons |= Aspect_VKeyMouse_RightButton;
      }

      if (aReport.xmotion.state & ControlMask)
      {
        aFlags |= Aspect_VKeyFlags_CTRL;
      }
      if (aReport.xmotion.state & ShiftMask)
      {
        aFlags |= Aspect_VKeyFlags_SHIFT;
      }
      if (ViewerTest::CurrentEventManager()->Keys().IsKeyDown (Aspect_VKey_Alt))
      {
        aFlags |= Aspect_VKeyFlags_ALT;
      }

      ViewerTest::CurrentEventManager()->UpdateMousePosition (aPos, aButtons, aFlags, false);
      ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), ViewerTest::CurrentView(), true);
      break;
    }
  }
  return (!toPick || ViewerTest::CurrentEventManager()->ToPickPoint()) ? 1 : 0;
}

//==============================================================================
//function : VProcessEvents
//purpose  : manage the event in the Viewer window (see Tcl_CreateFileHandler())
//==============================================================================
static void VProcessEvents (ClientData theDispX, int)
{
  Display* aDispX = (Display* )theDispX;
  Handle(Aspect_DisplayConnection) aDispConn;
  for (NCollection_DoubleMap<TCollection_AsciiString, Handle(Graphic3d_GraphicDriver)>::Iterator
       aDriverIter (ViewerTest_myDrivers); aDriverIter.More(); aDriverIter.Next())
  {
    const Handle(Aspect_DisplayConnection)& aDispConnTmp = aDriverIter.Key2()->GetDisplayConnection();
    if (aDispConnTmp->GetDisplay() == aDispX)
    {
      aDispConn = aDispConnTmp;
      break;
    }
  }
  if (aDispConn.IsNull())
  {
    Message::SendFail ("Error: ViewerTest is unable processing messages for unknown X Display");
    return;
  }

  // process new events in queue
  SetDisplayConnection (aDispConn);
  int aNbRemain = 0;
  for (int aNbEventsMax = XPending (aDispX), anEventIter (0);;)
  {
    const int anEventResult = ViewerMainLoop (0, NULL);
    if (anEventResult == 0)
    {
      return;
    }

    aNbRemain = XPending (aDispX);
    if (++anEventIter >= aNbEventsMax
     || aNbRemain <= 0)
    {
      break;
    }
  }

  // Listening X events through Tcl_CreateFileHandler() callback is fragile,
  // it is possible that new events will arrive to queue before the end of this callback
  // so that either this callback should go into an infinite loop (blocking processing of other events)
  // or to keep unprocessed events till the next queue update (which can arrive not soon).
  // Sending a dummy event in this case is a simple workaround (still, it is possible that new event will be queued in-between).
  if (aNbRemain != 0)
  {
    XEvent aDummyEvent;
    memset (&aDummyEvent, 0, sizeof(aDummyEvent));
    aDummyEvent.type = ClientMessage;
    aDummyEvent.xclient.format = 32;
    XSendEvent (aDispX, InputFocus, False, 0, &aDummyEvent);
    XFlush (aDispX);
  }

  if (const Handle(AIS_InteractiveContext)& anActiveCtx = ViewerTest::GetAISContext())
  {
    SetDisplayConnection (anActiveCtx->CurrentViewer()->Driver()->GetDisplayConnection());
  }
}
#endif

//==============================================================================
//function : OSWindowSetup
//purpose  : Setup for the X11 window to be able to cath the event
//==============================================================================


static void OSWindowSetup()
{
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  // X11

  Window  window   = VT_GetWindow()->XWindow();
  SetDisplayConnection (ViewerTest::CurrentView()->Viewer()->Driver()->GetDisplayConnection());
  Display *aDisplay = GetDisplayConnection()->GetDisplay();
  XSynchronize(aDisplay, 1);

  // X11 : For keyboard on SUN
  XWMHints wmhints;
  wmhints.flags = InputHint;
  wmhints.input = 1;

  XSetWMHints( aDisplay, window, &wmhints);

  XSelectInput( aDisplay, window,  ExposureMask | KeyPressMask | KeyReleaseMask |
    ButtonPressMask | ButtonReleaseMask |
    StructureNotifyMask |
    PointerMotionMask |
    Button1MotionMask | Button2MotionMask |
    Button3MotionMask | FocusChangeMask
    );
  Atom aDeleteWindowAtom = GetDisplayConnection()->GetAtom(Aspect_XA_DELETE_WINDOW);
  XSetWMProtocols(aDisplay, window, &aDeleteWindowAtom, 1);

  XSynchronize(aDisplay, 0);

#else
  // _WIN32
#endif

}

//==============================================================================
//function : VFit
//purpose  :
//==============================================================================

static int VFit (Draw_Interpretor& /*theDi*/, Standard_Integer theArgNb, const char** theArgv)
{
  const Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Standard_Boolean toFit = Standard_True;
  ViewerTest_AutoUpdater anUpdateTool (Handle(AIS_InteractiveContext)(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgv[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      continue;
    }
    else if (anArg == "-selected")
    {
      ViewerTest::GetAISContext()->FitSelected (aView, 0.01, Standard_False);
      toFit = Standard_False;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
    }
  }

  if (toFit)
  {
    aView->FitAll (0.01, Standard_False);
  }
  return 0;
}

//=======================================================================
//function : VFitArea
//purpose  : Fit view to show area located between two points
//         : given in world 2D or 3D coordinates.
//=======================================================================
static int VFitArea (Draw_Interpretor& theDI, Standard_Integer  theArgNb, const char** theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: No active viewer");
    return 1;
  }

  // Parse arguments.
  gp_Pnt aWorldPnt1 (0.0, 0.0, 0.0);
  gp_Pnt aWorldPnt2 (0.0, 0.0, 0.0);

  if (theArgNb == 5)
  {
    aWorldPnt1.SetX (Draw::Atof (theArgVec[1]));
    aWorldPnt1.SetY (Draw::Atof (theArgVec[2]));
    aWorldPnt2.SetX (Draw::Atof (theArgVec[3]));
    aWorldPnt2.SetY (Draw::Atof (theArgVec[4]));
  }
  else if (theArgNb == 7)
  {
    aWorldPnt1.SetX (Draw::Atof (theArgVec[1]));
    aWorldPnt1.SetY (Draw::Atof (theArgVec[2]));
    aWorldPnt1.SetZ (Draw::Atof (theArgVec[3]));
    aWorldPnt2.SetX (Draw::Atof (theArgVec[4]));
    aWorldPnt2.SetY (Draw::Atof (theArgVec[5]));
    aWorldPnt2.SetZ (Draw::Atof (theArgVec[6]));
  }
  else
  {
    Message::SendFail ("Syntax error: Invalid number of arguments");
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  // Convert model coordinates to view space
  Handle(Graphic3d_Camera) aCamera = aView->Camera();
  gp_Pnt aViewPnt1 = aCamera->ConvertWorld2View (aWorldPnt1);
  gp_Pnt aViewPnt2 = aCamera->ConvertWorld2View (aWorldPnt2);

  // Determine fit area
  gp_Pnt2d aMinCorner (Min (aViewPnt1.X(), aViewPnt2.X()), Min (aViewPnt1.Y(), aViewPnt2.Y()));
  gp_Pnt2d aMaxCorner (Max (aViewPnt1.X(), aViewPnt2.X()), Max (aViewPnt1.Y(), aViewPnt2.Y()));

  Standard_Real aDiagonal = aMinCorner.Distance (aMaxCorner);

  if (aDiagonal < Precision::Confusion())
  {
    Message::SendFail ("Error: view area is too small");
    return 1;
  }

  aView->FitAll (aMinCorner.X(), aMinCorner.Y(), aMaxCorner.X(), aMaxCorner.Y());
  return 0;
}

//==============================================================================
//function : VZFit
//purpose  : ZFitall, no DRAW arguments
//Draw arg : No args
//==============================================================================
static int VZFit (Draw_Interpretor& /*theDi*/, Standard_Integer theArgsNb, const char** theArgVec)
{
  const Handle(V3d_View)& aCurrentView = ViewerTest::CurrentView();

  if (aCurrentView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  if (theArgsNb == 1)
  {
    aCurrentView->ZFitAll();
    aCurrentView->Redraw();
    return 0;
  }

  Standard_Real aScale = 1.0;

  if (theArgsNb >= 2)
  {
    aScale = Draw::Atoi (theArgVec[1]);
  }

  aCurrentView->ZFitAll (aScale);
  aCurrentView->Redraw();

  return 0;
}

//==============================================================================
//function : VRepaint
//purpose  :
//==============================================================================
static int VRepaint (Draw_Interpretor& , Standard_Integer theArgNb, const char** theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Standard_Boolean isImmediateUpdate = Standard_False;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-immediate"
     || anArg == "-imm")
    {
      isImmediateUpdate = Standard_True;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], isImmediateUpdate))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-continuous"
          || anArg == "-cont"
          || anArg == "-fps"
          || anArg == "-framerate")
    {
      Standard_Real aFps = -1.0;
      if (anArgIter + 1 < theArgNb
       && TCollection_AsciiString (theArgVec[anArgIter + 1]).IsRealValue())
      {
        aFps = Draw::Atof (theArgVec[++anArgIter]);
      }

      ViewerTest_ContinuousRedrawer& aRedrawer = ViewerTest_ContinuousRedrawer::Instance();
      if (Abs (aFps) >= 1.0)
      {
        aRedrawer.Start (aView->Window(), aFps);
      }
      else
      {
        aRedrawer.Stop();
      }
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  if (isImmediateUpdate)
  {
    aView->RedrawImmediate();
  }
  else
  {
    aView->Redraw();
  }
  return 0;
}

//==============================================================================
//function : VClear
//purpose  : Remove all the object from the viewer
//Draw arg : No args
//==============================================================================

static int VClear(Draw_Interpretor& , Standard_Integer , const char** )
{
  Handle(V3d_View) V = ViewerTest::CurrentView();
  if(!V.IsNull())
    ViewerTest::Clear();
  return 0;
}

//==============================================================================
//function : VPick
//purpose  :
//==============================================================================

static int VPick (Draw_Interpretor& ,
                  Standard_Integer theNbArgs,
                  const char** theArgVec)
{
  if (ViewerTest::CurrentView().IsNull())
  {
    return 1;
  }

  if (theNbArgs < 4)
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }

  while (ViewerMainLoop (theNbArgs, theArgVec))
  {
    //
  }

  return 0;
}

namespace
{

  //! Changes the background
  //! @param theDrawInterpretor the interpreter of the Draw Harness application
  //! @param theNumberOfCommandLineArguments the number of passed command line arguments
  //! @param theCommandLineArguments the array of command line arguments
  //! @return TCL_OK if changing was successful, or TCL_ERROR otherwise
  static int vbackground (Draw_Interpretor&      theDrawInterpretor,
                          const Standard_Integer theNumberOfCommandLineArguments,
                          const char** const     theCommandLineArguments)
  {
    if (theNumberOfCommandLineArguments < 1)
    {
      return TCL_ERROR;
    }
    BackgroundChanger aBackgroundChanger;
    if (!aBackgroundChanger.ProcessCommandLine (theDrawInterpretor,
                                                theNumberOfCommandLineArguments,
                                                theCommandLineArguments))
    {
      theDrawInterpretor << "Wrong command arguments.\n"
                            "Type 'help "
                         << theCommandLineArguments[0] << "' for information about command options and its arguments.\n";
      return TCL_ERROR;
    }
    return TCL_OK;
  }

} // namespace

//==============================================================================
//function : VScale
//purpose  : View Scaling
//==============================================================================

static int VScale(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  if ( V3dView.IsNull() ) return 1;

  if ( argc != 4 ) {
    di << argv[0] << "Invalid number of arguments\n";
    return 1;
  }
  V3dView->SetAxialScale( Draw::Atof(argv[1]),  Draw::Atof(argv[2]),  Draw::Atof(argv[3]) );
  return 0;
}
//==============================================================================
//function : VZBuffTrihedron
//purpose  :
//==============================================================================

static int VZBuffTrihedron (Draw_Interpretor& /*theDI*/,
                            Standard_Integer  theArgNb,
                            const char**      theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  ViewerTest_AutoUpdater anUpdateTool (ViewerTest::GetAISContext(), aView);

  Aspect_TypeOfTriedronPosition aPosition     = Aspect_TOTP_LEFT_LOWER;
  V3d_TypeOfVisualization       aVisType      = V3d_ZBUFFER;
  Quantity_Color                aLabelsColor  = Quantity_NOC_WHITE;
  Quantity_Color                anArrowColorX = Quantity_NOC_RED;
  Quantity_Color                anArrowColorY = Quantity_NOC_GREEN;
  Quantity_Color                anArrowColorZ = Quantity_NOC_BLUE1;
  Standard_Real                 aScale        = 0.1;
  Standard_Real                 aSizeRatio    = 0.8;
  Standard_Real                 anArrowDiam   = 0.05;
  Standard_Integer              aNbFacets     = 12;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg = theArgVec[anArgIter];
    TCollection_AsciiString aFlag (anArg);
    aFlag.LowerCase();
    if (anUpdateTool.parseRedrawMode (aFlag))
    {
      continue;
    }
    else if (aFlag == "-on")
    {
      continue;
    }
    else if (aFlag == "-off")
    {
      aView->TriedronErase();
      return 0;
    }
    else if (aFlag == "-pos"
          || aFlag == "-position"
          || aFlag == "-corner")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aPosName (theArgVec[anArgIter]);
      aPosName.LowerCase();
      if (aPosName == "center")
      {
        aPosition = Aspect_TOTP_CENTER;
      }
      else if (aPosName == "left_lower"
            || aPosName == "lower_left"
            || aPosName == "leftlower"
            || aPosName == "lowerleft")
      {
        aPosition = Aspect_TOTP_LEFT_LOWER;
      }
      else if (aPosName == "left_upper"
            || aPosName == "upper_left"
            || aPosName == "leftupper"
            || aPosName == "upperleft")
      {
        aPosition = Aspect_TOTP_LEFT_UPPER;
      }
      else if (aPosName == "right_lower"
            || aPosName == "lower_right"
            || aPosName == "rightlower"
            || aPosName == "lowerright")
      {
        aPosition = Aspect_TOTP_RIGHT_LOWER;
      }
      else if (aPosName == "right_upper"
            || aPosName == "upper_right"
            || aPosName == "rightupper"
            || aPosName == "upperright")
      {
        aPosition = Aspect_TOTP_RIGHT_UPPER;
      }
      else
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "' - unknown position '" << aPosName << "'";
        return 1;
      }
    }
    else if (aFlag == "-type")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aTypeName (theArgVec[anArgIter]);
      aTypeName.LowerCase();
      if (aTypeName == "wireframe"
       || aTypeName == "wire")
      {
        aVisType = V3d_WIREFRAME;
      }
      else if (aTypeName == "zbuffer"
            || aTypeName == "shaded")
      {
        aVisType = V3d_ZBUFFER;
      }
      else
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "' - unknown type '" << aTypeName << "'";
      }
    }
    else if (aFlag == "-scale")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      aScale = Draw::Atof (theArgVec[anArgIter]);
    }
    else if (aFlag == "-size"
          || aFlag == "-sizeratio")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      aSizeRatio = Draw::Atof (theArgVec[anArgIter]);
    }
    else if (aFlag == "-arrowdiam"
          || aFlag == "-arrowdiameter")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      anArrowDiam = Draw::Atof (theArgVec[anArgIter]);
    }
    else if (aFlag == "-nbfacets")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      aNbFacets = Draw::Atoi (theArgVec[anArgIter]);
    }
    else if (aFlag == "-colorlabel"
          || aFlag == "-colorlabels")
    {
      Standard_Integer aNbParsed = Draw::ParseColor (theArgNb  - anArgIter - 1,
                                                     theArgVec + anArgIter + 1,
                                                     aLabelsColor);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }
      anArgIter += aNbParsed;
    }
    else if (aFlag == "-colorarrowx")
    {
      Standard_Integer aNbParsed = Draw::ParseColor (theArgNb  - anArgIter - 1,
                                                     theArgVec + anArgIter + 1,
                                                     anArrowColorX);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }
      anArgIter += aNbParsed;
    }
    else if (aFlag == "-colorarrowy")
    {
      Standard_Integer aNbParsed = Draw::ParseColor (theArgNb  - anArgIter - 1,
                                                     theArgVec + anArgIter + 1,
                                                     anArrowColorY);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }
      anArgIter += aNbParsed;
    }
    else if (aFlag == "-colorarrowz")
    {
      Standard_Integer aNbParsed = Draw::ParseColor (theArgNb  - anArgIter - 1,
                                                     theArgVec + anArgIter + 1,
                                                     anArrowColorZ);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }
      anArgIter += aNbParsed;
    }
    else
    {
      Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
      return 1;
    }
  }

  aView->ZBufferTriedronSetup (anArrowColorX.Name(), anArrowColorY.Name(), anArrowColorZ.Name(),
                               aSizeRatio, anArrowDiam, aNbFacets);
  aView->TriedronDisplay (aPosition, aLabelsColor.Name(), aScale, aVisType);
  aView->ZFitAll();
  return 0;
}

//==============================================================================
//function : VRotate
//purpose  : Camera Rotating
//==============================================================================

static int VRotate (Draw_Interpretor& /*theDi*/, Standard_Integer theArgNb, const char** theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Standard_Boolean hasFlags = Standard_False;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg (theArgVec[anArgIter]);
    TCollection_AsciiString aFlag (anArg);
    aFlag.LowerCase();
    if (aFlag == "-mousestart"
     || aFlag == "-mousefrom")
    {
      hasFlags = Standard_True;
      if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      Standard_Integer anX = Draw::Atoi (theArgVec[++anArgIter]);
      Standard_Integer anY = Draw::Atoi (theArgVec[++anArgIter]);
      aView->StartRotation (anX, anY);
    }
    else if (aFlag == "-mousemove")
    {
      hasFlags = Standard_True;
      if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      Standard_Integer anX = Draw::Atoi (theArgVec[++anArgIter]);
      Standard_Integer anY = Draw::Atoi (theArgVec[++anArgIter]);
      aView->Rotation (anX, anY);
    }
    else if (theArgNb != 4
          && theArgNb != 7)
    {
      Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
      return 1;
    }
  }

  if (hasFlags)
  {
    return 0;
  }
  else if (theArgNb == 4)
  {
    Standard_Real anAX = Draw::Atof (theArgVec[1]);
    Standard_Real anAY = Draw::Atof (theArgVec[2]);
    Standard_Real anAZ = Draw::Atof (theArgVec[3]);
    aView->Rotate (anAX, anAY, anAZ);
    return 0;
  }
  else if (theArgNb == 7)
  {
    Standard_Real anAX = Draw::Atof (theArgVec[1]);
    Standard_Real anAY = Draw::Atof (theArgVec[2]);
    Standard_Real anAZ = Draw::Atof (theArgVec[3]);

    Standard_Real anX = Draw::Atof (theArgVec[4]);
    Standard_Real anY = Draw::Atof (theArgVec[5]);
    Standard_Real anZ = Draw::Atof (theArgVec[6]);

    aView->Rotate (anAX, anAY, anAZ, anX, anY, anZ);
    return 0;
  }

  Message::SendFail ("Error: Invalid number of arguments");
  return 1;
}

//==============================================================================
//function : VZoom
//purpose  : View zoom in / out (relative to current zoom)
//==============================================================================

static int VZoom( Draw_Interpretor& di, Standard_Integer argc, const char** argv ) {
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  if ( V3dView.IsNull() ) {
    return 1;
  }

  if ( argc == 2 ) {
    Standard_Real coef = Draw::Atof(argv[1]);
    if ( coef <= 0.0 ) {
      di << argv[1] << "Invalid value\n";
      return 1;
    }
    V3dView->SetZoom( Draw::Atof(argv[1]) );
    return 0;
  } else {
    di << argv[0] << " Invalid number of arguments\n";
    return 1;
  }
}

//==============================================================================
//function : VPan
//purpose  : View panning (in pixels)
//==============================================================================

static int VPan( Draw_Interpretor& di, Standard_Integer argc, const char** argv ) {
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  if ( V3dView.IsNull() ) return 1;

  if ( argc == 3 ) {
    V3dView->Pan( Draw::Atoi(argv[1]), Draw::Atoi(argv[2]) );
    return 0;
  } else {
    di << argv[0] << " Invalid number of arguments\n";
    return 1;
  }
}

//==============================================================================
//function : VPlace
//purpose  : Place the point (in pixels) at the center of the window
//==============================================================================
static int VPlace (Draw_Interpretor& /*theDi*/, Standard_Integer theArgNb, const char** theArgs)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  if (theArgNb != 3)
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }

  aView->Place (Draw::Atoi (theArgs[1]), Draw::Atoi (theArgs[2]), aView->Scale());

  return 0;
}

static int VColorScale (Draw_Interpretor& theDI,
                        Standard_Integer  theArgNb,
                        const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  Handle(V3d_View)               aView    = ViewerTest::CurrentView();
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }
  if (theArgNb <= 1)
  {
    Message::SendFail() << "Error: wrong syntax at command '" << theArgVec[0] << "'";
    return 1;
  }

  Handle(AIS_ColorScale) aColorScale;
  if (GetMapOfAIS().IsBound2 (theArgVec[1]))
  {
    // find existing object
    aColorScale = Handle(AIS_ColorScale)::DownCast (GetMapOfAIS().Find2 (theArgVec[1]));
    if (aColorScale.IsNull())
    {
      Message::SendFail() << "Error: object '" << theArgVec[1] << "'is already defined and is not a color scale";
      return 1;
    }
  }

  if (theArgNb <= 2)
  {
    if (aColorScale.IsNull())
    {
      Message::SendFail() << "Syntax error: colorscale with a given name does not exist";
      return 1;
    }

    theDI << "Color scale parameters for '"<< theArgVec[1] << "':\n"
          << "Min range: "            << aColorScale->GetMin() << "\n"
          << "Max range: "            << aColorScale->GetMax() << "\n"
          << "Number of intervals: "  << aColorScale->GetNumberOfIntervals() << "\n"
          << "Text height: "          << aColorScale->GetTextHeight() << "\n"
          << "Color scale position: " << aColorScale->GetXPosition() << " " << aColorScale->GetYPosition() << "\n"
          << "Color scale title: "    << aColorScale->GetTitle() << "\n"
          << "Label position: ";
    switch (aColorScale->GetLabelPosition())
    {
      case Aspect_TOCSP_NONE:
        theDI << "None\n";
        break;
      case Aspect_TOCSP_LEFT:
        theDI << "Left\n";
        break;
      case Aspect_TOCSP_RIGHT:
        theDI << "Right\n";
        break;
      case Aspect_TOCSP_CENTER:
        theDI << "Center\n";
        break;
    }
    return 0;
  }

  if (aColorScale.IsNull())
  {
    aColorScale = new AIS_ColorScale();
    aColorScale->SetZLayer (Graphic3d_ZLayerId_TopOSD);
    aContext->SetTransformPersistence (aColorScale, new Graphic3d_TransformPers (Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER));
  }

  ViewerTest_AutoUpdater anUpdateTool (aContext, aView);
  for (Standard_Integer anArgIter = 2; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg = theArgVec[anArgIter];
    TCollection_AsciiString aFlag (anArg);
    aFlag.LowerCase();
    if (anUpdateTool.parseRedrawMode (aFlag))
    {
      continue;
    }
    else if (aFlag == "-range")
    {
      if (anArgIter + 3 >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at argument '" << anArg << "'";
        return 1;
      }

      const TCollection_AsciiString aRangeMin    (theArgVec[++anArgIter]);
      const TCollection_AsciiString aRangeMax    (theArgVec[++anArgIter]);
      const TCollection_AsciiString aNbIntervals (theArgVec[++anArgIter]);
      if (!aRangeMin.IsRealValue()
       || !aRangeMax.IsRealValue())
      {
        Message::SendFail ("Syntax error: the range values should be real");
        return 1;
      }
      else if (!aNbIntervals.IsIntegerValue())
      {
        Message::SendFail ("Syntax error: the number of intervals should be integer");
        return 1;
      }

      aColorScale->SetRange (aRangeMin.RealValue(), aRangeMax.RealValue());
      aColorScale->SetNumberOfIntervals (aNbIntervals.IntegerValue());
    }
    else if (aFlag == "-font")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      TCollection_AsciiString aFontArg(theArgVec[anArgIter + 1]);
      if (!aFontArg.IsIntegerValue())
      {
        Message::SendFail ("Syntax error: HeightFont value should be integer");
        return 1;
      }

      aColorScale->SetTextHeight (aFontArg.IntegerValue());
      anArgIter += 1;
    }
    else if (aFlag == "-textpos")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aTextPosArg(theArgVec[++anArgIter]);
      aTextPosArg.LowerCase();
      Aspect_TypeOfColorScalePosition aLabPosition = Aspect_TOCSP_NONE;
      if (aTextPosArg == "none")
      {
        aLabPosition = Aspect_TOCSP_NONE;
      }
      else if (aTextPosArg == "left")
      {
        aLabPosition = Aspect_TOCSP_LEFT;
      }
      else if (aTextPosArg == "right")
      {
        aLabPosition = Aspect_TOCSP_RIGHT;
      }
      else if (aTextPosArg == "center")
      {
        aLabPosition = Aspect_TOCSP_CENTER;
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown position '" << aTextPosArg << "'";
        return 1;
      }
      aColorScale->SetLabelPosition (aLabPosition);
    }
    else if (aFlag == "-logarithmic"
          || aFlag == "-log")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Synta error at argument '" << anArg << "'";
        return 1;
      }

      Standard_Boolean IsLog;
      if (!Draw::ParseOnOff(theArgVec[++anArgIter], IsLog))
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      aColorScale->SetLogarithmic (IsLog);
    }
    else if (aFlag == "-huerange"
          || aFlag == "-hue")
    {
      if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Real aHueMin = Draw::Atof (theArgVec[++anArgIter]);
      const Standard_Real aHueMax = Draw::Atof (theArgVec[++anArgIter]);
      aColorScale->SetHueRange (aHueMin, aHueMax);
    }
    else if (aFlag == "-colorrange")
    {
      Quantity_Color aColorMin, aColorMax;
      Standard_Integer aNbParsed1 = Draw::ParseColor (theArgNb  - (anArgIter + 1),
                                                      theArgVec + (anArgIter + 1),
                                                      aColorMin);
      anArgIter += aNbParsed1;
      Standard_Integer aNbParsed2 = Draw::ParseColor (theArgNb  - (anArgIter + 1),
                                                      theArgVec + (anArgIter + 1),
                                                      aColorMax);
      anArgIter += aNbParsed2;
      if (aNbParsed1 == 0
       || aNbParsed2 == 0)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      aColorScale->SetColorRange (aColorMin, aColorMax);
    }
    else if (aFlag == "-reversed"
          || aFlag == "-inverted"
          || aFlag == "-topdown"
          || aFlag == "-bottomup")
    {
      Standard_Boolean toEnable = Standard_True;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff(theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      aColorScale->SetReversed ((aFlag == "-topdown") ? !toEnable : toEnable);
    }
    else if (aFlag == "-smooth"
          || aFlag == "-smoothtransition")
    {
      Standard_Boolean toEnable = Standard_True;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      aColorScale->SetSmoothTransition (toEnable);
    }
    else if (aFlag == "-xy")
    {
      if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const TCollection_AsciiString anX (theArgVec[++anArgIter]);
      const TCollection_AsciiString anY (theArgVec[++anArgIter]);
      if (!anX.IsIntegerValue()
       || !anY.IsIntegerValue())
      {
        Message::SendFail ("Syntax error: coordinates should be integer values");
        return 1;
      }

      aColorScale->SetPosition (anX.IntegerValue(), anY.IntegerValue());
    }
    else if (aFlag == "-width"
          || aFlag == "-w"
          || aFlag == "-breadth")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const TCollection_AsciiString aBreadth (theArgVec[++anArgIter]);
      if (!aBreadth.IsIntegerValue())
      {
        Message::SendFail ("Syntax error: a width should be an integer value");
        return 1;
      }
      aColorScale->SetBreadth (aBreadth.IntegerValue());
    }
    else if (aFlag == "-height"
          || aFlag == "-h")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const TCollection_AsciiString aHeight (theArgVec[++anArgIter]);
      if (!aHeight.IsIntegerValue())
      {
        Message::SendFail ("Syntax error: a width should be an integer value");
        return 1;
      }
      aColorScale->SetHeight (aHeight.IntegerValue());
    }
    else if (aFlag == "-color")
    {
      if (aColorScale->GetColorType() != Aspect_TOCSD_USER)
      {
        Message::SendFail ("Syntax error: wrong color type. Call -colors before to set user-specified colors");
        return 1;
      }
      else if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const TCollection_AsciiString anInd (theArgVec[++anArgIter]);
      if (!anInd.IsIntegerValue())
      {
        Message::SendFail ("Syntax error: Index value should be integer");
        return 1;
      }
      const Standard_Integer anIndex = anInd.IntegerValue();
      if (anIndex <= 0 || anIndex > aColorScale->GetNumberOfIntervals())
      {
        Message::SendFail() << "Syntax error: Index value should be within range 1.." << aColorScale->GetNumberOfIntervals();
        return 1;
      }

      Quantity_Color aColor;
      Standard_Integer aNbParsed = Draw::ParseColor (theArgNb  - (anArgIter + 1),
                                                     theArgVec + (anArgIter + 1),
                                                     aColor);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }
      aColorScale->SetIntervalColor (aColor, anIndex);
      aColorScale->SetColorType (Aspect_TOCSD_USER);
      anArgIter += aNbParsed;
    }
    else if (aFlag == "-label")
    {
      if (aColorScale->GetColorType() != Aspect_TOCSD_USER)
      {
        Message::SendFail ("Syntax error: wrong label type. Call -labels before to set user-specified labels");
        return 1;
      }
      else if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      Standard_Integer anIndex = Draw::Atoi (theArgVec[anArgIter + 1]);
      if (anIndex <= 0 || anIndex > aColorScale->GetNumberOfIntervals() + 1)
      {
        Message::SendFail() << "Syntax error: Index value should be within range 1.." << (aColorScale->GetNumberOfIntervals() + 1);
        return 1;
      }

      TCollection_ExtendedString aText (theArgVec[anArgIter + 2], Standard_True);
      aColorScale->SetLabel     (aText, anIndex);
      aColorScale->SetLabelType (Aspect_TOCSD_USER);
      anArgIter += 2;
    }
    else if (aFlag == "-labelat"
          || aFlag == "-labat"
          || aFlag == "-labelatborder"
          || aFlag == "-labatborder"
          || aFlag == "-labelatcenter"
          || aFlag == "-labatcenter")
    {
      Standard_Boolean toEnable = Standard_True;
      if (aFlag == "-labelat"
       || aFlag == "-labat")
      {
        Standard_Integer aLabAtBorder = -1;
        if (++anArgIter >= theArgNb)
        {
          TCollection_AsciiString anAtBorder (theArgVec[anArgIter]);
          anAtBorder.LowerCase();
          if (anAtBorder == "border")
          {
            aLabAtBorder = 1;
          }
          else if (anAtBorder == "center")
          {
            aLabAtBorder = 0;
          }
        }
        if (aLabAtBorder == -1)
        {
          Message::SendFail() << "Syntax error at argument '" << anArg << "'";
          return 1;
        }
        toEnable = (aLabAtBorder == 1);
      }
      else if (anArgIter + 1 < theArgNb
            && Draw::ParseOnOff (theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      aColorScale->SetLabelAtBorder (aFlag == "-labelatcenter"
                                  || aFlag == "-labatcenter"
                                   ? !toEnable
                                   :  toEnable);
    }
    else if (aFlag == "-colors")
    {
      Aspect_SequenceOfColor aSeq;
      for (;;)
      {
        Quantity_Color aColor;
        Standard_Integer aNbParsed = Draw::ParseColor (theArgNb  - (anArgIter + 1),
                                                       theArgVec + (anArgIter + 1),
                                                       aColor);
        if (aNbParsed == 0)
        {
          break;
        }
        anArgIter += aNbParsed;
        aSeq.Append (aColor);
      }
      if (aSeq.Length() != aColorScale->GetNumberOfIntervals())
      {
        Message::SendFail() << "Error: not enough arguments! You should provide color names or RGB color values for every interval of the "
                            << aColorScale->GetNumberOfIntervals() << " intervals";
        return 1;
      }

      aColorScale->SetColors    (aSeq);
      aColorScale->SetColorType (Aspect_TOCSD_USER);
    }
    else if (aFlag == "-uniform")
    {
      const Standard_Real aLightness = Draw::Atof (theArgVec[++anArgIter]);
      const Standard_Real aHueStart = Draw::Atof (theArgVec[++anArgIter]);
      const Standard_Real aHueEnd = Draw::Atof (theArgVec[++anArgIter]);
      aColorScale->SetUniformColors (aLightness, aHueStart, aHueEnd);
      aColorScale->SetColorType (Aspect_TOCSD_USER);
    }
    else if (aFlag == "-labels"
          || aFlag == "-freelabels")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      Standard_Integer aNbLabels = aColorScale->IsLabelAtBorder()
                                 ? aColorScale->GetNumberOfIntervals() + 1
                                 : aColorScale->GetNumberOfIntervals();
      if (aFlag == "-freelabels")
      {
        ++anArgIter;
        aNbLabels = Draw::Atoi (theArgVec[anArgIter]);
      }
      if (anArgIter + aNbLabels >= theArgNb)
      {
        Message::SendFail() << "Syntax error: not enough arguments. " << aNbLabels << " text labels are expected";
        return 1;
      }

      TColStd_SequenceOfExtendedString aSeq;
      for (Standard_Integer aLabelIter = 0; aLabelIter < aNbLabels; ++aLabelIter)
      {
        aSeq.Append (TCollection_ExtendedString (theArgVec[++anArgIter], Standard_True));
      }
      aColorScale->SetLabels (aSeq);
      aColorScale->SetLabelType (Aspect_TOCSD_USER);
    }
    else if (aFlag == "-title")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      Standard_Boolean isTwoArgs = Standard_False;
      if (anArgIter + 2 < theArgNb)
      {
        TCollection_AsciiString aSecondArg (theArgVec[anArgIter + 2]);
        aSecondArg.LowerCase();
      Standard_DISABLE_DEPRECATION_WARNINGS
        if (aSecondArg == "none")
        {
          aColorScale->SetTitlePosition (Aspect_TOCSP_NONE);
          isTwoArgs = Standard_True;
        }
        else if (aSecondArg == "left")
        {
          aColorScale->SetTitlePosition (Aspect_TOCSP_LEFT);
          isTwoArgs = Standard_True;
        }
        else if (aSecondArg == "right")
        {
          aColorScale->SetTitlePosition (Aspect_TOCSP_RIGHT);
          isTwoArgs = Standard_True;
        }
        else if (aSecondArg == "center")
        {
          aColorScale->SetTitlePosition (Aspect_TOCSP_CENTER);
          isTwoArgs = Standard_True;
        }
      Standard_ENABLE_DEPRECATION_WARNINGS
      }

      TCollection_ExtendedString aTitle(theArgVec[anArgIter + 1], Standard_True);
      aColorScale->SetTitle (aTitle);
      if (isTwoArgs)
      {
        anArgIter += 1;
      }
      anArgIter += 1;
    }
    else if (aFlag == "-demoversion"
          || aFlag == "-demo")
    {
      aColorScale->SetPosition (0, 0);
      aColorScale->SetTextHeight (16);
      aColorScale->SetRange (0.0, 100.0);
      aColorScale->SetNumberOfIntervals (10);
      aColorScale->SetBreadth (0);
      aColorScale->SetHeight  (0);
      aColorScale->SetLabelPosition (Aspect_TOCSP_RIGHT);
      aColorScale->SetColorType (Aspect_TOCSD_AUTO);
      aColorScale->SetLabelType (Aspect_TOCSD_AUTO);
    }
    else if (aFlag == "-findcolor")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString anArg1 (theArgVec[++anArgIter]);

      if (!anArg1.IsRealValue())
      {
        Message::SendFail ("Syntax error: the value should be real");
        return 1;
      }

      Quantity_Color aColor;
      aColorScale->FindColor (anArg1.RealValue(), aColor);
      theDI << Quantity_Color::StringName (aColor.Name());
      return 0;
    }
    else
    {
      Message::SendFail() << "Syntax error at " << anArg << " - unknown argument";
      return 1;
    }
  }

  Standard_Integer aWinWidth = 0, aWinHeight = 0;
  aView->Window()->Size (aWinWidth, aWinHeight);
  if (aColorScale->GetBreadth() == 0)
  {
    aColorScale->SetBreadth (aWinWidth);
  }
  if (aColorScale->GetHeight() == 0)
  {
    aColorScale->SetHeight (aWinHeight);
  }
  aColorScale->SetToUpdate();
  ViewerTest::Display (theArgVec[1], aColorScale, Standard_False, Standard_True);
  return 0;
}

//==============================================================================
//function : VGraduatedTrihedron
//purpose  : Displays or hides a graduated trihedron
//==============================================================================
static Standard_Boolean GetColor (const TCollection_AsciiString& theValue,
                                  Quantity_Color& theColor)
{
  Quantity_NameOfColor aColorName;
  TCollection_AsciiString aVal = theValue;
  aVal.UpperCase();
  if (!Quantity_Color::ColorFromName (aVal.ToCString(), aColorName))
  {
    return Standard_False;
  }
  theColor = Quantity_Color (aColorName);
  return Standard_True;
}

static int VGraduatedTrihedron (Draw_Interpretor& /*theDi*/, Standard_Integer theArgNum, const char** theArgs)
{
  if (theArgNum < 2)
  {
    Message::SendFail() << "Syntax error: wrong number of parameters. Type 'help"
                        << theArgs[0] <<"' for more information";
    return 1;
  }

  NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HSequenceOfAsciiString)> aMapOfArgs;
  TCollection_AsciiString aParseKey;
  for (Standard_Integer anArgIt = 1; anArgIt < theArgNum; ++anArgIt)
  {
    TCollection_AsciiString anArg (theArgs [anArgIt]);

    if (anArg.Value (1) == '-' && !anArg.IsRealValue())
    {
      aParseKey = anArg;
      aParseKey.Remove (1);
      aParseKey.LowerCase();
      aMapOfArgs.Bind (aParseKey, new TColStd_HSequenceOfAsciiString);
      continue;
    }

    if (aParseKey.IsEmpty())
    {
      continue;
    }

    aMapOfArgs(aParseKey)->Append (anArg);
  }

  // Check parameters
  for (NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HSequenceOfAsciiString)>::Iterator aMapIt (aMapOfArgs);
       aMapIt.More(); aMapIt.Next())
  {
    const TCollection_AsciiString& aKey = aMapIt.Key();
    const Handle(TColStd_HSequenceOfAsciiString)& anArgs = aMapIt.Value();

    // Bool key, without arguments
    if ((aKey.IsEqual ("on") || aKey.IsEqual ("off"))
        && anArgs->IsEmpty())
    {
      continue;
    }

    // One argument
    if ( (aKey.IsEqual ("xname") || aKey.IsEqual ("yname") || aKey.IsEqual ("zname"))
          && anArgs->Length() == 1)
    {
      continue;
    }

    // On/off arguments
    if ((aKey.IsEqual ("xdrawname") || aKey.IsEqual ("ydrawname") || aKey.IsEqual ("zdrawname")
        || aKey.IsEqual ("xdrawticks") || aKey.IsEqual ("ydrawticks") || aKey.IsEqual ("zdrawticks")
        || aKey.IsEqual ("xdrawvalues") || aKey.IsEqual ("ydrawvalues") || aKey.IsEqual ("zdrawvalues")
        || aKey.IsEqual ("drawgrid") || aKey.IsEqual ("drawaxes"))
        && anArgs->Length() == 1 && (anArgs->Value(1).IsEqual ("on") || anArgs->Value(1).IsEqual ("off")))
    {
      continue;
    }

    // One string argument
    if ( (aKey.IsEqual ("xnamecolor") || aKey.IsEqual ("ynamecolor") || aKey.IsEqual ("znamecolor")
          || aKey.IsEqual ("xcolor") || aKey.IsEqual ("ycolor") || aKey.IsEqual ("zcolor"))
          && anArgs->Length() == 1 && !anArgs->Value(1).IsIntegerValue() && !anArgs->Value(1).IsRealValue())
    {
      continue;
    }

    // One integer argument
    if ( (aKey.IsEqual ("xticks") || aKey.IsEqual ("yticks") || aKey.IsEqual ("zticks")
          || aKey.IsEqual ("xticklength") || aKey.IsEqual ("yticklength") || aKey.IsEqual ("zticklength")
          || aKey.IsEqual ("xnameoffset") || aKey.IsEqual ("ynameoffset") || aKey.IsEqual ("znameoffset")
          || aKey.IsEqual ("xvaluesoffset") || aKey.IsEqual ("yvaluesoffset") || aKey.IsEqual ("zvaluesoffset"))
         && anArgs->Length() == 1 && anArgs->Value(1).IsIntegerValue())
    {
      continue;
    }

    // One real argument
    if ( aKey.IsEqual ("arrowlength")
         && anArgs->Length() == 1 && (anArgs->Value(1).IsIntegerValue() || anArgs->Value(1).IsRealValue()))
    {
      continue;
    }

    // Two string arguments
    if ( (aKey.IsEqual ("namefont") || aKey.IsEqual ("valuesfont"))
         && anArgs->Length() == 1 && !anArgs->Value(1).IsIntegerValue() && !anArgs->Value(1).IsRealValue())
    {
      continue;
    }

    TCollection_AsciiString aLowerKey;
    aLowerKey  = "-";
    aLowerKey += aKey;
    aLowerKey.LowerCase();
    Message::SendFail() << "Syntax error: " << aLowerKey << " is unknown option, or the arguments are unacceptable.\n"
                        << "Type help for more information";
    return 1;
  }

  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  if (anAISContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Standard_Boolean toDisplay = Standard_True;
  Quantity_Color aColor;
  Graphic3d_GraduatedTrihedron aTrihedronData;
  // Process parameters
  Handle(TColStd_HSequenceOfAsciiString) aValues;
  if (aMapOfArgs.Find ("off", aValues))
  {
    toDisplay = Standard_False;
  }

  // AXES NAMES
  if (aMapOfArgs.Find ("xname", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetName (aValues->Value(1));
  }
  if (aMapOfArgs.Find ("yname", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetName (aValues->Value(1));
  }
  if (aMapOfArgs.Find ("zname", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetName (aValues->Value(1));
  }
  if (aMapOfArgs.Find ("xdrawname", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetDrawName (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("ydrawname", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetDrawName (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("zdrawname", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetDrawName (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("xnameoffset", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetNameOffset (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("ynameoffset", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetNameOffset (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("znameoffset", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetNameOffset (aValues->Value(1).IntegerValue());
  }

  // COLORS
  if (aMapOfArgs.Find ("xnamecolor", aValues))
  {
    if (!GetColor (aValues->Value(1), aColor))
    {
      Message::SendFail ("Syntax error: -xnamecolor wrong color name");
      return 1;
    }
    aTrihedronData.ChangeXAxisAspect().SetNameColor (aColor);
  }
  if (aMapOfArgs.Find ("ynamecolor", aValues))
  {
    if (!GetColor (aValues->Value(1), aColor))
    {
      Message::SendFail ("Syntax error: -ynamecolor wrong color name");
      return 1;
    }
    aTrihedronData.ChangeYAxisAspect().SetNameColor (aColor);
  }
  if (aMapOfArgs.Find ("znamecolor", aValues))
  {
    if (!GetColor (aValues->Value(1), aColor))
    {
      Message::SendFail ("Syntax error: -znamecolor wrong color name");
      return 1;
    }
    aTrihedronData.ChangeZAxisAspect().SetNameColor (aColor);
  }
  if (aMapOfArgs.Find ("xcolor", aValues))
  {
    if (!GetColor (aValues->Value(1), aColor))
    {
      Message::SendFail ("Syntax error: -xcolor wrong color name");
      return 1;
    }
    aTrihedronData.ChangeXAxisAspect().SetColor (aColor);
  }
  if (aMapOfArgs.Find ("ycolor", aValues))
  {
    if (!GetColor (aValues->Value(1), aColor))
    {
      Message::SendFail ("Syntax error: -ycolor wrong color name");
      return 1;
    }
    aTrihedronData.ChangeYAxisAspect().SetColor (aColor);
  }
  if (aMapOfArgs.Find ("zcolor", aValues))
  {
    if (!GetColor (aValues->Value(1), aColor))
    {
      Message::SendFail ("Syntax error: -zcolor wrong color name");
      return 1;
    }
    aTrihedronData.ChangeZAxisAspect().SetColor (aColor);
  }

  // TICKMARKS
  if (aMapOfArgs.Find ("xticks", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetTickmarksNumber (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("yticks", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetTickmarksNumber (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("zticks", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetTickmarksNumber (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("xticklength", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetTickmarksLength (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("yticklength", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetTickmarksLength (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("zticklength", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetTickmarksLength (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("xdrawticks", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetDrawTickmarks (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("ydrawticks", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetDrawTickmarks (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("zdrawticks", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetDrawTickmarks (aValues->Value(1).IsEqual ("on"));
  }

  // VALUES
  if (aMapOfArgs.Find ("xdrawvalues", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetDrawValues (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("ydrawvalues", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetDrawValues (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("zdrawvalues", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetDrawValues (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("xvaluesoffset", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetValuesOffset (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("yvaluesoffset", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetValuesOffset (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("zvaluesoffset", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetValuesOffset (aValues->Value(1).IntegerValue());
  }

  // ARROWS
  if (aMapOfArgs.Find ("arrowlength", aValues))
  {
    aTrihedronData.SetArrowsLength ((Standard_ShortReal) aValues->Value(1).RealValue());
  }

  // FONTS
  if (aMapOfArgs.Find ("namefont", aValues))
  {
    aTrihedronData.SetNamesFont (aValues->Value(1));
  }
  if (aMapOfArgs.Find ("valuesfont", aValues))
  {
    aTrihedronData.SetValuesFont (aValues->Value(1));
  }

  if (aMapOfArgs.Find ("drawgrid", aValues))
  {
    aTrihedronData.SetDrawGrid (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("drawaxes", aValues))
  {
    aTrihedronData.SetDrawAxes (aValues->Value(1).IsEqual ("on"));
  }

  // The final step: display of erase trihedron
  if (toDisplay)
  {
    ViewerTest::CurrentView()->GraduatedTrihedronDisplay (aTrihedronData);
  }
  else
  {
    ViewerTest::CurrentView()->GraduatedTrihedronErase();
  }

  ViewerTest::GetAISContext()->UpdateCurrentViewer();
  ViewerTest::CurrentView()->Redraw();

  return 0;
}

//==============================================================================
//function : VTile
//purpose  :
//==============================================================================
static int VTile (Draw_Interpretor& theDI,
                  Standard_Integer  theArgNb,
                  const char**      theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Graphic3d_CameraTile aTile = aView->Camera()->Tile();
  if (theArgNb < 2)
  {
    theDI << "Total size: " << aTile.TotalSize.x() << " " << aTile.TotalSize.y() << "\n"
          << "Tile  size: " << aTile.TileSize.x()  << " " << aTile.TileSize.y()  << "\n"
          << "Lower left: " << aTile.Offset.x()    << " " << aTile.Offset.y()    << "\n";
    return 0;
  }

  aView->Window()->Size (aTile.TileSize.x(), aTile.TileSize.y());
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-lowerleft"
     || anArg == "-upperleft")
    {
      if (anArgIter + 3 < theArgNb)
      {
        Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }
      aTile.IsTopDown = (anArg == "-upperleft") == Standard_True;
      aTile.Offset.x() = Draw::Atoi (theArgVec[anArgIter + 1]);
      aTile.Offset.y() = Draw::Atoi (theArgVec[anArgIter + 2]);
    }
    else if (anArg == "-total"
          || anArg == "-totalsize"
          || anArg == "-viewsize")
    {
      if (anArgIter + 3 < theArgNb)
      {
        Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }
      aTile.TotalSize.x() = Draw::Atoi (theArgVec[anArgIter + 1]);
      aTile.TotalSize.y() = Draw::Atoi (theArgVec[anArgIter + 2]);
      if (aTile.TotalSize.x() < 1
       || aTile.TotalSize.y() < 1)
      {
        Message::SendFail ("Error: total size is incorrect");
        return 1;
      }
    }
    else if (anArg == "-tilesize")
    {
      if (anArgIter + 3 < theArgNb)
      {
        Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }

      aTile.TileSize.x() = Draw::Atoi (theArgVec[anArgIter + 1]);
      aTile.TileSize.y() = Draw::Atoi (theArgVec[anArgIter + 2]);
      if (aTile.TileSize.x() < 1
       || aTile.TileSize.y() < 1)
      {
        Message::SendFail ("Error: tile size is incorrect");
        return 1;
      }
    }
    else if (anArg == "-unset")
    {
      aView->Camera()->SetTile (Graphic3d_CameraTile());
      aView->Redraw();
      return 0;
    }
  }

  if (aTile.TileSize.x() < 1
   || aTile.TileSize.y() < 1)
  {
    Message::SendFail ("Error: tile size is undefined");
    return 1;
  }
  else if (aTile.TotalSize.x() < 1
        || aTile.TotalSize.y() < 1)
  {
    Message::SendFail ("Error: total size is undefined");
    return 1;
  }

  aView->Camera()->SetTile (aTile);
  aView->Redraw();
  return 0;
}

//! Format ZLayer ID.
inline const char* formZLayerId (const Standard_Integer theLayerId)
{
  switch (theLayerId)
  {
    case Graphic3d_ZLayerId_UNKNOWN: return "[INVALID]";
    case Graphic3d_ZLayerId_Default: return "[DEFAULT]";
    case Graphic3d_ZLayerId_Top:     return "[TOP]";
    case Graphic3d_ZLayerId_Topmost: return "[TOPMOST]";
    case Graphic3d_ZLayerId_TopOSD:  return "[OVERLAY]";
    case Graphic3d_ZLayerId_BotOSD:  return "[UNDERLAY]";
  }
  return "";
}

//! Print the ZLayer information.
inline void printZLayerInfo (Draw_Interpretor& theDI,
                             const Graphic3d_ZLayerSettings& theLayer)
{
  if (!theLayer.Name().IsEmpty())
  {
    theDI << "  Name: " << theLayer.Name() << "\n";
  }
  if (theLayer.IsImmediate())
  {
    theDI << "  Immediate: TRUE\n";
  }
  theDI << "  Origin: " << theLayer.Origin().X() << " " << theLayer.Origin().Y() << " " << theLayer.Origin().Z() << "\n";
  theDI << "  Culling distance: "      << theLayer.CullingDistance() << "\n";
  theDI << "  Culling size: "          << theLayer.CullingSize() << "\n";
  theDI << "  Depth test:   "          << (theLayer.ToEnableDepthTest() ? "enabled" : "disabled") << "\n";
  theDI << "  Depth write:  "          << (theLayer.ToEnableDepthWrite() ? "enabled" : "disabled") << "\n";
  theDI << "  Depth buffer clearing: " << (theLayer.ToClearDepth() ? "enabled" : "disabled") << "\n";
  if (theLayer.PolygonOffset().Mode != Aspect_POM_None)
  {
    theDI << "  Depth offset: " << theLayer.PolygonOffset().Factor << " " << theLayer.PolygonOffset().Units << "\n";
  }
}

//==============================================================================
//function : VZLayer
//purpose  : Test z layer operations for v3d viewer
//==============================================================================
static int VZLayer (Draw_Interpretor& theDI,
                    Standard_Integer  theArgNb,
                    const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  const Handle(V3d_Viewer)& aViewer = aContextAIS->CurrentViewer();
  if (theArgNb < 2)
  {
    TColStd_SequenceOfInteger aLayers;
    aViewer->GetAllZLayers (aLayers);
    for (TColStd_SequenceOfInteger::Iterator aLayeriter (aLayers); aLayeriter.More(); aLayeriter.Next())
    {
      theDI << "ZLayer " << aLayeriter.Value() << " " << formZLayerId (aLayeriter.Value()) << "\n";
      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (aLayeriter.Value());
      printZLayerInfo (theDI, aSettings);
    }
    return 1;
  }

  Standard_Integer anArgIter = 1;
  Standard_Integer aLayerId = Graphic3d_ZLayerId_UNKNOWN;
  ViewerTest_AutoUpdater anUpdateTool (aContextAIS, ViewerTest::CurrentView());
  if (anUpdateTool.parseRedrawMode (theArgVec[anArgIter]))
  {
    ++anArgIter;
  }

  {
    TCollection_AsciiString aFirstArg (theArgVec[anArgIter]);
    if (aFirstArg.IsIntegerValue())
    {
      ++anArgIter;
      aLayerId = aFirstArg.IntegerValue();
    }
    else
    {
      if (ViewerTest::ParseZLayerName (aFirstArg.ToCString(), aLayerId))
      {
        ++anArgIter;
      }
    }
  }

  Graphic3d_ZLayerId anOtherLayerId = Graphic3d_ZLayerId_UNKNOWN;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    // perform operation
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      //
    }
    else if (anArg == "-add"
          || anArg == "add")
    {
      aLayerId = Graphic3d_ZLayerId_UNKNOWN;
      if (!aViewer->AddZLayer (aLayerId))
      {
        Message::SendFail ("Error: can not add a new z layer");
        return 0;
      }

      theDI << aLayerId;
    }
    else if (anArg == "-insertbefore"
          && anArgIter + 1 < theArgNb
          && ViewerTest::ParseZLayer (theArgVec[anArgIter + 1], anOtherLayerId))
    {
      ++anArgIter;
      aLayerId = Graphic3d_ZLayerId_UNKNOWN;
      if (!aViewer->InsertLayerBefore (aLayerId, Graphic3d_ZLayerSettings(), anOtherLayerId))
      {
        Message::SendFail ("Error: can not add a new z layer");
        return 0;
      }

      theDI << aLayerId;
    }
    else if (anArg == "-insertafter"
          && anArgIter + 1 < theArgNb
          && ViewerTest::ParseZLayer (theArgVec[anArgIter + 1], anOtherLayerId))
    {
      ++anArgIter;
      aLayerId = Graphic3d_ZLayerId_UNKNOWN;
      if (!aViewer->InsertLayerAfter (aLayerId, Graphic3d_ZLayerSettings(), anOtherLayerId))
      {
        Message::SendFail ("Error: can not add a new z layer");
        return 0;
      }

      theDI << aLayerId;
    }
    else if (anArg == "-del"
          || anArg == "-delete"
          || anArg == "del")
    {
      if (aLayerId == Graphic3d_ZLayerId_UNKNOWN)
      {
        if (++anArgIter >= theArgNb)
        {
          Message::SendFail ("Syntax error: id of z layer to remove is missing");
          return 1;
        }

        aLayerId = Draw::Atoi (theArgVec[anArgIter]);
      }

      if (aLayerId == Graphic3d_ZLayerId_UNKNOWN
       || aLayerId == Graphic3d_ZLayerId_Default
       || aLayerId == Graphic3d_ZLayerId_Top
       || aLayerId == Graphic3d_ZLayerId_Topmost
       || aLayerId == Graphic3d_ZLayerId_TopOSD
       || aLayerId == Graphic3d_ZLayerId_BotOSD)
      {
        Message::SendFail ("Syntax error: standard Z layer can not be removed");
        return 1;
      }

      // move all object displayed in removing layer to default layer
      for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anObjIter (GetMapOfAIS());
           anObjIter.More(); anObjIter.Next())
      {
        const Handle(AIS_InteractiveObject)& aPrs = anObjIter.Key1();
        if (aPrs.IsNull()
         || aPrs->ZLayer() != aLayerId)
        {
          continue;
        }
        aPrs->SetZLayer (Graphic3d_ZLayerId_Default);
      }

      if (!aViewer->RemoveZLayer (aLayerId))
      {
        Message::SendFail ("Z layer can not be removed");
      }
      else
      {
        theDI << aLayerId << " ";
      }
    }
    else if (anArg == "-get"
          || anArg == "get")
    {
      TColStd_SequenceOfInteger aLayers;
      aViewer->GetAllZLayers (aLayers);
      for (TColStd_SequenceOfInteger::Iterator aLayeriter (aLayers); aLayeriter.More(); aLayeriter.Next())
      {
        theDI << aLayeriter.Value() << " ";
      }

      theDI << "\n";
    }
    else if (anArg == "-name")
    {
      if (aLayerId == Graphic3d_ZLayerId_UNKNOWN)
      {
        Message::SendFail ("Syntax error: id of Z layer is missing");
        return 1;
      }

      if (++anArgIter >= theArgNb)
      {
        Message::SendFail ("Syntax error: name is missing");
        return 1;
      }

      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (aLayerId);
      aSettings.SetName (theArgVec[anArgIter]);
      aViewer->SetZLayerSettings (aLayerId, aSettings);
    }
    else if (anArg == "-origin")
    {
      if (aLayerId == Graphic3d_ZLayerId_UNKNOWN)
      {
        Message::SendFail ("Syntax error: id of Z layer is missing");
        return 1;
      }

      if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail ("Syntax error: origin coordinates are missing");
        return 1;
      }

      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (aLayerId);
      gp_XYZ anOrigin;
      anOrigin.SetX (Draw::Atof (theArgVec[anArgIter + 1]));
      anOrigin.SetY (Draw::Atof (theArgVec[anArgIter + 2]));
      anOrigin.SetZ (0.0);
      if (anArgIter + 3 < theArgNb)
      {
        anOrigin.SetZ (Draw::Atof (theArgVec[anArgIter + 3]));
        anArgIter += 3;
      }
      else
      {
        anArgIter += 2;
      }
      aSettings.SetOrigin (anOrigin);
      aViewer->SetZLayerSettings (aLayerId, aSettings);
    }
    else if (aLayerId != Graphic3d_ZLayerId_UNKNOWN
          && anArgIter + 1 < theArgNb
          && (anArg == "-cullingdistance"
           || anArg == "-cullingdist"
           || anArg == "-culldistance"
           || anArg == "-culldist"
           || anArg == "-distcull"
           || anArg == "-distculling"
           || anArg == "-distanceculling"))
    {
      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (aLayerId);
      const Standard_Real aDist = Draw::Atof (theArgVec[++anArgIter]);
      aSettings.SetCullingDistance (aDist);
      aViewer->SetZLayerSettings (aLayerId, aSettings);
    }
    else if (aLayerId != Graphic3d_ZLayerId_UNKNOWN
          && anArgIter + 1 < theArgNb
          && (anArg == "-cullingsize"
           || anArg == "-cullsize"
           || anArg == "-sizecull"
           || anArg == "-sizeculling"))
    {
      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (aLayerId);
      const Standard_Real aSize = Draw::Atof (theArgVec[++anArgIter]);
      aSettings.SetCullingSize (aSize);
      aViewer->SetZLayerSettings (aLayerId, aSettings);
    }
    else if (anArg == "-settings"
          || anArg == "settings")
    {
      if (aLayerId == Graphic3d_ZLayerId_UNKNOWN)
      {
        if (++anArgIter >= theArgNb)
        {
          Message::SendFail ("Syntax error: id of Z layer is missing");
          return 1;
        }

        aLayerId = Draw::Atoi (theArgVec[anArgIter]);
      }

      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (aLayerId);
      printZLayerInfo (theDI, aSettings);
    }
    else if (anArg == "-enable"
          || anArg == "enable"
          || anArg == "-disable"
          || anArg == "disable")
    {
      const Standard_Boolean toEnable = anArg == "-enable"
                                     || anArg == "enable";
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail ("Syntax error: option name is missing");
        return 1;
      }

      TCollection_AsciiString aSubOp (theArgVec[anArgIter]);
      aSubOp.LowerCase();
      if (aLayerId == Graphic3d_ZLayerId_UNKNOWN)
      {
        if (++anArgIter >= theArgNb)
        {
          Message::SendFail ("Syntax error: id of Z layer is missing");
          return 1;
        }

        aLayerId = Draw::Atoi (theArgVec[anArgIter]);
      }

      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (aLayerId);
      if (aSubOp == "depthtest"
       || aSubOp == "test")
      {
        aSettings.SetEnableDepthTest (toEnable);
      }
      else if (aSubOp == "depthwrite"
            || aSubOp == "write")
      {
        aSettings.SetEnableDepthWrite (toEnable);
      }
      else if (aSubOp == "depthclear"
            || aSubOp == "clear")
      {
        aSettings.SetClearDepth (toEnable);
      }
      else if (aSubOp == "depthoffset"
            || aSubOp == "offset")
      {
        Graphic3d_PolygonOffset aParams;
        aParams.Mode = toEnable ? Aspect_POM_Fill : Aspect_POM_None;
        if (toEnable)
        {
          if (anArgIter + 2 >= theArgNb)
          {
            Message::SendFail ("Syntax error: factor and units values for depth offset are missing");
            return 1;
          }

          aParams.Factor = static_cast<Standard_ShortReal> (Draw::Atof (theArgVec[++anArgIter]));
          aParams.Units  = static_cast<Standard_ShortReal> (Draw::Atof (theArgVec[++anArgIter]));
        }
        aSettings.SetPolygonOffset (aParams);
      }
      else if (aSubOp == "positiveoffset"
            || aSubOp == "poffset")
      {
        if (toEnable)
        {
          aSettings.SetDepthOffsetPositive();
        }
        else
        {
          aSettings.SetPolygonOffset (Graphic3d_PolygonOffset());
        }
      }
      else if (aSubOp == "negativeoffset"
            || aSubOp == "noffset")
      {
        if (toEnable)
        {
          aSettings.SetDepthOffsetNegative();
        }
        else
        {
          aSettings.SetPolygonOffset(Graphic3d_PolygonOffset());
        }
      }
      else if (aSubOp == "textureenv")
      {
        aSettings.SetEnvironmentTexture (toEnable);
      }
      else if (aSubOp == "raytracing")
      {
        aSettings.SetRaytracable (toEnable);
      }

      aViewer->SetZLayerSettings (aLayerId, aSettings);
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown option " << theArgVec[anArgIter];
      return 1;
    }
  }

  return 0;
}

// The interactive presentation of 2d layer item
// for "vlayerline" command it provides a presentation of
// line with user-defined linewidth, linetype and transparency.
class V3d_LineItem : public AIS_InteractiveObject
{
public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI_INLINE(V3d_LineItem,AIS_InteractiveObject)

  // constructor
  Standard_EXPORT V3d_LineItem(Standard_Real X1, Standard_Real Y1,
                               Standard_Real X2, Standard_Real Y2,
                               Aspect_TypeOfLine theType = Aspect_TOL_SOLID,
                               Standard_Real theWidth    = 0.5,
                               Standard_Real theTransp   = 1.0);

  private:

  void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                const Handle(Prs3d_Presentation)& thePresentation,
                const Standard_Integer theMode) Standard_OVERRIDE;

  void ComputeSelection (const Handle(SelectMgr_Selection)& /*aSelection*/,
                         const Standard_Integer /*aMode*/) Standard_OVERRIDE
  {}

private:

  Standard_Real       myX1, myY1, myX2, myY2;
  Aspect_TypeOfLine   myType;
  Standard_Real       myWidth;
};

// default constructor for line item
V3d_LineItem::V3d_LineItem(Standard_Real X1, Standard_Real Y1,
                           Standard_Real X2, Standard_Real Y2,
                           Aspect_TypeOfLine theType,
                           Standard_Real theWidth,
                           Standard_Real theTransp) :
  myX1(X1), myY1(Y1), myX2(X2), myY2(Y2),
  myType(theType), myWidth(theWidth)
{
  SetTransparency (1-theTransp);
}

// render line
void V3d_LineItem::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePresentationManager*/,
                            const Handle(Prs3d_Presentation)& thePresentation,
                            const Standard_Integer /*theMode*/)
{
  thePresentation->Clear();
  Quantity_Color aColor (Quantity_NOC_RED);
  Standard_Integer aWidth, aHeight;
  ViewerTest::CurrentView()->Window()->Size (aWidth, aHeight);
  Handle(Graphic3d_Group) aGroup = thePresentation->CurrentGroup();
  Handle(Graphic3d_ArrayOfPolylines) aPrim = new Graphic3d_ArrayOfPolylines(5);
  aPrim->AddVertex(myX1, aHeight-myY1, 0.);
  aPrim->AddVertex(myX2, aHeight-myY2, 0.);
  Handle(Prs3d_LineAspect) anAspect = new Prs3d_LineAspect (aColor, (Aspect_TypeOfLine)myType, myWidth);
  aGroup->SetPrimitivesAspect (anAspect->Aspect());
  aGroup->AddPrimitiveArray (aPrim);
}

//=============================================================================
//function : VLayerLine
//purpose  : Draws line in the v3d view layer with given attributes: linetype,
//         : linewidth, transparency coefficient
//============================================================================
static int VLayerLine(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    di << "Call vinit before!\n";
    return 1;
  }
  else if (argc < 5)
  {
    di << "Use: " << argv[0];
    di << " x1 y1 x2 y2 [linewidth = 0.5] [linetype = 0] [transparency = 1]\n";
    di << " linetype : { 0 | 1 | 2 | 3 } \n";
    di << "              0 - solid  \n";
    di << "              1 - dashed \n";
    di << "              2 - dot    \n";
    di << "              3 - dashdot\n";
    di << " transparency : { 0.0 - 1.0 } \n";
    di << "                  0.0 - transparent\n";
    di << "                  1.0 - visible    \n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  // get the input params
  Standard_Real X1 = Draw::Atof(argv[1]);
  Standard_Real Y1 = Draw::Atof(argv[2]);
  Standard_Real X2 = Draw::Atof(argv[3]);
  Standard_Real Y2 = Draw::Atof(argv[4]);

  Standard_Real aWidth = 0.5;
  Standard_Real aTransparency = 1.0;

  // has width
  if (argc > 5)
    aWidth = Draw::Atof(argv[5]);

  // select appropriate line type
  Aspect_TypeOfLine aLineType = Aspect_TOL_SOLID;
  if (argc > 6
  && !ViewerTest::ParseLineType (argv[6], aLineType))
  {
    Message::SendFail() << "Syntax error: unknown line type '" << argv[6] << "'";
    return 1;
  }

  // has transparency
  if (argc > 7)
  {
    aTransparency = Draw::Atof(argv[7]);
    if (aTransparency < 0 || aTransparency > 1.0)
      aTransparency = 1.0;
  }

  static Handle (V3d_LineItem) aLine;
  if (!aLine.IsNull())
  {
    aContext->Erase (aLine, Standard_False);
  }
  aLine = new V3d_LineItem (X1, Y1, X2, Y2,
                            aLineType, aWidth,
                            aTransparency);

  aContext->SetTransformPersistence (aLine, new Graphic3d_TransformPers (Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER));
  aLine->SetZLayer (Graphic3d_ZLayerId_TopOSD);
  aLine->SetToUpdate();
  aContext->Display (aLine, Standard_True);

  return 0;
}


//==============================================================================
//function : VGrid
//purpose  :
//==============================================================================

static int VGrid (Draw_Interpretor& /*theDI*/,
                  Standard_Integer  theArgNb,
                  const char**      theArgVec)
{
  Handle(V3d_View)   aView   = ViewerTest::CurrentView();
  Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
  if (aView.IsNull() || aViewer.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Aspect_GridType     aType = aViewer->GridType();
  Aspect_GridDrawMode aMode = aViewer->GridDrawMode();
  Graphic3d_Vec2d aNewOriginXY, aNewStepXY, aNewSizeXY;
  Standard_Real aNewRotAngle = 0.0, aNewZOffset = 0.0;
  bool hasOrigin = false, hasStep = false, hasRotAngle = false, hasSize = false, hasZOffset = false;
  ViewerTest_AutoUpdater anUpdateTool (ViewerTest::GetAISContext(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode (theArgVec[anArgIter]))
    {
      continue;
    }
    else if (anArgIter + 1 < theArgNb
          && anArg == "-type")
    {
      TCollection_AsciiString anArgNext (theArgVec[++anArgIter]);
      anArgNext.LowerCase();
      if (anArgNext == "r"
       || anArgNext == "rect"
       || anArgNext == "rectangular")
      {
        aType = Aspect_GT_Rectangular;
      }
      else if (anArgNext == "c"
            || anArgNext == "circ"
            || anArgNext == "circular")
      {
        aType = Aspect_GT_Circular;
      }
      else
      {
        Message::SendFail() << "Syntax error at '" << anArgNext << "'";
        return 1;
      }
    }
    else if (anArgIter + 1 < theArgNb
          && anArg == "-mode")
    {
      TCollection_AsciiString anArgNext (theArgVec[++anArgIter]);
      anArgNext.LowerCase();
      if (anArgNext == "l"
       || anArgNext == "line"
       || anArgNext == "lines")
      {
        aMode = Aspect_GDM_Lines;
      }
      else if (anArgNext == "p"
            || anArgNext == "point"
            || anArgNext == "points")
      {
        aMode = Aspect_GDM_Points;
      }
      else
      {
        Message::SendFail() << "Syntax error at '" << anArgNext << "'";
        return 1;
      }
    }
    else if (anArgIter + 2 < theArgNb
          && (anArg == "-origin"
           || anArg == "-orig"))
    {
      hasOrigin = true;
      aNewOriginXY.SetValues (Draw::Atof (theArgVec[anArgIter + 1]),
                              Draw::Atof (theArgVec[anArgIter + 2]));
      anArgIter += 2;
    }
    else if (anArgIter + 2 < theArgNb
          && anArg == "-step")
    {
      hasStep = true;
      aNewStepXY.SetValues (Draw::Atof (theArgVec[anArgIter + 1]),
                            Draw::Atof (theArgVec[anArgIter + 2]));
      if (aNewStepXY.x() <= 0.0
       || aNewStepXY.y() <= 0.0)
      {
        Message::SendFail() << "Syntax error: wrong step '" << theArgVec[anArgIter + 1] << " " << theArgVec[anArgIter + 2] << "'";
        return 1;
      }
      anArgIter += 2;
    }
    else if (anArgIter + 1 < theArgNb
          && (anArg == "-angle"
           || anArg == "-rotangle"
           || anArg == "-rotationangle"))
    {
      hasRotAngle = true;
      aNewRotAngle = Draw::Atof (theArgVec[++anArgIter]);
    }
    else if (anArgIter + 1 < theArgNb
          && (anArg == "-zoffset"
           || anArg == "-dz"))
    {
      hasZOffset = true;
      aNewZOffset = Draw::Atof (theArgVec[++anArgIter]);
    }
    else if (anArgIter + 1 < theArgNb
          && anArg == "-radius")
    {
      hasSize = true;
      ++anArgIter;
      aNewSizeXY.SetValues (Draw::Atof (theArgVec[anArgIter]), 0.0);
      if (aNewStepXY.x() <= 0.0)
      {
        Message::SendFail() << "Syntax error: wrong size '" << theArgVec[anArgIter] << "'";
        return 1;
      }
    }
    else if (anArgIter + 2 < theArgNb
          && anArg == "-size")
    {
      hasSize = true;
      aNewSizeXY.SetValues (Draw::Atof (theArgVec[anArgIter + 1]),
                            Draw::Atof (theArgVec[anArgIter + 2]));
      if (aNewStepXY.x() <= 0.0
       || aNewStepXY.y() <= 0.0)
      {
        Message::SendFail() << "Syntax error: wrong size '" << theArgVec[anArgIter + 1] << " " << theArgVec[anArgIter + 2] << "'";
        return 1;
      }
      anArgIter += 2;
    }
    else if (anArg == "r"
          || anArg == "rect"
          || anArg == "rectangular")
    {
      aType = Aspect_GT_Rectangular;
    }
    else if (anArg == "c"
          || anArg == "circ"
          || anArg == "circular")
    {
      aType = Aspect_GT_Circular;
    }
    else if (anArg == "l"
          || anArg == "line"
          || anArg == "lines")
    {
      aMode = Aspect_GDM_Lines;
    }
    else if (anArg == "p"
          || anArg == "point"
          || anArg == "points")
    {
      aMode = Aspect_GDM_Points;
    }
    else if (anArgIter + 1 >= theArgNb
          && anArg == "off")
    {
      aViewer->DeactivateGrid();
      return 0;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  if (aType == Aspect_GT_Rectangular)
  {
    Graphic3d_Vec2d anOrigXY, aStepXY;
    Standard_Real aRotAngle = 0.0;
    aViewer->RectangularGridValues (anOrigXY.x(), anOrigXY.y(), aStepXY.x(), aStepXY.y(), aRotAngle);
    if (hasOrigin)
    {
      anOrigXY = aNewOriginXY;
    }
    if (hasStep)
    {
      aStepXY = aNewStepXY;
    }
    if (hasRotAngle)
    {
      aRotAngle = aNewRotAngle;
    }
    aViewer->SetRectangularGridValues (anOrigXY.x(), anOrigXY.y(), aStepXY.x(), aStepXY.y(), aRotAngle);
    if (hasSize || hasZOffset)
    {
      Graphic3d_Vec3d aSize;
      aViewer->RectangularGridGraphicValues (aSize.x(), aSize.y(), aSize.z());
      if (hasSize)
      {
        aSize.x() = aNewSizeXY.x();
        aSize.y() = aNewSizeXY.y();
      }
      if (hasZOffset)
      {
        aSize.z() = aNewZOffset;
      }
      aViewer->SetRectangularGridGraphicValues (aSize.x(), aSize.y(), aSize.z());
    }
  }
  else if (aType == Aspect_GT_Circular)
  {
    Graphic3d_Vec2d anOrigXY;
    Standard_Real aRadiusStep;
    Standard_Integer aDivisionNumber;
    Standard_Real aRotAngle = 0.0;
    aViewer->CircularGridValues (anOrigXY.x(), anOrigXY.y(), aRadiusStep, aDivisionNumber, aRotAngle);
    if (hasOrigin)
    {
      anOrigXY = aNewOriginXY;
    }
    if (hasStep)
    {
      aRadiusStep     = aNewStepXY[0];
      aDivisionNumber = (int )aNewStepXY[1];
      if (aDivisionNumber < 1)
      {
        Message::SendFail() << "Syntax error: invalid division number '" << aNewStepXY[1] << "'";
        return 1;
      }
    }
    if (hasRotAngle)
    {
      aRotAngle = aNewRotAngle;
    }

    aViewer->SetCircularGridValues (anOrigXY.x(), anOrigXY.y(), aRadiusStep, aDivisionNumber, aRotAngle);
    if (hasSize || hasZOffset)
    {
      Standard_Real aRadius = 0.0, aZOffset = 0.0;
      aViewer->CircularGridGraphicValues (aRadius, aZOffset);
      if (hasSize)
      {
        aRadius = aNewSizeXY.x();
        if (aNewSizeXY.y() != 0.0)
        {
          Message::SendFail ("Syntax error: circular size should be specified as radius");
          return 1;
        }
      }
      if (hasZOffset)
      {
        aZOffset = aNewZOffset;
      }
      aViewer->SetCircularGridGraphicValues (aRadius, aZOffset);
    }
  }
  aViewer->ActivateGrid (aType, aMode);
  return 0;
}

//==============================================================================
//function : VPriviledgedPlane
//purpose  :
//==============================================================================

static int VPriviledgedPlane (Draw_Interpretor& theDI,
                              Standard_Integer  theArgNb,
                              const char**      theArgVec)
{
  if (theArgNb != 1 && theArgNb != 7 && theArgNb != 10)
  {
    Message::SendFail ("Error: wrong number of arguments! See usage:");
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  // get the active viewer
  Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
  if (aViewer.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  if (theArgNb == 1)
  {
    gp_Ax3 aPriviledgedPlane = aViewer->PrivilegedPlane();
    const gp_Pnt& anOrig = aPriviledgedPlane.Location();
    const gp_Dir& aNorm = aPriviledgedPlane.Direction();
    const gp_Dir& aXDir = aPriviledgedPlane.XDirection();
    theDI << "Origin: " << anOrig.X() << " " << anOrig.Y() << " " << anOrig.Z() << " "
          << "Normal: " << aNorm.X() << " " << aNorm.Y() << " " << aNorm.Z() << " "
          << "X-dir: "  << aXDir.X() << " " << aXDir.Y() << " " << aXDir.Z() << "\n";
    return 0;
  }

  Standard_Integer anArgIdx = 1;
  Standard_Real anOrigX = Draw::Atof (theArgVec[anArgIdx++]);
  Standard_Real anOrigY = Draw::Atof (theArgVec[anArgIdx++]);
  Standard_Real anOrigZ = Draw::Atof (theArgVec[anArgIdx++]);
  Standard_Real aNormX  = Draw::Atof (theArgVec[anArgIdx++]);
  Standard_Real aNormY  = Draw::Atof (theArgVec[anArgIdx++]);
  Standard_Real aNormZ  = Draw::Atof (theArgVec[anArgIdx++]);

  gp_Ax3 aPriviledgedPlane;
  gp_Pnt anOrig (anOrigX, anOrigY, anOrigZ);
  gp_Dir aNorm (aNormX, aNormY, aNormZ);
  if (theArgNb > 7)
  {
    Standard_Real aXDirX = Draw::Atof (theArgVec[anArgIdx++]);
    Standard_Real aXDirY = Draw::Atof (theArgVec[anArgIdx++]);
    Standard_Real aXDirZ = Draw::Atof (theArgVec[anArgIdx++]);
    gp_Dir aXDir (aXDirX, aXDirY, aXDirZ);
    aPriviledgedPlane = gp_Ax3 (anOrig, aNorm, aXDir);
  }
  else
  {
    aPriviledgedPlane = gp_Ax3 (anOrig, aNorm);
  }

  aViewer->SetPrivilegedPlane (aPriviledgedPlane);

  return 0;
}

//==============================================================================
//function : VConvert
//purpose  :
//==============================================================================

static int VConvert (Draw_Interpretor& theDI,
                     Standard_Integer  theArgNb,
                     const char**      theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  enum { Model, Ray, View, Window, Grid } aMode = Model;

  // access coordinate arguments
  TColStd_SequenceOfReal aCoord;
  Standard_Integer anArgIdx = 1;
  for (; anArgIdx < 4 && anArgIdx < theArgNb; ++anArgIdx)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIdx]);
    if (!anArg.IsRealValue())
    {
      break;
    }
    aCoord.Append (anArg.RealValue());
  }

  // non-numeric argument too early
  if (aCoord.IsEmpty())
  {
    Message::SendFail ("Error: wrong number of arguments! See usage:");
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  // collect all other arguments and options
  for (; anArgIdx < theArgNb; ++anArgIdx)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIdx]);
    anArg.LowerCase();
    if      (anArg == "window") aMode = Window;
    else if (anArg == "view")   aMode = View;
    else if (anArg == "grid")   aMode = Grid;
    else if (anArg == "ray")    aMode = Ray;
    else
    {
      Message::SendFail() << "Error: wrong argument " << anArg << "! See usage:";
      theDI.PrintHelp (theArgVec[0]);
      return 1;
    }
  }

  // complete input checks
  if ((aCoord.Length() == 1 && theArgNb > 3) ||
      (aCoord.Length() == 2 && theArgNb > 4) ||
      (aCoord.Length() == 3 && theArgNb > 5))
  {
    Message::SendFail ("Error: wrong number of arguments! See usage:");
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  Standard_Real aXYZ[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  Standard_Integer aXYp[2] = {0, 0};

  // convert one-dimensional coordinate
  if (aCoord.Length() == 1)
  {
    switch (aMode)
    {
      case View   : theDI << "View Vv: "   << aView->Convert ((Standard_Integer)aCoord (1)); return 0;
      case Window : theDI << "Window Vp: " << aView->Convert (aCoord (1)); return 0;
      default:
        Message::SendFail ("Error: wrong arguments! See usage:");
        theDI.PrintHelp (theArgVec[0]);
        return 1;
    }
  }

  // convert 2D coordinates from projection or view reference space
  if (aCoord.Length() == 2)
  {
    switch (aMode)
    {
      case Model :
        aView->Convert ((Standard_Integer) aCoord (1), (Standard_Integer) aCoord (2), aXYZ[0], aXYZ[1], aXYZ[2]);
        theDI << "Model X,Y,Z: " << aXYZ[0] << " " << aXYZ[1] << " " << aXYZ[2] << "\n";
        return 0;

      case View :
        aView->Convert ((Standard_Integer) aCoord (1), (Standard_Integer) aCoord (2), aXYZ[0], aXYZ[1]);
        theDI << "View Xv,Yv: " << aXYZ[0] << " " << aXYZ[1] << "\n";
        return 0;

      case Window :
        aView->Convert (aCoord (1), aCoord (2), aXYp[0], aXYp[1]);
        theDI << "Window Xp,Yp: " << aXYp[0] << " " << aXYp[1] << "\n";
        return 0;

      case Grid :
        aView->Convert ((Standard_Integer) aCoord (1), (Standard_Integer) aCoord (2), aXYZ[0], aXYZ[1], aXYZ[2]);
        aView->ConvertToGrid (aXYZ[0], aXYZ[1], aXYZ[2], aXYZ[3], aXYZ[4], aXYZ[5]);
        theDI << "Model X,Y,Z: " << aXYZ[3] << " " << aXYZ[4] << " " << aXYZ[5] << "\n";
        return 0;

      case Ray :
        aView->ConvertWithProj ((Standard_Integer) aCoord (1),
                                (Standard_Integer) aCoord (2),
                                aXYZ[0], aXYZ[1], aXYZ[2],
                                aXYZ[3], aXYZ[4], aXYZ[5]);
        theDI << "Model DX,DY,DZ: " << aXYZ[3] << " " << aXYZ[4] << " " << aXYZ[5] << "\n";
        return 0;

      default:
        Message::SendFail ("Error: wrong arguments! See usage:");
        theDI.PrintHelp (theArgVec[0]);
        return 1;
    }
  }

  // convert 3D coordinates from view reference space
  else if (aCoord.Length() == 3)
  {
    switch (aMode)
    {
      case Window :
        aView->Convert (aCoord (1), aCoord (2), aCoord (3), aXYp[0], aXYp[1]);
        theDI << "Window Xp,Yp: " << aXYp[0] << " " << aXYp[1] << "\n";
        return 0;

      case Grid :
        aView->ConvertToGrid (aCoord (1), aCoord (2), aCoord (3), aXYZ[0], aXYZ[1], aXYZ[2]);
        theDI << "Model X,Y,Z: " << aXYZ[0] << " " << aXYZ[1] << " " << aXYZ[2] << "\n";
        return 0;

      default:
        Message::SendFail ("Error: wrong arguments! See usage:");
        theDI.PrintHelp (theArgVec[0]);
        return 1;
    }
  }

  return 0;
}

//==============================================================================
//function : VFps
//purpose  :
//==============================================================================

static int VFps (Draw_Interpretor& theDI,
                 Standard_Integer  theArgNb,
                 const char**      theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Standard_Integer aFramesNb = -1;
  Standard_Real aDuration = -1.0;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (aDuration < 0.0
     && anArgIter + 1 < theArgNb
     && (anArg == "-duration"
      || anArg == "-dur"
      || anArg == "-time"))
    {
      aDuration = Draw::Atof (theArgVec[++anArgIter]);
    }
    else if (aFramesNb < 0
          && anArg.IsIntegerValue())
    {
      aFramesNb = anArg.IntegerValue();
      if (aFramesNb <= 0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }
  if (aFramesNb < 0 && aDuration < 0.0)
  {
    aFramesNb = 100;
  }

  // the time is meaningless for first call
  // due to async OpenGl rendering
  aView->Redraw();

  // redraw view in loop to estimate average values
  OSD_Timer aTimer;
  aTimer.Start();
  Standard_Integer aFrameIter = 1;
  for (;; ++aFrameIter)
  {
    aView->Redraw();
    if ((aFramesNb > 0
      && aFrameIter >= aFramesNb)
     || (aDuration > 0.0
      && aTimer.ElapsedTime() >= aDuration))
    {
      break;
    }
  }
  aTimer.Stop();
  Standard_Real aCpu;
  const Standard_Real aTime = aTimer.ElapsedTime();
  aTimer.OSD_Chronometer::Show (aCpu);

  const Standard_Real aFpsAver = Standard_Real(aFrameIter) / aTime;
  const Standard_Real aCpuAver = aCpu / Standard_Real(aFrameIter);

  // return statistics
  theDI << "FPS: " << aFpsAver << "\n"
        << "CPU: " << (1000.0 * aCpuAver) << " msec\n";

  // compute additional statistics in ray-tracing mode
  const Graphic3d_RenderingParams& aParams = aView->RenderingParams();
  if (aParams.Method == Graphic3d_RM_RAYTRACING)
  {
    Graphic3d_Vec2i aWinSize (0, 0);
    aView->Window()->Size (aWinSize.x(), aWinSize.y());

    // 1 shadow ray and 1 secondary ray pew each bounce
    const Standard_Real aMRays = aWinSize.x() * aWinSize.y() * aFpsAver * aParams.RaytracingDepth * 2 / 1.0e6f;
    theDI << "MRays/sec (upper bound): " << aMRays << "\n";
  }

  return 0;
}

//! Auxiliary function for parsing glsl dump level argument.
static Standard_Boolean parseGlslSourceFlag (Standard_CString               theArg,
                                             OpenGl_ShaderProgramDumpLevel& theGlslDumpLevel)
{
  TCollection_AsciiString aTypeStr (theArg);
  aTypeStr.LowerCase();
  if (aTypeStr == "off"
   || aTypeStr == "0")
  {
    theGlslDumpLevel = OpenGl_ShaderProgramDumpLevel_Off;
  }
  else if (aTypeStr == "short")
  {
    theGlslDumpLevel = OpenGl_ShaderProgramDumpLevel_Short;
  }
  else if (aTypeStr == "full"
        || aTypeStr == "1")
  {
    theGlslDumpLevel = OpenGl_ShaderProgramDumpLevel_Full;
  }
  else
  {
    return Standard_False;
  }
  return Standard_True;
}

//==============================================================================
//function : VGlDebug
//purpose  :
//==============================================================================

static int VGlDebug (Draw_Interpretor& theDI,
                     Standard_Integer  theArgNb,
                     const char**      theArgVec)
{
  Handle(OpenGl_GraphicDriver) aDriver;
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (!aView.IsNull())
  {
    aDriver = Handle(OpenGl_GraphicDriver)::DownCast (aView->Viewer()->Driver());
  }
  OpenGl_Caps* aDefCaps = &ViewerTest_myDefaultCaps;
  OpenGl_Caps* aCaps    = !aDriver.IsNull() ? &aDriver->ChangeOptions() : NULL;

  if (theArgNb < 2)
  {
    TCollection_AsciiString aDebActive, aSyncActive;
    if (aCaps == NULL)
    {
      aCaps = aDefCaps;
    }
    else
    {
      Standard_Boolean isActive = OpenGl_Context::CheckExtension ((const char* )::glGetString (GL_EXTENSIONS),
                                                                  "GL_ARB_debug_output");
      aDebActive = isActive ? " (active)" : " (inactive)";
      if (isActive)
      {
        // GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB
        aSyncActive = ::glIsEnabled (0x8242) == GL_TRUE ? " (active)" : " (inactive)";
      }
    }

    TCollection_AsciiString aGlslCodeDebugStatus = TCollection_AsciiString()
      + "glslSourceCode: "
      + (aCaps->glslDumpLevel == OpenGl_ShaderProgramDumpLevel_Off
         ? "Off"
         : aCaps->glslDumpLevel == OpenGl_ShaderProgramDumpLevel_Short
          ? "Short"
          : "Full")
      + "\n";
    theDI << "debug:          " << (aCaps->contextDebug      ? "1" : "0") << aDebActive  << "\n"
          << "sync:           " << (aCaps->contextSyncDebug  ? "1" : "0") << aSyncActive << "\n"
          << "glslWarn:       " << (aCaps->glslWarnings      ? "1" : "0") << "\n"
          << aGlslCodeDebugStatus
          << "extraMsg:       " << (aCaps->suppressExtraMsg  ? "0" : "1") << "\n";
    return 0;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg     = theArgVec[anArgIter];
    TCollection_AsciiString anArgCase (anArg);
    anArgCase.LowerCase();
    Standard_Boolean toEnableDebug = Standard_True;
    if (anArgCase == "-glsl"
     || anArgCase == "-glslwarn"
     || anArgCase == "-glslwarns"
     || anArgCase == "-glslwarnings")
    {
      Standard_Boolean toShowWarns = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toShowWarns))
      {
        --anArgIter;
      }
      aDefCaps->glslWarnings = toShowWarns;
      if (aCaps != NULL)
      {
        aCaps->glslWarnings = toShowWarns;
      }
    }
    else if (anArgCase == "-extra"
          || anArgCase == "-extramsg"
          || anArgCase == "-extramessages")
    {
      Standard_Boolean toShow = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toShow))
      {
        --anArgIter;
      }
      aDefCaps->suppressExtraMsg = !toShow;
      if (aCaps != NULL)
      {
        aCaps->suppressExtraMsg = !toShow;
      }
    }
    else if (anArgCase == "-noextra"
          || anArgCase == "-noextramsg"
          || anArgCase == "-noextramessages")
    {
      Standard_Boolean toSuppress = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toSuppress))
      {
        --anArgIter;
      }
      aDefCaps->suppressExtraMsg = toSuppress;
      if (aCaps != NULL)
      {
        aCaps->suppressExtraMsg = toSuppress;
      }
    }
    else if (anArgCase == "-sync")
    {
      Standard_Boolean toSync = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toSync))
      {
        --anArgIter;
      }
      aDefCaps->contextSyncDebug = toSync;
      if (toSync)
      {
        aDefCaps->contextDebug = Standard_True;
      }
    }
    else if (anArgCase == "-glslsourcecode"
          || anArgCase == "-glslcode")
    {
      OpenGl_ShaderProgramDumpLevel aGslsDumpLevel = OpenGl_ShaderProgramDumpLevel_Full;
      if (++anArgIter < theArgNb
      && !parseGlslSourceFlag (theArgVec[anArgIter], aGslsDumpLevel))
      {
        --anArgIter;
      }
      aDefCaps->glslDumpLevel = aGslsDumpLevel;
      if (aCaps != NULL)
      {
        aCaps->glslDumpLevel = aGslsDumpLevel;
      }
    }
    else if (anArgCase == "-debug")
    {
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnableDebug))
      {
        --anArgIter;
      }
      aDefCaps->contextDebug = toEnableDebug;
    }
    else if (Draw::ParseOnOff (anArg, toEnableDebug)
          && (anArgIter + 1 == theArgNb))
    {
      // simple alias to turn on almost everything
      aDefCaps->contextDebug     = toEnableDebug;
      aDefCaps->contextSyncDebug = toEnableDebug;
      aDefCaps->glslWarnings     = toEnableDebug;
      if (!toEnableDebug)
      {
        aDefCaps->glslDumpLevel = OpenGl_ShaderProgramDumpLevel_Off;
      }
      aDefCaps->suppressExtraMsg = !toEnableDebug;
      if (aCaps != NULL)
      {
        aCaps->contextDebug     = toEnableDebug;
        aCaps->contextSyncDebug = toEnableDebug;
        aCaps->glslWarnings     = toEnableDebug;
        if (!toEnableDebug)
        {
          aCaps->glslDumpLevel = OpenGl_ShaderProgramDumpLevel_Off;
        }
        aCaps->suppressExtraMsg = !toEnableDebug;
      }
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  return 0;
}

//==============================================================================
//function : VVbo
//purpose  :
//==============================================================================

static int VVbo (Draw_Interpretor& theDI,
                 Standard_Integer  theArgNb,
                 const char**      theArgVec)
{
  const Standard_Boolean toSet    = (theArgNb > 1);
  const Standard_Boolean toUseVbo = toSet ? (Draw::Atoi (theArgVec[1]) == 0) : 1;
  if (toSet)
  {
    ViewerTest_myDefaultCaps.vboDisable = toUseVbo;
  }

  // get the context
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    if (!toSet)
    {
      Message::SendFail ("Error: no active viewer");
    }
    return 1;
  }
  Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast (aContextAIS->CurrentViewer()->Driver());
  if (!aDriver.IsNull())
  {
    if (!toSet)
    {
      theDI << (aDriver->Options().vboDisable ? "0" : "1") << "\n";
    }
    else
    {
      aDriver->ChangeOptions().vboDisable = toUseVbo;
    }
  }

  return 0;
}

//==============================================================================
//function : VCaps
//purpose  :
//==============================================================================

static int VCaps (Draw_Interpretor& theDI,
                  Standard_Integer  theArgNb,
                  const char**      theArgVec)
{
  OpenGl_Caps* aCaps = &ViewerTest_myDefaultCaps;
  Handle(OpenGl_GraphicDriver)   aDriver;
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (!aContext.IsNull())
  {
    aDriver = Handle(OpenGl_GraphicDriver)::DownCast (aContext->CurrentViewer()->Driver());
    aCaps   = &aDriver->ChangeOptions();
  }

  if (theArgNb < 2)
  {
    theDI << "sRGB:    " << (aCaps->sRGBDisable       ? "0" : "1") << "\n";
    theDI << "VBO:     " << (aCaps->vboDisable        ? "0" : "1") << "\n";
    theDI << "Sprites: " << (aCaps->pntSpritesDisable ? "0" : "1") << "\n";
    theDI << "SoftMode:" << (aCaps->contextNoAccel    ? "1" : "0") << "\n";
    theDI << "FFP:     " << (aCaps->ffpEnable         ? "1" : "0") << "\n";
    theDI << "PolygonMode: " << (aCaps->usePolygonMode ? "1" : "0") << "\n";
    theDI << "VSync:   " <<  aCaps->swapInterval                   << "\n";
    theDI << "Compatible:" << (aCaps->contextCompatible ? "1" : "0") << "\n";
    theDI << "Stereo:  " << (aCaps->contextStereo ? "1" : "0") << "\n";
    theDI << "WinBuffer: " << (aCaps->useSystemBuffer ? "1" : "0") << "\n";
    theDI << "NoExt:"    << (aCaps->contextNoExtensions ? "1" : "0") << "\n";
    theDI << "MaxVersion:" << aCaps->contextMajorVersionUpper << "." << aCaps->contextMinorVersionUpper << "\n";
    theDI << "CompressTextures: " << (aCaps->compressedTexturesDisable ? "0" : "1") << "\n";
    return 0;
  }

  ViewerTest_AutoUpdater anUpdateTool (aContext, ViewerTest::CurrentView());
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg     = theArgVec[anArgIter];
    TCollection_AsciiString anArgCase (anArg);
    anArgCase.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      continue;
    }
    else if (anArgCase == "-vsync"
          || anArgCase == "-swapinterval")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->swapInterval = toEnable;
    }
    else if (anArgCase == "-ffp")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->ffpEnable = toEnable;
    }
    else if (anArgCase == "-polygonmode")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->usePolygonMode = toEnable;
    }
    else if (anArgCase == "-srgb")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->sRGBDisable = !toEnable;
    }
    else if (anArgCase == "-compressedtextures")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->compressedTexturesDisable = !toEnable;
    }
    else if (anArgCase == "-vbo")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->vboDisable = !toEnable;
    }
    else if (anArgCase == "-sprite"
          || anArgCase == "-sprites")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->pntSpritesDisable = !toEnable;
    }
    else if (anArgCase == "-softmode")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->contextNoAccel = toEnable;
    }
    else if (anArgCase == "-winbuffer"
          || anArgCase == "-windowbuffer"
          || anArgCase == "-usewinbuffer"
          || anArgCase == "-usewindowbuffer"
          || anArgCase == "-usesystembuffer")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->useSystemBuffer = toEnable;
    }
    else if (anArgCase == "-accel"
          || anArgCase == "-acceleration")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->contextNoAccel = !toEnable;
    }
    else if (anArgCase == "-compat"
          || anArgCase == "-compatprofile"
          || anArgCase == "-compatible"
          || anArgCase == "-compatibleprofile")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->contextCompatible = toEnable;
      if (!aCaps->contextCompatible)
      {
        aCaps->ffpEnable = Standard_False;
      }
    }
    else if (anArgCase == "-core"
          || anArgCase == "-coreprofile")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->contextCompatible = !toEnable;
      if (!aCaps->contextCompatible)
      {
        aCaps->ffpEnable = Standard_False;
      }
    }
    else if (anArgCase == "-stereo"
          || anArgCase == "-quadbuffer")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->contextStereo = toEnable;
    }
    else if (anArgCase == "-noext"
          || anArgCase == "-noextensions"
          || anArgCase == "-noextension")
    {
      Standard_Boolean toDisable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toDisable))
      {
        --anArgIter;
      }
      aCaps->contextNoExtensions = toDisable;
    }
    else if (anArgCase == "-maxversion"
          || anArgCase == "-upperversion"
          || anArgCase == "-limitversion")
    {
      Standard_Integer aVer[2] = { -2, -1 };
      for (Standard_Integer aValIter = 0; aValIter < 2; ++aValIter)
      {
        if (anArgIter + 1 < theArgNb)
        {
          const TCollection_AsciiString aStr (theArgVec[anArgIter + 1]);
          if (aStr.IsIntegerValue())
          {
            aVer[aValIter] = aStr.IntegerValue();
            ++anArgIter;
          }
        }
      }
      if (aVer[0] < -1
       || aVer[1] < -1)
      {
        Message::SendFail() << "Syntax error at '" << anArgCase << "'";
        return 1;
      }
      aCaps->contextMajorVersionUpper = aVer[0];
      aCaps->contextMinorVersionUpper = aVer[1];
    }
    else
    {
      Message::SendFail() << "Error: unknown argument '" << anArg << "'";
      return 1;
    }
  }
  if (aCaps != &ViewerTest_myDefaultCaps)
  {
    ViewerTest_myDefaultCaps = *aCaps;
  }
  return 0;
}

//==============================================================================
//function : VMemGpu
//purpose  :
//==============================================================================

static int VMemGpu (Draw_Interpretor& theDI,
                    Standard_Integer  theArgNb,
                    const char**      theArgVec)
{
  // get the context
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Handle(Graphic3d_GraphicDriver) aDriver = aContextAIS->CurrentViewer()->Driver();
  if (aDriver.IsNull())
  {
    Message::SendFail ("Error: graphic driver not available");
    return 1;
  }

  Standard_Size aFreeBytes = 0;
  TCollection_AsciiString anInfo;
  if (!aDriver->MemoryInfo (aFreeBytes, anInfo))
  {
    Message::SendFail ("Error: information not available");
    return 1;
  }

  if (theArgNb > 1 && *theArgVec[1] == 'f')
  {
    theDI << Standard_Real (aFreeBytes);
  }
  else
  {
    theDI << anInfo;
  }

  return 0;
}

// ==============================================================================
// function : VReadPixel
// purpose  :
// ==============================================================================
static int VReadPixel (Draw_Interpretor& theDI,
                       Standard_Integer  theArgNb,
                       const char**      theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }
  else if (theArgNb < 3)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments.\n"
                        << "Usage: " << theArgVec[0] << " xPixel yPixel [{rgb|rgba|depth|hls|rgbf|rgbaf}=rgba] [name]";
    return 1;
  }

  Image_Format         aFormat     = Image_Format_RGBA;
  Graphic3d_BufferType aBufferType = Graphic3d_BT_RGBA;

  Standard_Integer aWidth, aHeight;
  aView->Window()->Size (aWidth, aHeight);
  const Standard_Integer anX = Draw::Atoi (theArgVec[1]);
  const Standard_Integer anY = Draw::Atoi (theArgVec[2]);
  if (anX < 0 || anX >= aWidth || anY < 0 || anY > aHeight)
  {
    Message::SendFail() << "Error: pixel coordinates (" << anX << "; " << anY << ") are out of view (" << aWidth << " x " << aHeight << ")";
    return 1;
  }

  bool toShowName = false, toShowHls = false, toShowHex = false, toShow_sRGB = false;
  for (Standard_Integer anIter = 3; anIter < theArgNb; ++anIter)
  {
    TCollection_AsciiString aParam (theArgVec[anIter]);
    aParam.LowerCase();
    if (aParam == "-rgb"
     || aParam == "rgb"
     || aParam == "-srgb"
     || aParam == "srgb")
    {
      aFormat     = Image_Format_RGB;
      aBufferType = Graphic3d_BT_RGB;
      toShow_sRGB = aParam == "-srgb" || aParam == "srgb";
    }
    else if (aParam == "-hls"
          || aParam == "hls")
    {
      aFormat     = Image_Format_RGB;
      aBufferType = Graphic3d_BT_RGB;
      toShowHls   = Standard_True;
    }
    else if (aParam == "-rgbf"
          || aParam == "rgbf")
    {
      aFormat     = Image_Format_RGBF;
      aBufferType = Graphic3d_BT_RGB;
    }
    else if (aParam == "-rgba"
          || aParam == "rgba"
          || aParam == "-srgba"
          || aParam == "srgba")
    {
      aFormat     = Image_Format_RGBA;
      aBufferType = Graphic3d_BT_RGBA;
      toShow_sRGB = aParam == "-srgba" || aParam == "srgba";
    }
    else if (aParam == "-rgbaf"
          || aParam == "rgbaf")
    {
      aFormat     = Image_Format_RGBAF;
      aBufferType = Graphic3d_BT_RGBA;
    }
    else if (aParam == "-depth"
          || aParam == "depth")
    {
      aFormat     = Image_Format_GrayF;
      aBufferType = Graphic3d_BT_Depth;
    }
    else if (aParam == "-name"
          || aParam == "name")
    {
      toShowName = Standard_True;
    }
    else if (aParam == "-hex"
          || aParam == "hex")
    {
      toShowHex = Standard_True;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << aParam << "'";
      return 1;
    }
  }

  Image_PixMap anImage;
  if (!anImage.InitTrash (aFormat, aWidth, aHeight))
  {
    Message::SendFail ("Error: image allocation failed");
    return 1;
  }
  else if (!aView->ToPixMap (anImage, aWidth, aHeight, aBufferType))
  {
    Message::SendFail ("Error: image dump failed");
    return 1;
  }

  // redirect possible warning messages that could have been added by ToPixMap
  // into the Tcl interpretor (via DefaultMessenger) to cout, so that they do not
  // contaminate result of the command
  Standard_CString aWarnLog = theDI.Result();
  if (aWarnLog != NULL && aWarnLog[0] != '\0')
  {
    std::cout << aWarnLog << std::endl;
  }
  theDI.Reset();

  Quantity_ColorRGBA aColor = anImage.PixelColor (anX, anY, true);
  if (toShowName)
  {
    if (aBufferType == Graphic3d_BT_RGBA)
    {
      theDI << Quantity_Color::StringName (aColor.GetRGB().Name()) << " " << aColor.Alpha();
    }
    else
    {
      theDI << Quantity_Color::StringName (aColor.GetRGB().Name());
    }
  }
  else if (toShowHex)
  {
    if (aBufferType == Graphic3d_BT_RGBA)
    {
      theDI << Quantity_ColorRGBA::ColorToHex (aColor);
    }
    else
    {
      theDI << Quantity_Color::ColorToHex (aColor.GetRGB());
    }
  }
  else
  {
    switch (aBufferType)
    {
      default:
      case Graphic3d_BT_RGB:
      {
        if (toShowHls)
        {
          theDI << aColor.GetRGB().Hue() << " " << aColor.GetRGB().Light() << " " << aColor.GetRGB().Saturation();
        }
        else if (toShow_sRGB)
        {
          const Graphic3d_Vec4 aColor_sRGB = Quantity_ColorRGBA::Convert_LinearRGB_To_sRGB ((Graphic3d_Vec4 )aColor);
          theDI << aColor_sRGB.r() << " " << aColor_sRGB.g() << " " << aColor_sRGB.b();
        }
        else
        {
          theDI << aColor.GetRGB().Red() << " " << aColor.GetRGB().Green() << " " << aColor.GetRGB().Blue();
        }
        break;
      }
      case Graphic3d_BT_RGBA:
      {
        const Graphic3d_Vec4 aVec4 = toShow_sRGB ? Quantity_ColorRGBA::Convert_LinearRGB_To_sRGB ((Graphic3d_Vec4 )aColor) : (Graphic3d_Vec4 )aColor;
        theDI << aVec4.r() << " " << aVec4.g() << " " << aVec4.b() << " " << aVec4.a();
        break;
      }
      case Graphic3d_BT_Depth:
      {
        theDI << aColor.GetRGB().Red();
        break;
      }
    }
  }

  return 0;
}

//! Auxiliary presentation for an image plane.
class ViewerTest_ImagePrs : public AIS_InteractiveObject
{
public:
  //! Main constructor.
  ViewerTest_ImagePrs (const Handle(Image_PixMap)& theImage,
                       const Standard_Real theWidth,
                       const Standard_Real theHeight,
                       const TCollection_AsciiString& theLabel)
  : myLabel (theLabel), myWidth (theWidth), myHeight(theHeight)
  {
    SetDisplayMode (0);
    SetHilightMode (1);
    myDynHilightDrawer->SetZLayer (Graphic3d_ZLayerId_Topmost);
    {
      myDrawer->SetShadingAspect (new Prs3d_ShadingAspect());
      const Handle(Graphic3d_AspectFillArea3d)& aFillAspect = myDrawer->ShadingAspect()->Aspect();
      Graphic3d_MaterialAspect aMat;
      aMat.SetMaterialType (Graphic3d_MATERIAL_PHYSIC);
      aMat.SetAmbientColor  (Quantity_NOC_BLACK);
      aMat.SetDiffuseColor  (Quantity_NOC_WHITE);
      aMat.SetSpecularColor (Quantity_NOC_BLACK);
      aMat.SetEmissiveColor (Quantity_NOC_BLACK);
      aFillAspect->SetFrontMaterial (aMat);
      aFillAspect->SetTextureMap (new Graphic3d_Texture2Dmanual (theImage));
      aFillAspect->SetTextureMapOn();
    }
    {
      Handle(Prs3d_TextAspect) aTextAspect = new Prs3d_TextAspect();
      aTextAspect->SetHorizontalJustification (Graphic3d_HTA_CENTER);
      aTextAspect->SetVerticalJustification   (Graphic3d_VTA_CENTER);
      myDrawer->SetTextAspect (aTextAspect);
    }
    {
      const gp_Dir aNorm (0.0, 0.0, 1.0);
      myTris = new Graphic3d_ArrayOfTriangles (4, 6, true, false, true);
      myTris->AddVertex (gp_Pnt(-myWidth * 0.5, -myHeight * 0.5, 0.0), aNorm, gp_Pnt2d (0.0, 0.0));
      myTris->AddVertex (gp_Pnt( myWidth * 0.5, -myHeight * 0.5, 0.0), aNorm, gp_Pnt2d (1.0, 0.0));
      myTris->AddVertex (gp_Pnt(-myWidth * 0.5,  myHeight * 0.5, 0.0), aNorm, gp_Pnt2d (0.0, 1.0));
      myTris->AddVertex (gp_Pnt( myWidth * 0.5,  myHeight * 0.5, 0.0), aNorm, gp_Pnt2d (1.0, 1.0));
      myTris->AddEdge (1);
      myTris->AddEdge (2);
      myTris->AddEdge (3);
      myTris->AddEdge (3);
      myTris->AddEdge (2);
      myTris->AddEdge (4);

      myRect = new Graphic3d_ArrayOfPolylines (4);
      myRect->AddVertex (myTris->Vertice (1));
      myRect->AddVertex (myTris->Vertice (3));
      myRect->AddVertex (myTris->Vertice (4));
      myRect->AddVertex (myTris->Vertice (2));
    }
  }

  //! Returns TRUE for accepted display modes.
  virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const Standard_OVERRIDE { return theMode == 0 || theMode == 1; }

  //! Compute presentation.
  virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& , const Handle(Prs3d_Presentation)& thePrs, const Standard_Integer theMode) Standard_OVERRIDE
  {
    switch (theMode)
    {
      case 0:
      {
        Handle(Graphic3d_Group) aGroup = thePrs->NewGroup();
        aGroup->AddPrimitiveArray (myTris);
        aGroup->SetGroupPrimitivesAspect (myDrawer->ShadingAspect()->Aspect());
        aGroup->AddPrimitiveArray (myRect);
        aGroup->SetGroupPrimitivesAspect (myDrawer->LineAspect()->Aspect());
        return;
      }
      case 1:
      {
        Prs3d_Text::Draw (thePrs->NewGroup(), myDrawer->TextAspect(), myLabel, gp_Pnt(0.0, 0.0, 0.0));
        Handle(Graphic3d_Group) aGroup = thePrs->NewGroup();
        aGroup->AddPrimitiveArray (myRect);
        aGroup->SetGroupPrimitivesAspect (myDrawer->LineAspect()->Aspect());
        return;
      }
    }
  }

  //! Compute selection.
  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel, const Standard_Integer theMode) Standard_OVERRIDE
  {
    if (theMode == 0)
    {
      Handle(SelectMgr_EntityOwner) anEntityOwner = new SelectMgr_EntityOwner (this, 5);
      Handle(Select3D_SensitivePrimitiveArray) aSensitive = new Select3D_SensitivePrimitiveArray (anEntityOwner);
      aSensitive->InitTriangulation (myTris->Attributes(), myTris->Indices(), TopLoc_Location());
      theSel->Add (aSensitive);
    }
  }

private:
  Handle(Graphic3d_ArrayOfTriangles) myTris;
  Handle(Graphic3d_ArrayOfPolylines) myRect;
  TCollection_AsciiString myLabel;
  Standard_Real myWidth;
  Standard_Real myHeight;
};

//==============================================================================
//function : VDiffImage
//purpose  : The draw-command compares two images.
//==============================================================================

static int VDiffImage (Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb < 3)
  {
    Message::SendFail ("Syntax error: not enough arguments");
    return 1;
  }

  Standard_Integer anArgIter = 1;
  TCollection_AsciiString anImgPathRef (theArgVec[anArgIter++]);
  TCollection_AsciiString anImgPathNew (theArgVec[anArgIter++]);
  TCollection_AsciiString aDiffImagePath;
  Standard_Real    aTolColor        = -1.0;
  Standard_Integer toBlackWhite     = -1;
  Standard_Integer isBorderFilterOn = -1;
  Standard_Boolean isOldSyntax = Standard_False;
  TCollection_AsciiString aViewName, aPrsNameRef, aPrsNameNew, aPrsNameDiff;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArgIter + 1 < theArgNb
     && (anArg == "-toleranceofcolor"
      || anArg == "-tolerancecolor"
      || anArg == "-tolerance"
      || anArg == "-toler"))
    {
      aTolColor = Atof (theArgVec[++anArgIter]);
      if (aTolColor < 0.0 || aTolColor > 1.0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << " " << theArgVec[anArgIter] << "'";
        return 1;
      }
    }
    else if (anArg == "-blackwhite")
    {
      Standard_Boolean toEnable = Standard_True;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      toBlackWhite = toEnable ? 1 : 0;
    }
    else if (anArg == "-borderfilter")
    {
      Standard_Boolean toEnable = Standard_True;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      isBorderFilterOn = toEnable ? 1 : 0;
    }
    else if (anArg == "-exitonclose")
    {
      ViewerTest_EventManager::ToExitOnCloseView() = true;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], ViewerTest_EventManager::ToExitOnCloseView()))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-closeonescape"
          || anArg == "-closeonesc")
    {
      ViewerTest_EventManager::ToCloseViewOnEscape() = true;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], ViewerTest_EventManager::ToCloseViewOnEscape()))
      {
        ++anArgIter;
      }
    }
    else if (anArgIter + 3 < theArgNb
          && anArg == "-display")
    {
      aViewName   = theArgVec[++anArgIter];
      aPrsNameRef = theArgVec[++anArgIter];
      aPrsNameNew = theArgVec[++anArgIter];
      if (anArgIter + 1 < theArgNb
      && *theArgVec[anArgIter + 1] != '-')
      {
        aPrsNameDiff = theArgVec[++anArgIter];
      }
    }
    else if (aTolColor < 0.0
          && anArg.IsRealValue())
    {
      isOldSyntax = Standard_True;
      aTolColor = anArg.RealValue();
      if (aTolColor < 0.0 || aTolColor > 1.0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << " " << theArgVec[anArgIter] << "'";
        return 1;
      }
    }
    else if (isOldSyntax
          && toBlackWhite == -1
          && (anArg == "0" || anArg == "1"))
    {
      toBlackWhite = anArg == "1" ? 1 : 0;
    }
    else if (isOldSyntax
          && isBorderFilterOn == -1
          && (anArg == "0" || anArg == "1"))
    {
      isBorderFilterOn = anArg == "1" ? 1 : 0;
    }
    else if (aDiffImagePath.IsEmpty())
    {
      aDiffImagePath = theArgVec[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  Handle(Image_AlienPixMap) anImgRef = new Image_AlienPixMap();
  Handle(Image_AlienPixMap) anImgNew = new Image_AlienPixMap();
  if (!anImgRef->Load (anImgPathRef))
  {
    Message::SendFail() << "Error: image file '" << anImgPathRef << "' cannot be read";
    return 1;
  }
  if (!anImgNew->Load (anImgPathNew))
  {
    Message::SendFail() << "Error: image file '" << anImgPathNew << "' cannot be read";
    return 1;
  }

  // compare the images
  Image_Diff aComparer;
  Standard_Integer aDiffColorsNb = -1;
  if (aComparer.Init (anImgRef, anImgNew, toBlackWhite == 1))
  {
    aComparer.SetColorTolerance (aTolColor >= 0.0 ? aTolColor : 0.0);
    aComparer.SetBorderFilterOn (isBorderFilterOn == 1);
    aDiffColorsNb = aComparer.Compare();
    theDI << aDiffColorsNb << "\n";
  }

  // save image of difference
  Handle(Image_AlienPixMap) aDiff;
  if (aDiffColorsNb > 0
  && (!aDiffImagePath.IsEmpty() || !aPrsNameDiff.IsEmpty()))
  {
    aDiff = new Image_AlienPixMap();
    if (!aDiff->InitTrash (Image_Format_Gray, anImgRef->SizeX(), anImgRef->SizeY()))
    {
      Message::SendFail() << "Error: cannot allocate memory for diff image " << anImgRef->SizeX() << "x" << anImgRef->SizeY();
      return 1;
    }
    aComparer.SaveDiffImage (*aDiff);
    if (!aDiffImagePath.IsEmpty()
     && !aDiff->Save (aDiffImagePath))
    {
      Message::SendFail() << "Error: diff image file '" << aDiffImagePath << "' cannot be written";
      return 1;
    }
  }

  if (aViewName.IsEmpty())
  {
    return 0;
  }

  ViewerTest_Names aViewNames (aViewName);
  if (ViewerTest_myViews.IsBound1 (aViewNames.GetViewName()))
  {
    TCollection_AsciiString aCommand = TCollection_AsciiString ("vclose ") + aViewNames.GetViewName();
    theDI.Eval (aCommand.ToCString());
  }

  Standard_Integer aPxLeft = 0;
  Standard_Integer aPxTop  = 0;
  Standard_Integer aWinSizeX = int(anImgRef->SizeX() * 2);
  Standard_Integer aWinSizeY = !aDiff.IsNull() && !aPrsNameDiff.IsEmpty()
                              ? int(anImgRef->SizeY() * 2)
                              : int(anImgRef->SizeY());
  TCollection_AsciiString aDisplayName;
  TCollection_AsciiString aViewId = ViewerTest::ViewerInit (aPxLeft, aPxTop, aWinSizeX, aWinSizeY,
                                                            aViewName, aDisplayName);

  Standard_Real aRatio = anImgRef->Ratio();
  Standard_Real aSizeX = 1.0;
  Standard_Real aSizeY = aSizeX / aRatio;
  {
    OSD_Path aPath (anImgPathRef);
    TCollection_AsciiString aLabelRef;
    if (!aPath.Name().IsEmpty())
    {
      aLabelRef = aPath.Name() + aPath.Extension();
    }
    aLabelRef += TCollection_AsciiString() + "\n" + int(anImgRef->SizeX()) + "x" + int(anImgRef->SizeY());

    Handle(ViewerTest_ImagePrs) anImgRefPrs = new ViewerTest_ImagePrs (anImgRef, aSizeX, aSizeY, aLabelRef);
    gp_Trsf aTrsfRef;
    aTrsfRef.SetTranslationPart (gp_Vec (-aSizeX * 0.5, 0.0, 0.0));
    anImgRefPrs->SetLocalTransformation (aTrsfRef);
    ViewerTest::Display (aPrsNameRef, anImgRefPrs, false, true);
  }
  {
    OSD_Path aPath (anImgPathNew);
    TCollection_AsciiString aLabelNew;
    if (!aPath.Name().IsEmpty())
    {
      aLabelNew = aPath.Name() + aPath.Extension();
    }
    aLabelNew += TCollection_AsciiString() + "\n" + int(anImgNew->SizeX()) + "x" + int(anImgNew->SizeY());

    Handle(ViewerTest_ImagePrs) anImgNewPrs = new ViewerTest_ImagePrs (anImgNew, aSizeX, aSizeY, aLabelNew);
    gp_Trsf aTrsfRef;
    aTrsfRef.SetTranslationPart (gp_Vec (aSizeX * 0.5, 0.0, 0.0));
    anImgNewPrs->SetLocalTransformation (aTrsfRef);
    ViewerTest::Display (aPrsNameNew, anImgNewPrs, false, true);
  }
  Handle(ViewerTest_ImagePrs) anImgDiffPrs;
  if (!aDiff.IsNull())
  {
    anImgDiffPrs = new ViewerTest_ImagePrs (aDiff, aSizeX, aSizeY, TCollection_AsciiString() + "Difference: " + aDiffColorsNb + " pixels");
    gp_Trsf aTrsfDiff;
    aTrsfDiff.SetTranslationPart (gp_Vec (0.0, -aSizeY, 0.0));
    anImgDiffPrs->SetLocalTransformation (aTrsfDiff);
  }
  if (!aPrsNameDiff.IsEmpty())
  {
    ViewerTest::Display (aPrsNameDiff, anImgDiffPrs, false, true);
  }
  ViewerTest::CurrentView()->SetProj (V3d_Zpos);
  ViewerTest::CurrentView()->FitAll();
  return 0;
}

//=======================================================================
//function : VSelect
//purpose  : Emulates different types of selection by mouse:
//           1) single click selection
//           2) selection with rectangle having corners at pixel positions (x1,y1) and (x2,y2)
//           3) selection with polygon having corners at
//           pixel positions (x1,y1),...,(xn,yn)
//           4) any of these selections with shift button pressed
//=======================================================================
static Standard_Integer VSelect (Draw_Interpretor& ,
                                 Standard_Integer theNbArgs,
                                 const char** theArgVec)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  NCollection_Sequence<Graphic3d_Vec2i> aPnts;
  bool isShiftSelection = false, toAllowOverlap = false;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-allowoverlap")
    {
      toAllowOverlap = true;
      if (anArgIter + 1 < theNbArgs
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toAllowOverlap))
      {
        ++anArgIter;
      }
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg.IsIntegerValue()
          && TCollection_AsciiString (theArgVec[anArgIter + 1]).IsIntegerValue())
    {
      const TCollection_AsciiString anArgNext (theArgVec[++anArgIter]);
      aPnts.Append (Graphic3d_Vec2i (anArg.IntegerValue(), anArgNext.IntegerValue()));
    }
    else if (anArgIter + 1 == theNbArgs
          && anArg.IsIntegerValue())
    {
      isShiftSelection = anArg.IntegerValue() == 1;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  if (toAllowOverlap)
  {
    aCtx->MainSelector()->AllowOverlapDetection (toAllowOverlap);
  }

  Handle(ViewerTest_EventManager) aCurrentEventManager = ViewerTest::CurrentEventManager();
  if (aPnts.IsEmpty())
  {
    if (isShiftSelection)
    {
      aCtx->ShiftSelect (false);
    }
    else
    {
      aCtx->Select (false);
    }
    aCtx->CurrentViewer()->Invalidate();
  }
  else if (aPnts.Length() == 2)
  {
    if (toAllowOverlap
     && aPnts.First().y() < aPnts.Last().y())
    {
      std::swap (aPnts.ChangeFirst(), aPnts.ChangeLast());
    }
    else if (!toAllowOverlap
           && aPnts.First().y() > aPnts.Last().y())
    {
      std::swap (aPnts.ChangeFirst(), aPnts.ChangeLast());
    }
    aCurrentEventManager->SelectInViewer (aPnts, isShiftSelection);
  }
  else
  {
    aCurrentEventManager->SelectInViewer (aPnts, isShiftSelection);
  }
  aCurrentEventManager->FlushViewEvents (aCtx, ViewerTest::CurrentView(), true);
  return 0;
}

//=======================================================================
//function : VMoveTo
//purpose  : Emulates cursor movement to defined pixel position
//=======================================================================
static Standard_Integer VMoveTo (Draw_Interpretor& theDI,
                                Standard_Integer theNbArgs,
                                const char**     theArgVec)
{
  const Handle(AIS_InteractiveContext)& aContext = ViewerTest::GetAISContext();
  const Handle(V3d_View)&               aView    = ViewerTest::CurrentView();
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Graphic3d_Vec2i aMousePos (IntegerLast(), IntegerLast());
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArgStr (theArgVec[anArgIter]);
    anArgStr.LowerCase();
    if (anArgStr == "-reset"
     || anArgStr == "-clear")
    {
      if (anArgIter + 1 < theNbArgs)
      {
        Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter + 1] << "'";
        return 1;
      }

      const Standard_Boolean toEchoGrid = aContext->CurrentViewer()->Grid()->IsActive()
                                       && aContext->CurrentViewer()->GridEcho();
      if (toEchoGrid)
      {
        aContext->CurrentViewer()->HideGridEcho (aView);
      }
      if (aContext->ClearDetected() || toEchoGrid)
      {
        aContext->CurrentViewer()->RedrawImmediate();
      }
      return 0;
    }
    else if (aMousePos.x() == IntegerLast()
          && anArgStr.IsIntegerValue())
    {
      aMousePos.x() = anArgStr.IntegerValue();
    }
    else if (aMousePos.y() == IntegerLast()
          && anArgStr.IsIntegerValue())
    {
      aMousePos.y() = anArgStr.IntegerValue();
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (aMousePos.x() == IntegerLast()
   || aMousePos.y() == IntegerLast())
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }

  ViewerTest::CurrentEventManager()->ResetPreviousMoveTo();
  ViewerTest::CurrentEventManager()->UpdateMousePosition (aMousePos, Aspect_VKeyMouse_NONE, Aspect_VKeyFlags_NONE, false);
  ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), aView, true);

  gp_Pnt aTopPnt (RealLast(), RealLast(), RealLast());
  const Handle(SelectMgr_EntityOwner)& aDetOwner = aContext->DetectedOwner();
  for (Standard_Integer aDetIter = 1; aDetIter <= aContext->MainSelector()->NbPicked(); ++aDetIter)
  {
    if (aContext->MainSelector()->Picked (aDetIter) == aDetOwner)
    {
      aTopPnt = aContext->MainSelector()->PickedPoint (aDetIter);
      break;
    }
  }
  theDI << aTopPnt.X() << " " << aTopPnt.Y() << " " << aTopPnt.Z();
  return 0;
}

namespace
{
  //! Global map storing all animations registered in ViewerTest.
  static NCollection_DataMap<TCollection_AsciiString, Handle(AIS_Animation)> ViewerTest_AnimationTimelineMap;

  //! The animation calling the Draw Harness command.
  class ViewerTest_AnimationProc : public AIS_Animation
  {
  public:

    //! Main constructor.
    ViewerTest_AnimationProc (const TCollection_AsciiString& theAnimationName,
                              Draw_Interpretor* theDI,
                              const TCollection_AsciiString& theCommand)
    : AIS_Animation (theAnimationName),
      myDrawInter(theDI),
      myCommand  (theCommand)
    {
      //
    }

  protected:

    //! Evaluate the command.
    virtual void update (const AIS_AnimationProgress& theProgress) Standard_OVERRIDE
    {
      TCollection_AsciiString aCmd = myCommand;
      replace (aCmd, "%pts",             TCollection_AsciiString(theProgress.Pts));
      replace (aCmd, "%localpts",        TCollection_AsciiString(theProgress.LocalPts));
      replace (aCmd, "%ptslocal",        TCollection_AsciiString(theProgress.LocalPts));
      replace (aCmd, "%normalized",      TCollection_AsciiString(theProgress.LocalNormalized));
      replace (aCmd, "%localnormalized", TCollection_AsciiString(theProgress.LocalNormalized));
      myDrawInter->Eval (aCmd.ToCString());
    }

    //! Find the keyword in the command and replace it with value.
    //! @return the position of the keyword to pass value
    void replace (TCollection_AsciiString&       theCmd,
                  const TCollection_AsciiString& theKey,
                  const TCollection_AsciiString& theVal)
    {
      TCollection_AsciiString aCmd (theCmd);
      aCmd.LowerCase();
      const Standard_Integer aPos = aCmd.Search (theKey);
      if (aPos == -1)
      {
        return;
      }

      TCollection_AsciiString aPart1, aPart2;
      Standard_Integer aPart1To = aPos - 1;
      if (aPart1To >= 1
       && aPart1To <= theCmd.Length())
      {
        aPart1 = theCmd.SubString (1, aPart1To);
      }

      Standard_Integer aPart2From = aPos + theKey.Length();
      if (aPart2From >= 1
       && aPart2From <= theCmd.Length())
      {
        aPart2 = theCmd.SubString (aPart2From, theCmd.Length());
      }

      theCmd = aPart1 + theVal + aPart2;
    }

  protected:

    Draw_Interpretor*       myDrawInter;
    TCollection_AsciiString myCommand;

  };

  //! Replace the animation with the new one.
  static void replaceAnimation (const Handle(AIS_Animation)& theParentAnimation,
                                Handle(AIS_Animation)&       theAnimation,
                                const Handle(AIS_Animation)& theAnimationNew)
  {
    theAnimationNew->CopyFrom (theAnimation);
    if (!theParentAnimation.IsNull())
    {
      theParentAnimation->Replace (theAnimation, theAnimationNew);
    }
    else
    {
      ViewerTest_AnimationTimelineMap.UnBind (theAnimationNew->Name());
      ViewerTest_AnimationTimelineMap.Bind   (theAnimationNew->Name(), theAnimationNew);
    }
    theAnimation = theAnimationNew;
  }

  //! Parse the point.
  static Standard_Boolean parseXYZ (const char** theArgVec, gp_XYZ& thePnt)
  {
    const TCollection_AsciiString anXYZ[3] = { theArgVec[0], theArgVec[1], theArgVec[2] };
    if (!anXYZ[0].IsRealValue()
     || !anXYZ[1].IsRealValue()
     || !anXYZ[2].IsRealValue())
    {
      return Standard_False;
    }

    thePnt.SetCoord (anXYZ[0].RealValue(), anXYZ[1].RealValue(), anXYZ[2].RealValue());
    return Standard_True;
  }

  //! Parse the quaternion.
  static Standard_Boolean parseQuaternion (const char** theArgVec, gp_Quaternion& theQRot)
  {
    const TCollection_AsciiString anXYZW[4] = {theArgVec[0], theArgVec[1], theArgVec[2], theArgVec[3]};
    if (!anXYZW[0].IsRealValue()
     || !anXYZW[1].IsRealValue()
     || !anXYZW[2].IsRealValue()
     || !anXYZW[3].IsRealValue())
    {
      return Standard_False;
    }

    theQRot.Set (anXYZW[0].RealValue(), anXYZW[1].RealValue(), anXYZW[2].RealValue(), anXYZW[3].RealValue());
    return Standard_True;
  }

  //! Auxiliary class for flipping image upside-down.
  class ImageFlipper
  {
  public:

    //! Empty constructor.
    ImageFlipper() : myTmp (NCollection_BaseAllocator::CommonBaseAllocator()) {}

    //! Perform flipping.
    Standard_Boolean FlipY (Image_PixMap& theImage)
    {
      if (theImage.IsEmpty()
       || theImage.SizeX() == 0
       || theImage.SizeY() == 0)
      {
        return Standard_False;
      }

      const Standard_Size aRowSize = theImage.SizeRowBytes();
      if (myTmp.Size() < aRowSize
      && !myTmp.Allocate (aRowSize))
      {
        return Standard_False;
      }

      // for odd height middle row should be left as is
      Standard_Size aNbRowsHalf = theImage.SizeY() / 2;
      for (Standard_Size aRowT = 0, aRowB = theImage.SizeY() - 1; aRowT < aNbRowsHalf; ++aRowT, --aRowB)
      {
        Standard_Byte* aTop = theImage.ChangeRow (aRowT);
        Standard_Byte* aBot = theImage.ChangeRow (aRowB);
        memcpy (myTmp.ChangeData(), aTop,         aRowSize);
        memcpy (aTop,               aBot,         aRowSize);
        memcpy (aBot,               myTmp.Data(), aRowSize);
      }
      return Standard_True;
    }

  private:
    NCollection_Buffer myTmp;
  };

}

//=================================================================================================
//function : VViewParams
//purpose  : Gets or sets AIS View characteristics
//=================================================================================================
static int VViewParams (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Standard_Boolean toSetProj     = Standard_False;
  Standard_Boolean toSetUp       = Standard_False;
  Standard_Boolean toSetAt       = Standard_False;
  Standard_Boolean toSetEye      = Standard_False;
  Standard_Boolean toSetScale    = Standard_False;
  Standard_Boolean toSetSize     = Standard_False;
  Standard_Boolean toSetCenter2d = Standard_False;
  Standard_Real    aViewScale = aView->Scale();
  Standard_Real    aViewSize  = 1.0;
  Graphic3d_Vec2i  aCenter2d;
  gp_XYZ aViewProj, aViewUp, aViewAt, aViewEye;
  aView->Proj (aViewProj.ChangeCoord (1), aViewProj.ChangeCoord (2), aViewProj.ChangeCoord (3));
  aView->Up   (aViewUp  .ChangeCoord (1), aViewUp  .ChangeCoord (2), aViewUp  .ChangeCoord (3));
  aView->At   (aViewAt  .ChangeCoord (1), aViewAt  .ChangeCoord (2), aViewAt  .ChangeCoord (3));
  aView->Eye  (aViewEye .ChangeCoord (1), aViewEye .ChangeCoord (2), aViewEye .ChangeCoord (3));
  if (theArgsNb == 1)
  {
    // print all of the available view parameters
    char aText[4096];
    Sprintf (aText,
             "Scale: %g\n"
             "Proj:  %12g %12g %12g\n"
             "Up:    %12g %12g %12g\n"
             "At:    %12g %12g %12g\n"
             "Eye:   %12g %12g %12g\n",
              aViewScale,
              aViewProj.X(), aViewProj.Y(), aViewProj.Z(),
              aViewUp.X(),   aViewUp.Y(),   aViewUp.Z(),
              aViewAt.X(),   aViewAt.Y(),   aViewAt.Z(),
              aViewEye.X(),  aViewEye.Y(),  aViewEye.Z());
    theDi << aText;
    return 0;
  }

  ViewerTest_AutoUpdater anUpdateTool (ViewerTest::GetAISContext(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      continue;
    }
    else if (anArg == "-cmd"
          || anArg == "-command"
          || anArg == "-args")
    {
      char aText[4096];
      Sprintf (aText,
               "-scale %g "
               "-proj %g %g %g "
               "-up %g %g %g "
               "-at %g %g %g\n",
                aViewScale,
                aViewProj.X(), aViewProj.Y(), aViewProj.Z(),
                aViewUp.X(),   aViewUp.Y(),   aViewUp.Z(),
                aViewAt.X(),   aViewAt.Y(),   aViewAt.Z());
      theDi << aText;
    }
    else if (anArg == "-scale"
          || anArg == "-size")
    {
      if (anArgIter + 1 < theArgsNb
       && *theArgVec[anArgIter + 1] != '-')
      {
        const TCollection_AsciiString aValueArg (theArgVec[anArgIter + 1]);
        if (aValueArg.IsRealValue())
        {
          ++anArgIter;
          if (anArg == "-scale")
          {
            toSetScale = Standard_True;
            aViewScale = aValueArg.RealValue();
          }
          else if (anArg == "-size")
          {
            toSetSize = Standard_True;
            aViewSize = aValueArg.RealValue();
          }
          continue;
        }
      }
      if (anArg == "-scale")
      {
        theDi << "Scale: " << aView->Scale() << "\n";
      }
      else if (anArg == "-size")
      {
        Graphic3d_Vec2d aSizeXY;
        aView->Size (aSizeXY.x(), aSizeXY.y());
        theDi << "Size: " << aSizeXY.x() << " " << aSizeXY.y() << "\n";
      }
    }
    else if (anArg == "-eye"
          || anArg == "-at"
          || anArg == "-up"
          || anArg == "-proj")
    {
      if (anArgIter + 3 < theArgsNb)
      {
        gp_XYZ anXYZ;
        if (parseXYZ (theArgVec + anArgIter + 1, anXYZ))
        {
          anArgIter += 3;
          if (anArg == "-eye")
          {
            toSetEye = Standard_True;
            aViewEye = anXYZ;
          }
          else if (anArg == "-at")
          {
            toSetAt = Standard_True;
            aViewAt = anXYZ;
          }
          else if (anArg == "-up")
          {
            toSetUp = Standard_True;
            aViewUp = anXYZ;
          }
          else if (anArg == "-proj")
          {
            toSetProj = Standard_True;
            aViewProj = anXYZ;
          }
          continue;
        }
      }

      if (anArg == "-eye")
      {
        theDi << "Eye:  " << aViewEye.X() << " " << aViewEye.Y() << " " << aViewEye.Z() << "\n";
      }
      else if (anArg == "-at")
      {
        theDi << "At:   " << aViewAt.X() << " " << aViewAt.Y() << " " << aViewAt.Z() << "\n";
      }
      else if (anArg == "-up")
      {
        theDi << "Up:   " << aViewUp.X() << " " << aViewUp.Y() << " " << aViewUp.Z() << "\n";
      }
      else if (anArg == "-proj")
      {
        theDi << "Proj: " << aViewProj.X() << " " << aViewProj.Y() << " " << aViewProj.Z() << "\n";
      }
    }
    else if (anArg == "-center")
    {
      if (anArgIter + 2 < theArgsNb)
      {
        const TCollection_AsciiString anX (theArgVec[anArgIter + 1]);
        const TCollection_AsciiString anY (theArgVec[anArgIter + 2]);
        if (anX.IsIntegerValue()
         && anY.IsIntegerValue())
        {
          toSetCenter2d = Standard_True;
          aCenter2d = Graphic3d_Vec2i (anX.IntegerValue(), anY.IntegerValue());
        }
      }
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  // change view parameters in proper order
  if (toSetScale)
  {
    aView->SetScale (aViewScale);
  }
  if (toSetSize)
  {
    aView->SetSize (aViewSize);
  }
  if (toSetEye)
  {
    aView->SetEye (aViewEye.X(), aViewEye.Y(), aViewEye.Z());
  }
  if (toSetAt)
  {
    aView->SetAt (aViewAt.X(), aViewAt.Y(), aViewAt.Z());
  }
  if (toSetProj)
  {
    aView->SetProj (aViewProj.X(), aViewProj.Y(), aViewProj.Z());
  }
  if (toSetUp)
  {
    aView->SetUp (aViewUp.X(), aViewUp.Y(), aViewUp.Z());
  }
  if (toSetCenter2d)
  {
    aView->SetCenter (aCenter2d.x(), aCenter2d.y());
  }

  return 0;
}

//==============================================================================
//function : V2DMode
//purpose  :
//==============================================================================
static Standard_Integer V2DMode (Draw_Interpretor&, Standard_Integer theArgsNb, const char** theArgVec)
{
  bool is2dMode = true;
  Handle(ViewerTest_V3dView) aV3dView = Handle(ViewerTest_V3dView)::DownCast (ViewerTest::CurrentView());
  if (aV3dView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }
  for (Standard_Integer anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    const TCollection_AsciiString anArg = theArgVec[anArgIt];
    TCollection_AsciiString anArgCase = anArg;
    anArgCase.LowerCase();
    if (anArgIt + 1 < theArgsNb
     && anArgCase == "-name")
    {
      ViewerTest_Names aViewNames (theArgVec[++anArgIt]);
      TCollection_AsciiString aViewName = aViewNames.GetViewName();
      if (!ViewerTest_myViews.IsBound1 (aViewName))
      {
        Message::SendFail() << "Syntax error: unknown view '" << theArgVec[anArgIt - 1] << "'";
        return 1;
      }
      aV3dView = Handle(ViewerTest_V3dView)::DownCast (ViewerTest_myViews.Find1 (aViewName));
    }
    else if (anArgCase == "-mode")
    {
      if (anArgIt + 1 < theArgsNb
       && Draw::ParseOnOff (theArgVec[anArgIt + 1], is2dMode))
      {
        ++anArgIt;
      }
    }
    else if (Draw::ParseOnOff (theArgVec[anArgIt], is2dMode))
    {
      //
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument " << anArg;
      return 1;
    }
  }

  aV3dView->SetView2DMode (is2dMode);
  return 0;
}

//==============================================================================
//function : VAnimation
//purpose  :
//==============================================================================
static Standard_Integer VAnimation (Draw_Interpretor& theDI,
                                    Standard_Integer  theArgNb,
                                    const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (theArgNb < 2)
  {
    for (NCollection_DataMap<TCollection_AsciiString, Handle(AIS_Animation)>::Iterator
         anAnimIter (ViewerTest_AnimationTimelineMap); anAnimIter.More(); anAnimIter.Next())
    {
      theDI << anAnimIter.Key() << " " << anAnimIter.Value()->Duration() << " sec\n";
    }
    return 0;
  }
  if (aCtx.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Standard_Integer anArgIter = 1;
  TCollection_AsciiString aNameArg (theArgVec[anArgIter++]);
  if (aNameArg.IsEmpty())
  {
    Message::SendFail ("Syntax error: animation name is not defined");
    return 1;
  }

  TCollection_AsciiString aNameArgLower = aNameArg;
  aNameArgLower.LowerCase();
  if (aNameArgLower == "-reset"
   || aNameArgLower == "-clear")
  {
    ViewerTest_AnimationTimelineMap.Clear();
    return 0;
  }
  else if (aNameArg.Value (1) == '-')
  {
    Message::SendFail() << "Syntax error: invalid animation name '" << aNameArg  << "'";
    return 1;
  }

  const char* aNameSplitter = "/";
  Standard_Integer aSplitPos = aNameArg.Search (aNameSplitter);
  if (aSplitPos == -1)
  {
    aNameSplitter = ".";
    aSplitPos = aNameArg.Search (aNameSplitter);
  }

  // find existing or create a new animation by specified name within syntax "parent.child".
  Handle(AIS_Animation) aRootAnimation, aParentAnimation, anAnimation;
  for (; !aNameArg.IsEmpty();)
  {
    TCollection_AsciiString aNameParent;
    if (aSplitPos != -1)
    {
      if (aSplitPos == aNameArg.Length())
      {
        Message::SendFail ("Syntax error: animation name is not defined");
        return 1;
      }

      aNameParent = aNameArg.SubString (            1, aSplitPos - 1);
      aNameArg    = aNameArg.SubString (aSplitPos + 1, aNameArg.Length());

      aSplitPos = aNameArg.Search (aNameSplitter);
    }
    else
    {
      aNameParent = aNameArg;
      aNameArg.Clear();
    }

    if (anAnimation.IsNull())
    {
      if (!ViewerTest_AnimationTimelineMap.Find (aNameParent, anAnimation))
      {
        anAnimation = new AIS_Animation (aNameParent);
        ViewerTest_AnimationTimelineMap.Bind (aNameParent, anAnimation);
      }
      aRootAnimation = anAnimation;
    }
    else
    {
      aParentAnimation = anAnimation;
      anAnimation = aParentAnimation->Find (aNameParent);
      if (anAnimation.IsNull())
      {
        anAnimation = new AIS_Animation (aNameParent);
        aParentAnimation->Add (anAnimation);
      }
    }
  }

  if (anArgIter >= theArgNb)
  {
    // just print the list of children
    for (NCollection_Sequence<Handle(AIS_Animation)>::Iterator anAnimIter (anAnimation->Children()); anAnimIter.More(); anAnimIter.Next())
    {
      theDI << anAnimIter.Value()->Name() << " " << anAnimIter.Value()->Duration() << " sec\n";
    }
    return 0;
  }

  // animation parameters
  Standard_Boolean toPlay = Standard_False;
  Standard_Real aPlaySpeed     = 1.0;
  Standard_Real aPlayStartTime = anAnimation->StartPts();
  Standard_Real aPlayDuration  = anAnimation->Duration();
  Standard_Boolean isFreeCamera = Standard_False;
  Standard_Boolean isLockLoop   = Standard_False;

  // video recording parameters
  TCollection_AsciiString aRecFile;
  Image_VideoParams aRecParams;

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    // general options
    if (anArg == "-reset"
     || anArg == "-clear")
    {
      anAnimation->Clear();
    }
    else if (anArg == "-remove"
          || anArg == "-del"
          || anArg == "-delete")
    {
      if (!aParentAnimation.IsNull())
      {
        ViewerTest_AnimationTimelineMap.UnBind (anAnimation->Name());
      }
      else
      {
        aParentAnimation->Remove (anAnimation);
      }
    }
    // playback options
    else if (anArg == "-play")
    {
      toPlay = Standard_True;
      if (++anArgIter < theArgNb)
      {
        if (*theArgVec[anArgIter] == '-')
        {
          --anArgIter;
          continue;
        }
        aPlayStartTime = Draw::Atof (theArgVec[anArgIter]);

        if (++anArgIter < theArgNb)
        {
          if (*theArgVec[anArgIter] == '-')
          {
            --anArgIter;
            continue;
          }
          aPlayDuration = Draw::Atof (theArgVec[anArgIter]);
        }
      }
    }
    else if (anArg == "-resume")
    {
      toPlay = Standard_True;
      aPlayStartTime = anAnimation->ElapsedTime();
      if (++anArgIter < theArgNb)
      {
        if (*theArgVec[anArgIter] == '-')
        {
          --anArgIter;
          continue;
        }

        aPlayDuration = Draw::Atof (theArgVec[anArgIter]);
      }
    }
    else if (anArg == "-playspeed"
          || anArg == "-speed")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg << "";
        return 1;
      }
      aPlaySpeed = Draw::Atof (theArgVec[anArgIter]);
    }
    else if (anArg == "-lock"
          || anArg == "-lockloop"
          || anArg == "-playlockloop")
    {
      isLockLoop = Standard_True;
    }
    else if (anArg == "-freecamera"
          || anArg == "-playfreecamera"
          || anArg == "-freelook")
    {
      isFreeCamera = Standard_True;
    }
    // video recodring options
    else if (anArg == "-rec"
          || anArg == "-record")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      aRecFile = theArgVec[anArgIter];
      if (aRecParams.FpsNum <= 0)
      {
        aRecParams.FpsNum = 24;
      }

      if (anArgIter + 2 < theArgNb
      && *theArgVec[anArgIter + 1] != '-'
      && *theArgVec[anArgIter + 2] != '-')
      {
        TCollection_AsciiString aWidthArg  (theArgVec[anArgIter + 1]);
        TCollection_AsciiString aHeightArg (theArgVec[anArgIter + 2]);
        if (aWidthArg .IsIntegerValue()
         && aHeightArg.IsIntegerValue())
        {
          aRecParams.Width  = aWidthArg .IntegerValue();
          aRecParams.Height = aHeightArg.IntegerValue();
          anArgIter += 2;
        }
      }
    }
    else if (anArg == "-fps")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      TCollection_AsciiString aFpsArg (theArgVec[anArgIter]);
      Standard_Integer aSplitIndex = aFpsArg.FirstLocationInSet ("/", 1, aFpsArg.Length());
      if (aSplitIndex == 0)
      {
        aRecParams.FpsNum = aFpsArg.IntegerValue();
      }
      else
      {
        const TCollection_AsciiString aDenStr = aFpsArg.Split (aSplitIndex);
        aFpsArg.Split (aFpsArg.Length() - 1);
        const TCollection_AsciiString aNumStr = aFpsArg;
        aRecParams.FpsNum = aNumStr.IntegerValue();
        aRecParams.FpsDen = aDenStr.IntegerValue();
        if (aRecParams.FpsDen < 1)
        {
          Message::SendFail() << "Syntax error at " << anArg;
          return 1;
        }
      }
    }
    else if (anArg == "-format")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      aRecParams.Format = theArgVec[anArgIter];
    }
    else if (anArg == "-pix_fmt"
          || anArg == "-pixfmt"
          || anArg == "-pixelformat")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      aRecParams.PixelFormat = theArgVec[anArgIter];
    }
    else if (anArg == "-codec"
          || anArg == "-vcodec"
          || anArg == "-videocodec")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      aRecParams.VideoCodec = theArgVec[anArgIter];
    }
    else if (anArg == "-crf"
          || anArg == "-preset"
          || anArg == "-qp")
    {
      const TCollection_AsciiString aParamName = anArg.SubString (2, anArg.Length());
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      aRecParams.VideoCodecParams.Bind (aParamName, theArgVec[anArgIter]);
    }
    // animation definition options
    else if (anArg == "-start"
          || anArg == "-starttime"
          || anArg == "-startpts")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      anAnimation->SetStartPts (Draw::Atof (theArgVec[anArgIter]));
      aRootAnimation->UpdateTotalDuration();
    }
    else if (anArg == "-end"
          || anArg == "-endtime"
          || anArg == "-endpts")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      anAnimation->SetOwnDuration (Draw::Atof (theArgVec[anArgIter]) - anAnimation->StartPts());
      aRootAnimation->UpdateTotalDuration();
    }
    else if (anArg == "-dur"
          || anArg == "-duration")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      anAnimation->SetOwnDuration (Draw::Atof (theArgVec[anArgIter]));
      aRootAnimation->UpdateTotalDuration();
    }
    else if (anArg == "-command"
          || anArg == "-cmd"
          || anArg == "-invoke"
          || anArg == "-eval"
          || anArg == "-proc")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      Handle(ViewerTest_AnimationProc) aCmdAnimation = new ViewerTest_AnimationProc (anAnimation->Name(), &theDI, theArgVec[anArgIter]);
      replaceAnimation (aParentAnimation, anAnimation, aCmdAnimation);
    }
    else if (anArg == "-objecttrsf"
          || anArg == "-objectransformation"
          || anArg == "-objtransformation"
          || anArg == "-objtrsf"
          || anArg == "-object"
          || anArg == "-obj")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      TCollection_AsciiString anObjName (theArgVec[anArgIter]);
      const ViewerTest_DoubleMapOfInteractiveAndName& aMapOfAIS = GetMapOfAIS();
      Handle(AIS_InteractiveObject) anObject;
      if (!aMapOfAIS.Find2 (anObjName, anObject))
      {
        Message::SendFail() << "Syntax error: wrong object name at " << anArg;
        return 1;
      }

      gp_Trsf       aTrsfs   [2] = { anObject->LocalTransformation(), anObject->LocalTransformation() };
      gp_Quaternion aRotQuats[2] = { aTrsfs[0].GetRotation(),         aTrsfs[1].GetRotation() };
      gp_XYZ        aLocPnts [2] = { aTrsfs[0].TranslationPart(),     aTrsfs[1].TranslationPart() };
      Standard_Real aScales  [2] = { aTrsfs[0].ScaleFactor(),         aTrsfs[1].ScaleFactor() };
      Standard_Boolean isTrsfSet = Standard_False;
      Standard_Integer aTrsfArgIter = anArgIter + 1;
      for (; aTrsfArgIter < theArgNb; ++aTrsfArgIter)
      {
        TCollection_AsciiString aTrsfArg (theArgVec[aTrsfArgIter]);
        aTrsfArg.LowerCase();
        const Standard_Integer anIndex = aTrsfArg.EndsWith ("1") ? 0 : 1;
        if (aTrsfArg.StartsWith ("-rotation")
         || aTrsfArg.StartsWith ("-rot"))
        {
          isTrsfSet = Standard_True;
          if (aTrsfArgIter + 4 >= theArgNb
          || !parseQuaternion (theArgVec + aTrsfArgIter + 1, aRotQuats[anIndex]))
          {
            Message::SendFail() << "Syntax error at " << aTrsfArg;
            return 1;
          }
          aTrsfArgIter += 4;
        }
        else if (aTrsfArg.StartsWith ("-location")
              || aTrsfArg.StartsWith ("-loc"))
        {
          isTrsfSet = Standard_True;
          if (aTrsfArgIter + 3 >= theArgNb
          || !parseXYZ (theArgVec + aTrsfArgIter + 1, aLocPnts[anIndex]))
          {
            Message::SendFail() << "Syntax error at " << aTrsfArg;
            return 1;
          }
          aTrsfArgIter += 3;
        }
        else if (aTrsfArg.StartsWith ("-scale"))
        {
          isTrsfSet = Standard_True;
          if (++aTrsfArgIter >= theArgNb)
          {
            Message::SendFail() << "Syntax error at " << aTrsfArg;
            return 1;
          }

          const TCollection_AsciiString aScaleStr (theArgVec[aTrsfArgIter]);
          if (!aScaleStr.IsRealValue())
          {
            Message::SendFail() << "Syntax error at " << aTrsfArg;
            return 1;
          }
          aScales[anIndex] = aScaleStr.RealValue();
        }
        else
        {
          anArgIter = aTrsfArgIter - 1;
          break;
        }
      }
      if (!isTrsfSet)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      else if (aTrsfArgIter >= theArgNb)
      {
        anArgIter = theArgNb;
      }

      aTrsfs[0].SetRotation        (aRotQuats[0]);
      aTrsfs[1].SetRotation        (aRotQuats[1]);
      aTrsfs[0].SetTranslationPart (aLocPnts[0]);
      aTrsfs[1].SetTranslationPart (aLocPnts[1]);
      aTrsfs[0].SetScaleFactor     (aScales[0]);
      aTrsfs[1].SetScaleFactor     (aScales[1]);

      Handle(AIS_AnimationObject) anObjAnimation = new AIS_AnimationObject (anAnimation->Name(), aCtx, anObject, aTrsfs[0], aTrsfs[1]);
      replaceAnimation (aParentAnimation, anAnimation, anObjAnimation);
    }
    else if (anArg == "-viewtrsf"
          || anArg == "-view")
    {
      Handle(AIS_AnimationCamera) aCamAnimation = Handle(AIS_AnimationCamera)::DownCast (anAnimation);
      if (aCamAnimation.IsNull())
      {
        aCamAnimation = new AIS_AnimationCamera (anAnimation->Name(), aView);
        replaceAnimation (aParentAnimation, anAnimation, aCamAnimation);
      }

      Handle(Graphic3d_Camera) aCams[2] =
      {
        new Graphic3d_Camera (aCamAnimation->View()->Camera()),
        new Graphic3d_Camera (aCamAnimation->View()->Camera())
      };

      Standard_Boolean isTrsfSet = Standard_False;
      Standard_Integer aViewArgIter = anArgIter + 1;
      for (; aViewArgIter < theArgNb; ++aViewArgIter)
      {
        TCollection_AsciiString aViewArg (theArgVec[aViewArgIter]);
        aViewArg.LowerCase();
        const Standard_Integer anIndex = aViewArg.EndsWith("1") ? 0 : 1;
        if (aViewArg.StartsWith ("-scale"))
        {
          isTrsfSet = Standard_True;
          if (++aViewArgIter >= theArgNb)
          {
            Message::SendFail() << "Syntax error at " << anArg;
            return 1;
          }

          const TCollection_AsciiString aScaleStr (theArgVec[aViewArgIter]);
          if (!aScaleStr.IsRealValue())
          {
            Message::SendFail() << "Syntax error at " << aViewArg;
            return 1;
          }
          Standard_Real aScale = aScaleStr.RealValue();
          aScale = aCamAnimation->View()->DefaultCamera()->Scale() / aScale;
          aCams[anIndex]->SetScale (aScale);
        }
        else if (aViewArg.StartsWith ("-eye")
              || aViewArg.StartsWith ("-center")
              || aViewArg.StartsWith ("-at")
              || aViewArg.StartsWith ("-up"))
        {
          isTrsfSet = Standard_True;
          gp_XYZ anXYZ;
          if (aViewArgIter + 3 >= theArgNb
          || !parseXYZ (theArgVec + aViewArgIter + 1, anXYZ))
          {
            Message::SendFail() << "Syntax error at " << aViewArg;
            return 1;
          }
          aViewArgIter += 3;

          if (aViewArg.StartsWith ("-eye"))
          {
            aCams[anIndex]->SetEye (anXYZ);
          }
          else if (aViewArg.StartsWith ("-center")
                || aViewArg.StartsWith ("-at"))
          {
            aCams[anIndex]->SetCenter (anXYZ);
          }
          else if (aViewArg.StartsWith ("-up"))
          {
            aCams[anIndex]->SetUp (anXYZ);
          }
        }
        else
        {
          anArgIter = aViewArgIter - 1;
          break;
        }
      }
      if (!isTrsfSet)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      else if (aViewArgIter >= theArgNb)
      {
        anArgIter = theArgNb;
      }

      aCamAnimation->SetCameraStart(aCams[0]);
      aCamAnimation->SetCameraEnd  (aCams[1]);
    }
    else
    {
      Message::SendFail() << "Syntax error at " << anArg;
      return 1;
    }
  }

  if (!toPlay && aRecFile.IsEmpty())
  {
    return 0;
  }

  // Start animation timeline and process frame updating.
  TheIsAnimating = Standard_True;
  const Standard_Boolean wasImmediateUpdate = aView->SetImmediateUpdate (Standard_False);
  Handle(Graphic3d_Camera) aCameraBack = new Graphic3d_Camera (aView->Camera());
  anAnimation->StartTimer (aPlayStartTime, aPlaySpeed, Standard_True, aPlayDuration <= 0.0);
  if (isFreeCamera)
  {
    aView->Camera()->Copy (aCameraBack);
  }

  const Standard_Real anUpperPts = aPlayStartTime + aPlayDuration;
  if (aRecParams.FpsNum <= 0)
  {
    while (!anAnimation->IsStopped())
    {
      aCameraBack->Copy (aView->Camera());
      const Standard_Real aPts = anAnimation->UpdateTimer();
      if (isFreeCamera)
      {
        aView->Camera()->Copy (aCameraBack);
      }

      if (aPts >= anUpperPts)
      {
        anAnimation->Pause();
        break;
      }

      if (aView->IsInvalidated())
      {
        aView->Redraw();
      }
      else
      {
        aView->RedrawImmediate();
      }

      if (!isLockLoop)
      {
        // handle user events
        theDI.Eval ("after 1 set waiter 1");
        theDI.Eval ("vwait waiter");
      }
      if (!TheIsAnimating)
      {
        anAnimation->Pause();
        theDI << aPts;
        break;
      }
    }

    if (aView->IsInvalidated())
    {
      aView->Redraw();
    }
    else
    {
      aView->RedrawImmediate();
    }
  }
  else
  {
    OSD_Timer aPerfTimer;
    aPerfTimer.Start();

    Handle(Image_VideoRecorder) aRecorder;
    ImageFlipper aFlipper;
    Handle(Draw_ProgressIndicator) aProgress;
    if (!aRecFile.IsEmpty())
    {
      if (aRecParams.Width  <= 0
       || aRecParams.Height <= 0)
      {
        aView->Window()->Size (aRecParams.Width, aRecParams.Height);
      }

      aRecorder = new Image_VideoRecorder();
      if (!aRecorder->Open (aRecFile.ToCString(), aRecParams))
      {
        Message::SendFail ("Error: failed to open video file for recording");
        return 0;
      }

      aProgress = new Draw_ProgressIndicator (theDI, 1);
    }

    // Manage frame-rated animation here
    Standard_Real aPts = aPlayStartTime;
    int64_t aNbFrames = 0;
    Message_ProgressScope aPS(Message_ProgressIndicator::Start(aProgress),
                              "Video recording, sec", Max(1, Standard_Integer(aPlayDuration / aPlaySpeed)));
    Standard_Integer aSecondsProgress = 0;
    for (; aPts <= anUpperPts && aPS.More();)
    {
      const Standard_Real aRecPts = aPlaySpeed * ((Standard_Real(aRecParams.FpsDen) / Standard_Real(aRecParams.FpsNum)) * Standard_Real(aNbFrames));
      aPts = aPlayStartTime + aRecPts;
      ++aNbFrames;
      if (!anAnimation->Update (aPts))
      {
        break;
      }

      if (!aRecorder.IsNull())
      {
        V3d_ImageDumpOptions aDumpParams;
        aDumpParams.Width          = aRecParams.Width;
        aDumpParams.Height         = aRecParams.Height;
        aDumpParams.BufferType     = Graphic3d_BT_RGBA;
        aDumpParams.StereoOptions  = V3d_SDO_MONO;
        aDumpParams.ToAdjustAspect = Standard_True;
        if (!aView->ToPixMap (aRecorder->ChangeFrame(), aDumpParams))
        {
          Message::SendFail ("Error: view dump is failed");
          return 0;
        }
        aFlipper.FlipY (aRecorder->ChangeFrame());
        if (!aRecorder->PushFrame())
        {
          return 0;
        }
      }
      else
      {
        aView->Redraw();
      }

      while (aSecondsProgress < Standard_Integer(aRecPts / aPlaySpeed))
      {
        aPS.Next();
        ++aSecondsProgress;
      }
    }

    aPerfTimer.Stop();
    anAnimation->Stop();
    const Standard_Real aRecFps = Standard_Real(aNbFrames) / aPerfTimer.ElapsedTime();
    theDI << "Average FPS: " << aRecFps << "\n"
          << "Nb. Frames: "  << Standard_Real(aNbFrames);

    aView->Redraw();
  }

  aView->SetImmediateUpdate (wasImmediateUpdate);
  TheIsAnimating = Standard_False;
  return 0;
}


//=======================================================================
//function : VChangeSelected
//purpose  : Adds the shape to selection or remove one from it
//=======================================================================
static Standard_Integer VChangeSelected (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  if(argc != 2)
  {
    di<<"Usage : " << argv[0] << " shape \n";
    return 1;
  }
  //get AIS_Shape:
  TCollection_AsciiString aName(argv[1]);
  Handle(AIS_InteractiveObject) anAISObject;
  if (!GetMapOfAIS().Find2 (aName, anAISObject)
    || anAISObject.IsNull())
  {
    di<<"Use 'vdisplay' before";
    return 1;
  }

  ViewerTest::GetAISContext()->AddOrRemoveSelected(anAISObject, Standard_True);
  return 0;
}

//=======================================================================
//function : VNbSelected
//purpose  : Returns number of selected objects
//=======================================================================
static Standard_Integer VNbSelected (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  if(argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  di << aContext->NbSelected() << "\n";
  return 0;
}

//=======================================================================
//function : VPurgeDisplay
//purpose  : Switches altialiasing on or off
//=======================================================================
static Standard_Integer VPurgeDisplay (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  if (argc > 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  di << aContext->PurgeDisplay() << "\n";
  return 0;
}

//=======================================================================
//function : VSetViewSize
//purpose  :
//=======================================================================
static Standard_Integer VSetViewSize (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if(argc != 2)
  {
    di<<"Usage : " << argv[0] << " Size\n";
    return 1;
  }
  Standard_Real aSize = Draw::Atof (argv[1]);
  if (aSize <= 0.)
  {
    di<<"Bad Size value  : " << aSize << "\n";
    return 1;
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->SetSize(aSize);
  return 0;
}

//=======================================================================
//function : VMoveView
//purpose  :
//=======================================================================
static Standard_Integer VMoveView (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if(argc < 4 || argc > 5)
  {
    di<<"Usage : " << argv[0] << " Dx Dy Dz [Start = 1|0]\n";
    return 1;
  }
  Standard_Real Dx = Draw::Atof (argv[1]);
  Standard_Real Dy = Draw::Atof (argv[2]);
  Standard_Real Dz = Draw::Atof (argv[3]);
  Standard_Boolean aStart = Standard_True;
  if (argc == 5)
  {
      aStart = (Draw::Atoi (argv[4]) > 0);
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->Move(Dx,Dy,Dz,aStart);
  return 0;
}

//=======================================================================
//function : VTranslateView
//purpose  :
//=======================================================================
static Standard_Integer VTranslateView (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if(argc < 4 || argc > 5)
  {
    di<<"Usage : " << argv[0] << " Dx Dy Dz [Start = 1|0]\n";
    return 1;
  }
  Standard_Real Dx = Draw::Atof (argv[1]);
  Standard_Real Dy = Draw::Atof (argv[2]);
  Standard_Real Dz = Draw::Atof (argv[3]);
  Standard_Boolean aStart = Standard_True;
  if (argc == 5)
  {
      aStart = (Draw::Atoi (argv[4]) > 0);
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->Translate(Dx,Dy,Dz,aStart);
  return 0;
}

//=======================================================================
//function : VTurnView
//purpose  :
//=======================================================================
static Standard_Integer VTurnView (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if(argc < 4 || argc > 5){
    di<<"Usage : " << argv[0] << " Ax Ay Az [Start = 1|0]\n";
    return 1;
  }
  Standard_Real Ax = Draw::Atof (argv[1]);
  Standard_Real Ay = Draw::Atof (argv[2]);
  Standard_Real Az = Draw::Atof (argv[3]);
  Standard_Boolean aStart = Standard_True;
  if (argc == 5)
  {
      aStart = (Draw::Atoi (argv[4]) > 0);
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->Turn(Ax,Ay,Az,aStart);
  return 0;
}

//==============================================================================
//function : VTextureEnv
//purpose  : ENables or disables environment mapping
//==============================================================================
class OCC_TextureEnv : public Graphic3d_TextureEnv
{
public:
  OCC_TextureEnv(const Standard_CString FileName);
  OCC_TextureEnv(const Graphic3d_NameOfTextureEnv aName);
  void SetTextureParameters(const Standard_Boolean theRepeatFlag,
                            const Standard_Boolean theModulateFlag,
                            const Graphic3d_TypeOfTextureFilter theFilter,
                            const Standard_ShortReal theXScale,
                            const Standard_ShortReal theYScale,
                            const Standard_ShortReal theXShift,
                            const Standard_ShortReal theYShift,
                            const Standard_ShortReal theAngle);
  DEFINE_STANDARD_RTTI_INLINE(OCC_TextureEnv,Graphic3d_TextureEnv)
};
DEFINE_STANDARD_HANDLE(OCC_TextureEnv, Graphic3d_TextureEnv)

OCC_TextureEnv::OCC_TextureEnv(const Standard_CString theFileName)
  : Graphic3d_TextureEnv(theFileName)
{
}

OCC_TextureEnv::OCC_TextureEnv(const Graphic3d_NameOfTextureEnv theTexId)
  : Graphic3d_TextureEnv(theTexId)
{
}

void OCC_TextureEnv::SetTextureParameters(const Standard_Boolean theRepeatFlag,
                                          const Standard_Boolean theModulateFlag,
                                          const Graphic3d_TypeOfTextureFilter theFilter,
                                          const Standard_ShortReal theXScale,
                                          const Standard_ShortReal theYScale,
                                          const Standard_ShortReal theXShift,
                                          const Standard_ShortReal theYShift,
                                          const Standard_ShortReal theAngle)
{
  myParams->SetRepeat     (theRepeatFlag);
  myParams->SetModulate   (theModulateFlag);
  myParams->SetFilter     (theFilter);
  myParams->SetScale      (Graphic3d_Vec2(theXScale, theYScale));
  myParams->SetTranslation(Graphic3d_Vec2(theXShift, theYShift));
  myParams->SetRotation   (theAngle);
}

static int VTextureEnv (Draw_Interpretor& /*theDI*/, Standard_Integer theArgNb, const char** theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  // Checking the input arguments
  Standard_Boolean anEnableFlag = Standard_False;
  Standard_Boolean isOk         = theArgNb >= 2;
  if (isOk)
  {
    TCollection_AsciiString anEnableOpt(theArgVec[1]);
    anEnableFlag = anEnableOpt.IsEqual("on");
    isOk         = anEnableFlag || anEnableOpt.IsEqual("off");
  }
  if (anEnableFlag)
  {
    isOk = (theArgNb == 3 || theArgNb == 11);
    if (isOk)
    {
      TCollection_AsciiString aTextureOpt(theArgVec[2]);
      isOk = (!aTextureOpt.IsIntegerValue() ||
             (aTextureOpt.IntegerValue() >= 0 && aTextureOpt.IntegerValue() < Graphic3d_NOT_ENV_UNKNOWN));

      if (isOk && theArgNb == 11)
      {
        TCollection_AsciiString aRepeatOpt  (theArgVec[3]),
                                aModulateOpt(theArgVec[4]),
                                aFilterOpt  (theArgVec[5]),
                                aSScaleOpt  (theArgVec[6]),
                                aTScaleOpt  (theArgVec[7]),
                                aSTransOpt  (theArgVec[8]),
                                aTTransOpt  (theArgVec[9]),
                                anAngleOpt  (theArgVec[10]);
        isOk = ((aRepeatOpt.  IsEqual("repeat")   || aRepeatOpt.  IsEqual("clamp")) &&
                (aModulateOpt.IsEqual("modulate") || aModulateOpt.IsEqual("decal")) &&
                (aFilterOpt.  IsEqual("nearest")  || aFilterOpt.  IsEqual("bilinear") || aFilterOpt.IsEqual("trilinear")) &&
                aSScaleOpt.IsRealValue() && aTScaleOpt.IsRealValue() &&
                aSTransOpt.IsRealValue() && aTTransOpt.IsRealValue() &&
                anAngleOpt.IsRealValue());
      }
    }
  }

  if (!isOk)
  {
    Message::SendFail() << "Usage:\n"
                        << theArgVec[0] << " off\n"
                        << theArgVec[0] << " on {index_of_std_texture(0..7)|texture_file_name} [{clamp|repeat} {decal|modulate} {nearest|bilinear|trilinear} scale_s scale_t translation_s translation_t rotation_degrees]";
    return 1;
  }

  if (anEnableFlag)
  {
    TCollection_AsciiString aTextureOpt(theArgVec[2]);
    Handle(OCC_TextureEnv) aTexEnv = aTextureOpt.IsIntegerValue() ?
                                     new OCC_TextureEnv((Graphic3d_NameOfTextureEnv)aTextureOpt.IntegerValue()) :
                                     new OCC_TextureEnv(theArgVec[2]);

    if (theArgNb == 11)
    {
      TCollection_AsciiString aRepeatOpt(theArgVec[3]), aModulateOpt(theArgVec[4]), aFilterOpt(theArgVec[5]);
      aTexEnv->SetTextureParameters(
        aRepeatOpt.  IsEqual("repeat"),
        aModulateOpt.IsEqual("modulate"),
        aFilterOpt.  IsEqual("nearest") ? Graphic3d_TOTF_NEAREST :
                                          aFilterOpt.IsEqual("bilinear") ? Graphic3d_TOTF_BILINEAR :
                                                                           Graphic3d_TOTF_TRILINEAR,
        (Standard_ShortReal)Draw::Atof(theArgVec[6]),
        (Standard_ShortReal)Draw::Atof(theArgVec[7]),
        (Standard_ShortReal)Draw::Atof(theArgVec[8]),
        (Standard_ShortReal)Draw::Atof(theArgVec[9]),
        (Standard_ShortReal)Draw::Atof(theArgVec[10])
        );
    }
    aView->SetTextureEnv(aTexEnv);
  }
  else // Disabling environment mapping
  {
    Handle(Graphic3d_TextureEnv) aTexture;
    aView->SetTextureEnv(aTexture); // Passing null handle to clear the texture data
  }

  aView->Redraw();
  return 0;
}

namespace
{
  typedef NCollection_DataMap<TCollection_AsciiString, Handle(Graphic3d_ClipPlane)> MapOfPlanes;

  //! Remove registered clipping plane from all views and objects.
  static void removePlane (MapOfPlanes& theRegPlanes,
                           const TCollection_AsciiString& theName)
  {
    Handle(Graphic3d_ClipPlane) aClipPlane;
    if (!theRegPlanes.Find (theName, aClipPlane))
    {
      Message::SendWarning ("Warning: no such plane");
      return;
    }

    theRegPlanes.UnBind (theName);
    for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIObjIt (GetMapOfAIS());
         anIObjIt.More(); anIObjIt.Next())
    {
      const Handle(AIS_InteractiveObject)& aPrs = anIObjIt.Key1();
      aPrs->RemoveClipPlane (aClipPlane);
    }

    for (NCollection_DoubleMap<TCollection_AsciiString, Handle(V3d_View)>::Iterator aViewIt(ViewerTest_myViews);
         aViewIt.More(); aViewIt.Next())
    {
      const Handle(V3d_View)& aView = aViewIt.Key2();
      aView->RemoveClipPlane(aClipPlane);
    }

    ViewerTest::RedrawAllViews();
  }
}

//===============================================================================================
//function : VClipPlane
//purpose  :
//===============================================================================================
static int VClipPlane (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  // use short-cut for created clip planes map of created (or "registered by name") clip planes
  static MapOfPlanes aRegPlanes;

  if (theArgsNb < 2)
  {
    for (MapOfPlanes::Iterator aPlaneIter (aRegPlanes); aPlaneIter.More(); aPlaneIter.Next())
    {
      theDi << aPlaneIter.Key() << " ";
    }
    return 0;
  }

  TCollection_AsciiString aCommand (theArgVec[1]);
  aCommand.LowerCase();
  const Handle(V3d_View)& anActiveView = ViewerTest::CurrentView();
  if (anActiveView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  // print maximum number of planes for current viewer
  if (aCommand == "-maxplanes"
   || aCommand == "maxplanes")
  {
    theDi << anActiveView->Viewer()->Driver()->InquirePlaneLimit()
          << " plane slots provided by driver.\n";
    return 0;
  }

  // create / delete plane instance
  if (aCommand == "-create"
   || aCommand == "create"
   || aCommand == "-delete"
   || aCommand == "delete"
   || aCommand == "-clone"
   || aCommand == "clone")
  {
    if (theArgsNb < 3)
    {
      Message::SendFail ("Syntax error: plane name is required");
      return 1;
    }

    Standard_Boolean toCreate = aCommand == "-create"
                             || aCommand == "create";
    Standard_Boolean toClone  = aCommand == "-clone"
                             || aCommand == "clone";
    Standard_Boolean toDelete = aCommand == "-delete"
                             || aCommand == "delete";
    TCollection_AsciiString aPlane (theArgVec[2]);

    if (toCreate)
    {
      if (aRegPlanes.IsBound (aPlane))
      {
        std::cout << "Warning: existing plane has been overridden.\n";
        toDelete = true;
      }
      else
      {
        aRegPlanes.Bind (aPlane, new Graphic3d_ClipPlane());
        return 0;
      }
    }
    else if (toClone) // toClone
    {
      if (!aRegPlanes.IsBound (aPlane))
      {
        Message::SendFail ("Error: no such plane");
        return 1;
      }
      else if (theArgsNb < 4)
      {
        Message::SendFail ("Syntax error: enter name for new plane");
        return 1;
      }

      TCollection_AsciiString aClone (theArgVec[3]);
      if (aRegPlanes.IsBound (aClone))
      {
        Message::SendFail ("Error: plane name is in use");
        return 1;
      }

      const Handle(Graphic3d_ClipPlane)& aClipPlane = aRegPlanes.Find (aPlane);

      aRegPlanes.Bind (aClone, aClipPlane->Clone());
      return 0;
    }

    if (toDelete)
    {
      if (aPlane == "ALL"
       || aPlane == "all"
       || aPlane == "*")
      {
        for (MapOfPlanes::Iterator aPlaneIter (aRegPlanes); aPlaneIter.More();)
        {
          aPlane = aPlaneIter.Key();
          removePlane (aRegPlanes, aPlane);
          aPlaneIter = MapOfPlanes::Iterator (aRegPlanes);
        }
      }
      else
      {
        removePlane (aRegPlanes, aPlane);
      }
    }

    if (toCreate)
    {
      aRegPlanes.Bind (aPlane, new Graphic3d_ClipPlane());
    }
    return 0;
  }

  // set / unset plane command
  if (aCommand == "set"
   || aCommand == "unset")
  {
    if (theArgsNb < 5)
    {
      Message::SendFail ("Syntax error: need more arguments");
      return 1;
    }

    // redirect to new syntax
    NCollection_Array1<const char*> anArgVec (1, theArgsNb - 1);
    anArgVec.SetValue (1, theArgVec[0]);
    anArgVec.SetValue (2, theArgVec[2]);
    anArgVec.SetValue (3, aCommand == "set" ? "-set" : "-unset");
    for (Standard_Integer anIt = 4; anIt < theArgsNb; ++anIt)
    {
      anArgVec.SetValue (anIt, theArgVec[anIt]);
    }

    return VClipPlane (theDi, anArgVec.Length(), &anArgVec.ChangeFirst());
  }

  // change plane command
  TCollection_AsciiString aPlaneName;
  Handle(Graphic3d_ClipPlane) aClipPlane;
  Standard_Integer anArgIter = 0;
  if (aCommand == "-change"
   || aCommand == "change")
  {
    // old syntax support
    if (theArgsNb < 3)
    {
      Message::SendFail ("Syntax error: need more arguments");
      return 1;
    }

    anArgIter  = 3;
    aPlaneName = theArgVec[2];
    if (!aRegPlanes.Find (aPlaneName, aClipPlane))
    {
      Message::SendFail() << "Error: no such plane '" << aPlaneName << "'";
      return 1;
    }
  }
  else if (aRegPlanes.Find (theArgVec[1], aClipPlane))
  {
    anArgIter  = 2;
    aPlaneName = theArgVec[1];
  }
  else
  {
    anArgIter  = 2;
    aPlaneName = theArgVec[1];
    aClipPlane = new Graphic3d_ClipPlane();
    aRegPlanes.Bind (aPlaneName, aClipPlane);
    theDi << "Created new plane " << aPlaneName << ".\n";
  }

  if (theArgsNb - anArgIter < 1)
  {
    Message::SendFail ("Syntax error: need more arguments");
    return 1;
  }

  for (; anArgIter < theArgsNb; ++anArgIter)
  {
    const char**     aChangeArgs   = theArgVec + anArgIter;
    Standard_Integer aNbChangeArgs = theArgsNb - anArgIter;
    TCollection_AsciiString aChangeArg (aChangeArgs[0]);
    aChangeArg.LowerCase();

    Standard_Boolean toEnable = Standard_True;
    if (Draw::ParseOnOff (aChangeArgs[0], toEnable))
    {
      aClipPlane->SetOn (toEnable);
    }
    else if (aChangeArg.StartsWith ("-equation")
          || aChangeArg.StartsWith ("equation"))
    {
      if (aNbChangeArgs < 5)
      {
        Message::SendFail ("Syntax error: need more arguments");
        return 1;
      }

      Standard_Integer aSubIndex = 1;
      Standard_Integer aPrefixLen = 8 + (aChangeArg.Value (1) == '-' ? 1 : 0);
      if (aPrefixLen < aChangeArg.Length())
      {
        TCollection_AsciiString aSubStr = aChangeArg.SubString (aPrefixLen + 1, aChangeArg.Length());
        if (!aSubStr.IsIntegerValue()
          || aSubStr.IntegerValue() <= 0)
        {
          Message::SendFail() << "Syntax error: unknown argument '" << aChangeArg << "'";
          return 1;
        }
        aSubIndex = aSubStr.IntegerValue();
      }

      Standard_Real aCoeffA = Draw::Atof (aChangeArgs[1]);
      Standard_Real aCoeffB = Draw::Atof (aChangeArgs[2]);
      Standard_Real aCoeffC = Draw::Atof (aChangeArgs[3]);
      Standard_Real aCoeffD = Draw::Atof (aChangeArgs[4]);
      Handle(Graphic3d_ClipPlane) aSubPln = aClipPlane;
      for (Standard_Integer aSubPlaneIter = 1; aSubPlaneIter < aSubIndex; ++aSubPlaneIter)
      {
        if (aSubPln->ChainNextPlane().IsNull())
        {
          aSubPln->SetChainNextPlane (new Graphic3d_ClipPlane (*aSubPln));
        }
        aSubPln = aSubPln->ChainNextPlane();
      }
      aSubPln->SetChainNextPlane (Handle(Graphic3d_ClipPlane)());
      aSubPln->SetEquation (gp_Pln (aCoeffA, aCoeffB, aCoeffC, aCoeffD));
      anArgIter += 4;
    }
    else if ((aChangeArg == "-boxinterior"
           || aChangeArg == "-boxint"
           || aChangeArg == "-box")
            && aNbChangeArgs >= 7)
    {
      Graphic3d_BndBox3d aBndBox;
      aBndBox.Add (Graphic3d_Vec3d (Draw::Atof (aChangeArgs[1]), Draw::Atof (aChangeArgs[2]), Draw::Atof (aChangeArgs[3])));
      aBndBox.Add (Graphic3d_Vec3d (Draw::Atof (aChangeArgs[4]), Draw::Atof (aChangeArgs[5]), Draw::Atof (aChangeArgs[6])));
      anArgIter += 6;

      Standard_Integer aNbSubPlanes = 6;
      const Graphic3d_Vec3d aDirArray[6] =
      {
        Graphic3d_Vec3d (-1, 0, 0),
        Graphic3d_Vec3d ( 1, 0, 0),
        Graphic3d_Vec3d ( 0,-1, 0),
        Graphic3d_Vec3d ( 0, 1, 0),
        Graphic3d_Vec3d ( 0, 0,-1),
        Graphic3d_Vec3d ( 0, 0, 1),
      };
      Handle(Graphic3d_ClipPlane) aSubPln = aClipPlane;
      for (Standard_Integer aSubPlaneIter = 0; aSubPlaneIter < aNbSubPlanes; ++aSubPlaneIter)
      {
        const Graphic3d_Vec3d& aDir = aDirArray[aSubPlaneIter];
        const Standard_Real aW = -aDir.Dot ((aSubPlaneIter % 2 == 1) ? aBndBox.CornerMax() : aBndBox.CornerMin());
        aSubPln->SetEquation (gp_Pln (aDir.x(), aDir.y(), aDir.z(), aW));
        if (aSubPlaneIter + 1 == aNbSubPlanes)
        {
          aSubPln->SetChainNextPlane (Handle(Graphic3d_ClipPlane)());
        }
        else
        {
          aSubPln->SetChainNextPlane (new Graphic3d_ClipPlane (*aSubPln));
        }
        aSubPln = aSubPln->ChainNextPlane();
      }
    }
    else if (aChangeArg == "-capping"
          || aChangeArg == "capping")
    {
      if (aNbChangeArgs < 2)
      {
        Message::SendFail ("Syntax error: need more arguments");
        return 1;
      }

      if (Draw::ParseOnOff (aChangeArgs[1], toEnable))
      {
        aClipPlane->SetCapping (toEnable);
        anArgIter += 1;
      }
      else
      {
        // just skip otherwise (old syntax)
      }
    }
    else if (aChangeArg == "-useobjectmaterial"
          || aChangeArg == "-useobjectmat"
          || aChangeArg == "-useobjmat"
          || aChangeArg == "-useobjmaterial")
    {
      if (aNbChangeArgs < 2)
      {
        Message::SendFail ("Syntax error: need more arguments");
        return 1;
      }

      if (Draw::ParseOnOff (aChangeArgs[1], toEnable))
      {
        aClipPlane->SetUseObjectMaterial (toEnable == Standard_True);
        anArgIter += 1;
      }
    }
    else if (aChangeArg == "-useobjecttexture"
          || aChangeArg == "-useobjecttex"
          || aChangeArg == "-useobjtexture"
          || aChangeArg == "-useobjtex")
    {
      if (aNbChangeArgs < 2)
      {
        Message::SendFail ("Syntax error: need more arguments");
        return 1;
      }

      if (Draw::ParseOnOff (aChangeArgs[1], toEnable))
      {
        aClipPlane->SetUseObjectTexture (toEnable == Standard_True);
        anArgIter += 1;
      }
    }
    else if (aChangeArg == "-useobjectshader"
          || aChangeArg == "-useobjshader")
    {
      if (aNbChangeArgs < 2)
      {
        Message::SendFail ("Syntax error: need more arguments");
        return 1;
      }

      if (Draw::ParseOnOff (aChangeArgs[1], toEnable))
      {
        aClipPlane->SetUseObjectShader (toEnable == Standard_True);
        anArgIter += 1;
      }
    }
    else if (aChangeArg == "-color"
          || aChangeArg == "color")
    {
      Quantity_Color aColor;
      Standard_Integer aNbParsed = Draw::ParseColor (aNbChangeArgs - 1,
                                                     aChangeArgs + 1,
                                                     aColor);
      if (aNbParsed == 0)
      {
        Message::SendFail ("Syntax error: need more arguments");
        return 1;
      }
      aClipPlane->SetCappingColor (aColor);
      anArgIter += aNbParsed;
    }
    else if (aNbChangeArgs >= 1
          && (aChangeArg == "-material"
           || aChangeArg == "material"))
    {
      ++anArgIter;
      Graphic3d_NameOfMaterial aMatName;
      if (!Graphic3d_MaterialAspect::MaterialFromName (aChangeArgs[1], aMatName))
      {
        Message::SendFail() << "Syntax error: unknown material '" << aChangeArgs[1] << "'";
        return 1;
      }
      aClipPlane->SetCappingMaterial (aMatName);
    }
    else if ((aChangeArg == "-transparency"
           || aChangeArg == "-transp")
          && aNbChangeArgs >= 2)
    {
      TCollection_AsciiString aValStr (aChangeArgs[1]);
      Handle(Graphic3d_AspectFillArea3d) anAspect = aClipPlane->CappingAspect();
      if (aValStr.IsRealValue())
      {
        Graphic3d_MaterialAspect aMat = aClipPlane->CappingMaterial();
        aMat.SetTransparency ((float )aValStr.RealValue());
        anAspect->SetAlphaMode (Graphic3d_AlphaMode_BlendAuto);
        aClipPlane->SetCappingMaterial (aMat);
      }
      else
      {
        aValStr.LowerCase();
        Graphic3d_AlphaMode aMode = Graphic3d_AlphaMode_BlendAuto;
        if (aValStr == "opaque")
        {
          aMode = Graphic3d_AlphaMode_Opaque;
        }
        else if (aValStr == "mask")
        {
          aMode = Graphic3d_AlphaMode_Mask;
        }
        else if (aValStr == "blend")
        {
          aMode = Graphic3d_AlphaMode_Blend;
        }
        else if (aValStr == "blendauto")
        {
          aMode = Graphic3d_AlphaMode_BlendAuto;
        }
        else
        {
          Message::SendFail() << "Syntax error at '" << aValStr << "'";
          return 1;
        }
        anAspect->SetAlphaMode (aMode);
        aClipPlane->SetCappingAspect (anAspect);
      }
      anArgIter += 1;
    }
    else if (aChangeArg == "-texname"
          || aChangeArg == "texname")
    {
      if (aNbChangeArgs < 2)
      {
        Message::SendFail ("Syntax error: need more arguments");
        return 1;
      }

      TCollection_AsciiString aTextureName (aChangeArgs[1]);
      Handle(Graphic3d_Texture2Dmanual) aTexture = new Graphic3d_Texture2Dmanual(aTextureName);
      if (!aTexture->IsDone())
      {
        aClipPlane->SetCappingTexture (NULL);
      }
      else
      {
        aTexture->EnableModulate();
        aTexture->EnableRepeat();
        aClipPlane->SetCappingTexture (aTexture);
      }
      anArgIter += 1;
    }
    else if (aChangeArg == "-texscale"
          || aChangeArg == "texscale")
    {
      if (aClipPlane->CappingTexture().IsNull())
      {
        Message::SendFail ("Error: no texture is set");
        return 1;
      }

      if (aNbChangeArgs < 3)
      {
        Message::SendFail ("Syntax error: need more arguments");
        return 1;
      }

      Standard_ShortReal aSx = (Standard_ShortReal)Draw::Atof (aChangeArgs[1]);
      Standard_ShortReal aSy = (Standard_ShortReal)Draw::Atof (aChangeArgs[2]);
      aClipPlane->CappingTexture()->GetParams()->SetScale (Graphic3d_Vec2 (aSx, aSy));
      anArgIter += 2;
    }
    else if (aChangeArg == "-texorigin"
          || aChangeArg == "texorigin") // texture origin
    {
      if (aClipPlane->CappingTexture().IsNull())
      {
        Message::SendFail ("Error: no texture is set");
        return 1;
      }

      if (aNbChangeArgs < 3)
      {
        Message::SendFail ("Syntax error: need more arguments");
        return 1;
      }

      Standard_ShortReal aTx = (Standard_ShortReal)Draw::Atof (aChangeArgs[1]);
      Standard_ShortReal aTy = (Standard_ShortReal)Draw::Atof (aChangeArgs[2]);

      aClipPlane->CappingTexture()->GetParams()->SetTranslation (Graphic3d_Vec2 (aTx, aTy));
      anArgIter += 2;
    }
    else if (aChangeArg == "-texrotate"
          || aChangeArg == "texrotate") // texture rotation
    {
      if (aClipPlane->CappingTexture().IsNull())
      {
        Message::SendFail ("Error: no texture is set");
        return 1;
      }

      if (aNbChangeArgs < 2)
      {
        Message::SendFail ("Syntax error: need more arguments");
        return 1;
      }

      Standard_ShortReal aRot = (Standard_ShortReal)Draw::Atof (aChangeArgs[1]);
      aClipPlane->CappingTexture()->GetParams()->SetRotation (aRot);
      anArgIter += 1;
    }
    else if (aChangeArg == "-hatch"
          || aChangeArg == "hatch")
    {
      if (aNbChangeArgs < 2)
      {
        Message::SendFail ("Syntax error: need more arguments");
        return 1;
      }

      TCollection_AsciiString aHatchStr (aChangeArgs[1]);
      aHatchStr.LowerCase();
      if (aHatchStr == "on")
      {
        aClipPlane->SetCappingHatchOn();
      }
      else if (aHatchStr == "off")
      {
        aClipPlane->SetCappingHatchOff();
      }
      else
      {
        aClipPlane->SetCappingHatch ((Aspect_HatchStyle)Draw::Atoi (aChangeArgs[1]));
      }
      anArgIter += 1;
    }
    else if (aChangeArg == "-delete"
          || aChangeArg == "delete")
    {
      removePlane (aRegPlanes, aPlaneName);
      return 0;
    }
    else if (aChangeArg == "-set"
          || aChangeArg == "-unset"
          || aChangeArg == "-setoverrideglobal")
    {
      // set / unset plane command
      const Standard_Boolean toSet            = aChangeArg.StartsWith ("-set");
      const Standard_Boolean toOverrideGlobal = aChangeArg == "-setoverrideglobal";
      Standard_Integer anIt = 1;
      for (; anIt < aNbChangeArgs; ++anIt)
      {
        TCollection_AsciiString anEntityName (aChangeArgs[anIt]);
        if (anEntityName.IsEmpty()
         || anEntityName.Value (1) == '-')
        {
          break;
        }
        else if (!toOverrideGlobal
               && ViewerTest_myViews.IsBound1 (anEntityName))
        {
          Handle(V3d_View) aView = ViewerTest_myViews.Find1 (anEntityName);
          if (toSet)
          {
            aView->AddClipPlane (aClipPlane);
          }
          else
          {
            aView->RemoveClipPlane (aClipPlane);
          }
          continue;
        }
        else if (GetMapOfAIS().IsBound2 (anEntityName))
        {
          Handle(AIS_InteractiveObject) aIObj = GetMapOfAIS().Find2 (anEntityName);
          if (toSet)
          {
            aIObj->AddClipPlane (aClipPlane);
          }
          else
          {
            aIObj->RemoveClipPlane (aClipPlane);
          }
          if (!aIObj->ClipPlanes().IsNull())
          {
            aIObj->ClipPlanes()->SetOverrideGlobal (toOverrideGlobal);
          }
        }
        else
        {
          Message::SendFail() << "Error: object/view '" << anEntityName << "' is not found";
          return 1;
        }
      }

      if (anIt == 1)
      {
        // apply to active view
        if (toSet)
        {
          anActiveView->AddClipPlane (aClipPlane);
        }
        else
        {
          anActiveView->RemoveClipPlane (aClipPlane);
        }
      }
      else
      {
        anArgIter = anArgIter + anIt - 1;
      }
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << aChangeArg << "'";
      return 1;
    }
  }

  ViewerTest::RedrawAllViews();
  return 0;
}

//===============================================================================================
//function : VZRange
//purpose  :
//===============================================================================================
static int VZRange (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  const Handle(V3d_View)& aCurrentView = ViewerTest::CurrentView();

  if (aCurrentView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Handle(Graphic3d_Camera) aCamera = aCurrentView->Camera();

  if (theArgsNb < 2)
  {
    theDi << "ZNear: " << aCamera->ZNear() << "\n";
    theDi << "ZFar: " << aCamera->ZFar() << "\n";
    return 0;
  }

  if (theArgsNb == 3)
  {
    Standard_Real aNewZNear = Draw::Atof (theArgVec[1]);
    Standard_Real aNewZFar  = Draw::Atof (theArgVec[2]);

    if (aNewZNear >= aNewZFar)
    {
      Message::SendFail ("Syntax error: invalid arguments: znear should be less than zfar");
      return 1;
    }

    if (!aCamera->IsOrthographic() && (aNewZNear <= 0.0 || aNewZFar <= 0.0))
    {
      Message::SendFail ("Syntax error: invalid arguments: znear, zfar should be positive for perspective camera");
      return 1;
    }

    aCamera->SetZRange (aNewZNear, aNewZFar);
  }
  else
  {
    Message::SendFail ("Syntax error: wrong command arguments");
    return 1;
  }

  aCurrentView->Redraw();

  return 0;
}

//===============================================================================================
//function : VAutoZFit
//purpose  :
//===============================================================================================
static int VAutoZFit (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  const Handle(V3d_View)& aCurrentView = ViewerTest::CurrentView();

  if (aCurrentView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Standard_Real aScale = aCurrentView->AutoZFitScaleFactor();

  if (theArgsNb > 3)
  {
    Message::SendFail ("Syntax error: wrong command arguments");
    return 1;
  }

  if (theArgsNb < 2)
  {
    theDi << "Auto z-fit mode: \n"
          << "On: " << (aCurrentView->AutoZFitMode() ? "enabled" : "disabled") << "\n"
          << "Scale: " << aScale << "\n";
    return 0;
  }

  Standard_Boolean isOn = Draw::Atoi (theArgVec[1]) == 1;

  if (theArgsNb >= 3)
  {
    aScale = Draw::Atoi (theArgVec[2]);
  }

  aCurrentView->SetAutoZFitMode (isOn, aScale);
  aCurrentView->Redraw();
  return 0;
}

//! Auxiliary function to print projection type
inline const char* projTypeName (Graphic3d_Camera::Projection theProjType)
{
  switch (theProjType)
  {
    case Graphic3d_Camera::Projection_Orthographic: return "orthographic";
    case Graphic3d_Camera::Projection_Perspective:  return "perspective";
    case Graphic3d_Camera::Projection_Stereo:       return "stereoscopic";
    case Graphic3d_Camera::Projection_MonoLeftEye:  return "monoLeftEye";
    case Graphic3d_Camera::Projection_MonoRightEye: return "monoRightEye";
  }
  return "UNKNOWN";
}

//===============================================================================================
//function : VCamera
//purpose  :
//===============================================================================================
static int VCamera (Draw_Interpretor& theDI,
                    Standard_Integer  theArgsNb,
                    const char**      theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Handle(Graphic3d_Camera) aCamera = aView->Camera();
  if (theArgsNb < 2)
  {
    theDI << "ProjType:   " << projTypeName (aCamera->ProjectionType()) << "\n";
    theDI << "FOVy:       " << aCamera->FOVy() << "\n";
    theDI << "FOVx:       " << aCamera->FOVx() << "\n";
    theDI << "FOV2d:      " << aCamera->FOV2d() << "\n";
    theDI << "Distance:   " << aCamera->Distance() << "\n";
    theDI << "IOD:        " << aCamera->IOD() << "\n";
    theDI << "IODType:    " << (aCamera->GetIODType() == Graphic3d_Camera::IODType_Absolute   ? "absolute" : "relative") << "\n";
    theDI << "ZFocus:     " << aCamera->ZFocus() << "\n";
    theDI << "ZFocusType: " << (aCamera->ZFocusType() == Graphic3d_Camera::FocusType_Absolute ? "absolute" : "relative") << "\n";
    return 0;
  }

  TCollection_AsciiString aPrsName;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    Standard_CString        anArg = theArgVec[anArgIter];
    TCollection_AsciiString anArgCase (anArg);
    anArgCase.LowerCase();
    if (anArgCase == "-proj"
     || anArgCase == "-projection"
     || anArgCase == "-projtype"
     || anArgCase == "-projectiontype")
    {
      theDI << projTypeName (aCamera->ProjectionType()) << " ";
    }
    else if (anArgCase == "-ortho"
          || anArgCase == "-orthographic")
    {
      aCamera->SetProjectionType (Graphic3d_Camera::Projection_Orthographic);
    }
    else if (anArgCase == "-persp"
          || anArgCase == "-perspective"
          || anArgCase == "-perspmono"
          || anArgCase == "-perspectivemono"
          || anArgCase == "-mono")
    {
      aCamera->SetProjectionType (Graphic3d_Camera::Projection_Perspective);
    }
    else if (anArgCase == "-stereo"
          || anArgCase == "-stereoscopic"
          || anArgCase == "-perspstereo"
          || anArgCase == "-perspectivestereo")
    {
      aCamera->SetProjectionType (Graphic3d_Camera::Projection_Stereo);
    }
    else if (anArgCase == "-left"
          || anArgCase == "-lefteye"
          || anArgCase == "-monoleft"
          || anArgCase == "-monolefteye"
          || anArgCase == "-perpsleft"
          || anArgCase == "-perpslefteye")
    {
      aCamera->SetProjectionType (Graphic3d_Camera::Projection_MonoLeftEye);
    }
    else if (anArgCase == "-right"
          || anArgCase == "-righteye"
          || anArgCase == "-monoright"
          || anArgCase == "-monorighteye"
          || anArgCase == "-perpsright")
    {
      aCamera->SetProjectionType (Graphic3d_Camera::Projection_MonoRightEye);
    }
    else if (anArgCase == "-dist"
          || anArgCase == "-distance")
    {
      Standard_CString anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : NULL;
      if (anArgValue != NULL
      && *anArgValue != '-')
      {
        ++anArgIter;
        aCamera->SetDistance (Draw::Atof (anArgValue));
        continue;
      }
      theDI << aCamera->Distance() << " ";
    }
    else if (anArgCase == "-iod")
    {
      Standard_CString anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : NULL;
      if (anArgValue != NULL
      && *anArgValue != '-')
      {
        ++anArgIter;
        aCamera->SetIOD (aCamera->GetIODType(), Draw::Atof (anArgValue));
        continue;
      }
      theDI << aCamera->IOD() << " ";
    }
    else if (anArgCase == "-iodtype")
    {
      Standard_CString        anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : "";
      TCollection_AsciiString anValueCase (anArgValue);
      anValueCase.LowerCase();
      if (anValueCase == "abs"
       || anValueCase == "absolute")
      {
        ++anArgIter;
        aCamera->SetIOD (Graphic3d_Camera::IODType_Absolute, aCamera->IOD());
        continue;
      }
      else if (anValueCase == "rel"
            || anValueCase == "relative")
      {
        ++anArgIter;
        aCamera->SetIOD (Graphic3d_Camera::IODType_Relative, aCamera->IOD());
        continue;
      }
      else if (*anArgValue != '-')
      {
        Message::SendFail() << "Error: unknown IOD type '" << anArgValue << "'";
        return 1;
      }
      switch (aCamera->GetIODType())
      {
        case Graphic3d_Camera::IODType_Absolute: theDI << "absolute "; break;
        case Graphic3d_Camera::IODType_Relative: theDI << "relative "; break;
      }
    }
    else if (anArgCase == "-zfocus")
    {
      Standard_CString anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : NULL;
      if (anArgValue != NULL
      && *anArgValue != '-')
      {
        ++anArgIter;
        aCamera->SetZFocus (aCamera->ZFocusType(), Draw::Atof (anArgValue));
        continue;
      }
      theDI << aCamera->ZFocus() << " ";
    }
    else if (anArgCase == "-zfocustype")
    {
      Standard_CString        anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : "";
      TCollection_AsciiString anValueCase (anArgValue);
      anValueCase.LowerCase();
      if (anValueCase == "abs"
       || anValueCase == "absolute")
      {
        ++anArgIter;
        aCamera->SetZFocus (Graphic3d_Camera::FocusType_Absolute, aCamera->ZFocus());
        continue;
      }
      else if (anValueCase == "rel"
            || anValueCase == "relative")
      {
        ++anArgIter;
        aCamera->SetZFocus (Graphic3d_Camera::FocusType_Relative, aCamera->ZFocus());
        continue;
      }
      else if (*anArgValue != '-')
      {
        Message::SendFail() << "Error: unknown ZFocus type '" << anArgValue << "'";
        return 1;
      }
      switch (aCamera->ZFocusType())
      {
        case Graphic3d_Camera::FocusType_Absolute: theDI << "absolute "; break;
        case Graphic3d_Camera::FocusType_Relative: theDI << "relative "; break;
      }
    }
    else if (anArgCase == "-lockzup"
          || anArgCase == "-turntable")
    {
      bool toLockUp = true;
      if (++anArgIter < theArgsNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toLockUp))
      {
        --anArgIter;
      }
      ViewerTest::CurrentEventManager()->SetLockOrbitZUp (toLockUp);
    }
    else if (anArgCase == "-fov"
          || anArgCase == "-fovy"
          || anArgCase == "-fovx"
          || anArgCase == "-fov2d")
    {
      Standard_CString anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : NULL;
      if (anArgValue != NULL
      && *anArgValue != '-')
      {
        ++anArgIter;
        if (anArgCase == "-fov2d")
        {
          aCamera->SetFOV2d (Draw::Atof (anArgValue));
        }
        else if (anArgCase == "-fovx")
        {
          aCamera->SetFOVy (Draw::Atof (anArgValue) / aCamera->Aspect());///
        }
        else
        {
          aCamera->SetFOVy (Draw::Atof (anArgValue));
        }
        continue;
      }
      if (anArgCase == "-fov2d")
      {
        theDI << aCamera->FOV2d() << " ";
      }
      else if (anArgCase == "-fovx")
      {
        theDI << aCamera->FOVx() << " ";
      }
      else
      {
        theDI << aCamera->FOVy() << " ";
      }
    }
    else if (anArgIter + 1 < theArgsNb
          && anArgCase == "-xrpose")
    {
      TCollection_AsciiString anXRArg (theArgVec[++anArgIter]);
      anXRArg.LowerCase();
      if (anXRArg == "base")
      {
        aCamera = aView->View()->BaseXRCamera();
      }
      else if (anXRArg == "head")
      {
        aCamera = aView->View()->PosedXRCamera();
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown XR pose '" << anXRArg << "'";
        return 1;
      }
      if (aCamera.IsNull())
      {
        Message::SendFail() << "Error: undefined XR pose";
        return 0;
      }
      if (aView->AutoZFitMode())
      {
        const Bnd_Box aMinMaxBox  = aView->View()->MinMaxValues (false);
        const Bnd_Box aGraphicBox = aView->View()->MinMaxValues (true);
        aCamera->ZFitAll (aView->AutoZFitScaleFactor(), aMinMaxBox, aGraphicBox);
      }
    }
    else if (aPrsName.IsEmpty()
         && !anArgCase.StartsWith ("-"))
    {
      aPrsName = anArg;
    }
    else
    {
      Message::SendFail() << "Error: unknown argument '" << anArg << "'";
      return 1;
    }
  }

  if (aPrsName.IsEmpty()
   || theArgsNb > 2)
  {
    aView->Redraw();
  }

  if (!aPrsName.IsEmpty())
  {
    Handle(AIS_CameraFrustum) aCameraFrustum;
    if (GetMapOfAIS().IsBound2 (aPrsName))
    {
      // find existing object
      aCameraFrustum = Handle(AIS_CameraFrustum)::DownCast (GetMapOfAIS().Find2 (theArgVec[1]));
      if (aCameraFrustum.IsNull())
      {
        Message::SendFail() << "Error: object '" << aPrsName << "'is already defined and is not a camera frustum";
        return 1;
      }
    }

    if (aCameraFrustum.IsNull())
    {
      aCameraFrustum = new AIS_CameraFrustum();
    }
    else
    {
      // not include displayed object of old camera frustum in the new one.
      ViewerTest::GetAISContext()->Erase (aCameraFrustum, false);
      aView->ZFitAll();
    }
    aCameraFrustum->SetCameraFrustum (aCamera);

    ViewerTest::Display (aPrsName, aCameraFrustum);
  }

  return 0;
}

//! Parse stereo output mode
inline Standard_Boolean parseStereoMode (Standard_CString      theArg,
                                         Graphic3d_StereoMode& theMode)
{
  TCollection_AsciiString aFlag (theArg);
  aFlag.LowerCase();
  if (aFlag == "quadbuffer")
  {
    theMode = Graphic3d_StereoMode_QuadBuffer;
  }
  else if (aFlag == "anaglyph")
  {
    theMode = Graphic3d_StereoMode_Anaglyph;
  }
  else if (aFlag == "row"
        || aFlag == "rowinterlaced")
  {
    theMode = Graphic3d_StereoMode_RowInterlaced;
  }
  else if (aFlag == "col"
        || aFlag == "colinterlaced"
        || aFlag == "columninterlaced")
  {
    theMode = Graphic3d_StereoMode_ColumnInterlaced;
  }
  else if (aFlag == "chess"
        || aFlag == "chessboard")
  {
    theMode = Graphic3d_StereoMode_ChessBoard;
  }
  else if (aFlag == "sbs"
        || aFlag == "sidebyside")
  {
    theMode = Graphic3d_StereoMode_SideBySide;
  }
  else if (aFlag == "ou"
        || aFlag == "overunder")
  {
    theMode = Graphic3d_StereoMode_OverUnder;
  }
  else if (aFlag == "pageflip"
        || aFlag == "softpageflip")
  {
    theMode = Graphic3d_StereoMode_SoftPageFlip;
  }
  else if (aFlag == "openvr"
        || aFlag == "vr")
  {
    theMode = Graphic3d_StereoMode_OpenVR;
  }
  else
  {
    return Standard_False;
  }
  return Standard_True;
}

//! Parse anaglyph filter
inline Standard_Boolean parseAnaglyphFilter (Standard_CString                     theArg,
                                             Graphic3d_RenderingParams::Anaglyph& theFilter)
{
  TCollection_AsciiString aFlag (theArg);
  aFlag.LowerCase();
  if (aFlag == "redcyansimple")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_RedCyan_Simple;
  }
  else if (aFlag == "redcyan"
        || aFlag == "redcyanoptimized")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_RedCyan_Optimized;
  }
  else if (aFlag == "yellowbluesimple")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_YellowBlue_Simple;
  }
  else if (aFlag == "yellowblue"
        || aFlag == "yellowblueoptimized")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_YellowBlue_Optimized;
  }
  else if (aFlag == "greenmagenta"
        || aFlag == "greenmagentasimple")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_GreenMagenta_Simple;
  }
  else
  {
    return Standard_False;
  }
  return Standard_True;
}

//==============================================================================
//function : VStereo
//purpose  :
//==============================================================================

static int VStereo (Draw_Interpretor& theDI,
                    Standard_Integer  theArgNb,
                    const char**      theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (theArgNb < 2)
  {
    if (aView.IsNull())
    {
      Message::SendFail ("Error: no active viewer");
      return 0;
    }

    Standard_Boolean isActive = ViewerTest_myDefaultCaps.contextStereo;
    theDI << "Stereo " << (isActive ? "ON" : "OFF") << "\n";
    if (isActive)
    {
      TCollection_AsciiString aMode;
      switch (aView->RenderingParams().StereoMode)
      {
        case Graphic3d_StereoMode_QuadBuffer       : aMode = "quadBuffer";       break;
        case Graphic3d_StereoMode_RowInterlaced    : aMode = "rowInterlaced";    break;
        case Graphic3d_StereoMode_ColumnInterlaced : aMode = "columnInterlaced"; break;
        case Graphic3d_StereoMode_ChessBoard       : aMode = "chessBoard";       break;
        case Graphic3d_StereoMode_SideBySide       : aMode = "sideBySide";       break;
        case Graphic3d_StereoMode_OverUnder        : aMode = "overUnder";        break;
        case Graphic3d_StereoMode_SoftPageFlip     : aMode = "softpageflip";     break;
        case Graphic3d_StereoMode_OpenVR           : aMode = "openVR";           break;
        case Graphic3d_StereoMode_Anaglyph  :
          aMode = "anaglyph";
          switch (aView->RenderingParams().AnaglyphFilter)
          {
            case Graphic3d_RenderingParams::Anaglyph_RedCyan_Simple      : aMode.AssignCat (" (redCyanSimple)");      break;
            case Graphic3d_RenderingParams::Anaglyph_RedCyan_Optimized   : aMode.AssignCat (" (redCyan)");            break;
            case Graphic3d_RenderingParams::Anaglyph_YellowBlue_Simple   : aMode.AssignCat (" (yellowBlueSimple)");   break;
            case Graphic3d_RenderingParams::Anaglyph_YellowBlue_Optimized: aMode.AssignCat (" (yellowBlue)");         break;
            case Graphic3d_RenderingParams::Anaglyph_GreenMagenta_Simple : aMode.AssignCat (" (greenMagentaSimple)"); break;
            default: break;
          }
        default: break;
      }
      theDI << "Mode " << aMode << "\n";
    }
    return 0;
  }

  Handle(Graphic3d_Camera) aCamera;
  Graphic3d_RenderingParams*   aParams   = NULL;
  Graphic3d_StereoMode         aMode     = Graphic3d_StereoMode_QuadBuffer;
  if (!aView.IsNull())
  {
    aParams   = &aView->ChangeRenderingParams();
    aMode     = aParams->StereoMode;
    aCamera   = aView->Camera();
  }

  ViewerTest_AutoUpdater anUpdateTool (ViewerTest::GetAISContext(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg = theArgVec[anArgIter];
    TCollection_AsciiString aFlag (anArg);
    aFlag.LowerCase();
    if (anUpdateTool.parseRedrawMode (aFlag))
    {
      continue;
    }
    else if (aFlag == "0"
          || aFlag == "off")
    {
      if (++anArgIter < theArgNb)
      {
        Message::SendFail ("Error: wrong number of arguments");
        return 1;
      }

      if (!aCamera.IsNull()
       &&  aCamera->ProjectionType() == Graphic3d_Camera::Projection_Stereo)
      {
        aCamera->SetProjectionType (Graphic3d_Camera::Projection_Perspective);
      }
      ViewerTest_myDefaultCaps.contextStereo = Standard_False;
      return 0;
    }
    else if (aFlag == "1"
          || aFlag == "on")
    {
      if (++anArgIter < theArgNb)
      {
        Message::SendFail ("Error: wrong number of arguments");
        return 1;
      }

      if (!aCamera.IsNull())
      {
        aCamera->SetProjectionType (Graphic3d_Camera::Projection_Stereo);
      }
      ViewerTest_myDefaultCaps.contextStereo = Standard_True;
      if (aParams->StereoMode != Graphic3d_StereoMode_OpenVR)
      {
        return 0;
      }
    }
    else if (aFlag == "-reverse"
          || aFlag == "-reversed"
          || aFlag == "-swap")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams->ToReverseStereo = toEnable;
    }
    else if (aFlag == "-noreverse"
          || aFlag == "-noswap")
    {
      Standard_Boolean toDisable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toDisable))
      {
        --anArgIter;
      }
      aParams->ToReverseStereo = !toDisable;
    }
    else if (aFlag == "-mode"
          || aFlag == "-stereomode")
    {
      if (++anArgIter >= theArgNb
      || !parseStereoMode (theArgVec[anArgIter], aMode))
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }

      if (aMode == Graphic3d_StereoMode_QuadBuffer)
      {
        ViewerTest_myDefaultCaps.contextStereo = Standard_True;
      }
    }
    else if (aFlag == "-anaglyph"
          || aFlag == "-anaglyphfilter")
    {
      Graphic3d_RenderingParams::Anaglyph aFilter = Graphic3d_RenderingParams::Anaglyph_RedCyan_Simple;
      if (++anArgIter >= theArgNb
      || !parseAnaglyphFilter (theArgVec[anArgIter], aFilter))
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }

      aMode = Graphic3d_StereoMode_Anaglyph;
      aParams->AnaglyphFilter = aFilter;
    }
    else if (parseStereoMode (anArg, aMode)) // short syntax
    {
      if (aMode == Graphic3d_StereoMode_QuadBuffer)
      {
        ViewerTest_myDefaultCaps.contextStereo = Standard_True;
      }
    }
    else if (anArgIter + 1 < theArgNb
          && aFlag == "-hmdfov2d")
    {
      aParams->HmdFov2d = (float )Draw::Atof (theArgVec[++anArgIter]);
      if (aParams->HmdFov2d < 10.0f
       || aParams->HmdFov2d > 180.0f)
      {
        Message::SendFail() << "Error: FOV is out of range";
        return 1;
      }
    }
    else if (aFlag == "-mirror"
          || aFlag == "-mirrorcomposer")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams->ToMirrorComposer = toEnable;
    }
    else if (anArgIter + 1 < theArgNb
          && (aFlag == "-unitfactor"
           || aFlag == "-unitscale"))
    {
      aView->View()->SetUnitFactor (Draw::Atof (theArgVec[++anArgIter]));
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  if (!aView.IsNull())
  {
    aParams->StereoMode = aMode;
    aCamera->SetProjectionType (Graphic3d_Camera::Projection_Stereo);
    if (aParams->StereoMode == Graphic3d_StereoMode_OpenVR)
    {
      // initiate implicit continuous rendering
      ViewerTest::CurrentEventManager()->FlushViewEvents (ViewerTest::GetAISContext(), aView, true);
    }
  }
  return 0;
}

//===============================================================================================
//function : VDefaults
//purpose  :
//===============================================================================================
static int VDefaults (Draw_Interpretor& theDi,
                      Standard_Integer  theArgsNb,
                      const char**      theArgVec)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Handle(Prs3d_Drawer) aDefParams = aCtx->DefaultDrawer();
  if (theArgsNb < 2)
  {
    if (aDefParams->TypeOfDeflection() == Aspect_TOD_RELATIVE)
    {
      theDi << "DeflType:           relative\n"
            << "DeviationCoeff:     " << aDefParams->DeviationCoefficient() << "\n";
    }
    else
    {
      theDi << "DeflType:           absolute\n"
            << "AbsoluteDeflection: " << aDefParams->MaximalChordialDeviation() << "\n";
    }
    theDi << "AngularDeflection:  " << (180.0 * aDefParams->DeviationAngle() / M_PI) << "\n";
    theDi << "AutoTriangulation:  " << (aDefParams->IsAutoTriangulation() ? "on" : "off") << "\n";
    return 0;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.UpperCase();
    if (anArg == "-ABSDEFL"
     || anArg == "-ABSOLUTEDEFLECTION"
     || anArg == "-DEFL"
     || anArg == "-DEFLECTION")
    {
      if (++anArgIter >= theArgsNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      aDefParams->SetTypeOfDeflection         (Aspect_TOD_ABSOLUTE);
      aDefParams->SetMaximalChordialDeviation (Draw::Atof (theArgVec[anArgIter]));
    }
    else if (anArg == "-RELDEFL"
          || anArg == "-RELATIVEDEFLECTION"
          || anArg == "-DEVCOEFF"
          || anArg == "-DEVIATIONCOEFF"
          || anArg == "-DEVIATIONCOEFFICIENT")
    {
      if (++anArgIter >= theArgsNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      aDefParams->SetTypeOfDeflection     (Aspect_TOD_RELATIVE);
      aDefParams->SetDeviationCoefficient (Draw::Atof (theArgVec[anArgIter]));
    }
    else if (anArg == "-ANGDEFL"
          || anArg == "-ANGULARDEFL"
          || anArg == "-ANGULARDEFLECTION")
    {
      if (++anArgIter >= theArgsNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      aDefParams->SetDeviationAngle (M_PI * Draw::Atof (theArgVec[anArgIter]) / 180.0);
    }
    else if (anArg == "-AUTOTR"
          || anArg == "-AUTOTRIANG"
          || anArg == "-AUTOTRIANGULATION")
    {
      ++anArgIter;
      bool toTurnOn = true;
      if (anArgIter >= theArgsNb
      || !Draw::ParseOnOff (theArgVec[anArgIter], toTurnOn))
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }
      aDefParams->SetAutoTriangulation (toTurnOn);
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << anArg << "'";
      return 1;
    }
  }

  return 0;
}

//! Auxiliary method
inline void addLight (const Handle(V3d_Light)& theLightNew,
                      const Graphic3d_ZLayerId theLayer,
                      const Standard_Boolean   theIsGlobal)
{
  if (theLightNew.IsNull())
  {
    return;
  }

  Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
  if (theLayer == Graphic3d_ZLayerId_UNKNOWN)
  {
    aViewer->AddLight (theLightNew);
    if (theIsGlobal)
    {
      aViewer->SetLightOn (theLightNew);
    }
    else
    {
      ViewerTest::CurrentView()->SetLightOn (theLightNew);
    }
  }
  else
  {
    Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (theLayer);
    if (aSettings.Lights().IsNull())
    {
      aSettings.SetLights (new Graphic3d_LightSet());
    }
    aSettings.Lights()->Add (theLightNew);
    aViewer->SetZLayerSettings (theLayer, aSettings);
  }
}

//! Auxiliary method
inline Standard_Integer getLightId (const TCollection_AsciiString& theArgNext)
{
  TCollection_AsciiString anArgNextCase (theArgNext);
  anArgNextCase.UpperCase();
  if (anArgNextCase.Length() > 5
   && anArgNextCase.SubString (1, 5).IsEqual ("LIGHT"))
  {
    return theArgNext.SubString (6, theArgNext.Length()).IntegerValue();
  }
  else
  {
    return theArgNext.IntegerValue();
  }
}

//===============================================================================================
//function : VLight
//purpose  :
//===============================================================================================
static int VLight (Draw_Interpretor& theDi,
                   Standard_Integer  theArgsNb,
                   const char**      theArgVec)
{
  Handle(V3d_View)   aView   = ViewerTest::CurrentView();
  Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
  if (aView.IsNull()
   || aViewer.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Standard_Real anXYZ[3]   = {};
  Standard_Real anAtten[2] = {};
  if (theArgsNb < 2)
  {
    // print lights info
    Standard_Integer aLightId = 0;
    for (V3d_ListOfLightIterator aLightIter (aView->ActiveLightIterator()); aLightIter.More(); aLightIter.Next(), ++aLightId)
    {
      Handle(V3d_Light) aLight = aLightIter.Value();
      const Quantity_Color aColor = aLight->Color();
      theDi << "Light #" << aLightId
            << (!aLight->Name().IsEmpty() ? (TCollection_AsciiString(" ") + aLight->Name()) : "")
            << " [" << aLight->GetId() << "]" << "\n";
      switch (aLight->Type())
      {
        case V3d_AMBIENT:
        {
          theDi << "  Type:       Ambient\n";
          theDi << "  Intensity:  " << aLight->Intensity() << "\n";
          break;
        }
        case V3d_DIRECTIONAL:
        {
          theDi << "  Type:       Directional\n";
          theDi << "  Intensity:  " << aLight->Intensity() << "\n";
          theDi << "  Headlight:  " << (aLight->Headlight() ? "TRUE" : "FALSE") << "\n";
          theDi << "  Smoothness: " << aLight->Smoothness() << "\n";
          aLight->Direction (anXYZ[0], anXYZ[1], anXYZ[2]);
          theDi << "  Direction:  " << anXYZ[0] << ", " << anXYZ[1] << ", " << anXYZ[2] << "\n";
          break;
        }
        case V3d_POSITIONAL:
        {
          theDi << "  Type:       Positional\n";
          theDi << "  Intensity:  " << aLight->Intensity() << "\n";
          theDi << "  Headlight:  " << (aLight->Headlight() ? "TRUE" : "FALSE") << "\n";
          theDi << "  Smoothness: " << aLight->Smoothness() << "\n";
          aLight->Position  (anXYZ[0], anXYZ[1], anXYZ[2]);
          theDi << "  Position:   " << anXYZ[0] << ", " << anXYZ[1] << ", " << anXYZ[2] << "\n";
          aLight->Attenuation (anAtten[0], anAtten[1]);
          theDi << "  Atten.:     " << anAtten[0] << " " << anAtten[1] << "\n";
          theDi << "  Range:      " << aLight->Range() << "\n";
          break;
        }
        case V3d_SPOT:
        {
          theDi << "  Type:       Spot\n";
          theDi << "  Intensity:  " << aLight->Intensity() << "\n";
          theDi << "  Headlight:  " << (aLight->Headlight() ? "TRUE" : "FALSE") << "\n";
          aLight->Position  (anXYZ[0], anXYZ[1], anXYZ[2]);
          theDi << "  Position:   " << anXYZ[0] << ", " << anXYZ[1] << ", " << anXYZ[2] << "\n";
          aLight->Direction (anXYZ[0], anXYZ[1], anXYZ[2]);
          theDi << "  Direction:  " << anXYZ[0] << ", " << anXYZ[1] << ", " << anXYZ[2] << "\n";
          aLight->Attenuation (anAtten[0], anAtten[1]);
          theDi << "  Atten.:     " << anAtten[0] << " " << anAtten[1] << "\n";
          theDi << "  Angle:      " << (aLight->Angle() * 180.0 / M_PI) << "\n";
          theDi << "  Exponent:   " << aLight->Concentration() << "\n";
          theDi << "  Range:      " << aLight->Range() << "\n";
          break;
        }
        default:
        {
          theDi << "  Type:       UNKNOWN\n";
          break;
        }
      }
      theDi << "  Color:      " << aColor.Red() << ", " << aColor.Green() << ", " << aColor.Blue() << " [" << Quantity_Color::StringName (aColor.Name()) << "]\n";
    }
  }

  Handle(V3d_Light) aLightNew;
  Handle(V3d_Light) aLightOld;
  Graphic3d_ZLayerId aLayer = Graphic3d_ZLayerId_UNKNOWN;
  Standard_Boolean  isGlobal = Standard_True;
  Standard_Boolean  toCreate = Standard_False;
  ViewerTest_AutoUpdater anUpdateTool (ViewerTest::GetAISContext(), aView);
  for (Standard_Integer anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    Handle(V3d_Light) aLightCurr = aLightNew.IsNull() ? aLightOld : aLightNew;

    TCollection_AsciiString aName, aValue;
    const TCollection_AsciiString anArg (theArgVec[anArgIt]);
    TCollection_AsciiString anArgCase (anArg);
    anArgCase.UpperCase();
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      continue;
    }

    if (anArgCase.IsEqual ("NEW")
     || anArgCase.IsEqual ("ADD")
     || anArgCase.IsEqual ("CREATE")
     || anArgCase.IsEqual ("-NEW")
     || anArgCase.IsEqual ("-ADD")
     || anArgCase.IsEqual ("-CREATE"))
    {
      toCreate = Standard_True;
    }
    else if (anArgCase.IsEqual ("-LAYER")
          || anArgCase.IsEqual ("-ZLAYER"))
    {
      if (++anArgIt >= theArgsNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aValStr (theArgVec[anArgIt]);
      aValStr.LowerCase();
      if (aValStr == "default"
       || aValStr == "def")
      {
        aLayer = Graphic3d_ZLayerId_Default;
      }
      else if (aValStr == "top")
      {
        aLayer = Graphic3d_ZLayerId_Top;
      }
      else if (aValStr == "topmost")
      {
        aLayer = Graphic3d_ZLayerId_Topmost;
      }
      else if (aValStr == "toposd"
            || aValStr == "osd")
      {
        aLayer = Graphic3d_ZLayerId_TopOSD;
      }
      else if (aValStr == "botosd"
            || aValStr == "bottom")
      {
        aLayer = Graphic3d_ZLayerId_BotOSD;
      }
      else if (aValStr.IsIntegerValue())
      {
        aLayer = Draw::Atoi (theArgVec[anArgIt]);
      }
      else
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
    }
    else if (anArgCase.IsEqual ("GLOB")
          || anArgCase.IsEqual ("GLOBAL")
          || anArgCase.IsEqual ("-GLOB")
          || anArgCase.IsEqual ("-GLOBAL"))
    {
      isGlobal = Standard_True;
    }
    else if (anArgCase.IsEqual ("LOC")
          || anArgCase.IsEqual ("LOCAL")
          || anArgCase.IsEqual ("-LOC")
          || anArgCase.IsEqual ("-LOCAL"))
    {
      isGlobal = Standard_False;
    }
    else if (anArgCase.IsEqual ("DEF")
          || anArgCase.IsEqual ("DEFAULTS")
          || anArgCase.IsEqual ("-DEF")
          || anArgCase.IsEqual ("-DEFAULTS"))
    {
      toCreate = Standard_False;
      aViewer->SetDefaultLights();
    }
    else if (anArgCase.IsEqual ("CLR")
          || anArgCase.IsEqual ("CLEAR")
          || anArgCase.IsEqual ("-CLR")
          || anArgCase.IsEqual ("-CLEAR"))
    {
      toCreate = Standard_False;

      TColStd_SequenceOfInteger aLayers;
      aViewer->GetAllZLayers (aLayers);
      for (TColStd_SequenceOfInteger::Iterator aLayeriter (aLayers); aLayeriter.More(); aLayeriter.Next())
      {
        if (aLayeriter.Value() == aLayer
         || aLayer == Graphic3d_ZLayerId_UNKNOWN)
        {
          Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (aLayeriter.Value());
          aSettings.SetLights (Handle(Graphic3d_LightSet)());
          aViewer->SetZLayerSettings (aLayeriter.Value(), aSettings);
          if (aLayer != Graphic3d_ZLayerId_UNKNOWN)
          {
            break;
          }
        }
      }

      if (aLayer == Graphic3d_ZLayerId_UNKNOWN)
      {
        for (V3d_ListOfLightIterator aLightIter (aView->ActiveLightIterator()); aLightIter.More();)
        {
          Handle(V3d_Light) aLight = aLightIter.Value();
          aViewer->DelLight (aLight);
          aLightIter = aView->ActiveLightIterator();
        }
      }
    }
    else if (anArgCase.IsEqual ("AMB")
          || anArgCase.IsEqual ("AMBIENT")
          || anArgCase.IsEqual ("AMBLIGHT"))
    {
      if (!toCreate)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      addLight (aLightNew, aLayer, isGlobal);
      toCreate  = Standard_False;
      aLightNew = new V3d_AmbientLight();
    }
    else if (anArgCase.IsEqual ("DIRECTIONAL")
          || anArgCase.IsEqual ("DIRLIGHT"))
    {
      if (!toCreate)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      addLight (aLightNew, aLayer, isGlobal);
      toCreate  = Standard_False;
      aLightNew = new V3d_DirectionalLight();
    }
    else if (anArgCase.IsEqual ("SPOT")
          || anArgCase.IsEqual ("SPOTLIGHT"))
    {
      if (!toCreate)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      addLight (aLightNew, aLayer, isGlobal);
      toCreate  = Standard_False;
      aLightNew = new V3d_SpotLight (gp_Pnt (0.0, 0.0, 0.0));
    }
    else if (anArgCase.IsEqual ("POSLIGHT")
          || anArgCase.IsEqual ("POSITIONAL"))
    {
      if (!toCreate)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      addLight (aLightNew, aLayer, isGlobal);
      toCreate  = Standard_False;
      aLightNew = new V3d_PositionalLight (gp_Pnt (0.0, 0.0, 0.0));
    }
    else if (anArgCase.IsEqual ("CHANGE")
          || anArgCase.IsEqual ("-CHANGE"))
    {
      if (++anArgIt >= theArgsNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      addLight (aLightNew, aLayer, isGlobal);
      aLightNew.Nullify();
      const Standard_Integer aLightId = getLightId (theArgVec[anArgIt]);
      Standard_Integer aLightIt = 0;
      for (V3d_ListOfLightIterator aLightIter (aView->ActiveLightIterator()); aLightIter.More(); aLightIter.Next(), ++aLightIt)
      {
        if (aLightIt == aLightId)
        {
          aLightOld = aLightIter.Value();
          break;
        }
      }

      if (aLightOld.IsNull())
      {
        Message::SendFail() << "Error: Light " << theArgVec[anArgIt] << " is undefined";
        return 1;
      }
    }
    else if (anArgCase.IsEqual ("DEL")
          || anArgCase.IsEqual ("DELETE")
          || anArgCase.IsEqual ("-DEL")
          || anArgCase.IsEqual ("-DELETE"))
    {
      Handle(V3d_Light) aLightDel;
      if (++anArgIt >= theArgsNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const TCollection_AsciiString anArgNext (theArgVec[anArgIt]);
      const Standard_Integer aLightDelId = getLightId (theArgVec[anArgIt]);
      Standard_Integer aLightIt = 0;
      for (V3d_ListOfLightIterator aLightIter (aView->ActiveLightIterator()); aLightIter.More(); aLightIter.Next(), ++aLightIt)
      {
        aLightDel = aLightIter.Value();
        if (aLightIt == aLightDelId)
        {
          break;
        }
      }
      if (aLightDel.IsNull())
      {
        continue;
      }

      TColStd_SequenceOfInteger aLayers;
      aViewer->GetAllZLayers (aLayers);
      for (TColStd_SequenceOfInteger::Iterator aLayeriter (aLayers); aLayeriter.More(); aLayeriter.Next())
      {
        if (aLayeriter.Value() == aLayer
         || aLayer == Graphic3d_ZLayerId_UNKNOWN)
        {
          Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (aLayeriter.Value());
          if (!aSettings.Lights().IsNull())
          {
            aSettings.Lights()->Remove (aLightDel);
            if (aSettings.Lights()->IsEmpty())
            {
              aSettings.SetLights (Handle(Graphic3d_LightSet)());
            }
          }
          aViewer->SetZLayerSettings (aLayeriter.Value(), aSettings);
          if (aLayer != Graphic3d_ZLayerId_UNKNOWN)
          {
            break;
          }
        }
      }

      if (aLayer == Graphic3d_ZLayerId_UNKNOWN)
      {
        aViewer->DelLight (aLightDel);
      }
    }
    else if (anArgCase.IsEqual ("COLOR")
          || anArgCase.IsEqual ("COLOUR")
          || anArgCase.IsEqual ("-COLOR")
          || anArgCase.IsEqual ("-COLOUR"))
    {
      Quantity_Color aColor;
      Standard_Integer aNbParsed = Draw::ParseColor (theArgsNb - anArgIt - 1,
                                                     theArgVec + anArgIt + 1,
                                                     aColor);
      anArgIt += aNbParsed;
      if (aNbParsed == 0
       || aLightCurr.IsNull())
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      aLightCurr->SetColor (aColor);
    }
    else if (anArgCase.IsEqual ("POS")
          || anArgCase.IsEqual ("POSITION")
          || anArgCase.IsEqual ("-POS")
          || anArgCase.IsEqual ("-POSITION"))
    {
      if ((anArgIt + 3) >= theArgsNb
       || aLightCurr.IsNull()
       || (aLightCurr->Type() != Graphic3d_TOLS_POSITIONAL
        && aLightCurr->Type() != Graphic3d_TOLS_SPOT))
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      anXYZ[0] = Atof (theArgVec[++anArgIt]);
      anXYZ[1] = Atof (theArgVec[++anArgIt]);
      anXYZ[2] = Atof (theArgVec[++anArgIt]);
      aLightCurr->SetPosition (anXYZ[0], anXYZ[1], anXYZ[2]);
    }
    else if (anArgCase.IsEqual ("DIR")
          || anArgCase.IsEqual ("DIRECTION")
          || anArgCase.IsEqual ("-DIR")
          || anArgCase.IsEqual ("-DIRECTION"))
    {
      if ((anArgIt + 3) >= theArgsNb
       || aLightCurr.IsNull()
       || (aLightCurr->Type() != Graphic3d_TOLS_DIRECTIONAL
        && aLightCurr->Type() != Graphic3d_TOLS_SPOT))
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      anXYZ[0] = Atof (theArgVec[++anArgIt]);
      anXYZ[1] = Atof (theArgVec[++anArgIt]);
      anXYZ[2] = Atof (theArgVec[++anArgIt]);
      aLightCurr->SetDirection (anXYZ[0], anXYZ[1], anXYZ[2]);
    }
    else if (anArgCase.IsEqual ("SM")
          || anArgCase.IsEqual ("SMOOTHNESS")
          || anArgCase.IsEqual ("-SM")
          || anArgCase.IsEqual ("-SMOOTHNESS"))
    {
      if (++anArgIt >= theArgsNb
       || aLightCurr.IsNull())
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      Standard_ShortReal aSmoothness = (Standard_ShortReal )Atof (theArgVec[anArgIt]);
      if (Abs (aSmoothness) <= ShortRealEpsilon())
      {
        aLightCurr->SetIntensity (1.f);
      }
      else if (Abs (aLightCurr->Smoothness()) <= ShortRealEpsilon())
      {
        aLightCurr->SetIntensity ((aSmoothness * aSmoothness) / 3.f);
      }
      else
      {
        Standard_ShortReal aSmoothnessRatio = static_cast<Standard_ShortReal> (aSmoothness / aLightCurr->Smoothness());
        aLightCurr->SetIntensity (aLightCurr->Intensity() / (aSmoothnessRatio * aSmoothnessRatio));
      }

      if (aLightCurr->Type() == Graphic3d_TOLS_POSITIONAL)
      {
        aLightCurr->SetSmoothRadius (aSmoothness);
      }
      else if (aLightCurr->Type() == Graphic3d_TOLS_DIRECTIONAL)
      {
        aLightCurr->SetSmoothAngle (aSmoothness);
      }
    }
    else if (anArgCase.IsEqual ("INT")
          || anArgCase.IsEqual ("INTENSITY")
          || anArgCase.IsEqual ("-INT")
          || anArgCase.IsEqual ("-INTENSITY"))
    {
      if (++anArgIt >= theArgsNb
       || aLightCurr.IsNull())
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      Standard_ShortReal aIntensity = (Standard_ShortReal )Atof (theArgVec[anArgIt]);
      aLightCurr->SetIntensity (aIntensity);
    }
    else if (anArgCase.IsEqual ("ANG")
          || anArgCase.IsEqual ("ANGLE")
          || anArgCase.IsEqual ("-ANG")
          || anArgCase.IsEqual ("-ANGLE"))
    {
      if (++anArgIt >= theArgsNb
       || aLightCurr.IsNull()
       || aLightCurr->Type() != Graphic3d_TOLS_SPOT)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      Standard_ShortReal anAngle = (Standard_ShortReal )Atof (theArgVec[anArgIt]);
      aLightCurr->SetAngle (Standard_ShortReal (anAngle / 180.0 * M_PI));
    }
    else if (anArgCase.IsEqual ("CONSTATTEN")
          || anArgCase.IsEqual ("CONSTATTENUATION")
          || anArgCase.IsEqual ("-CONSTATTEN")
          || anArgCase.IsEqual ("-CONSTATTENUATION"))
    {
      if (++anArgIt >= theArgsNb
       || aLightCurr.IsNull()
       || (aLightCurr->Type() != Graphic3d_TOLS_POSITIONAL
        && aLightCurr->Type() != Graphic3d_TOLS_SPOT))
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      aLightCurr->Attenuation (anAtten[0], anAtten[1]);
      anAtten[0] = Atof (theArgVec[anArgIt]);
      aLightCurr->SetAttenuation ((Standard_ShortReal )anAtten[0], (Standard_ShortReal )anAtten[1]);
    }
    else if (anArgCase.IsEqual ("LINATTEN")
          || anArgCase.IsEqual ("LINEARATTEN")
          || anArgCase.IsEqual ("LINEARATTENUATION")
          || anArgCase.IsEqual ("-LINATTEN")
          || anArgCase.IsEqual ("-LINEARATTEN")
          || anArgCase.IsEqual ("-LINEARATTENUATION"))
    {
      if (++anArgIt >= theArgsNb
       || aLightCurr.IsNull()
       || (aLightCurr->Type() != Graphic3d_TOLS_POSITIONAL
        && aLightCurr->Type() != Graphic3d_TOLS_SPOT))
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      aLightCurr->Attenuation (anAtten[0], anAtten[1]);
      anAtten[1] = Atof (theArgVec[anArgIt]);
      aLightCurr->SetAttenuation ((Standard_ShortReal )anAtten[0], (Standard_ShortReal )anAtten[1]);
    }
    else if (anArgCase.IsEqual ("EXP")
          || anArgCase.IsEqual ("EXPONENT")
          || anArgCase.IsEqual ("SPOTEXP")
          || anArgCase.IsEqual ("SPOTEXPONENT")
          || anArgCase.IsEqual ("-EXP")
          || anArgCase.IsEqual ("-EXPONENT")
          || anArgCase.IsEqual ("-SPOTEXP")
          || anArgCase.IsEqual ("-SPOTEXPONENT"))
    {
      if (++anArgIt >= theArgsNb
       || aLightCurr.IsNull()
       || aLightCurr->Type() != Graphic3d_TOLS_SPOT)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      aLightCurr->SetConcentration ((Standard_ShortReal )Atof (theArgVec[anArgIt]));
    }
    else if (anArgCase.IsEqual("RANGE")
          || anArgCase.IsEqual("-RANGE"))
    {
      if (++anArgIt >= theArgsNb
       || aLightCurr.IsNull()
       || aLightCurr->Type() == Graphic3d_TOLS_AMBIENT
       || aLightCurr->Type() == Graphic3d_TOLS_DIRECTIONAL)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      aLightCurr->SetRange ((Standard_ShortReal)Atof (theArgVec[anArgIt]));
    }
    else if (anArgCase.IsEqual ("HEAD")
          || anArgCase.IsEqual ("HEADLIGHT")
          || anArgCase.IsEqual ("-HEAD")
          || anArgCase.IsEqual ("-HEADLIGHT"))
    {
      if (aLightCurr.IsNull()
       || aLightCurr->Type() == Graphic3d_TOLS_AMBIENT)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      Standard_Boolean isHeadLight = Standard_True;
      if (anArgIt + 1 < theArgsNb
       && Draw::ParseOnOff (theArgVec[anArgIt + 1], isHeadLight))
      {
        ++anArgIt;
      }
      aLightCurr->SetHeadlight (isHeadLight);
    }
    else
    {
      Message::SendFail() << "Warning: unknown argument '" << anArg << "'";
    }
  }

  addLight (aLightNew, aLayer, isGlobal);
  return 0;
}

//===============================================================================================
//function : VPBREnvironment
//purpose  :
//===============================================================================================
static int VPBREnvironment (Draw_Interpretor&,
                            Standard_Integer theArgsNb,
                            const char**     theArgVec)
{
  if (theArgsNb > 2)
  {
    Message::SendFail ("Syntax error: 'vpbrenv' command has only one argument");
    return 1;
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  TCollection_AsciiString anArg = TCollection_AsciiString (theArgVec[1]);
  anArg.LowerCase();

  if (anArg == "-generate"
   || anArg == "-gen")
  {
    aView->GeneratePBREnvironment (Standard_True);
  }
  else if (anArg == "-clear")
  {
    aView->ClearPBREnvironment (Standard_True);
  }
  else
  {
    Message::SendFail() << "Syntax error: unknown argument [" << theArgVec[1] << "] for 'vpbrenv' command";
    return 1;
  }

  return 0;
}

//! Read Graphic3d_RenderingParams::PerfCounters flag.
static Standard_Boolean parsePerfStatsFlag (const TCollection_AsciiString& theValue,
                                            Standard_Boolean& theToReset,
                                            Graphic3d_RenderingParams::PerfCounters& theFlagsRem,
                                            Graphic3d_RenderingParams::PerfCounters& theFlagsAdd)
{
  Graphic3d_RenderingParams::PerfCounters aFlag = Graphic3d_RenderingParams::PerfCounters_NONE;
  TCollection_AsciiString aVal = theValue;
  Standard_Boolean toReverse = Standard_False;
  if (aVal == "none")
  {
    theToReset = Standard_True;
    return Standard_True;
  }
  else if (aVal.StartsWith ("-"))
  {
    toReverse = Standard_True;
    aVal = aVal.SubString (2, aVal.Length());
  }
  else if (aVal.StartsWith ("no"))
  {
    toReverse = Standard_True;
    aVal = aVal.SubString (3, aVal.Length());
  }
  else if (aVal.StartsWith ("+"))
  {
    aVal = aVal.SubString (2, aVal.Length());
  }
  else
  {
    theToReset = Standard_True;
  }

  if (     aVal == "fps"
        || aVal == "framerate")  aFlag = Graphic3d_RenderingParams::PerfCounters_FrameRate;
  else if (aVal == "cpu")        aFlag = Graphic3d_RenderingParams::PerfCounters_CPU;
  else if (aVal == "layers")     aFlag = Graphic3d_RenderingParams::PerfCounters_Layers;
  else if (aVal == "structs"
        || aVal == "structures"
        || aVal == "objects")    aFlag = Graphic3d_RenderingParams::PerfCounters_Structures;
  else if (aVal == "groups")     aFlag = Graphic3d_RenderingParams::PerfCounters_Groups;
  else if (aVal == "arrays")     aFlag = Graphic3d_RenderingParams::PerfCounters_GroupArrays;
  else if (aVal == "tris"
        || aVal == "triangles")  aFlag = Graphic3d_RenderingParams::PerfCounters_Triangles;
  else if (aVal == "pnts"
        || aVal == "points")     aFlag = Graphic3d_RenderingParams::PerfCounters_Points;
  else if (aVal == "lines")      aFlag = Graphic3d_RenderingParams::PerfCounters_Lines;
  else if (aVal == "mem"
        || aVal == "gpumem"
        || aVal == "estimmem")   aFlag = Graphic3d_RenderingParams::PerfCounters_EstimMem;
  else if (aVal == "skipimmediate"
        || aVal == "noimmediate") aFlag = Graphic3d_RenderingParams::PerfCounters_SkipImmediate;
  else if (aVal == "frametime"
        || aVal == "frametimers"
        || aVal == "time")       aFlag = Graphic3d_RenderingParams::PerfCounters_FrameTime;
  else if (aVal == "basic")      aFlag = Graphic3d_RenderingParams::PerfCounters_Basic;
  else if (aVal == "extended"
        || aVal == "verbose"
        || aVal == "extra")      aFlag = Graphic3d_RenderingParams::PerfCounters_Extended;
  else if (aVal == "full"
        || aVal == "all")        aFlag = Graphic3d_RenderingParams::PerfCounters_All;
  else
  {
    return Standard_False;
  }

  if (toReverse)
  {
    theFlagsRem = Graphic3d_RenderingParams::PerfCounters(theFlagsRem | aFlag);
  }
  else
  {
    theFlagsAdd = Graphic3d_RenderingParams::PerfCounters(theFlagsAdd | aFlag);
  }
  return Standard_True;
}

//! Read Graphic3d_RenderingParams::PerfCounters flags.
static Standard_Boolean convertToPerfStatsFlags (const TCollection_AsciiString& theValue,
                                                 Graphic3d_RenderingParams::PerfCounters& theFlags)
{
  TCollection_AsciiString aValue = theValue;
  Graphic3d_RenderingParams::PerfCounters aFlagsRem = Graphic3d_RenderingParams::PerfCounters_NONE;
  Graphic3d_RenderingParams::PerfCounters aFlagsAdd = Graphic3d_RenderingParams::PerfCounters_NONE;
  Standard_Boolean toReset = Standard_False;
  for (;;)
  {
    Standard_Integer aSplitPos = aValue.Search ("|");
    if (aSplitPos <= 0)
    {
      if (!parsePerfStatsFlag (aValue, toReset, aFlagsRem, aFlagsAdd))
      {
        return Standard_False;
      }
      if (toReset)
      {
        theFlags = Graphic3d_RenderingParams::PerfCounters_NONE;
      }
      theFlags = Graphic3d_RenderingParams::PerfCounters(theFlags |  aFlagsAdd);
      theFlags = Graphic3d_RenderingParams::PerfCounters(theFlags & ~aFlagsRem);
      return Standard_True;
    }

    if (aSplitPos > 1)
    {
      TCollection_AsciiString aSubValue = aValue.SubString (1, aSplitPos - 1);
      if (!parsePerfStatsFlag (aSubValue, toReset, aFlagsRem, aFlagsAdd))
      {
        return Standard_False;
      }
    }
    aValue = aValue.SubString (aSplitPos + 1, aValue.Length());
  }
}

//=======================================================================
//function : VRenderParams
//purpose  : Enables/disables rendering features
//=======================================================================

static Standard_Integer VRenderParams (Draw_Interpretor& theDI,
                                       Standard_Integer  theArgNb,
                                       const char**      theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Graphic3d_RenderingParams& aParams = aView->ChangeRenderingParams();
  TCollection_AsciiString aCmdName (theArgVec[0]);
  aCmdName.LowerCase();
  if (aCmdName == "vraytrace")
  {
    if (theArgNb == 1)
    {
      theDI << (aParams.Method == Graphic3d_RM_RAYTRACING ? "on" : "off") << " ";
      return 0;
    }
    else if (theArgNb == 2)
    {
      TCollection_AsciiString aValue (theArgVec[1]);
      aValue.LowerCase();
      if (aValue == "on"
       || aValue == "1")
      {
        aParams.Method = Graphic3d_RM_RAYTRACING;
        aView->Redraw();
        return 0;
      }
      else if (aValue == "off"
            || aValue == "0")
      {
        aParams.Method = Graphic3d_RM_RASTERIZATION;
        aView->Redraw();
        return 0;
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown argument '" << theArgVec[1] << "'";
        return 1;
      }
    }
    else
    {
      Message::SendFail ("Syntax error: wrong number of arguments");
      return 1;
    }
  }

  if (theArgNb < 2)
  {
    theDI << "renderMode:  ";
    switch (aParams.Method)
    {
      case Graphic3d_RM_RASTERIZATION: theDI << "rasterization "; break;
      case Graphic3d_RM_RAYTRACING:    theDI << "raytrace ";      break;
    }
    theDI << "\n";
    theDI << "transparency:  ";
    switch (aParams.TransparencyMethod)
    {
      case Graphic3d_RTM_BLEND_UNORDERED: theDI << "Basic blended transparency with non-commuting operator "; break;
      case Graphic3d_RTM_BLEND_OIT:       theDI << "Weighted Blended Order-Independent Transparency, depth weight factor: "
                                                << TCollection_AsciiString (aParams.OitDepthFactor); break;
    }
    theDI << "\n";
    theDI << "msaa:           " <<  aParams.NbMsaaSamples                               << "\n";
    theDI << "rendScale:      " <<  aParams.RenderResolutionScale                       << "\n";
    theDI << "rayDepth:       " <<  aParams.RaytracingDepth                             << "\n";
    theDI << "fsaa:           " << (aParams.IsAntialiasingEnabled       ? "on" : "off") << "\n";
    theDI << "shadows:        " << (aParams.IsShadowEnabled             ? "on" : "off") << "\n";
    theDI << "reflections:    " << (aParams.IsReflectionEnabled         ? "on" : "off") << "\n";
    theDI << "gleam:          " << (aParams.IsTransparentShadowEnabled  ? "on" : "off") << "\n";
    theDI << "GI:             " << (aParams.IsGlobalIlluminationEnabled ? "on" : "off") << "\n";
    theDI << "blocked RNG:    " << (aParams.CoherentPathTracingMode     ? "on" : "off") << "\n";
    theDI << "iss:            " << (aParams.AdaptiveScreenSampling      ? "on" : "off") << "\n";
    theDI << "iss debug:      " << (aParams.ShowSamplingTiles           ? "on" : "off") << "\n";
    theDI << "two-sided BSDF: " << (aParams.TwoSidedBsdfModels          ? "on" : "off") << "\n";
    theDI << "max radiance:   " <<  aParams.RadianceClampingValue                       << "\n";
    theDI << "nb tiles (iss): " <<  aParams.NbRayTracingTiles                           << "\n";
    theDI << "tile size (iss):" <<  aParams.RayTracingTileSize << "x" << aParams.RayTracingTileSize << "\n";
    theDI << "shadingModel: ";
    switch (aView->ShadingModel())
    {
      case Graphic3d_TOSM_DEFAULT:   theDI << "default";   break;
      case Graphic3d_TOSM_UNLIT:     theDI << "unlit";     break;
      case Graphic3d_TOSM_FACET:     theDI << "flat";      break;
      case Graphic3d_TOSM_VERTEX:    theDI << "gouraud";   break;
      case Graphic3d_TOSM_FRAGMENT:  theDI << "phong";     break;
      case Graphic3d_TOSM_PBR:       theDI << "pbr";       break;
      case Graphic3d_TOSM_PBR_FACET: theDI << "pbr_facet"; break;
    }
    {
      theDI << "perfCounters:";
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_FrameRate) != 0)
      {
        theDI << " fps";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_CPU) != 0)
      {
        theDI << " cpu";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_Structures) != 0)
      {
        theDI << " structs";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_Groups) != 0)
      {
        theDI << " groups";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_GroupArrays) != 0)
      {
        theDI << " arrays";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_Triangles) != 0)
      {
        theDI << " tris";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_Lines) != 0)
      {
        theDI << " lines";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_Points) != 0)
      {
        theDI << " pnts";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_EstimMem) != 0)
      {
        theDI << " gpumem";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_FrameTime) != 0)
      {
        theDI << " frameTime";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_SkipImmediate) != 0)
      {
        theDI << " skipimmediate";
      }
      if (aParams.CollectedStats == Graphic3d_RenderingParams::PerfCounters_NONE)
      {
        theDI << " none";
      }
      theDI << "\n";
    }
    theDI << "depth pre-pass: " << (aParams.ToEnableDepthPrepass        ? "on" : "off") << "\n";
    theDI << "alpha to coverage: " << (aParams.ToEnableAlphaToCoverage  ? "on" : "off") << "\n";
    theDI << "frustum culling: " << (aParams.FrustumCullingState == Graphic3d_RenderingParams::FrustumCulling_On  ? "on" :
                                     aParams.FrustumCullingState == Graphic3d_RenderingParams::FrustumCulling_Off ? "off" :
                                                                                                                    "noUpdate") << "\n";
    theDI << "\n";
    return 0;
  }

  bool toPrint = false, toSyncDefaults = false, toSyncAllViews = false;
  ViewerTest_AutoUpdater anUpdateTool (ViewerTest::GetAISContext(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg (theArgVec[anArgIter]);
    TCollection_AsciiString aFlag (anArg);
    aFlag.LowerCase();
    if (anUpdateTool.parseRedrawMode (aFlag))
    {
      continue;
    }
    else if (aFlag == "-echo"
          || aFlag == "-print")
    {
      toPrint = Standard_True;
      anUpdateTool.Invalidate();
    }
    else if (aFlag == "-reset")
    {
      aParams = ViewerTest::GetViewerFromContext()->DefaultRenderingParams();
    }
    else if (aFlag == "-sync"
          && (anArgIter + 1 < theArgNb))
    {
      TCollection_AsciiString aSyncFlag (theArgVec[++anArgIter]);
      aSyncFlag.LowerCase();
      if (aSyncFlag == "default"
       || aSyncFlag == "defaults"
       || aSyncFlag == "viewer")
      {
        toSyncDefaults = true;
      }
      else if (aSyncFlag == "allviews"
            || aSyncFlag == "views")
      {
        toSyncAllViews = true;
      }
      else
      {
        Message::SendFail ("Syntax error: unknown parameter to -sync argument");
        return 1;
      }
    }
    else if (aFlag == "-mode"
          || aFlag == "-rendermode"
          || aFlag == "-render_mode")
    {
      if (toPrint)
      {
        switch (aParams.Method)
        {
          case Graphic3d_RM_RASTERIZATION: theDI << "rasterization "; break;
          case Graphic3d_RM_RAYTRACING:    theDI << "ray-tracing ";   break;
        }
        continue;
      }
      else
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-ray"
          || aFlag == "-raytrace")
    {
      if (toPrint)
      {
        theDI << (aParams.Method == Graphic3d_RM_RAYTRACING ? "true" : "false") << " ";
        continue;
      }

      bool isRayTrace = true;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], isRayTrace))
      {
        ++anArgIter;
      }
      aParams.Method = isRayTrace ? Graphic3d_RM_RAYTRACING : Graphic3d_RM_RASTERIZATION;
    }
    else if (aFlag == "-rast"
          || aFlag == "-raster"
          || aFlag == "-rasterization")
    {
      if (toPrint)
      {
        theDI << (aParams.Method == Graphic3d_RM_RASTERIZATION ? "true" : "false") << " ";
        continue;
      }

      bool isRaster = true;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], isRaster))
      {
        ++anArgIter;
      }
      aParams.Method = isRaster ? Graphic3d_RM_RASTERIZATION : Graphic3d_RM_RAYTRACING;
    }
    else if (aFlag == "-msaa")
    {
      if (toPrint)
      {
        theDI << aParams.NbMsaaSamples << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aNbSamples = Draw::Atoi (theArgVec[anArgIter]);
      if (aNbSamples < 0)
      {
        Message::SendFail() << "Syntax error: invalid number of MSAA samples " << aNbSamples << "";
        return 1;
      }
      else
      {
        aParams.NbMsaaSamples = aNbSamples;
      }
    }
    else if (aFlag == "-linefeather"
          || aFlag == "-edgefeather"
          || aFlag == "-feather")
    {
      if (toPrint)
      {
        theDI << " " << aParams.LineFeather << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aParam = theArgVec[anArgIter];
      const Standard_ShortReal aFeather = (Standard_ShortReal) Draw::Atof (theArgVec[anArgIter]);
      if (aFeather <= 0.0f)
      {
        Message::SendFail() << "Syntax error: invalid value of line width feather " << aFeather << ". Should be > 0";
        return 1;
      }
      aParams.LineFeather = aFeather;
    }
    else if (aFlag == "-oit")
    {
      if (toPrint)
      {
        if (aParams.TransparencyMethod == Graphic3d_RTM_BLEND_OIT)
        {
          theDI << "on, depth weight factor: " << TCollection_AsciiString (aParams.OitDepthFactor) << " ";
        }
        else
        {
          theDI << "off" << " ";
        }
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aParam = theArgVec[anArgIter];
      aParam.LowerCase();
      if (aParam.IsRealValue())
      {
        const Standard_ShortReal aWeight = (Standard_ShortReal) Draw::Atof (theArgVec[anArgIter]);
        if (aWeight < 0.f || aWeight > 1.f)
        {
          Message::SendFail() << "Syntax error: invalid value of Weighted Order-Independent Transparency depth weight factor " << aWeight << ". Should be within range [0.0; 1.0]";
          return 1;
        }

        aParams.TransparencyMethod = Graphic3d_RTM_BLEND_OIT;
        aParams.OitDepthFactor     = aWeight;
      }
      else if (aParam == "off")
      {
        aParams.TransparencyMethod = Graphic3d_RTM_BLEND_UNORDERED;
      }
      else
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-depthprepass")
    {
      if (toPrint)
      {
        theDI << (aParams.ToEnableDepthPrepass ? "on " : "off ");
        continue;
      }
      aParams.ToEnableDepthPrepass = Standard_True;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], aParams.ToEnableDepthPrepass))
      {
        ++anArgIter;
      }
    }
    else if (aFlag == "-samplealphatocoverage"
          || aFlag == "-alphatocoverage")
    {
      if (toPrint)
      {
        theDI << (aParams.ToEnableAlphaToCoverage ? "on " : "off ");
        continue;
      }
      aParams.ToEnableAlphaToCoverage = Standard_True;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], aParams.ToEnableAlphaToCoverage))
      {
        ++anArgIter;
      }
    }
    else if (aFlag == "-rendscale"
          || aFlag == "-renderscale"
          || aFlag == "-renderresolutionscale")
    {
      if (toPrint)
      {
        theDI << aParams.RenderResolutionScale << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Real aScale = Draw::Atof (theArgVec[anArgIter]);
      if (aScale < 0.01)
      {
        Message::SendFail() << "Syntax error: invalid rendering resolution scale " << aScale << "";
        return 1;
      }
      else
      {
        aParams.RenderResolutionScale = Standard_ShortReal(aScale);
      }
    }
    else if (aFlag == "-raydepth"
          || aFlag == "-ray_depth")
    {
      if (toPrint)
      {
        theDI << aParams.RaytracingDepth << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aDepth = Draw::Atoi (theArgVec[anArgIter]);

      // We allow RaytracingDepth be more than 10 in case of GI enabled
      if (aDepth < 1 || (aDepth > 10 && !aParams.IsGlobalIlluminationEnabled))
      {
        Message::SendFail() << "Syntax error: invalid ray-tracing depth " << aDepth << ". Should be within range [1; 10]";
        return 1;
      }
      else
      {
        aParams.RaytracingDepth = aDepth;
      }
    }
    else if (aFlag == "-shad"
          || aFlag == "-shadows")
    {
      if (toPrint)
      {
        theDI << (aParams.IsShadowEnabled ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.IsShadowEnabled = toEnable;
    }
    else if (aFlag == "-refl"
          || aFlag == "-reflections")
    {
      if (toPrint)
      {
        theDI << (aParams.IsReflectionEnabled ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.IsReflectionEnabled = toEnable;
    }
    else if (aFlag == "-fsaa")
    {
      if (toPrint)
      {
        theDI << (aParams.IsAntialiasingEnabled ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.IsAntialiasingEnabled = toEnable;
    }
    else if (aFlag == "-gleam")
    {
      if (toPrint)
      {
        theDI << (aParams.IsTransparentShadowEnabled ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.IsTransparentShadowEnabled = toEnable;
    }
    else if (aFlag == "-gi")
    {
      if (toPrint)
      {
        theDI << (aParams.IsGlobalIlluminationEnabled ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.IsGlobalIlluminationEnabled = toEnable;
      if (!toEnable)
      {
        aParams.RaytracingDepth = Min (aParams.RaytracingDepth, 10);
      }
    }
    else if (aFlag == "-blockedrng"
          || aFlag == "-brng")
    {
      if (toPrint)
      {
        theDI << (aParams.CoherentPathTracingMode ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
        && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.CoherentPathTracingMode = toEnable;
    }
    else if (aFlag == "-maxrad")
    {
      if (toPrint)
      {
        theDI << aParams.RadianceClampingValue << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const TCollection_AsciiString aMaxRadStr = theArgVec[anArgIter];
      if (!aMaxRadStr.IsRealValue())
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Real aMaxRadiance = aMaxRadStr.RealValue();
      if (aMaxRadiance <= 0.0)
      {
        Message::SendFail() << "Syntax error: invalid radiance clamping value " << aMaxRadiance;
        return 1;
      }
      else
      {
        aParams.RadianceClampingValue = static_cast<Standard_ShortReal> (aMaxRadiance);
      }
    }
    else if (aFlag == "-iss")
    {
      if (toPrint)
      {
        theDI << (aParams.AdaptiveScreenSampling ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
        && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.AdaptiveScreenSampling = toEnable;
    }
    else if (aFlag == "-issatomic")
    {
      if (toPrint)
      {
        theDI << (aParams.AdaptiveScreenSamplingAtomic ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.AdaptiveScreenSamplingAtomic = toEnable;
    }
    else if (aFlag == "-issd")
    {
      if (toPrint)
      {
        theDI << (aParams.ShowSamplingTiles ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
        && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.ShowSamplingTiles = toEnable;
    }
    else if (aFlag == "-tilesize")
    {
      if (toPrint)
      {
        theDI << aParams.RayTracingTileSize << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aTileSize = Draw::Atoi (theArgVec[anArgIter]);
      if (aTileSize < 1)
      {
        Message::SendFail() << "Syntax error: invalid size of ISS tile " << aTileSize;
        return 1;
      }
      aParams.RayTracingTileSize = aTileSize;
    }
    else if (aFlag == "-nbtiles")
    {
      if (toPrint)
      {
        theDI << aParams.NbRayTracingTiles << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aNbTiles = Draw::Atoi (theArgVec[anArgIter]);
      if (aNbTiles < -1)
      {
        Message::SendFail() << "Syntax error: invalid number of ISS tiles " << aNbTiles;
        return 1;
      }
      else if (aNbTiles > 0
            && (aNbTiles < 64
             || aNbTiles > 1024))
      {
        Message::SendWarning() << "Warning: suboptimal number of ISS tiles " << aNbTiles << ". Recommended range: [64, 1024].";
      }
      aParams.NbRayTracingTiles = aNbTiles;
    }
    else if (aFlag == "-env")
    {
      if (toPrint)
      {
        theDI << (aParams.UseEnvironmentMapBackground ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
        && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.UseEnvironmentMapBackground = toEnable;
    }
    else if (aFlag == "-ignorenormalmap")
    {
      if (toPrint)
      {
        theDI << (aParams.ToIgnoreNormalMapInRayTracing ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
        && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.ToIgnoreNormalMapInRayTracing = toEnable;
    }
    else if (aFlag == "-twoside")
    {
      if (toPrint)
      {
        theDI << (aParams.TwoSidedBsdfModels ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
        && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.TwoSidedBsdfModels = toEnable;
    }
    else if (aFlag == "-shademodel"
          || aFlag == "-shadingmodel"
          || aFlag == "-shading")
    {
      if (toPrint)
      {
        switch (aView->ShadingModel())
        {
          case Graphic3d_TOSM_DEFAULT:   theDI << "default";   break;
          case Graphic3d_TOSM_UNLIT:     theDI << "unlit ";    break;
          case Graphic3d_TOSM_FACET:     theDI << "flat ";     break;
          case Graphic3d_TOSM_VERTEX:    theDI << "gouraud ";  break;
          case Graphic3d_TOSM_FRAGMENT:  theDI << "phong ";    break;
          case Graphic3d_TOSM_PBR:       theDI << "pbr";       break;
          case Graphic3d_TOSM_PBR_FACET: theDI << "pbr_facet"; break;
        }
        continue;
      }

      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
      }

      Graphic3d_TypeOfShadingModel aModel = Graphic3d_TOSM_DEFAULT;
      if (ViewerTest::ParseShadingModel (theArgVec[anArgIter], aModel)
       && aModel != Graphic3d_TOSM_DEFAULT)
      {
        aView->SetShadingModel (aModel);
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown shading model '" << theArgVec[anArgIter] << "'";
        return 1;
      }
    }
    else if (aFlag == "-pbrenvpow2size"
          || aFlag == "-pbrenvp2s"
          || aFlag == "-pep2s")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aPbrEnvPow2Size = Draw::Atoi (theArgVec[anArgIter]);
      if (aPbrEnvPow2Size < 1)
      {
        Message::SendFail ("Syntax error: 'Pow2Size' of PBR Environment has to be greater or equal 1");
        return 1;
      }
      aParams.PbrEnvPow2Size = aPbrEnvPow2Size;
    }
    else if (aFlag == "-pbrenvspecmaplevelsnumber"
          || aFlag == "-pbrenvspecmapnblevels"
          || aFlag == "-pbrenvspecmaplevels"
          || aFlag == "-pbrenvsmln"
          || aFlag == "-pesmln")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aPbrEnvSpecMapNbLevels = Draw::Atoi (theArgVec[anArgIter]);
      if (aPbrEnvSpecMapNbLevels < 2)
      {
        Message::SendFail ("Syntax error: 'SpecMapLevelsNumber' of PBR Environment has to be greater or equal 2");
        return 1;
      }
      aParams.PbrEnvSpecMapNbLevels = aPbrEnvSpecMapNbLevels;
    }
    else if (aFlag == "-pbrenvbakngdiffsamplesnumber"
          || aFlag == "-pbrenvbakingdiffsamples"
          || aFlag == "-pbrenvbdsn")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aPbrEnvBakingDiffNbSamples = Draw::Atoi (theArgVec[anArgIter]);
      if (aPbrEnvBakingDiffNbSamples < 1)
      {
        Message::SendFail ("Syntax error: 'BakingDiffSamplesNumber' of PBR Environtment has to be greater or equal 1");
        return 1;
      }
      aParams.PbrEnvBakingDiffNbSamples = aPbrEnvBakingDiffNbSamples;
    }
    else if (aFlag == "-pbrenvbakngspecsamplesnumber"
          || aFlag == "-pbrenvbakingspecsamples"
          || aFlag == "-pbrenvbssn")
    {
    if (++anArgIter >= theArgNb)
    {
      Message::SendFail() << "Syntax error at argument '" << anArg << "'";
      return 1;
    }

    const Standard_Integer aPbrEnvBakingSpecNbSamples = Draw::Atoi(theArgVec[anArgIter]);
    if (aPbrEnvBakingSpecNbSamples < 1)
    {
      Message::SendFail ("Syntax error: 'BakingSpecSamplesNumber' of PBR Environtment has to be greater or equal 1");
      return 1;
    }
    aParams.PbrEnvBakingSpecNbSamples = aPbrEnvBakingSpecNbSamples;
    }
    else if (aFlag == "-pbrenvbakingprobability"
          || aFlag == "-pbrenvbp")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      const Standard_ShortReal aPbrEnvBakingProbability = static_cast<Standard_ShortReal>(Draw::Atof (theArgVec[anArgIter]));
      if (aPbrEnvBakingProbability < 0.f
       || aPbrEnvBakingProbability > 1.f)
      {
        Message::SendFail ("Syntax error: 'BakingProbability' of PBR Environtment has to be in range of [0, 1]");
        return 1;
      }
      aParams.PbrEnvBakingProbability = aPbrEnvBakingProbability;
    }
    else if (aFlag == "-resolution")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aResolution (theArgVec[anArgIter]);
      if (aResolution.IsIntegerValue())
      {
        aView->ChangeRenderingParams().Resolution = static_cast<unsigned int> (Draw::Atoi (aResolution.ToCString()));
      }
      else
      {
        Message::SendFail() << "Syntax error: wrong syntax at argument'" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-rebuildglsl"
          || aFlag == "-rebuild")
    {
      if (toPrint)
      {
        theDI << (aParams.RebuildRayTracingShaders ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
          && !Draw::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.RebuildRayTracingShaders = toEnable;
    }
    else if (aFlag == "-focal")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aParam (theArgVec[anArgIter]);
      if (aParam.IsRealValue())
      {
        float aFocalDist = static_cast<float> (aParam.RealValue());
        if (aFocalDist < 0)
        {
          Message::SendFail() << "Error: parameter can't be negative at argument '" << anArg << "'";
          return 1;
        }
        aView->ChangeRenderingParams().CameraFocalPlaneDist = aFocalDist;
      }
      else
      {
        Message::SendFail() << "Syntax error at argument'" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-aperture")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aParam(theArgVec[anArgIter]);
      if (aParam.IsRealValue())
      {
        float aApertureSize = static_cast<float> (aParam.RealValue());
        if (aApertureSize < 0)
        {
          Message::SendFail() << "Error: parameter can't be negative at argument '" << anArg << "'";
          return 1;
        }
        aView->ChangeRenderingParams().CameraApertureRadius = aApertureSize;
      }
      else
      {
        Message::SendFail() << "Syntax error at argument'" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-exposure")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString anExposure (theArgVec[anArgIter]);
      if (anExposure.IsRealValue())
      {
        aView->ChangeRenderingParams().Exposure = static_cast<float> (anExposure.RealValue());
      }
      else
      {
        Message::SendFail() << "Syntax error at argument'" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-whitepoint")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aWhitePoint (theArgVec[anArgIter]);
      if (aWhitePoint.IsRealValue())
      {
        aView->ChangeRenderingParams().WhitePoint = static_cast<float> (aWhitePoint.RealValue());
      }
      else
      {
        Message::SendFail() << "Syntax error at argument'" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-tonemapping")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aMode (theArgVec[anArgIter]);
      aMode.LowerCase();

      if (aMode == "disabled")
      {
        aView->ChangeRenderingParams().ToneMappingMethod = Graphic3d_ToneMappingMethod_Disabled;
      }
      else if (aMode == "filmic")
      {
        aView->ChangeRenderingParams().ToneMappingMethod = Graphic3d_ToneMappingMethod_Filmic;
      }
      else
      {
        Message::SendFail() << "Syntax error at argument'" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-performancestats"
          || aFlag == "-performancecounters"
          || aFlag == "-perfstats"
          || aFlag == "-perfcounters"
          || aFlag == "-stats")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aFlagsStr (theArgVec[anArgIter]);
      aFlagsStr.LowerCase();
      Graphic3d_RenderingParams::PerfCounters aFlags = aView->ChangeRenderingParams().CollectedStats;
      if (!convertToPerfStatsFlags (aFlagsStr, aFlags))
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      aView->ChangeRenderingParams().CollectedStats = aFlags;
      aView->ChangeRenderingParams().ToShowStats = aFlags != Graphic3d_RenderingParams::PerfCounters_NONE;
    }
    else if (aFlag == "-perfupdateinterval"
          || aFlag == "-statsupdateinterval")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      aView->ChangeRenderingParams().StatsUpdateInterval = (Standard_ShortReal )Draw::Atof (theArgVec[anArgIter]);
    }
    else if (aFlag == "-perfchart"
          || aFlag == "-statschart")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      aView->ChangeRenderingParams().StatsNbFrames = Draw::Atoi (theArgVec[anArgIter]);
    }
    else if (aFlag == "-perfchartmax"
          || aFlag == "-statschartmax")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      aView->ChangeRenderingParams().StatsMaxChartTime = (Standard_ShortReal )Draw::Atof (theArgVec[anArgIter]);
    }
    else if (aFlag == "-frustumculling"
          || aFlag == "-culling")
    {
      if (toPrint)
      {
        theDI << ((aParams.FrustumCullingState == Graphic3d_RenderingParams::FrustumCulling_On)  ? "on" :
                  (aParams.FrustumCullingState == Graphic3d_RenderingParams::FrustumCulling_Off) ? "off" :
                                                                                                   "noUpdate") << " ";
        continue;
      }

      Graphic3d_RenderingParams::FrustumCulling aState = Graphic3d_RenderingParams::FrustumCulling_On;
      if (++anArgIter < theArgNb)
      {
        TCollection_AsciiString aStateStr(theArgVec[anArgIter]);
        aStateStr.LowerCase();
        bool toEnable = true;
        if (Draw::ParseOnOff (aStateStr.ToCString(), toEnable))
        {
          aState = toEnable ? Graphic3d_RenderingParams::FrustumCulling_On : Graphic3d_RenderingParams::FrustumCulling_Off;
        }
        else if (aStateStr == "noupdate"
              || aStateStr == "freeze")
        {
          aState = Graphic3d_RenderingParams::FrustumCulling_NoUpdate;
        }
        else
        {
          --anArgIter;
        }
      }
      aParams.FrustumCullingState = aState;
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown flag '" << anArg << "'";
      return 1;
    }
  }

  // set current view parameters as defaults
  if (toSyncDefaults)
  {
    ViewerTest::GetViewerFromContext()->SetDefaultRenderingParams (aParams);
  }
  if (toSyncAllViews)
  {
    for (V3d_ListOfViewIterator aViewIter = ViewerTest::GetViewerFromContext()->DefinedViewIterator(); aViewIter.More(); aViewIter.Next())
    {
      aViewIter.Value()->ChangeRenderingParams() = aParams;
    }
  }
  return 0;
}

//=======================================================================
//function : searchInfo
//purpose  :
//=======================================================================
inline TCollection_AsciiString searchInfo (const TColStd_IndexedDataMapOfStringString& theDict,
                                           const TCollection_AsciiString&              theKey)
{
  for (TColStd_IndexedDataMapOfStringString::Iterator anIter (theDict); anIter.More(); anIter.Next())
  {
    if (TCollection_AsciiString::IsSameString (anIter.Key(), theKey, Standard_False))
    {
      return anIter.Value();
    }
  }
  return TCollection_AsciiString();
}

//=======================================================================
//function : VStatProfiler
//purpose  :
//=======================================================================
static Standard_Integer VStatProfiler (Draw_Interpretor& theDI,
                                       Standard_Integer  theArgNb,
                                       const char**      theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Standard_Boolean toRedraw = Standard_True;
  Graphic3d_RenderingParams::PerfCounters aPrevCounters = aView->ChangeRenderingParams().CollectedStats;
  Standard_ShortReal aPrevUpdInterval = aView->ChangeRenderingParams().StatsUpdateInterval;
  Graphic3d_RenderingParams::PerfCounters aRenderParams = Graphic3d_RenderingParams::PerfCounters_NONE;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg (theArgVec[anArgIter]);
    TCollection_AsciiString aFlag (anArg);
    aFlag.LowerCase();
    if (aFlag == "-noredraw")
    {
      toRedraw = Standard_False;
    }
    else
    {
      Graphic3d_RenderingParams::PerfCounters aParam = Graphic3d_RenderingParams::PerfCounters_NONE;
      if      (aFlag == "fps")        aParam = Graphic3d_RenderingParams::PerfCounters_FrameRate;
      else if (aFlag == "cpu")        aParam = Graphic3d_RenderingParams::PerfCounters_CPU;
      else if (aFlag == "alllayers"
            || aFlag == "layers")     aParam = Graphic3d_RenderingParams::PerfCounters_Layers;
      else if (aFlag == "allstructs"
            || aFlag == "allstructures"
            || aFlag == "structs"
            || aFlag == "structures") aParam = Graphic3d_RenderingParams::PerfCounters_Structures;
      else if (aFlag == "groups")     aParam = Graphic3d_RenderingParams::PerfCounters_Groups;
      else if (aFlag == "allarrays"
            || aFlag == "fillarrays"
            || aFlag == "linearrays"
            || aFlag == "pointarrays"
            || aFlag == "textarrays") aParam = Graphic3d_RenderingParams::PerfCounters_GroupArrays;
      else if (aFlag == "triangles")  aParam = Graphic3d_RenderingParams::PerfCounters_Triangles;
      else if (aFlag == "lines")      aParam = Graphic3d_RenderingParams::PerfCounters_Lines;
      else if (aFlag == "points")     aParam = Graphic3d_RenderingParams::PerfCounters_Points;
      else if (aFlag == "geommem"
            || aFlag == "texturemem"
            || aFlag == "framemem")   aParam = Graphic3d_RenderingParams::PerfCounters_EstimMem;
      else if (aFlag == "elapsedframe"
            || aFlag == "cpuframeaverage"
            || aFlag == "cpupickingaverage"
            || aFlag == "cpucullingaverage"
            || aFlag == "cpudynaverage"
            || aFlag == "cpuframemax"
            || aFlag == "cpupickingmax"
            || aFlag == "cpucullingmax"
            || aFlag == "cpudynmax")  aParam = Graphic3d_RenderingParams::PerfCounters_FrameTime;
      else
      {
        Message::SendFail() << "Error: unknown argument '" << theArgVec[anArgIter] << "'";
        continue;
      }

      aRenderParams = Graphic3d_RenderingParams::PerfCounters (aRenderParams | aParam);
    }
  }

  if (aRenderParams != Graphic3d_RenderingParams::PerfCounters_NONE)
  {
    aView->ChangeRenderingParams().CollectedStats =
      Graphic3d_RenderingParams::PerfCounters (aView->RenderingParams().CollectedStats | aRenderParams);

    if (toRedraw)
    {
      aView->ChangeRenderingParams().StatsUpdateInterval = -1;
      aView->Redraw();
      aView->ChangeRenderingParams().StatsUpdateInterval = aPrevUpdInterval;
    }

    TColStd_IndexedDataMapOfStringString aDict;
    aView->StatisticInformation (aDict);

    aView->ChangeRenderingParams().CollectedStats = aPrevCounters;

    for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
    {
      Standard_CString        anArg(theArgVec[anArgIter]);
      TCollection_AsciiString aFlag(anArg);
      aFlag.LowerCase();
      if (aFlag == "fps")
      {
        theDI << searchInfo (aDict, "FPS") << " ";
      }
      else if (aFlag == "cpu")
      {
        theDI << searchInfo (aDict, "CPU FPS") << " ";
      }
      else if (aFlag == "alllayers")
      {
        theDI << searchInfo (aDict, "Layers") << " ";
      }
      else if (aFlag == "layers")
      {
        theDI << searchInfo (aDict, "Rendered layers") << " ";
      }
      else if (aFlag == "allstructs"
            || aFlag == "allstructures")
      {
        theDI << searchInfo (aDict, "Structs") << " ";
      }
      else if (aFlag == "structs"
            || aFlag == "structures")
      {
        TCollection_AsciiString aRend = searchInfo (aDict, "Rendered structs");
        if (aRend.IsEmpty()) // all structures rendered
        {
          aRend = searchInfo (aDict, "Structs");
        }
        theDI << aRend << " ";
      }
      else if (aFlag == "groups")
      {
        theDI << searchInfo (aDict, "Rendered groups") << " ";
      }
      else if (aFlag == "allarrays")
      {
        theDI << searchInfo (aDict, "Rendered arrays") << " ";
      }
      else if (aFlag == "fillarrays")
      {
        theDI << searchInfo (aDict, "Rendered [fill] arrays") << " ";
      }
      else if (aFlag == "linearrays")
      {
        theDI << searchInfo (aDict, "Rendered [line] arrays") << " ";
      }
      else if (aFlag == "pointarrays")
      {
        theDI << searchInfo (aDict, "Rendered [point] arrays") << " ";
      }
      else if (aFlag == "textarrays")
      {
        theDI << searchInfo (aDict, "Rendered [text] arrays") << " ";
      }
      else if (aFlag == "triangles")
      {
        theDI << searchInfo (aDict, "Rendered triangles") << " ";
      }
      else if (aFlag == "points")
      {
        theDI << searchInfo (aDict, "Rendered points") << " ";
      }
      else if (aFlag == "geommem")
      {
        theDI << searchInfo (aDict, "GPU Memory [geometry]") << " ";
      }
      else if (aFlag == "texturemem")
      {
        theDI << searchInfo (aDict, "GPU Memory [textures]") << " ";
      }
      else if (aFlag == "framemem")
      {
        theDI << searchInfo (aDict, "GPU Memory [frames]") << " ";
      }
      else if (aFlag == "elapsedframe")
      {
        theDI << searchInfo (aDict, "Elapsed Frame (average)") << " ";
      }
      else if (aFlag == "cpuframe_average")
      {
        theDI << searchInfo (aDict, "CPU Frame (average)") << " ";
      }
      else if (aFlag == "cpupicking_average")
      {
        theDI << searchInfo (aDict, "CPU Picking (average)") << " ";
      }
      else if (aFlag == "cpuculling_average")
      {
        theDI << searchInfo (aDict, "CPU Culling (average)") << " ";
      }
      else if (aFlag == "cpudyn_average")
      {
        theDI << searchInfo (aDict, "CPU Dynamics (average)") << " ";
      }
      else if (aFlag == "cpuframe_max")
      {
        theDI << searchInfo (aDict, "CPU Frame (max)") << " ";
      }
      else if (aFlag == "cpupicking_max")
      {
        theDI << searchInfo (aDict, "CPU Picking (max)") << " ";
      }
      else if (aFlag == "cpuculling_max")
      {
        theDI << searchInfo (aDict, "CPU Culling (max)") << " ";
      }
      else if (aFlag == "cpudyn_max")
      {
        theDI << searchInfo (aDict, "CPU Dynamics (max)") << " ";
      }
    }
  }
  else
  {
    if (toRedraw)
    {
      aView->ChangeRenderingParams().StatsUpdateInterval = -1;
      aView->Redraw();
      aView->ChangeRenderingParams().StatsUpdateInterval = aPrevUpdInterval;
    }
    theDI << "Statistic info:\n" << aView->StatisticInformation();
  }
  return 0;
}

//=======================================================================
//function : VXRotate
//purpose  :
//=======================================================================
static Standard_Integer VXRotate (Draw_Interpretor& di,
                                   Standard_Integer argc,
                                   const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << argv[0] << "ERROR : use 'vinit' command before \n";
    return 1;
  }

  if (argc != 3)
  {
    di << "ERROR : Usage : " << argv[0] << " name angle\n";
    return 1;
  }

  TCollection_AsciiString aName (argv[1]);
  Standard_Real anAngle = Draw::Atof (argv[2]);

  // find object
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  Handle(AIS_InteractiveObject) anIObj;
  if (!aMap.Find2 (aName, anIObj))
  {
    di << "Use 'vdisplay' before\n";
    return 1;
  }

  gp_Trsf aTransform;
  aTransform.SetRotation (gp_Ax1 (gp_Pnt (0.0, 0.0, 0.0), gp_Vec (1.0, 0.0, 0.0)), anAngle);
  aTransform.SetTranslationPart (anIObj->LocalTransformation().TranslationPart());

  aContext->SetLocation (anIObj, aTransform);
  aContext->UpdateCurrentViewer();
  return 0;
}

//===============================================================================================
//function : VManipulator
//purpose  :
//===============================================================================================
static int VManipulator (Draw_Interpretor& theDi,
                         Standard_Integer  theArgsNb,
                         const char**      theArgVec)
{
  Handle(V3d_View)   aCurrentView   = ViewerTest::CurrentView();
  Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
  if (aCurrentView.IsNull()
   || aViewer.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  ViewerTest_AutoUpdater anUpdateTool (ViewerTest::GetAISContext(), ViewerTest::CurrentView());
  Standard_Integer anArgIter = 1;
  for (; anArgIter < theArgsNb; ++anArgIter)
  {
    anUpdateTool.parseRedrawMode (theArgVec[anArgIter]);
  }

  ViewerTest_CmdParser aCmd;
  aCmd.SetDescription ("Manages manipulator for interactive objects:");
  aCmd.AddOption ("attach",         "... object - attach manipulator to an object");
  aCmd.AddOption ("adjustPosition", "... {0|1} - adjust position when attaching");
  aCmd.AddOption ("adjustSize",     "... {0|1} - adjust size when attaching ");
  aCmd.AddOption ("enableModes",    "... {0|1} - enable modes when attaching ");
  aCmd.AddOption ("view",           "... {active | [view name]} - define view in which manipulator will be displayed, 'all' by default");
  aCmd.AddOption ("detach",         "...       - detach manipulator");

  aCmd.AddOption ("startTransform",   "... mouse_x mouse_y - invoke start transformation");
  aCmd.AddOption ("transform",        "... mouse_x mouse_y - invoke transformation");
  aCmd.AddOption ("stopTransform",    "... [abort] - invoke stop transformation");

  aCmd.AddOption ("move",   "... x y z - move object");
  aCmd.AddOption ("rotate", "... x y z dx dy dz angle - rotate object");
  aCmd.AddOption ("scale",  "... factor - scale object");

  aCmd.AddOption ("autoActivate",      "... {0|1} - set activation on detection");
  aCmd.AddOption ("followTranslation", "... {0|1} - set following translation transform");
  aCmd.AddOption ("followRotation",    "... {0|1} - set following rotation transform");
  aCmd.AddOption ("followDragging",    "... {0|1} - set following dragging transform");
  aCmd.AddOption ("gap",               "... value - set gap between sub-parts");
  aCmd.AddOption ("part",              "... axis mode {0|1} - set visual part");
  aCmd.AddOption ("parts",             "... all axes mode {0|1} - set visual part");
  aCmd.AddOption ("pos",               "... x y z [nx ny nz [xx xy xz]] - set position of manipulator");
  aCmd.AddOption ("size",              "... size - set size of manipulator");
  aCmd.AddOption ("zoomable",          "... {0|1} - set zoom persistence");

  aCmd.Parse (theArgsNb, theArgVec);

  if (aCmd.HasOption ("help"))
  {
    theDi.PrintHelp (theArgVec[0]);
    return 0;
  }

  ViewerTest_DoubleMapOfInteractiveAndName& aMapAIS = GetMapOfAIS();

  TCollection_AsciiString aName (aCmd.Arg (ViewerTest_CmdParser::THE_UNNAMED_COMMAND_OPTION_KEY, 0).c_str());

  if (aName.IsEmpty())
  {
    Message::SendFail ("Syntax error: please specify AIS manipulator's name as the first argument");
    return 1;
  }

  // ----------------------------------
  // detach existing manipulator object
  // ----------------------------------

  if (aCmd.HasOption ("detach"))
  {
    if (!aMapAIS.IsBound2 (aName))
    {
      Message::SendFail() << "Syntax error: could not find \"" << aName << "\" AIS object";
      return 1;
    }

    Handle(AIS_Manipulator) aManipulator = Handle(AIS_Manipulator)::DownCast (aMapAIS.Find2 (aName));
    if (aManipulator.IsNull())
    {
      Message::SendFail() << "Syntax error: \"" << aName << "\" is not an AIS manipulator";
      return 1;
    }

    aManipulator->Detach();
    aMapAIS.UnBind2 (aName);
    ViewerTest::GetAISContext()->Remove (aManipulator, Standard_True);

    return 0;
  }

  // -----------------------------------------------
  // find or create manipulator if it does not exist
  // -----------------------------------------------

  Handle(AIS_Manipulator) aManipulator;
  if (!aMapAIS.IsBound2 (aName))
  {
    std::cout << theArgVec[0] << ": AIS object \"" << aName << "\" has been created.\n";

    aManipulator = new AIS_Manipulator();
    aManipulator->SetModeActivationOnDetection (true);
    aMapAIS.Bind (aManipulator, aName);
  }
  else
  {
    aManipulator = Handle(AIS_Manipulator)::DownCast (aMapAIS.Find2 (aName));
    if (aManipulator.IsNull())
    {
      Message::SendFail() << "Syntax error: \"" << aName << "\" is not an AIS manipulator";
      return 1;
    }
  }

  // -----------------------------------------
  // change properties of manipulator instance
  // -----------------------------------------

  if (aCmd.HasOption ("autoActivate", 1, Standard_True))
  {
    aManipulator->SetModeActivationOnDetection (aCmd.ArgBool ("autoActivate"));
  }
  if (aCmd.HasOption ("followTranslation", 1, Standard_True))
  {
    aManipulator->ChangeTransformBehavior().SetFollowTranslation (aCmd.ArgBool ("followTranslation"));
  }
  if (aCmd.HasOption ("followRotation", 1, Standard_True))
  {
    aManipulator->ChangeTransformBehavior().SetFollowRotation (aCmd.ArgBool ("followRotation"));
  }
  if (aCmd.HasOption("followDragging", 1, Standard_True))
  {
    aManipulator->ChangeTransformBehavior().SetFollowDragging(aCmd.ArgBool("followDragging"));
  }
  if (aCmd.HasOption ("gap", 1, Standard_True))
  {
    aManipulator->SetGap (aCmd.ArgFloat ("gap"));
  }
  if (aCmd.HasOption ("part", 3, Standard_True))
  {
    Standard_Integer anAxis = aCmd.ArgInt  ("part", 0);
    Standard_Integer aMode  = aCmd.ArgInt  ("part", 1);
    Standard_Boolean aOnOff = aCmd.ArgBool ("part", 2);
    if (aMode < 1 || aMode > 4)
    {
      Message::SendFail ("Syntax error: mode value should be in range [1, 4]");
      return 1;
    }

    aManipulator->SetPart (anAxis, static_cast<AIS_ManipulatorMode> (aMode), aOnOff);
  }
  if (aCmd.HasOption("parts", 2, Standard_True))
  {
    Standard_Integer aMode = aCmd.ArgInt("parts", 0);
    Standard_Boolean aOnOff = aCmd.ArgBool("parts", 1);
    if (aMode < 1 || aMode > 4)
    {
      Message::SendFail ("Syntax error: mode value should be in range [1, 4]");
      return 1;
    }

    aManipulator->SetPart(static_cast<AIS_ManipulatorMode>(aMode), aOnOff);
  }
  if (aCmd.HasOption ("pos", 3, Standard_True))
  {
    gp_Pnt aLocation = aCmd.ArgPnt ("pos", 0);
    gp_Dir aVDir     = aCmd.HasOption ("pos", 6) ? gp_Dir (aCmd.ArgVec ("pos", 3)) : aManipulator->Position().Direction();
    gp_Dir aXDir     = aCmd.HasOption ("pos", 9) ? gp_Dir (aCmd.ArgVec ("pos", 6)) : aManipulator->Position().XDirection();

    aManipulator->SetPosition (gp_Ax2 (aLocation, aVDir, aXDir));
  }
  if (aCmd.HasOption ("size", 1, Standard_True))
  {
    aManipulator->SetSize (aCmd.ArgFloat ("size"));
  }
  if (aCmd.HasOption ("zoomable", 1, Standard_True))
  {
    aManipulator->SetZoomPersistence (!aCmd.ArgBool ("zoomable"));

    if (ViewerTest::GetAISContext()->IsDisplayed (aManipulator))
    {
      ViewerTest::GetAISContext()->Remove  (aManipulator, Standard_False);
      ViewerTest::GetAISContext()->Display (aManipulator, Standard_False);
    }
  }

  // ---------------------------------------------------
  // attach, detach or access manipulator from an object
  // ---------------------------------------------------

  if (aCmd.HasOption ("attach"))
  {
    // Find an object and attach manipulator to it
    if (!aCmd.HasOption ("attach", 1, Standard_True))
    {
      return 1;
    }

    TCollection_AsciiString anObjName (aCmd.Arg ("attach", 0).c_str());
    Handle(AIS_InteractiveObject) anObject;
    if (!aMapAIS.Find2 (anObjName, anObject))
    {
      Message::SendFail() << "Syntax error: AIS object \"" << anObjName << "\" does not exist";
      return 1;
    }

    for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter (aMapAIS);
         anIter.More(); anIter.Next())
    {
      Handle(AIS_Manipulator) aManip = Handle(AIS_Manipulator)::DownCast (anIter.Key1());
      if (!aManip.IsNull()
       && aManip->IsAttached()
       && aManip->Object() == anObject)
      {
        Message::SendFail() << "Syntax error: AIS object \"" << anObjName << "\" already has manipulator";
        return 1;
      }
    }

    AIS_Manipulator::OptionsForAttach anOptions;
    if (aCmd.HasOption ("adjustPosition", 1, Standard_True))
    {
      anOptions.SetAdjustPosition (aCmd.ArgBool ("adjustPosition"));
    }
    if (aCmd.HasOption ("adjustSize", 1, Standard_True))
    {
      anOptions.SetAdjustSize (aCmd.ArgBool ("adjustSize"));
    }
    if (aCmd.HasOption ("enableModes", 1, Standard_True))
    {
      anOptions.SetEnableModes (aCmd.ArgBool ("enableModes"));
    }

    aManipulator->Attach (anObject, anOptions);

    // Check view option
    if (aCmd.HasOption ("view"))
    {
      if (!aCmd.HasOption ("view", 1, Standard_True))
      {
        return 1;
      }
      TCollection_AsciiString aViewString (aCmd.Arg ("view", 0).c_str());
      Handle(V3d_View) aView;
      if (aViewString.IsEqual ("active"))
      {
        aView = ViewerTest::CurrentView();
      }
      else // Check view name
      {
        ViewerTest_Names aViewNames (aViewString);
        if (!ViewerTest_myViews.IsBound1 (aViewNames.GetViewName()))
        {
          Message::SendFail() << "Syntax error: wrong view name '" << aViewString << "'";
          return 1;
        }
        aView = ViewerTest_myViews.Find1 (aViewNames.GetViewName());
        if (aView.IsNull())
        {
          Message::SendFail() << "Syntax error: cannot find view with name '" << aViewString << "'";
          return 1;
        }
      }
      for (NCollection_DoubleMap <TCollection_AsciiString, Handle(V3d_View)>::Iterator
        anIter (ViewerTest_myViews); anIter.More(); anIter.Next())
      {
        ViewerTest::GetAISContext()->SetViewAffinity (aManipulator, anIter.Value(), Standard_False);
      }
      ViewerTest::GetAISContext()->SetViewAffinity (aManipulator, aView, Standard_True);
    }
  }

  // --------------------------------------
  // apply transformation using manipulator
  // --------------------------------------

  if (aCmd.HasOption ("startTransform", 2, Standard_True))
  {
    aManipulator->StartTransform (aCmd.ArgInt ("startTransform", 0), aCmd.ArgInt ("startTransform", 1), ViewerTest::CurrentView());
  }
  if (aCmd.HasOption ("transform", 2, Standard_True))
  {
    aManipulator->Transform (aCmd.ArgInt ("transform", 0), aCmd.ArgInt ("transform", 1), ViewerTest::CurrentView());
  }
  if (aCmd.HasOption ("stopTransform"))
  {
    Standard_Boolean toApply = !aCmd.HasOption ("stopTransform", 1) || (aCmd.Arg ("stopTransform", 0) != "abort");

    aManipulator->StopTransform (toApply);
  }

  gp_Trsf aT;
  if (aCmd.HasOption ("move", 3, Standard_True))
  {
    aT.SetTranslationPart (aCmd.ArgVec ("move"));
  }
  if (aCmd.HasOption ("rotate", 7, Standard_True))
  {
    aT.SetRotation (gp_Ax1 (aCmd.ArgPnt ("rotate", 0), aCmd.ArgVec ("rotate", 3)), aCmd.ArgDouble ("rotate", 6));
  }
  if (aCmd.HasOption ("scale", 1))
  {
    aT.SetScale (gp_Pnt(), aCmd.ArgDouble("scale"));
  }

  if (aT.Form() != gp_Identity)
  {
    aManipulator->Transform (aT);
  }

  ViewerTest::GetAISContext()->Redisplay (aManipulator, Standard_True);

  return 0;
}

//===============================================================================================
//function : VSelectionProperties
//purpose  :
//===============================================================================================
static int VSelectionProperties (Draw_Interpretor& theDi,
                                 Standard_Integer  theArgsNb,
                                 const char**      theArgVec)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  if (TCollection_AsciiString (theArgVec[0]) == "vhighlightselected")
  {
    // handle obsolete alias
    bool toEnable = true;
    if (theArgsNb < 2)
    {
      theDi << (aCtx->ToHilightSelected() ? "on" : "off");
      return 0;
    }
    else if (theArgsNb != 2
         || !Draw::ParseOnOff (theArgVec[1], toEnable))
    {
      Message::SendFail ("Syntax error: wrong number of parameters");
      return 1;
    }
    if (toEnable != aCtx->ToHilightSelected())
    {
      aCtx->ClearDetected();
      aCtx->SetToHilightSelected (toEnable);
    }
    return 0;
  }

  Standard_Boolean toPrint  = theArgsNb == 1;
  Standard_Boolean toRedraw = Standard_False;
  Standard_Integer anArgIter = 1;
  Prs3d_TypeOfHighlight aType = Prs3d_TypeOfHighlight_None;
  if (anArgIter < theArgsNb)
  {
    TCollection_AsciiString anArgFirst (theArgVec[anArgIter]);
    anArgFirst.LowerCase();
    ++anArgIter;
    if (anArgFirst == "dynhighlight"
     || anArgFirst == "dynhilight"
     || anArgFirst == "dynamichighlight"
     || anArgFirst == "dynamichilight")
    {
      aType = Prs3d_TypeOfHighlight_Dynamic;
    }
    else if (anArgFirst == "localdynhighlight"
          || anArgFirst == "localdynhilight"
          || anArgFirst == "localdynamichighlight"
          || anArgFirst == "localdynamichilight")
    {
      aType = Prs3d_TypeOfHighlight_LocalDynamic;
    }
    else if (anArgFirst == "selhighlight"
          || anArgFirst == "selhilight"
          || anArgFirst == "selectedhighlight"
          || anArgFirst == "selectedhilight")
    {
      aType = Prs3d_TypeOfHighlight_Selected;
    }
    else if (anArgFirst == "localselhighlight"
          || anArgFirst == "localselhilight"
          || anArgFirst == "localselectedhighlight"
          || anArgFirst == "localselectedhilight")
    {
      aType = Prs3d_TypeOfHighlight_LocalSelected;
    }
    else
    {
      --anArgIter;
    }
  }
  for (; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-help")
    {
      theDi.PrintHelp (theArgVec[0]);
      return 0;
    }
    else if (anArg == "-print")
    {
      toPrint = Standard_True;
    }
    else if (anArg == "-autoactivate")
    {
      Standard_Boolean toEnable = Standard_True;
      if (anArgIter + 1 < theArgsNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      aCtx->SetAutoActivateSelection (toEnable);
    }
    else if (anArg == "-automatichighlight"
          || anArg == "-automatichilight"
          || anArg == "-autohighlight"
          || anArg == "-autohilight")
    {
      Standard_Boolean toEnable = Standard_True;
      if (anArgIter + 1 < theArgsNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      aCtx->ClearSelected (false);
      aCtx->ClearDetected();
      aCtx->SetAutomaticHilight (toEnable);
      toRedraw = true;
    }
    else if (anArg == "-highlightselected"
          || anArg == "-hilightselected")
    {
      Standard_Boolean toEnable = Standard_True;
      if (anArgIter + 1 < theArgsNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      aCtx->ClearDetected();
      aCtx->SetToHilightSelected (toEnable);
      toRedraw = true;
    }
    else if (anArg == "-pickstrategy"
          || anArg == "-pickingstrategy")
    {
      if (++anArgIter >= theArgsNb)
      {
        Message::SendFail ("Syntax error: type of highlighting is undefined");
        return 1;
      }

      SelectMgr_PickingStrategy aStrategy = SelectMgr_PickingStrategy_FirstAcceptable;
      TCollection_AsciiString aVal (theArgVec[anArgIter]);
      aVal.LowerCase();
      if (aVal == "first"
       || aVal == "firstaccepted"
       || aVal == "firstacceptable")
      {
        aStrategy = SelectMgr_PickingStrategy_FirstAcceptable;
      }
      else if (aVal == "topmost"
            || aVal == "onlyTopmost")
      {
        aStrategy = SelectMgr_PickingStrategy_OnlyTopmost;
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown picking strategy '" << aVal << "'";
        return 1;
      }

      aCtx->SetPickingStrategy (aStrategy);
    }
    else if (anArg == "-pixtol"
          && anArgIter + 1 < theArgsNb)
    {
      aCtx->SetPixelTolerance (Draw::Atoi (theArgVec[++anArgIter]));
    }
    else if (anArg == "-preferclosest")
    {
      bool toPreferClosest = true;
      if (anArgIter + 1 < theArgsNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toPreferClosest))
      {
        ++anArgIter;
      }
      aCtx->MainSelector()->SetPickClosest (toPreferClosest);
    }
    else if ((anArg == "-depthtol"
           || anArg == "-depthtolerance")
          && anArgIter + 1 < theArgsNb)
    {
      TCollection_AsciiString aTolType (theArgVec[++anArgIter]);
      aTolType.LowerCase();
      if (aTolType == "uniform")
      {
        if (anArgIter + 1 >= theArgsNb)
        {
          Message::SendFail() << "Syntax error: wrong number of arguments";
          return 1;
        }
        aCtx->MainSelector()->SetDepthTolerance (SelectMgr_TypeOfDepthTolerance_Uniform,
                                                 Draw::Atof (theArgVec[++anArgIter]));
      }
      else if (aTolType == "uniformpx")
      {
        if (anArgIter + 1 >= theArgsNb)
        {
          Message::SendFail() << "Syntax error: wrong number of arguments";
          return 1;
        }
        aCtx->MainSelector()->SetDepthTolerance (SelectMgr_TypeOfDepthTolerance_UniformPixels,
                                                 Draw::Atof (theArgVec[++anArgIter]));
      }
      else if (aTolType == "sensfactor")
      {
        aCtx->MainSelector()->SetDepthTolerance (SelectMgr_TypeOfDepthTolerance_SensitivityFactor, 0.0);
      }
      else
      {
        Message::SendFail() << "Syntax error at '" << aTolType << "'";
        return 1;
      }
    }
    else if ((anArg == "-mode"
           || anArg == "-dispmode")
          && anArgIter + 1 < theArgsNb)
    {
      if (aType == Prs3d_TypeOfHighlight_None)
      {
        Message::SendFail ("Syntax error: type of highlighting is undefined");
        return 1;
      }

      const Standard_Integer aDispMode = Draw::Atoi (theArgVec[++anArgIter]);
      const Handle(Prs3d_Drawer)& aStyle = aCtx->HighlightStyle (aType);
      aStyle->SetDisplayMode (aDispMode);
      toRedraw = Standard_True;
    }
    else if (anArg == "-layer"
          && anArgIter + 1 < theArgsNb)
    {
      if (aType == Prs3d_TypeOfHighlight_None)
      {
        Message::SendFail ("Syntax error: type of highlighting is undefined");
        return 1;
      }

      ++anArgIter;
      Graphic3d_ZLayerId aNewLayer = Graphic3d_ZLayerId_UNKNOWN;
      if (!ViewerTest::ParseZLayer (theArgVec[anArgIter], aNewLayer))
      {
        Message::SendFail() << "Syntax error at " << theArgVec[anArgIter];
        return 1;
      }

      const Handle(Prs3d_Drawer)& aStyle = aCtx->HighlightStyle (aType);
      aStyle->SetZLayer (aNewLayer);
      toRedraw = Standard_True;
    }
    else if (anArg == "-hicolor"
          || anArg == "-selcolor"
          || anArg == "-color")
    {
      if (anArg.StartsWith ("-hi"))
      {
        aType = Prs3d_TypeOfHighlight_Dynamic;
      }
      else if (anArg.StartsWith ("-sel"))
      {
        aType = Prs3d_TypeOfHighlight_Selected;
      }
      else if (aType == Prs3d_TypeOfHighlight_None)
      {
        Message::SendFail ("Syntax error: type of highlighting is undefined");
        return 1;
      }

      Quantity_Color aColor;
      Standard_Integer aNbParsed = Draw::ParseColor (theArgsNb - anArgIter - 1,
                                                     theArgVec + anArgIter + 1,
                                                     aColor);
      if (aNbParsed == 0)
      {
        Message::SendFail ("Syntax error: need more arguments");
        return 1;
      }
      anArgIter += aNbParsed;

      const Handle(Prs3d_Drawer)& aStyle = aCtx->HighlightStyle (aType);
      aStyle->SetColor (aColor);
      toRedraw = Standard_True;
    }
    else if ((anArg == "-transp"
           || anArg == "-transparency"
           || anArg == "-hitransp"
           || anArg == "-seltransp"
           || anArg == "-hitransplocal"
           || anArg == "-seltransplocal")
          && anArgIter + 1 < theArgsNb)
    {
      if (anArg.StartsWith ("-hi"))
      {
        aType = Prs3d_TypeOfHighlight_Dynamic;
      }
      else if (anArg.StartsWith ("-sel"))
      {
        aType = Prs3d_TypeOfHighlight_Selected;
      }
      else if (aType == Prs3d_TypeOfHighlight_None)
      {
        Message::SendFail ("Syntax error: type of highlighting is undefined");
        return 1;
      }

      const Standard_Real aTransp = Draw::Atof (theArgVec[++anArgIter]);
      const Handle(Prs3d_Drawer)& aStyle = aCtx->HighlightStyle (aType);
      aStyle->SetTransparency ((Standard_ShortReal )aTransp);
      toRedraw = Standard_True;
    }
    else if ((anArg == "-mat"
           || anArg == "-material")
          && anArgIter + 1 < theArgsNb)
    {
      if (aType == Prs3d_TypeOfHighlight_None)
      {
        Message::SendFail ("Syntax error: type of highlighting is undefined");
        return 1;
      }

      const Handle(Prs3d_Drawer)& aStyle = aCtx->HighlightStyle (aType);
      Graphic3d_NameOfMaterial aMatName = Graphic3d_MaterialAspect::MaterialFromName (theArgVec[anArgIter + 1]);
      if (aMatName != Graphic3d_NameOfMaterial_DEFAULT)
      {
        ++anArgIter;
        Handle(Graphic3d_AspectFillArea3d) anAspect = new Graphic3d_AspectFillArea3d();
        *anAspect = *aCtx->DefaultDrawer()->ShadingAspect()->Aspect();
        Graphic3d_MaterialAspect aMat (aMatName);
        aMat.SetColor (aStyle->Color());
        aMat.SetTransparency (aStyle->Transparency());
        anAspect->SetFrontMaterial (aMat);
        anAspect->SetInteriorColor (aStyle->Color());
        aStyle->SetBasicFillAreaAspect (anAspect);
      }
      else
      {
        aStyle->SetBasicFillAreaAspect (Handle(Graphic3d_AspectFillArea3d)());
      }
      toRedraw = Standard_True;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (toPrint)
  {
    const Handle(Prs3d_Drawer)& aHiStyle  = aCtx->HighlightStyle();
    const Handle(Prs3d_Drawer)& aSelStyle = aCtx->SelectionStyle();
    theDi << "Auto-activation                : " << (aCtx->GetAutoActivateSelection() ? "On" : "Off") << "\n";
    theDi << "Auto-highlight                 : " << (aCtx->AutomaticHilight() ? "On" : "Off") << "\n";
    theDi << "Highlight selected             : " << (aCtx->ToHilightSelected() ? "On" : "Off") << "\n";
    theDi << "Selection pixel tolerance      : " << aCtx->MainSelector()->PixelTolerance() << "\n";
    theDi << "Selection color                : " << Quantity_Color::StringName (aSelStyle->Color().Name()) << "\n";
    theDi << "Dynamic highlight color        : " << Quantity_Color::StringName (aHiStyle->Color().Name()) << "\n";
    theDi << "Selection transparency         : " << aSelStyle->Transparency() << "\n";
    theDi << "Dynamic highlight transparency : " << aHiStyle->Transparency() << "\n";
    theDi << "Selection mode                 : " << aSelStyle->DisplayMode() << "\n";
    theDi << "Dynamic highlight mode         : " << aHiStyle->DisplayMode() << "\n";
    theDi << "Selection layer                : " << aSelStyle->ZLayer() << "\n";
    theDi << "Dynamic layer                  : " << aHiStyle->ZLayer() << "\n";
  }

  if (aCtx->NbSelected() != 0 && toRedraw)
  {
    aCtx->HilightSelected (Standard_True);
  }

  return 0;
}

//===============================================================================================
//function : VDumpSelectionImage
//purpose  :
//===============================================================================================
static int VDumpSelectionImage (Draw_Interpretor& /*theDi*/,
                                Standard_Integer  theArgsNb,
                                const char**      theArgVec)
{
  if (theArgsNb < 2)
  {
    Message::SendFail() << "Syntax error: wrong number arguments for '" << theArgVec[0] << "'";
    return 1;
  }

  const Handle(AIS_InteractiveContext)& aContext = ViewerTest::GetAISContext();
  const Handle(V3d_View)& aView = ViewerTest::CurrentView();
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  TCollection_AsciiString aFile;
  StdSelect_TypeOfSelectionImage aType = StdSelect_TypeOfSelectionImage_NormalizedDepth;
  Handle(Graphic3d_Camera) aCustomCam;
  Image_Format anImgFormat = Image_Format_BGR;
  Standard_Integer aPickedIndex = 1;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString aParam (theArgVec[anArgIter]);
    aParam.LowerCase();
    if (aParam == "-type")
    {
      if (++anArgIter >= theArgsNb)
      {
        Message::SendFail ("Syntax error: wrong number parameters of flag '-depth'");
        return 1;
      }

      TCollection_AsciiString aValue (theArgVec[anArgIter]);
      aValue.LowerCase();
      if (aValue == "depth"
       || aValue == "normdepth"
       || aValue == "normalizeddepth")
      {
        aType       = StdSelect_TypeOfSelectionImage_NormalizedDepth;
        anImgFormat = Image_Format_GrayF;
      }
      if (aValue == "depthinverted"
       || aValue == "normdepthinverted"
       || aValue == "normalizeddepthinverted"
       || aValue == "inverted")
      {
        aType       = StdSelect_TypeOfSelectionImage_NormalizedDepthInverted;
        anImgFormat = Image_Format_GrayF;
      }
      else if (aValue == "unnormdepth"
            || aValue == "unnormalizeddepth")
      {
        aType       = StdSelect_TypeOfSelectionImage_UnnormalizedDepth;
        anImgFormat = Image_Format_GrayF;
      }
      else if (aValue == "objectcolor"
            || aValue == "object"
            || aValue == "color")
      {
        aType = StdSelect_TypeOfSelectionImage_ColoredDetectedObject;
      }
      else if (aValue == "entitycolor"
            || aValue == "entity")
      {
        aType = StdSelect_TypeOfSelectionImage_ColoredEntity;
      }
      else if (aValue == "ownercolor"
            || aValue == "owner")
      {
        aType = StdSelect_TypeOfSelectionImage_ColoredOwner;
      }
      else if (aValue == "selectionmodecolor"
            || aValue == "selectionmode"
            || aValue == "selmodecolor"
            || aValue == "selmode")
      {
        aType = StdSelect_TypeOfSelectionImage_ColoredSelectionMode;
      }
    }
    else if (aParam == "-picked"
          || aParam == "-pickeddepth"
          || aParam == "-pickedindex")
    {
      if (++anArgIter >= theArgsNb)
      {
        Message::SendFail() << "Syntax error: wrong number parameters at '" << aParam << "'";
        return 1;
      }

      aPickedIndex = Draw::Atoi (theArgVec[anArgIter]);
    }
    else if (anArgIter + 1 < theArgsNb
          && aParam == "-xrpose")
    {
      TCollection_AsciiString anXRArg (theArgVec[++anArgIter]);
      anXRArg.LowerCase();
      if (anXRArg == "base")
      {
        aCustomCam = aView->View()->BaseXRCamera();
      }
      else if (anXRArg == "head")
      {
        aCustomCam = aView->View()->PosedXRCamera();
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown XR pose '" << anXRArg << "'";
        return 1;
      }
      if (aCustomCam.IsNull())
      {
        Message::SendFail() << "Error: undefined XR pose";
        return 0;
      }
    }
    else if (aFile.IsEmpty())
    {
      aFile = theArgVec[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }
  if (aFile.IsEmpty())
  {
    Message::SendFail ("Syntax error: image file name is missing");
    return 1;
  }

  Standard_Integer aWidth = 0, aHeight = 0;
  aView->Window()->Size (aWidth, aHeight);

  Image_AlienPixMap aPixMap;
  if (!aPixMap.InitZero (anImgFormat, aWidth, aHeight))
  {
    Message::SendFail ("Error: can't allocate image");
    return 1;
  }

  const bool wasImmUpdate = aView->SetImmediateUpdate (false);
  Handle(Graphic3d_Camera) aCamBack = aView->Camera();
  if (!aCustomCam.IsNull())
  {
    aView->SetCamera (aCustomCam);
  }
  if (!aContext->MainSelector()->ToPixMap (aPixMap, aView, aType, aPickedIndex))
  {
    Message::SendFail ("Error: can't generate selection image");
    return 1;
  }
  if (!aCustomCam.IsNull())
  {
    aView->SetCamera (aCamBack);
  }
  aView->SetImmediateUpdate (wasImmUpdate);

  if (!aPixMap.Save (aFile))
  {
    Message::SendFail ("Error: can't save selection image");
    return 0;
  }
  return 0;
}

//===============================================================================================
//function : VViewCube
//purpose  :
//===============================================================================================
static int VViewCube (Draw_Interpretor& ,
                      Standard_Integer  theNbArgs,
                      const char**      theArgVec)
{
  const Handle(AIS_InteractiveContext)& aContext = ViewerTest::GetAISContext();
  const Handle(V3d_View)& aView = ViewerTest::CurrentView();
  if (aContext.IsNull() || aView.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }
  else if (theNbArgs < 2)
  {
    Message::SendFail ("Syntax error: wrong number arguments");
    return 1;
  }

  Handle(AIS_ViewCube) aViewCube;
  ViewerTest_AutoUpdater anUpdateTool (aContext, aView);
  Quantity_Color aColorRgb;
  TCollection_AsciiString aName;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      //
    }
    else if (aViewCube.IsNull())
    {
      aName = theArgVec[anArgIter];
      if (aName.StartsWith ("-"))
      {
        Message::SendFail ("Syntax error: object name should be specified");
        return 1;
      }
      Handle(AIS_InteractiveObject) aPrs;
      GetMapOfAIS().Find2 (aName, aPrs);
      aViewCube = Handle(AIS_ViewCube)::DownCast (aPrs);
      if (aViewCube.IsNull())
      {
        aViewCube = new AIS_ViewCube();
        aViewCube->SetBoxColor (Quantity_NOC_GRAY50);
        aViewCube->SetViewAnimation (ViewerTest::CurrentEventManager()->ViewAnimation());
        aViewCube->SetFixedAnimationLoop (false);
      }
    }
    else if (anArg == "-reset")
    {
      aViewCube->ResetStyles();
    }
    else if (anArg == "-color"
          || anArg == "-boxcolor"
          || anArg == "-boxsidecolor"
          || anArg == "-sidecolor"
          || anArg == "-boxedgecolor"
          || anArg == "-edgecolor"
          || anArg == "-boxcornercolor"
          || anArg == "-cornercolor"
          || anArg == "-innercolor"
          || anArg == "-textcolor")
    {
      Standard_Integer aNbParsed = Draw::ParseColor (theNbArgs - anArgIter - 1,
                                                     theArgVec + anArgIter + 1,
                                                     aColorRgb);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }
      anArgIter += aNbParsed;
      if (anArg == "-boxcolor")
      {
        aViewCube->SetBoxColor (aColorRgb);
      }
      else if (anArg == "-boxsidecolor"
            || anArg == "-sidecolor")
      {
        aViewCube->BoxSideStyle()->SetColor (aColorRgb);
        aViewCube->SynchronizeAspects();
      }
      else if (anArg == "-boxedgecolor"
            || anArg == "-edgecolor")
      {
        aViewCube->BoxEdgeStyle()->SetColor (aColorRgb);
        aViewCube->SynchronizeAspects();
      }
      else if (anArg == "-boxcornercolor"
            || anArg == "-cornercolor")
      {
        aViewCube->BoxCornerStyle()->SetColor (aColorRgb);
        aViewCube->SynchronizeAspects();
      }
      else if (anArg == "-innercolor")
      {
        aViewCube->SetInnerColor (aColorRgb);
      }
      else if (anArg == "-textcolor")
      {
        aViewCube->SetTextColor (aColorRgb);
      }
      else
      {
        aViewCube->SetColor (aColorRgb);
      }
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-transparency"
           || anArg == "-boxtransparency"))
    {
      const Standard_Real aValue = Draw::Atof (theArgVec[++anArgIter]);
      if (aValue < 0.0 || aValue > 1.0)
      {
        Message::SendFail() << "Syntax error: invalid transparency value " << theArgVec[anArgIter];
        return 1;
      }

      if (anArg == "-boxtransparency")
      {
        aViewCube->SetBoxTransparency (aValue);
      }
      else
      {
        aViewCube->SetTransparency (aValue);
      }
    }
    else if (anArg == "-axes"
          || anArg == "-edges"
          || anArg == "-vertices"
          || anArg == "-vertexes"
          || anArg == "-fixedanimation")
    {
      bool toShow = true;
      if (anArgIter + 1 < theNbArgs
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toShow))
      {
        ++anArgIter;
      }
      if (anArg == "-fixedanimation")
      {
        aViewCube->SetFixedAnimationLoop (toShow);
      }
      else if (anArg == "-axes")
      {
        aViewCube->SetDrawAxes (toShow);
      }
      else if (anArg == "-edges")
      {
        aViewCube->SetDrawEdges (toShow);
      }
      else
      {
        aViewCube->SetDrawVertices (toShow);
      }
    }
    else if (anArg == "-yup"
          || anArg == "-zup")
    {
      bool isOn = true;
      if (anArgIter + 1 < theNbArgs
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], isOn))
      {
        ++anArgIter;
      }
      if (anArg == "-yup")
      {
        aViewCube->SetYup (isOn);
      }
      else
      {
        aViewCube->SetYup (!isOn);
      }
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-font")
    {
      aViewCube->SetFont (theArgVec[++anArgIter]);
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-fontheight")
    {
      aViewCube->SetFontHeight (Draw::Atof (theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-size"
           || anArg == "-boxsize"))
    {
      aViewCube->SetSize (Draw::Atof (theArgVec[++anArgIter]),
                          anArg != "-boxsize");
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-boxfacet"
           || anArg == "-boxfacetextension"
           || anArg == "-facetextension"
           || anArg == "-extension"))
    {
      aViewCube->SetBoxFacetExtension (Draw::Atof (theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-boxedgegap"
           || anArg == "-edgegap"))
    {
      aViewCube->SetBoxEdgeGap (Draw::Atof (theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-boxedgeminsize"
           || anArg == "-edgeminsize"))
    {
      aViewCube->SetBoxEdgeMinSize (Draw::Atof (theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-boxcornerminsize"
           || anArg == "-cornerminsize"))
    {
      aViewCube->SetBoxCornerMinSize (Draw::Atof (theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-axespadding")
    {
      aViewCube->SetAxesPadding (Draw::Atof (theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-roundradius")
    {
      aViewCube->SetRoundRadius (Draw::Atof (theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-duration")
    {
      aViewCube->SetDuration (Draw::Atof (theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-axesradius")
    {
      aViewCube->SetAxesRadius (Draw::Atof (theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-axesconeradius")
    {
      aViewCube->SetAxesConeRadius (Draw::Atof (theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-axessphereradius")
    {
      aViewCube->SetAxesSphereRadius (Draw::Atof (theArgVec[++anArgIter]));
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << anArg << "'";
      return 1;
    }
  }
  if (aViewCube.IsNull())
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }

  ViewerTest::Display (aName, aViewCube, false);
  return 0;
}

//===============================================================================================
//function : VColorConvert
//purpose  :
//===============================================================================================
static int VColorConvert (Draw_Interpretor& theDI, Standard_Integer  theNbArgs, const char** theArgVec)
{
  if (theNbArgs != 6)
  {
    std::cerr << "Error: command syntax is incorrect, see help" << std::endl;
    return 1;
  }

  Standard_Boolean convertFrom = (! strcasecmp (theArgVec[1], "from"));
  if (! convertFrom && strcasecmp (theArgVec[1], "to"))
  {
    std::cerr << "Error: first argument must be either \"to\" or \"from\"" << std::endl;
    return 1;
  }

  const char* aTypeStr = theArgVec[2];
  Quantity_TypeOfColor aType = Quantity_TOC_RGB;
  if (! strcasecmp (aTypeStr, "srgb"))
  {
    aType = Quantity_TOC_sRGB;
  }
  else if (! strcasecmp (aTypeStr, "hls"))
  {
    aType = Quantity_TOC_HLS;
  }
  else if (! strcasecmp (aTypeStr, "lab"))
  {
    aType = Quantity_TOC_CIELab;
  }
  else if (! strcasecmp (aTypeStr, "lch"))
  {
    aType = Quantity_TOC_CIELch;
  }
  else
  {
    std::cerr << "Error: unknown colorspace type: " << aTypeStr << std::endl;
    return 1;
  }

  double aC1 = Draw::Atof (theArgVec[3]);
  double aC2 = Draw::Atof (theArgVec[4]);
  double aC3 = Draw::Atof (theArgVec[5]);

  Quantity_Color aColor (aC1, aC2, aC3, convertFrom ? aType : Quantity_TOC_RGB);
  aColor.Values (aC1, aC2, aC3, convertFrom ? Quantity_TOC_RGB : aType);

  // print values with 6 decimal digits
  char buffer[1024];
  Sprintf (buffer, "%.6f %.6f %.6f", aC1, aC2, aC3);
  theDI << buffer;

  return 0;
}
 
//===============================================================================================
//function : VColorDiff
//purpose  :
//===============================================================================================
static int VColorDiff (Draw_Interpretor& theDI, Standard_Integer  theNbArgs, const char** theArgVec)
{
  if (theNbArgs != 7)
  {
    std::cerr << "Error: command syntax is incorrect, see help" << std::endl;
    return 1;
  }

  double aR1 = Draw::Atof (theArgVec[1]);
  double aG1 = Draw::Atof (theArgVec[2]);
  double aB1 = Draw::Atof (theArgVec[3]);
  double aR2 = Draw::Atof (theArgVec[4]);
  double aG2 = Draw::Atof (theArgVec[5]);
  double aB2 = Draw::Atof (theArgVec[6]);

  Quantity_Color aColor1 (aR1, aG1, aB1, Quantity_TOC_RGB);
  Quantity_Color aColor2 (aR2, aG2, aB2, Quantity_TOC_RGB);

  theDI << aColor1.DeltaE2000 (aColor2);

  return 0;
}
 
//===============================================================================================
//function : VBVHPrebuid
//purpose  :
//===============================================================================================
static int VSelBvhBuild (Draw_Interpretor& /*theDI*/, Standard_Integer theNbArgs, const char** theArgVec)
{
  const Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  if (theNbArgs < 2)
  {
    Message::SendFail ("Error: command syntax is incorrect, see help");
    return 1;
  }

  Standard_Integer toEnable = -1;
  Standard_Integer aThreadsNb = -1;
  Standard_Boolean toWait = Standard_False;

  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();

    if (anArg == "-nbthreads"
        && anArgIter + 1 < theNbArgs)
    {
      aThreadsNb = Draw::Atoi (theArgVec[++anArgIter]);
      if (aThreadsNb < 1)
      {
        aThreadsNb = Max (1, OSD_Parallel::NbLogicalProcessors() - 1);
      }
    }
    else if (anArg == "-wait")
    {
      toWait = Standard_True;
    }
    else if (toEnable == -1)
    {
      Standard_Boolean toEnableValue = Standard_True;
      if (Draw::ParseOnOff (anArg.ToCString(), toEnableValue))
      {
        toEnable = toEnableValue ? 1 : 0;
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown argument '" << anArg << "'";
        return 1;
      }
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << anArg << "'";
      return 1;
    }
  }

  if (aThreadsNb == -1)
  {
    aThreadsNb = 1;
  }
  if (toEnable != -1)
  {
    aCtx->MainSelector()->SetToPrebuildBVH (toEnable == 1, aThreadsNb);
  }
  if (toWait)
  {
    aCtx->MainSelector()->WaitForBVHBuild();
  }

  return 0;
}

//=======================================================================
//function : ViewerCommands
//purpose  :
//=======================================================================

void ViewerTest::ViewerCommands(Draw_Interpretor& theCommands)
{

  const char *group = "ZeViewer";
  theCommands.Add("vinit",
          "vinit [-name viewName] [-left leftPx] [-top topPx] [-width widthPx] [-height heightPx]"
    "\n\t\t:     [-exitOnClose] [-closeOnEscape] [-cloneActive] [-2d_mode {on|off}=off]"
  #if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
    "\n\t\t:     [-display displayName]"
  #endif
    "\n\t\t: Creates new View window with specified name viewName."
    "\n\t\t: By default the new view is created in the viewer and in"
    "\n\t\t: graphic driver shared with active view."
    "\n\t\t:  -name {driverName/viewerName/viewName | viewerName/viewName | viewName}"
    "\n\t\t: If driverName isn't specified the driver will be shared with active view."
    "\n\t\t: If viewerName isn't specified the viewer will be shared with active view."
#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
    "\n\t\t:  -display HostName.DisplayNumber[:ScreenNumber]"
    "\n\t\t: Display name will be used within creation of graphic driver, when specified."
#endif
    "\n\t\t:  -left,  -top    pixel position of left top corner of the window."
    "\n\t\t:  -width, -height width and heigth of window respectively."
    "\n\t\t:  -cloneActive floag to copy camera and dimensions of active view."
    "\n\t\t:  -exitOnClose when specified, closing the view will exit application."
    "\n\t\t:  -closeOnEscape when specified, view will be closed on pressing Escape."
    "\n\t\t:  -2d_mode when on, view will not react on rotate scene events"
    "\n\t\t: Additional commands for operations with views: vclose, vactivate, vviewlist.",
    __FILE__,VInit,group);
  theCommands.Add("vclose" ,
    "[view_id [keep_context=0|1]]\n"
    "or vclose ALL - to remove all created views\n"
    " - removes view(viewer window) defined by its view_id.\n"
    " - keep_context: by default 0; if 1 and the last view is deleted"
    " the current context is not removed.",
    __FILE__,VClose,group);
  theCommands.Add("vactivate" ,
    "vactivate view_id [-noUpdate]"
    " - activates view(viewer window) defined by its view_id",
    __FILE__,VActivate,group);
  theCommands.Add("vviewlist",
    "vviewlist [format={tree, long}]"
    " - prints current list of views per viewer and graphic_driver ID shared between viewers"
    " - format: format of result output, if tree the output is a tree view;"
    "otherwise it's a list of full view names. By default format = tree",
    __FILE__,VViewList,group);
  theCommands.Add("vhelp" ,
    "vhelp            : display help on the viewer commands",
    __FILE__,VHelp,group);
  theCommands.Add("vviewproj",
          "vviewproj [top|bottom|left|right|front|back|axoLeft|axoRight]"
    "\n\t\t:         [+-X+-Y+-Z] [-Zup|-Yup] [-frame +-X+-Y]"
    "\n\t\t: Setup view direction"
    "\n\t\t:   -Yup      use Y-up convention instead of Zup (which is default)."
    "\n\t\t:   +-X+-Y+-Z define direction as combination of DX, DY and DZ;"
    "\n\t\t:             for example '+Z' will show front of the model,"
    "\n\t\t:             '-X-Y+Z' will define left axonometrical view."
    "\n\t\t:   -frame    define camera Up and Right directions (regardless Up convention);"
    "\n\t\t:             for example '+X+Z' will show front of the model with Z-up."
    __FILE__,VViewProj,group);
  theCommands.Add("vtop" ,
    "vtop or <T>      : Top view. Orientation +X+Y" ,
    __FILE__,VViewProj,group);
  theCommands.Add("vbottom" ,
    "vbottom          : Bottom view. Orientation +X-Y" ,
    __FILE__,VViewProj,group);
  theCommands.Add("vleft" ,
    "vleft            : Left view. Orientation -Y+Z" ,
    __FILE__,VViewProj,group);
  theCommands.Add("vright" ,
    "vright           : Right view. Orientation +Y+Z" ,
    __FILE__,VViewProj,group);
  theCommands.Add("vaxo" ,
    " vaxo or <A>     : Axonometric view. Orientation +X-Y+Z",
    __FILE__,VViewProj,group);
  theCommands.Add("vfront" ,
    "vfront           : Front view. Orientation +X+Z" ,
    __FILE__,VViewProj,group);
  theCommands.Add("vback" ,
    "vback            : Back view. Orientation -X+Z" ,
    __FILE__,VViewProj,group);
  theCommands.Add("vpick" ,
    "vpick           : vpick X Y Z [shape subshape] ( all variables as string )",
    VPick,group);
  theCommands.Add("vfit",
    "vfit or <F> [-selected] [-noupdate]"
    "\n\t\t: [-selected] fits the scene according to bounding box of currently selected objects",
    __FILE__,VFit,group);
  theCommands.Add ("vfitarea",
    "vfitarea x1 y1 x2 y2"
    "\n\t\t: vfitarea x1 y1 z1 x2 y2 z2"
    "\n\t\t: Fit view to show area located between two points"
    "\n\t\t: given in world 2D or 3D corrdinates.",
    __FILE__, VFitArea, group);
  theCommands.Add ("vzfit", "vzfit [scale]\n"
    "   Matches Z near, Z far view volume planes to the displayed objects.\n"
    "   \"scale\" - specifies factor to scale computed z range.\n",
    __FILE__, VZFit, group);
  theCommands.Add("vrepaint",
            "vrepaint [-immediate] [-continuous FPS]"
    "\n\t\t: force redraw of active View"
    "\n\t\t:   -immediate  flag performs redraw of immediate layers only;"
    "\n\t\t:   -continuous activates/deactivates continuous redraw of active View,"
    "\n\t\t:                0 means no continuous rendering,"
    "\n\t\t:               -1 means non-stop redraws,"
    "\n\t\t:               >0 specifies target framerate,",
    __FILE__,VRepaint,group);
  theCommands.Add("vclear",
    "vclear          : vclear"
    "\n\t\t: remove all the object from the viewer",
    __FILE__,VClear,group);
  theCommands.Add (
    "vbackground",
    "Changes background or some background settings.\n"
    "\n"
    "Usage:\n"
    "  vbackground -imageFile ImageFile [-imageMode FillType]\n"
    "  vbackground -imageMode FillType\n"
    "  vbackground -gradient Color1 Color2 [-gradientMode FillMethod]\n"
    "  vbackground -gradientMode FillMethod\n"
    "  vbackground -cubemap CubemapFile1 [CubeMapFiles2-5] [-order TilesIndexes1-6] [-invertedz]\n"
    "  vbackground -color Color\n"
    "  vbackground -default -gradient Color1 Color2 [-gradientMode FillType]\n"
    "  vbackground -default -color Color\n"
    "  vbackground -help\n"
    "\n"
    "Options:\n"
    "  -imageFile    (-imgFile, -image, -img):             sets filename of image used as background\n"
    "  -imageMode    (-imgMode, -imageMd, -imgMd):         sets image fill type\n"
    "  -gradient     (-grad, -gr):                         sets background gradient starting and ending colors\n"
    "  -gradientMode (-gradMode, -gradMd, -grMode, -grMd): sets gradient fill method\n"
    "  -cubemap      (-cmap, -cm):                         sets environmet cubemap as background\n"
    "  -invertedz    (-invz, -iz):                         sets inversion of Z axis for background cubemap rendering\n"
    "  -order        (-o):                                 defines order of tiles in one image cubemap\n"
    "                                                      (has no effect in case of multi image cubemaps)\n"
    "  -color        (-col):                               sets background color\n"
    "  -default      (-def):                               sets background default gradient or color\n"
    "  -help         (-h):                                 outputs short help message\n"
    "\n"
    "Arguments:\n"
    "  Color:        Red Green Blue  - where Red, Green, Blue must be integers within the range [0, 255]\n"
    "                                  or reals within the range [0.0, 1.0]\n"
    "                ColorName       - one of WHITE, BLACK, RED, GREEN, BLUE, etc.\n"
    "                #HHH, [#]HHHHHH - where H is a hexadecimal digit (0 .. 9, a .. f, or A .. F)\n"
    "  FillMethod:   one of NONE, HOR[IZONTAL], VER[TICAL], DIAG[ONAL]1, DIAG[ONAL]2, CORNER1, CORNER2, CORNER3, "
    "CORNER4\n"
    "  FillType:     one of CENTERED, TILED, STRETCH, NONE\n"
    "  ImageFile:    a name of the file with the image used as a background\n"
    "  CubemapFilei: a name of the file with one image packed cubemap or names of separate files with every cubemap side\n"
    "  TileIndexi:   a cubemap side index in range [0, 5] for i tile of one image packed cubemap\n",
    __FILE__,
    vbackground,
    group);
  theCommands.Add ("vsetbg",
                   "Loads image as background."
                   "\n\t\t: vsetbg ImageFile [FillType]"
                   "\n\t\t: vsetbg -imageFile ImageFile [-imageMode FillType]"
                   "\n\t\t: Alias for 'vbackground -imageFile ImageFile [-imageMode FillType]'.",
                   __FILE__,
                   vbackground,
                   group);
  theCommands.Add ("vsetbgmode",
                   "Changes background image fill type."
                   "\n\t\t: vsetbgmode [-imageMode] FillType"
                   "\n\t\t: Alias for 'vbackground -imageMode FillType'.",
                   __FILE__,
                   vbackground,
                   group);
  theCommands.Add ("vsetgradientbg",
                   "Mounts gradient background."
                   "\n\t\t: vsetgradientbg Color1 Color2 [FillMethod]"
                   "\n\t\t: vsetgradientbg -gradient Color1 Color2 [-gradientMode FillMethod]"
                   "\n\t\t: Alias for 'vbackground -gradient Color1 Color2 -gradientMode FillMethod'.",
                   __FILE__,
                   vbackground,
                   group);
  theCommands.Add ("vsetgrbgmode",
                   "Changes gradient background fill method."
                   "\n\t\t: vsetgrbgmode [-gradientMode] FillMethod"
                   "\n\t\t: Alias for 'vbackground -gradientMode FillMethod'.",
                   __FILE__,
                   vbackground,
                   group);
  theCommands.Add ("vsetcolorbg",
                   "Sets background color."
                   "\n\t\t: vsetcolorbg [-color] Color."
                   "\n\t\t: Alias for 'vbackground -color Color'.",
                   __FILE__,
                   vbackground,
                   group);
  theCommands.Add ("vsetdefaultbg",
                   "Sets default viewer background fill color (flat/gradient)."
                   "\n\t\t: vsetdefaultbg Color1 Color2 [FillMethod]"
                   "\n\t\t: vsetdefaultbg -gradient Color1 Color2 [-gradientMode FillMethod]"
                   "\n\t\t: Alias for 'vbackground -default -gradient Color1 Color2 [-gradientMode FillMethod]'."
                   "\n\t\t: vsetdefaultbg [-color] Color"
                   "\n\t\t: Alias for 'vbackground -default -color Color'.",
                   __FILE__,
                   vbackground,
                   group);
  theCommands.Add("vscale",
    "vscale          : vscale X Y Z",
    __FILE__,VScale,group);
  theCommands.Add("vzbufftrihedron",
            "vzbufftrihedron [{-on|-off}=-on] [-type {wireframe|zbuffer}=zbuffer]"
    "\n\t\t:       [-position center|left_lower|left_upper|right_lower|right_upper]"
    "\n\t\t:       [-scale value=0.1] [-size value=0.8] [-arrowDiam value=0.05]"
    "\n\t\t:       [-colorArrowX color=RED] [-colorArrowY color=GREEN] [-colorArrowZ color=BLUE]"
    "\n\t\t:       [-nbfacets value=12] [-colorLabels color=WHITE]"
    "\n\t\t: Displays a trihedron",
    __FILE__,VZBuffTrihedron,group);
  theCommands.Add("vrotate",
    "vrotate [[-mouseStart X Y] [-mouseMove X Y]]|[AX AY AZ [X Y Z]]"
    "\n                : Option -mouseStart starts rotation according to the mouse position"
    "\n                : Option -mouseMove continues rotation with angle computed"
    "\n                : from last and new mouse position."
    "\n                : vrotate AX AY AZ [X Y Z]",
    __FILE__,VRotate,group);
  theCommands.Add("vzoom",
    "vzoom           : vzoom coef",
    __FILE__,VZoom,group);
  theCommands.Add("vpan",
    "vpan            : vpan dx dy",
    __FILE__,VPan,group);
  theCommands.Add("vcolorscale",
    "vcolorscale name [-noupdate|-update] [-demo]"
    "\n\t\t:       [-range RangeMin=0 RangeMax=1 NbIntervals=10]"
    "\n\t\t:       [-font HeightFont=20]"
    "\n\t\t:       [-logarithmic {on|off}=off] [-reversed {on|off}=off]"
    "\n\t\t:       [-smoothTransition {on|off}=off]"
    "\n\t\t:       [-hueRange MinAngle=230 MaxAngle=0]"
    "\n\t\t:       [-colorRange MinColor=BLUE1 MaxColor=RED]"
    "\n\t\t:       [-textpos {left|right|center|none}=right]"
    "\n\t\t:       [-labelAtBorder {on|off}=on]"
    "\n\t\t:       [-colors Color1 Color2 ...] [-color Index Color]"
    "\n\t\t:       [-labels Label1 Label2 ...] [-label Index Label]"
    "\n\t\t:       [-freeLabels NbOfLabels Label1 Label2 ...]"
    "\n\t\t:       [-xy Left=0 Bottom=0]"
    "\n\t\t:       [-uniform lightness hue_from hue_to]"
    "\n\t\t:  -demo     - displays a color scale with demonstratio values"
    "\n\t\t:  -colors   - set colors for all intervals"
    "\n\t\t:  -color    - set color for specific interval"
    "\n\t\t:  -uniform  - generate colors with the same lightness"
    "\n\t\t:  -textpos  - horizontal label position relative to color scale bar"
    "\n\t\t:  -labelAtBorder - vertical label position relative to color interval;"
    "\n\t\t:              at border means the value inbetween neighbor intervals,"
    "\n\t\t:              at center means the center value within current interval"
    "\n\t\t:  -labels   - set labels for all intervals"
    "\n\t\t:  -freeLabels - same as -labels but does not require"
    "\n\t\t:              matching the number of intervals"
    "\n\t\t:  -label    - set label for specific interval"
    "\n\t\t:  -title    - set title"
    "\n\t\t:  -reversed - setup smooth color transition between intervals"
    "\n\t\t:  -smoothTransition - swap colorscale direction"
    "\n\t\t:  -hueRange - set hue angles corresponding to minimum and maximum values",
    __FILE__, VColorScale, group);
  theCommands.Add("vgraduatedtrihedron",
    "vgraduatedtrihedron : -on/-off [-xname Name] [-yname Name] [-zname Name] [-arrowlength Value]\n"
    "\t[-namefont Name] [-valuesfont Name]\n"
    "\t[-xdrawname on/off] [-ydrawname on/off] [-zdrawname on/off]\n"
    "\t[-xnameoffset IntVal] [-ynameoffset IntVal] [-znameoffset IntVal]"
    "\t[-xnamecolor Color] [-ynamecolor Color] [-znamecolor Color]\n"
    "\t[-xdrawvalues on/off] [-ydrawvalues on/off] [-zdrawvalues on/off]\n"
    "\t[-xvaluesoffset IntVal] [-yvaluesoffset IntVal] [-zvaluesoffset IntVal]"
    "\t[-xcolor Color] [-ycolor Color] [-zcolor Color]\n"
    "\t[-xdrawticks on/off] [-ydrawticks on/off] [-zdrawticks on/off]\n"
    "\t[-xticks Number] [-yticks Number] [-zticks Number]\n"
    "\t[-xticklength IntVal] [-yticklength IntVal] [-zticklength IntVal]\n"
    "\t[-drawgrid on/off] [-drawaxes on/off]\n"
    " - Displays or erases graduated trihedron"
    " - xname, yname, zname - names of axes, default: X, Y, Z\n"
    " - namefont - font of axes names. Default: Arial\n"
    " - xnameoffset, ynameoffset, znameoffset - offset of name from values or tickmarks or axis. Default: 30\n"
    " - xnamecolor, ynamecolor, znamecolor - colors of axes names\n"
    " - xvaluesoffset, yvaluesoffset, zvaluesoffset - offset of values from tickmarks or axis. Default: 10\n"
    " - valuesfont - font of axes values. Default: Arial\n"
    " - xcolor, ycolor, zcolor - color of axis and values\n"
    " - xticks, yticks, xzicks - number of tickmark on axes. Default: 5\n"
    " - xticklength, yticklength, xzicklength - length of tickmark on axes. Default: 10\n",
    __FILE__,VGraduatedTrihedron,group);
  theCommands.Add("vtile" ,
            "vtile [-totalSize W H] [-lowerLeft X Y] [-upperLeft X Y] [-tileSize W H]"
    "\n\t\t: Setup view to draw a tile (a part of virtual bigger viewport)."
    "\n\t\t:  -totalSize the size of virtual bigger viewport"
    "\n\t\t:  -tileSize  tile size (the view size will be used if omitted)"
    "\n\t\t:  -lowerLeft tile offset as lower left corner"
    "\n\t\t:  -upperLeft tile offset as upper left corner",
    __FILE__, VTile, group);
  theCommands.Add("vzlayer",
              "vzlayer [layerId]"
      "\n\t\t:         [-add|-delete|-get|-settings] [-insertBefore AnotherLayer] [-insertAfter AnotherLayer]"
      "\n\t\t:         [-origin X Y Z] [-cullDist Distance] [-cullSize Size]"
      "\n\t\t:         [-enable|-disable {depthTest|depthWrite|depthClear|depthoffset}]"
      "\n\t\t:         [-enable|-disable {positiveOffset|negativeOffset|textureenv|rayTracing}]"
      "\n\t\t: ZLayer list management:"
      "\n\t\t:   -add      add new z layer to viewer and print its id"
      "\n\t\t:   -insertBefore add new z layer and insert it before existing one"
      "\n\t\t:   -insertAfter  add new z layer and insert it after  existing one"
      "\n\t\t:   -delete   delete z layer"
      "\n\t\t:   -get      print sequence of z layers"
      "\n\t\t:   -settings print status of z layer settings"
      "\n\t\t:   -disable  disables given setting"
      "\n\t\t:   -enable   enables  given setting",
    __FILE__,VZLayer,group);
  theCommands.Add("vlayerline",
    "vlayerline : vlayerline x1 y1 x2 y2 [linewidth=0.5] [linetype=0] [transparency=1.0]",
    __FILE__,VLayerLine,group);
  theCommands.Add("vgrid",
              "vgrid [off] [-type {rect|circ}] [-mode {line|point}] [-origin X Y] [-rotAngle Angle] [-zoffset DZ]"
      "\n\t\t:       [-step X Y] [-size DX DY]"
      "\n\t\t:       [-step StepRadius NbDivisions] [-radius Radius]",
    __FILE__, VGrid, group);
  theCommands.Add ("vpriviledgedplane",
    "vpriviledgedplane [Ox Oy Oz Nx Ny Nz [Xx Xy Xz]]"
    "\n\t\t:   Ox, Oy, Oz - plane origin"
    "\n\t\t:   Nx, Ny, Nz - plane normal direction"
    "\n\t\t:   Xx, Xy, Xz - plane x-reference axis direction"
    "\n\t\t: Sets or prints viewer's priviledged plane geometry.",
    __FILE__, VPriviledgedPlane, group);
  theCommands.Add ("vconvert",
    "vconvert v [Mode={window|view}]"
    "\n\t\t: vconvert x y [Mode={window|view|grid|ray}]"
    "\n\t\t: vconvert x y z [Mode={window|grid}]"
    "\n\t\t:   window - convert to window coordinates, pixels"
    "\n\t\t:   view   - convert to view projection plane"
    "\n\t\t:   grid   - convert to model coordinates, given on grid"
    "\n\t\t:   ray    - convert projection ray to model coordiantes"
    "\n\t\t: - vconvert v window : convert view to window;"
    "\n\t\t: - vconvert v view   : convert window to view;"
    "\n\t\t: - vconvert x y window : convert view to window;"
    "\n\t\t: - vconvert x y view : convert window to view;"
    "\n\t\t: - vconvert x y : convert window to model;"
    "\n\t\t: - vconvert x y grid : convert window to model using grid;"
    "\n\t\t: - vconvert x y ray : convert window projection line to model;"
    "\n\t\t: - vconvert x y z window : convert model to window;"
    "\n\t\t: - vconvert x y z grid : convert view to model using grid;"
    "\n\t\t: Converts the given coordinates to window/view/model space.",
    __FILE__, VConvert, group);
  theCommands.Add ("vfps",
    "vfps [framesNb=100] [-duration seconds] : estimate average frame rate for active view",
    __FILE__, VFps, group);
  theCommands.Add ("vgldebug",
            "vgldebug [-sync {0|1}] [-debug {0|1}] [-glslWarn {0|1}]"
    "\n\t\t:          [-glslCode {off|short|full}] [-extraMsg {0|1}] [{0|1}]"
    "\n\t\t: Request debug GL context. Should be called BEFORE vinit."
    "\n\t\t: Debug context can be requested only on Windows"
    "\n\t\t: with GL_ARB_debug_output extension implemented by GL driver!"
    "\n\t\t:  -sync     - request synchronized debug GL context"
    "\n\t\t:  -glslWarn - log GLSL compiler/linker warnings,"
    "\n\t\t:              which are suppressed by default,"
    "\n\t\t:  -glslCode - log GLSL program source code,"
    "\n\t\t:              which are suppressed by default,"
    "\n\t\t:  -extraMsg - log extra diagnostic messages from GL context,"
    "\n\t\t:              which are suppressed by default",
    __FILE__, VGlDebug, group);
  theCommands.Add ("vvbo",
    "vvbo [{0|1}] : turn VBO usage On/Off; affects only newly displayed objects",
    __FILE__, VVbo, group);
  theCommands.Add ("vstereo",
            "vstereo [0|1] [-mode Mode] [-reverse {0|1}]"
    "\n\t\t:         [-mirrorComposer] [-hmdfov2d AngleDegrees] [-unitFactor MetersFactor]"
    "\n\t\t:         [-anaglyph Filter]"
    "\n\t\t: Control stereo output mode."
    "\n\t\t: When -mirrorComposer is specified, VR rendered frame will be mirrored in window (debug)."
    "\n\t\t: Parameter -unitFactor specifies meters scale factor for mapping VR input."
    "\n\t\t: Available modes for -mode:"
    "\n\t\t:  quadBuffer        - OpenGL QuadBuffer stereo,"
    "\n\t\t:                     requires driver support."
    "\n\t\t:                     Should be called BEFORE vinit!"
    "\n\t\t:  anaglyph         - Anaglyph glasses"
    "\n\t\t:  rowInterlaced    - row-interlaced display"
    "\n\t\t:  columnInterlaced - column-interlaced display"
    "\n\t\t:  chessBoard       - chess-board output"
    "\n\t\t:  sideBySide       - horizontal pair"
    "\n\t\t:  overUnder        - vertical   pair"
    "\n\t\t:  openVR           - OpenVR (HMD)"
    "\n\t\t: Available Anaglyph filters for -anaglyph:"
    "\n\t\t:  redCyan, redCyanSimple, yellowBlue, yellowBlueSimple,"
    "\n\t\t:  greenMagentaSimple",
    __FILE__, VStereo, group);
  theCommands.Add ("vcaps",
            "vcaps [-sRGB {0|1}] [-vbo {0|1}] [-sprites {0|1}] [-ffp {0|1}] [-polygonMode {0|1}]"
    "\n\t\t:       [-compatibleProfile {0|1}] [-compressedTextures {0|1}]"
    "\n\t\t:       [-vsync {0|1}] [-useWinBuffer {0|1}]"
    "\n\t\t:       [-quadBuffer {0|1}] [-stereo {0|1}]"
    "\n\t\t:       [-softMode {0|1}] [-noupdate|-update]"
    "\n\t\t:       [-noExtensions {0|1}] [-maxVersion Major Minor]"
    "\n\t\t: Modify particular graphic driver options:"
    "\n\t\t:  sRGB     - enable/disable sRGB rendering"
    "\n\t\t:  FFP      - use fixed-function pipeline instead of"
    "\n\t\t:             built-in GLSL programs"
    "\n\t\t:            (requires compatible profile)"
    "\n\t\t:  polygonMode - use Polygon Mode instead of built-in GLSL programs"
    "\n\t\t:  compressedTexture - allow uploading of GPU-supported compressed texture formats"
    "\n\t\t:  VBO      - use Vertex Buffer Object (copy vertex"
    "\n\t\t:             arrays to GPU memory)"
    "\n\t\t:  sprite   - use textured sprites instead of bitmaps"
    "\n\t\t:  vsync    - switch VSync on or off"
    "\n\t\t:  winBuffer - allow using window buffer for rendering"
    "\n\t\t: Context creation options:"
    "\n\t\t:  softMode          - software OpenGL implementation"
    "\n\t\t:  compatibleProfile - backward-compatible profile"
    "\n\t\t:  quadbuffer        - QuadBuffer"
    "\n\t\t:  noExtensions      - disallow usage of extensions"
    "\n\t\t:  maxVersion        - force upper OpenGL version to be used"
    "\n\t\t: Unlike vrenderparams, these parameters control alternative"
    "\n\t\t: rendering paths producing the same visual result when"
    "\n\t\t: possible."
    "\n\t\t: Command is intended for testing old hardware compatibility.",
    __FILE__, VCaps, group);
  theCommands.Add ("vmemgpu",
    "vmemgpu [f]: print system-dependent GPU memory information if available;"
    " with f option returns free memory in bytes",
    __FILE__, VMemGpu, group);
  theCommands.Add ("vreadpixel",
    "vreadpixel xPixel yPixel [{rgb|rgba|sRGB|sRGBa|depth|hls|rgbf|rgbaf}=rgba] [-name|-hex]"
    " : Read pixel value for active view",
    __FILE__, VReadPixel, group);
  theCommands.Add("diffimage",
            "diffimage imageFile1 imageFile2 [diffImageFile]"
    "\n\t\t:           [-toleranceOfColor {0..1}=0] [-blackWhite {on|off}=off] [-borderFilter {on|off}=off]"
    "\n\t\t:           [-display viewName prsName1 prsName2 prsNameDiff] [-exitOnClose] [-closeOnEscape]"
    "\n\t\t: Compare two images by content and generate difference image."
    "\n\t\t: When -exitOnClose is specified, closing the view will exit application."
    "\n\t\t: When -closeOnEscape is specified, view will be closed on pressing Escape.",
    __FILE__, VDiffImage, group);
  theCommands.Add ("vselect",
    "vselect x1 y1 [x2 y2 [x3 y3 ... xn yn]] [-allowoverlap 0|1] [shift_selection = 0|1]\n"
    "- emulates different types of selection:\n"
    "- 1) single click selection\n"
    "- 2) selection with rectangle having corners at pixel positions (x1,y1) and (x2,y2)\n"
    "- 3) selection with polygon having corners in pixel positions (x1,y1), (x2,y2),...,(xn,yn)\n"
    "- 4) -allowoverlap manages overlap and inclusion detection in rectangular and polygonal selection.\n"
    "     If the flag is set to 1, both sensitives that were included completely and overlapped partially by defined \n"
    "     rectangle or polygon will be detected, otherwise algorithm will chose only fully included sensitives.\n"
    "     Default behavior is to detect only full inclusion. (partial inclusion - overlap - is not allowed by default)\n"
    "- 5) any of these selections with shift button pressed",
    __FILE__, VSelect, group);
  theCommands.Add ("vmoveto",
    "vmoveto [x y] [-reset]"
    "\n\t\t: Emulates cursor movement to pixel position (x,y)."
    "\n\t\t:   -reset resets current highlighting",
    __FILE__, VMoveTo, group);
  theCommands.Add ("vviewparams",
              "vviewparams [-args] [-scale [s]]"
      "\n\t\t:             [-eye [x y z]] [-at [x y z]] [-up [x y z]]"
      "\n\t\t:             [-proj [x y z]] [-center x y] [-size sx]"
      "\n\t\t: Manage current view parameters or prints all"
      "\n\t\t: current values when called without argument."
      "\n\t\t:   -scale [s]    prints or sets viewport relative scale"
      "\n\t\t:   -eye  [x y z] prints or sets eye location"
      "\n\t\t:   -at   [x y z] prints or sets center of look"
      "\n\t\t:   -up   [x y z] prints or sets direction of up vector"
      "\n\t\t:   -proj [x y z] prints or sets direction of look"
      "\n\t\t:   -center x y   sets location of center of the screen in pixels"
      "\n\t\t:   -size [sx]    prints viewport projection width and height sizes"
      "\n\t\t:                 or changes the size of its maximum dimension"
      "\n\t\t:   -args         prints vviewparams arguments for restoring current view",
    __FILE__, VViewParams, group);

  theCommands.Add("v2dmode",
    "v2dmode [-name viewName] [-mode {-on|-off}=-on]"
    "\n\t\t:   name   - name of existing view, if not defined, the active view is changed"
    "\n\t\t:   mode   - switches On/Off rotation mode"
    "\n\t\t: Set 2D mode of the active viewer manipulating. The following mouse and key actions are disabled:"
    "\n\t\t:   - rotation of the view by 3rd mouse button with Ctrl active"
    "\n\t\t:   - set view projection using key buttons: A/D/T/B/L/R for AXO, Reset, Top, Bottom, Left, Right"
    "\n\t\t: View camera position might be changed only by commands.",
    __FILE__, V2DMode, group);

  theCommands.Add("vanimation", "Alias for vanim",
    __FILE__, VAnimation, group);

  theCommands.Add("vanim",
            "List existing animations:"
    "\n\t\t:  vanim"
    "\n\t\t: Animation playback:"
    "\n\t\t:  vanim name -play|-resume [playFrom [playDuration]]"
    "\n\t\t:            [-speed Coeff] [-freeLook] [-lockLoop]"
    "\n\t\t:   -speed    playback speed (1.0 is normal speed)"
    "\n\t\t:   -freeLook skip camera animations"
    "\n\t\t:   -lockLoop disable any interactions"
    "\n\t\t:"
    "\n\t\t: Animation definition:"
    "\n\t\t:  vanim Name/sub/name [-clear] [-delete]"
    "\n\t\t:        [start TimeSec] [duration TimeSec]"
    "\n\t\t:"
    "\n\t\t: Animation name defined in path-style (anim/name or anim.name)"
    "\n\t\t: specifies nested animations."
    "\n\t\t: There is no syntax to explicitly add new animation,"
    "\n\t\t: and all non-existing animations within the name will be"
    "\n\t\t: implicitly created on first use (including parents)."
    "\n\t\t:"
    "\n\t\t: Each animation might define the SINGLE action (see below),"
    "\n\t\t: like camera transition, object transformation or custom callback."
    "\n\t\t: Child animations can be used for defining concurrent actions."
    "\n\t\t:"
    "\n\t\t: Camera animation:"
    "\n\t\t:  vanim name -view [-eye1 X Y Z] [-eye2 X Y Z]"
    "\n\t\t:                   [-at1  X Y Z] [-at2  X Y Z]"
    "\n\t\t:                   [-up1  X Y Z] [-up2  X Y Z]"
    "\n\t\t:                   [-scale1 Scale] [-scale2 Scale]"
    "\n\t\t:   -eyeX   camera Eye positions pair (start and end)"
    "\n\t\t:   -atX    camera Center positions pair"
    "\n\t\t:   -upX    camera Up directions pair"
    "\n\t\t:   -scaleX camera Scale factors pair"
    "\n\t\t: Object animation:"
    "\n\t\t:  vanim name -object [-loc1 X Y Z] [-loc2 X Y Z]"
    "\n\t\t:                     [-rot1 QX QY QZ QW] [-rot2 QX QY QZ QW]"
    "\n\t\t:                     [-scale1 Scale] [-scale2 Scale]"
    "\n\t\t:   -locX   object Location points pair (translation)"
    "\n\t\t:   -rotX   object Orientations pair (quaternions)"
    "\n\t\t:   -scaleX object Scale factors pair (quaternions)"
    "\n\t\t: Custom callback:"
    "\n\t\t:  vanim name -invoke \"Command Arg1 Arg2 %Pts %LocalPts %Normalized ArgN\""
    "\n\t\t:   %Pts        overall animation presentation timestamp"
    "\n\t\t:   %LocalPts   local animation timestamp"
    "\n\t\t:   %Normalized local animation normalized value in range 0..1"
    "\n\t\t:"
    "\n\t\t: Video recording:"
    "\n\t\t:  vanim name -record FileName [Width Height] [-fps FrameRate=24]"
    "\n\t\t:             [-format Format] [-vcodec Codec] [-pix_fmt PixelFormat]"
    "\n\t\t:             [-crf Value] [-preset Preset]"
    "\n\t\t:   -fps     video framerate"
    "\n\t\t:   -format  file format, container (matroska, etc.)"
    "\n\t\t:   -vcodec  video codec identifier (ffv1, mjpeg, etc.)"
    "\n\t\t:   -pix_fmt image pixel format (yuv420p, rgb24, etc.)"
    "\n\t\t:   -crf     constant rate factor (specific to codec)"
    "\n\t\t:   -preset  codec parameters preset (specific to codec)"
    __FILE__, VAnimation, group);

  theCommands.Add("vchangeselected",
    "vchangeselected shape"
    "- adds to shape to selection or remove one from it",
		__FILE__, VChangeSelected, group);
  theCommands.Add ("vnbselected",
    "vnbselected"
    "\n\t\t: Returns number of selected objects", __FILE__, VNbSelected, group);
  theCommands.Add ("vcamera",
              "vcamera [PrsName] [-ortho] [-projtype]"
      "\n\t\t:         [-persp]"
      "\n\t\t:         [-fovy   [Angle]] [-distance [Distance]]"
      "\n\t\t:         [-stereo] [-leftEye] [-rightEye]"
      "\n\t\t:         [-iod [Distance]] [-iodType    [absolute|relative]]"
      "\n\t\t:         [-zfocus [Value]] [-zfocusType [absolute|relative]]"
      "\n\t\t:         [-fov2d  [Angle]] [-lockZup {0|1}]"
      "\n\t\t:         [-xrPose base|head=base]"
      "\n\t\t: Manages camera parameters."
      "\n\t\t: Displays frustum when presntation name PrsName is specified."
      "\n\t\t: Prints current value when option called without argument."
      "\n\t\t: Orthographic camera:"
      "\n\t\t:   -ortho      activate orthographic projection"
      "\n\t\t: Perspective camera:"
      "\n\t\t:   -persp      activate perspective  projection (mono)"
      "\n\t\t:   -fovy       field of view in y axis, in degrees"
      "\n\t\t:   -fov2d      field of view limit for 2d on-screen elements"
      "\n\t\t:   -distance   distance of eye from camera center"
      "\n\t\t:   -lockZup    lock Z up (tunrtable mode)"
      "\n\t\t: Stereoscopic camera:"
      "\n\t\t:   -stereo     perspective  projection (stereo)"
      "\n\t\t:   -leftEye    perspective  projection (left  eye)"
      "\n\t\t:   -rightEye   perspective  projection (right eye)"
      "\n\t\t:   -iod        intraocular distance value"
      "\n\t\t:   -iodType    distance type, absolute or relative"
      "\n\t\t:   -zfocus     stereographic focus value"
      "\n\t\t:   -zfocusType focus type, absolute or relative",
    __FILE__, VCamera, group);
  theCommands.Add ("vautozfit", "command to enable or disable automatic z-range adjusting\n"
    "- vautozfit [on={1|0}] [scale]\n"
    "    Prints or changes parameters of automatic z-fit mode:\n"
    "   \"on\" - turns automatic z-fit on or off\n"
    "   \"scale\" - specifies factor to scale computed z range.\n",
    __FILE__, VAutoZFit, group);
  theCommands.Add ("vzrange", "command to manually access znear and zfar values\n"
    "   vzrange                - without parameters shows current values\n"
    "   vzrange [znear] [zfar] - applies provided values to view",
    __FILE__,VZRange, group);
  theCommands.Add ("vpurgedisplay",
    "vpurgedisplay"
    "- removes structures which don't belong to objects displayed in neutral point",
    __FILE__, VPurgeDisplay, group);
  theCommands.Add("vsetviewsize",
    "vsetviewsize size",
    __FILE__,VSetViewSize,group);
  theCommands.Add("vmoveview",
    "vmoveview Dx Dy Dz [Start = 1|0]",
    __FILE__,VMoveView,group);
  theCommands.Add("vtranslateview",
    "vtranslateview Dx Dy Dz [Start = 1|0)]",
    __FILE__,VTranslateView,group);
  theCommands.Add("vturnview",
    "vturnview Ax Ay Az [Start = 1|0]",
    __FILE__,VTurnView,group);
  theCommands.Add("vtextureenv",
    "Enables or disables environment mapping in the 3D view, loading the texture from the given standard "
    "or user-defined file and optionally applying texture mapping parameters\n"
    "                  Usage:\n"
    "                  vtextureenv off - disables environment mapping\n"
    "                  vtextureenv on {std_texture|texture_file_name} [rep mod flt ss st ts tt rot] - enables environment mapping\n"
    "                              std_texture = (0..7)\n"
    "                              rep         = {clamp|repeat}\n"
    "                              mod         = {decal|modulate}\n"
    "                              flt         = {nearest|bilinear|trilinear}\n"
    "                              ss, st      - scale factors for s and t texture coordinates\n"
    "                              ts, tt      - translation for s and t texture coordinates\n"
    "                              rot         - texture rotation angle in degrees",
    __FILE__, VTextureEnv, group);
  theCommands.Add("vhlr",
            "vhlr {on|off} [-showHidden={1|0}] [-algoType={algo|polyAlgo}] [-noupdate]"
      "\n\t\t: Hidden Line Removal algorithm."
      "\n\t\t:   -showHidden if set ON, hidden lines are drawn as dotted ones"
      "\n\t\t:   -algoType   type of HLR algorithm.\n",
    __FILE__,VHLR,group);
  theCommands.Add("vhlrtype",
              "vhlrtype {algo|polyAlgo} [shape_1 ... shape_n] [-noupdate]"
      "\n\t\t: Changes the type of HLR algorithm using for shapes:"
      "\n\t\t:   'algo' - exact HLR algorithm is applied"
      "\n\t\t:   'polyAlgo' - polygonal HLR algorithm is applied"
      "\n\t\t: If shapes are not given - option is applied to all shapes in the view",
    __FILE__,VHLRType,group);
  theCommands.Add("vclipplane",
              "vclipplane planeName [{0|1}]"
      "\n\t\t:   [-equation1 A B C D]"
      "\n\t\t:   [-equation2 A B C D]"
      "\n\t\t:   [-boxInterior MinX MinY MinZ MaxX MaxY MaxZ]"
      "\n\t\t:   [-set|-unset|-setOverrideGlobal [objects|views]]"
      "\n\t\t:   [-maxPlanes]"
      "\n\t\t:   [-capping {0|1}]"
      "\n\t\t:     [-color R G B] [-transparency Value] [-hatch {on|off|ID}]"
      "\n\t\t:     [-texName Texture] [-texScale SX SY] [-texOrigin TX TY]"
      "\n\t\t:       [-texRotate Angle]"
      "\n\t\t:     [-useObjMaterial {0|1}] [-useObjTexture {0|1}]"
      "\n\t\t:       [-useObjShader {0|1}]"
      "\n\t\t: Clipping planes management:"
      "\n\t\t:   -maxPlanes   print plane limit for view"
      "\n\t\t:   -delete      delete plane with given name"
      "\n\t\t:   {off|on|0|1} turn clipping on/off"
      "\n\t\t:   -set|-unset  set/unset plane for Object or View list;"
      "\n\t\t:                applied to active View when list is omitted"
      "\n\t\t:   -equation A B C D change plane equation"
      "\n\t\t:   -clone SourcePlane NewPlane clone the plane definition."
      "\n\t\t: Capping options:"
      "\n\t\t:   -capping {off|on|0|1} turn capping on/off"
      "\n\t\t:   -color R G B          set capping color"
      "\n\t\t:   -transparency Value   set capping transparency 0..1"
      "\n\t\t:   -texName Texture      set capping texture"
      "\n\t\t:   -texScale SX SY       set capping tex scale"
      "\n\t\t:   -texOrigin TX TY      set capping tex origin"
      "\n\t\t:   -texRotate Angle      set capping tex rotation"
      "\n\t\t:   -hatch {on|off|ID}    set capping hatching mask"
      "\n\t\t:   -useObjMaterial {off|on|0|1} use material of clipped object"
      "\n\t\t:   -useObjTexture  {off|on|0|1} use texture of clipped object"
      "\n\t\t:   -useObjShader   {off|on|0|1} use shader program of object",
      __FILE__, VClipPlane, group);
  theCommands.Add("vdefaults",
               "vdefaults [-absDefl value]"
       "\n\t\t:           [-devCoeff value]"
       "\n\t\t:           [-angDefl value]"
       "\n\t\t:           [-autoTriang {off/on | 0/1}]"
    , __FILE__, VDefaults, group);
  theCommands.Add("vlight",
    "tool to manage light sources, without arguments shows list of lights."
    "\n    Main commands: "
    "\n      '-clear' to clear lights"
    "\n      '-{def}aults' to load deafault lights"
    "\n      '-add' <type> to add any light source"
    "\n          where <type> is one of {amb}ient|directional|{spot}light|positional"
    "\n      'change' <lightId> to edit light source with specified lightId"
    "\n\n      In addition to 'add' and 'change' commands you can use light parameters:"
    "\n        -layer Id"
    "\n        -{pos}ition X Y Z"
    "\n        -{dir}ection X Y Z (for directional light or for spotlight)"
    "\n        -color colorName"
    "\n        -{head}light 0|1"
    "\n        -{sm}oothness value"
    "\n        -{int}ensity value"
    "\n        -{constAtten}uation value"
    "\n        -{linearAtten}uation value"
    "\n        -angle angleDeg"
    "\n        -{spotexp}onent value"
    "\n        -range value"
    "\n        -local|-global"
    "\n\n        example: vlight -add positional -head 1 -pos 0 1 1 -color red"
    "\n        example: vlight -change 0 -direction 0 -1 0 -linearAttenuation 0.2",
    __FILE__, VLight, group);
  theCommands.Add("vpbrenv",
    "vpbrenv -clear|-generate"
    "\n\t\t: Clears or generates PBR environment map of active view."
    "\n\t\t:  -clear clears PBR environment (fills by white color)"
    "\n\t\t:  -generate generates PBR environment from current background cubemap",
    __FILE__, VPBREnvironment, group);
  theCommands.Add("vraytrace",
            "vraytrace [0|1]"
    "\n\t\t: Turns on/off ray-tracing renderer."
    "\n\t\t:   'vraytrace 0' alias for 'vrenderparams -raster'."
    "\n\t\t:   'vraytrace 1' alias for 'vrenderparams -rayTrace'.",
    __FILE__, VRenderParams, group);
  theCommands.Add("vrenderparams",
    "\n\t\t: Manages rendering parameters, affecting visual appearance, quality and performance."
    "\n\t\t: Should be applied taking into account GPU hardware capabilities and performance."
    "\n\t\t: Common parameters:"
    "\n\t\t: vrenderparams [-raster] [-shadingModel {unlit|facet|gouraud|phong|pbr|pbr_facet}=gouraud]"
    "\n\t\t:               [-msaa 0..8=0] [-rendScale scale=1] [-resolution value=72]"
    "\n\t\t:               [-oit {off|0.0-1.0}=off]"
    "\n\t\t:               [-depthPrePass {on|off}=off] [-alphaToCoverage {on|off}=on]"
    "\n\t\t:               [-frustumCulling {on|off|noupdate}=on] [-lineFeather width=1.0]"
    "\n\t\t:               [-sync {default|views}] [-reset]"
    "\n\t\t:   -raster          Disables GPU ray-tracing."
    "\n\t\t:   -shadingModel    Controls shading model."
    "\n\t\t:   -msaa            Specifies number of samples for MSAA."
    "\n\t\t:   -rendScale       Rendering resolution scale factor (supersampling, alternative to MSAA)."
    "\n\t\t:   -resolution      Sets new pixels density (PPI) used as text scaling factor."
    "\n\t\t:   -lineFeather     Sets line feather factor while displaying mesh edges."
    "\n\t\t:   -alphaToCoverage Enables/disables alpha to coverage (needs MSAA)."
    "\n\t\t:   -oit             Enables/disables order-independent transparency (OIT) rendering;"
    "\n\t\t:                    weight OIT fixes transparency artifacts at the cost of blurry result,"
    "\n\t\t:                    it is managed by depth weight factor (0.0 value also enables weight OIT)."
    "\n\t\t:   -depthPrePass    Enables/disables depth pre-pass."
    "\n\t\t:   -frustumCulling  Enables/disables objects frustum clipping or"
    "\n\t\t:                    sets state to check structures culled previously."
    "\n\t\t:   -sync            Sets active View parameters as Viewer defaults / to other Views."
    "\n\t\t:   -reset           Resets active View parameters to Viewer defaults."
    "\n\t\t: Diagnostic output (on-screen overlay):"
    "\n\t\t: vrenderparams [-perfCounters none|fps|cpu|layers|structures|groups|arrays|triangles|points"
    "\n\t\t:                             |gpuMem|frameTime|basic|extended|full|nofps|skipImmediate]"
    "\n\t\t:               [-perfUpdateInterval nbSeconds=1] [-perfChart nbFrames=1] [-perfChartMax seconds=0.1]"
    "\n\t\t:   -perfCounters       Show/hide performance counters (flags can be combined)."
    "\n\t\t:   -perfUpdateInterval Performance counters update interval."
    "\n\t\t:   -perfChart          Show frame timers chart limited by specified number of frames."
    "\n\t\t:   -perfChartMax       Maximum time in seconds with the chart."
    "\n\t\t: Ray-Tracing options:"
    "\n\t\t: vrenderparams [-rayTrace] [-rayDepth {0..10}=3] [-shadows {on|off}=on] [-reflections {on|off}=off]"
    "\n\t\t:               [-fsaa {on|off}=off] [-gleam {on|off}=off] [-env {on|off}=off]"
    "\n\t\t:               [-gi {on|off}=off] [-brng {on|off}=off]"
    "\n\t\t:               [-iss {on|off}=off] [-tileSize {1..4096}=32] [-nbTiles {64..1024}=256]"
    "\n\t\t:               [-ignoreNormalMap {on|off}=off] [-twoSide {on|off}=off]"
    "\n\t\t:               [-maxRad {value>0}=30.0]"
    "\n\t\t:               [-aperture {value>=0}=0.0] [-focal {value>=0.0}=1.0]"
    "\n\t\t:               [-exposure value=0.0] [-whitePoint value=1.0] [-toneMapping {disabled|filmic}=disabled]"
    "\n\t\t:   -rayTrace     Enables  GPU ray-tracing."
    "\n\t\t:   -rayDepth     Defines maximum ray-tracing depth."
    "\n\t\t:   -shadows      Enables/disables shadows rendering."
    "\n\t\t:   -reflections  Enables/disables specular reflections."
    "\n\t\t:   -fsaa         Enables/disables adaptive anti-aliasing."
    "\n\t\t:   -gleam        Enables/disables transparency shadow effects."
    "\n\t\t:   -gi           Enables/disables global illumination effects (Path-Tracing)."
    "\n\t\t:   -env          Enables/disables environment map background."
    "\n\t\t:   -ignoreNormalMap Enables/disables normal map ignoring during path tracing."
    "\n\t\t:   -twoSide      Enables/disables two-sided BSDF models (PT mode)."
    "\n\t\t:   -iss          Enables/disables adaptive screen sampling (PT mode)."
    "\n\t\t:   -maxRad       Value used for clamping radiance estimation (PT mode)."
    "\n\t\t:   -tileSize     Specifies   size of screen tiles in ISS mode (32 by default)."
    "\n\t\t:   -nbTiles      Specifies number of screen tiles per Redraw in ISS mode (256 by default)."
    "\n\t\t:   -aperture     Aperture size  of perspective camera for depth-of-field effect (0 disables DOF)."
    "\n\t\t:   -focal        Focal distance of perspective camera for depth-of-field effect."
    "\n\t\t:   -exposure     Exposure value for tone mapping (0.0 value disables the effect)."
    "\n\t\t:   -whitePoint   White point value for filmic tone mapping."
    "\n\t\t:   -toneMapping  Tone mapping mode (disabled, filmic)."
    "\n\t\t: PBR environment baking parameters (advanced/debug):"
    "\n\t\t: vrenderparams [-pbrEnvPow2size {power>0}=9] [-pbrEnvSMLN {levels>1}=6] [-pbrEnvBP {0..1}=0.99]"
    "\n\t\t:               [-pbrEnvBDSN {samples>0}=1024] [-pbrEnvBSSN {samples>0}=256]"
    "\n\t\t:   -pbrEnvPow2size Controls size of IBL maps (real size can be calculates as 2^pbrenvpow2size)."
    "\n\t\t:   -pbrEnvSMLN     Controls number of mipmap levels used in specular IBL map."
    "\n\t\t:   -pbrEnvBDSN     Controls number of samples in Monte-Carlo integration during"
    "\n\t\t:                   diffuse IBL map's sherical harmonics calculation."
    "\n\t\t:   -pbrEnvBSSN     Controls maximum number of samples per mipmap level"
    "\n\t\t:                   in Monte-Carlo integration during specular IBL maps generation."
    "\n\t\t:   -pbrEnvBP       Controls strength of samples number reducing"
    "\n\t\t:                   during specular IBL maps generation (1 disables reducing)."
    "\n\t\t: Debug options:"
    "\n\t\t: vrenderparams [-issd {on|off}=off] [-rebuildGlsl on|off]"
    "\n\t\t:   -issd         Shows screen sampling distribution in ISS mode."
    "\n\t\t:   -rebuildGlsl  Rebuild Ray-Tracing GLSL programs (for debugging)."
    "\n\t\t:   -brng         Enables/disables blocked RNG (fast coherent PT).",
    __FILE__, VRenderParams, group);
  theCommands.Add("vstatprofiler",
    "\n vstatprofiler [fps|cpu|allLayers|layers|allstructures|structures|groups"
    "\n                |allArrays|fillArrays|lineArrays|pointArrays|textArrays"
    "\n                |triangles|points|geomMem|textureMem|frameMem"
    "\n                |elapsedFrame|cpuFrameAverage|cpuPickingAverage|cpuCullingAverage|cpuDynAverage"
    "\n                |cpuFrameMax|cpuPickingMax|cpuCullingMax|cpuDynMax]"
    "\n                [-noredraw]"
    "\n\t\t: Prints rendering statistics."
    "\n\t\t:   If there are some parameters - print corresponding statistic counters values,"
    "\n\t\t:   else - print all performance counters set previously."
    "\n\t\t:   '-noredraw' Flag to avoid additional redraw call and use already collected values.\n",
    __FILE__, VStatProfiler, group);
  theCommands.Add ("vplace",
            "vplace dx dy"
    "\n\t\t: Places the point (in pixels) at the center of the window",
    __FILE__, VPlace, group);
  theCommands.Add("vxrotate",
    "vxrotate",
    __FILE__,VXRotate,group);

    theCommands.Add("vmanipulator",
      "\n    vmanipulator Name [-attach AISObject | -detach | ...]"
      "\n    tool to create and manage AIS manipulators."
      "\n    Options: "
      "\n      '-attach AISObject'                 attach manipulator to AISObject"
      "\n      '-adjustPosition {0|1}'             adjust position when attaching"
      "\n      '-adjustSize     {0|1}'             adjust size when attaching"
      "\n      '-enableModes    {0|1}'             enable modes when attaching"
      "\n      '-view  {active | [name of view]}'  display manipulator only in defined view,"
      "\n                                          by default it is displayed in all views of the current viewer"
      "\n      '-detach'                           detach manipulator"
      "\n      '-startTransform mouse_x mouse_y' - invoke start of transformation"
      "\n      '-transform      mouse_x mouse_y' - invoke transformation"
      "\n      '-stopTransform  [abort]'         - invoke stop of transformation"
      "\n      '-move x y z'                     - move attached object"
      "\n      '-rotate x y z dx dy dz angle'    - rotate attached object"
      "\n      '-scale factor'                   - scale attached object"
      "\n      '-autoActivate      {0|1}'        - set activation on detection"
      "\n      '-followTranslation {0|1}'        - set following translation transform"
      "\n      '-followRotation    {0|1}'        - set following rotation transform"
      "\n      '-followDragging    {0|1}'        - set following dragging transform"
      "\n      '-gap value'                      - set gap between sub-parts"
      "\n      '-part axis mode    {0|1}'        - set visual part"
      "\n      '-parts axis mode   {0|1}'        - set visual part"
      "\n      '-pos x y z [nx ny nz [xx xy xz]' - set position of manipulator"
      "\n      '-size value'                     - set size of manipulator"
      "\n      '-zoomable {0|1}'                 - set zoom persistence",
    __FILE__, VManipulator, group);

  theCommands.Add("vselprops",
    "\n    vselprops [dynHighlight|localDynHighlight|selHighlight|localSelHighlight] [options]"
    "\n    Customizes selection and dynamic highlight parameters for the whole interactive context:"
    "\n    -autoActivate {0|1}     : disables|enables default computation and activation of global selection mode"
    "\n    -autoHighlight {0|1}    : disables|enables automatic highlighting in 3D Viewer"
    "\n    -highlightSelected {0|1}: disables|enables highlighting of detected object in selected state"
    "\n    -pickStrategy {first|topmost} : defines picking strategy"
    "\n                            'first'   to pick first acceptable (default)"
    "\n                            'topmost' to pick only topmost (and nothing, if topmost is rejected by filters)"
    "\n    -pixTol    value        : sets up pixel tolerance"
    "\n    -depthTol {uniform|uniformpx} value : sets tolerance for sorting results by depth"
    "\n    -depthTol {sensfactor}  : use sensitive factor for sorting results by depth"
    "\n    -preferClosest {0|1}    : sets if depth should take precedence over priority while sorting results"
    "\n    -dispMode  dispMode     : sets display mode for highlighting"
    "\n    -layer     ZLayer       : sets ZLayer for highlighting"
    "\n    -color     {name|r g b} : sets highlight color"
    "\n    -transp    value        : sets transparency coefficient for highlight"
    "\n    -material  material     : sets highlight material"
    "\n    -print                  : prints current state of all mentioned parameters",
    __FILE__, VSelectionProperties, group);
  theCommands.Add ("vhighlightselected",
                   "vhighlightselected [0|1]: alias for vselprops -highlightSelected.\n",
                   __FILE__, VSelectionProperties, group);

  theCommands.Add ("vseldump",
                   "vseldump file -type {depth|unnormDepth|object|owner|selMode|entity}=depth -pickedIndex Index=1"
                   "\n\t\t:       [-xrPose base|head=base]"
                   "\n\t\t: Generate an image based on detection results:"
                   "\n\t\t:   depth       normalized depth values"
                   "\n\t\t:   unnormDepth unnormalized depth values"
                   "\n\t\t:   object      color of detected object"
                   "\n\t\t:   owner       color of detected owner"
                   "\n\t\t:   selMode     color of selection mode"
                   "\n\t\t:   entity      color of etected entity",
                   __FILE__, VDumpSelectionImage, group);

  theCommands.Add ("vviewcube",
                   "vviewcube name"
                   "\n\t\t: Displays interactive view manipualtion object."
                   "\n\t\t: Options: "
                   "\n\t\t:   -reset                   reset geomertical and visual attributes'"
                   "\n\t\t:   -size Size               adapted size of View Cube"
                   "\n\t\t:   -boxSize Size            box size"
                   "\n\t\t:   -axes {0|1 }             show/hide axes (trihedron)"
                   "\n\t\t:   -edges {0|1}             show/hide edges of View Cube"
                   "\n\t\t:   -vertices {0|1}          show/hide vertices of View Cube"
                   "\n\t\t:   -Yup {0|1} -Zup {0|1}    set Y-up or Z-up view orientation"
                   "\n\t\t:   -color Color             color of View Cube"
                   "\n\t\t:   -boxColor Color          box color"
                   "\n\t\t:   -boxSideColor Color      box sides color"
                   "\n\t\t:   -boxEdgeColor Color      box edges color"
                   "\n\t\t:   -boxCornerColor Color    box corner color"
                   "\n\t\t:   -textColor Color         color of side text of view cube"
                   "\n\t\t:   -innerColor Color        inner box color"
                   "\n\t\t:   -transparency Value      transparency of object within [0, 1] range"
                   "\n\t\t:   -boxTransparency Value   transparency of box    within [0, 1] range"
                   "\n\t\t:   -font Name               font name"
                   "\n\t\t:   -fontHeight Value        font height"
                   "\n\t\t:   -boxFacetExtension Value box facet extension"
                   "\n\t\t:   -boxEdgeGap Value        gap between box edges and box sides"
                   "\n\t\t:   -boxEdgeMinSize Value    minimal box edge size"
                   "\n\t\t:   -boxCornerMinSize Value  minimal box corner size"
                   "\n\t\t:   -axesPadding Value       padding between box and arrows"
                   "\n\t\t:   -roundRadius Value       relative radius of corners of sides within [0.0, 0.5] range"
                   "\n\t\t:   -axesRadius Value        radius of axes of the trihedron"
                   "\n\t\t:   -axesConeRadius Value    radius of the cone (arrow) of the trihedron"
                   "\n\t\t:   -axesSphereRadius Value  radius of the sphere (central point) of trihedron"
                   "\n\t\t:   -fixedanimation {0|1}    uninterruptible animation loop"
                   "\n\t\t:   -duration Seconds        animation duration in seconds",
    __FILE__, VViewCube, group);

  theCommands.Add("vcolorconvert" ,
                  "vcolorconvert {from|to} type C1 C2 C2"
                  "\n\t\t: vcolorconvert from type C1 C2 C2: Converts color from specified color space to linear RGB"
                  "\n\t\t: vcolorconvert to type R G B: Converts linear RGB color to specified color space"
                  "\n\t\t: type can be sRGB, HLS, Lab, or Lch",
                  __FILE__,VColorConvert,group);
  theCommands.Add("vcolordiff" ,
                  "vcolordiff R1 G1 B1 R2 G2 B2: returns CIEDE2000 color difference between two RGB colors",
                  __FILE__,VColorDiff,group);
  theCommands.Add("vselbvhbuild",
                  "vselbvhbuild [{0|1}] [-nbThreads value] [-wait]"
                  "\n\t\t: Turns on/off prebuilding of BVH within background thread(s)"
                  "\n\t\t:   -nbThreads   number of threads, 1 by default; if < 1 then used (NbLogicalProcessors - 1)"
                  "\n\t\t:   -wait        waits for building all of BVH",
                  __FILE__,VSelBvhBuild,group);
}
