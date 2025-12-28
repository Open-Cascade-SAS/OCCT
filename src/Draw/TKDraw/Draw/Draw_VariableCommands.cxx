// Created on: 1995-02-23
// Created by: Remi LEQUETTE
// Copyright (c) 1995-1999 Matra Datavision
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

#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Axis2D.hxx>
#include <Draw_Axis3D.hxx>
#include <Draw_Display.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_Grid.hxx>
#include <Draw_Number.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <NCollection_Sequence.hxx>
#include <Message.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Map.hxx>
#include <Standard_SStream.hxx>
#include <Standard_Macro.hxx>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <Standard_NotImplemented.hxx>
#include <TCollection_AsciiString.hxx>

#include <ios>
#ifdef _WIN32
extern Draw_Viewer dout;
#endif

#include <tcl.h>
#include <errno.h>

#include <OSD_Environment.hxx>
#include <OSD_FileSystem.hxx>
#include <OSD_OpenFile.hxx>

bool Draw_ParseFailed = true;

static bool autodisp  = true;
static bool repaint2d = false, repaint3d = false;

//! Returns dictionary of variables
//! Variables are stored in a map Integer, Transient.
//! The Integer Value is the content of the Tcl variable.
static NCollection_Map<occ::handle<Draw_Drawable3D>>& Draw_changeDrawables()
{
  static NCollection_Map<occ::handle<Draw_Drawable3D>> theVariables;
  return theVariables;
}

//=================================================================================================

static int         p_id   = 0;
static int         p_X    = 0;
static int         p_Y    = 0;
static int         p_b    = 0;
static const char* p_Name = "";

