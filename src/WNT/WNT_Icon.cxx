// File:      WNT_Icon.cxx
// Copyright: Open Cascade 2008

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
