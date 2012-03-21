// Created by: GG
// Copyright (c) 1991-1999 Matra Datavision
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





//-Version

//-Design       

//-Warning     

//-References

//-Language     C++ 2.1

//-Declarations

// for the class
#include <Xw.ixx>

// Routines C a declarer en extern
//extern "C" {
#include <Xw_Cextern.hxx>
//}

//-Aliases

//-Global data definitions

//-Local data definitions
static Standard_Integer MyTraceLevel = 0 ;
static Standard_Integer MyErrorLevel = 4 ;

//-Constructors

void Xw::SetTrace(const Standard_Integer TraceLevel,const Standard_Integer ErrorLevel) {
        MyTraceLevel = TraceLevel ;
	MyErrorLevel = ErrorLevel ;

	Xw_set_trace((int)MyTraceLevel,(int)MyErrorLevel) ;
}

Standard_Integer Xw::TraceLevel() {

        return MyTraceLevel ;
}

Standard_Integer Xw::ErrorLevel() {

        return MyErrorLevel ;
}
