
/*
  Cree par JPT pour la protection SOK sur base CSF1.
  A ce jour on reconnait comme machine:
	- SUN/SOLARIS
	- ALPHA/OSF
	- SGI/IRIX
  Pour SUN et SGI on recupere le numero unique du host (hostid).
  Pour ALPHA on recupere l'adresse ethernet de la machine (ce qui est
  tout de meme beaucoup plus sur).

*/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif

#ifdef HAVE_SYS_SYSTEMINFO_H
# include <sys/systeminfo.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#ifdef HAVE_NET_IF_H
# include <net/if.h>
#endif

#include <Standard_Stream.hxx>

#ifdef HAVE_SYS_UTSNAME_H
# include <sys/utsname.h>
#endif

#include <errno.h>
#include <stdio.h>



#if defined(__sun) || defined(SOLARIS)
extern "C" { int sysinfo (int command , char *name, long namelen);}
             int atoi (char *);
#elif defined(__sgi)  || defined(IRIX)
extern "C" {int sysid (unsigned char *);}
extern "C" {int atoi (const char *str);}
#elif defined(__osf__) || defined(DECOSF1)
// generic interface stuctures
#include <string.h>
char  *ether_devices[] = {
       "qe0",
       "se0",
       "ln0",
       "de0",
       "ni0",
       "tu0",
        NULL
       };
#endif

// --------------------------------------------------------------------------
void Geteth(int tab[])
// --------------------------------------------------------------------------
{
#if defined(__sun) || defined(SOLARIS)
  /* Returns a SOLARIS host identification in 2 words */
  unsigned int i;
  char provider[16];
  char serial[16];
  
  i = (unsigned int ) sysinfo(SI_HW_PROVIDER,provider,16);
  
  i = (unsigned int ) sysinfo(SI_HW_SERIAL,serial,16);
  
  
  /*printf("\nProvider : %s Serial : %s\n",provider,serial);*/
  
  sscanf(serial,"%d",&i);
  
  tab[0] = 0;
  tab[1] = 0;
  
  tab[1] = tab[1] | (i >> 24);
  tab[1] = tab[1] | ( (i >> 8 ) & 0x0000ff00);
  tab[1] = tab[1] | ( (i << 8 ) & 0x00ff0000);
  tab[1] = tab[1] | ( (i << 24) & 0xff000000);
  
  tab[1] = -tab[1];
  

// unsigned int   stat;
//  char  name[100];
//  long  len = 100;
//  stat = sysinfo (SI_HW_SERIAL,name,len);
//  if (stat != -1){   // It is correct.....
//    int  i = atoi (name);
//    tab[0] = 0;
//    tab[1] = 0;
//    tab[1] = tab[1] | (i >> 24);
//    tab[1] = tab[1] | ( (i >> 8)  & 0x0000ff00);
//    tab[1] = tab[1] | ( (i << 8)  & 0x00ff0000);
//    tab[1] = tab[1] | ( (i << 24) & 0xff000000);
//    tab[1] = -tab[1];
//  }
//  else {             // It is nor normal : exit !
//    printf("SOK_Utility-Internal_Error_1:Unable to get hardware-specific serial number, errno = %d.\n",errno);
//    exit(0);
//  }

#elif defined(__sgi) || defined(IRIX)

/* Creee par JPT le 29-Oct-1992

   Renvoie l'identification d'un hostname SGI  dans 2 mots.

   Sur SGI l'appel a sysid renvois 16 caracteres qui identifie de
   fa on unique une machine.

   Sur SGI/xxxx avec xxxx autre que CRIMSON seuls les 8 premiers
   caracteres sont significatifs. Les 8 autres sont nuls.
   
   Pour  tre homogene avec les autres plateformes on prend les 16
   caracteres et on rend 12 chiffres hexadecimaux.


*/

unsigned int i                   		;
unsigned int tt  [4]                            ;
         char str[16]                           ;

sysid ( (unsigned char* )str )	              	;
tt[0] = tt[1] = tt[2] = tt[3] = 0               ;
tt[0]  = atoi (str)                             ;
tab[0] = 0			                ;
tab[1] = 0			                ;
i      = tt[2] ^ tt[3]		            	;
i      = i ^ tt [0]                        	;
tab[1] = tab[1] | (i >> 24)			;
tab[1] = tab[1] | ( (i >> 8 ) & 0x0000ff00)	;
tab[1] = tab[1] | ( (i << 8 ) & 0x00ff0000)	;
tab[1] = tab[1] | ( (i << 24) & 0xff000000)	;
tab[1] = -tab[1]                                ;

i      = tt[1] >> 16				;
i      = i ^ ( tt[1] & 0x0000ffff )		;
tab[0] = i					;
tab[0] = -tab[0]				;


#elif defined(__osf__) || defined(DECOSF1)

   struct ifdevea devea;

   int ss, i;

   char id[8];
   int *pid;

   ss = socket (AF_INET, SOCK_DGRAM, 0);
   if (ss < 0)
     return;
   else {
      for (i = 0; ether_devices[i] != NULL; i++) {
          strcpy (&devea.ifr_name[0], ether_devices[i]);
          if (ioctl (ss, SIOCRPHYSADDR, &devea) < 0) {
             if (errno == ENXIO)  // doesn't exist, try next device
               continue;
             else
               return;
	   }
          else
            break;                // found one, break out
	}
    }

   close (ss);

   if (ether_devices[i] == NULL)
      return;

   id[3]  = 0                  ;
   id[2]  = 0                  ;
   id[4]  = devea.default_pa[0];
   id[5]  = devea.default_pa[1];
   
   id[6]  = devea.default_pa[2];
   id[7]  = devea.default_pa[3];
   id[0]  = devea.default_pa[4];
   id[1]  = devea.default_pa[5];


   pid    = (int *)&id         ;
   tab[0] = pid[0]             ;
   tab[1] = pid[1]             ;
   tab[0] = - tab[0]           ;
   tab[1] = - tab[1]           ;


#elif defined(__hpux) || defined(HPUX)
long i;
struct utsname un;

tab[0] = 0;
tab[1] = 0;
if (uname(&un) != -1)
  {sscanf(un.idnumber,"%d",&i);
   tab[1] = int( tab[1] | (i >> 24) ) ;
   tab[1] = int( tab[1] | ( (i >> 8 ) & 0x0000ff00) ) ;
   tab[1] = int( tab[1] | ( (i << 8 ) & 0x00ff0000) ) ;
   tab[1] = int( tab[1] | ( (i << 24) & 0xff000000) ) ;

   tab[1] = -tab[1];};

//return ((long) tab);

#endif

}









