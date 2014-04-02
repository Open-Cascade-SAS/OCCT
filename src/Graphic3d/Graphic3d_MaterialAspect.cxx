// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#define BUC60565        //GG_19/10/99 ; Don't change the color components when the corresponding
//                      coefficient is modified because the resulting color is computed in
//                      low level driver.

#define IMP150200	//GG : Add IsEqual() IsDifferent() methods.
//                             Add NumberOfMaterials() MaterialName() class methods

#define IMP171201	//GG : Add Add MaterialName() SetMaterialName() instance
//			methods in order to set or retrieve name of the current
//			material.
//			Capitalize all material name strings.
//			Thanks to Stephane ROUTELOUS.


//-Version	

//-Design	Declaration of variables specific to the definition
//		of materials

//-Warning	A material is defined by :
//		- coefficient of transparence
//		- coefficient of disperse reflection
//		- coefficient of ambiant reflection
//		- coefficient of specular reflection
//              - coefficient of emission

//		2 properties permiting to define a material :
//		- transparence
//		- reflection, i.e. absorbtion properties
//		and light diffusion

//		Disperse reflection is considered a component
//		The specular reflection is considered a component
//		of the color of the light source

//		To determine 3 colors of reflection it is necessary :
//		- coefficient of disperse reflection
//		- coefficient of ambiant reflection
//		- coefficient of specular reflection

//		( Under GL, 3 colors are determined)

//-References	Getting started with DEC PHIGS, annexe C
//		Iris Advanced Graphics, unite D

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_MaterialAspect.ixx>
#include <Standard_Assert.hxx>
//-Aliases

//-Global data definitions

//	-- le coefficient de reflection diffuse, la couleur et l'activite
//	MyDiffuseCoef		:	Standard_ShortReal;
//	MyDiffuseColor		:	Color;
//	MyDiffuseActivity	:	Standard_Boolean;

//	-- le coefficient de reflection ambiante, la couleur et l'activite
//	MyAmbientCoef		:	Standard_ShortReal;
//	MyAmbientColor		:	Color;
//	MyAmbientActivity	:	Standard_Boolean;

//	-- le coefficient de reflection speculaire, la couleur et l'activite
//	MySpecularCoef		:	Standard_ShortReal;
//	MySpecularColor		:	Color;
//	MySpecularActivity	:	Standard_Boolean;

//	-- le coefficient d emission du materiau, la couleur et l'activite
//	MyEmissiveCoef		:	Standard_ShortReal;
//	MyEmissiveActivity	:	Standard_Boolean;

//	-- le coefficient de transparence
//	MyTransparencyCoef	:	Standard_ShortReal;

//	-- le coefficient de concentration lumineuse
//	MyShininess		:	Standard_ShortReal;

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_MaterialAspect::Graphic3d_MaterialAspect ()
: MyAmbientColor(0.2, 0.2, 0.2, Quantity_TOC_RGB)
{ 
#ifdef IMP171201
   MyRequestedMaterialName = Graphic3d_NOM_DEFAULT;
   Init(MyRequestedMaterialName);
#else
   Init(Graphic3d_NOM_DEFAULT);
#endif
}

Graphic3d_MaterialAspect::Graphic3d_MaterialAspect (const Graphic3d_NameOfMaterial AName) 
: MyAmbientColor(0.2, 0.2, 0.2, Quantity_TOC_RGB)
{
#ifdef IMP171201
   MyRequestedMaterialName = AName;
   Init(MyRequestedMaterialName);
#else
   Init(AName);
#endif
}

