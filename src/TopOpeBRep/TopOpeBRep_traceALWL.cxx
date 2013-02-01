// Created on: 1997-10-22
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#ifdef DEB

#include <Standard_Type.hxx>

class TopOpeBRep_ALWL {

public:
  TopOpeBRep_ALWL
    (const char* name,const Standard_Boolean b = Standard_False);
  void Set(const Standard_Boolean b)    
    { mydefdef = mypasdef = mynbpdef = myonetrue = b; }
  void SetDef(const Standard_Real p)    
    { mydeflectionmax = p; mydefdef = Standard_True; }
  void SetPas(const Standard_Real p)
    { mypasUVmax = p; mypasdef = Standard_True; }
  void SetNbp(const Standard_Integer p) 
    { mynbpointsmax = p; mynbpdef = Standard_True; }
  
  Standard_Boolean GetDef(Standard_Real& p) 
    { p = mydeflectionmax; return mydefdef; }
  Standard_Boolean  GetPas(Standard_Real& p) 
    { p = mypasUVmax; return mypasdef; }
  Standard_Boolean GetNbp(Standard_Integer& p) 
    { p = mynbpointsmax; return mynbpdef; }
  Standard_Boolean Get()
    { return myonetrue; }
  
  void Set(const Standard_Boolean b, Standard_Integer n, char** a);
  void Print();
  
private:
  Standard_Real    mydeflectionmax;
  Standard_Boolean mydefdef;
  
  Standard_Real    mypasUVmax;
  Standard_Boolean mypasdef;
  
  Standard_Integer mynbpointsmax;
  Standard_Boolean mynbpdef;
  
  Standard_Boolean myonetrue;
  char myname[100];
};

TopOpeBRep_ALWL::TopOpeBRep_ALWL(const char* name, const Standard_Boolean b) :
  mydeflectionmax(0.01),mydefdef(Standard_False),
  mypasUVmax(0.05),mypasdef(Standard_False),
  mynbpointsmax(200),mynbpdef(Standard_False)
{ 
  strcpy(myname,name);
  Set(b);
}

void TopOpeBRep_ALWL::Set(const Standard_Boolean b,
			  Standard_Integer n, char** a)
{ 
  if (!n) Set(b);
  else {
    Set(Standard_False);
    for (Standard_Integer i=0; i < n; i++) {
      const char *p = a[i];
      if      ( !strcasecmp(p,"def") ) {
	if ( ++i < n ) SetDef(Atof(a[i]));
      }
      else if ( !strcasecmp(p,"pas") ) {
	if ( ++i < n ) SetPas(Atof(a[i]));
      }
      else if ( !strcasecmp(p,"nbp") ) { 
	if ( ++i < n ) SetNbp(atoi(a[i]));
      }
    }
  }
  myonetrue = mydefdef || mypasdef || mynbpdef;
  Print();
}

void TopOpeBRep_ALWL::Print()
{
  cout<<myname<<" defined :";
  Standard_Integer n = 0;
  if (mydefdef) { cout<<" Def = "<<mydeflectionmax; n++; }
  if (mypasdef) { cout<<" Pas = "<<mypasUVmax; n++; }
  if (mynbpdef) { cout<<" Nbp = "<<mynbpointsmax; n++; }
  if (!n) cout<<" none";
  cout<<endl;
}

static TopOpeBRep_ALWL TopOpeBRep_contextALWL
("LineGeomTool ALWL parameters");

void TopOpeBRep_SetcontextALWL
  (const Standard_Boolean b, Standard_Integer narg, char** a) 
{ TopOpeBRep_contextALWL.Set(b,narg,a); }

Standard_Boolean TopOpeBRep_GetcontextALWLNBP(Standard_Integer& n) 
{ Standard_Boolean b = TopOpeBRep_contextALWL.GetNbp(n);
  return b;
}

// #ifdef DEB
#endif
