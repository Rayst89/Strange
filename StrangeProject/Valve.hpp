#pragma once

#include "Core.hpp"
#include "Framework.hpp"

#include "dt_common.hpp"
#include "dt_recv.hpp"

#include "checksum_md5.hpp"

#include <d3d9.h>


//
// defines
//
#define	CONTENTS_EMPTY			0		// No contents

#define	CONTENTS_SOLID			0x1		// an eye is never valid in a solid
#define	CONTENTS_WINDOW			0x2		// translucent, but not watery (glass)
#define	CONTENTS_AUX			0x4
#define	CONTENTS_GRATE			0x8		// alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't
#define	CONTENTS_SLIME			0x10
#define	CONTENTS_WATER			0x20
#define	CONTENTS_MIST			0x40
#define CONTENTS_OPAQUE			0x80	// things that cannot be seen through (may be non-solid though)
#define	LAST_VISIBLE_CONTENTS	0x80

#define ALL_VISIBLE_CONTENTS (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define CONTENTS_TESTFOGVOLUME	0x100

// unused 
// NOTE: If it's visible, grab from the top + update LAST_VISIBLE_CONTENTS
// if not visible, then grab from the bottom.
#define CONTENTS_UNUSED3		0x200
#define CONTENTS_UNUSED4		0x400
#define CONTENTS_UNUSED5		0x800
#define CONTENTS_UNUSED6		0x1000
#define CONTENTS_UNUSED7		0x2000


// hits entities which are MOVETYPE_PUSH (doors, plats, etc.)
#define CONTENTS_MOVEABLE		0x4000

// remaining contents are non-visible, and don't eat brushes
#define	CONTENTS_AREAPORTAL		0x8000

#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000

// currents can be added to any other contents, and may be mixed
#define	CONTENTS_CURRENT_0		0x40000
#define	CONTENTS_CURRENT_90		0x80000
#define	CONTENTS_CURRENT_180	0x100000
#define	CONTENTS_CURRENT_270	0x200000
#define	CONTENTS_CURRENT_UP		0x400000
#define	CONTENTS_CURRENT_DOWN	0x800000

#define	CONTENTS_ORIGIN			0x1000000	// removed before bsping an entity

#define	CONTENTS_MONSTER		0x2000000	// should never be on a brush, only in game
#define	CONTENTS_DEBRIS			0x4000000
#define	CONTENTS_DETAIL			0x8000000	// brushes to be added after vis leafs
#define	CONTENTS_TRANSLUCENT	0x10000000	// auto set if any surface has trans
#define	CONTENTS_LADDER			0x20000000
#define CONTENTS_HITBOX			0x40000000	// use accurate hitboxes on trace

#define	MASK_ALL					(0xFFFFFFFF)
// everything that is normally solid
#define	MASK_SOLID					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// everything that blocks player movement
#define	MASK_PLAYERSOLID			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// blocks npc movement
#define	MASK_NPCSOLID				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// water physics in these contents
#define	MASK_WATER					(CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
// everything that blocks line of sight
#define	MASK_OPAQUE					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
// everything that blocks line of sight, but with monsters added.
#define MASK_OPAQUE_AND_NPCS		(MASK_OPAQUE|CONTENTS_MONSTER)
// bullets see these as solid
#define	MASK_SHOT					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
// non-raycasted weapons see this as solid (includes grates)
#define MASK_SHOT_HULL				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
// everything normally solid, except monsters (world+brush only)
#define MASK_SOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
// everything normally solid for player movement, except monsters (world+brush only)
#define MASK_PLAYERSOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
// everything normally solid for npc movement, except monsters (world+brush only)
#define MASK_NPCSOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// just the world, used for route rebuilding
#define MASK_NPCWORLDSTATIC			(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// These are things that can split areaportals
#define MASK_SPLITAREAPORTAL		(CONTENTS_WATER|CONTENTS_SLIME)

// UNDONE: This is untested, any moving water
#define MASK_CURRENT				(CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)

// everything that blocks corpse movement
// UNDONE: Not used yet / may be deleted
#define	MASK_DEADSOLID				(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)


#ifdef GetProp
#undef GetProp
#endif
#ifdef DrawText
#undef DrawText
#endif

enum
{
	// Source Engine Indexes
	IBaseClientDLL_GetAllClasses = 5,
	IBaseClientDLL_CreateMove = 18,
	IBaseClientDLL_FrameStageNotify = 32,

	IClientEntityList_GetBaseEntity = 3,
	IClientEntityList_GetBaseEntityFromHandle = 4,
	IClientEntityList_GetHighestEntityIndex = 6,

	IGameMovement_ProcessMovement = 1,

	IPrediction_RunCommand = 19,
	IPrediction_SetupMove = 20,
	IPrediction_FinishMove = 21,
	IPrediction_Update = 23,

	IInput_GetUserCmd = 8,
	IInput_ResetMouse = 23,

	IVEngineClient_GetScreenSize = 5,
	IVEngineClient_GetPlayerInfo = 8,
	IVEngineClient_GetLocalPlayer = 12,
	IVEngineClient_GetViewAngles = 19,
	IVEngineClient_SetViewAngles = 20,
	IVEngineClient_GetMaxClients = 21,
	IVEngineClient_WorldToScreenMatrix = 38,

	IVModelInfoClient_GetModelName = 3,
	IVModelInfoClient_GetStudioModel = 27,

	IVRenderView_SetBlend = 4,
	IVRenderView_SetColorModulation = 6,

	IVModelRender_ForcedMaterialOverride = 1,
	IVModelRender_DrawModelEx = 19,

	IEngineTrace_GetPointContents = 0,
	IEngineTrace_TraceRay = 4,

	ICvar_FindVar = 7,

	IPhysicsSurfaceProps_GetSurfaceData = 5,

	IMaterialSystem_CreateMaterial = 123,

	// Direct3D indexes
	IDirect3DDevice9_Reset = 16,
	IDirect3DDevice9_Present = 17,
};

enum
{
	MULTIPLAYER_BACKUP = 90,
};

enum
{
	FL_ONGROUND = ( 1 << 0 ),
	FL_DUCKING = ( 1 << 1 ),
};