void Graphic3d_MaterialAspect::Init(const Graphic3d_NameOfMaterial AName) {

        MyMaterialType = Graphic3d_MATERIAL_ASPECT;
        MyTransparencyCoef = Standard_ShortReal (0.0);
        MyDiffuseActivity = Standard_True;
        MyDiffuseCoef = Standard_ShortReal (0.65); 
        MyAmbientActivity = Standard_True;
        MyAmbientCoef = Standard_ShortReal (0.3);
        MySpecularActivity = Standard_True;
        MySpecularCoef = Standard_ShortReal (0.0);
        MyEmissiveActivity = Standard_False;
        MyEmissiveCoef = Standard_ShortReal (0.0);
        MyEnvReflexion = Standard_ShortReal(0.0);
        MyShininess = Standard_ShortReal (0.039);
        MyDiffuseColor.SetValues (0.0, 0.0, 0.0, Quantity_TOC_RGB);
        MySpecularColor.SetValues (1.0, 1.0, 1.0, Quantity_TOC_RGB);
        MyMaterialName = AName;
#ifdef IMP171201
	Standard_Integer index = Standard_Integer(AName);
        if( index < NumberOfMaterials() ) {
          MyStringName = MaterialName(index+1);
        } 
#endif

	switch (AName) {
		case Graphic3d_NOM_PLASTIC : /* Blue plastic */
			/* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.0078125);
			MyAmbientCoef		= Standard_ShortReal (0.5);
			MyDiffuseCoef		= Standard_ShortReal (0.24);
			MySpecularCoef		= Standard_ShortReal (0.06);
		break;				
		
	        case Graphic3d_NOM_SHINY_PLASTIC : /* black plastic */
			/* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (1.0);
			MyAmbientCoef		= Standard_ShortReal (0.44);
			MyDiffuseCoef		= Standard_ShortReal (0.5);
			MySpecularCoef		= Standard_ShortReal (1.0);
		break;

		case Graphic3d_NOM_SATIN :
			MyShininess		= Standard_ShortReal (0.09375);
			MyAmbientCoef		= Standard_ShortReal (0.33);
			MyDiffuseCoef		= Standard_ShortReal (0.4);
			MySpecularCoef		= Standard_ShortReal (0.44);
		break;
		
		case Graphic3d_NOM_NEON_GNC: 
			/* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.05);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (0.62);
			MyEmissiveCoef          = Standard_ShortReal (1.0);
			MyEmissiveActivity = Standard_True;
			MyAmbientActivity  = Standard_False;											  
		break;
		
		case Graphic3d_NOM_METALIZED :			
			/* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.13);
			MyAmbientCoef		= Standard_ShortReal (0.9);
			MyDiffuseCoef		= Standard_ShortReal (0.47);
			MySpecularCoef		= Standard_ShortReal (0.45);
			MyAmbientActivity  = Standard_False;
						
			/* Color resulting from dispersed */
			//MyDiffuseColor.SetValues (0.87, 0.96, 1.0, Quantity_TOC_RGB);
			
			/* Color resulting from specular */
			//MySpecularColor.SetValues (0.93, 0.95, 0.78, Quantity_TOC_RGB);						
		break;

// Ascending Compatibility physical materials. The same definition is taken
// as in the next constructor.	

		case Graphic3d_NOM_BRASS :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
			/* Initialisation of coefficients and lighting color : brass */
			MyShininess		= Standard_ShortReal (0.21794844);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Color resulting from ambient */
			MyAmbientColor.SetValues (0.329412, 0.223529, 0.027451, Quantity_TOC_RGB);
			
			/* Color resulting from dispersed */
			MyDiffuseColor.SetValues (0.780392, 0.568627, 0.113725, Quantity_TOC_RGB);
			
			/* Color resulting from specular */
			MySpecularColor.SetValues (0.992157, 0.941176, 0.807843, Quantity_TOC_RGB);								
		break;

		case Graphic3d_NOM_BRONZE :						
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
		        /* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.2);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Color resulting from ambient */
			MyAmbientColor.SetValues (0.2125, 0.1275, 0.054, Quantity_TOC_RGB);
			
			/* Color resulting from dispersed */
			MyDiffuseColor.SetValues (0.714, 0.4284, 0.18144, Quantity_TOC_RGB);
			
			/* Color resulting from specular */
			MySpecularColor.SetValues (0.393548, 0.271906, 0.166721, Quantity_TOC_RGB);						
		break;

		case Graphic3d_NOM_COPPER :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
                        /*  Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.93);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Color resulting from ambient */
			MyAmbientColor.SetValues (0.33, 0.26, 0.23, Quantity_TOC_RGB);
			
			/* Color resulting from dispersed */
			MyDiffuseColor.SetValues (0.50, 0.11, 0.0, Quantity_TOC_RGB);
			
			/* Color resulting from specular */
			MySpecularColor.SetValues (0.95, 0.73, 0.0, Quantity_TOC_RGB);			
		break;

		case Graphic3d_NOM_GOLD :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
		        /* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.0625);
			MyAmbientCoef		= Standard_ShortReal (0.3);
			MyDiffuseCoef		= Standard_ShortReal (0.4);
			MySpecularCoef		= Standard_ShortReal (0.9);
			
			/* Color resulting from ambient */
			MyAmbientColor.SetValues (1.0, 0.76862745, 0.31764706, Quantity_TOC_RGB);
			
			/* Color resulting from dispersed */
			MyDiffuseColor.SetValues (1.0, 0.69, 0.0, Quantity_TOC_RGB);
			
			/* Color resulting from specular */
			MySpecularColor.SetValues (1.0, 0.98, 0.78, Quantity_TOC_RGB);									
		break;		

		case Graphic3d_NOM_PEWTER :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
			/* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.076923047);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Color resulting from ambient */
			MyAmbientColor.SetValues (0.105882, 0.058824, 0.113725, Quantity_TOC_RGB);
			
			/* Color resulting from dispersed */
			MyDiffuseColor.SetValues (0.427451, 0.470588, 0.541176, Quantity_TOC_RGB);
			
			/* Color resulting from specular */
			MySpecularColor.SetValues (0.333333, 0.333333, 0.521569, Quantity_TOC_RGB);					
		break;	

		case Graphic3d_NOM_PLASTER :
#ifdef BUG      // The plaster material must be considered as Generic.
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
                        /*  Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.2);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
#else
                        /* Lighting Coefficient */
                        MyShininess             = Standard_ShortReal (0.0078125)
;
                        MyAmbientCoef           = Standard_ShortReal (0.26);
                        MyDiffuseCoef           = Standard_ShortReal (0.23);
                        MySpecularCoef          = Standard_ShortReal (0.06);
#endif
			
			/* Color resulting from ambient */
			MyAmbientColor.SetValues (0.19225, 0.19225, 0.19225, Quantity_TOC_RGB);
			
			/* Color resulting from dispersed */
			MyDiffuseColor.SetValues (0.50754, 0.50754, 0.50754, Quantity_TOC_RGB);
			
			/* Color resulting from specular */
			MySpecularColor.SetValues (0.508273, 0.508273, 0.508273, Quantity_TOC_RGB);
		break;

		case Graphic3d_NOM_SILVER :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
                        /* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.2);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Color resulting from ambient */
			MyAmbientColor.SetValues (0.19225, 0.19225, 0.19225, Quantity_TOC_RGB);
			
			/* Color resulting from dispersed */
			MyDiffuseColor.SetValues (0.50754, 0.50754, 0.50754, Quantity_TOC_RGB);
			
			/* Color resulting from specular */
			MySpecularColor.SetValues (0.508273, 0.508273, 0.508273, Quantity_TOC_RGB);
		break;
		
		case Graphic3d_NOM_STEEL :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
		        /* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.06); 
			MyAmbientCoef		= Standard_ShortReal (0.01);
			MyDiffuseCoef		= Standard_ShortReal (0.03);
			MySpecularCoef		= Standard_ShortReal (0.98);
			
			/* Color resulting from specular */
			MySpecularColor.SetValues (1.0, 1.0, 1.0, Quantity_TOC_RGB);
		break;	
		
		case Graphic3d_NOM_STONE :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
		        /* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.17);
			MyAmbientCoef		= Standard_ShortReal (0.19);
			MyDiffuseCoef		= Standard_ShortReal (0.75);
			MySpecularCoef		= Standard_ShortReal (0.08);
			
			/* Color resulting from ambient */
			MyAmbientColor.SetValues (1.0, 0.8, 0.62, Quantity_TOC_RGB);
			
			/* Color resulting from dispersed */
			MyDiffuseColor.SetValues (1.0, 0.8, 0.62, Quantity_TOC_RGB);
			
			/* Color resulting from specular */						
			MySpecularColor.SetValues (0.98, 1.0, 0.60, Quantity_TOC_RGB);			
		break;

// Ascending Compatibility of physical materials. Takes the same definition
// as in the next constructor. New materials
		
		case Graphic3d_NOM_CHROME :			
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
			/* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.1);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Color resulting from ambient */
			MyAmbientColor.SetValues (0.35, 0.35, 0.35, Quantity_TOC_RGB);
			
			/* Color resulting from dispersed */
			MyDiffuseColor.SetValues (0.4, 0.4, 0.4, Quantity_TOC_RGB);
			
			/*  Color resulting from specular */
			MySpecularColor.SetValues (0.974597, 0.974597, 0.974597, Quantity_TOC_RGB);						
		break;
		
		case Graphic3d_NOM_NEON_PHC: 
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
			/* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.05);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (0.62);
			MyEmissiveCoef          = Standard_ShortReal (0.9);
			MyEmissiveActivity = Standard_True;
			MyAmbientActivity = Standard_False;
			MyDiffuseActivity  = Standard_False;
			
			/* Color resulting from ambient */
			MyAmbientColor.SetValues (1.0, 1.0, 1.0, Quantity_TOC_RGB);
			
			/* Color resulting from dispersed */
			MyDiffuseColor.SetValues (1.0, 1.0, 1.0, Quantity_TOC_RGB);
			
			/* Color resulting from specular */
			MySpecularColor.SetValues (1.0, 1.0, 1.0, Quantity_TOC_RGB);
			
			/* Color resulting from specular */			
			MyEmissiveColor.SetValues (0.0, 1.0, 0.46, Quantity_TOC_RGB);							  
		break;
		
		case Graphic3d_NOM_ALUMINIUM :			
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
			/* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.09);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Color resulting from ambient */
			MyAmbientColor.SetValues (0.30, 0.30, 0.30, Quantity_TOC_RGB);
			
			/* Color resulting from dispersed */
			MyDiffuseColor.SetValues (0.30, 0.30, 0.30, Quantity_TOC_RGB);
			
			/* Color resulting from specular */
			MySpecularColor.SetValues (0.70, 0.70, 0.80, Quantity_TOC_RGB);						
		break;
		
		case Graphic3d_NOM_OBSIDIAN :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
			/* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.3);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Color resulting from ambient */
			MyAmbientColor.SetValues (0.05375, 0.05, 0.06625, Quantity_TOC_RGB);
			
			/* Color resulting from dispersed */
			MyDiffuseColor.SetValues (0.18275, 0.17, 0.22525, Quantity_TOC_RGB);
			
			/* Color resulting from specular */
			MySpecularColor.SetValues (0.332741, 0.328634, 0.346435, Quantity_TOC_RGB);						
		break;	
		
		case Graphic3d_NOM_JADE :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
			/* Lighting Coefficient */
			MyShininess		= Standard_ShortReal (0.1);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Color resulting from ambient */
			MyAmbientColor.SetValues (0.135, 0.2225, 0.1575, Quantity_TOC_RGB);
			
			/* Color resulting from dispersed */
			MyDiffuseColor.SetValues (0.54, 0.89, 0.63, Quantity_TOC_RGB);
			
			/* Color resulting from specular */
			MySpecularColor.SetValues (0.316228, 0.316228, 0.316228, Quantity_TOC_RGB);						
		break;
		default:
#ifdef IMP171201
		case Graphic3d_NOM_DEFAULT :
			MyStringName = "Default";
		break;
		case Graphic3d_NOM_UserDefined :
			MyStringName = "UserDefined";
#endif
                break ;
	}
}

