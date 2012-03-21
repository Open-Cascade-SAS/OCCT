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

#include <Vrml_Separator.ixx>

Vrml_Separator::Vrml_Separator(const Vrml_SeparatorRenderCulling aRenderCulling)
{
  myRenderCulling = aRenderCulling;
  myFlagPrint = 0;
}

 Vrml_Separator::Vrml_Separator()
{
  myRenderCulling = Vrml_AUTO;
  myFlagPrint = 0;
}

void Vrml_Separator::SetRenderCulling(const Vrml_SeparatorRenderCulling aRenderCulling)
{
  myRenderCulling = aRenderCulling;
}

Vrml_SeparatorRenderCulling Vrml_Separator::RenderCulling() const 
{
  return myRenderCulling;
}

Standard_OStream& Vrml_Separator::Print(Standard_OStream& anOStream) 
{
  if ( myFlagPrint == 0 )
    {
      anOStream  << "Separator {" << endl;
      if ( myRenderCulling != Vrml_AUTO )
	{
	 if ( myRenderCulling == Vrml_ON )
	    anOStream  << "    renderCulling" << "\tON" << endl;
	 else
	    anOStream  << "    renderCulling" << "\tOFF" << endl;
	}
      myFlagPrint = 1;
    } //End of if
  else 
    {
     anOStream  << '}' << endl;
     myFlagPrint = 0;
    }
 return anOStream;
}

