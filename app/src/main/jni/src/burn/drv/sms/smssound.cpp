/*
    sound.c --
    Sound emulation.
*/
#include "tiles_generic.h"
#include "smsshared.h"
#include "sn76496.h"

snd_t snd;

int sound_init(void)
{
    /* If we are reinitializing, shut down sound emulation */
    if(snd.enabled)
    {
        sound_shutdown();
    }

    /* Disable sound until initialization is complete */
    snd.enabled = 0;

    /* Check if sample rate is invalid */
    if(snd.sample_rate < 8000 || snd.sample_rate > 48000)
        return 0;

    // Init sound emulation
	SN76489Init(0, snd.psg_clock, 0);
	SN76496SetRoute(0, 0.70, BURN_SND_ROUTE_BOTH);

	FM_Init();

    /* Inform other functions that we can use sound */
    snd.enabled = 1;

    return 1;
}


void sound_shutdown(void)
{
    if(!snd.enabled)
        return;

    /* Shut down SN76489 emulation */
	SN76496Exit();

    /* Shut down YM2413 emulation */
	FM_Shutdown();

	snd.enabled = 0;
}


void sound_reset(void)
{
    if(!snd.enabled)
        return;

    /* Reset SN76489 emulator */
    SN76496Reset();

    /* Reset YM2413 emulator */
    FM_Reset();
}


/*--------------------------------------------------------------------------*/
/* Sound chip access handlers                                               */
/*--------------------------------------------------------------------------*/

void psg_stereo_w(int data)
{
    if(!snd.enabled)
        return;

	SN76496StereoWrite(0, data);
}


void psg_write(int data)
{
    if(!snd.enabled)
        return;

	SN76496Write(0, data);
}

/*--------------------------------------------------------------------------*/
/* Mark III FM Unit / Master System (J) built-in FM handlers                */
/*--------------------------------------------------------------------------*/

int fmunit_detect_r(void)
{
    return sms.fm_detect;
}

void fmunit_detect_w(int data)
{
    sms.fm_detect = data;
}

void fmunit_write(int offset, int data)
{
    if(!snd.enabled || !sms.use_fm)
        return;

    FM_Write(offset, data);
}