void Graphic3d_MaterialAspect::IncreaseShine (const Standard_Real ADelta) {

Standard_ShortReal OldShine;

	OldShine	= MyShininess;
	MyShininess	= (Standard_ShortReal)(MyShininess + MyShininess * ADelta / 100.);
	if ( (MyShininess > 1.0) || (MyShininess < 0.0) ) {
		MyShininess	= OldShine;
	}
}

/* Attribution of the type of material */
void Graphic3d_MaterialAspect::SetMaterialType( const Graphic3d_TypeOfMaterial AType ) {
       MyMaterialType = AType ;
#ifdef IMP171201
       if( AType != MyMaterialType )
		 SetMaterialName("UserDefined");
#endif
}


void Graphic3d_MaterialAspect::SetAmbient (const Standard_Real AValue) {

	if ((AValue < 0.0) || (AValue > 1.0))
		Graphic3d_MaterialDefinitionError::Raise
			("Bad value for SetAmbient < 0. or > 1.0");


	MyAmbientCoef	= Standard_ShortReal (AValue);
#ifndef BUC60565
	Standard_Real R, G, B;
	MyAmbientColor.Values (R, G, B, Quantity_TOC_RGB);
	MyAmbientColor.SetValues
			(R*AValue, G*AValue, B*AValue, Quantity_TOC_RGB);
#endif
#ifdef IMP171201
        if( MyAmbientActivity && 
		(MyMaterialType == Graphic3d_MATERIAL_PHYSIC) )
		SetMaterialName("UserDefined");
#endif
}