//=======================================================================
// save
//=======================================================================
static int save(Draw_Interpretor& theDI, int theNbArgs, const char** theArgVec)
{
  if (theNbArgs != 3)
  {
    theDI << "Syntax error: wrong number of arguments!\n";
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  occ::handle<Draw_Drawable3D> aDrawable = Draw::Get(theArgVec[1]);
  if (aDrawable.IsNull())
  {
    theDI << "Syntax error: '" << theArgVec[1] << "' is not a drawable";
    return 1;
  }

  const char*                        aName       = theArgVec[2];
  const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream>      aStream =
    aFileSystem->OpenOStream(aName, std::ios::out | std::ios::binary);
  aStream->precision(15);
  if (aStream.get() == nullptr || !aStream->good())
  {
    theDI << "Error: cannot open file for writing " << aName;
    return 1;
  }

  try
  {
    occ::handle<Draw_ProgressIndicator> aProgress     = new Draw_ProgressIndicator(theDI, 1);
    const char*                         aToolTypeName = aDrawable->DynamicType()->Name();
    *aStream << aToolTypeName << "\n";
    Draw::SetProgressBar(aProgress);
    aDrawable->Save(*aStream);
  }
  catch (const Standard_NotImplemented&)
  {
    theDI << "Error: no method for saving " << theArgVec[1];
    return 1;
  }
  *aStream << "\n";
  *aStream << "0\n\n";
  Draw::SetProgressBar(occ::handle<Draw_ProgressIndicator>());

  errno           = 0;
  const bool aRes = aStream->good() && !errno;
  if (!aRes)
  {
    theDI << "Error: file has not been written";
    return 1;
  }

  theDI << theArgVec[1];
  return 0;
}

//=======================================================================
// read
//=======================================================================
static int restore(Draw_Interpretor& theDI, int theNbArgs, const char** theArgVec)
{
  if (theNbArgs != 3)
  {
    return 1;
  }

  const char* aFileName = theArgVec[1];
  const char* aVarName  = theArgVec[2];

  const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream>      aStream = aFileSystem->OpenIStream(aFileName, std::ios::in);
  if (aStream.get() == nullptr)
  {
    theDI << "Error: cannot open file for reading: '" << aFileName << "'";
    return 1;
  }
  char aType[255] = {};
  *aStream >> aType;
  if (aStream->fail())
  {
    theDI << "Error: cannot read file: '" << aFileName << "'";
    return 1;
  }

  {
    occ::handle<Draw_ProgressIndicator> aProgress = new Draw_ProgressIndicator(theDI, 1);
    Draw::SetProgressBar(aProgress);
    occ::handle<Draw_Drawable3D> aDrawable = Draw_Drawable3D::Restore(aType, *aStream);
    if (aDrawable.IsNull())
    {
      // assume that this file stores a DBRep_DrawableShape variable
      aStream->seekg(0, std::ios::beg);
      aDrawable = Draw_Drawable3D::Restore("DBRep_DrawableShape", *aStream);
    }
    if (aDrawable.IsNull())
    {
      theDI << "Error: cannot restore a " << aType;
      return 1;
    }

    Draw::Set(aVarName, aDrawable, aDrawable->IsDisplayable() && autodisp);
    Draw::SetProgressBar(occ::handle<Draw_ProgressIndicator>());
  }

  theDI << aVarName;
  return 0;
}

//=======================================================================
// display
//=======================================================================

static int display(Draw_Interpretor& di, int n, const char** a)
{
  if (n <= 1)
    return 1;
  else
  {
    for (int i = 1; i < n; i++)
    {
      occ::handle<Draw_Drawable3D> D = Draw::Get(a[i]);
      if (!D.IsNull())
      {
        if (!D->Visible())
        {
          dout << D;
          di << a[i] << " ";
        }
      }
    }
  }
  return 0;
}

//=======================================================================
// erase, clear, donly
//=======================================================================

static int erase(Draw_Interpretor& di, int n, const char** a)
{
  static bool draw_erase_mute = false;
  if (n == 2)
  {
    if (!strcasecmp(a[1], "-mute"))
    {
      draw_erase_mute = true;
      return 0;
    }
  }

  bool donly = !strcasecmp(a[0], "donly");

  if (n <= 1 || donly)
  {
    // clear, 2dclear, donly, erase (without arguments)
    int i;

    // solve the names for "." before erasing
    if (donly)
    {
      for (i = 1; i < n; i++)
      {
        occ::handle<Draw_Drawable3D> D = Draw::Get(a[i]);
        if (D.IsNull())
        {
          if ((a[i][0] == '.') && (a[i][1] == '\0'))
            std::cout << "Missed !!!" << std::endl;
          return 0;
        }
      }
    }

    // sauvegarde des proteges visibles
    NCollection_Sequence<occ::handle<Draw_Drawable3D>> prot;
    for (NCollection_Map<occ::handle<Draw_Drawable3D>>::Iterator aMapIt(Draw::Drawables());
         aMapIt.More();
         aMapIt.Next())
    {
      const occ::handle<Draw_Drawable3D>& D = aMapIt.Key();
      if (!D.IsNull())
      {
        if (D->Protected() && D->Visible())
        {
          prot.Append(D);
        }
      }
    }

    // effacement de toutes les variables
    int erasemode = 1;
    if (a[0][0] == '2')
      erasemode = 2;
    if (a[0][0] == 'c')
      erasemode = 3;

    // effacement des graphiques non variables
    if (erasemode == 2)
      dout.Clear2D();
    else if (erasemode == 3)
      dout.Clear3D();
    else
      dout.Clear();

    // affichage pour donly
    if (donly)
    {
      for (i = 1; i < n; i++)
      {
        occ::handle<Draw_Drawable3D> D = Draw::Get(a[i]);
        if (!D.IsNull())
        {
          if (!D->Visible())
          {
            dout << D;
            if (!draw_erase_mute)
              di << a[i] << " ";
          }
        }
      }
    }

    // afficahge des proteges
    for (i = 1; i <= prot.Length(); i++)
      dout << prot(i);
  }
  else
  {
    for (int i = 1; i < n; i++)
    {
      occ::handle<Draw_Drawable3D> D = Draw::Get(a[i]);
      if (!D.IsNull())
      {
        if (D->Visible())
        {
          dout.RemoveDrawable(D);
          if (!draw_erase_mute)
            di << D->Name() << " ";
        }
      }
    }
    dout.Repaint2D();
    dout.Repaint3D();
  }
  draw_erase_mute = false;
  repaint2d       = false;
  repaint3d       = false;
  dout.Flush();
  return 0;
}

//=======================================================================
// draw
//=======================================================================

static int draw(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;
  int id = Draw::Atoi(a[1]);
  if (!dout.HasView(id))
  {
    Message::SendFail() << "bad view number in draw";
    return 1;
  }
  int          mo = Draw::Atoi(a[2]);
  Draw_Display d  = dout.MakeDisplay(id);
  d.SetMode(mo);
  int i;
  for (i = 3; i < n; i++)
  {
    occ::handle<Draw_Drawable3D> D = Draw::Get(a[i]);
    if (!D.IsNull())
      D->DrawOn(d);
  }
  d.Flush();
  return 0;
}

//=======================================================================
// protect, unprotect
//=======================================================================

static int protect(Draw_Interpretor& di, int n, const char** a)
{
  if (n <= 1)
    return 1;
  bool prot = *a[0] != 'u';
  for (int i = 1; i < n; i++)
  {
    occ::handle<Draw_Drawable3D> D = Draw::Get(a[i]);
    if (!D.IsNull())
    {
      D->Protected(prot);
      di << a[i] << " ";
    }
  }
  return 0;
}

//=======================================================================
// autodisplay
//=======================================================================

static int autodisplay(Draw_Interpretor& di, int n, const char** a)
{
  if (n <= 1)
    autodisp = !autodisp;
  else
    autodisp = !(!strcasecmp(a[1], "0"));

  if (autodisp)
    di << "1";
  else
    di << "0";

  return 0;
}

//=======================================================================
// whatis
//=======================================================================

static int whatis(Draw_Interpretor& di, int n, const char** a)
{
  if (n <= 1)
    return 1;
  for (int i = 1; i < n; i++)
  {
    occ::handle<Draw_Drawable3D> D = Draw::Get(a[i]);
    if (!D.IsNull())
    {
      D->Whatis(di);
    }
  }
  return 0;
}

//=======================================================================
// value
//=======================================================================

static int value(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 2)
    return 1;
  di << Draw::Atof(a[1]);

  return 0;
}

