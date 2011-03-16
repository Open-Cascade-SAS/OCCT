//
// AdvApp2Var_SysBase.hxx
//
#ifndef AdvApp2Var_SysBase_HeaderFile
#define AdvApp2Var_SysBase_HeaderFile

#include <Standard_Macro.hxx>
#include <AdvApp2Var_Data_f2c.hxx>

//
class AdvApp2Var_SysBase {
 public: 
  //
  Standard_EXPORT static int mainial_();

  Standard_EXPORT static int macinit_(int *, 
				      int *);
  //
  Standard_EXPORT static int mcrdelt_(integer *iunit, 
				      integer *isize, 
				      doublereal *t, 
				      long int *iofset, 
				      integer *iercod);

  Standard_EXPORT static int mcrfill_(integer *size, 
				      char *tin, 
				      char *tout);

  Standard_EXPORT static int mcrrqst_(integer *iunit, 
				      integer *isize, 
				      doublereal *t, 
				      long int *iofset, 
				      integer *iercod);
  Standard_EXPORT static integer mnfndeb_();

  Standard_EXPORT static integer mnfnimp_();
  Standard_EXPORT static int do__fio(); 
  Standard_EXPORT static int do__lio ();
  Standard_EXPORT static int e__wsfe ();
  Standard_EXPORT static int e__wsle ();
  Standard_EXPORT static int s__wsfe ();
  Standard_EXPORT static int s__wsle ();
  Standard_EXPORT static int macrai4_(integer *nbelem, 
				      integer *maxelm, 
				      integer *itablo,
				      long int *iofset,
				      integer *iercod);
  Standard_EXPORT static int macrar8_(integer *nbelem, 
				      integer *maxelm,
				      doublereal *xtablo, 
				      long int *iofset, 
				      integer *iercod);
  Standard_EXPORT static int macrdi4_(integer *nbelem, 
				      integer *maxelm, 
				      integer *itablo, 
				      long int *iofset, 
				      integer *iercod);

  Standard_EXPORT static int macrdr8_(integer *nbelem,
				      integer *maxelm, 
				      doublereal *xtablo, 
				      long int *iofset, 
				      integer *iercod);
  Standard_EXPORT static int maermsg_(const char *cnompg, 
				      integer *icoder, 
				      ftnlen cnompg_len);
  Standard_EXPORT static int maitbr8_(integer *itaill, 
				      doublereal *xtab, 
				      doublereal *xval);
  Standard_EXPORT static int maovsr8_(integer *ivalcs);
  Standard_EXPORT static int mgenmsg_(const char *nomprg, 
				      ftnlen nomprg_len);

  Standard_EXPORT static int mgsomsg_(const char *nomprg, 
				      ftnlen nomprg_len);
  Standard_EXPORT static void miraz_(integer *taille,
				     char *adt);
  Standard_EXPORT static int msifill_(integer *nbintg, 
				      integer *ivecin,
				      integer *ivecou);
  Standard_EXPORT static int msrfill_(integer *nbreel, 
				      doublereal *vecent,
				      doublereal * vecsor);
  Standard_EXPORT static int mswrdbg_(const char *ctexte, 
				      ftnlen ctexte_len);
  Standard_EXPORT static void mvriraz_(integer *taille,
				       char *adt);
  
};

#endif
