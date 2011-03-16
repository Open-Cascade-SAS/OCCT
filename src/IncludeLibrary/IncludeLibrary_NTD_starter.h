#define HOSTLENGTH 20
#define ENGINELENGTH 131
#define SERVERLENGTH 80
#define MAXENGINE 32

struct START {
	int lvl, func, narg ;
	int prtcl ;
	char hostname[HOSTLENGTH+1] ;
	char engine[ENGINELENGTH+1] ;
} ;

struct STARTRSP {
	int lvl, func, narg ;
	char hostname[HOSTLENGTH+1] ;
	char server[SERVERLENGTH+1] ;
	char prtcl ;
	char serverd[SERVERLENGTH+1] ;
	char prtcld ;
} ;

//struct STARTED {
//	int lvl, func, narg ;
//	char engine[ENGINELENGTH+1] ;
//	char server[SERVERLENGTH+1] ;
//	int prtcl ;
//} ;

//struct STARTEDRSP {
//	int lvl, func, narg ;
//} ;