//=================================================================================================

static int dname(Draw_Interpretor& di, int n, const char** a)
{
  if (n <= 1)
  {
    return 1;
  }
  //
  Standard_PCharacter          pC;
  int                          i;
  occ::handle<Draw_Drawable3D> aD;
  //
  for (i = 1; i < n; ++i)
  {
    aD = Draw::Get(a[i]);
    if (!aD.IsNull())
    {
      // modified by NIZNHY-PKV Tue Jun 10 10:18:13 2008f
      // di << a[i];
      pC = (Standard_PCharacter)aD->Name();
      di << pC;
      // modified by NIZNHY-PKV Tue Jun 10 10:18:18 2008t
    }
  }
  return 0;
}

//=======================================================================
// dump
//=======================================================================

static int dump(Draw_Interpretor& DI, int n, const char** a)
{
  if (n < 2)
    return 1;
  int i;
  for (i = 1; i < n; i++)
  {
    occ::handle<Draw_Drawable3D> D = Draw::Get(a[i]);
    if (!D.IsNull())
    {
      Standard_SStream sss;
      sss.precision(15);
      sss << "\n\n*********** Dump of " << a[i] << " *************\n";
      D->Dump(sss);
      DI << sss;
    }
  }
  return 0;
}

//=======================================================================
// copy
//=======================================================================

static int copy(Draw_Interpretor&, int n, const char** a)
{
  if (n < 3)
    return 1;
  bool cop = !strcasecmp(a[0], "copy");

  occ::handle<Draw_Drawable3D> D;
  for (int i = 1; i < n; i += 2)
  {
    if (i + 1 >= n)
      return 0;
    D = Draw::Get(a[i]);
    if (!D.IsNull())
    {
      if (cop)
        D = D->Copy();
      else
        // clear old name
        Draw::Set(a[i], occ::handle<Draw_Drawable3D>());

      Draw::Set(a[i + 1], D);
    }
  }
  return 0;
}

//=================================================================================================

static int repaintall(Draw_Interpretor&, int, const char**)
{
  if (repaint2d)
    dout.Repaint2D();
  repaint2d = false;
  if (repaint3d)
    dout.Repaint3D();
  repaint3d = false;
  dout.Flush();
  return 0;
}

//=================================================================================================

static int set(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
    return 1;
  int    i   = 1;
  double val = 0;
  for (i = 1; i < n; i += 2)
  {
    val = 0;
    if (i + 1 < n)
      val = Draw::Atof(a[i + 1]);
    Draw::Set(a[i], val);
  }
  di << val;
  return 0;
}