enum
{
	IN_ATTACK = ( 1 << 0 ),
	IN_JUMP = ( 1 << 1 ),
	IN_DUCK = ( 1 << 2 ),
	IN_FORWARD = ( 1 << 3 ),
	IN_BACK = ( 1 << 4 ),
	IN_USE = ( 1 << 5 ),
	IN_CANCEL = ( 1 << 6 ),
	IN_LEFT = ( 1 << 7 ),
	IN_RIGHT = ( 1 << 8 ),
	IN_MOVELEFT = ( 1 << 9 ),
	IN_MOVERIGHT = ( 1 << 10 ),
	IN_ATTACK2 = ( 1 << 11 ),
	IN_RUN = ( 1 << 12 ),
	IN_RELOAD = ( 1 << 13 ),
	IN_ALT1 = ( 1 << 14 ),
	IN_ALT2 = ( 1 << 15 ),
	IN_SCORE = ( 1 << 16 ),
	IN_SPEED = ( 1 << 17 ),
	IN_WALK = ( 1 << 18 ),
	IN_ZOOM = ( 1 << 19 ),
	IN_WEAPON1 = ( 1 << 20 ),
	IN_WEAPON2 = ( 1 << 21 ),
	IN_BULLRUSH = ( 1 << 22 ),
	IN_GRENADE1 = ( 1 << 23 ),
	IN_GRENADE2 = ( 1 << 24 ),
};

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END,
};

enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
};

enum CSWeaponType
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN,
};

enum CSWeaponID
{
	WEAPON_NONE = 0,
	WEAPON_P228,
	WEAPON_GLOCK,
	WEAPON_SCOUT,
	WEAPON_HEGRENADE,
	WEAPON_XM1014,
	WEAPON_C4,
	WEAPON_MAC10,
	WEAPON_AUG,
	WEAPON_SMOKEGRENADE,
	WEAPON_ELITE,
	WEAPON_FIVESEVEN,
	WEAPON_UMP45,
	WEAPON_SG550,
	WEAPON_GALIL,
	WEAPON_FAMAS,
	WEAPON_USP,
	WEAPON_AWP,
	WEAPON_MP5NAVY,
	WEAPON_M249,
	WEAPON_M3,
	WEAPON_M4A1,
	WEAPON_TMP,
	WEAPON_G3SG1,
	WEAPON_FLASHBANG,
	WEAPON_DEAGLE,
	WEAPON_SG552,
	WEAPON_AK47,
	WEAPON_KNIFE,
	WEAPON_P90,
	WEAPON_SHIELD,
	WEAPON_KEVLAR,
	WEAPON_ASSAULTSUIT,
	WEAPON_NIGHTVISION,

	WEAPON_MAX,
};

enum MaterialType_t
{
	CHAR_TEX_ANTLION = 'A',
	CHAR_TEX_BLOODYFLESH = 'B',
	CHAR_TEX_CONCRETE = 'C',
	CHAR_TEX_DIRT = 'D',
	CHAR_TEX_EGGSHELL = 'E',
	CHAR_TEX_FLESH = 'F',
	CHAR_TEX_GRATE = 'G',
	CHAR_TEX_ALIENFLESH	= 'H',
	CHAR_TEX_CLIP = 'I',
	CHAR_TEX_PLASTIC = 'L',
	CHAR_TEX_METAL = 'M',
	CHAR_TEX_SAND = 'N',
	CHAR_TEX_FOLIAGE = 'O',
	CHAR_TEX_COMPUTER = 'P',
	CHAR_TEX_SLOSH = 'S',
	CHAR_TEX_TILE = 'T',
	CHAR_TEX_VENT = 'V',
	CHAR_TEX_WOOD = 'W',
	CHAR_TEX_GLASS = 'Y',
	CHAR_TEX_WARPSHIELD	= 'Z'
};

enum MaterialVarFlags_t
{
	MATERIAL_VAR_DEBUG = ( 1 << 0 ),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = ( 1 << 1 ),
	MATERIAL_VAR_NO_DRAW = ( 1 << 2 ),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = ( 1 << 3 ),

	MATERIAL_VAR_VERTEXCOLOR = ( 1 << 4 ),
	MATERIAL_VAR_VERTEXALPHA = ( 1 << 5 ),
	MATERIAL_VAR_SELFILLUM = ( 1 << 6 ),
	MATERIAL_VAR_ADDITIVE = ( 1 << 7 ),
	MATERIAL_VAR_ALPHATEST = ( 1 << 8 ),
	MATERIAL_VAR_MULTIPASS = ( 1 << 9 ),
	MATERIAL_VAR_ZNEARER = ( 1 << 10 ),
	MATERIAL_VAR_MODEL = ( 1 << 11 ),
	MATERIAL_VAR_FLAT = ( 1 << 12 ),
	MATERIAL_VAR_NOCULL = ( 1 << 13 ),
	MATERIAL_VAR_NOFOG = ( 1 << 14 ),
	MATERIAL_VAR_IGNOREZ = ( 1 << 15 ),
	MATERIAL_VAR_DECAL = ( 1 << 16 ),
	MATERIAL_VAR_ENVMAPSPHERE = ( 1 << 17 ),
	MATERIAL_VAR_NOALPHAMOD = ( 1 << 18 ),
	MATERIAL_VAR_ENVMAPCAMERASPACE = ( 1 << 19 ),
	MATERIAL_VAR_BASEALPHAENVMAPMASK = ( 1 << 20 ),
	MATERIAL_VAR_TRANSLUCENT = ( 1 << 21 ),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = ( 1 << 22 ),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = ( 1 << 23 ),
	MATERIAL_VAR_OPAQUETEXTURE = ( 1 << 24 ),
	MATERIAL_VAR_ENVMAPMODE = ( 1 << 25 ),
	MATERIAL_VAR_SUPPRESS_DECALS = ( 1 << 26 ),
	MATERIAL_VAR_HALFLAMBERT = ( 1 << 27 ),
	MATERIAL_VAR_WIREFRAME = ( 1 << 28 ),
};

enum
{
	LIFE_ALIVE = 0,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY
};

