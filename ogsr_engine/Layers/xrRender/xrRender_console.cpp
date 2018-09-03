#include	"stdafx.h"

#include	"xrRender_console.h"
#include "..\xrRender_R2\r2_fog_volumes.h"

u32			ps_Preset				=	2	;
xr_token							qpreset_token							[ ]={
	{ "Minimum",					0											},
	{ "Low",						1											},
	{ "Default",					2											},
	{ "High",						3											},
	{ "Extreme",					4											},
	{ 0,							0											}
};
u32			ps_Render_mode = 0;
xr_token							render_mode_token[] = {
	{ "Normal",						0 },
	{ "Thermal",					1 },
	{ 0,							0 }
};

u32			ps_ssao_mode = 0;
xr_token							ssao_mode_token[] = {
	{ "ssdo",						0 },
	{ "hbao",						1 },
	{ 0,							0 }
};
u32			ps_aa_mode = 0;
xr_token							aa_mode_token[] = {
	{ "fxaa",						0 },
	{ "smaa",						1 },
	//	{ "smaa_t2x",					2											},
	{ 0,							0 }
};
u32			ps_sunshafts_mode = 0;
xr_token							sunshafts_mode_token[] = {
	{ "volumetric",					0 },
	{ "screen_space",				1 },
	{ 0,							0 }
};
u32			ps_refl_mode = 0;
xr_token							ext_refl_mode_token[] = {
	{ "water",						0 },
	{ "ground",						1 },
	{ "both",						2 },
	{ 0,							0 }
};

int ps_GlowsPerFrame = 16;
u32			ps_ssao_quality = 0;
u32			ps_sunshafts_quality = 0;
u32			ps_dof_quality = 0;
u32			ps_aa_quality = 0;
u32			ps_soft_shadows = 0;
u32			ps_steep_parallax = 0;
u32			ps_refl_quality = 0;
u32			ps_light_shadow_quality = 0;
u32			SMAA_MAX_SEARCH_STEPS = (1 << (ps_aa_quality + 1));

int			rmap_size = 2048;
Fvector		ps_r2_blur_params = { 0.5,5,30 };
Fvector4	ps_r2_color_grading_params = { 0.0,0.0,0.0,0.0 };
Fvector		ps_r2_details_opt = { 150,200,250 };
float		center_height = 40;
float		ps_r2_sharpening = 0.f;
float		ps_r2_ndr = 0.f;
float		ps_r2_ao_intensity = 1.f;

xr_token							dual_quality_token[] = {
	{ "qt_off",						0 },
	{ "qt_low",						1 },
	{ "qt_high",					2 },
	{ 0,							0 }
};
xr_token							quality_token[] = {
	{ "qt_low",						1 },
	{ "qt_medium",					2 },
	{ "qt_high",					3 },
	{ "qt_extreme",					4 },
	{ 0,							0 }
};
xr_token							ext_quality_token[] = {
	{ "qt_off",						0 },
	{ "qt_low",						1 },
	{ "qt_medium",					2 },
	{ "qt_high",					3 },
	{ "qt_extreme",					4 },
	{ 0,							0 }
};
xr_token							light_shadow_quality_token[] = {
	{ "qt_off",						0 },
	{ "qt_low",						3 },
	{ "qt_medium",					4 },
	{ "qt_high",					5 },
	{ "qt_extreme",					6 },
	{ 0,							0 }
};

// Common
//int		ps_r__Supersample			= 1		;
int			ps_r__LightSleepFrames		= 10	;

float		ps_r__Detail_l_ambient		= 0.9f	;
float		ps_r__Detail_l_aniso		= 0.25f	;
float		ps_r__Detail_density		= 0.3f	;
float		ps_r__Detail_rainbow_hemi	= 0.75f	;

float		ps_r__Tree_w_rot			= 10.0f	;
float		ps_r__Tree_w_speed			= 1.00f	;
float		ps_r__Tree_w_amp			= 0.005f;
Fvector		ps_r__Tree_Wave				= {.1f, .01f, .11f};
float		ps_r__Tree_SBC				= 1.5f	;	// scale bias correct

float		ps_r__WallmarkTTL			= 300.f	;
float		ps_r__WallmarkSHIFT			= 0.0001f;
float		ps_r__WallmarkSHIFT_V		= 0.0001f;

float		ps_r__GLOD_ssa_start		= 256.f	;
float		ps_r__GLOD_ssa_end			=  64.f	;
float		ps_r__LOD					=  1.f	;
//. float		ps_r__LOD_Power				=  1.5f	;
float		ps_r__ssaDISCARD			=  3.5f	;					//RO
float		ps_r__ssaDONTSORT			=  32.f	;					//RO
float		ps_r__ssaHZBvsTEX			=  96.f	;					//RO

int			ps_r__tf_Anisotropic		= 4		;

// R1
float		ps_r1_ssaLOD_A				= 64.f	;
float		ps_r1_ssaLOD_B				= 48.f	;
float		ps_r1_tf_Mipbias			= 0.0f	;
Flags32		ps_r1_flags					= { R1FLAG_DLIGHTS };		// r1-only
float		ps_r1_lmodel_lerp			= 0.1f	;
float		ps_r1_dlights_clip			= 30.f	;
float		ps_r1_pps_u					= 0.f	;
float		ps_r1_pps_v					= 0.f	;