void Graphic3d_MaterialAspect::SetColor (const Quantity_Color& AColor) {

	MyAmbientColor	= AColor;
#ifdef IMP171201
        if( MyAmbientActivity && 
		(MyMaterialType == Graphic3d_MATERIAL_PHYSIC) )
		SetMaterialName("UserDefined");
#endif
}

/* Attribution of color ambient lighting */
void Graphic3d_MaterialAspect::SetAmbientColor (const Quantity_Color& AColor) {

	MyAmbientColor	= AColor;
#ifdef IMP171201
        if( MyAmbientActivity && 
		(MyMaterialType == Graphic3d_MATERIAL_PHYSIC) )
		SetMaterialName("UserDefined");
#endif
}

/*  Attribution of color dispersed lighting */
void Graphic3d_MaterialAspect::SetDiffuseColor (const Quantity_Color& AColor) {

	MyDiffuseColor	= AColor;
#ifdef IMP171201
        if( MyDiffuseActivity && 
		(MyMaterialType == Graphic3d_MATERIAL_PHYSIC) )
		SetMaterialName("UserDefined");
#endif
}

/*  Attribution of color specular lighting */
void Graphic3d_MaterialAspect::SetSpecularColor (const Quantity_Color& AColor) {

	MySpecularColor	= AColor;
#ifdef IMP171201
        if( MySpecularActivity && 
		(MyMaterialType == Graphic3d_MATERIAL_PHYSIC) )
		SetMaterialName("UserDefined");
#endif
}

