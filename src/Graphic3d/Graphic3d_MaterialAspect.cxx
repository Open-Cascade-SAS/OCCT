// File		Graphic3d_MaterialAspect.cxx
// Created	Fevrier 1992
// Author	NW,JPB,CAL
// modified     1/07/97 ; PCT : ajout texture mapping
//              8/04/98 ; FGU : Ajout Emission
//              9/07/98 ; CAL : Ajout de l'initialisation de l'EmissiveCoef
//              26/03/99 ; FMN : Ajout methodes SetColor() et Color()
//              26/03/99 : FMN ; Compatibilite ascendante: Ajout des anciens noms de materiaux.
//              09/04/99 : GG ; Compatibilite ascendante: NameOfPhysicalMaterial disparait
//		07/07/99 : GG ; MyEmissiveActivity MUST be initialized in the
//			   constructor of the class.
//              23/11/99 : GG ; BUC60488 : Add Name() & Reset() methods

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

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration des variables specifiques aux definition
//		des matieres

//-Warning	Une matiere est definie par :
//		- Un coefficient de transparence
//		- Un coefficient de reflection diffuse
//		- Un coefficient de reflection ambiante
//		- Un coefficient de reflection speculaire
//              - Un coefficient d emission

//		2 proprietes permettent de definir une matiere :
//		- sa transparence
//		- sa reflection c-a-d ses proprietes d'absorbtion
//		et de diffusion de la lumiere

//		La reflection diffuse est percue comme une composante
//		La reflection speculaire est percue comme une composante
//		de la couleur de la source lumineuse

//		Pour determiner les 3 couleurs de reflection il faut :
//		- Un coefficient de reflection diffuse
//		- Un coefficient de reflection ambiante
//		- Un coefficient de reflection speculaire

//		( Sous GL, on determine 3 couleurs )

//-References	Getting started with DEC PHIGS, annexe C
//		Iris Advanced Graphics, unite D

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_MaterialAspect.ixx>

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
			/* Coefficient d eclairage */
			MyShininess		= Standard_ShortReal (0.0078125);
			MyAmbientCoef		= Standard_ShortReal (0.5);
			MyDiffuseCoef		= Standard_ShortReal (0.24);
			MySpecularCoef		= Standard_ShortReal (0.06);
		break;				
		
	        case Graphic3d_NOM_SHINY_PLASTIC : /* black plastic */
			/* Coef d eclairage */
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
			/* Coef d eclairage */
			MyShininess		= Standard_ShortReal (0.05);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (0.62);
			MyEmissiveCoef          = Standard_ShortReal (1.0);
			MyEmissiveActivity = Standard_True;
			MyAmbientActivity  = Standard_False;											  
		break;
		
		case Graphic3d_NOM_METALIZED :			
			/* Coef d eclairage */
			MyShininess		= Standard_ShortReal (0.13);
			MyAmbientCoef		= Standard_ShortReal (0.9);
			MyDiffuseCoef		= Standard_ShortReal (0.47);
			MySpecularCoef		= Standard_ShortReal (0.45);
			MyAmbientActivity  = Standard_False;
						
			/* Couleur issue du diffus */
			//MyDiffuseColor.SetValues (0.87, 0.96, 1.0, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */
			//MySpecularColor.SetValues (0.93, 0.95, 0.78, Quantity_TOC_RGB);						
		break;