// R2
float		ps_r2_ssaLOD_A				= 48.f	;
float		ps_r2_ssaLOD_B				= 32.f	;
float		ps_r2_tf_Mipbias			= 0.0f	;

// R2-specific
Flags32		ps_r2_ls_flags				= { R2FLAG_SUN | R2FLAG_SUN_IGNORE_PORTALS | R2FLAG_EXP_DONT_TEST_UNSHADOWED | R2FLAG_USE_NVSTENCIL | R2FLAG_EXP_SPLIT_SCENE | R2FLAG_EXP_MT_CALC};	// r2-only
float		ps_r2_df_parallax_h			= 0.02f;
float		ps_r2_df_parallax_range		= 75.f;
float		ps_r2_tonemap_middlegray	= 0.25f;			// r2-only
float		ps_r2_tonemap_adaptation	= 5.f;				// r2-only
float		ps_r2_tonemap_low_lum		= 0.001f;			// r2-only
float		ps_r2_tonemap_amount		= 0.5f;				// r2-only
float		ps_r2_ls_bloom_kernel_g		= 3.3f;				// r2-only
float		ps_r2_ls_bloom_kernel_b		= .7f;				// r2-only
float		ps_r2_ls_bloom_speed		= 10.f;				// r2-only
float		ps_r2_ls_bloom_kernel_scale	= 1.0f;				// r2-only	// gauss
float		ps_r2_ls_dsm_kernel			= .7f;				// r2-only
float		ps_r2_ls_psm_kernel			= .7f;				// r2-only
float		ps_r2_ls_ssm_kernel			= .7f;				// r2-only
float		ps_r2_ls_bloom_threshold	= .3f;				// r2-only
Fvector		ps_r2_aa_barier				= { .8f, .1f, 0};	// r2-only
Fvector		ps_r2_aa_weight				= { .25f,.25f,0};	// r2-only
float		ps_r2_aa_kernel				= .5f;				// r2-only
float		ps_r2_mblur					= .5f;				// .5f
int			ps_r2_GI_depth				= 1;				// 1..5
int			ps_r2_GI_photons			= 16;				// 8..64
float		ps_r2_GI_clip				= EPS_L;			// EPS
float		ps_r2_GI_refl				= .9f;				// .9f
float		ps_r2_ls_depth_scale		= 1.00001f;			// 1.00001f
float		ps_r2_ls_depth_bias			= -0.0001f;			// -0.0001f
float		ps_r2_ls_squality			= 1.0f;				// 1.00f
float		ps_r2_sun_tsm_projection	= 0.18f;			// 0.18f
float		ps_r2_sun_tsm_bias			= -0.05f;			// 
float		ps_r2_sun_near				= 12.f;				// 12.0f
float		ps_r2_sun_near_border		= 0.66f;			// 1.0f
float		ps_r2_sun_limit			= 180.f;
float		ps_r2_sun_depth_far_scale	= 1.00000f;			// 1.00001f
float		ps_r2_sun_depth_far_bias	= 0.00000f;			// -0.0000f
float		ps_r2_sun_depth_near_scale	= 1.00001f;			// 1.00001f
float		ps_r2_sun_depth_near_bias	= -0.00004f;		// -0.00005f
float		ps_r2_sun_lumscale			= 1.0f;				// 1.0f
float		ps_r2_sun_lumscale_hemi		= 1.0f;				// 1.0f
float		ps_r2_sun_lumscale_amb		= 1.0f;
float		ps_r2_gmaterial				= 0.f;				// 
float		ps_r2_zfill					= 0.1f;				// .1f

float		ps_r2_dhemi_scale			= 1.f;				// 1.5f
int			ps_r2_dhemi_count			= 5;				// 5
int			ps_r2_wait_sleep			= 0;

float		ps_r2_lt_smooth				= 1.f;				// 1.f
float		ps_r2_slight_fade			= 1.f;				// 1.f

// KD start
Flags32		ps_common_flags				= { 0 };		// r1-only
u32			dm_size						= 24;
u32 		dm_cache1_line				= 12;	//dm_size*2/dm_cache1_count
u32			dm_cache_line				= 49;	//dm_size+1+dm_size
u32			dm_cache_size				= 2401;	//dm_cache_line*dm_cache_line
float		dm_fade						= 47.5;	//float(2*dm_size)-.5f;
u32			dm_current_size				= 24;
u32 		dm_current_cache1_line		= 12;	//dm_current_size*2/dm_cache1_count
u32			dm_current_cache_line		= 49;	//dm_current_size+1+dm_current_size
u32			dm_current_cache_size		= 2401;	//dm_current_cache_line*dm_current_cache_line
float		dm_current_fade				= 47.5;	//float(2*dm_current_size)-.5f;
float		ps_current_detail_density = 0.6;
u32		ps_r__detail_radius = 49;

// KD end

//- Mad Max
float		ps_r2_gloss_factor			= 1.0f;
//- Mad Max
#include	"..\..\xr_3da\xr_ioconsole.h"
#include	"..\..\xr_3da\xr_ioc_cmd.h"