//=================================================================================================

static int dsetenv(Draw_Interpretor& /*di*/, int argc, const char** argv)
{
  if (argc < 2)
  {
    Message::SendFail() << "Use: " << argv[0] << " {varname} [value]";
    return 1;
  }

  OSD_Environment env(argv[1]);
  if (argc > 2 && argv[2][0] != '\0')
  {
    env.SetValue(argv[2]);
    env.Build();
  }
  else
    env.Remove();
  return env.Failed();
}

//=================================================================================================

static int dgetenv(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
  {
    Message::SendFail() << "Use: " << argv[0] << " {varname}";
    return 1;
  }

  const char* val = getenv(argv[1]);
  di << (val ? val : "");
  return 0;
}

//=================================================================================================

static int isdraw(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 2)
    return 1;
  occ::handle<Draw_Drawable3D> D = Draw::Get(a[1]);
  if (D.IsNull())
    di << "0";
  else
    di << "1";
  return 0;
}

//=================================================================================================

int isprot(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 2)
    return 1;
  occ::handle<Draw_Drawable3D> D = Draw::Get(a[1]);
  if (D.IsNull())
    di << "0";
  else
  {
    if (D->Protected())
      di << "1";
    else
      di << "0";
  }
  return 0;
}

//=================================================================================================

static int pick(Draw_Interpretor&, int n, const char** a)
{
  if (n < 6)
    return 1;
  int  id;
  int  X, Y, b;
  bool wait = (n == 6);
  if (!wait)
    id = Draw::Atoi(a[1]);
  dout.Select(id, X, Y, b, wait);
  double  z = dout.Zoom(id);
  gp_Pnt  P((double)X / z, (double)Y / z, 0);
  gp_Trsf T;
  dout.GetTrsf(id, T);
  T.Invert();
  P.Transform(T);
  Draw::Set(a[1], id);
  Draw::Set(a[2], P.X());
  Draw::Set(a[3], P.Y());
  Draw::Set(a[4], P.Z());
  Draw::Set(a[5], b);
  return 0;
}

//=================================================================================================

static int lastrep(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 5)
    return 1;

  Draw::Set(a[1], p_id);
  if (n == 5)
  {
    Draw::Set(a[2], p_X);
    Draw::Set(a[3], p_Y);
  }
  else if (n == 6)
  {
    double  z = dout.Zoom(p_id);
    gp_Pnt  P((double)p_X / z, (double)p_Y / z, 0);
    gp_Trsf T;
    dout.GetTrsf(p_id, T);
    T.Invert();
    P.Transform(T);
    Draw::Set(a[2], P.X());
    Draw::Set(a[3], P.Y());
    Draw::Set(a[4], P.Z());
  }
  else
  {
    di << "Too many args";
    return 1;
  }
  Draw::Set(a[n - 1], p_b);
  di << p_Name;
  return 0;
}

//=================================================================================================

void Draw::Set(const char* name, const occ::handle<Draw_Drawable3D>& D)
{
  Draw::Set(name, D, autodisp);
}

// MKV 29.03.05
static char* tracevar(ClientData CD, Tcl_Interp*, const char* name, const char*, int)
{
  // protect if the map was destroyed before the interpreter
  if (Draw::Drawables().IsEmpty())
  {
    return nullptr;
  }

  Draw_Interpretor& aCommands = Draw::GetInterpretor();

  // MSV 9.10.14 CR25344
  occ::handle<Draw_Drawable3D> D(reinterpret_cast<Draw_Drawable3D*>(CD));
  if (D.IsNull())
  {
    Tcl_UntraceVar(aCommands.Interp(), name, TCL_TRACE_UNSETS | TCL_TRACE_WRITES, tracevar, CD);
    return nullptr;
  }
  if (D->Protected())
  {
    D->Name(Tcl_SetVar(aCommands.Interp(), name, name, 0));
    return (char*)"variable is protected";
  }
  else
  {
    if (D->Visible())
    {
      dout.RemoveDrawable(D);
      if (D->Is3D())
        repaint3d = true;
      else
        repaint2d = true;
    }
    Tcl_UntraceVar(aCommands.Interp(), name, TCL_TRACE_UNSETS | TCL_TRACE_WRITES, tracevar, CD);
    Draw_changeDrawables().Remove(D);
    return nullptr;
  }
}

