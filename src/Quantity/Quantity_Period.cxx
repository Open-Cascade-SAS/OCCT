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

// -------------------------------------------------------------
// C matra datavision 1993
// Period class implementation.
// Updated :
// -------------------------------------------------------------

#include <Quantity_Period.hxx>
#include <Quantity_PeriodDefinitionError.hxx>

// -----------------------------------------------------------
// IsValid : Checks the validity of a date
// With:      
// 0 <= dd
// 0 <= hh 
// 0 <= mn 
// 0 <= ss 
// 0 <= mis
// 0 <= mics 
// -----------------------------------------------------------
Standard_Boolean Quantity_Period::IsValid(
                                        const Standard_Integer dd,
                                        const Standard_Integer hh,
                                        const Standard_Integer mn,
                                        const Standard_Integer ss,
                                        const Standard_Integer mis,
                                        const Standard_Integer mics){

return ( (dd < 0 || hh < 0 || mn < 0 || 
          ss < 0 || mis < 0 || mics < 0 ) ? Standard_False : Standard_True );
}
// -------------------------------------------------------------
// IsValid : Checks the validity of a date
// With:      
// 0 <= ss 
// 0 <= mics 
// -------------------------------------------------------------
Standard_Boolean Quantity_Period::IsValid(
                                        const Standard_Integer ss,
                                        const Standard_Integer mics){

return ( (ss < 0 || mics < 0 ) ? Standard_False : Standard_True );
}

// -------------------------------------------------------------
// Create : Creates a period with a number of seconds 
// ~~~~~~   and microseconds.
//  
// -------------------------------------------------------------
Quantity_Period::Quantity_Period(const Standard_Integer dd,
                                 const Standard_Integer hh,
                                 const Standard_Integer mn,
                                 const Standard_Integer ss,
                                 const Standard_Integer mils,
                                 const Standard_Integer mics){

SetValues (dd,hh,mn,ss,mils,mics);
}

// -------------------------------------------------------------
// Create : Creates a period with a number of seconds 
// ~~~~~~   and microseconds.
//  
// -------------------------------------------------------------
Quantity_Period::Quantity_Period(const Standard_Integer ss,
                                 const Standard_Integer mics){
                          
  SetValues(ss,mics);
}


// -------------------------------------------------------------
// Values : Returns a period with the number of days,hours,
// ~~~~~~   minutes,seconds,milliseconds and microseconds.
// -------------------------------------------------------------
void Quantity_Period::Values(
                         Standard_Integer& dd,
                         Standard_Integer& hh,
                         Standard_Integer& mn,
                         Standard_Integer& ss,
                         Standard_Integer& mis, 
                         Standard_Integer& mics
                                     )const{
Standard_Integer carry = mySec;
dd    = carry / ( 24 * 3600 );
carry -= dd * 24 * 3600 ;
hh    = carry / 3600;
carry -= 3600 * hh;
mn    = carry / 60;
carry -= mn * 60;
ss    = carry;
mis   = myUSec / 1000;
mics  = myUSec - ( mis * 1000);
}

// -------------------------------------------------------------
// Values : Returns a period with the number of seconds and 
// ~~~~~~   microseconds.
// -------------------------------------------------------------
void Quantity_Period::Values(
                         Standard_Integer& ss,
                         Standard_Integer& mics
                                     )const{

ss   = mySec;
mics = myUSec;
}

// -------------------------------------------------------------
// SetValues : Sets a period with a number of days,hours,minutes,
// ~~~~~~~~~   seconds and microseconds.
// -------------------------------------------------------------
void Quantity_Period::SetValues( const Standard_Integer dd,
                                 const Standard_Integer hh,
                                 const Standard_Integer mn,
                                 const Standard_Integer ss,
                                 const Standard_Integer mils,
                                 const Standard_Integer mics){
SetValues( ( dd * 24 * 3600 ) + ( hh * 3600 ) + ( 60 * mn ) + ss , 
           mils * 1000 + mics );
}

// -------------------------------------------------------------
// SetValues : Sets a period with a number of seconds and 
// ~~~~~~~~~   microseconds.
// -------------------------------------------------------------
void Quantity_Period::SetValues(
                         const Standard_Integer ss,
                         const Standard_Integer mics) {

if ( ! Quantity_Period::IsValid(ss,mics) )
   throw Quantity_PeriodDefinitionError("Quantity_Period::SetValues invalid parameters");

mySec  = ss;
myUSec = mics;
while ( myUSec > 1000000 )
  {
  myUSec -= 1000000;
  mySec++;
  }
}                                   
// -------------------------------------------------------------
// Subtract : Subtracts a period to another period
// ~~~~~~~~
// -------------------------------------------------------------
Quantity_Period Quantity_Period::Subtract(const Quantity_Period& 
                                          OtherPeriod)const{
Quantity_Period result (mySec,myUSec);


result.mySec  -= OtherPeriod.mySec;
result.myUSec -= OtherPeriod.myUSec;

if ( result.mySec >= 0 && result.myUSec < 0 ) { 
   result.mySec--;
   result.myUSec = 1000000 + result.myUSec ;
   }
else if ( result.mySec <0   && result.myUSec >= 0 ) {
   result.mySec = Abs(result.mySec);
   if ( result.myUSec > 0 ){
      result.mySec--;
      result.myUSec = 1000000 - result.myUSec ;
    }
}
else if ( result.mySec <0   && result.myUSec <  0 ) {
   result.mySec = Abs(result.mySec);
   result.myUSec = Abs(result.myUSec);
}
return (result);
}

// -------------------------------------------------------------
// Add : Adds a period to another period
// ~~~
// -------------------------------------------------------------
Quantity_Period Quantity_Period::Add(const Quantity_Period& OtherPeriod)
                                 const{

Quantity_Period result (mySec,myUSec);
result.mySec  += OtherPeriod.mySec;
result.myUSec += OtherPeriod.myUSec;
if (result.myUSec > 1000000)
  {
  result.myUSec -= 1000000;
  result.mySec++;
  }
return (result);
}


// -------------------------------------------------------------
// IsEqual : returns true if two periods are equal
// ~~~~~~~
// -------------------------------------------------------------
Standard_Boolean Quantity_Period::IsEqual(const Quantity_Period& 
                                             OtherPeriod)const{

return 
( ( mySec  == OtherPeriod.mySec &&
    myUSec == OtherPeriod.myUSec ) ? Standard_True : Standard_False);
}


// -------------------------------------------------------------
// IsShorter : returns true if a date is shorter then an other 
// ~~~~~~~~~   date
// -------------------------------------------------------------
Standard_Boolean Quantity_Period::IsShorter(
               const Quantity_Period& OtherPeriod)const{

if ( mySec < OtherPeriod.mySec ) return Standard_True;
else if ( mySec > OtherPeriod.mySec ) return Standard_False;
else return 
   ( ( myUSec < OtherPeriod.myUSec ) ? Standard_True : Standard_False);
}


// -------------------------------------------------------------
// IsLonger : returns true if a date is longer then an other 
// ~~~~~~~~   date
// -------------------------------------------------------------
Standard_Boolean Quantity_Period::IsLonger(
                const Quantity_Period& OtherPeriod)const{

if ( mySec > OtherPeriod.mySec ) return Standard_True;
else if ( mySec < OtherPeriod.mySec ) return Standard_False;
else return 
   ( ( myUSec > OtherPeriod.myUSec ) ? Standard_True : Standard_False);
}

  
