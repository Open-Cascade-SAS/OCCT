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
// Updated :
// -------------------------------------------------------------

#include <Quantity_Date.hxx>
#include <Quantity_DateDefinitionError.hxx>
#include <Quantity_Period.hxx>
#include <Standard_OutOfRange.hxx>

static int  month_table[12] = {
  31,     // January
  28,     // February
  31,     // March
  30,     // April
  31,     // May
  30,     // June
  31,     // July
  31,     // August
  30,     // September
  31,     // October
  30,     // November
  31};    // December

static int SecondsByYear     = 365 * 24 * 3600 ; // Normal Year
static int SecondsByLeapYear = 366 * 24 * 3600 ; // Leap Year

// -----------------------------------------
// Initialize a date to January,1 1979 00:00
// -----------------------------------------

Quantity_Date::Quantity_Date(): mySec(0),myUSec(0) {}



// -----------------------------------------------------------
// IsValid : Checks the validity of a date
// This is the complete way month, day, year, ... micro second
// -----------------------------------------------------------


Standard_Boolean Quantity_Date::IsValid(const Standard_Integer mm,
                                        const Standard_Integer dd,
                                        const Standard_Integer yy,
                                        const Standard_Integer hh,
                                        const Standard_Integer mn,
                                        const Standard_Integer ss,
                                        const Standard_Integer mis,
                                        const Standard_Integer mics){

if (mm < 1 || mm > 12) return Standard_False;


if (yy < 1979 ) return Standard_False;


if ( Quantity_Date::IsLeap (yy) ) month_table[1] = 29;
else month_table[1] = 28;

if (dd < 1 || dd > month_table[mm-1]) return Standard_False;


if (hh < 0 || hh > 23) return Standard_False;


if (mn < 0 || mn > 59) return Standard_False;


if (ss < 0 || ss > 59) return Standard_False;


if (mis < 0 || mis > 999) return Standard_False;


if (mics < 0 || mics > 999) return Standard_False;

return Standard_True;

}

// -----------------------------------------------------------
// Initialize a Date :
// This is the complete way month, day, year, ... micro second
// -----------------------------------------------------------


Quantity_Date::Quantity_Date(const Standard_Integer mm,
                       const Standard_Integer dd,
                       const Standard_Integer yy,
                       const Standard_Integer hh,
                       const Standard_Integer mn,
                       const Standard_Integer ss,
                       const Standard_Integer mis,
                       const Standard_Integer mics){

SetValues (mm,dd,yy,hh,mn,ss,mis,mics);
}

// ------------------------------------------------------------
// Set values of a Date :
// This is the complete way month, day, year, ... micro second
// ------------------------------------------------------------

void Quantity_Date::SetValues(const Standard_Integer mm,
                           const Standard_Integer dd,
                           const Standard_Integer yy,
                           const Standard_Integer hh,
                           const Standard_Integer mn,
                           const Standard_Integer ss,
                           const Standard_Integer mis,
                           const Standard_Integer mics){

Standard_Integer i;

if ( ! Quantity_Date::IsValid (mm,dd,yy,hh,mn,ss,mis,mics))
   throw Quantity_DateDefinitionError("Quantity_Date::Quantity_Date invalid parameters");

if ( Quantity_Date::IsLeap (yy) ) month_table[1] = 29;
else month_table[1] = 28;

mySec  = 0;
myUSec = 0;
for(i = 1979; i < yy; i++) {
   if ( ! Quantity_Date::IsLeap (i) ) mySec += SecondsByYear;
   else mySec += SecondsByLeapYear;
   }

for(i = 1; i< mm; i++) {
   mySec += month_table[i-1] * 3600 * 24 ;
   }


mySec  += 3600 * 24 * (dd-1);

mySec  += 3600 * hh;

mySec  += 60   * mn;

mySec  += ss;

myUSec += mis * 1000;

myUSec += mics;

}