//=================================================================================================

void Draw::Set(const char* name, const occ::handle<Draw_Drawable3D>& D, const bool displ)
{
  Draw_Interpretor& aCommands = Draw::GetInterpretor();

  if ((name[0] == '.') && (name[1] == '\0'))
  {
    if (!D.IsNull())
    {
      dout.RemoveDrawable(D);
      if (displ)
        dout << D;
    }
  }
  else
  {
    // Check if the variable with the same name exists
    ClientData                   aCD = Tcl_VarTraceInfo(aCommands.Interp(),
                                      name,
                                      TCL_TRACE_UNSETS | TCL_TRACE_WRITES,
                                      tracevar,
                                      nullptr);
    occ::handle<Draw_Drawable3D> anOldD(reinterpret_cast<Draw_Drawable3D*>(aCD));
    if (!anOldD.IsNull())
    {
      if (Draw::Drawables().Contains(anOldD) && anOldD->Protected())
      {
        std::cout << "variable is protected" << std::endl;
        return;
      }
      anOldD.Nullify();
    }

    Tcl_UnsetVar(aCommands.Interp(), name, 0);

    if (!D.IsNull())
    {
      Draw_changeDrawables().Add(D);
      D->Name(Tcl_SetVar(aCommands.Interp(), name, name, 0));

      // set the trace function
      Tcl_TraceVar(aCommands.Interp(),
                   name,
                   TCL_TRACE_UNSETS | TCL_TRACE_WRITES,
                   tracevar,
                   reinterpret_cast<ClientData>(D.operator->()));
      if (displ)
      {
        if (!D->Visible())
          dout << D;
      }
      else if (D->Visible())
        dout.RemoveDrawable(D);
    }
  }
}

//=================================================================================================

void Draw::Set(const char* theName, const double theValue)
{
  if (occ::handle<Draw_Number> aNumber = occ::down_cast<Draw_Number>(Draw::GetExisting(theName)))
  {
    aNumber->Value(theValue);
  }
  else
  {
    aNumber = new Draw_Number(theValue);
    Draw::Set(theName, aNumber, false);
  }
}

//=================================================================================================

occ::handle<Draw_Drawable3D> Draw::getDrawable(const char*& theName, bool theToAllowPick)
{
  const bool toPick = ((theName[0] == '.') && (theName[1] == '\0'));
  if (!toPick)
  {
    ClientData                   aCD       = Tcl_VarTraceInfo(Draw::GetInterpretor().Interp(),
                                      theName,
                                      TCL_TRACE_UNSETS | TCL_TRACE_WRITES,
                                      tracevar,
                                      nullptr);
    occ::handle<Draw_Drawable3D> aDrawable = reinterpret_cast<Draw_Drawable3D*>(aCD);
    return Draw::Drawables().Contains(aDrawable) ? aDrawable : occ::handle<Draw_Drawable3D>();
  }
  else if (!theToAllowPick)
  {
    return occ::handle<Draw_Drawable3D>();
  }

  std::cout << "Pick an object" << std::endl;
  occ::handle<Draw_Drawable3D> aDrawable;
  dout.Select(p_id, p_X, p_Y, p_b);
  dout.Pick(p_id, p_X, p_Y, 5, aDrawable, 0);
  if (!aDrawable.IsNull() && aDrawable->Name() != nullptr)
  {
    theName = p_Name = aDrawable->Name();
  }
  return aDrawable;
}

//=================================================================================================

bool Draw::Get(const char* theName, double& theValue)
{
  if (occ::handle<Draw_Number> aNumber = occ::down_cast<Draw_Number>(Draw::GetExisting(theName)))
  {
    theValue = aNumber->Value();
    return true;
  }
  return false;
}

//=================================================================================================

void Draw::LastPick(int& view, int& X, int& Y, int& button)
{
  view   = p_id;
  X      = p_X;
  Y      = p_Y;
  button = p_b;
}

//=================================================================================================

void Draw::Repaint()
{
  repaint2d = true;
  repaint3d = true;
}

//=================================================================================================

