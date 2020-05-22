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
#include <Draw_Interpretor.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressScale.hxx>
#include <Precision.hxx>

#include <stdio.h>
#include <time.h>
IMPLEMENT_STANDARD_RTTIEXT(Draw_ProgressIndicator,Message_ProgressIndicator)

//=======================================================================
//function : Draw_ProgressIndicator
//purpose  : 
//=======================================================================
Draw_ProgressIndicator::Draw_ProgressIndicator (const Draw_Interpretor &di, Standard_Real theUpdateThreshold)
: myTextMode ( DefaultTextMode() ),
  myGraphMode ( DefaultGraphMode() ),
  myTclOutput ( DefaultTclOutput() ),
  myDraw ( (Draw_Interpretor*)&di ),
  myShown ( Standard_False ),
  myBreak ( Standard_False ),
  myUpdateThreshold ( 0.01 * theUpdateThreshold ),
  myLastPosition ( -1. ),
  myStartTime ( 0 )
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
  if ( myShown ) {
    myDraw->Eval ( "destroy .xprogress" );
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

Standard_Boolean Draw_ProgressIndicator::Show(const Standard_Boolean force)
{
  if ( ! myGraphMode && ! myTextMode )
    return Standard_False;

  // remember time of the first call to Show as process start time
  if ( ! myStartTime )
  {
    time_t aTimeT;
    time ( &aTimeT );
    myStartTime = (Standard_Size)aTimeT;
  }

  // unless show is forced, show updated state only if at least 1% progress has been reached since the last update
  Standard_Real aPosition = GetPosition();
  if ( ! force && aPosition < 1. && Abs (aPosition - myLastPosition) < myUpdateThreshold)
    return Standard_False; // return if update interval has not elapsed
  myLastPosition = aPosition;
  
  // Prepare textual progress info
  std::stringstream aText;
  aText.setf (std::ios::fixed, std:: ios::floatfield);
  aText.precision(0);
  aText << "Progress: " << 100. * GetPosition() << "%";
  for ( Standard_Integer i=GetNbScopes(); i >=1; i-- ) {
    const Message_ProgressScale &scale = GetScope ( i );
    if ( scale.GetName().IsNull() ) continue; // skip unnamed scopes
    aText << " " << scale.GetName()->ToCString() << ": ";

    // if scope has subscopes, print end of subscope as it s current position
    Standard_Real locPos = ( (i >1 && GetPosition()!=0) ? GetScope ( i-1 ).GetLast() : GetPosition() );
    // print progress info differently for finite and infinite scopes
    if ( scale.GetInfinite() )
    {
      Standard_Real aVal = scale.BaseToLocal(locPos);
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
      aText << scale.BaseToLocal ( locPos ) << " / " << scale.GetMax();
    }
  }

  // Show graphic progress bar
  if ( myGraphMode ) {

    // In addition, write elapsed/estimated/remaining time
    if ( GetPosition() > 0.01 ) { 
      time_t aTimeT;
      time ( &aTimeT );
      Standard_Size aTime = (Standard_Size)aTimeT;
      aText << "\nElapsed/estimated time: " << (long)(aTime - myStartTime) <<
               "/" << ( aTime - myStartTime ) / GetPosition() << " sec";
    }
  
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
    aCommand << ".xprogress.bar coords progress_next 2 2 " << (1 + 400 * GetScope(1).GetLast()) << " 21;";
    aCommand << ".xprogress.text configure -text \"" << aText.str() << "\";";
    aCommand << "update";
    myDraw->Eval (aCommand.str().c_str());
  }

  // Print textual progress info
  if (myTextMode)
  {
    if (myTclOutput && myDraw)
    {
      *myDraw << aText.str().c_str() << "\n";
    }
    else
    {
      std::cout << aText.str().c_str() << "\n";
    }
  }
  return Standard_True;
}

//=======================================================================
//function : UserBreak
//purpose  : 
//=======================================================================

Standard_Boolean Draw_ProgressIndicator::UserBreak()
{
  if ( StopIndicator() == this ) {
//    std::cout << "Progress Indicator - User Break: " << StopIndicator() << ", " << (void*)this << std::endl;
    myBreak = Standard_True;
    myDraw->Eval ( "XProgress -stop 0" );
  }
  return myBreak;
}
       
//=======================================================================
//function : SetTextMode
//purpose  : Sets text output mode (on/off)
//=======================================================================

void Draw_ProgressIndicator::SetTextMode(const Standard_Boolean theTextMode)
{
  myTextMode = theTextMode;
}

//=======================================================================
//function : GetTextMode
//purpose  : Returns text output mode (on/off)
//=======================================================================

Standard_Boolean Draw_ProgressIndicator::GetTextMode() const
{
  return myTextMode;
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
//function : DefaultTextMode
//purpose  : 
//=======================================================================

Standard_Boolean &Draw_ProgressIndicator::DefaultTextMode()
{
  static Standard_Boolean defTextMode = Standard_False;
  return defTextMode;
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
//function : DefaultTclOutput
//purpose  :
//=======================================================================

Standard_Boolean &Draw_ProgressIndicator::DefaultTclOutput()
{
  static Standard_Boolean defTclOutput = Standard_False;
  return defTclOutput;
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


