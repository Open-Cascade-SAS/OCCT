
#define XTRACE
#define XDESTROY

// File		Graphic3d_DataStructureManager.cxx
// Created	24 fevrier 1995
// Author	CAL
//              11/97 ; CAL : retrait des DataStructure

//-Copyright	MatraDatavision 1995

//-Version	

//-Design	

//-Warning	

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_DataStructureManager.ixx>

//-Aliases

//-Global data definitions

//-Constructors

Graphic3d_DataStructureManager::Graphic3d_DataStructureManager () {
}

//-Destructors

void Graphic3d_DataStructureManager::Destroy () {

#ifdef DESTROY
	cout << "Graphic3d_DataStructureManager::Destroy ()\n";
#endif

}