enum MapLoadType_t
{
	MapLoad_NewGame = 0,
	MapLoad_LoadGame,
	MapLoad_Transition,
	MapLoad_Background,
};

typedef enum
{
	PREDICTION_SIMULATION_RESULTS_ARRIVING_ON_SEND_FRAME = 0,
	PREDICTION_NORMAL,
} PREDICTION_REASON;

enum MoveType_t
{
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4
};

//
// structs
//
struct model_t;

struct player_info_t;
struct mstudiobone_t;
struct mstudiobbox_t;
struct mstudiohitboxset_t;
struct studiohdr_t;
struct cplane_t;
struct csurface_t;
struct Ray_t;
struct surfacephysicsparams_t;
struct surfaceaudioparams_t;
struct surfacesoundnames_t;
struct surfacegameprops_t;
struct surfacedata_t;
struct string_t;
struct ModelRenderInfo_t;

//
// classes
//
class IConVar;
class IMoveHelper;
class IMaterial;

class IHandleEntity;
class IClientNetworkable;
class IClientRenderable;

class C_BaseEntity;
class C_CSPlayer;
class C_WeaponCSBaseGun;
class CCSWeaponInfo;

class ConVar;
class ClientClass;
class CGlobalVars;
class CUserCmd;
class CVerifiedUserCmd;
class CBaseTrace;
class CGameTrace;
class CMoveData;

class IBaseClientDLL;
class IClientEntityList;
class IGameMovement;
class IPrediction;
class IInput;
class IVEngineClient;
class IVModelInfoClient;
class IVRenderView;
class IVModelRender;
class IEngineTrace;
class ICvar;
class CPanel;
class IPhysicsSurfaceProps;
class IClientMode;
class IMaterialSystem;
class IGameEvent;
class IGameEventListener2;
class IGameEventManager2;

class KeyValues;
class D3DDeviceWrapper;

class ITraceFilter;
class CTraceFilter;
class CTraceFilterSimple;
class CTraceFilterSkipTwoEntities;

//
// typedefs
//
typedef void* ( *CreateInterfaceFn )( const char*, int* );
typedef void ( *FnChangeCallback_t )( IConVar*, const char*, float );

typedef unsigned long CBaseHandle;
typedef unsigned int VPANEL;
typedef CGameTrace trace_t;
typedef unsigned short WEAPON_FILE_INFO_HANDLE;
typedef unsigned short ModelInstanceHandle_t;
typedef unsigned short MaterialHandle_t;

//
// hooks
//
typedef void ( __thiscall* CreateMoveFn )( void*, int, float, bool );
typedef void ( __thiscall* FrameStageNotifyFn )( void*, ClientFrameStage_t );
typedef void ( __thiscall* RunCommandFn )( void*, C_CSPlayer*, CUserCmd*, IMoveHelper* );
typedef void ( __thiscall* FinishMoveFn )( void*, C_CSPlayer*, CUserCmd*, CMoveData* );
typedef void ( __thiscall* UpdateFn )( void*, bool, bool, int, int );
typedef void ( __thiscall* ResetMouseFn )( void* );
typedef int ( __thiscall* DrawModelExFn )( void*, ModelRenderInfo_t* );
typedef void( __thiscall* OverrideViewFn )( void*, void* );
typedef bool( __thiscall* FireEvent )( void*, IGameEvent* );
typedef void( __thiscall* PaintTraverseFn )( void*, VPANEL, bool, bool );
typedef bool( __thiscall* IsPlayingTimeDemoFn )( void* );

typedef HRESULT ( D3DAPI* ResetFn )( IDirect3DDevice9*, D3DPRESENT_PARAMETERS* );
typedef HRESULT ( D3DAPI* PresentFn )( IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA* );

//
// implementation
//
struct player_info_t
{
	char			name[ 32 ];
	int				userID;
	char			guid[ 33 ];
	unsigned int	friendsID;
	char			friendsName[ 32 ];
	bool			fakeplayer;
	bool			ishltv;
	unsigned long	customFiles[ 4 ];
	unsigned char	filesDownloaded;
};

struct mstudiobone_t
{
	int			sznameindex;
	int			parent;
	int			bonecontroller[ 6 ];
	Vector3		pos;
	float		quat[ 4 ];
	Vector3		rot;
	Vector3		posscale;
	Vector3		rotscale;
	Matrix3x4	poseToBone;
	float		qAlignment[ 4 ];
	int			flags;
	int			proctype;
	int			procindex;
	mutable int	physicsbone;
	int			surfacepropidx;
	int			contents;
	int			unused[ 8 ];

	const char* GetName() const
	{
		return ( ( const char* )this ) + sznameindex;
	}
};

struct mstudiobbox_t
{
	int		bone;
	int		group;
	Vector3	bbmin;
	Vector3	bbmax;
	int		szhitboxnameindex;
	int		unused[ 3 ];
	float	radius;
	int		unused2[ 4 ];

	const char* GetName() const
	{
		if( szhitboxnameindex == 0 )
			return "";

		return ( ( const char* )this ) + szhitboxnameindex;
	}
};

struct mstudiohitboxset_t
{
	int sznameindex;
	int numhitboxes;
	int hitboxindex;

	const char* GetName() const
	{
		return ( ( const char* )this ) + sznameindex;
	}

	mstudiobbox_t* GetHitbox( int i ) const
	{
		return ( mstudiobbox_t* )( ( ( byte* )this ) + hitboxindex ) + i;
	}

	int GetHitboxCount() const
	{
		return numhitboxes;
	}
};

struct studiohdr_t
{
	int			id;
	int			version;
	long		checksum;
	char		name[64];
	int			length;
	Vector3		eyeposition;
	Vector3		illumposition;
	Vector3		hull_min;
	Vector3		hull_max;
	Vector3		view_bbmin;
	Vector3		view_bbmax;
	int			flags;
	int			numbones;
	int			boneindex;
	int			numbonecontrollers;
	int			bonecontrollerindex;
	int			numhitboxsets;
	int			hitboxsetindex;
	int			numlocalanim;
	int			localanimindex;
	mutable int	activitylistversion;
	mutable int	eventsindexed;
	int			numtextures;
	int			textureindex;