/*  Affectation couleur d emission */
void Graphic3d_MaterialAspect::SetEmissiveColor (const Quantity_Color& AColor) {

	MyEmissiveColor	= AColor;
#ifdef IMP171201
        if( MyEmissiveActivity && 
		(MyMaterialType == Graphic3d_MATERIAL_PHYSIC) )
		SetMaterialName("UserDefined");
#endif
}

void Graphic3d_MaterialAspect::SetDiffuse (const Standard_Real AValue) {

	if ((AValue < 0.0) || (AValue > 1.0))
		Graphic3d_MaterialDefinitionError::Raise
			("Bad value for SetDiffuse < 0. or > 1.0");


	MyDiffuseCoef	= Standard_ShortReal (AValue);
#ifndef BUC60565
        Standard_Real R, G, B;
	MyDiffuseColor.Values (R, G, B, Quantity_TOC_RGB);
	MyDiffuseColor.SetValues
			(R*AValue, G*AValue, B*AValue, Quantity_TOC_RGB);
#endif
#ifdef IMP171201
        if( MyDiffuseActivity && 
		(MyMaterialType == Graphic3d_MATERIAL_PHYSIC) )
		SetMaterialName("UserDefined");
#endif
}

/* Attribution of coefficient of emission */
void Graphic3d_MaterialAspect::SetEmissive (const Standard_Real AValue) {

	if ((AValue < 0.0) || (AValue > 1.0))
		Graphic3d_MaterialDefinitionError::Raise
			("Bad value for SetEmissive < 0. or > 1.0");

//Standard_Real R, G, B;

	MyEmissiveCoef	= Standard_ShortReal (AValue);
#ifdef IMP171201
        if( MyDiffuseActivity && 
		(MyMaterialType == Graphic3d_MATERIAL_PHYSIC) )
		SetMaterialName("UserDefined");
#endif
}