// static int trigo (Draw_Interpretor& di, int n, const char** a)
static int trigo(Draw_Interpretor& di, int, const char** a)
{

  double x = Draw::Atof(a[1]);

  if (!strcasecmp(a[0], "cos"))
    di << std::cos(x);
  else if (!strcasecmp(a[0], "sin"))
    di << std::sin(x);
  else if (!strcasecmp(a[0], "tan"))
    di << std::tan(x);
  else if (!strcasecmp(a[0], "sqrt"))
    di << std::sqrt(x);
  else if (!strcasecmp(a[0], "acos"))
    di << std::acos(x);
  else if (!strcasecmp(a[0], "asin"))
    di << std::asin(x);
  else if (!strcasecmp(a[0], "atan2"))
    di << std::atan2(x, Draw::Atof(a[2]));

  return 0;
}

//=======================================================================
// Atof and Atoi
//=======================================================================

static bool Numeric(char c)
{
  return (c == '.' || (c >= '0' && c <= '9'));
}

static bool Alphabetic(char c)
{
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'));
}

static double Parse(char*&);

static double ParseValue(char*& theName)
{
  while (*theName == ' ' || *theName == '\t')
  {
    ++theName;
  }
  double x = 0;
  switch (*theName)
  {
    case '\0': {
      break;
    }
    case '(': {
      ++theName;
      x = Parse(theName);
      if (*theName != ')')
      {
        std::cout << "Mismatched parenthesis" << std::endl;
      }
      ++theName;
      break;
    }
    case '+': {
      ++theName;
      x = ParseValue(theName);
      break;
    }
    case '-': {
      ++theName;
      x = -ParseValue(theName);
      break;
    }
    default: {
      // process a string
      char* p = theName;
      while (Numeric(*p))
      {
        ++p;
      }
      // process scientific notation
      if ((*p == 'e') || (*p == 'E'))
      {
        if (Numeric(*(p + 1)) || *(p + 1) == '+' || *(p + 1) == '-')
        {
          p += 2;
        }
      }
      while (Numeric(*p) || Alphabetic(*p))
      {
        p++;
      }
      char c = *p;
      *p     = '\0';

      if (Numeric(*theName)) // numeric literal
      {
        x = Atof(theName);
      }
      else if (!Draw::Get((const char*)theName, x)) // variable
      {
        // search for a function ...
        *p = c;
        // count arguments
        int   argc = 1;
        char* q    = p;
        while ((*q == ' ') || (*q == '\t'))
        {
          ++q;
        }
        if (*q == '(')
        {
          int pc = 1;
          argc   = 2;
          q++;
          while ((pc > 0) && *q)
          {
            if (*q == '(')
            {
              ++pc;
            }
            if (*q == ')')
            {
              --pc;
            }
            if ((pc == 1) && (*q == ','))
            {
              ++argc;
            }
            ++q;
          }
          if (pc > 0)
          {
            std::cout << "Unclosed parenthesis" << std::endl;
            x = 0;
          }
          else
          {
            // build function call
            // replace , and first and last () by space
            if (argc > 1)
            {
              while (*p != '(')
              {
                ++p;
              }
              *p = ' ';
              ++p;
              pc = 1;
              while (pc > 0)
              {
                if (*p == '(')
                {
                  ++pc;
                }
                if (*p == ')')
                {
                  --pc;
                }
                if ((pc == 1) && (*p == ','))
                {
                  *p = ' ';
                  ++p;
                }
                else
                {
                  ++p;
                }
              }
              *(p - 1) = '\0';
              c        = *p;

              Draw_Interpretor& aCommands = Draw::GetInterpretor();

              // call the function, save the current result
              TCollection_AsciiString sv(aCommands.Result());
              if (*aCommands.Result())
              {
                aCommands.Reset();
              }
              if (aCommands.Eval(theName) != 0)
              {
                std::cout << "Call of function " << theName << " failed" << std::endl;
                x = 0;
              }
              else
              {
                x = Atof(aCommands.Result());
              }
              aCommands.Reset();
              if (!sv.IsEmpty())
              {
                aCommands << sv;
              }
            }
          }
        }
        else
        {
          Draw_ParseFailed = true;
        }
      }
      *p      = c;
      theName = p;
    }
    break;
  }

  while (*theName == ' ' || *theName == '\t')
  {
    ++theName;
  }
  return x;
}

