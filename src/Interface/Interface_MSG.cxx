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


#include <Interface_MSG.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_Process.hxx>
#include <Quantity_Date.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Stream.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>

#include <stdio.h>
static NCollection_DataMap<TCollection_AsciiString, Handle(TCollection_HAsciiString)> thedic;
static NCollection_DataMap<TCollection_AsciiString, Standard_Integer>                 thelist;
static Handle(TColStd_HSequenceOfHAsciiString) thedup;
static Standard_Boolean theprint  = Standard_True;
static Standard_Boolean therec    = Standard_False;
static Standard_Boolean therun    = Standard_False;
static Standard_Boolean theraise  = Standard_False;

static char blank[] =
"                                                                            ";
static Standard_Integer maxblank = (Standard_Integer) strlen(blank);


Interface_MSG::Interface_MSG (const Standard_CString key)
    : thekey (key) , theval (NULL)
{
  
}


Interface_MSG::Interface_MSG
  (const Standard_CString key, const Standard_Integer i1)
    : thekey (key) , theval (NULL)
{
  char mess[300];
  sprintf (mess, Interface_MSG::Translated(thekey), i1);
  theval = new char[strlen (mess) + 1];
  strcpy (theval,mess);
}


Interface_MSG::Interface_MSG
  (const Standard_CString key, const Standard_Integer i1, const Standard_Integer i2)
    : thekey (key) , theval (NULL)
{
  char mess[300];
  sprintf (mess, Interface_MSG::Translated(thekey), i1,i2);
  theval = new char[strlen (mess) + 1];
  strcpy (theval,mess);
}


Interface_MSG::Interface_MSG
  (const Standard_CString key, const Standard_Real r1, const Standard_Integer intervals)
    : thekey (key) , theval (NULL)
{
  char mess[300];
  sprintf (mess, Interface_MSG::Translated(thekey),
	   (intervals < 0 ? r1 : Interface_MSG::Intervalled(r1,intervals)) );
  theval = new char[strlen (mess) + 1];
  strcpy (theval,mess);
}


Interface_MSG::Interface_MSG
  (const Standard_CString key, const Standard_CString str)
    : thekey (key) , theval (NULL)
{
  char mess[300];
  sprintf (mess, Interface_MSG::Translated(thekey), str);
  theval = new char[strlen (mess) + 1];
  strcpy (theval,mess);
}


Interface_MSG::Interface_MSG
  (const Standard_CString key,
   const Standard_Integer val, const Standard_CString str)
    : thekey (key) , theval (NULL)
{
  char mess[300];
  sprintf (mess, Interface_MSG::Translated(thekey), val, str);
  theval = new char[strlen (mess) + 1];
  strcpy (theval,mess);
}


Standard_CString  Interface_MSG::Value () const
{  return (theval ? theval : Interface_MSG::Translated(thekey));
 }


void  Interface_MSG::Destroy ()
{
  if (theval)  {
    delete [] theval;
    theval = NULL;
  }
}


Interface_MSG::operator Standard_CString () const
{
  return Value();
}


//  ###########    Lecture Ecriture Fichier    ##########

Standard_Integer  Interface_MSG::Read (Standard_IStream& S)
{
  Standard_Integer i,nb = 0;
  char buf[200], key[200];
  buf[0] = '\0';
  while (S.getline (buf,200)) {
    if (buf[0] == '@' && buf[1] == '@') continue;
    if (buf[0] == '\0') continue;
    if (buf[0] == '@') {
      nb ++;
      for (i = 1; i <= 199; i ++) {
	key[i-1] = buf[i];
	if (buf[i] == '\0') break;
      }
    }
    else Record (key,buf);
    buf[0] = '\0';
  }
  return nb;
}

    Standard_Integer  Interface_MSG::Read (const Standard_CString file)
{
  ifstream S(file);
  if (!S) return -1;
  return Read (S);
}

    Standard_Integer  Interface_MSG::Write
  (Standard_OStream& S, const Standard_CString rootkey)
{
  Standard_Integer nb = 0;
  if (thedic.IsEmpty()) return nb;
  if (rootkey[0] != '\0') S<<"@@ ROOT:"<<rootkey<<endl;
  NCollection_DataMap<TCollection_AsciiString, Handle(TCollection_HAsciiString)>::Iterator iter(thedic);
  for (; iter.More(); iter.Next()) {
    if (!iter.Key().StartsWith(rootkey)) continue;
    S<<"@"<<iter.Key()<<"\n";
    const Handle(TCollection_HAsciiString) str = iter.Value();
    if (str.IsNull()) continue;
    nb ++;
    S<<str->ToCString()<<"\n";
  }
  S<<flush;
  return nb;
}


