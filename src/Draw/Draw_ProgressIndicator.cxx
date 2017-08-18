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
#include <Standard_Type.hxx>

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
  myDraw ( (Standard_Address)&di ),
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
    ((Draw_Interpretor*)myDraw)->Eval ( "destroy .xprogress" );
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
  char text[2048];
  Standard_Integer n = 0;
  n += Sprintf ( &text[n], "Progress: %.0f%%", 100. * GetPosition() );
  for ( Standard_Integer i=GetNbScopes(); i >=1; i-- ) {
    const Message_ProgressScale &scale = GetScope ( i );
    if ( scale.GetName().IsNull() ) continue; // skip unnamed scopes
    // if scope has subscopes, print end of subscope as its current position
    Standard_Real locPos = ( i >1 ? GetScope ( i-1 ).GetLast() : GetPosition() );
    // print progress info differently for finite and infinite scopes
    if ( scale.GetInfinite() )
      n += Sprintf ( &text[n], " %s: %.0f", scale.GetName()->ToCString(), 
                     scale.BaseToLocal ( locPos ) );
    else 
      n += Sprintf ( &text[n], " %s: %.0f / %.0f", scale.GetName()->ToCString(), 
                     scale.BaseToLocal ( locPos ), scale.GetMax() );
  }

  // Show graphic progress bar
  if ( myGraphMode ) {

    // In addition, write elapsed/estimated/remaining time
    if ( GetPosition() > 0.01 ) { 
      time_t aTimeT;
      time ( &aTimeT );
      Standard_Size aTime = (Standard_Size)aTimeT;
      n += Sprintf ( &text[n], "\nElapsed/estimated time: %ld/%.0f sec", 
                     (long)(aTime - myStartTime), ( aTime - myStartTime ) / GetPosition() );
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
      ((Draw_Interpretor*)myDraw)->Eval ( command );
      myShown = Standard_True;
    }
    char command[1024];
    Standard_Integer num = 0;
    num += Sprintf ( &command[num], ".xprogress.bar coords progress 2 2 %.0f 21;", 
                  1+400*GetPosition() );
    num += Sprintf ( &command[num], ".xprogress.bar coords progress_next 2 2 %.0f 21;", 
                  1+400*GetScope(1).GetLast() );
    num += Sprintf ( &command[num], ".xprogress.text configure -text \"%s\";", text );
    num += Sprintf ( &command[num], "update" );
    ((Draw_Interpretor*)myDraw)->Eval ( command );
  }

  // Print textual progress info
  if ( myTextMode )
    Message::DefaultMessenger()->Send (text, Message_Info);
  
  return Standard_True;
}
       
//=======================================================================
//function : UserBreak
//purpose  : 
//=======================================================================

Standard_Boolean Draw_ProgressIndicator::UserBreak()
{
  if ( StopIndicator() == this ) {
//    cout << "Progress Indicator - User Break: " << StopIndicator() << ", " << (void*)this << endl;
    myBreak = Standard_True;
    ((Draw_Interpretor*)myDraw)->Eval ( "XProgress -stop 0" );
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

Standard_Boolean &Draw_ProgressIndicator::DefaultTextMode () 
{
  static Standard_Boolean defTextMode = Standard_False;
  return defTextMode;
}
    
//=======================================================================
//function : DefaultGraphMode
//purpose  : 
//=======================================================================

Standard_Boolean &Draw_ProgressIndicator::DefaultGraphMode () 
{
  static Standard_Boolean defGraphMode = Standard_False;
  return defGraphMode;
}
    
//=======================================================================
//function : StopIndicator
//purpose  : 
//=======================================================================

Standard_Address &Draw_ProgressIndicator::StopIndicator ()
{
  static Standard_Address stopIndicator = 0;
  return stopIndicator;
}


