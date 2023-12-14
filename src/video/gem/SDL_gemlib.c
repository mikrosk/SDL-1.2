/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

/*
 * Replacement of some gemlib functions,
 * so that SDL does not depend on it
 *
 * 2023 Thorsten Otto
 */

#include <gem.h>
#include "gem_aesP.h"
#include "gem_vdiP.h"


/*
 * only the ones used by SDL are implemented here,
 * if that list ever changes, copy them from gemlib
 */

/*
 * AES bindings
 */

short
mt_appl_init(short *global_aes)
{
	AES_PARAMS(10,0,1,0,0);
	
	/* clear some variable that may be used to check if an AES is loaded */
	global_aes[0] = 0;  /* AES version number */
	global_aes[2] = -1;  /* AES application ID */
	aes_intout[0] = -1;  /* AES application ID */

	AES_TRAP(aes_params);

	return aes_intout[0];
}

short
mt_appl_exit(short *global_aes)
{
	AES_PARAMS(19,0,1,0,0);

	AES_TRAP(aes_params);

	return aes_intout[0];
}

short
mt_appl_getinfo (short type, short *out1, short *out2, short *out3, short *out4, short *global_aes)
{
	static int has_agi = -1; /* do the check only once */

	AES_PARAMS(130,1,5,0,0);

	if (has_agi < 0) {
		has_agi = global_aes[0] >= 0x400 ||
		          mt_appl_find("?AGI\0\0\0\0",global_aes) >= 0;
	}
	if (!has_agi) {
		return 0;
	}

	aes_intin[0] = type;

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (out1)
#endif
	*out1 = aes_intout[1];
#if CHECK_NULLPTR
	if (out2)
#endif
	*out2 = aes_intout[2];
#if CHECK_NULLPTR
	if (out3)
#endif
	*out3 = aes_intout[3];
#if CHECK_NULLPTR
	if (out4)
#endif
	*out4 = aes_intout[4];

	return (aes_intout[0]);
}

short
mt_appl_write(short ap_id, short length, void *ap_pbuff, short *global_aes)
{
	AES_PARAMS(12,2,1,1,0);

	aes_intin[0]  = ap_id;
	aes_intin[1]  = length;
	aes_addrin[0] = (long)ap_pbuff;

	AES_TRAP(aes_params);

	return aes_intout[0];
}

short
mt_appl_find(const char *name, short *global_aes)
{
	AES_PARAMS(13,0,1,1,0);

	aes_addrin[0] = (long)name;

	AES_TRAP(aes_params);

	return aes_intout[0];
}

short
mt_evnt_multi_fast (const EVMULT_IN * em_in, short msg[], EVMULT_OUT * em_out, short *global_aes)
{
	AES_PARAMS(25,16,7,1,0);

	aes_params.intin = (const short*)em_in;				/* input integer array */
	aes_params.intout = (short*)em_out;					/* output integer array */

	aes_addrin[0] = (long)msg;

	AES_TRAP(aes_params);

	return em_out->emo_events;
}

short
mt_form_dial(short mode,
			 short x1, short y1, short w1, short h1,
			 short x2, short y2, short w2, short h2, short *global_aes)
{
	AES_PARAMS(51,9,1,0,0);

	aes_intin[0] = mode;
	aes_intin[1] = x1;
	aes_intin[2] = y1;
	aes_intin[3] = w1;
	aes_intin[4] = h1;
	aes_intin[5] = x2;
	aes_intin[6] = y2;
	aes_intin[7] = w2;
	aes_intin[8] = h2;

	AES_TRAP(aes_params);

	return aes_intout[0];
}

short
mt_graf_handle (short *wcell, short *hcell, short *wbox, short *hbox, short *global_aes)
{
#if !(CHECK_NULLPTR)
	short *ptr;
#endif

	AES_PARAMS(77,0,5,0,0);

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (wcell) *wcell = aes_intout[1];
	if (hcell) *hcell = aes_intout[2];
	if (wbox)  *wbox  = aes_intout[3];
	if (hbox)  *hbox  = aes_intout[4];
#else
	ptr = &aes_intout[1];
	*wcell = *(ptr ++);									/* [1] */
	*hcell = *(ptr ++);									/* [2] */
	*wbox  = *(ptr ++);									/* [3] */
	*hbox  = *(ptr);									/* [4] */
#endif

	return aes_intout[0];
}

short
mt_graf_mkstate (short *mx, short *my, short *mbutton, short *kmeta, short *global_aes)
{
#if !(CHECK_NULLPTR)
	short *ptr;
#endif

	AES_PARAMS(79,0,5,0,0);

	AES_TRAP(aes_params);

#if CHECK_NULLPTR
	if (mx) 	 *mx	  = aes_intout[1];
	if (my) 	 *my	  = aes_intout[2];
	if (mbutton) *mbutton = aes_intout[3];
	if (kmeta)	 *kmeta   = aes_intout[4];
#else
	ptr = &aes_intout[1];
	*mx = *(ptr ++);									/* [1] */
	*my = *(ptr ++);									/* [2] */
	*mbutton = *(ptr ++);							    /* [3] */
	*kmeta = *(ptr);									/* [4] */
#endif

	return aes_intout[0];
}

