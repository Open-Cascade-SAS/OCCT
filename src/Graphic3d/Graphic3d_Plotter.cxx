
// File		Graphic3d_Plotter.cxx
// Created	Lundi 21 avril 1997
// Author	JLF, CAL

//-Copyright	MatraDatavision 1997

//-Version	

//-Design	Declaration des variables specifiques aux plotters

//-Warning	

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_Plotter.ixx>

//-Aliases

//-Global data definitions

//-Constructors

Graphic3d_Plotter::Graphic3d_Plotter ():
MyPlottingState (Standard_False) {

}

//-Destructors

void Graphic3d_Plotter::Destroy () {

}

//-Methods, in order

Standard_Boolean Graphic3d_Plotter::BeginPlot (const Handle(Graphic3d_DataStructureManager)& aProjector) {

	if (MyPlottingState)
		Graphic3d_PlotterDefinitionError::Raise
			("Graphic3d_Plotter::BeginPlot\n");

	MyPlottingState	= Standard_True;

	Graphic3d_PlotterDefinitionError::Raise
		("Graphic3d_Plotter::BeginPlot\n");

	return MyPlottingState;

}

void Graphic3d_Plotter::EndPlot () {

	if (! MyPlottingState)
		Graphic3d_PlotterDefinitionError::Raise
			("Graphic3d_Plotter::EndPlot\n");

	MyPlottingState	= Standard_False;

	Graphic3d_PlotterDefinitionError::Raise
		("Graphic3d_Plotter::EndPlot\n");

}

Standard_Boolean Graphic3d_Plotter::PlottingState () const {

	return (MyPlottingState);

}