static double ParseFactor(char*& name)
{
  double x = ParseValue(name);

  for (;;)
  {
    char c = *name;
    if (c == '\0')
      return x;
    name++;
    switch (c)
    {

      case '*':
        x *= ParseValue(name);
        break;

      case '/':
        x /= ParseValue(name);
        break;

      default:
        name--;
        return x;
    }
  }
}

static double Parse(char*& name)
{
  double x = ParseFactor(name);

  for (;;)
  {
    char c = *name;
    if (c == '\0')
      return x;
    name++;
    switch (c)
    {

      case '+':
        x += ParseFactor(name);
        break;

      case '-':
        x -= ParseFactor(name);
        break;

      default:
        name--;
        return x;
    }
  }
}

//=================================================================================================

double Draw::Atof(const char* theName)
{
  // copy the string
  NCollection_Array1<char> aBuff(0, (int)strlen(theName));
  char*                    n = &aBuff.ChangeFirst();
  strcpy(n, theName);
  Draw_ParseFailed = false;
  double x         = Parse(n);
  while ((*n == ' ') || (*n == '\t'))
    n++;
  if (*n)
    Draw_ParseFailed = true;
  return x;
}

//=================================================================================================

bool Draw::ParseReal(const char* theExpressionString, double& theParsedRealValue)
{
  const double aParsedRealValue = Atof(theExpressionString);
  if (Draw_ParseFailed)
  {
    Draw_ParseFailed = false;
    return false;
  }
  theParsedRealValue = aParsedRealValue;
  return true;
}

//=================================================================================================

int Draw::Atoi(const char* name)
{
  return (int)Draw::Atof(name);
}

//=================================================================================================

bool Draw::ParseInteger(const char* theExpressionString, int& theParsedIntegerValue)
{
  double aParsedRealValue = 0.0;
  if (!ParseReal(theExpressionString, aParsedRealValue))
  {
    return false;
  }
  const int aParsedIntegerValue = static_cast<int>(aParsedRealValue);
  if (static_cast<double>(aParsedIntegerValue) != aParsedRealValue)
  {
    return false;
  }
  theParsedIntegerValue = aParsedIntegerValue;
  return true;
}

//=================================================================================================

void Draw::Set(const char* Name, const char* val)
{
  Standard_PCharacter pName, pVal;
  //
  pName = (Standard_PCharacter)Name;
  pVal  = (Standard_PCharacter)val;
  //
  Tcl_SetVar(Draw::GetInterpretor().Interp(), pName, pVal, 0);
}

//=================================================================================================

const NCollection_Map<occ::handle<Draw_Drawable3D>>& Draw::Drawables()
{
  return Draw_changeDrawables();
}

//=======================================================================
// Command management
// refresh the screen
//=======================================================================

static void before()
{
  repaint2d = false;
  repaint3d = false;
}

void Draw_RepaintNowIfNecessary()
{
  if (repaint2d)
    dout.Repaint2D();
  if (repaint3d)
    dout.Repaint3D();
  repaint2d = false;
  repaint3d = false;
}

static void after(int)
{
  Draw_RepaintNowIfNecessary();
}

extern void (*Draw_BeforeCommand)();
extern void (*Draw_AfterCommand)(int);

//=================================================================================================