	const char* GetName() const
	{
		return name;
	}

	mstudiobone_t* GetBone( int i ) const
	{
		return ( mstudiobone_t* )( ( ( byte* )this ) + boneindex ) + i;
	}

	int GetBoneCount() const
	{
		return numbones;
	}

	mstudiohitboxset_t* GetHitboxSet( int i ) const
	{
		return ( mstudiohitboxset_t* )( ( ( byte* )this ) + hitboxsetindex ) + i;
	}
};

struct cplane_t
{
	Vector3	normal;
	float	dist;
	byte	type;
	byte	signbits;
	byte	pad[ 2 ];
};

struct csurface_t
{
	const char*		name;
	short			surfaceProps;
	unsigned short	flags;
};

struct Ray_t
{
	Vector3				m_Start;
	Vector3				m_Delta;
	Vector3				m_StartOffset;
	Vector3				m_Extents;

	bool				m_IsRay;
	bool				m_IsSwept;

	void Set( const Vector3& vStart, const Vector3& vEnd )
	{
		m_Delta.x = vEnd.x - vStart.x;
		m_Delta.y = vEnd.y - vStart.y;
		m_Delta.z = vEnd.z - vStart.z;
		m_IsSwept = ( m_Delta.LengthSqr() != 0 );
		m_Extents.Set();
		m_IsRay = true;
		m_StartOffset.Set();
		m_Start.x = vStart.x;
		m_Start.y = vStart.y;
		m_Start.z = vStart.z;
	}
};

struct surfacephysicsparams_t
{
	float	friction;
	float	elasticity;
	float	density;
	float	thickness;
	float	dampening;
};

struct surfaceaudioparams_t
{
	float	reflectivity;
	float	hardnessFactor;
	float	roughnessFactor;

	float	roughThreshold;
	float	hardThreshold;
	float	hardVelocityThreshold;
};

struct surfacesoundnames_t
{
	unsigned short	stepleft;
	unsigned short	stepright;

	unsigned short	impactSoft;
	unsigned short	impactHard;

	unsigned short	scrapeSmooth;
	unsigned short	scrapeRough;

	unsigned short	bulletImpact;
	unsigned short	rolling;

	unsigned short	breakSound;
	unsigned short	strainSound;
};

struct surfacesoundhandles_t
{
	short	stepleft;
	short	stepright;

	short	impactSoft;
	short	impactHard;

	short	scrapeSmooth;
	short	scrapeRough;

	short	bulletImpact;
	short	rolling;

	short	breakSound;
	short	strainSound;
};

struct surfacegameprops_t
{
	float			maxSpeedFactor;
	float			jumpFactor;

	unsigned short	material;

	unsigned char	climbable;
	unsigned char	pad;
};

struct surfacedata_t
{
	surfacephysicsparams_t	physics;
	surfaceaudioparams_t	audio;
	surfacesoundnames_t		sounds;
	surfacegameprops_t		game;

	surfacesoundhandles_t	soundhandles;
};

struct string_t
{
public:
	bool operator!() const
	{
		return ( pszValue == nullptr );
	}

	bool operator==( const string_t& rhs ) const
	{
		return ( pszValue == rhs.pszValue );
	}

	bool operator!=( const string_t& rhs ) const
	{
		return ( pszValue != rhs.pszValue );
	}

	bool operator<( const string_t& rhs ) const
	{
		return ( ( void* )pszValue < ( void* )rhs.pszValue );
	}

	const char* ToCStr() const
	{
		return ( pszValue ) ? pszValue : "";
	}

protected:
	const char* pszValue;
};

struct ModelRenderInfo_t
{
	int						flags;
	IClientRenderable*		pRenderable;
	ModelInstanceHandle_t	instance;
	int						ent_index;
	const model_t*			pModel;
	Vector3					origin;
	Vector3					angles;
	int						skin;
	int						body;
	int						hitboxset;
	const Matrix3x4*		pModelToWorld;
	const Matrix3x4*		pLightingOffset;
	const Vector3*			pLightingOrigin;

	ModelRenderInfo_t( void )
		:	pModelToWorld( nullptr ),
			pLightingOffset( nullptr ),
			pLightingOrigin( nullptr )
	{

	}
};

class IMaterial
{
public:
	const char* GetName()
	{
		typedef const char* ( __thiscall* GetNameFn )( void* );
		return Memory::VCall< GetNameFn >( this, 0 )( this );
	}

	const char* GetTextureGroupName()
	{
		typedef const char* ( __thiscall* GetTextureGroupNameFn )( void* );
		return Memory::VCall< GetTextureGroupNameFn >( this, 1 )( this );
	}

	void IncrementReferenceCount()
	{
		typedef void( __thiscall* IncrementReferenceCountFn )( void* );
		Memory::VCall< IncrementReferenceCountFn >( this, 12 )( this );
	}

	void AlphaModulate( float alpha )
	{
		typedef void( __thiscall* AlphaModulateFn )( void*, float );
		Memory::VCall< AlphaModulateFn >( this, 29 )( this, alpha );
	}

	void ColorModulate( float r, float g, float b )
	{
		typedef void( __thiscall* ColorModulateFn )( void*, float, float, float );
		Memory::VCall< ColorModulateFn >( this, 30 )( this, r, g, b );
	}

	void SetMaterialVarFlag( MaterialVarFlags_t flag, bool on )
	{
		typedef void( __thiscall* SetMaterialVarFlagFn )( void*, MaterialVarFlags_t, bool );
		Memory::VCall< SetMaterialVarFlagFn >( this, 31 )( this, flag, on );
	}

	bool IsErrorMaterial()
	{
		typedef bool( __thiscall* IsErrorMaterialFn )( void* );
		return Memory::VCall< IsErrorMaterialFn>( this, 44 )( this );
	}
};

inline bool IsErrorMaterialCheck( IMaterial *pMat )
{
	return !pMat || pMat->IsErrorMaterial();
}

