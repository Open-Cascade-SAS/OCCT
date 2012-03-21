// Created on: 1997-07-30
// Created by: Denis PASCAL
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


#include <DDataStd.hxx>
#include <DDataStd_DrawPresentation.hxx>
#include <DDF.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>

#include <TDF_Data.hxx>
#include <TDF_Label.hxx>

#include <DBRep.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>


// LES ATTRIBUTES

#include <TNaming_NamedShape.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Tool.hxx>


//=======================================================================
//function : DDataStd_SetShape
//purpose  : SetShape (DF, entry, drawshape)
//=======================================================================

static Standard_Integer DDataStd_SetShape (Draw_Interpretor& di,
					Standard_Integer nb, 
					const char** arg) 
{ 
  if (nb == 4) {    
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(arg[1],DF)) return 1;  
    TopoDS_Shape s = DBRep::Get(arg[3]);  
    if (s.IsNull()) { di <<"shape not found"<< "\n"; return 1;}  
    TDF_Label L;
    DDF::AddLabel(DF, arg[2], L);
    TNaming_Builder SI (L);
    SI.Generated(s);
    return 0;
  }
  di << "DDataStd_SetShape : Error" << "\n";
  return 1;
}


//=======================================================================
//function : NamedShapeCommands
//purpose  : 
//=======================================================================

void DDataStd::NamedShapeCommands (Draw_Interpretor& theCommands)
{  

  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  const char* g = "DData : Standard Attribute Commands";
  

  theCommands.Add ("SetShape", 
                   "SetShape (DF, entry, drawname)",
		   __FILE__, DDataStd_SetShape, g);

}
