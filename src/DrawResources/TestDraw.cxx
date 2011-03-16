

// modele de programme principal Draw

#include <Draw.hxx>
#include <Draw_Appli.hxx>

// main passe la main a Draw

main(int argc, char** argv)
{
  Draw_Appli(argc,argv);
}

//*******************************
//
// Retirez les includes inutiles
// pour ne pas surcharger le link
//
//********************************

#include <GeometryTest.hxx>

// seulement si on fait de la topologie
#include <BRepTest.hxx>

// pour les commandes utilisateurs topologie
#include <DBRep.hxx>


// exemple de commande utilisateur

static Standard_Integer macommande (Draw_Interpretor& di,
				    Standard_Integer n, char** a)
{
  if (n < 2) return 1;   // erreur si pas assez d'arguments

  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) {
    cout << a[1] << " n'est pas un shape" << endl;
    return 1;
  }

  // .... faite ce que vous voulez a S .....

  //... pour retourner un chaine a TCL, mettez la dans di

  di << a[1];

  return 0;
}


// definition des commandes

void Draw_InitAppli(Draw_Interpretor& theCommands)
{
  Draw::Commands(theCommands);

  // geometry
  GeometryTest::AllCommands(theCommands);   // voir GeometryTest.cdl pour etre plus fin

  // pour la topologie
  BRepTest::AllCommands(theCommands);       // voir BRepTest.cdl pour etre plus fin


  // commandes utilisateur

  theCommands.Add("macommande","macommande et son help",macommande);
}
