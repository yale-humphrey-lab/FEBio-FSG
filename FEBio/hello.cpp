#include "stdafx.h"
#include "version.h"

unsigned char banner[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,8,8,8,8,8,0,0,0,8,8,8,8,8,8,8,8,0,0,0,8,8,8,8,8,8,8,8,0,0,0,0,8,8,7,0,0,0,0,0,0,8,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,8,8,8,8,8,7,0,0,8,8,8,8,8,8,8,8,7,0,0,8,8,8,8,8,8,8,8,8,7,0,0,0,7,7,0,0,0,0,8,8,8,8,8,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,7,7,7,7,7,7,7,0,0,8,8,7,7,7,7,7,7,7,0,0,8,8,7,7,7,7,7,8,8,8,7,0,0,0,0,0,0,0,8,8,8,7,7,7,7,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,8,8,7,0,8,8,0,0,0,0,8,8,7,7,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,8,8,7,0,8,8,7,0,0,8,8,7,7,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,8,8,8,7,0,8,8,7,0,0,8,8,7,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,8,8,8,8,8,8,8,8,8,7,7,0,8,8,7,0,0,8,8,7,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,8,8,8,8,7,0,0,0,8,8,8,8,8,8,8,7,0,0,0,8,8,8,8,8,8,8,8,8,7,0,0,8,8,7,0,0,8,8,7,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,7,7,7,7,7,7,0,0,0,8,8,7,7,7,7,7,7,0,0,0,8,8,7,7,7,7,7,8,8,8,0,0,8,8,7,0,0,8,8,7,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,8,8,7,0,8,8,7,0,0,8,8,7,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,8,8,7,0,8,8,7,0,0,0,8,8,0,0,0,0,0,0,8,8,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,8,8,8,7,0,8,8,7,0,0,0,8,8,8,0,0,0,0,8,8,8,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,8,8,8,8,8,8,8,8,0,0,0,8,8,8,8,8,8,8,8,8,7,7,0,8,8,7,0,0,0,0,8,8,8,8,8,8,8,8,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,7,0,0,0,0,0,0,0,0,8,8,8,8,8,8,8,8,7,0,0,8,8,8,8,8,8,8,8,7,7,0,0,8,8,7,0,0,0,0,0,7,8,8,8,8,7,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,0,0,0,0,0,0,0,0,0,7,7,7,7,7,7,7,7,0,0,0,7,7,7,7,7,7,7,7,0,0,0,0,7,7,0,0,0,0,0,0,0,7,7,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};


///////////////////////////////////////////////////////////////////////////////
// FUNCTION : Hello
// Prints the FEBio banner to a file
//

#ifdef WIN32

#include "console.h"
#include "windows.h"

void print_banner()
{
	char sz[] = "                                                                          ";

	Console* pwnd = Console::GetHandle();

	pwnd->Write(sz, 0xF0 );
	pwnd->Draw(banner, 80, 18);

	printf("                                                                           \n");
	printf("      F I N I T E   E L E M E N T S   F O R   B I O M E C H A N I C S      \n");
	printf("                                                                           \n");
	printf("                 --- v e r s i o n - %d . %d . %d . %d ---                 \n", 
		VERSION, SUBVERSION, SUBSUBVERSION, SVNREVISION);
	printf("                                                                           \n");
	printf("                                                                           \n");
	printf("  Musculoskeletal Research Laboratory                                      \n");
	printf("  University of Utah                                                       \n");
	printf("  http://mrl.sci.utah.edu                                                  \n");
	printf("                                                                           \n");
	printf("  copyright (c) 2006-2009 - All rights reserved                            \n");
	printf("                                                                           \n");
	pwnd->Write(sz, 0xF0 );
	printf("\n\n");
}

#else

void print_banner() {}

#endif

void Hello(FILE* fp)
{
	fprintf(fp,"===========================================================================\n");
	fprintf(fp,"         ________    _________   _________     __     _________            \n");
	fprintf(fp,"        |        |\\ |        |\\ |        |\\   |  |\\  /         \\\\          \n");
	fprintf(fp,"        |    ____|| |    ____|| |    __  ||   |__|| |    ___    ||         \n");
	fprintf(fp,"        |   |\\___\\| |   |\\___\\| |   |\\_| ||    \\_\\| |   //  \\   ||         \n");
	fprintf(fp,"        |   ||      |   ||      |   || | ||    __   |  ||    |  ||         \n");
	fprintf(fp,"        |   ||__    |   ||__    |   ||_| ||   |  |\\ |  ||    |  ||         \n");
	fprintf(fp,"        |       |\\  |       |\\  |         \\\\  |  || |  ||    |  ||         \n");
	fprintf(fp,"        |    ___||  |    ___||  |    ___   || |  || |  ||    |  ||         \n");
	fprintf(fp,"        |   |\\__\\|  |   |\\__\\|  |   |\\__|  || |  || |  ||    |  ||         \n");
	fprintf(fp,"        |   ||      |   ||      |   ||  |  || |  || |  ||    |  ||         \n");
	fprintf(fp,"        |   ||      |   ||___   |   ||__|  || |  || |   \\\\__/   ||         \n");
	fprintf(fp,"        |   ||      |        |\\ |          || |  || |           ||         \n");
	fprintf(fp,"        |___||      |________|| |__________|| |__||  \\_________//          \n");
	fprintf(fp,"                                                                           \n");
	fprintf(fp,"      F I N I T E   E L E M E N T S   F O R   B I O M E C H A N I C S      \n");
	fprintf(fp,"                                                                           \n");
	fprintf(fp,"                 --- v e r s i o n - %d . %d . %d . %d ---                 \n", 
		VERSION, SUBVERSION, SUBSUBVERSION, SVNREVISION);
	fprintf(fp,"                                                                           \n");
	fprintf(fp,"                                                                           \n");
	fprintf(fp,"  Musculoskeletal Research Laboratory                                      \n");
	fprintf(fp,"  University of Utah                                                       \n");
	fprintf(fp,"  http://mrl.sci.utah.edu                                                  \n");
	fprintf(fp,"                                                                           \n");
	fprintf(fp,"  copyright (c) 2006-2009 - All rights reserved                            \n");
	fprintf(fp,"                                                                           \n");
	fprintf(fp,"===========================================================================\n");
	fprintf(fp,"\n\n");
}