void Graphic3d_MaterialAspect::SetReflectionModeOn (const Graphic3d_TypeOfReflection AType) {

	switch (AType) {

		case Graphic3d_TOR_AMBIENT :
			MyAmbientActivity	= Standard_True;
		break;

		case Graphic3d_TOR_DIFFUSE :
			MyDiffuseActivity	= Standard_True;
		break;

		case Graphic3d_TOR_SPECULAR :
			MySpecularActivity	= Standard_True;
		break;	
			
		case Graphic3d_TOR_EMISSION :
			MyEmissiveActivity	= Standard_True;	
		break;
	}
#ifdef IMP171201
        if( MyMaterialType == Graphic3d_MATERIAL_PHYSIC )
		SetMaterialName("UserDefined");
#endif
}

void Graphic3d_MaterialAspect::SetReflectionModeOff (const Graphic3d_TypeOfReflection AType) {

	switch (AType) {

		case Graphic3d_TOR_AMBIENT :
			MyAmbientActivity	= Standard_False;
		break;

		case Graphic3d_TOR_DIFFUSE :
			MyDiffuseActivity	= Standard_False;
		break;

		case Graphic3d_TOR_SPECULAR :
			MySpecularActivity	= Standard_False;
		break;
		
		case Graphic3d_TOR_EMISSION :
			MyEmissiveActivity	= Standard_False;
		break;
	}
#ifdef IMP171201
        if( MyMaterialType == Graphic3d_MATERIAL_PHYSIC )
		SetMaterialName("UserDefined");
#endif
}

void Graphic3d_MaterialAspect::SetSpecular (const Standard_Real AValue) {

	if ((AValue < 0.0) || (AValue > 1.0))
		Graphic3d_MaterialDefinitionError::Raise
			("Bad value for SetSpecular < 0. or > 1.0");


	MySpecularCoef	= Standard_ShortReal (AValue);
#ifndef BUC60565
        Standard_Real R, G, B;
	MySpecularColor.Values (R, G, B, Quantity_TOC_RGB);
	MySpecularColor.SetValues
			(R*AValue, G*AValue, B*AValue, Quantity_TOC_RGB);
#endif
#ifdef IMP171201
        if( MySpecularActivity && 
		(MyMaterialType == Graphic3d_MATERIAL_PHYSIC) )
		SetMaterialName("UserDefined");
#endif
}

void Graphic3d_MaterialAspect::SetTransparency (const Standard_Real AValue) {

	if ((AValue < 0.0) || (AValue > 1.0))
		Graphic3d_MaterialDefinitionError::Raise
			("Bad value for SetTransparency < 0. or > 1.0");

	MyTransparencyCoef	= Standard_ShortReal (AValue);
}

Quantity_Color Graphic3d_MaterialAspect::Color () const {
	return (MyAmbientColor);
}

/* Access to the color of the ambient lighting */
Quantity_Color Graphic3d_MaterialAspect::AmbientColor () const {

	return (MyAmbientColor);
}

/* Access to the color of the dispersed lighting */
Quantity_Color Graphic3d_MaterialAspect::DiffuseColor () const {

	return (MyDiffuseColor);
}

/* Access to the color of the specular lighting */
Quantity_Color Graphic3d_MaterialAspect::SpecularColor () const {

	return (MySpecularColor);
}

/* Access to the color of emission */
Quantity_Color Graphic3d_MaterialAspect::EmissiveColor () const {

	return (MyEmissiveColor);
}

/* Access to the type of material */
Standard_Boolean Graphic3d_MaterialAspect::MaterialType( const Graphic3d_TypeOfMaterial AType ) const {
       return ( MyMaterialType == AType ) ;
}