// KD
class CCC_detail_radius		: public CCC_Integer
{
public:
	void	apply	()	{
		dm_current_size				= iFloor((float)ps_r__detail_radius/4)*2;
		dm_current_cache1_line		= dm_current_size*2/4;		// assuming cache1_count = 4
		dm_current_cache_line		= dm_current_size+1+dm_current_size;
		dm_current_cache_size		= dm_current_cache_line*dm_current_cache_line;
		dm_current_fade				= float(2*dm_current_size)-.5f;
	}
	CCC_detail_radius(LPCSTR N, u32* V, int _min=0, int _max=999) : CCC_Integer(N, (int *)V, _min, _max)		{ };
	virtual void Execute	(LPCSTR args)
	{
		CCC_Integer::Execute	(args);
		apply					();
	}
	virtual void	Status	(TStatus& S)
	{	
		CCC_Integer::Status		(S);
	}
};
// KD

class CCC_detail_density : public CCC_Float
{
public:
	void	apply() {
		ps_r__Detail_density = ps_current_detail_density;
	}
	CCC_detail_density(LPCSTR N, float* V, float _min = 0, float _max = 1.0) : CCC_Float(N, V, _min, _max) { };
	virtual void Execute(LPCSTR args)
	{
		CCC_Float::Execute(args);
		apply();
	}
	virtual void	Status(TStatus& S)
	{
		CCC_Float::Status(S);
	}
};
//-------------------------------------------------------------------------
class CCC_PP : public CCC_Token			// token with mask changing and with vid restart
{
protected:
	Flags32*	flag;
	u32			flag_mask;
	//	char		sh_name[32];
public:
	CCC_PP(LPCSTR N, u32* V, xr_token* T, Flags32* fl, u32 M/*, const char* STR*/) : CCC_Token(N, V, T) { flag = fl; flag_mask = M;/* strcpy(sh_name,STR)*/ };

	virtual void	Execute(LPCSTR args) {
		TStatus S;
		Status(S);
		/*		if (_stricmp(S, args) != 0)
		invalidate_shader(sh_name);*/
		CCC_Token::Execute(args);
		if (_stricmp("qt_off", args) != 0)
			flag->set(flag_mask, true);
		else
			flag->set(flag_mask, false);
		SMAA_MAX_SEARCH_STEPS = (1 << (ps_aa_quality + 1));
		//		Console->Execute		("vid_restart");
	}
};
//-----------------------------------------------------------------------
class CCC_FV : public IConsole_Command
{
public:
	CCC_FV(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		if (ps_r2_ls_flags.test(R2FLAG_FOG_VOLUME) && FV && FV->m_bLoaded) {
			FV->Unload();
			xr_delete(FV);
			FV = xr_new<CLevelFog>();
			FV->Load();
		}
	}
};
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
class CCC_Mask_VR : public CCC_Mask			// mask with vid restart
{
public:
	CCC_Mask_VR(LPCSTR N, Flags32* V, u32 M) : CCC_Mask(N, V, M) {};
	virtual void	Execute(LPCSTR args) {
		CCC_Mask::Execute(args);
		Console->Execute("vid_restart");
	}
};
//-----------------------------------------------------------------------
class CCC_tf_Aniso		: public CCC_Integer
{
public:
	void	apply	()	{
		if (0==HW.pDevice)	return	;
		int	val = *value;	clamp(val,1,16);
		for (u32 i=0; i<HW.Caps.raster.dwStages; i++)
			CHK_DX(HW.pDevice->SetSamplerState( i, D3DSAMP_MAXANISOTROPY, val	));
	}
	CCC_tf_Aniso(LPCSTR N, int*	v) : CCC_Integer(N, v, 1, 16)		{ };
	virtual void Execute	(LPCSTR args)
	{
		CCC_Integer::Execute	(args);
		apply					();
	}
	virtual void	Status	(TStatus& S)
	{	
		CCC_Integer::Status		(S);
		apply					();
	}
};
class CCC_tf_MipBias: public CCC_Float
{
public:
	void	apply	()	{
		if (0==HW.pDevice)	return	;
		for (u32 i=0; i<HW.Caps.raster.dwStages; i++)
			CHK_DX(HW.pDevice->SetSamplerState( i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) value)));
	}

//	CCC_tf_MipBias(LPCSTR N, float*	v) : CCC_Float(N, v, -0.5f, +0.5f)	{ };
	CCC_tf_MipBias(LPCSTR N, float*	v) : CCC_Float(N, v, -3.0f, +3.0f)	{ };		// KD: extended from -0.5;0.5 to -3.0;3.0
	virtual void Execute(LPCSTR args)
	{
		CCC_Float::Execute	(args);
		apply				();
	}
	virtual void	Status	(TStatus& S)
	{	
		CCC_Float::Status	(S);
		apply				();
	}
};
class CCC_R2GM		: public CCC_Float
{
public:
	CCC_R2GM(LPCSTR N, float*	v) : CCC_Float(N, v, 0.f, 4.f) { *v = 0; };
	virtual void	Execute	(LPCSTR args)
	{
		if (0==xr_strcmp(args,"on"))	{
			ps_r2_ls_flags.set	(R2FLAG_GLOBALMATERIAL,TRUE);
		} else if (0==xr_strcmp(args,"off"))	{
			ps_r2_ls_flags.set	(R2FLAG_GLOBALMATERIAL,FALSE);
		} else {
			CCC_Float::Execute	(args);
			if (ps_r2_ls_flags.test(R2FLAG_GLOBALMATERIAL))	{
				static LPCSTR	name[4]	=	{ "oren", "blin", "phong", "metal" };
				float	mid		= *value	;
				int		m0		= iFloor(mid)	% 4;
				int		m1		= (m0+1)		% 4;
				float	frc		= mid - float(iFloor(mid));
				Msg		("* material set to [%s]-[%s], with lerp of [%f]",name[m0],name[m1],frc);
			}
		}
	}
};
class CCC_Screenshot : public IConsole_Command
{
public:
	CCC_Screenshot(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		string_path	name;	name[0]=0;
		sscanf		(args,"%s",	name);
		LPCSTR		image	= xr_strlen(name)?name:0;
		::Render->Screenshot(IRender_interface::SM_NORMAL,image);
	}
};
class CCC_ModelPoolStat : public IConsole_Command
{
public:
	CCC_ModelPoolStat(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		RImplementation.Models->dump();
	}
};
//-----------------------------------------------------------------------
class	CCC_Preset		: public CCC_Token
{
public:
	CCC_Preset(LPCSTR N, u32* V, xr_token* T) : CCC_Token(N,V,T)	{}	;