// Compatibilite ascendante materiaux physiques. On prend la meme definition
// que dans le createur suivant.	

		case Graphic3d_NOM_BRASS :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
			/* Initialisation des coefficients et couleur d eclairage : brass */
			MyShininess		= Standard_ShortReal (0.21794844);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Couleur issue de l ambient */
			MyAmbientColor.SetValues (0.329412, 0.223529, 0.027451, Quantity_TOC_RGB);
			
			/* Couleur issue du diffus */
			MyDiffuseColor.SetValues (0.780392, 0.568627, 0.113725, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */
			MySpecularColor.SetValues (0.992157, 0.941176, 0.807843, Quantity_TOC_RGB);								
		break;

		case Graphic3d_NOM_BRONZE :						
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
		        /* Coef d eclairage */
			MyShininess		= Standard_ShortReal (0.2);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Couleur issue de l ambient */
			MyAmbientColor.SetValues (0.2125, 0.1275, 0.054, Quantity_TOC_RGB);
			
			/* Couleur issue du diffus */
			MyDiffuseColor.SetValues (0.714, 0.4284, 0.18144, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */
			MySpecularColor.SetValues (0.393548, 0.271906, 0.166721, Quantity_TOC_RGB);						
		break;

		case Graphic3d_NOM_COPPER :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
                        /* Coefficient d eclairage */
			MyShininess		= Standard_ShortReal (0.93);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Couleur issue de l ambient */
			MyAmbientColor.SetValues (0.33, 0.26, 0.23, Quantity_TOC_RGB);
			
			/* Couleur issue du diffus */
			MyDiffuseColor.SetValues (0.50, 0.11, 0.0, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */
			MySpecularColor.SetValues (0.95, 0.73, 0.0, Quantity_TOC_RGB);			
		break;

		case Graphic3d_NOM_GOLD :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
		        /* Coefficient d eclairage */
			MyShininess		= Standard_ShortReal (0.0625);
			MyAmbientCoef		= Standard_ShortReal (0.3);
			MyDiffuseCoef		= Standard_ShortReal (0.4);
			MySpecularCoef		= Standard_ShortReal (0.9);
			
			/* Couleur issue de l ambient */
			MyAmbientColor.SetValues (1.0, 0.76862745, 0.31764706, Quantity_TOC_RGB);
			
			/* Couleur issue du diffus */
			MyDiffuseColor.SetValues (1.0, 0.69, 0.0, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */
			MySpecularColor.SetValues (1.0, 0.98, 0.78, Quantity_TOC_RGB);									
		break;		

		case Graphic3d_NOM_PEWTER :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
			/* Coefficient d eclairage */
			MyShininess		= Standard_ShortReal (0.076923047);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Couleur issue de l ambient */
			MyAmbientColor.SetValues (0.105882, 0.058824, 0.113725, Quantity_TOC_RGB);
			
			/* Couleur issue du diffus */
			MyDiffuseColor.SetValues (0.427451, 0.470588, 0.541176, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */
			MySpecularColor.SetValues (0.333333, 0.333333, 0.521569, Quantity_TOC_RGB);					
		break;	

		case Graphic3d_NOM_PLASTER :
#ifdef BUG      // The plaster material must be considered as Generic.
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
                        /* Coefficient d eclairage */
			MyShininess		= Standard_ShortReal (0.2);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
#else
                        /* Coefficient d eclairage */
                        MyShininess             = Standard_ShortReal (0.0078125)
;
                        MyAmbientCoef           = Standard_ShortReal (0.26);
                        MyDiffuseCoef           = Standard_ShortReal (0.23);
                        MySpecularCoef          = Standard_ShortReal (0.06);
#endif
			
			/* Couleur issue de l ambient */
			MyAmbientColor.SetValues (0.19225, 0.19225, 0.19225, Quantity_TOC_RGB);
			
			/* Couleur issue du diffus */
			MyDiffuseColor.SetValues (0.50754, 0.50754, 0.50754, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */
			MySpecularColor.SetValues (0.508273, 0.508273, 0.508273, Quantity_TOC_RGB);
		break;

		case Graphic3d_NOM_SILVER :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
                        /* Coefficient d eclairage */
			MyShininess		= Standard_ShortReal (0.2);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Couleur issue de l ambient */
			MyAmbientColor.SetValues (0.19225, 0.19225, 0.19225, Quantity_TOC_RGB);
			
			/* Couleur issue du diffus */
			MyDiffuseColor.SetValues (0.50754, 0.50754, 0.50754, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */
			MySpecularColor.SetValues (0.508273, 0.508273, 0.508273, Quantity_TOC_RGB);
		break;
		
		case Graphic3d_NOM_STEEL :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
		        /* Coefficient d eclairage */
			MyShininess		= Standard_ShortReal (0.06); 
			MyAmbientCoef		= Standard_ShortReal (0.01);
			MyDiffuseCoef		= Standard_ShortReal (0.03);
			MySpecularCoef		= Standard_ShortReal (0.98);
			
			/* Couleur issue du speculaire */
			MySpecularColor.SetValues (1.0, 1.0, 1.0, Quantity_TOC_RGB);
		break;	
		
		case Graphic3d_NOM_STONE :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
		        /* Coefficient d eclairage */
			MyShininess		= Standard_ShortReal (0.17);
			MyAmbientCoef		= Standard_ShortReal (0.19);
			MyDiffuseCoef		= Standard_ShortReal (0.75);
			MySpecularCoef		= Standard_ShortReal (0.08);
			
			/* Couleur issue de l ambient */
			MyAmbientColor.SetValues (1.0, 0.8, 0.62, Quantity_TOC_RGB);
			
			/* Couleur issue du diffus */
			MyDiffuseColor.SetValues (1.0, 0.8, 0.62, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */						
			MySpecularColor.SetValues (0.98, 1.0, 0.60, Quantity_TOC_RGB);			
		break;

// Compatibilite ascendante materiaux physiques. On prend la meme definition
// que dans le createur suivant. Nouveaux materials
		
		case Graphic3d_NOM_CHROME :			
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
			/* Coef d eclairage */
			MyShininess		= Standard_ShortReal (0.1);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Couleur issue de l ambient */
			MyAmbientColor.SetValues (0.35, 0.35, 0.35, Quantity_TOC_RGB);
			
			/* Couleur issue du diffus */
			MyDiffuseColor.SetValues (0.4, 0.4, 0.4, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */
			MySpecularColor.SetValues (0.974597, 0.974597, 0.974597, Quantity_TOC_RGB);						
		break;
		
		case Graphic3d_NOM_NEON_PHC: 
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
			/* Coef d eclairage */
			MyShininess		= Standard_ShortReal (0.05);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (0.62);
			MyEmissiveCoef          = Standard_ShortReal (0.9);
			MyEmissiveActivity = Standard_True;
			MyAmbientActivity = Standard_False;
			MyDiffuseActivity  = Standard_False;
			
			/* Couleur issue de l ambient */
			MyAmbientColor.SetValues (1.0, 1.0, 1.0, Quantity_TOC_RGB);
			
			/* Couleur issue du diffus */
			MyDiffuseColor.SetValues (1.0, 1.0, 1.0, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */
			MySpecularColor.SetValues (1.0, 1.0, 1.0, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */			
			MyEmissiveColor.SetValues (0.0, 1.0, 0.46, Quantity_TOC_RGB);							  
		break;
		
		case Graphic3d_NOM_ALUMINIUM :			
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
			/* Coef d eclairage */
			MyShininess		= Standard_ShortReal (0.09);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Couleur issue de l ambient */
			MyAmbientColor.SetValues (0.30, 0.30, 0.30, Quantity_TOC_RGB);
			
			/* Couleur issue du diffus */
			MyDiffuseColor.SetValues (0.30, 0.30, 0.30, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */
			MySpecularColor.SetValues (0.70, 0.70, 0.80, Quantity_TOC_RGB);						
		break;
		
		case Graphic3d_NOM_OBSIDIAN :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
			/* Coef d eclairage */
			MyShininess		= Standard_ShortReal (0.3);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Couleur issue de l ambient */
			MyAmbientColor.SetValues (0.05375, 0.05, 0.06625, Quantity_TOC_RGB);
			
			/* Couleur issue du diffus */
			MyDiffuseColor.SetValues (0.18275, 0.17, 0.22525, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */
			MySpecularColor.SetValues (0.332741, 0.328634, 0.346435, Quantity_TOC_RGB);						
		break;	
		
		case Graphic3d_NOM_JADE :
			MyMaterialType = Graphic3d_MATERIAL_PHYSIC;
			/* Coef d eclairage */
			MyShininess		= Standard_ShortReal (0.1);
			MyAmbientCoef		= Standard_ShortReal (1.0);
			MyDiffuseCoef		= Standard_ShortReal (1.0);
			MySpecularCoef		= Standard_ShortReal (1.0);
			
			/* Couleur issue de l ambient */
			MyAmbientColor.SetValues (0.135, 0.2225, 0.1575, Quantity_TOC_RGB);
			
			/* Couleur issue du diffus */
			MyDiffuseColor.SetValues (0.54, 0.89, 0.63, Quantity_TOC_RGB);
			
			/* Couleur issue du speculaire */
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

/* Affectation type du materiel */
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

/* Affectation couleur eclairage ambient */
void Graphic3d_MaterialAspect::SetAmbientColor (const Quantity_Color& AColor) {

	MyAmbientColor	= AColor;
#ifdef IMP171201
        if( MyAmbientActivity && 
		(MyMaterialType == Graphic3d_MATERIAL_PHYSIC) )
		SetMaterialName("UserDefined");
#endif
}

/*  Affectation couleur eclairage diffus */
void Graphic3d_MaterialAspect::SetDiffuseColor (const Quantity_Color& AColor) {

	MyDiffuseColor	= AColor;
#ifdef IMP171201
        if( MyDiffuseActivity && 
		(MyMaterialType == Graphic3d_MATERIAL_PHYSIC) )
		SetMaterialName("UserDefined");
#endif
}

/*  Affectation couleur eclairage speculaire */
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

/* Affectation du coefficient d emission */
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

/* Acces a la couleur de l eclairage ambient */
Quantity_Color Graphic3d_MaterialAspect::AmbientColor () const {

	return (MyAmbientColor);
}

/* Acces a la couleur de l eclairage diffus */
Quantity_Color Graphic3d_MaterialAspect::DiffuseColor () const {

	return (MyDiffuseColor);
}

/* Acces a la couleur de l eclairage speculaire */
Quantity_Color Graphic3d_MaterialAspect::SpecularColor () const {

	return (MySpecularColor);
}

/* Acces a la couleur d emission */
Quantity_Color Graphic3d_MaterialAspect::EmissiveColor () const {

	return (MyEmissiveColor);
}

/* Acces type du materiel */
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

/* Acces a la couleur de l eclairage ambient */
Standard_Real Graphic3d_MaterialAspect::Ambient () const {

	return (Standard_Real (MyAmbientCoef));

}

/* Acces a la couleur de l eclairage diffus */
Standard_Real Graphic3d_MaterialAspect::Diffuse () const {

	return (Standard_Real (MyDiffuseCoef));

}

/* Valeur du coefficient d emission */
Standard_Real Graphic3d_MaterialAspect::Emissive () const {

	return (Standard_Real (MyEmissiveCoef));

}

/* Acces a la couleur de l eclairage speculaire */
Standard_Real Graphic3d_MaterialAspect::Specular () const {

	return (Standard_Real (MySpecularCoef));

}

/* Acces au coefficient de transparence */
Standard_Real Graphic3d_MaterialAspect::Transparency () const {

	return (Standard_Real (MyTransparencyCoef));

}

/* Acces au coefficient de brillance */
Standard_Real Graphic3d_MaterialAspect::Shininess () const {

	return (Standard_Real (MyShininess));

}

/* Affectation du coefficient de brillance */
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
Standard_Integer n =sizeof(theMaterials)/sizeof(Material);
  if( n > Graphic3d_NOM_DEFAULT ) {
    cout << " *** Graphic3d_MaterialAspect::NumberOfMaterials() may returns a badvalue due to incoherente size between material name array and enum" << endl;
  }
  return Graphic3d_NOM_DEFAULT;
}

Standard_CString Graphic3d_MaterialAspect::MaterialName(const Standard_Integer aRank) {

  if( aRank < 1 || aRank > NumberOfMaterials() )
        Standard_OutOfRange::Raise(" BAD index of material");

  return theMaterials[aRank-1].name;
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
