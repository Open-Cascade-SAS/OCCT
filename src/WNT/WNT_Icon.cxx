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


#include <WNT_Icon.ixx>

//=======================================================================
//function : WNT_Icon
//purpose  : 
//=======================================================================

WNT_Icon::WNT_Icon (
             const Standard_CString aName,
             const Aspect_Handle    aBitmap,
             const Standard_Integer aHashCode
            ) : WNT_Image ( aBitmap, aHashCode )
{

  myName = new char[ strlen ( aName ) + 1 ];

  strcpy ( myName, aName );

}  // end constructor

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void WNT_Icon::Destroy () {

  delete [] myName;

}  // end WNT_Icon :: Destroy

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void WNT_Icon::SetName ( const Standard_CString aName ) {

  if (  strlen ( myName ) < strlen ( aName )  ) {
 
    delete [] myName;

    myName = new char[ strlen ( aName ) + 1 ];
 
  }  // end if

  strcpy ( myName, aName );

}  // end WNT_Icon :: SetName
