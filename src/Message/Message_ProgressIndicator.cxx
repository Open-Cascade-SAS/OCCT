// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <Message_ProgressIndicator.ixx>

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
//  Show(Standard_False); // to update textual representation, if any
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
    
