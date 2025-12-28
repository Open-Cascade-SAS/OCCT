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

#include <Draw_ProgressIndicator.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_List.hxx>
#include <Precision.hxx>
#include <OSD.hxx>
#include <OSD_Exception_CTRL_BREAK.hxx>
#include <OSD_Thread.hxx>

#include <stdio.h>
#include <time.h>
IMPLEMENT_STANDARD_RTTIEXT(Draw_ProgressIndicator, Message_ProgressIndicator)

//=================================================================================================

Draw_ProgressIndicator::Draw_ProgressIndicator(const Draw_Interpretor& di,
                                               double                  theUpdateThreshold)
    : myTclMode(DefaultTclMode()),
      myConsoleMode(DefaultConsoleMode()),
      myGraphMode(DefaultGraphMode()),
      myDraw((Draw_Interpretor*)&di),
      myShown(false),
      myBreak(false),
      myUpdateThreshold(0.01 * theUpdateThreshold),
      myLastPosition(-1.),
      myStartTime(0),
      myGuiThreadId(OSD_Thread::Current())
{
}

//=================================================================================================

Draw_ProgressIndicator::~Draw_ProgressIndicator()
{
  Reset();
}

//=================================================================================================

void Draw_ProgressIndicator::Reset()
{
  Message_ProgressIndicator::Reset();
  if (myShown)
  {
    // eval will reset current string result - backup it beforehand
    const TCollection_AsciiString aTclResStr(myDraw->Result());
    myDraw->Eval("destroy .xprogress");
    *myDraw << aTclResStr;
    myShown = false;
  }
  myBreak        = false;
  myLastPosition = -1.;
  myStartTime    = 0;
}

//=================================================================================================

void Draw_ProgressIndicator::Show(const Message_ProgressScope& theScope, const bool force)
{
  if (!myGraphMode && !myTclMode && !myConsoleMode)
    return;

  // remember time of the first call to Show as process start time
  if (!myStartTime)
  {
    if (!myStartTime)
    {
      time_t aTimeT;
      time(&aTimeT);
      myStartTime = (size_t)aTimeT;
    }
  }

  // unless show is forced, show updated state only if at least 1% progress has been reached since
  // the last update
  double aPosition = GetPosition();
  if (!force && (1. - aPosition) > Precision::Confusion()
      && std::abs(aPosition - myLastPosition) < myUpdateThreshold)
    return; // return if update interval has not elapsed

  myLastPosition = aPosition;

  // Prepare textual progress info
  std::stringstream aText;
  aText.setf(std::ios::fixed, std::ios::floatfield);
  aText.precision(0);
  aText << "Progress: " << 100. * GetPosition() << "%";
  NCollection_List<const Message_ProgressScope*> aScopes;
  for (const Message_ProgressScope* aPS = &theScope; aPS; aPS = aPS->Parent())
    aScopes.Prepend(aPS);
  for (NCollection_List<const Message_ProgressScope*>::Iterator it(aScopes); it.More(); it.Next())
  {
    const Message_ProgressScope* aPS = it.Value();
    if (!aPS->Name())
      continue; // skip unnamed scopes
    aText << " " << aPS->Name() << ": ";

    // print progress info differently for finite and infinite scopes
    double aVal = aPS->Value();
    if (aPS->IsInfinite())
    {
      if (Precision::IsInfinite(aVal))
      {
        aText << "finished";
      }
      else
      {
        aText << aVal;
      }
    }
    else
    {
      aText << aVal << " / " << aPS->MaxValue();
    }
  }

  // Show graphic progress bar.
  // It will be updated only within GUI thread.
  if (myGraphMode && myGuiThreadId == OSD_Thread::Current())
  {
    // In addition, write elapsed/estimated/remaining time
    if (GetPosition() > 0.01)
    {
      time_t aTimeT;
      time(&aTimeT);
      size_t aTime = (size_t)aTimeT;
      aText << "\nElapsed/estimated time: " << (long)(aTime - myStartTime) << "/"
            << (aTime - myStartTime) / GetPosition() << " sec";
    }

    // eval will reset current string result - backup it beforehand
    const TCollection_AsciiString aTclResStr(myDraw->Result());
    if (!myShown)
    {
      char command[1024];
      Sprintf(command,
              "toplevel .xprogress -height 100 -width 410;"
              "wm title .xprogress \"Progress\";"
              "set xprogress_stop 0;"
              "canvas .xprogress.bar -width 402 -height 22;"
              ".xprogress.bar create rectangle 2 2 2 21 -fill blue -tags progress;"
              ".xprogress.bar create rectangle 2 2 2 21 -outline black -tags progress_next;"
              "message .xprogress.text -width 400 -text \"Progress 0%%\";"
              "button .xprogress.stop -text \"Break\" -relief groove -width 9 -command {XProgress "
              "-stop %p};"
              "pack .xprogress.bar .xprogress.text .xprogress.stop -side top;",
              this);
      myDraw->Eval(command);
      myShown = true;
    }
    std::stringstream aCommand;
    aCommand.setf(std::ios::fixed, std::ios::floatfield);
    aCommand.precision(0);
    aCommand << ".xprogress.bar coords progress 2 2 " << (1 + 400 * GetPosition()) << " 21;";
    aCommand << ".xprogress.bar coords progress_next 2 2 " << (1 + 400 * theScope.GetPortion())
             << " 21;";
    aCommand << ".xprogress.text configure -text \"" << aText.str() << "\";";
    aCommand << "update";

    myDraw->Eval(aCommand.str().c_str());
    *myDraw << aTclResStr;
  }

  // Print textual progress info
  if (myTclMode && myDraw)
  {
    *myDraw << aText.str().c_str() << "\n";
  }
  if (myConsoleMode)
  {
    std::cout << aText.str().c_str() << "\n";
  }
}

