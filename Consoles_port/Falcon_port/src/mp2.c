//
// C Wrapper for MP2 Audio Player - by Orion_ [2013]
//
// DSP MPEG Audio Layer 2 player by Tomas Berndtsson, NoBrain/NoCrew
//
// Add these files to your compilation command: mp2.c mp2.s
//

#include <tos.h>
#include "mp2.h"

extern	unsigned long	asm_mp2_player[];
LONG					asm_mp2_start(void);
LONG					asm_mp2_stop(void);
unsigned char	*DMAbuffer = 0;
int				DMAbuflen;

char	*MP2_Load(char *filename)
{
	#ifndef NO_DSP
	unsigned short	mp2filehandle;
	int	handle, size;

	if (Dsp_Lock() != 0)
		return ("");

	if (Locksnd() != 1)
	{
		Dsp_Unlock();
		return ("");
	}

	handle = Fopen(filename, 1);
	if (handle < 0)
	{
		Unlocksnd();
		Dsp_Unlock();
		return ("");
	}

	mp2filehandle = handle & 0xFFFF;
	size = Fseek(0, mp2filehandle, 2);
	Fseek(0, mp2filehandle, 0);

	// Sound buffer, always in ST Ram !
	DMAbuffer = Mxalloc(size, MX_STRAM);

	if (DMAbuffer)	// If enough memory
	{
		Fread(mp2filehandle, size, DMAbuffer);
		Fclose(mp2filehandle);
		DMAbuflen = size;
	}
	else
	{
		Fclose(mp2filehandle);
		Unlocksnd();
		Dsp_Unlock();
		return ("");
	}
	#endif
	return (0);
}


void	MP2_Start(unsigned char loop)
{
	#ifndef NO_DSP
	if (DMAbuffer)
	{
		unsigned long	*ptr = &asm_mp2_player[7];

		*ptr++ = (unsigned long)DMAbuffer;
		*ptr++ = (unsigned long)DMAbuflen;
		*ptr++ = MP2_INT_SPEED;
		*ptr++ = MP2_EXT_SPEED;
		*ptr++ = loop*8;

		Supexec(asm_mp2_start);
	}
	#endif
}


void	MP2_Stop(void)
{
	#ifndef NO_DSP
	if (DMAbuffer)
	{
		Supexec(asm_mp2_stop);
		Mfree(DMAbuffer);
		DMAbuffer = 0;
	}
	Unlocksnd();
	Dsp_Unlock();
	#endif
}
