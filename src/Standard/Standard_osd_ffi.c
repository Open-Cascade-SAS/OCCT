/*
 Copyright (c) 1998-1999 Matra Datavision
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

/**********************************************************
*/ 
/**    RBA                  12/10/88
*/ 
/**    ---                  --------
*/ 
/**
*/ 
/**  Fichier  : OSD_FFI.C    (SUN/UNIX)
*/ 
/**  Titre    : osd_ffi_(dir, name, tab_res, max, l_dir, l_name, l_tab)
*/ 
/**             char *dir, *nmae, *tab-res;
*/ 
/**             int *max;
*/ 
/**             int l_dir, l_name, l_tab;
*/ 
/**  Retour:
*/ 
/**            Nombre de fichier trouve (1 ou plus)
*/ 
/**            0    sinon.
*/ 
/**
*/ 
/**  Role     : Trouver tous les fichier qui ont le meme nom que 'name' sachant
*/ 
/**             que 'name' peut etre un nom de fichier comportant le caractere
*/ 
/**             joker '*'.
*/ 
/**  Exemple  :
*/ 
/**             Appel en Fortran
*/ 
/**
*/ 
/**             integer nb_fichier, osd_ffi
*/ 
/**             character*80 dir, name
*/ 
/**             character*80 tab_res(100)
*/ 
/**             ....
*/ 
/**             ....
*/ 
/**             name = 'TOTO*.*'
*/ 
/**             ....
*/ 
/**             nb_fichier = osd_ffi(dir, name, tab_res, 100)
*/ 
/**
*/ 
/**  Modifs : - sga 28/04/92 correction bug closedir
*/ 
/**           - asi 11/04/90 correction de strcmp_joker
*/ 
/**           - fsa 26/04/90 ajout du tri du tableau tab_res
*/ 
/**             par ordre aphabethique
*/ 
/**           - asi 06/09/90 ajout de la directorie aux noms de fichier
*/ 
/**             retourne
*/ 
/**           - J.P. TIRAULT le 3-Avril-1992 : Faire le close du directory
*/ 
/**             apres l'avoir ouvert.
*/ 
/**
*/ 
/*************************************************************/ 
#if defined(_MSC_VER) && !defined(WNT)
#error "Wrong compiler options has been detected. Add /DWNT option for proper compilation!!!!!"
#endif

#ifndef WNT

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* This hack is required to get HP's c89 to include a definition for DIR */
#ifdef __hpux
# ifndef _INCLUDE_POSIX_SOURCE
#  define _INCLUDE_POSIX_SOURCE
# endif
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#ifdef	HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
# ifdef VMS
extern char *vmsify PARAMS ((char *name, int type));
# endif
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# ifdef HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# ifdef HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# ifdef HAVE_NDIR_H
#  include <ndir.h>
# endif
# ifdef HAVE_VMSDIR_H
#  include "vmsdir.h"
# endif /* HAVE_VMSDIR_H */
#endif

/* In GNU systems, <dirent.h> defines this macro for us.  */
#ifdef _D_NAMLEN
# undef NAMLEN
# define NAMLEN(d) _D_NAMLEN(d)
#endif

#if (defined (POSIX) || defined (VMS) || defined (WINDOWS32)) && !defined (__GNU_LIBRARY__)
/* Posix does not require that the d_ino field be present, and some
   systems do not provide it. */
# define REAL_DIR_ENTRY(dp) 1
# define FAKE_DIR_ENTRY(dp)
#else
# define REAL_DIR_ENTRY(dp) (dp->d_ino != 0)
# define FAKE_DIR_ENTRY(dp) (dp->d_ino = 1)
#endif /* POSIX */

DIR *dirp;
struct dirent *dp;

void tri(int taille, char *list,int l_tab);
int strcmp_joker(char *s1, char *s2);