class ConVar
{
public:
	void*		m_pVmt;
	ConVar*		m_pNext;
	BOOL		m_bRegistered;
	const char*	m_pszName;
	const char*	m_pszHelpString;
	int			m_nFlags;
	ConVar*		m_pParent;
	const char*	m_pszDefaultValue;
	char*		m_pszString;
	int			m_StringLength;
	float		m_fValue;
	int			m_nValue;
	bool		m_bHasMin;
	float		m_fMinVal;
	bool		m_bHasMax;
	float		m_fMaxVal;
	void*		m_fnChangeCallback;

	float GetFloat( void )
	{
		return m_fValue;
	}

	int GetInt( void )
	{
		return m_nValue;
	}

	bool GetBool( void )
	{
		return !!GetInt();
	}

	void SetValue( int value )
	{
		typedef void( __thiscall* SetValueFn )( void*, int );
		Memory::VCall<SetValueFn>( this, 9 )( this, value );
	}

	void SetValue( float value )
	{
		typedef void( __thiscall* SetValueFn )( void*, float );
		Memory::VCall<SetValueFn>( this, 10 )( this, value );
	}

	void SetValue( double value )
	{
		typedef void( __thiscall* SetValueFn )( void*, float );
		Memory::VCall<SetValueFn>( this, 10 )( this, value );
	}

	void SetValue( const char* value )
	{
		typedef void( __thiscall* SetValueFn )( void*, const char* );
		Memory::VCall<SetValueFn>( this, 11 )( this, value );
	}

};

class ClientClass
{
public:
	void*			m_pCreateFn;
	void*			m_pCreateEventFn;
	const char*		m_pNetworkName;
	RecvTable*		m_pRecvTable;
	ClientClass*	m_pNext;
	int				m_ClassID;
};

class CGlobalVarsBase
{
public:
	float	realtime;
	int		framecount;
	float	absoluteframetime;
	float	curtime;
	float	frametime;
	int		maxClients;
	int		tickcount;
	float	interval_per_tick;
	float	interpolation_amount;
	int		simTicksThisFrame;
	int		network_protocol;
	void*	pSaveData;
	bool	m_bClient;
	int		nTimestampNetworkingBase;
	int		nTimestampRandomizeWindow;
};

class CGlobalVars : public CGlobalVarsBase
{
public:
	string_t		mapname;
	string_t		mapGroupName;
	int				mapversion;
	string_t		startspot;
	MapLoadType_t	eLoadType;
	bool			bMapLoadFailed;
	bool			deathmatch;
	bool			coop;
	bool			teamplay;
	int				maxEntities;
	int				serverCount;
	void*			pEdicts;
};

class CUserCmd
{
public:
	CUserCmd()
	{
		Reset();
	}

	virtual ~CUserCmd()
	{
	}

	void Reset()
	{
		command_number = 0;
		tick_count = 0;
		viewangles.Set();
		forwardmove = 0.0f;
		sidemove = 0.0f;
		upmove = 0.0f;
		buttons = 0;
		impulse = 0;
		weaponselect = 0;
		weaponsubtype = 0;
		random_seed = 0;
		mousedx = 0;
		mousedy = 0;
		hasbeenpredicted = false;
	}

	CUserCmd& operator=( const CUserCmd& src )
	{
		if( this == &src )
			return *this;

		command_number		= src.command_number;
		tick_count			= src.tick_count;
		viewangles			= src.viewangles;
		forwardmove			= src.forwardmove;
		sidemove			= src.sidemove;
		upmove				= src.upmove;
		buttons				= src.buttons;
		impulse				= src.impulse;
		weaponselect		= src.weaponselect;
		weaponsubtype		= src.weaponsubtype;
		random_seed			= src.random_seed;
		mousedx				= src.mousedx;
		mousedy				= src.mousedy;
		hasbeenpredicted	= src.hasbeenpredicted;

		return *this;
	}

	CUserCmd( const CUserCmd& src )
	{
		*this = src;
	}

public:
	int		command_number;
	int		tick_count;
	Vector3	viewangles;
	float	forwardmove;
	float	sidemove;
	float	upmove;
	int		buttons;
	byte	impulse;
	int		weaponselect;
	int		weaponsubtype;
	int		random_seed;
	short	mousedx;
	short	mousedy;
	bool	hasbeenpredicted;
};

class CBaseTrace
{
public:
	Vector3			startpos;
	Vector3			endpos;
	cplane_t		plane;
	float			fraction;
	int				contents;
	unsigned short	dispFlags;
	bool			allsolid;
	bool			startsolid;
};

class CGameTrace : public CBaseTrace
{
public:
	float			fractionleftsolid;
	csurface_t		surface;
	int				hitgroup;
	short			physicsbone;
	C_BaseEntity*	m_pEnt;
	int				hitbox;

public:
	bool			DidHitWorld( void ) const;
	bool			DidHitNonWorldEntity( void ) const;
};

class CMoveData
{
private:
	MakePad( 1024 );
};

class IBaseClientDLL
{
public:
	ClientClass* GetAllClasses()
	{
		typedef ClientClass* ( __thiscall* GetAllClassesFn )( void* );
		return Memory::VCall< GetAllClassesFn >( this, IBaseClientDLL_GetAllClasses )( this );
	}
};

class IClientEntityList
{
public:
	C_BaseEntity* GetBaseEntity( int index )
	{
		typedef C_BaseEntity* ( __thiscall* GetBaseEntityFn )( void*, int );
		return Memory::VCall< GetBaseEntityFn >( this, IClientEntityList_GetBaseEntity )( this, index );
	}

	C_BaseEntity* GetBaseEntityFromHandle( CBaseHandle hEntity )
	{
		typedef C_BaseEntity* ( __thiscall* GetBaseEntityFromHandleFn )( void*, CBaseHandle );
		return Memory::VCall< GetBaseEntityFromHandleFn >( this, IClientEntityList_GetBaseEntityFromHandle )( this, hEntity );
	}

	int GetHighestEntityIndex()
	{
		typedef int ( __thiscall* GetHighestEntityIndexFn )( void* );
		return Memory::VCall< GetHighestEntityIndexFn >( this, IClientEntityList_GetHighestEntityIndex )( this );
	}
};