void Draw::VariableCommands(Draw_Interpretor& theCommandsArg)
{
  static bool Done = false;
  if (Done)
    return;
  Done = true;

  // set up start and stop command
  Draw_BeforeCommand = &before;
  Draw_AfterCommand  = &after;

  // Register save/restore tools
  Draw_Number::RegisterFactory();

  // set up some variables
  const char*              n;
  occ::handle<Draw_Axis3D> theAxes3d = new Draw_Axis3D(gp_Pnt(0, 0, 0), Draw_bleu, 20);
  n                                  = "axes";
  Draw::Set(n, theAxes3d);
  theAxes3d->Protected(true);

  occ::handle<Draw_Axis2D> theAxes2d = new Draw_Axis2D(gp_Pnt2d(0, 0), Draw_bleu, 20);
  n                                  = "axes2d";
  Draw::Set(n, theAxes2d);
  theAxes2d->Protected(true);

  n = "pi";
  Draw::Set(n, M_PI);
  Draw::Get(n)->Protected(true);

  n = "pinf";
  Draw::Set(n, RealLast());
  Draw::Get(n)->Protected(true);

  n = "minf";
  Draw::Set(n, RealFirst());
  Draw::Get(n)->Protected(true);

  n                              = "grid";
  occ::handle<Draw_Grid> theGrid = new Draw_Grid();
  Draw::Set(n, theGrid);
  theGrid->Protected(true);

  const char* g;

  g = "DRAW Numeric functions";

  theCommandsArg.Add("cos", "cos(x)", __FILE__, trigo, g);
  theCommandsArg.Add("sin", "sin(x)", __FILE__, trigo, g);
  theCommandsArg.Add("tan", "tan(x)", __FILE__, trigo, g);
  theCommandsArg.Add("acos", "acos(x)", __FILE__, trigo, g);
  theCommandsArg.Add("asin", "asin(x)", __FILE__, trigo, g);
  theCommandsArg.Add("atan2", "atan2(x,y)", __FILE__, trigo, g);
  theCommandsArg.Add("sqrt", "sqrt(x)", __FILE__, trigo, g);

  g = "DRAW Variables management";

  theCommandsArg.Add("protect", "protect name ...", __FILE__, protect, g);
  theCommandsArg.Add("unprotect", "unprotect name ...", __FILE__, protect, g);

  theCommandsArg.Add("bsave", "bsave name filename", __FILE__, save, g);
  theCommandsArg.Add("brestore", "brestore filename name", __FILE__, restore, g);

  theCommandsArg.Add("isdraw", "isdraw var, return 1 if Draw value", __FILE__, isdraw, g);
  theCommandsArg.Add("isprot",
                     "isprot var, return 1 if Draw var is protected",
                     __FILE__,
                     isprot,
                     g);

  theCommandsArg.Add("autodisplay", "toggle autodisplay [0/1]", __FILE__, autodisplay, g);
  theCommandsArg.Add("display",
                     "display [name1 name2 ...], no names display all",
                     __FILE__,
                     display,
                     g);
  theCommandsArg.Add("donly", "donly [name1 name2 ...], erase and display", __FILE__, erase, g);
  theCommandsArg.Add("erase", "erase [name1 name2 ...], no names erase all", __FILE__, erase, g);
  theCommandsArg.Add("draw",
                     "draw view mode [name1 name2 ...], draw on view with mode",
                     __FILE__,
                     draw,
                     g);
  theCommandsArg.Add("clear", "clear display", __FILE__, erase, g);
  theCommandsArg.Add("2dclear", "clear display (2d objects)", __FILE__, erase, g);
  theCommandsArg.Add("repaint", "repaint, force redraw", __FILE__, repaintall, g);

  theCommandsArg.Add("dtyp", "dtyp name1 name2", __FILE__, whatis, g);
  theCommandsArg.Add("dval", "dval name, return value", __FILE__, value, g);
  theCommandsArg.Add("dname", "dname name, print name", __FILE__, dname, g);
  theCommandsArg.Add("dump", "dump name1 name2 ...", __FILE__, dump, g);
  theCommandsArg.Add("copy", "copy name1 toname1 name2 toname2 ...", __FILE__, copy, g);
  // san - 02/08/2002 - `rename` command changed to `renamevar` since it conflicts with
  // the built-in Tcl command `rename`
  // theCommands.Add("rename","rename name1 toname1 name2 toname2 ...",__FILE__,copy,g);
  theCommandsArg.Add("renamevar", "renamevar name1 toname1 name2 toname2 ...", __FILE__, copy, g);
  theCommandsArg.Add("dset", "var1 value1 vr2 value2 ...", __FILE__, set, g);

  // commands to access C environment variables; see Mantis issue #23197
  theCommandsArg.Add("dgetenv",
                     "var : get value of environment variable in C subsystem",
                     __FILE__,
                     dgetenv,
                     g);
  theCommandsArg.Add(
    "dsetenv",
    "var [value] : set (unset if value is empty) environment variable in C subsystem",
    __FILE__,
    dsetenv,
    g);

  theCommandsArg.Add("pick", "pick id X Y Z b [nowait]", __FILE__, pick, g);
  theCommandsArg.Add("lastrep", "lastrep id X Y [Z] b, return name", __FILE__, lastrep, g);
}