	virtual void	Execute	(LPCSTR args)	{
		CCC_Token::Execute	(args);
		string_path		_cfg;
		string_path		cmd;
		
		switch	(*value)	{
			case 0:		strcpy(_cfg, "rspec_minimum.ltx");	break;
			case 1:		strcpy(_cfg, "rspec_low.ltx");		break;
			case 2:		strcpy(_cfg, "rspec_default.ltx");	break;
			case 3:		strcpy(_cfg, "rspec_high.ltx");		break;
			case 4:		strcpy(_cfg, "rspec_extreme.ltx");	break;
		}
		FS.update_path			(_cfg,"$game_config$",_cfg);
		strconcat				(sizeof(cmd),cmd,"cfg_load", " ", _cfg);
		Console->Execute		(cmd);
	}
};

#if RENDER==R_R2
#include "r__pixel_calculator.h"
class CCC_BuildSSA : public IConsole_Command
{
public:
	CCC_BuildSSA(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		r_pixel_calculator	c;
		c.run				();
	}
};
#endif
//-----------------------------------------------------------------------

Flags32		ps_r2_dof_flags = { 0 };
Flags32		ps_r2_pp_flags = { 0 };
Flags32		ps_service_flags = { 0 };
Flags32		ps_r2_test_flags = { 0 };
float		ssa_discard = 3.5f;
void		xrRender_initconsole	()
{
	CMD3(CCC_Preset,	"_preset",				&ps_Preset,	qpreset_token	);

// Common
	CMD1(CCC_Screenshot,"screenshot"			);
#ifdef DEBUG
#if RENDER==R_R2
	CMD1(CCC_BuildSSA,	"build_ssa"				);
#endif
	CMD4(CCC_Integer,	"r__lsleep_frames",		&ps_r__LightSleepFrames,	4,		30		);
	CMD4(CCC_Float,		"r__ssa_glod_start",	&ps_r__GLOD_ssa_start,		128,	512		);
	CMD4(CCC_Float,		"r__ssa_glod_end",		&ps_r__GLOD_ssa_end,		16,		96		);
	CMD4(CCC_Float,		"r__wallmark_shift_pp",	&ps_r__WallmarkSHIFT,		0.0f,	1.f		);
	CMD4(CCC_Float,		"r__wallmark_shift_v",	&ps_r__WallmarkSHIFT_V,		0.0f,	1.f		);
//	CMD4(CCC_Float,		"r__wallmark_ttl",		&ps_r__WallmarkTTL,			1.0f,	5.f*60.f);
	CMD1(CCC_ModelPoolStat,"stat_models"		);
#endif // DEBUG
	CMD4(CCC_Float,		"r__wallmark_ttl",		&ps_r__WallmarkTTL,			1.0f,	50.f*60.f);

//	CMD4(CCC_Integer,	"r__supersample",		&ps_r__Supersample,			1,		4		);

	Fvector4	tw_min, tw_max;

	Fvector		bp_min, bp_max;
	Fvector		dopt_min, dopt_max;
	
	CMD4(CCC_Float,		"r__geometry_lod",		&ps_r__LOD,					0.1f,	/*1.2f*/ 5.0f		);	// KD: extended from 1.2 to 3.0
//.	CMD4(CCC_Float,		"r__geometry_lod_pow",	&ps_r__LOD_Power,			0,		2		);

//.	CMD4(CCC_Float,		"r__detail_density",	&ps_r__Detail_density,		.05f,	0.99f	);
	CMD4(CCC_Float,		"r__detail_density",	&ps_current_detail_density/*&ps_r__Detail_density*/,		0.06f/*.2f*/,	0.6f	);	// KD: extended from 0.2 to 0.04 and replaced variable

#ifdef DEBUG
	CMD4(CCC_Float,		"r__detail_l_ambient",	&ps_r__Detail_l_ambient,	.5f,	.95f	);
	CMD4(CCC_Float,		"r__detail_l_aniso",	&ps_r__Detail_l_aniso,		.1f,	.5f		);

	CMD4(CCC_Float,		"r__d_tree_w_amp",		&ps_r__Tree_w_amp,			.001f,	1.f		);
	CMD4(CCC_Float,		"r__d_tree_w_rot",		&ps_r__Tree_w_rot,			.01f,	100.f	);
	CMD4(CCC_Float,		"r__d_tree_w_speed",	&ps_r__Tree_w_speed,		1.0f,	10.f	);

	Fvector _tw_min;  _tw_min.set			(EPS, EPS, EPS);
	Fvector _tw_max;  _tw_max.set			(2,2,2);
	CMD4(CCC_Vector3,	"r__d_tree_wave",		&ps_r__Tree_Wave,			_tw_min, _tw_max	);
#endif // DEBUG

	CMD2(CCC_tf_Aniso,	"r__tf_aniso",			&ps_r__tf_Anisotropic		); //	{1..16}

	// R1
/*	CMD4(CCC_Float,		"r1_ssa_lod_a",			&ps_r1_ssaLOD_A,			16,		96		);
	CMD4(CCC_Float,		"r1_ssa_lod_b",			&ps_r1_ssaLOD_B,			16,		64		);
	CMD4(CCC_Float,		"r1_lmodel_lerp",		&ps_r1_lmodel_lerp,			0,		0.333f	);
	CMD2(CCC_tf_MipBias,"r1_tf_mipbias",		&ps_r1_tf_Mipbias			);//	{-3 +3}
	CMD3(CCC_Mask,		"r1_dlights",			&ps_r1_flags,				R1FLAG_DLIGHTS	);
	CMD4(CCC_Float,		"r1_dlights_clip",		&ps_r1_dlights_clip,		10.f,	150.f	);
	CMD4(CCC_Float,		"r1_pps_u",				&ps_r1_pps_u,				-1.f,	+1.f	);
	CMD4(CCC_Float,		"r1_pps_v",				&ps_r1_pps_v,				-1.f,	+1.f	);
	CMD4(CCC_Float,		"r1_dlights_clip",		&ps_r1_dlights_clip,		10.f,	150.f	);
*/

	// R2
	CMD4(CCC_Float,		"r2_ssa_lod_a",			&ps_r2_ssaLOD_A,			16,		96		);
	CMD4(CCC_Float,		"r2_ssa_lod_b",			&ps_r2_ssaLOD_B,			32,		64		);
	CMD2(CCC_tf_MipBias,"r2_tf_mipbias",		&ps_r2_tf_Mipbias			);

	// R2-specific
//	CMD2(CCC_R2GM,		"r2em",					&ps_r2_gmaterial							);
	CMD3(CCC_Mask,		"r2_tonemap",			&ps_r2_ls_flags,			R2FLAG_TONEMAP	);
	CMD4(CCC_Float,		"r2_tonemap_middlegray",&ps_r2_tonemap_middlegray,	0.0f,	2.0f	);
	CMD4(CCC_Float,		"r2_tonemap_adaptation",&ps_r2_tonemap_adaptation,	0.01f,	10.0f	);
	CMD4(CCC_Float,		"r2_tonemap_lowlum",	&ps_r2_tonemap_low_lum,		0.0001f,1.0f	);
	CMD4(CCC_Float,		"r2_tonemap_amount",	&ps_r2_tonemap_amount,		0.0000f,1.0f	);
	CMD4(CCC_Float,		"r2_ls_bloom_kernel_scale",&ps_r2_ls_bloom_kernel_scale,	0.5f,	2.f);
	CMD4(CCC_Float,		"r2_ls_bloom_kernel_g",	&ps_r2_ls_bloom_kernel_g,	1.f,	7.f		);
	CMD4(CCC_Float,		"r2_ls_bloom_kernel_b",	&ps_r2_ls_bloom_kernel_b,	0.01f,	1.f		);
	CMD4(CCC_Float,		"r2_ls_bloom_threshold",&ps_r2_ls_bloom_threshold,	0.f,	1.f		);
	CMD4(CCC_Float,		"r2_ls_bloom_speed",	&ps_r2_ls_bloom_speed,		0.f,	100.f	);
	CMD3(CCC_Mask,		"r2_ls_bloom_fast",		&ps_r2_ls_flags,			R2FLAG_FASTBLOOM);
	CMD4(CCC_Float,		"r2_ls_dsm_kernel",		&ps_r2_ls_dsm_kernel,		.1f,	3.f		);
	CMD4(CCC_Float,		"r2_ls_psm_kernel",		&ps_r2_ls_psm_kernel,		.1f,	3.f		);
	CMD4(CCC_Float,		"r2_ls_ssm_kernel",		&ps_r2_ls_ssm_kernel,		.1f,	3.f		);
	CMD4(CCC_Float,		"r2_ls_squality",		&ps_r2_ls_squality,			.5f,	1.f		);

	CMD3(CCC_Mask,		"r2_zfill",				&ps_r2_ls_flags,			R2FLAG_ZFILL	);
	CMD4(CCC_Float,		"r2_zfill_depth",		&ps_r2_zfill,				.001f,	.5f		);
	CMD3(CCC_Mask,		"r2_allow_r1_lights",	&ps_r2_ls_flags,			R2FLAG_R1LIGHTS	);

	//- Mad Max
	CMD4(CCC_Float,		"r2_gloss_factor",		&ps_r2_gloss_factor,		.0f,	10.f	);
	//- Mad Max

#ifdef DEBUG
	CMD3(CCC_Mask,		"r2_use_nvdbt",			&ps_r2_ls_flags,			R2FLAG_USE_NVDBT);
	CMD3(CCC_Mask,		"r2_mt",				&ps_r2_ls_flags,			R2FLAG_EXP_MT_CALC);
#endif // DEBUG

	CMD3(CCC_Mask,		"r2_sun",				&ps_r2_ls_flags,			R2FLAG_SUN		);
	CMD3(CCC_Mask,		"r2_sun_details",		&ps_r2_ls_flags,			R2FLAG_SUN_DETAILS);
	CMD3(CCC_Mask,		"r2_sun_focus",			&ps_r2_ls_flags,			R2FLAG_SUN_FOCUS);
//	CMD3(CCC_Mask,		"r2_sun_static",		&ps_r2_ls_flags,			R2FLAG_SUN_STATIC);
//	CMD3(CCC_Mask,		"r2_exp_splitscene",	&ps_r2_ls_flags,			R2FLAG_EXP_SPLIT_SCENE);
//	CMD3(CCC_Mask,		"r2_exp_donttest_uns",	&ps_r2_ls_flags,			R2FLAG_EXP_DONT_TEST_UNSHADOWED);
	
	CMD3(CCC_Mask,		"r_glows_use",			&ps_r2_pp_flags,			R_FLAG_GLOW_USE);
	CMD4(CCC_Integer,	"r__glows_per_frame",	&ps_GlowsPerFrame,	2, 32);

	CMD3(CCC_Mask,		"r2_sun_tsm",			&ps_r2_ls_flags,			R2FLAG_SUN_TSM	);
	CMD4(CCC_Float,		"r2_sun_tsm_proj",		&ps_r2_sun_tsm_projection,	.001f,	0.8f	);
	CMD4(CCC_Float,		"r2_sun_tsm_bias",		&ps_r2_sun_tsm_bias,		-0.5,	+0.5	);
	CMD4(CCC_Float,		"r2_sun_near",			&ps_r2_sun_near,			1.f,	/*50.f*/150.f	);	// KD: extended from 50 to 150
	CMD4(CCC_Float,		"r2_sun_near_border",	&ps_r2_sun_near_border,		.5f,	1.0f	);
	CMD4(CCC_Float,		"r2_sun_limit",			&ps_r2_sun_limit, 1.f, 180.f );
	CMD4(CCC_Float,		"r2_sun_depth_far_scale",&ps_r2_sun_depth_far_scale,0.5,	1.5		);
	CMD4(CCC_Float,		"r2_sun_depth_far_bias",&ps_r2_sun_depth_far_bias,	-0.5,	+0.5	);
	CMD4(CCC_Float,		"r2_sun_depth_near_scale",&ps_r2_sun_depth_near_scale,0.5,	1.5		);
	CMD4(CCC_Float,		"r2_sun_depth_near_bias",&ps_r2_sun_depth_near_bias,-0.5,	+0.5	);
	CMD4(CCC_Float,		"r2_sun_lumscale",		&ps_r2_sun_lumscale,		-1.0,	+3.0	);
	CMD4(CCC_Float,		"r2_sun_lumscale_hemi",	&ps_r2_sun_lumscale_hemi,	0.0,	+3.0	);
	CMD4(CCC_Float,		"r2_sun_lumscale_amb",	&ps_r2_sun_lumscale_amb,	0.0,	+3.0	);

/*	CMD3(CCC_Mask,		"r2_aa",				&ps_r2_ls_flags,			R2FLAG_AA);
	CMD4(CCC_Float,		"r2_aa_kernel",			&ps_r2_aa_kernel,			0.3f,	0.7f	);
	CMD4(CCC_Float,		"r2_mblur",				&ps_r2_mblur,				0.0f,	1.0f	);*/

	CMD4(CCC_Float,		"r2_mblur_amount",		&ps_r2_mblur, 0.0f, 1.0f);

	CMD3(CCC_Mask,		"r2_gi",				&ps_r2_ls_flags,			R2FLAG_GI);
	CMD4(CCC_Float,		"r2_gi_clip",			&ps_r2_GI_clip,				EPS,	0.1f	);
	CMD4(CCC_Integer,	"r2_gi_depth",			&ps_r2_GI_depth,			1,		5		);
	CMD4(CCC_Integer,	"r2_gi_photons",		&ps_r2_GI_photons,			8,		256		);
	CMD4(CCC_Float,		"r2_gi_refl",			&ps_r2_GI_refl,				EPS_L,	0.99f	);

	CMD4(CCC_Integer,	"r2_wait_sleep",		&ps_r2_wait_sleep,			0,		1		);

#ifdef DEBUG
	CMD4(CCC_Integer,	"r2_dhemi_count",		&ps_r2_dhemi_count,			4,		25		);
	CMD4(CCC_Float,		"r2_dhemi_scale",		&ps_r2_dhemi_scale,			.5f,	3.f		);
	CMD4(CCC_Float,		"r2_dhemi_smooth",		&ps_r2_lt_smooth,			0.f,	10.f	);
#endif // DEBUG

	CMD4(CCC_Float,		"r2_ls_depth_scale",	&ps_r2_ls_depth_scale,		0.5,	1.5		);
	CMD4(CCC_Float,		"r2_ls_depth_bias",		&ps_r2_ls_depth_bias,		-0.5,	+0.5	);

	CMD4(CCC_Float,		"r2_parallax_h",		&ps_r2_df_parallax_h,		.0f,	.5f		);
//	CMD4(CCC_Float,		"r2_parallax_range",	&ps_r2_df_parallax_range,	5.0f,	175.0f	);

	CMD4(CCC_Float,		"r2_slight_fade",		&ps_r2_slight_fade,			.02f,	2.f		);

/*	tw_min.set			(0,0,0);	tw_max.set	(1,1,1);
	CMD4(CCC_Vector3,	"r2_aa_break",			&ps_r2_aa_barier,			tw_min, tw_max	);

	tw_min.set			(0,0,0);	tw_max.set	(1,1,1);
	CMD4(CCC_Vector3,	"r2_aa_weight",			&ps_r2_aa_weight,			tw_min, tw_max	);*/

	// KD
	// render modes (current: 0 - Normal rendering, 1 - Thermal vision)
	CMD3(CCC_Token, "r2_render_mode", &ps_Render_mode, render_mode_token);
	// postprocess effects
	CMD3(CCC_Mask, "r2_mblur", &ps_r2_pp_flags, R2PP_FLAG_MBLUR);
	CMD3(CCC_Mask, "r2_rain_drops", &ps_r2_pp_flags, R2PP_FLAG_RAIN_DROPS);
	CMD3(CCC_Mask, "r2_rain_drops_control", &ps_r2_pp_flags, R2PP_FLAG_RAIN_DROPS_CONTROL);
	CMD3(CCC_Mask, "r2_lens_dirt_control", &ps_r2_pp_flags, R2PP_FLAG_LENS_DIRT_CONTROL);
	CMD3(CCC_Mask, "r__bloodmarks", &ps_r2_ls_flags, R2FLAG_BLOODMARKS);

	CMD3(CCC_Token, "r2_ssao_mode", &ps_ssao_mode, ssao_mode_token);
	CMD3(CCC_Token, "r2_aa_mode", &ps_aa_mode, aa_mode_token);
	CMD3(CCC_Token, "r2_sunshafts_mode", &ps_sunshafts_mode, sunshafts_mode_token);

	CMD5(CCC_PP, "r2_aa", &ps_aa_quality, ext_quality_token, &ps_r2_pp_flags, R2PP_FLAG_AA);
	CMD5(CCC_PP, "r2_dof_quality", &ps_dof_quality, ext_quality_token, &ps_r2_pp_flags, R2PP_FLAG_DOF);
	CMD5(CCC_PP, "r2_sunshafts", &ps_sunshafts_quality, ext_quality_token, &ps_r2_pp_flags, R2PP_FLAG_SUNSHAFTS);
	CMD5(CCC_PP, "r2_ssao", &ps_ssao_quality, ext_quality_token, &ps_r2_pp_flags, R2PP_FLAG_SSAO);
	CMD5(CCC_PP, "r2_reflections", &ps_refl_quality, dual_quality_token, &ps_r2_pp_flags, R2PP_FLAG_REFLECTIONS);
	CMD3(CCC_Token, "r2_reflections_mode", &ps_refl_mode, ext_refl_mode_token);

	CMD3(CCC_Token, "r2_soft_shadows", &ps_soft_shadows, ext_quality_token);
	CMD3(CCC_Token, "r2_steep_parallax", &ps_steep_parallax, ext_quality_token);

	CMD3(CCC_Mask, "r2_dof", &ps_r2_dof_flags, R2FLAG_DOF_COMMON);
	CMD3(CCC_Mask, "r2_dof_zoom", &ps_r2_dof_flags, R2FLAG_DOF_ZOOM);
	CMD3(CCC_Mask, "r2_dof_reload", &ps_r2_dof_flags, R2FLAG_DOF_RELOAD);

	CMD3(CCC_Mask, "r2_dof_control", &ps_r2_pp_flags, R2PP_FLAG_DOF_CONTROL);

	CMD3(CCC_Mask, "r2_soft_water", &ps_r2_ls_flags, R2FLAG_SOFT_WATER);
	CMD3(CCC_Mask, "r2_soft_particles", &ps_r2_ls_flags, R2FLAG_SOFT_PARTICLES);
	//	CMD3(CCC_Mask,		"r2_hwi",				&ps_r2_test_flags,			R2FLAG_HW_INSTANSING);
	CMD3(CCC_Mask, "r2_volumetric_fog", &ps_r2_ls_flags, R2FLAG_FOG_VOLUME);
	//	CMD3(CCC_Mask,		"r2_reproject",			&ps_r2_test_flags,			R2FLAG_REPROJECT);

	CMD4(CCC_detail_radius, "r__detail_radius", &ps_r__detail_radius, 49, 250);
	CMD3(CCC_Mask, "r2_detail_bump", &ps_r2_ls_flags, R2FLAG_DETAIL_BUMP_DEBUG);
	tw_min.set(0, 0, 0, 0);	tw_max.set(1.0, 1.0, 1.0, 1.0);
	CMD4(CCC_Vector4, "r2_color_grading", &ps_r2_color_grading_params, tw_min, tw_max);
	bp_min.set(0, 0, 0);	bp_max.set(1.0, 50, 100);
	CMD4(CCC_Vector3, "r2_blur_params", &ps_r2_blur_params, bp_min, bp_max);
	//	CMD3(CCC_Mask,		"r2_supersampling",		&ps_r2_pp_flags,			R2PP_FLAG_SUPERSAMPLING_AA);
	CMD1(CCC_FV, "reset_fv");

	//rain map
	/*	CMD4(CCC_Float,		"r2_rmap_tsm_proj",			&ps_r2_rmap_tsm_projection,		.001f,	0.8f	);
	CMD4(CCC_Float,		"r2_rmap_tsm_bias",			&ps_r2_rmap_tsm_bias,			-0.5,	+0.5	);
	CMD4(CCC_Float,		"r2_rmap_near",				&ps_r2_rmap_near,				1.f,	150.f	);
	CMD4(CCC_Float,		"r2_rmap_near_border",		&ps_r2_rmap_near_border,		.5f,	1.0f	);
	CMD4(CCC_Float,		"r2_rmap_depth_far_scale",	&ps_r2_rmap_depth_far_scale,	0.5,	1.5		);
	CMD4(CCC_Float,		"r2_rmap_depth_far_bias",	&ps_r2_rmap_depth_far_bias,		-0.5,	+0.5	);
	CMD4(CCC_Float,		"r2_rmap_depth_near_scale",	&ps_r2_rmap_depth_near_scale,	0.5,	1.5		);
	CMD4(CCC_Float,		"r2_rmap_depth_near_bias",	&ps_r2_rmap_depth_near_bias,	-0.5,	+0.5	);*/

	//	CMD3(CCC_Mask,		"r2_gbuffer_opt",			&ps_r2_ls_flags,		R2FLAG_GBUFFER_OPT);
	//	CMD3(CCC_Mask,		"r2_nv_dbt",				&ps_r2_ls_flags,		R2FLAG_USE_NVDBT);
	CMD3(CCC_Mask, "r2_lens_flare", &ps_r2_ls_flags, R2FLAG_LENS_FLARES);
	CMD3(CCC_Mask, "r2_lens_dirt", &ps_r2_ls_flags, R2FLAG_LENS_DIRT);
	//	CMD3(CCC_Mask,		"r2_rain_map",				&ps_r2_test_flags,		R2FLAG_RAIN_MAP);

	CMD4(CCC_Float, "r2_image_sharpening", &ps_r2_sharpening, 0.0, 1.0);
	CMD3(CCC_Mask, "r2_wet_surfaces", &ps_r2_ls_flags, R2FLAG_WET_SURFACES);
	CMD4(CCC_Float,	"r2_no_details_radius",		&ps_r2_ndr,					0.f, 5.f);
	CMD3(CCC_Mask, "r2_puddles", &ps_r2_ls_flags, R2FLAG_PUDDLES);
	CMD3(CCC_Mask, "r2_true_sun_shadows", &ps_r2_ls_flags, R2FLAG_TRUE_SHADOWS);

	CMD3(CCC_Mask, "rs_detail", &psDeviceFlags, rsDetails);

	CMD3(CCC_Mask, "r2_volumetric_lights", &ps_r2_ls_flags, R2FLAG_VOLLIGHT);

	CMD5(CCC_PP, "r2_lights_shadow_fade", &ps_light_shadow_quality, light_shadow_quality_token, &ps_r2_pp_flags, R2PP_FLAG_LIGHT_SHADOW_FADE);


	dopt_min.set(1, 1, 1);	dopt_max.set(390, 390, 390);
	CMD4(CCC_Vector3, "r2_details_opt", &ps_r2_details_opt, dopt_min, dopt_max);
	CMD4(CCC_Float, "r2_ao_intensity", &ps_r2_ao_intensity, 0.2, 2.0);
	CMD3(CCC_Mask, "r2_ao", &ps_r2_ls_flags, R2FLAG_AO);
	CMD3(CCC_Mask, "r__actor_shadow", &ps_r2_ls_flags, R2FLAG_ACTOR_SHADOW);

/*	CMD1(CCC_RWS, "r2_reset_weapon_states");
	CMD1(CCC_QS, "ogse_quick_save");*/

	//	CMD4(CCC_SSADiscard,		"r2_ssa_discard",	&ssa_discard,				3.f,	20.f		);
	//	CMD4(CCC_Float,		"r2_ssa_lod_a",			&ps_r2_ssaLOD_A,			16,		192		);
	//	CMD4(CCC_Float,		"r2_ssa_lod_b",			&ps_r2_ssaLOD_B,			16,		128		);

	//	CMD4(CCC_Integer,	"r2_lod_limit",			&is_lods_limited,			0,		1		);
	//	CMD1(CCC_Textures, "dump_resources");

	//	CMD3(CCC_Mask_VR,	"r2_pcss",					&ps_r2_test_flags,			R2FLAG_PCSS		);

	//	CMD3(CCC_Mask,		"r2_exp_donttest_uns",	&ps_r2_ls_flags,			R2FLAG_EXP_DONT_TEST_UNSHADOWED);

	//	CMD3(CCC_Mask,		"extended_graph_stats",		&ps_service_flags,			S_EXTENDED_GRAPH_STATS);
}
