// Copyright (c) 1998-1999 Matra Datavision
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



// model of main program Draw

#include <Draw.hxx>
#include <Draw_Appli.hxx>

// main passes main to Draw

main(int argc, char** argv)
{
  Draw_Appli(argc,argv);
}

//*******************************
//
// Remove unused includes
// to avoid overloading the link
//
//********************************

#include <GeometryTest.hxx>

// only is one makes the topology
#include <BRepTest.hxx>

// for the commands using topology
#include <DBRep.hxx>


// example of user command

static Standard_Integer macommande (Draw_Interpretor& di,
				    Standard_Integer n, char** a)
{
  if (n < 2) return 1;   // error if not enough arguments

  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) {
    cout << a[1] << " is not a shape" << endl;
    return 1;
  }

  // .... do what you like with S .....

  //... to return a chain to TCL, place it in di

  di << a[1];

  return 0;
}


// definition of commands

void Draw_InitAppli(Draw_Interpretor& theCommands)
{
  Draw::Commands(theCommands);

  // geometry
  GeometryTest::AllCommands(theCommands);   // see GeometryTest.cdl for details

  // for the topology
  BRepTest::AllCommands(theCommands);       // see BRepTest.cdl for details


  // user commands

  theCommands.Add("macommande","macommande and its help",macommande);
}
