// Created on: 1996-10-01
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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


#ifndef _TopOpeBRepTool_KRO_HeaderFile
#define _TopOpeBRepTool_KRO_HeaderFile
#ifdef DEB
#include <OSD_Chronometer.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_OStream.hxx>

// POP pour NT
class TOPKRO:
public OSD_Chronometer{
public:
  TOPKRO(const TCollection_AsciiString& n)
    :myname(n),mystart(0),mystop(0){myname.RightJustify(30,' ');}
  virtual void Start(){mystart=1;OSD_Chronometer::Start();}
  virtual void Stop(){OSD_Chronometer::Stop();mystop=1;}
  void Print(Standard_OStream& OS){Standard_Real s;Show(s);OS<<myname<<" : ";
				   if(!mystart)OS<<"(inactif)";else{OS<<s<<" secondes";if(!mystop)OS<<" (run)";}}
private:
  TCollection_AsciiString myname;Standard_Integer mystart,mystop;
};
#endif
#endif
