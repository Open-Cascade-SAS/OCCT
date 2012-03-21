// Created on: 1997-04-21
// Created by: JLF, CAL
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
