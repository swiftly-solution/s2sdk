#include "public/game/shareddefs.h"

CViewVectors::CViewVectors()
{}

CViewVectors::CViewVectors( 
	Vector vView,
	Vector vHullMin,
	Vector vHullMax,
	Vector vDuckHullMin,
	Vector vDuckHullMax,
	Vector vDuckView,
	Vector vObsHullMin,
	Vector vObsHullMax,
	Vector vDeadViewHeight )
{
	m_vView = vView;
	m_vHullMin = vHullMin;
	m_vHullMax = vHullMax;
	m_vDuckHullMin = vDuckHullMin;
	m_vDuckHullMax = vDuckHullMax;
	m_vDuckView = vDuckView;
	m_vObsHullMin = vObsHullMin;
	m_vObsHullMax = vObsHullMax;
	m_vDeadViewHeight = vDeadViewHeight;
}

FireBulletsInfo_t::FireBulletsInfo_t()
{
	m_iShots = 1;
	m_vecSpread.Init( 0, 0, 0 );
	m_flDistance = 8192;
	m_iTracerFreq = 4;
	m_flDamage = 0.0f;
	m_flPlayerDamage = 0.0f;
	m_pAttacker = NULL;
	m_nFlags = 0;
	m_pAdditionalIgnoreEnt = NULL;
	m_flDamageForceScale = 1.0f;

#ifdef _DEBUG
	m_iAmmoType = -1;
	m_vecSrc.Init( VEC_T_NAN, VEC_T_NAN, VEC_T_NAN );
	m_vecDirShooting.Init( VEC_T_NAN, VEC_T_NAN, VEC_T_NAN );
#endif
	m_bPrimaryAttack = true;
}

FireBulletsInfo_t::FireBulletsInfo_t( int nShots, const Vector &vecSrc, const Vector &vecDir, const Vector &vecSpread, float flDistance, int nAmmoType, bool bPrimaryAttack )
{
	m_iShots = nShots;
	m_vecSrc = vecSrc;
	m_vecDirShooting = vecDir;
	m_vecSpread = vecSpread;
	m_flDistance = flDistance;
	m_iAmmoType = nAmmoType;
	m_iTracerFreq = 4;
	m_flDamage = 0;
	m_flPlayerDamage = 0;
	m_pAttacker = NULL;
	m_nFlags = 0;
	m_pAdditionalIgnoreEnt = NULL;
	m_flDamageForceScale = 1.0f;
	m_bPrimaryAttack = bPrimaryAttack;
}

EmitSound_t::EmitSound_t()
:
	m_pSoundName( nullptr ),
	m_flVolume( VOL_NORM ),
	m_flSoundTime( 0.0f ),
	m_unk001( -1 ),
	m_nForceGuid( 0 ),
	m_nSourceSoundscape( 0 ),
	m_nPitch( PITCH_NORM ),
	m_unk101( 1 ),
	m_unk102( 0 ),
	m_unk103( 0 ),
	m_bFindSourceSoundscape( false ),
	m_bUseOrigin( false )
{
}

bool IsSplitScreenSupported()
{
	return ( MAX_SPLITSCREEN_PLAYERS > 1 ) ? true : false;
}