int
osd_ffi(char *dir, char *name, char *tab_res, int *max, int l_dir, int l_name, int l_tab)
{
     char file[255], directory[255];
     int  ii, jj, kk, nb_file;

     /***** Le Nom de la directory ******/
      if (*dir == ' ') {
          directory[0] = '.';
          directory[1] = 0;
      }
      else {
          for (ii=0; ii<l_dir && ii<255 && *dir != ' '; ii++)
             directory[ii] = *(dir++);
          directory[ii]=0;
      }

      /***** Le Nom de fichier recherche *******/
      for (ii=0; ii<l_name  && ii<255 && *name != ' '; ii++)
             file[ii] = *(name++);
      file[ii]=0;

      /****** On ouvre le directory pour trouver les fichiers ********/
      nb_file=0;
      if ((dirp = opendir(directory)) != NULL){
          while ((nb_file < *max) && (dp=readdir(dirp))) {
              /*====== Si on trouve un fichier qui lui ressemble ==========*/
              if(strcmp_joker(file,dp->d_name) == 0) {
                 /*======= On copie sa valeur ========*/
                 /*        avec la directorie         */
                 /* jj tab_res, kk la ligne ,ii le nom */
                 jj = nb_file * l_tab;
                 kk = 0;
                 if (directory[0] != '.') {
                    for (; kk<l_tab && directory[kk] != 0; kk++,jj++)
                        *(tab_res + jj) = directory[kk];
                    if (directory[kk-1] != '/')  *(tab_res + (jj++)) = '/';
                 }
                 for (ii=0; (kk)<l_tab && *(dp->d_name+ii); ii++,jj++,kk++)
                     *(tab_res + jj) = *(dp->d_name + ii);
                 /*======= En completant avec blanc ======*/
                 for (; kk<l_tab ; jj++,kk++)
                     *(tab_res + jj) = ' ';
                 nb_file++;
              }
          }
      }
/***** tri du tableau tab_res par ordre alpha *****/
     if (dirp){
       closedir(dirp);
       tri( nb_file , tab_res ,l_tab );
      }
      return(nb_file);
}

/***********************************************
*  RBA                        Date: 27/06/88
*  ---                             ---------
*
* Title: STRCMP_JOKER(ch1,ch2)
* Role : Compare ch1 with ch2
*        ch1 can have the joker caracter '*'
*        res = 0 they are identical
*        res <>0 otherwise
* Modif: asi 11/04/90 correction
***********************************************/


#define TRUE  1
#define FALSE 0
#define JOKER '*'

int
strcmp_joker(char *s1, char *s2)
{
    char *c1=NULL,*c2=NULL;  /* sauvegardes */
    int res;
    int deb = 1;  /* 0 si 1er car est le joker, 1 sinon */

    while (*s2) {
        if (*s1 == JOKER) {
            deb = 0;
            while(*s1 == JOKER) s1++;
            if (*s1) {
                while(*s2 && *s2 != *s1) s2++;
                c1 = s1, c2 = s2;
            }
            else return(0); /* s1 fini par un jocker */
        }
        else { /* comparaison de 2 car */
            res = *s1++ - *s2++;
            if (res != 0) {
                /* 1ere chaine pas de joker au debut */
                if (deb) return (res);
                else {  /* on recherche la meme chaine plus loin */
                    s1 = c1, s2 = ++c2;
                    while(*s2 && *s2 != *s1) s2++;
                    c2 = s2;
                }
            }
        }
    }
    while(*s1 == JOKER) s1++;
    return(*s1 - *s2);
}

/**************************************************
* FSA                               date: 26/04/90
* ---                                    ---------
*
* Titre : COMPAR (ch1,ch2)
* Role  : Comparer ch1 avec ch2
*         retourne :  0 si chaines identiques
*                    >0 si ch2 avant ch1
*                    <0 si ch1 avant ch2
* Modif :
**************************************************/

static int
compar(char *ch1, char *ch2)
{
  /* int strcmp(char *cha, const char *chb); */

 return(strcmp(ch1,ch2));
}

