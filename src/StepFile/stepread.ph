// stepread.h

/* lecture du fichier STEP (par appel a lex+yac) */

extern "C" FILE* stepread_setinput (char* nomfic) ;
extern "C" void stepread_endinput (FILE* infic, char* nomfic);
extern "C" int  stepread() ;
extern "C" void recfile_modeprint (int mode) ;     /* controle trace recfile */

/* creation du Direc a partir de recfile : entrys connues de c++ */
extern "C" void lir_file_nbr(int* nbh, int* nbr, int* nbp) ;
extern "C" int  lir_file_rec(char* *ident , char* *type , int* nbarg) ;
extern "C" void lir_file_finrec() ;
extern "C" int  lir_file_arg(int* type , char* *val) ;
extern "C" void lir_file_fin(int mode);

/* Interruption passant par C++  */
extern "C" void StepFile_Interrupt (char* nomfic);