short
mt_graf_mouse (short shape, const MFORM *shape_addr, short *global_aes)
{
	AES_PARAMS(78,1,1,1,0);

	aes_intin[0] = shape;
	aes_addrin[0] = (long)shape_addr;

	AES_TRAP(aes_params);

	return aes_intout[0];
}

short 
mt_wind_calc_grect(short Type, short Parts, const GRECT *In, GRECT *Out, short *global_aes)
{
	AES_PARAMS(108,6,5,0,0);
                    
	aes_intin[0]		    = Type;
	aes_intin[1]		    = Parts;
	*(GRECT*)(aes_intin +2) = *In;

	AES_TRAP(aes_params);

	*Out = *(GRECT*)(aes_intout +1);
	
	return aes_intout[0];
}

short 
mt_wind_close (short WindowHandle, short *global_aes)
{
	AES_PARAMS(102,1,1,0,0);

	aes_intin[0] = WindowHandle;

	AES_TRAP(aes_params);

	return (aes_intout[0]);
}

short 
mt_wind_create_grect(short Parts, const GRECT *r, short *global_aes)
{
	AES_PARAMS(100,5,1,0,0);
                    
	aes_intin[0] = Parts;
	*(GRECT*)(aes_intin +1) = *r;

	AES_TRAP(aes_params);

	return aes_intout[0];
}

short 
mt_wind_delete (short WindowHandle, short *global_aes)
{
	AES_PARAMS(103,1,1,0,0);

	aes_intin[0] = WindowHandle;

	AES_TRAP(aes_params);

	return (aes_intout[0]);
}

short 
mt_wind_get_grect(short WindowHandle, short What, GRECT *r, short *global_aes)
{
	AES_PARAMS(104,2,5,0,0);
                    
	aes_intin[0] = WindowHandle;
	aes_intin[1] = What;
	
	AES_TRAP(aes_params);

	*r = *(GRECT*)(aes_intout +1);
	
	return (aes_intout[0]);
}

short 
mt_wind_open_grect(short WindowHandle, const GRECT *r, short *global_aes)
{
	AES_PARAMS(101,5,1,0,0);
                    
	aes_intin[0] = WindowHandle;
	*(GRECT*)(aes_intin +1) = *r;
	
	AES_TRAP(aes_params);

	return aes_intout[0];
}

short 
mt_wind_set_grect(short WindowHandle, short What, const GRECT *r, short *global_aes)
{
	AES_PARAMS(105,6,1,0,0);

	aes_intin[0]			= WindowHandle;
	aes_intin[1]			= What;
	*(GRECT*)(aes_intin +2) = *r;
	
	AES_TRAP(aes_params);
	
	return (aes_intout[0]);

}

short 
mt_wind_set_str (short WindowHandle, short What, const char *str, short *global_aes)
{
	AES_PARAMS(105,6,1,0,0);

	aes_intin[0]                  = WindowHandle;
	aes_intin[1]                  = What;
	*(const char**)(aes_intin +2) = str;
	*(const char**)(aes_intin +4) = 0;
	
	AES_TRAP(aes_params);

	return (aes_intout[0]);
}

short 
mt_wind_set_int (short WindowHandle, short What, short W1, short *global_aes)
{
	short *ptr;

	AES_PARAMS(105,6,1,0,0);

	ptr = aes_intin;
	*(ptr ++) = WindowHandle;							/* aes_intin[0] */
	*(ptr ++) = What;									/* aes_intin[1] */
	*(ptr ++) = W1; 								    /* aes_intin[2] */
	*(ptr ++) = 0; 									    /* aes_intin[3] */
	*(ptr ++) = 0; 									    /* aes_intin[4] */
	*(ptr ++) = 0; 									    /* aes_intin[5] */

	AES_TRAP(aes_params);

	return (aes_intout[0]);
}

short 
mt_wind_update (short Code, short *global_aes)
{
	AES_PARAMS(107,1,1,0,0);

	aes_intin[0] = Code;

	AES_TRAP(aes_params);

	return (aes_intout[0]);
}

/*
 * VDI bindings
 */

void
v_bar (short handle, short pxy[])
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, pxy, vdi_dummy, vdi_dummy);
	
	VDI_TRAP_ESC (vdi_params, handle, 11,1, 2,0);
}

void
v_clsvwk (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_dummy, vdi_dummy );
	
	VDI_TRAP_00 (vdi_params, handle, 101);
}

void
v_hide_c (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_dummy, vdi_dummy );
	
	VDI_TRAP_00 (vdi_params, handle, 123);
}