class IGameMovement
{
public:
	void ProcessMovement( C_CSPlayer* pPlayer, CMoveData* pMove )
	{
		typedef void ( __thiscall* ProcessMovementFn )( void*, C_CSPlayer*, CMoveData* );
		Memory::VCall< ProcessMovementFn >( this, IGameMovement_ProcessMovement )( this, pPlayer, pMove );
	}

	bool LadderMove()
	{
		typedef bool( __thiscall* LadderMoveFn )( void* );
		return Memory::VCall<LadderMoveFn>( this, 21 )( this );
	}
};

class IGameEvent
{
public:
	const char* GetName()
	{
		typedef const char* ( __thiscall* GetNameFn )( void* );
		return Memory::VCall<GetNameFn>( this, 1 )( this );
	}

	bool GetBool( const char* keyName = NULL, bool defaultValue = false )
	{
		typedef bool( __thiscall* GetBoolFn )( void*, const char*, bool );
		return Memory::VCall<GetBoolFn>( this, 5 )( this, keyName, defaultValue );
	}

	int GetInt( const char* keyName = NULL, int defaultValue = 0 )
	{
		typedef int( __thiscall* GetIntFn )( void*, const char*, int );
		return Memory::VCall<GetIntFn>( this, 6 )( this, keyName, defaultValue );
	}

	float GetFloat( const char* keyName = NULL, float defaultValue = 0.0f )
	{
		typedef float( __thiscall* GetFloatFn )( void*, const char*, float );
		return Memory::VCall<GetFloatFn>( this, 7 )( this, keyName, defaultValue );
	}

	const char* GetString( const char* keyName = NULL, const char* defaultValue = "" )
	{
		typedef const char* ( __thiscall* GetStringFn )( void*, const char*, const char* );
		return Memory::VCall<GetStringFn>( this, 8 )( this, keyName, defaultValue );
	}
};

struct bf_read;
struct bf_write;
class KeyValues;

class IGameEventListener2
{
public:
	virtual	~IGameEventListener2( void ) { };
	virtual void FireGameEvent( IGameEvent *event ) = 0;
};

class IGameEventManager2
{
public:
	bool AddListener( IGameEventListener2* listener, const char* name, bool bServerSide )
	{
		typedef bool( __thiscall* AddListenerFn )( void*, IGameEventListener2*, const char*, bool );
		return Memory::VCall<AddListenerFn>( this, 3 )( this, listener, name, bServerSide );
	}

	void RemoveListener( IGameEventListener2 *listener )
	{
		typedef void( __thiscall* RemoveListenerFn )( void*, IGameEventListener2* );
		Memory::VCall< RemoveListenerFn>( this, 5 )( this, listener );
	}
};



class INetChannelInfo
{
public:

	enum
	{
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	virtual const char  *GetName( void ) const = 0;	// get channel name
	virtual const char  *GetAddress( void ) const = 0; // get channel IP address as string
	virtual float		GetTime( void ) const = 0;	// current net time
	virtual float		GetTimeConnected( void ) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize( void ) const = 0;	// netchannel packet history size
	virtual int			GetDataRate( void ) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback( void ) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut( void ) const = 0;	// true if timing out
	virtual bool		IsPlayback( void ) const = 0;	// true if demo playback

	virtual float		GetLatency( int flow ) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency( int flow ) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss( int flow ) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke( int flow ) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAvgData( int flow ) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets( int flow ) const = 0; // avg packets/sec
	virtual int			GetTotalData( int flow ) const = 0;	 // total flow in/out in bytes
	virtual int			GetSequenceNr( int flow ) const = 0;	// last send seq number
	virtual bool		IsValidPacket( int flow, int frame_number ) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime( int flow, int frame_number ) const = 0; // time when packet was send
	virtual int			GetPacketBytes( int flow, int frame_number, int group ) const = 0; // group size of this packet
	virtual bool		GetStreamProgress( int flow, int *received, int *total ) const = 0;
};

class IPrediction
{
public:
	void Update( int startframe, bool validframe, int incoming_acknowledged, int outgoing_command )
	{
		typedef void ( __thiscall* UpdateFn )( void*, int, bool, int, int );
		Memory::VCall< UpdateFn >( this, 3 )( this, startframe, validframe, incoming_acknowledged, outgoing_command );
	}

	void SetupMove( C_CSPlayer* pPlayer, CUserCmd* pCmd, IMoveHelper* pHelper, CMoveData* pMove )
	{
		typedef void ( __thiscall* SetupMoveFn )( void*, C_CSPlayer*, CUserCmd*, IMoveHelper*, CMoveData* );
		Memory::VCall< SetupMoveFn >( this, IPrediction_SetupMove )( this, pPlayer, pCmd, pHelper, pMove );
	}

	void FinishMove( C_CSPlayer* pPlayer, CUserCmd* pCmd, CMoveData* pMove )
	{
		typedef void ( __thiscall* FinishMoveFn )( void*, C_CSPlayer*, CUserCmd*, CMoveData* );
		Memory::VCall< FinishMoveFn >( this, IPrediction_FinishMove )( this, pPlayer, pCmd, pMove );
	}
};

class IInput
{
public:
	CUserCmd* GetUserCmd( int sequence_number )
	{
		return &m_pCmd[ sequence_number % MULTIPLAYER_BACKUP ];
	} 

	int CAM_IsThirdPerson( void )
	{
		typedef int( __thiscall* CAM_IsThirdPersonFn )( void* );
		return Memory::VCall< CAM_IsThirdPersonFn >( this, 26 )( this );
	}

	void CAM_GetCameraOffset( Vector3* ofs )
	{
		typedef void( __thiscall* CAM_GetCameraOffsetFn )( void*, Vector3* );
		return Memory::VCall< CAM_GetCameraOffsetFn >( this, 27 )( this, ofs );
	}

	void CAM_ToThirdPerson( void )
	{
		typedef void( __thiscall* CAM_ToThirdPersonFn )( void* );
		Memory::VCall< CAM_ToThirdPersonFn >( this, 28 )( this );
	}