//  ###########   EXPLOITATION   ##########

Standard_Boolean  Interface_MSG::IsKey (const Standard_CString key)
{
  return (key[0] == '^');
}


Standard_CString  Interface_MSG::Translated (const Standard_CString key)
{
  if (!therun) return key;
  if (!thedic.IsEmpty()) {
    Handle(TCollection_HAsciiString) str;
    if (thedic.Find(key, str))
      return str->ToCString();
  }
  if (theprint) cout<<" **  Interface_MSG:Translate ?? "<<key<<"  **"<<endl;
  if (therec) {
    if (thelist.IsBound(key)) {
      thelist.ChangeFind(key)++;
    } else
      thelist.Bind(key, 1);
  }
  if (theraise) throw Standard_DomainError("Interface_MSG : Translate");
  return key;
}


void  Interface_MSG::Record
(const Standard_CString key, const Standard_CString item)
{
  Handle(TCollection_HAsciiString) dup;
  Handle(TCollection_HAsciiString) str = new TCollection_HAsciiString(item);
  if (thedic.IsBound(key)) {
    thedic.ChangeFind(key) = str;
  } else {
    thedic.Bind(key,str);
    return;
  }
  if (theprint) cout<<" **  Interface_MSG:Record ?? "<<key<<" ** "<<item<<"  **"<<endl;
  if (therec) {
    if (thedup.IsNull()) thedup = new TColStd_HSequenceOfHAsciiString();
    dup = new TCollection_HAsciiString(key);
    thedup->Append(dup);
    dup = new TCollection_HAsciiString(item);
    thedup->Append(dup);
  }
  if (theraise) throw Standard_DomainError("Interface_MSG : Record");
}


void  Interface_MSG::SetTrace
  (const Standard_Boolean toprint, const Standard_Boolean torecord)
{
  theprint = toprint;
  therec = torecord;
}


void  Interface_MSG::SetMode (const Standard_Boolean running,
                              const Standard_Boolean raising)
{
  therun = running;  theraise = raising;
}


void  Interface_MSG::PrintTrace (Standard_OStream& S)
{
  Handle(TCollection_HAsciiString) dup;
  Standard_Integer i, nb = 0;
  if (!thedup.IsNull()) nb = thedup->Length()/2;
  for (i = 1; i <= nb; i ++) {
    dup = thedup->Value(2*i-1);
    S<<"** DUP:"<<dup->ToCString();
    dup = thedup->Value(2*i);
    S<<" ** "<<dup->ToCString()<<endl;
  }

  if (thelist.IsEmpty()) return;
  NCollection_DataMap<TCollection_AsciiString, Standard_Integer>::Iterator iter(thelist);
  for (; iter.More(); iter.Next()) {
    S<<"** MSG(NB="<<iter.Value()<<"): "<<iter.Key()<<endl;
  }
}


//  ###########    ARRONDIS DE FLOTTANTS    ############

