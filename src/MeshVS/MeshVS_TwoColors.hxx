// File:	MeshVS_TwoColors.hxx
// Created:	Wed Oct 08 2003
// Author:	Alexander SOLOVYOV
// Copyright:	 Open CASCADE 2003

#ifndef MeshVS_TWOCOLORSHXX
#define MeshVS_TWOCOLORSHXX

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#include <Quantity_Color.hxx>

Standard_EXPORT typedef struct {
  unsigned int r1 : 8;
  unsigned int g1 : 8;
  unsigned int b1 : 8;
  unsigned int r2 : 8;
  unsigned int g2 : 8;
  unsigned int b2 : 8;
}   MeshVS_TwoColors;


Standard_EXPORT Standard_Integer HashCode ( const MeshVS_TwoColors& theKey,
                                            const Standard_Integer  theUpper  );

Standard_EXPORT Standard_Boolean IsEqual (const MeshVS_TwoColors& K1,
                                          const MeshVS_TwoColors& K2  );

Standard_EXPORT Standard_Boolean operator== ( const MeshVS_TwoColors& K1,
                                              const MeshVS_TwoColors& K2  );

Standard_EXPORT MeshVS_TwoColors  BindTwoColors ( const Quantity_Color&, const Quantity_Color& );
Standard_EXPORT Quantity_Color    ExtractColor  ( MeshVS_TwoColors&, const Standard_Integer );
Standard_EXPORT void              ExtractColors ( MeshVS_TwoColors&, Quantity_Color&, Quantity_Color& );

#endif