	void CAM_ToFirstPerson( void )
	{
		typedef void( __thiscall* CAM_ToFirstPersonFn )( void* );
		Memory::VCall< CAM_ToFirstPersonFn >( this, 29 )( this );
	}

private:
	MakePad( 0xC4 );
	CUserCmd*			m_pCmd;
}; 

class IClientMode
{

public:
};

class IVEngineClient
{
public:
	void GetScreenSize( int& width, int& height )
	{
		typedef void ( __thiscall* GetScreenSizeFn )( void*, int&, int& );
		Memory::VCall< GetScreenSizeFn>( this, IVEngineClient_GetScreenSize )( this, width, height );
	}

	void ClientCmd( const char *szCmdString )
	{
		typedef void( __thiscall* ClientCmdFn )( void*, const char* );
		Memory::VCall< ClientCmdFn >( this, 7 )( this, szCmdString );	  // 101
	}

	int	GetPlayerForUserID( int userID )
	{
		typedef int( __thiscall* GetPlayerForUserIDFn )( void*, int );
		return Memory::VCall<GetPlayerForUserIDFn>( this, 9 )( this, userID );
	}

	bool GetPlayerInfo( int ent_num, player_info_t* pinfo )
	{
		typedef bool ( __thiscall* GetPlayerInfoFn )( void*, int, player_info_t* );
		return Memory::VCall< GetPlayerInfoFn >( this, IVEngineClient_GetPlayerInfo )( this, ent_num, pinfo );
	}

	int GetLocalPlayer( void )
	{
		typedef int ( __thiscall* GetLocalPlayerFn )( void* );
		return Memory::VCall< GetLocalPlayerFn >( this, IVEngineClient_GetLocalPlayer )( this );
	}

	void GetViewAngles( Vector3& va )
	{
		typedef void ( __thiscall* GetViewAnglesFn )( void*, Vector3& );
		Memory::VCall< GetViewAnglesFn >( this, IVEngineClient_GetViewAngles )( this, va );
	}

	void SetViewAngles( Vector3& va )
	{
		typedef void ( __thiscall* SetViewAnglesFn )( void*, Vector3& );
		Memory::VCall< SetViewAnglesFn >( this, IVEngineClient_SetViewAngles )( this, va );
	}

	float Time( void )
	{
		typedef float( __thiscall* TimeFn )( void* );
		return Memory::VCall<TimeFn>( this, 14 )( this );
	}

	int GetMaxClients( void )
	{
		typedef int ( __thiscall* GetMaxClientsFn )( void* );
		return Memory::VCall< GetMaxClientsFn >( this, IVEngineClient_GetMaxClients )( this );
	}

	const Matrix4x4& WorldToScreenMatrix( void )
	{
		typedef const Matrix4x4& ( __thiscall* WorldToScreenMatrixFn )( void* );
		return Memory::VCall< WorldToScreenMatrixFn >( this, IVEngineClient_WorldToScreenMatrix )( this );
	}

	bool IsConnected( void )
	{
		typedef bool( __thiscall* IsConnectedFn )( void* );
		return Memory::VCall<IsConnectedFn>( this, 27 )( this );
	}

	bool IsInGame( void )
	{
		typedef bool( __thiscall* IsInGameFn )( void* );
		return Memory::VCall<IsInGameFn>( this, 26 )( this );
	}

	bool Con_IsVisible( void )
	{
		typedef bool( __thiscall* Con_IsVisibleFn )( void* );
		return Memory::VCall<Con_IsVisibleFn>( this, 11 )( this );
	}

	bool IsLevelMainMenuBackground( void )
	{
		typedef bool( __thiscall* IsLevelMainMenuBackgroundFn )( void* );
		return Memory::VCall<IsLevelMainMenuBackgroundFn>( this, 82 )( this );
	}

	INetChannelInfo* GetNetChannelInfo()
	{
		typedef INetChannelInfo* ( __thiscall* GetNetChannelInfoFn )( void* );
		return Memory::VCall<GetNetChannelInfoFn>( this, 72 )( this );
	}
};

class CPanel
{
public:
	const char* GetName( VPANEL vguiPanel )
	{
		typedef const char* ( __thiscall* GetNameFn )( void*, VPANEL );
		return Memory::VCall<GetNameFn>( this, 35 )( this, vguiPanel );
	}
};

class IVModelInfoClient
{
public:
	const char* GetModelName( const model_t* model )
	{
		typedef const char* ( __thiscall* GetModelNameFn )( void*, const model_t* );
		return Memory::VCall< GetModelNameFn >( this, IVModelInfoClient_GetModelName )( this, model );
	}

	studiohdr_t* GetStudioModel( const model_t* mod )
	{
		typedef studiohdr_t* ( __thiscall* GetStudioModelFn )( void*, const model_t* );
		return Memory::VCall< GetStudioModelFn >( this, IVModelInfoClient_GetStudioModel )( this, mod );
	}
};

class IVRenderView
{
public:
	void SetBlend( float blend )
	{
		typedef void ( __thiscall* SetBlendFn )( void*, float );
		Memory::VCall< SetBlendFn >( this, IVRenderView_SetBlend )( this, blend );
	}

	void SetColorModulation( const float* color )
	{
		typedef void ( __thiscall* SetColorModulationFn )( void*, const float* );
		Memory::VCall< SetColorModulationFn >( this, IVRenderView_SetColorModulation )( this, color );
	}
};

class IVModelRender
{
public:
	void ForcedMaterialOverride( IMaterial* mat )
	{
		typedef void ( __thiscall* ForcedMaterialOverrideFn )( void*, IMaterial* );
		Memory::VCall< ForcedMaterialOverrideFn >( this, IVModelRender_ForcedMaterialOverride )( this, mat );
	}
	int DrawModelEx( ModelRenderInfo_t &pInfo )
	{
		typedef int( __thiscall* DrawModelExFn )( void*, ModelRenderInfo_t& );
		return Memory::VCall< DrawModelExFn >( this, 19 )( this, pInfo );
	}
};

class IEngineTrace
{
public:
	int GetPointContents( const Vector3& vecAbsPosition, IHandleEntity** ppEntity = nullptr )
	{
		typedef int ( __thiscall* GetPointContentsFn )( void*, const Vector3&, IHandleEntity** );
		return Memory::VCall< GetPointContentsFn >( this, IEngineTrace_GetPointContents )( this, vecAbsPosition, ppEntity );
	}