Standard_Real  Interface_MSG::Intervalled
  (const Standard_Real val,
   const Standard_Integer order, const Standard_Boolean upper)
{
  Standard_Real vl = (val > 0. ? val : -val);
  Standard_Real bl = 1., bu = 1.;
  if (vl >= 1.) {
    bu = 10.;
    for (Standard_Integer i = 0; i < 200; i ++) {
      if (vl <  bu) break;
      bl = bu;  bu *= 10.;
    }
  } else {
    bl = 0.1;
    for (Standard_Integer i = 0; i < 200; i ++) {
      if (vl >= bl) break;
      bu = bl;  bl /= 10.;
    }
    if (vl == 0.) return 0.;
  }

  Standard_Real rst = vl/bl;
  if        (order <= 1) rst = (upper ? 10. : 1.);
  else if   (order == 2) {
    if (rst <= 3.) rst = (upper ?  3. : 1.);
    else           rst = (upper ? 10. : 3.);
  } else if (order == 3) {
    if      (rst <= 2.) rst = (upper ?  2. : 1.);
    else if (rst <= 5.) rst = (upper ?  5. : 2.);
    else                rst = (upper ? 10. : 5.);
  } else if (order == 4) {
    if      (rst <= 2.) rst = (upper ?  2. : 1.);
    else if (rst <= 3.) rst = (upper ?  3. : 2.);
    else if (rst <= 6.) rst = (upper ?  6. : 3.);
    else                rst = (upper ? 10. : 6.);
  }
  else if (order <= 6) {
    if      (rst <= 1.5) rst = (upper ?  1.5 : 1. );
    else if (rst <= 2. ) rst = (upper ?  2.  : 1.5);
    else if (rst <= 3. ) rst = (upper ?  3.  : 2. );
    else if (rst <= 5. ) rst = (upper ?  5.  : 3. );
    else if (rst <= 7. ) rst = (upper ?  7.  : 5. );
    else                 rst = (upper ? 10.  : 7. );
  }
  else {   // n a de sens que jusqu a 10 ...
    if      (rst <= 1.2) rst = (upper ?  1.2 : 1. );
    else if (rst <= 1.5) rst = (upper ?  1.5 : 1.2);
    else if (rst <= 2. ) rst = (upper ?  2.  : 1.5);
    else if (rst <= 2.5) rst = (upper ?  2.5 : 2. );
    else if (rst <= 3. ) rst = (upper ?  3.  : 2.5);
    else if (rst <= 4. ) rst = (upper ?  4.  : 3. );
    else if (rst <= 5. ) rst = (upper ?  5.  : 4. );
    else if (rst <= 6. ) rst = (upper ?  6.  : 5. );
    else if (rst <= 8. ) rst = (upper ?  8.  : 6. );
    else                 rst = (upper ? 10.  : 8. );
  }
  return ((val < 0.) ? -(bl*rst) : (bl*rst) );
}


//  ###########    DATES    ############

void  Interface_MSG::TDate (const Standard_CString text,
                            const Standard_Integer yy,
                            const Standard_Integer mm,
                            const Standard_Integer dd,
                            const Standard_Integer hh,
                            const Standard_Integer mn,
                            const Standard_Integer ss,
                            const Standard_CString format)
{
//  valeurs nulles : en tete (avec au moins une non nulle, la derniere)
//  -> completees avec les valeurs actuelle (system date)
//  tout nul on laisse

  //svv #2 Standard_Integer y1 , m1 , d1 , h1 , n1 , s1;
  Standard_Integer y2 = yy, m2 = mm, d2 = dd, h2 = hh, n2 = mn, s2 = ss;
  if (yy == 0 && ss != 0) {
//  completion
    OSD_Process pourdate;
    Quantity_Date ladate = pourdate.SystemDate ();
    if ( yy == 0) {
      y2 = ladate.Year();
      if ( mm == 0) {
	m2 = ladate.Month();
	if (dd == 0) {
	  d2 = ladate.Day();
	  if (hh == 0) {
	    h2 = ladate.Hour();
	    if (mn == 0) {
	      n2 = ladate.Minute();
	      s2 = ladate.Second();
	    }
	  }
	}
      }
    }
  }
  char *pText=(char *)text;
  if (!format || format[0] == '\0')
    sprintf(pText,"%4.4d-%2.2d-%2.2d:%2.2d-%2.2d-%2.2d",y2,m2,d2,h2,n2,s2);
  else if ((format[0] == 'c' || format[0] == 'C') && format[1] == ':')
    sprintf (pText,&format[2],y2,m2,d2,h2,n2,s2);
}