Standard_Boolean Graphic3d_MaterialAspect::ReflectionMode (const Graphic3d_TypeOfReflection AType) const {

Standard_Boolean Result;

	Result	= Standard_False;

	switch (AType) {

		case Graphic3d_TOR_AMBIENT :
			Result	= MyAmbientActivity;
		break;

		case Graphic3d_TOR_DIFFUSE :
			Result	= MyDiffuseActivity;
		break;

		case Graphic3d_TOR_SPECULAR :
			Result	= MySpecularActivity;
		break;
		
		case Graphic3d_TOR_EMISSION :
			Result	= MyEmissiveActivity;
		break;
	}
	return (Result);

}

/* Access to the color of the ambient lighting */
Standard_Real Graphic3d_MaterialAspect::Ambient () const {

	return (Standard_Real (MyAmbientCoef));

}

/* Access to the color of the dispersed lighting */
Standard_Real Graphic3d_MaterialAspect::Diffuse () const {

	return (Standard_Real (MyDiffuseCoef));

}

/* Value of the coefficient of emission */
Standard_Real Graphic3d_MaterialAspect::Emissive () const {

	return (Standard_Real (MyEmissiveCoef));

}

/* Access to the color of the specular lighting */
Standard_Real Graphic3d_MaterialAspect::Specular () const {

	return (Standard_Real (MySpecularCoef));

}

/* Access to the coefficient of transparency */
Standard_Real Graphic3d_MaterialAspect::Transparency () const {

	return (Standard_Real (MyTransparencyCoef));

}

/* Access to the coefficient of shineness */
Standard_Real Graphic3d_MaterialAspect::Shininess () const {

	return (Standard_Real (MyShininess));

}

/* Attribution of the coefficient of shineness */
void Graphic3d_MaterialAspect::SetShininess (const Standard_Real AValue) {

	if ((AValue < 0.0) || (AValue > 1.0))
		Graphic3d_MaterialDefinitionError::Raise
			("Bad value for Shininess < 0. or > 1.0");

	MyShininess		= Standard_ShortReal (AValue);
#ifdef IMP171201
	SetMaterialName("UserDefined");
#endif
}


void Graphic3d_MaterialAspect::SetEnvReflexion(const Standard_ShortReal AValue)
{
  MyEnvReflexion = AValue;
}


Standard_ShortReal Graphic3d_MaterialAspect::EnvReflexion() const
{
  return MyEnvReflexion;
}

Graphic3d_NameOfMaterial Graphic3d_MaterialAspect::Name() const
{
  return MyMaterialName;
}

void Graphic3d_MaterialAspect::Reset() {
#ifdef IMP171201
  Init(MyRequestedMaterialName);
#else
  Init(MyMaterialName);
#endif
}

#ifdef IMP150200
Standard_Boolean Graphic3d_MaterialAspect::IsEqual(const Graphic3d_MaterialAspect& other) const {

  if( MyAmbientCoef == other.Ambient() &&
      MyDiffuseCoef == other.Diffuse() &&
      MySpecularCoef == other.Specular() &&
      MyEmissiveCoef == other.Emissive() &&
      MyTransparencyCoef == other.Transparency() &&
      MyShininess == other.Shininess() &&
      MyEnvReflexion == other.EnvReflexion() &&
      MyAmbientColor == other.AmbientColor() &&
      MyDiffuseColor == other.DiffuseColor() &&
      MySpecularColor == other.SpecularColor() &&
      MyEmissiveColor == other.EmissiveColor() &&
      MyAmbientActivity == other.ReflectionMode(Graphic3d_TOR_AMBIENT) &&
      MyDiffuseActivity == other.ReflectionMode(Graphic3d_TOR_DIFFUSE) &&
      MySpecularActivity == other.ReflectionMode(Graphic3d_TOR_SPECULAR) &&
      MyEmissiveActivity == other.ReflectionMode(Graphic3d_TOR_EMISSION) )
    return Standard_True;

  return Standard_False; 
}

Standard_Boolean Graphic3d_MaterialAspect::IsDifferent(const Graphic3d_MaterialAspect& other) const {

  return !IsEqual(other);
}

typedef struct _Material {
  const char* name;
  Graphic3d_TypeOfMaterial type;
} Material;