	void TraceRay( const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, trace_t* pTrace )
	{
		typedef void ( __thiscall* TraceRayFn )( void*, const Ray_t&, unsigned int, ITraceFilter*, trace_t* );
		Memory::VCall< TraceRayFn >( this, IEngineTrace_TraceRay )( this, ray, fMask, pTraceFilter, pTrace );
	}
};

class ICvar
{
public:
	ConVar* FindVar( const char* var_name )
	{
		typedef ConVar* ( __thiscall* FindVarFn )( void*, const char* );
		return Memory::VCall< FindVarFn >( this, ICvar_FindVar )( this, var_name );
	}
};

class IPhysicsSurfaceProps
{
public:
	surfacedata_t* GetSurfaceData( int dataIndex )
	{
		typedef surfacedata_t* ( __thiscall* GetSurfaceDataFn )( void*, int );
		return Memory::VCall< GetSurfaceDataFn >( this, IPhysicsSurfaceProps_GetSurfaceData )( this, dataIndex );
	}
};

class IMaterialSystem
{
public:
	MaterialHandle_t FirstMaterial()
	{
		typedef MaterialHandle_t(__thiscall* FirstMaterialFn)(void*);
		return  Memory::VCall< FirstMaterialFn >(this, 23)(this);
	}
	
	MaterialHandle_t NextMaterial(MaterialHandle_t h)
	{
		typedef MaterialHandle_t(__thiscall* NextMaterialFn)(void*, MaterialHandle_t);
		return  Memory::VCall< NextMaterialFn >(this, 24)(this, h);
	}
	
	MaterialHandle_t InvalidMaterial()
	{
		typedef MaterialHandle_t(__thiscall* InvalidMaterialFn)(void*);
		return  Memory::VCall< InvalidMaterialFn >(this, 25)(this);
	}
	
	IMaterial* GetMaterial(MaterialHandle_t h)
	{
		typedef IMaterial* (__thiscall* GetMaterialFn)(void*, MaterialHandle_t);
		return  Memory::VCall< GetMaterialFn >(this, 26)(this, h);
	}
	
	IMaterial* FindMaterial(const char* pMaterialName, const char* pTextureGroupName, bool complain = true, const char* pComplainPrefix = NULL)
	{
		typedef IMaterial* (__thiscall* FindMaterialFn)(void*, const char*, const char*, bool, const char*);
		return  Memory::VCall< FindMaterialFn >(this, 27)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
	}

	int GetNumMaterials()
	{
		typedef int( __thiscall* GetNumMaterialsFn )( void* );
		return Memory::VCall<GetNumMaterialsFn>( this, 34 )( this );
	}

	IMaterial* CreateMaterial( const char* name, KeyValues* key )
	{
		typedef IMaterial* ( __thiscall* CreateMaterialFn )( void*, const char*, KeyValues* );
		return Memory::VCall< CreateMaterialFn >( this, IMaterialSystem_CreateMaterial )( this, name, key );
	}
};

class KeyValues
{
public:
	KeyValues( const char* setName );

	bool LoadFromBuffer( const char* resourceName, const char* pBuffer, void* pFileSystem = nullptr, const char* pPathID = nullptr );

private:
	MakePad( 0x25 );
};

class D3DDeviceWrapper
{
public:
	IDirect3DDevice9* m_pD3DDevice;
};

class ITraceFilter
{
public:
	virtual bool ShouldHitEntity( IHandleEntity* pEntity, int contentsMask ) = 0;
	virtual TraceType_t GetTraceType( void ) const = 0;
};

class CTraceFilter : public ITraceFilter
{
public:
	virtual TraceType_t GetTraceType( void ) const;
};

class CTraceFilterSimple : public CTraceFilter
{
public:
	CTraceFilterSimple( void );
	CTraceFilterSimple( const C_BaseEntity* pPassEnt );

	virtual bool ShouldHitEntity( IHandleEntity* pEntity, int contentsMask );

private:
	const C_BaseEntity* m_pPassEnt;
};

class CTraceFilterSkipTwoEntities : public CTraceFilter
{
public:
	CTraceFilterSkipTwoEntities( const C_BaseEntity* pPassEnt, const C_BaseEntity* pPassEnt2 );

	virtual bool ShouldHitEntity( IHandleEntity* pEntity, int contentsMask );

private:
	const C_BaseEntity* m_pPassEnt;
	const C_BaseEntity* m_pPassEnt2;
};

namespace Valve
{
	extern CCSWeaponInfo*		GetCSWpnData( C_WeaponCSBaseGun* pWeapon );
	extern D3DDeviceWrapper*	Dx9Device();

	extern void					SetPredictionSeed( CUserCmd* pCmd );
	extern void					SetPredictionPlayer( C_CSPlayer* pPlayer );

	extern void					RandomSeed( unsigned int nSeed );
	extern float				RandomFloat( float flMin, float flMax );
	extern int					RandomInt( int iMin, int iMax );

	extern void					GetBulletTypeParameters( int iBulletType, float& flPenetrationPower, float& flPenetrationDistance );
	extern void					ClipTraceToPlayers( const Vector3& vStart, const Vector3& vEnd, unsigned int iMask, ITraceFilter* pTraceFilter, trace_t* pTrace );
	extern void					GetMaterialParameters( int iMaterial, float& flPenetrationModifier, float& flDamageModifier );
	extern bool					TraceToExit( Vector3& vStart, Vector3& vDirection, Vector3& vEnd, float flStepSize, float flMaxDistance );
	extern int					GetPlayerModDamage( float flDamage, int iArmorValue, float flArmorRatio, bool bIsHeadshot, bool bIsFriendly, bool bHasHelmet );
	extern float				GetHitgroupModDamage( float flDamage, int iHitgroup );
}

#ifdef GetImageBase
#undef GetImageBase
#endif // GetImageBase

extern HMODULE GetImageBase( const char* name );
extern FARPROC GetProcedure( HMODULE hMod, const char* name );