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
#include <Draw_SequenceOfDrawable3D.hxx>
#include <Message.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Map.hxx>
#include <Standard_SStream.hxx>
#include <Standard_Stream.hxx>
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

Standard_Boolean Draw_ParseFailed = Standard_True;

static Standard_Boolean autodisp  = Standard_True;
static Standard_Boolean repaint2d = Standard_False, repaint3d = Standard_False;

//! Returns dictionary of variables
//! Variables are stored in a map Integer, Transient.
//! The Integer Value is the content of the Tcl variable.
static NCollection_Map<Handle(Draw_Drawable3D)>& Draw_changeDrawables()
{
  static NCollection_Map<Handle(Draw_Drawable3D)> theVariables;
  return theVariables;
}

//=================================================================================================

static Standard_Integer p_id   = 0;
static Standard_Integer p_X    = 0;
static Standard_Integer p_Y    = 0;
static Standard_Integer p_b    = 0;
static const char*      p_Name = "";

//=======================================================================
// save
//=======================================================================
static Standard_Integer save(Draw_Interpretor& theDI,
                             Standard_Integer  theNbArgs,
                             const char**      theArgVec)
{
  if (theNbArgs != 3)
  {
    theDI << "Syntax error: wrong number of arguments!\n";
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  Handle(Draw_Drawable3D) aDrawable = Draw::Get(theArgVec[1]);
  if (aDrawable.IsNull())
  {
    theDI << "Syntax error: '" << theArgVec[1] << "' is not a drawable";
    return 1;
  }

  const char*                   aName       = theArgVec[2];
  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream> aStream =
    aFileSystem->OpenOStream(aName, std::ios::out | std::ios::binary);
  aStream->precision(15);
  if (aStream.get() == NULL || !aStream->good())
  {
    theDI << "Error: cannot open file for writing " << aName;
    return 1;
  }

  try
  {
    Handle(Draw_ProgressIndicator) aProgress     = new Draw_ProgressIndicator(theDI, 1);
    Standard_CString               aToolTypeName = aDrawable->DynamicType()->Name();
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
  Draw::SetProgressBar(Handle(Draw_ProgressIndicator)());

  errno                       = 0;
  const Standard_Boolean aRes = aStream->good() && !errno;
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
static Standard_Integer restore(Draw_Interpretor& theDI,
                                Standard_Integer  theNbArgs,
                                const char**      theArgVec)
{
  if (theNbArgs != 3)
  {
    return 1;
  }

  const char* aFileName = theArgVec[1];
  const char* aVarName  = theArgVec[2];

  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aStream     = aFileSystem->OpenIStream(aFileName, std::ios::in);
  if (aStream.get() == NULL)
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
    Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(theDI, 1);
    Draw::SetProgressBar(aProgress);
    Handle(Draw_Drawable3D) aDrawable = Draw_Drawable3D::Restore(aType, *aStream);
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
    Draw::SetProgressBar(Handle(Draw_ProgressIndicator)());
  }

  theDI << aVarName;
  return 0;
}

//=======================================================================
// display
//=======================================================================

static Standard_Integer display(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n <= 1)
    return 1;
  else
  {
    for (Standard_Integer i = 1; i < n; i++)
    {
      Handle(Draw_Drawable3D) D = Draw::Get(a[i]);
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

static Standard_Integer erase(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  static Standard_Boolean draw_erase_mute = Standard_False;
  if (n == 2)
  {
    if (!strcasecmp(a[1], "-mute"))
    {
      draw_erase_mute = Standard_True;
      return 0;
    }
  }

  Standard_Boolean donly = !strcasecmp(a[0], "donly");

  if (n <= 1 || donly)
  {
    // clear, 2dclear, donly, erase (without arguments)
    Standard_Integer i;

    // solve the names for "." before erasing
    if (donly)
    {
      for (i = 1; i < n; i++)
      {
        Handle(Draw_Drawable3D) D = Draw::Get(a[i]);
        if (D.IsNull())
        {
          if ((a[i][0] == '.') && (a[i][1] == '\0'))
            std::cout << "Missed !!!" << std::endl;
          return 0;
        }
      }
    }

    // sauvegarde des proteges visibles
    Draw_SequenceOfDrawable3D prot;
    for (NCollection_Map<Handle(Draw_Drawable3D)>::Iterator aMapIt(Draw::Drawables());
         aMapIt.More();
         aMapIt.Next())
    {
      const Handle(Draw_Drawable3D)& D = aMapIt.Key();
      if (!D.IsNull())
      {
        if (D->Protected() && D->Visible())
        {
          prot.Append(D);
        }
      }
    }

    // effacement de toutes les variables
    Standard_Integer erasemode = 1;
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
        Handle(Draw_Drawable3D) D = Draw::Get(a[i]);
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
    for (Standard_Integer i = 1; i < n; i++)
    {
      Handle(Draw_Drawable3D) D = Draw::Get(a[i]);
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
  draw_erase_mute = Standard_False;
  repaint2d       = Standard_False;
  repaint3d       = Standard_False;
  dout.Flush();
  return 0;
}

//=======================================================================
// draw
//=======================================================================

static Standard_Integer draw(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  if (n < 3)
    return 1;
  Standard_Integer id = Draw::Atoi(a[1]);
  if (!dout.HasView(id))
  {
    Message::SendFail() << "bad view number in draw";
    return 1;
  }
  Standard_Integer mo = Draw::Atoi(a[2]);
  Draw_Display     d  = dout.MakeDisplay(id);
  d.SetMode(mo);
  Standard_Integer i;
  for (i = 3; i < n; i++)
  {
    Handle(Draw_Drawable3D) D = Draw::Get(a[i]);
    if (!D.IsNull())
      D->DrawOn(d);
  }
  d.Flush();
  return 0;
}

//=======================================================================
// protect, unprotect
//=======================================================================

static Standard_Integer protect(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n <= 1)
    return 1;
  Standard_Boolean prot = *a[0] != 'u';
  for (Standard_Integer i = 1; i < n; i++)
  {
    Handle(Draw_Drawable3D) D = Draw::Get(a[i]);
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

static Standard_Integer autodisplay(Draw_Interpretor& di, Standard_Integer n, const char** a)
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

static Standard_Integer whatis(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n <= 1)
    return 1;
  for (Standard_Integer i = 1; i < n; i++)
  {
    Handle(Draw_Drawable3D) D = Draw::Get(a[i]);
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

static Standard_Integer value(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 2)
    return 1;
  di << Draw::Atof(a[1]);

  return 0;
}

//=================================================================================================

static Standard_Integer dname(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n <= 1)
  {
    return 1;
  }
  //
  Standard_PCharacter     pC;
  Standard_Integer        i;
  Handle(Draw_Drawable3D) aD;
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

static Standard_Integer dump(Draw_Interpretor& DI, Standard_Integer n, const char** a)
{
  if (n < 2)
    return 1;
  Standard_Integer i;
  for (i = 1; i < n; i++)
  {
    Handle(Draw_Drawable3D) D = Draw::Get(a[i]);
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

static Standard_Integer copy(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  if (n < 3)
    return 1;
  Standard_Boolean cop = !strcasecmp(a[0], "copy");

  Handle(Draw_Drawable3D) D;
  for (Standard_Integer i = 1; i < n; i += 2)
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
        Draw::Set(a[i], Handle(Draw_Drawable3D)());

      Draw::Set(a[i + 1], D);
    }
  }
  return 0;
}

//=================================================================================================

static Standard_Integer repaintall(Draw_Interpretor&, Standard_Integer, const char**)
{
  if (repaint2d)
    dout.Repaint2D();
  repaint2d = Standard_False;
  if (repaint3d)
    dout.Repaint3D();
  repaint3d = Standard_False;
  dout.Flush();
  return 0;
}

//=================================================================================================

static Standard_Integer set(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 2)
    return 1;
  Standard_Integer i   = 1;
  Standard_Real    val = 0;
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

static Standard_Integer dsetenv(Draw_Interpretor& /*di*/, Standard_Integer argc, const char** argv)
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

static Standard_Integer dgetenv(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
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

static Standard_Integer isdraw(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 2)
    return 1;
  Handle(Draw_Drawable3D) D = Draw::Get(a[1]);
  if (D.IsNull())
    di << "0";
  else
    di << "1";
  return 0;
}

//=================================================================================================

Standard_Integer isprot(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 2)
    return 1;
  Handle(Draw_Drawable3D) D = Draw::Get(a[1]);
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

static Standard_Integer pick(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  if (n < 6)
    return 1;
  Standard_Integer id;
  Standard_Integer X, Y, b;
  Standard_Boolean wait = (n == 6);
  if (!wait)
    id = Draw::Atoi(a[1]);
  dout.Select(id, X, Y, b, wait);
  Standard_Real z = dout.Zoom(id);
  gp_Pnt        P((Standard_Real)X / z, (Standard_Real)Y / z, 0);
  gp_Trsf       T;
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

static Standard_Integer lastrep(Draw_Interpretor& di, Standard_Integer n, const char** a)
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
    Standard_Real z = dout.Zoom(p_id);
    gp_Pnt        P((Standard_Real)p_X / z, (Standard_Real)p_Y / z, 0);
    gp_Trsf       T;
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

void Draw::Set(const Standard_CString name, const Handle(Draw_Drawable3D)& D)
{
  Draw::Set(name, D, autodisp);
}

// MKV 29.03.05
static char* tracevar(ClientData CD, Tcl_Interp*, const char* name, const char*, int)
{
  // protect if the map was destroyed before the interpretor
  if (Draw::Drawables().IsEmpty())
  {
    return NULL;
  }

  Draw_Interpretor& aCommands = Draw::GetInterpretor();

  // MSV 9.10.14 CR25344
  Handle(Draw_Drawable3D) D(reinterpret_cast<Draw_Drawable3D*>(CD));
  if (D.IsNull())
  {
    Tcl_UntraceVar(aCommands.Interp(), name, TCL_TRACE_UNSETS | TCL_TRACE_WRITES, tracevar, CD);
    return NULL;
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
        repaint3d = Standard_True;
      else
        repaint2d = Standard_True;
    }
    Tcl_UntraceVar(aCommands.Interp(), name, TCL_TRACE_UNSETS | TCL_TRACE_WRITES, tracevar, CD);
    Draw_changeDrawables().Remove(D);
    return NULL;
  }
}

//=================================================================================================

void Draw::Set(const Standard_CString         name,
               const Handle(Draw_Drawable3D)& D,
               const Standard_Boolean         displ)
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
    ClientData              aCD = Tcl_VarTraceInfo(aCommands.Interp(),
                                      name,
                                      TCL_TRACE_UNSETS | TCL_TRACE_WRITES,
                                      tracevar,
                                      NULL);
    Handle(Draw_Drawable3D) anOldD(reinterpret_cast<Draw_Drawable3D*>(aCD));
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

void Draw::Set(const Standard_CString theName, const Standard_Real theValue)
{
  if (Handle(Draw_Number) aNumber = Handle(Draw_Number)::DownCast(Draw::GetExisting(theName)))
  {
    aNumber->Value(theValue);
  }
  else
  {
    aNumber = new Draw_Number(theValue);
    Draw::Set(theName, aNumber, Standard_False);
  }
}

//=================================================================================================

Handle(Draw_Drawable3D) Draw::getDrawable(Standard_CString& theName,
                                          Standard_Boolean  theToAllowPick)
{
  const Standard_Boolean toPick = ((theName[0] == '.') && (theName[1] == '\0'));
  if (!toPick)
  {
    ClientData              aCD       = Tcl_VarTraceInfo(Draw::GetInterpretor().Interp(),
                                      theName,
                                      TCL_TRACE_UNSETS | TCL_TRACE_WRITES,
                                      tracevar,
                                      NULL);
    Handle(Draw_Drawable3D) aDrawable = reinterpret_cast<Draw_Drawable3D*>(aCD);
    return Draw::Drawables().Contains(aDrawable) ? aDrawable : Handle(Draw_Drawable3D)();
  }
  else if (!theToAllowPick)
  {
    return Handle(Draw_Drawable3D)();
  }

  std::cout << "Pick an object" << std::endl;
  Handle(Draw_Drawable3D) aDrawable;
  dout.Select(p_id, p_X, p_Y, p_b);
  dout.Pick(p_id, p_X, p_Y, 5, aDrawable, 0);
  if (!aDrawable.IsNull() && aDrawable->Name() != NULL)
  {
    theName = p_Name = aDrawable->Name();
  }
  return aDrawable;
}

//=================================================================================================

Standard_Boolean Draw::Get(const Standard_CString theName, Standard_Real& theValue)
{
  if (Handle(Draw_Number) aNumber = Handle(Draw_Number)::DownCast(Draw::GetExisting(theName)))
  {
    theValue = aNumber->Value();
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

void Draw::LastPick(Standard_Integer& view,
                    Standard_Integer& X,
                    Standard_Integer& Y,
                    Standard_Integer& button)
{
  view   = p_id;
  X      = p_X;
  Y      = p_Y;
  button = p_b;
}

//=================================================================================================

void Draw::Repaint()
{
  repaint2d = Standard_True;
  repaint3d = Standard_True;
}

//=================================================================================================

// static Standard_Integer trigo (Draw_Interpretor& di, Standard_Integer n, const char** a)
static Standard_Integer trigo(Draw_Interpretor& di, Standard_Integer, const char** a)
{

  Standard_Real x = Draw::Atof(a[1]);

  if (!strcasecmp(a[0], "cos"))
    di << Cos(x);
  else if (!strcasecmp(a[0], "sin"))
    di << Sin(x);
  else if (!strcasecmp(a[0], "tan"))
    di << Tan(x);
  else if (!strcasecmp(a[0], "sqrt"))
    di << Sqrt(x);
  else if (!strcasecmp(a[0], "acos"))
    di << ACos(x);
  else if (!strcasecmp(a[0], "asin"))
    di << ASin(x);
  else if (!strcasecmp(a[0], "atan2"))
    di << ATan2(x, Draw::Atof(a[2]));

  return 0;
}

//=======================================================================
// Atof and Atoi
//=======================================================================

static Standard_Boolean Numeric(char c)
{
  return (c == '.' || (c >= '0' && c <= '9'));
}

static Standard_Boolean Alphabetic(char c)
{
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'));
}

static Standard_Real Parse(char*&);

static Standard_Real ParseValue(char*& theName)
{
  while (*theName == ' ' || *theName == '\t')
  {
    ++theName;
  }
  Standard_Real x = 0;
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
      else if (!Draw::Get((Standard_CString)theName, x)) // variable
      {
        // search for a function ...
        *p = c;
        // count arguments
        Standard_Integer argc = 1;
        char*            q    = p;
        while ((*q == ' ') || (*q == '\t'))
        {
          ++q;
        }
        if (*q == '(')
        {
          Standard_Integer pc = 1;
          argc                = 2;
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
          Draw_ParseFailed = Standard_True;
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

static Standard_Real ParseFactor(char*& name)
{
  Standard_Real x = ParseValue(name);

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

static Standard_Real Parse(char*& name)
{
  Standard_Real x = ParseFactor(name);

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

Standard_Real Draw::Atof(const Standard_CString theName)
{
  // copy the string
  NCollection_Array1<char> aBuff(0, (Standard_Integer)strlen(theName));
  char*                    n = &aBuff.ChangeFirst();
  strcpy(n, theName);
  Draw_ParseFailed = Standard_False;
  Standard_Real x  = Parse(n);
  while ((*n == ' ') || (*n == '\t'))
    n++;
  if (*n)
    Draw_ParseFailed = Standard_True;
  return x;
}

//=================================================================================================

bool Draw::ParseReal(const Standard_CString theExpressionString, Standard_Real& theParsedRealValue)
{
  const Standard_Real aParsedRealValue = Atof(theExpressionString);
  if (Draw_ParseFailed)
  {
    Draw_ParseFailed = Standard_False;
    return false;
  }
  theParsedRealValue = aParsedRealValue;
  return true;
}

//=================================================================================================

Standard_Integer Draw::Atoi(const Standard_CString name)
{
  return (Standard_Integer)Draw::Atof(name);
}

//=================================================================================================

bool Draw::ParseInteger(const Standard_CString theExpressionString,
                        Standard_Integer&      theParsedIntegerValue)
{
  Standard_Real aParsedRealValue = 0.0;
  if (!ParseReal(theExpressionString, aParsedRealValue))
  {
    return false;
  }
  const Standard_Integer aParsedIntegerValue = static_cast<Standard_Integer>(aParsedRealValue);
  if (static_cast<Standard_Real>(aParsedIntegerValue) != aParsedRealValue)
  {
    return false;
  }
  theParsedIntegerValue = aParsedIntegerValue;
  return true;
}

//=======================================================================
// function : Set
// purpose  : set a TCL var
//=======================================================================
void Draw::Set(const Standard_CString Name, const Standard_CString val)
{
  Standard_PCharacter pName, pVal;
  //
  pName = (Standard_PCharacter)Name;
  pVal  = (Standard_PCharacter)val;
  //
  Tcl_SetVar(Draw::GetInterpretor().Interp(), pName, pVal, 0);
}

//=================================================================================================

const NCollection_Map<Handle(Draw_Drawable3D)>& Draw::Drawables()
{
  return Draw_changeDrawables();
}

//=======================================================================
// Command management
// refresh the screen
//=======================================================================

static void before()
{
  repaint2d = Standard_False;
  repaint3d = Standard_False;
}

void Draw_RepaintNowIfNecessary()
{
  if (repaint2d)
    dout.Repaint2D();
  if (repaint3d)
    dout.Repaint3D();
  repaint2d = Standard_False;
  repaint3d = Standard_False;
}

static void after(Standard_Integer)
{
  Draw_RepaintNowIfNecessary();
}

extern void (*Draw_BeforeCommand)();
extern void (*Draw_AfterCommand)(Standard_Integer);

//=================================================================================================

void Draw::VariableCommands(Draw_Interpretor& theCommandsArg)
{
  static Standard_Boolean Done = Standard_False;
  if (Done)
    return;
  Done = Standard_True;

  // set up start and stop command
  Draw_BeforeCommand = &before;
  Draw_AfterCommand  = &after;

  // Register save/restore tools
  Draw_Number::RegisterFactory();

  // set up some variables
  const char*         n;
  Handle(Draw_Axis3D) theAxes3d = new Draw_Axis3D(gp_Pnt(0, 0, 0), Draw_bleu, 20);
  n                             = "axes";
  Draw::Set(n, theAxes3d);
  theAxes3d->Protected(Standard_True);

  Handle(Draw_Axis2D) theAxes2d = new Draw_Axis2D(gp_Pnt2d(0, 0), Draw_bleu, 20);
  n                             = "axes2d";
  Draw::Set(n, theAxes2d);
  theAxes2d->Protected(Standard_True);

  n = "pi";
  Draw::Set(n, M_PI);
  Draw::Get(n)->Protected(Standard_True);

  n = "pinf";
  Draw::Set(n, RealLast());
  Draw::Get(n)->Protected(Standard_True);

  n = "minf";
  Draw::Set(n, RealFirst());
  Draw::Get(n)->Protected(Standard_True);

  n                         = "grid";
  Handle(Draw_Grid) theGrid = new Draw_Grid();
  Draw::Set(n, theGrid);
  theGrid->Protected(Standard_True);

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