// ---------------------------------------------
// Values : Returns the values of a date
// ~~~~~~
// ---------------------------------------------

void Quantity_Date::Values(Standard_Integer& mm,
                           Standard_Integer& dd,
                           Standard_Integer& yy,
                           Standard_Integer& hh,
                           Standard_Integer& mn,
                           Standard_Integer& ss,
                           Standard_Integer& mis, 
                           Standard_Integer& mics)const{


Standard_Integer i,carry;


for(yy = 1979, carry = mySec ;; yy++) {
   if ( ! Quantity_Date::IsLeap (yy) ) 
      {
      month_table[1] = 28; // normal year
      if (carry >= SecondsByYear ) carry -= SecondsByYear;
      else break;
      }
   else 
      { 
      month_table[1] = 29; // Leap year
      if (carry >= SecondsByLeapYear) carry -= SecondsByLeapYear;
      else break;
      }
   }


for(mm = 1 ; ; mm++) {
   i = month_table[mm-1] * 3600 * 24;
   if ( carry >= i ) carry -= i;
   else break;
   }

i = 3600 * 24;
for(dd = 1 ; ; dd++) {
   if ( carry >= i ) carry -= i;
   else break;
   }

for(hh = 0 ; ; hh++) {
   if ( carry >= 3600 ) carry -= 3600;
   else break;
   }

for(mn = 0 ; ; mn++) {
   if ( carry >= 60 ) carry -= 60;
   else break;
   }

ss = carry;

mis  = myUSec / 1000;
mics = myUSec - ( mis * 1000);
}


// ---------------------------------------------------------------------
// Difference : Subtract a date to a given date; the result is a period 
// ~~~~~~~~~~   of time
// ---------------------------------------------------------------------

Quantity_Period Quantity_Date::Difference(const Quantity_Date& OtherDate){

Standard_Integer i1,i2;

if (mySec == 0 && myUSec == 0)
     {
     i1 = OtherDate.mySec;
     i2 = OtherDate.myUSec;
     }
else {
     i1 = mySec  - OtherDate.mySec ;
     i2 = myUSec - OtherDate.myUSec;
     }

if ( i1 >= 0 && i2 < 0 ) { 
   i1--;
   i2 = 1000000 + i2 ;
   }
else if ( i1 <0   && i2 >= 0 ) {
   i1 = Abs(i1);
   if ( i2 > 0 ){
      i1--;
      i2 = 1000000 - i2 ;
    }
}
else if ( i1 <0   && i2 <  0 ) {
   i1 = Abs(i1);
   i2 = Abs(i2);
}

Quantity_Period   result ( i1 , i2 );

return (result);
}


// ------------------------------------------------------------------
// Subtract : subtracts a period to a date and returns a date.
// ~~~~~~~~
// ------------------------------------------------------------------

Quantity_Date Quantity_Date::Subtract(const Quantity_Period& During){

Standard_Integer ss,mics;
Quantity_Date result;
result.mySec  = mySec;
result.myUSec = myUSec;
During.Values (ss,mics);

result.mySec  -= ss;
result.myUSec -= mics;

if ( result.mySec >= 0 && result.myUSec < 0 ) { 
   result.mySec--;
   result.myUSec = 1000000 + result.myUSec ;
   }


if ( result.mySec <0   )
   throw Quantity_DateDefinitionError(
   "Quantity_Date::Subtract : The result date is anterior to Jan,1 1979");

return (result); 

}


// ----------------------------------------------------------------------
// Add : Adds a period of time to a date
// ~~~
// ----------------------------------------------------------------------
Quantity_Date Quantity_Date::Add(const Quantity_Period& During){

Quantity_Date result;
During.Values (result.mySec,result.myUSec);
result.mySec  += mySec;
result.myUSec += myUSec;
if ( result.myUSec >= 1000000 ) {
   result.mySec++;
   result.myUSec -= 1000000;
   }
return (result);
}