void
v_opnvwk (short work_in[], short *handle, short work_out[])
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, work_in, 0L, &work_out[0], &work_out[45] );
	
	VDI_TRAP (vdi_params, *handle, 100, 0,11);

	*handle = vdi_control[6];
	
	/* some VDI doesn't have the same default parameters.
	   Here is a fix */	
	
	if (vdi_control[6]!=0)
	{ 
		vsf_perimeter(vdi_control[6],PERIMETER_ON);
#if 0
		/* no line/text drawing functions are used in SDL */
		vsl_ends(vdi_control[6],0,0);
		vsl_width(vdi_control[6],1);
		vst_effects(vdi_control[6],0);
		vsm_height(vdi_control[6],9);
#endif
	}
}

void
v_show_c (short handle, short reset)
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, &reset, 0L, vdi_dummy, vdi_dummy );
		
	VDI_TRAP (vdi_params, handle, 122, 0,1);
}

short
vq_color (short handle, short index, short flag, short rgb[])
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[2];   
	short vdi_intout[4]; 

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, vdi_dummy );
	
	vdi_intin[0] = index;
	vdi_intin[1] = flag;
	
	VDI_TRAP (vdi_params, handle, 26, 0,2);
	
	rgb[0] = vdi_intout[1];
	rgb[1] = vdi_intout[2];
	rgb[2] = vdi_intout[3];
	
	return vdi_intout[0];
}

void
vq_extnd (short handle, short flag, short work_out[])
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, &flag, 0L, &work_out[0], &work_out[45] );
	
	VDI_TRAP (vdi_params, handle, 102, 0,1);
}

void
vq_scrninfo (short handle, short *work_out)
{
	short vdi_control[VDI_CNTRLMAX]; 
	static short vdi_intin[1] = {2};   

	VDI_PARAMS(vdi_control, vdi_intin, 0L, work_out, vdi_dummy);
	
	VDI_TRAP_ESC (vdi_params, handle, 102,1, 0,1);
}

void
vro_cpyfm (short handle, short mode, short pxy[], MFDB *src, MFDB *dst)
{
	short vdi_control[VDI_CNTRLMAX]; 
	
	VDI_PARAMS(vdi_control, &mode, pxy, vdi_dummy, vdi_dummy);
	
	vdi_control_ptr(0, MFDB *) = src;
	vdi_control_ptr(1, MFDB *) = dst;
	
	VDI_TRAP (vdi_params, handle, 109, 4,1);
}

void
vs_clip (short handle, short clip_flag, short pxy[])
{
	short vdi_control[VDI_CNTRLMAX]; 
	
	VDI_PARAMS(vdi_control, &clip_flag, pxy, vdi_dummy, vdi_dummy );
	
	VDI_TRAP (vdi_params, handle, 129, 2,1);
}

void
vs_color (short handle, short index, const short rgb[])
{
	short *ptr;
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[4];   
	
	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_dummy, vdi_dummy );
	
	ptr = vdi_intin;
	*(ptr ++) = index;		       /* vdi_intin[0] = index */
	*(ptr ++) = *(rgb ++);	       /* vdi_intin[1] = rgb[0] */
	*(ptr ++) = *(rgb ++);	       /* vdi_intin[2] = rgb[1] */
	*(ptr   ) = *(rgb   );	       /* vdi_intin[3] = rgb[2] */

	VDI_TRAP (vdi_params, handle, 14, 0,4);
}

short
vsf_color (short handle, short index)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[1]; 
	
	VDI_PARAMS(vdi_control, &index, 0L, vdi_intout, vdi_dummy );
	
	VDI_TRAP (vdi_params, handle, 25, 0,1);

	return vdi_intout[0];
}

short
vsf_interior (short handle, short style)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[1]; 
	
	VDI_PARAMS(vdi_control, &style, 0L, vdi_intout, vdi_dummy );
	
	VDI_TRAP (vdi_params, handle, 23, 0,1);

	return vdi_intout[0];
}

short
vsf_perimeter (short handle, short vis)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[1]; 
	
	VDI_PARAMS(vdi_control, &vis, 0L, vdi_intout, vdi_dummy );

	VDI_TRAP (vdi_params, handle, 104, 0,1);

	return vdi_intout[0];
}
 
/*
 * Utilities
 */

#undef min
#undef max
#define max(x,y)   	(((x)>(y))?(x):(y))
#define	min(x,y)   	(((x)<(y))?(x):(y))

short
rc_intersect (const GRECT * r1, GRECT * r2)
{
	short tx, ty, tw, th, ret;

	tx = max (r2->g_x, r1->g_x);
	tw = min (r2->g_x + r2->g_w, r1->g_x + r1->g_w) - tx;
	
	ret = (0 < tw);
	if (ret)
	{
		ty = max (r2->g_y, r1->g_y);
		th = min (r2->g_y + r2->g_h, r1->g_y + r1->g_h) - ty;
		
		ret = (0 < th);
		if (ret)
		{
			r2->g_x = tx;
			r2->g_y = ty;
			r2->g_w = tw;
			r2->g_h = th;
		}
	}
	
	return ret;
}