/**************************************************
* FSA                               date: 26/04/90
* ---                                    ---------
*
* Titre : TRI (taille,list,l_tab)
* Role  : Tri un tableau de chaines par ordre
*         alphabetique
* Modif :
**************************************************/

void
tri(int taille, char *list,int l_tab)
{
 qsort( list , taille , l_tab ,(int (*)(const void *,const void *)) compar );
}

#else
// VERSION WNT
//
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#ifdef VAC
#include <windows.h> // added for VAC++ version  -ChG 07081997
#endif

#ifdef DOS
#include <dos.h>
static struct _find_t zeptr ;

#else

#ifdef VAC
static LPWIN32_FIND_DATA zeptr ;
#else
static struct _finddata_t zeptr ;
#endif

#endif
#ifdef VAC
HANDLE dirp;
#else
ptrdiff_t dirp;
#endif

char filespec[255];


int
osd_ffi_(char *dir, char *name, char *tab_res, int *max, int l_dir, int l_name, int l_tab)
{
     char file[255], directory[255];
     int  ii, jj, kk, nb_file;
#ifdef VAC
     BOOL  fini ;
#else
     int  fini ;
#endif

     /***** Le Nom de la directory ******/
      if (*dir == ' ') {
          directory[0] = '.';
          directory[1] = '/';
          directory[2] = 0 ;
      }
      else {
          for (ii=0; ii<l_dir && ii<255 && *dir != ' '; ii++) {
             directory[ii] = *(dir++);
             if ( directory [ii] == '\\' ) directory [ii] = '/' ;
          }
          directory[ii]=0;
      }

      /***** Le Nom de fichier recherche *******/
      for (ii=0; ii<l_name  && ii<255 && *name != ' '; ii++)
             file[ii] = *(name++);
      file[ii]=0;

      /****** On ouvre le directory pour trouver les fichiers ********/
      nb_file=0;

      strcpy(filespec, directory );
      strcat(filespec, file );

#ifdef VAC
      if ((dirp = FindFirstFile(filespec, zeptr)) != INVALID_HANDLE_VALUE ) {
#else
      if ((dirp = _findfirst(filespec, &zeptr)) != -1 ) {
#endif
          fini = 0 ;
          while (nb_file < *max && (! fini ) ) {

              /*====== Si on trouve un fichier qui lui ressemble ==========*/


                 /*======= On copie sa valeur ========*/
                 /*        avec la directorie         */
                 /* jj tab_res, kk la ligne ,ii le nom */
                 jj = nb_file * l_tab;
                 kk = 0;
                 if (directory[0] != '.') {
                    for (; kk<l_tab && directory[kk] != 0; kk++,jj++)
                        *(tab_res + jj) = directory[kk];

                    if (directory[kk-1] != '/')  *(tab_res + (jj++)) = '/';
                 }

#ifdef VAC
                 for (ii=0; (kk)<l_tab && *((*zeptr).cFileName+ii); ii++,jj++,kk++)
                     *(tab_res + jj) = *((*zeptr).cFileName + ii);
#else
                 for (ii=0; (kk)<l_tab && *(zeptr.name+ii); ii++,jj++,kk++)
                     *(tab_res + jj) = *(zeptr.name + ii);
#endif

                 /*======= En completant avec blanc ======*/

                 for (; kk<l_tab ; jj++,kk++)
                     *(tab_res + jj) = ' ';
                 nb_file++;

#ifdef VAC
	      fini = FindNextFile(dirp, zeptr );
#else
	      fini = _findnext(dirp, &zeptr );
#endif
          }
      }
/***** pas de tri du tableau tab_res par ordre alpha *****/

#ifdef VAC
	  if (dirp != INVALID_HANDLE_VALUE) FindClose(dirp);
#else
	  if (dirp) _findclose(dirp);
#endif

      return(nb_file);
}
#endif