static Material theMaterials[] = {
 	{"Brass",Graphic3d_MATERIAL_PHYSIC},
	{"Bronze",Graphic3d_MATERIAL_PHYSIC},
	{"Copper",Graphic3d_MATERIAL_PHYSIC},
	{"Gold",Graphic3d_MATERIAL_PHYSIC},
	{"Pewter",Graphic3d_MATERIAL_PHYSIC},
	{"Plastered",Graphic3d_MATERIAL_ASPECT},
	{"Plastified",Graphic3d_MATERIAL_ASPECT},
	{"Silver",Graphic3d_MATERIAL_PHYSIC},
	{"Steel",Graphic3d_MATERIAL_PHYSIC},
	{"Stone",Graphic3d_MATERIAL_PHYSIC},
	{"Shiny_plastified",Graphic3d_MATERIAL_ASPECT},
	{"Satined",Graphic3d_MATERIAL_ASPECT},
	{"Metalized",Graphic3d_MATERIAL_ASPECT},
	{"Ionized",Graphic3d_MATERIAL_ASPECT},
	{"Chrome",Graphic3d_MATERIAL_PHYSIC},
	{"Aluminium",Graphic3d_MATERIAL_PHYSIC},
	{"Obsidian",Graphic3d_MATERIAL_PHYSIC},
	{"Neon",Graphic3d_MATERIAL_PHYSIC},
	{"Jade",Graphic3d_MATERIAL_PHYSIC}
};

Standard_Integer Graphic3d_MaterialAspect::NumberOfMaterials() {
  Standard_STATIC_ASSERT(sizeof(theMaterials)/sizeof(Material) == Graphic3d_NOM_DEFAULT);

  return Graphic3d_NOM_DEFAULT;
}

Standard_CString Graphic3d_MaterialAspect::MaterialName(const Standard_Integer aRank) {

  if( aRank < 1 || aRank > NumberOfMaterials() )
        Standard_OutOfRange::Raise(" BAD index of material");

  return theMaterials[aRank-1].name;
}

Graphic3d_NameOfMaterial Graphic3d_MaterialAspect::MaterialFromName (const Standard_CString theName)
{
  TCollection_AsciiString aName (theName);
  aName.LowerCase();
  aName.Capitalize();
  const Standard_Integer aNbMaterials = Graphic3d_MaterialAspect::NumberOfMaterials();
  for (Standard_Integer aMatIter = 1; aMatIter <= aNbMaterials; ++aMatIter)
  {
    if (aName == Graphic3d_MaterialAspect::MaterialName (aMatIter))
    {
      return Graphic3d_NameOfMaterial(aMatIter - 1);
    }
  }

  // parse aliases
  if (aName == "Plastic")            // Plastified
  {
    return Graphic3d_NOM_PLASTIC;
  }
  else if (aName == "Shiny_plastic") // Shiny_plastified
  {
    return Graphic3d_NOM_SHINY_PLASTIC;
  }
  else if (aName == "Plaster")       // Plastered
  {
    return Graphic3d_NOM_PLASTER;
  }
  else if (aName == "Satin")         // Satined
  {
    return Graphic3d_NOM_SATIN;
  }
  else if (aName == "Neon_gnc")      // Ionized
  {
    return Graphic3d_NOM_NEON_GNC;
  }
  else if (aName == "Neon_phc") // Neon
  {
    return Graphic3d_NOM_NEON_PHC;
  }
  return Graphic3d_NOM_DEFAULT;
}

Graphic3d_TypeOfMaterial Graphic3d_MaterialAspect::MaterialType(const Standard_Integer aRank) {

  if( aRank < 1 || aRank > NumberOfMaterials() )
        Standard_OutOfRange::Raise(" BAD index of material");

  return theMaterials[aRank-1].type;
}
#endif

#ifdef IMP171201
Standard_CString Graphic3d_MaterialAspect::MaterialName() const
{
        return MyStringName.ToCString();
}
void Graphic3d_MaterialAspect::SetMaterialName(const Standard_CString aNewName)
{
// if a component of a "standard" material change, the
// result is no more standard ( a blue gold is not a gold )
	MyMaterialName = Graphic3d_NOM_UserDefined;
        MyStringName = aNewName;
}
#endif