Standard_Boolean  Interface_MSG::NDate (const Standard_CString text,
                                        Standard_Integer& yy,
                                        Standard_Integer& mm,
                                        Standard_Integer& dd,
                                        Standard_Integer& hh,
                                        Standard_Integer& mn,
                                        Standard_Integer& ss)
{
  Standard_Integer i ,num = 1;
  for (i = 0; text[i] != '\0'; i ++) {
    char val = text[i];
    if (val >= 48 && val <= 57) {
      if ( (num & 1) == 0) num ++;
      if (num ==  1) yy = yy*10 + (val-48);
      if (num ==  3) mm = mm*10 + (val-48);
      if (num ==  5) dd = dd*10 + (val-48);
      if (num ==  7) hh = hh*10 + (val-48);
      if (num ==  9) mn = mn*10 + (val-48);
      if (num == 11) ss = ss*10 + (val-48);
    }
    else if ( (num & 1) != 0) num ++;
  }
  return (num > 0);
}


Standard_Integer  Interface_MSG::CDate (const Standard_CString text1,
                                        const Standard_CString text2)
{
  Standard_Integer i1=0,i2=0,i3=0,i4=0,i5=0,i6=0,j1=0,j2=0,j3=0,j4=0,j5=0,j6=0;
  if (!NDate (text1,i1,i2,i3,i4,i5,i6)) return 0;
  if (!NDate (text2,j1,j2,j3,j4,j5,j6)) return 0;
  if (i1 < j1) return -1;
  if (i1 > j1) return  1;
  if (i2 < j2) return -1;
  if (i2 > j2) return  1;
  if (i3 < j3) return -1;
  if (i3 > j3) return  1;
  if (i4 < j4) return -1;
  if (i4 > j4) return  1;
  if (i5 < j5) return -1;
  if (i5 > j5) return  1;
  if (i6 < j6) return -1;
  if (i6 > j6) return  1;
  return 0;
}


Standard_CString  Interface_MSG::Blanks (const Standard_Integer val,
                                         const Standard_Integer max)
{
 Standard_Integer count;
  if (val < 0)  return Interface_MSG::Blanks (-val,max-1);
  if      (val <         10) count = 9;
  else if (val <        100) count = 8;
  else if (val <       1000) count = 7;
  else if (val <      10000) count = 6;
  else if (val <     100000) count = 5;
  else if (val <    1000000) count = 4;
  else if (val <   10000000) count = 3;
  else if (val <  100000000) count = 2;
  else if (val < 1000000000) count = 1;
  else count = 0;
  count = count + max - 10;
  if (count < 0) count = 0;
  return &blank [maxblank - count];
}


Standard_CString  Interface_MSG::Blanks (const Standard_CString val,
                                         const Standard_Integer max)
{
  Standard_Integer lng = (Standard_Integer) strlen(val);
  if (lng > maxblank || lng > max) return "";
  return &blank [maxblank - max + lng];
}


Standard_CString  Interface_MSG::Blanks
  (const Standard_Integer count)
{
  if (count <= 0) return "";
  if (count >= maxblank) return blank;
  return &blank [maxblank-count];
}


void  Interface_MSG::Print (Standard_OStream& S, const Standard_CString val,
                            const Standard_Integer max,
                            const Standard_Integer just)
{
  if (max > maxblank)  {  Print(S,val,maxblank,just);  return;  }
  Standard_Integer lng = (Standard_Integer) strlen (val);
  if (lng > max)  {  S << val;  return;  }
  Standard_Integer m1 = (max-lng) /2;
  Standard_Integer m2 = max-lng - m1;
  if      (just <  0) S<<val<<&blank[maxblank-m1-m2];
  else if (just == 0) S<<&blank[maxblank-m1]<<val<<&blank[maxblank-m2];
  else                S<<&blank[maxblank-m1-m2]<<val;
}
