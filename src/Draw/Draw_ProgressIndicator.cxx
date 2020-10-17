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
IMPLEMENT_STANDARD_RTTIEXT(Draw_ProgressIndicator,Message_ProgressIndicator)

//=======================================================================
//function : Draw_ProgressIndicator
//purpose  : 
//=======================================================================
Draw_ProgressIndicator::Draw_ProgressIndicator (const Draw_Interpretor &di, Standard_Real theUpdateThreshold)
: myTclMode ( DefaultTclMode() ),
  myConsoleMode ( DefaultConsoleMode() ),
  myGraphMode ( DefaultGraphMode() ),
  myDraw ( (Draw_Interpretor*)&di ),
  myShown ( Standard_False ),
  myBreak ( Standard_False ),
  myUpdateThreshold ( 0.01 * theUpdateThreshold ),
  myLastPosition ( -1. ),
  myStartTime ( 0 ),
  myGuiThreadId (OSD_Thread::Current())
{
}

//=======================================================================
//function : ~Draw_ProgressIndicator
//purpose  : 
//=======================================================================

Draw_ProgressIndicator::~Draw_ProgressIndicator()
{
  Reset();
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void Draw_ProgressIndicator::Reset()
{
  Message_ProgressIndicator::Reset();
  if (myShown)
  {
    // eval will reset current string result - backup it beforehand
    const TCollection_AsciiString aTclResStr (myDraw->Result());
    myDraw->Eval ( "destroy .xprogress" );
    *myDraw << aTclResStr;
    myShown = Standard_False;
  }
  myBreak = Standard_False;
  myLastPosition = -1.;
  myStartTime = 0;
}

//=======================================================================
//function : Show
//purpose  : 
//=======================================================================

void Draw_ProgressIndicator::Show (const Message_ProgressScope& theScope, const Standard_Boolean force)
{
  if (!myGraphMode && !myTclMode && !myConsoleMode)
    return;

  // remember time of the first call to Show as process start time
  if ( ! myStartTime )
  {
    if (!myStartTime)
    {
      time_t aTimeT;
      time(&aTimeT);
      myStartTime = (Standard_Size)aTimeT;
    }
  }

  // unless show is forced, show updated state only if at least 1% progress has been reached since the last update
  Standard_Real aPosition = GetPosition();
  if ( ! force && aPosition < 1. && Abs (aPosition - myLastPosition) < myUpdateThreshold)
    return; // return if update interval has not elapsed

  myLastPosition = aPosition;
  
  // Prepare textual progress info
  std::stringstream aText;
  aText.setf (std::ios::fixed, std:: ios::floatfield);
  aText.precision(0);
  aText << "Progress: " << 100. * GetPosition() << "%";
  NCollection_List<const Message_ProgressScope*> aScopes;
  for (const Message_ProgressScope* aPS = &theScope; aPS; aPS = aPS->Parent())
    aScopes.Prepend(aPS);
  for (NCollection_List<const Message_ProgressScope*>::Iterator it(aScopes); it.More(); it.Next())
  {
    const Message_ProgressScope* aPS = it.Value();
    if (!aPS->Name()) continue; // skip unnamed scopes
    aText << " " << aPS->Name() << ": ";

    // print progress info differently for finite and infinite scopes
    Standard_Real aVal = aPS->Value();
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
    if ( GetPosition() > 0.01 ) { 
      time_t aTimeT;
      time ( &aTimeT );
      Standard_Size aTime = (Standard_Size)aTimeT;
      aText << "\nElapsed/estimated time: " << (long)(aTime - myStartTime) <<
               "/" << ( aTime - myStartTime ) / GetPosition() << " sec";
    }
  
    // eval will reset current string result - backup it beforehand
    const TCollection_AsciiString aTclResStr (myDraw->Result());
    if ( ! myShown ) {
      char command[1024];
      Sprintf ( command, "toplevel .xprogress -height 100 -width 410;"
                         "wm title .xprogress \"Progress\";"
                         "set xprogress_stop 0;"
                         "canvas .xprogress.bar -width 402 -height 22;"
                         ".xprogress.bar create rectangle 2 2 2 21 -fill blue -tags progress;"
                         ".xprogress.bar create rectangle 2 2 2 21 -outline black -tags progress_next;"
                         "message .xprogress.text -width 400 -text \"Progress 0%%\";"
                         "button .xprogress.stop -text \"Break\" -relief groove -width 9 -command {XProgress -stop %p};"
                         "pack .xprogress.bar .xprogress.text .xprogress.stop -side top;", this );
      myDraw->Eval ( command );
      myShown = Standard_True;
    }
    std::stringstream aCommand;
    aCommand.setf(std::ios::fixed, std::ios::floatfield);
    aCommand.precision(0);
    aCommand << ".xprogress.bar coords progress 2 2 " << (1 + 400 * GetPosition()) << " 21;";
    aCommand << ".xprogress.bar coords progress_next 2 2 " << (1 + 400 * theScope.GetPortion()) << " 21;";
    aCommand << ".xprogress.text configure -text \"" << aText.str() << "\";";
    aCommand << "update";

    myDraw->Eval (aCommand.str().c_str());
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

//=======================================================================
//function : UserBreak
//purpose  : 
//=======================================================================

Standard_Boolean Draw_ProgressIndicator::UserBreak()
{
  if ( StopIndicator() == this )
  {
//    std::cout << "Progress Indicator - User Break: " << StopIndicator() << ", " << (void*)this << std::endl;
    myBreak = Standard_True;
    myDraw->Eval ( "XProgress -stop 0" );
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
      myBreak = Standard_True;
    }
  }
  return myBreak;
}
       
//=======================================================================
//function : SetTclMode
//purpose  : Sets Tcl output mode (on/off)
//=======================================================================

void Draw_ProgressIndicator::SetTclMode(const Standard_Boolean theTclMode)
{
  myTclMode = theTclMode;
}

//=======================================================================
//function : GetTclMode
//purpose  : Returns Tcl output mode (on/off)
//=======================================================================

Standard_Boolean Draw_ProgressIndicator::GetTclMode() const
{
  return myTclMode;
}

//=======================================================================
//function : SetConsoleMode
//purpose  : Sets Console output mode (on/off)
//=======================================================================

void Draw_ProgressIndicator::SetConsoleMode(const Standard_Boolean theMode)
{
  myConsoleMode = theMode;
}

//=======================================================================
//function : GetConsoleMode
//purpose  : Returns Console output mode (on/off)
//=======================================================================

Standard_Boolean Draw_ProgressIndicator::GetConsoleMode() const
{
  return myConsoleMode;
}

//=======================================================================
//function : SetGraphMode
//purpose  : Sets graphical output mode (on/off)
//=======================================================================

void Draw_ProgressIndicator::SetGraphMode(const Standard_Boolean theGraphMode)
{
  myGraphMode = theGraphMode;
}

//=======================================================================
//function : GetGraphMode
//purpose  : Returns graphical output mode (on/off)
//=======================================================================

Standard_Boolean Draw_ProgressIndicator::GetGraphMode() const
{
  return myGraphMode;
}

//=======================================================================
//function : DefaultTclMode
//purpose  : 
//=======================================================================

Standard_Boolean &Draw_ProgressIndicator::DefaultTclMode()
{
  static Standard_Boolean defTclMode = Standard_False;
  return defTclMode;
}

//=======================================================================
//function : DefaultConsoleMode
//purpose  : 
//=======================================================================

Standard_Boolean &Draw_ProgressIndicator::DefaultConsoleMode()
{
  static Standard_Boolean defConsoleMode = Standard_False;
  return defConsoleMode;
}

//=======================================================================
//function : DefaultGraphMode
//purpose  : 
//=======================================================================

Standard_Boolean &Draw_ProgressIndicator::DefaultGraphMode()
{
  static Standard_Boolean defGraphMode = Standard_False;
  return defGraphMode;
}

//=======================================================================
//function : StopIndicator
//purpose  : 
//=======================================================================

Standard_Address &Draw_ProgressIndicator::StopIndicator()
{
  static Standard_Address stopIndicator = 0;
  return stopIndicator;
}