//=================================================================================================

bool Draw_ProgressIndicator::UserBreak()
{
  if (StopIndicator() == this)
  {
    //    std::cout << "Progress Indicator - User Break: " << StopIndicator() << ", " << (void*)this
    //    << std::endl;
    myBreak = true;
    myDraw->Eval("XProgress -stop 0");
  }
  else
  {
    // treatment of Ctrl-Break signal
    try
    {
      OSD::ControlBreak();
    }
    catch (const OSD_Exception_CTRL_BREAK&)
    {
      myBreak = true;
    }
  }
  return myBreak;
}

//=======================================================================
// function : SetTclMode
// purpose  : Sets Tcl output mode (on/off)
//=======================================================================

void Draw_ProgressIndicator::SetTclMode(const bool theTclMode)
{
  myTclMode = theTclMode;
}

//=======================================================================
// function : GetTclMode
// purpose  : Returns Tcl output mode (on/off)
//=======================================================================

bool Draw_ProgressIndicator::GetTclMode() const
{
  return myTclMode;
}

//=======================================================================
// function : SetConsoleMode
// purpose  : Sets Console output mode (on/off)
//=======================================================================

void Draw_ProgressIndicator::SetConsoleMode(const bool theMode)
{
  myConsoleMode = theMode;
}

//=======================================================================
// function : GetConsoleMode
// purpose  : Returns Console output mode (on/off)
//=======================================================================

bool Draw_ProgressIndicator::GetConsoleMode() const
{
  return myConsoleMode;
}

//=======================================================================
// function : SetGraphMode
// purpose  : Sets graphical output mode (on/off)
//=======================================================================

void Draw_ProgressIndicator::SetGraphMode(const bool theGraphMode)
{
  myGraphMode = theGraphMode;
}

//=======================================================================
// function : GetGraphMode
// purpose  : Returns graphical output mode (on/off)
//=======================================================================

bool Draw_ProgressIndicator::GetGraphMode() const
{
  return myGraphMode;
}

//=================================================================================================

bool& Draw_ProgressIndicator::DefaultTclMode()
{
  static bool defTclMode = false;
  return defTclMode;
}

//=================================================================================================

bool& Draw_ProgressIndicator::DefaultConsoleMode()
{
  static bool defConsoleMode = false;
  return defConsoleMode;
}

//=================================================================================================

bool& Draw_ProgressIndicator::DefaultGraphMode()
{
  static bool defGraphMode = false;
  return defGraphMode;
}

//=================================================================================================

void*& Draw_ProgressIndicator::StopIndicator()
{
  static void* stopIndicator = 0;
  return stopIndicator;
}
