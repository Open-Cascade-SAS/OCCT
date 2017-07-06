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


#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressScale.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_ProgressIndicator,Standard_Transient)

//=======================================================================
//function : Message_ProgressIndicator
//purpose  : 
//=======================================================================
Message_ProgressIndicator::Message_ProgressIndicator ()
{
  Reset();
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void Message_ProgressIndicator::Reset ()
{
  myPosition = 0.;
  
  Message_ProgressScale scale;
  scale.SetName ( "Step" );
  scale.SetSpan ( 0., 1. );

  myScopes.Clear();
  myScopes.Append ( scale );
}

//=======================================================================
//function : SetScale
//purpose  : 
//=======================================================================

void Message_ProgressIndicator::SetScale (const Standard_Real min, 
                                           const Standard_Real max, 
                                           const Standard_Real step,
                                           const Standard_Boolean isInf)
{
  Message_ProgressScale &scale = myScopes.ChangeValue(1);
  scale.SetRange ( min, max );
  scale.SetStep ( step );
  scale.SetInfinite ( isInf );  
}
    
//=======================================================================
//function : GetScale
//purpose  : 
//=======================================================================

void Message_ProgressIndicator::GetScale (Standard_Real &min, 
                                           Standard_Real &max, 
                                           Standard_Real &step,
                                           Standard_Boolean &isInf) const
{
  const Message_ProgressScale &scale = myScopes(1);
  min   = scale.GetMin();
  max   = scale.GetMax();
  step  = scale.GetStep();
  isInf = scale.GetInfinite();
}
	
//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void Message_ProgressIndicator::SetValue (const Standard_Real val)
{
  const Message_ProgressScale &scale = myScopes(1);
  Standard_Real p = scale.LocalToBase ( val );
  if ( myPosition < p ) {
    myPosition = Min ( p, 1. );
    Show(Standard_False);
  }
}

//=======================================================================
//function : GetValue
//purpose  : 
//=======================================================================

Standard_Real Message_ProgressIndicator::GetValue () const
{
  return myScopes(1).BaseToLocal ( myPosition );
}
    
//=======================================================================
//function : NewScope
//purpose  : 
//=======================================================================

Standard_Boolean Message_ProgressIndicator::NewScope (const Standard_Real span,
                                                       const Handle(TCollection_HAsciiString) &name)
{
  Message_ProgressScale scale;
  scale.SetName ( name );
  scale.SetSpan ( myPosition, myScopes(1).LocalToBase ( GetValue() + span ) );
  myScopes.Prepend ( scale );
  Show(Standard_False); // to update textual representation, if any
  return myPosition < 1.;
}

//=======================================================================
//function : EndScope
//purpose  : 
//=======================================================================

Standard_Boolean Message_ProgressIndicator::EndScope ()
{
  Standard_Real end = myScopes(1).GetLast();
  Standard_Boolean ret = ( myScopes.Length() >1 );
  if ( ret ) myScopes.Remove(1);
  if ( myPosition != end ) {
    myPosition = end;
    Show(Standard_False);
  }
  return ret;
}

//=======================================================================
//function : NextScope
//purpose  : 
//=======================================================================

Standard_Boolean Message_ProgressIndicator::NextScope (const Standard_Real span, 
                                                        const Standard_CString name)
{
  Message_ProgressScale &scale = myScopes.ChangeValue(1);
  if ( myPosition != scale.GetLast() ) {
    myPosition = scale.GetLast();
    Show(Standard_False);
  }
  if ( myScopes.Length() <2 ) return Standard_False;

  if ( name ) scale.SetName ( name );
  const Message_ProgressScale &scale2 = myScopes(2);
  scale.SetSpan ( myPosition, scale2.LocalToBase ( scale2.BaseToLocal(myPosition) + span ) );
//   if ( myMax - myMin <= gp::Resolution() ) return myLast;
//   Standard_Real next = ( myMax - myMin <= gp::Resolution() ? 1. - myPosition :
//                          span * ( scale2.GetLast() - scale2.GetFirst() ) / 
//                                 ( scale2.GetMax()  - scale2.GetMin() ) );
//   scale.SetSpan ( myPosition, myPosition + next );
  return myPosition < 1.;
}

//=======================================================================
//function : UserBreak
//purpose  : 
//=======================================================================

Standard_Boolean Message_ProgressIndicator::UserBreak () 
{
  return Standard_False;
}
    