// ----------------------------------------------------------------------
// Year : Return the year of a date
// ~~~~
// ----------------------------------------------------------------------
Standard_Integer Quantity_Date::Year(){
Standard_Integer dummy, year;
 Values(dummy, dummy, year, dummy, dummy, dummy, dummy, dummy);
 return (year);
}


// ----------------------------------------------------------------------
// Month : Return the month of a date
// ~~~~~
// ----------------------------------------------------------------------
Standard_Integer Quantity_Date::Month(){
Standard_Integer dummy, month;
 Values(month, dummy, dummy, dummy, dummy, dummy, dummy, dummy);
 return(month); 
}

// ----------------------------------------------------------------------
// Day : Return the day of a date
// ~~~
// ----------------------------------------------------------------------

Standard_Integer Quantity_Date::Day(){
Standard_Integer dummy, day;
 Values(dummy, day, dummy, dummy, dummy, dummy, dummy, dummy);
 return(day);
}

// ----------------------------------------------------------------------
// hour : Return the hour of a date
// ~~~~
// ----------------------------------------------------------------------

Standard_Integer Quantity_Date::Hour(){
Standard_Integer dummy, hour;
 Values(dummy, dummy, dummy, hour, dummy, dummy, dummy, dummy);
 return(hour);
}

// ----------------------------------------------------------------------
// Minute : Return the minute of a date
// ~~~~~~
// ----------------------------------------------------------------------

Standard_Integer Quantity_Date::Minute(){
Standard_Integer dummy, min;
 Values(dummy, dummy, dummy, dummy, min, dummy, dummy, dummy);
 return(min);
}

// ----------------------------------------------------------------------
// Second : Return the second of a date
// ~~~~~~
// ----------------------------------------------------------------------

Standard_Integer Quantity_Date::Second(){
Standard_Integer dummy, sec;
 Values(dummy, dummy, dummy, dummy, dummy, sec , dummy, dummy);
 return(sec);
}

// ----------------------------------------------------------------------
// millisecond : Return the millisecond of a date
// ~~~~~~~~~~~
// ----------------------------------------------------------------------

Standard_Integer Quantity_Date::MilliSecond(){
Standard_Integer dummy, msec;
 Values(dummy, dummy, dummy, dummy, dummy, dummy, msec, dummy);
return(msec);
}

// ----------------------------------------------------------------------
// Day : Return the day of a date
// ~~~
// ----------------------------------------------------------------------

Standard_Integer Quantity_Date::MicroSecond(){
Standard_Integer dummy, msec;
 Values(dummy, dummy, dummy, dummy, dummy, dummy, dummy, msec);
return(msec);
}

// ----------------------------------------------------------------------
// IsEarlier : Return true if the date is earlier than an other date
// ~~~~~~~~~
// ----------------------------------------------------------------------

Standard_Boolean Quantity_Date::IsEarlier(const Quantity_Date& other)const{
if (mySec < other.mySec) return Standard_True;
else if (mySec > other.mySec) return Standard_False;
else return ( ( myUSec < other.myUSec ) ? Standard_True : Standard_False);
}

// ----------------------------------------------------------------------
// IsLater : Return true if the date is later than an other date
// ~~~~~~~
// ----------------------------------------------------------------------

Standard_Boolean Quantity_Date::IsLater(const Quantity_Date& other)const{
if (mySec > other.mySec) return Standard_True;
else if (mySec < other.mySec) return Standard_False;
else return ( ( myUSec > other.myUSec ) ? Standard_True : Standard_False);
}


// ----------------------------------------------------------------------
// IsEqual : Return true if the date is the same than an other date
// ~~~~~~~
// ----------------------------------------------------------------------

Standard_Boolean Quantity_Date::IsEqual(const Quantity_Date& other)const{
return ( ( myUSec == other.myUSec &&
           mySec  == other.mySec     ) ? Standard_True : Standard_False);
}

