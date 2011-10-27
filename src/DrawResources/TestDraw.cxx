

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
