//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef GAMETRACE_H
#define GAMETRACE_H
#ifdef _WIN32
#pragma once
#endif


#include "public/mathlib/ray.h"
#include "public/tier0/color.h"
#include "public/entity2/entityinstance.h"
#include "public/mathlib/transform.h"
#include "public/tier0/generichash.h"
#include "public/tier0/utlvector.h"
#include "public/tier0/utlstring.h"

class IPhysicsBody;
class IPhysicsShape;

typedef IPhysicsBody* HPhysicsBody;
typedef IPhysicsShape* HPhysicsShape;

enum CollisionFunctionMask_t
{
	FCOLLISION_FUNC_ENABLE_SOLID_CONTACT 	= (1<<0),
	FCOLLISION_FUNC_ENABLE_TRACE_QUERY 		= (1<<1),
	FCOLLISION_FUNC_ENABLE_TOUCH_EVENT 		= (1<<2),
	FCOLLISION_FUNC_ENABLE_SELF_COLLISIONS 	= (1<<3),
	FCOLLISION_FUNC_IGNORE_FOR_HITBOX_TEST 	= (1<<4),
	FCOLLISION_FUNC_ENABLE_TOUCH_PERSISTS 	= (1<<5),
};

// these are on by default
#define FCOLLISION_FUNC_DEFAULT (FCOLLISION_FUNC_ENABLE_SOLID_CONTACT | FCOLLISION_FUNC_ENABLE_TRACE_QUERY | FCOLLISION_FUNC_ENABLE_TOUCH_EVENT)

enum RnQueryObjectSet
{
	// AMNOTE:
	// RNQUERY_OBJECTS_STATIC (0x1) is used in
	//      CLightQueryGameSystem::OnPostSimulate
	//      ray test by loading rays.bin
	//      CBaePlayerPawn::IsPlayerAimingAtTarget
	//      CBaseRagdoll related
	//      CSoundOpvarSetAutoRoomEntity::Spawn
	//      CNavSpaceBuildTreeJob

	// RNQUERY_OBJECTS_STATIC | RNQUERY_OBJECTS_KEYFRAMED (0x3) is used in
	//      when decoy hit entity isn't player pawn

	// RNQUERY_OBJECTS_KEYFRAMED | RNQUERY_OBJECTS_DYNAMIC (0x6) is used in
	//      checking if a grenade projectile has hit any player
	//      filtering bullet trace result
	//      checking if hit entity is a *_door_rotating
	//      checking if player is aiming at a CPointCommentaryNode

	// RNQUERY_OBJECTS_LOCATABLE (0x8) is used in
	//      checking if a CSmokeGrenadeProjectile is exploded from inferno
	//      CGamePhysicsQueryInterface::EntitiesAlong* to add g_LocatableEntities to results

	// RNQUERY_OBJECTS_KEYFRAMED | RNQUERY_OBJECTS_DYNAMIC | RNQUERY_OBJECTS_LOCATABLE (0xE) is used in
	//      CBotBreakableEnumerator
	//      CInferno::Think damage
	//      CBombTarget
	//      UTIL_EntitiesAlongRay
	//      CEnvShake::ApplyShake with type rumble
	//      CBaseFilter Think

	// RNQUERY_OBJECTS_STATIC | RNQUERY_OBJECTS_KEYFRAMED | RNQUERY_OBJECTS_DYNAMIC | RNQUERY_OBJECTS_LOCATABLE (0xF) is used in pretty much every trace filter

	RNQUERY_OBJECTS_STATIC          = (1 << 0), // static body
	RNQUERY_OBJECTS_KEYFRAMED       = (1 << 1), // keyframed body
	RNQUERY_OBJECTS_DYNAMIC         = (1 << 2), // dynamic body
	// AMNOTE: The physics world's casts ignore it and test only the three body bits
	RNQUERY_OBJECTS_LOCATABLE       = (1 << 3),


	RNQUERY_OBJECTS_ALL_GAME_ENTITIES = RNQUERY_OBJECTS_KEYFRAMED | RNQUERY_OBJECTS_DYNAMIC | RNQUERY_OBJECTS_LOCATABLE,
	RNQUERY_OBJECTS_ALL               = RNQUERY_OBJECTS_STATIC | RNQUERY_OBJECTS_ALL_GAME_ENTITIES,
};

enum HitGroup_t
{
	HITGROUP_INVALID = -1,
	HITGROUP_GENERIC = 0,
	HITGROUP_HEAD,
	HITGROUP_CHEST,
	HITGROUP_STOMACH,
	HITGROUP_LEFTARM,
	HITGROUP_RIGHTARM,
	HITGROUP_LEFTLEG,
	HITGROUP_RIGHTLEG,
	HITGROUP_NECK,
	HITGROUP_UNUSED,
	HITGROUP_GEAR,
	HITGROUP_SPECIAL,
	HITGROUP_COUNT,
};

enum HitboxShapeType_t
{
	HITBOX_SHAPE_HULL = 0,
	HITBOX_SHAPE_SPHERE,
	HITBOX_SHAPE_CAPSULE,
};

class CPhysSurfacePropertiesPhysics
{
public:
	CPhysSurfacePropertiesPhysics();
	
public:
	float m_friction;
	float m_elasticity;
	float m_density;
	float m_thickness;
	float m_softContactFrequency;
	float m_softContactDampingRatio;
};

class CPhysSurfacePropertiesVehicle
{
public:
	CPhysSurfacePropertiesVehicle();

public:
	float m_wheelDrag;
	float m_wheelFrictionScale;
};

class CPhysSurfacePropertiesSoundNames
{
public:
	CPhysSurfacePropertiesSoundNames();
	
public:
	CUtlString m_impactSoft;
	CUtlString m_impactHard;
	CUtlString m_scrapeSmooth;
	CUtlString m_scrapeRough;
	CUtlString m_bulletImpact;
	CUtlString m_rolling;
	CUtlString m_break;
	CUtlString m_strain;
	CUtlString m_meleeImpact;
	CUtlString m_pushOff;
	CUtlString m_skidStop;
	CUtlString m_resonant;
};

class CPhysSurfacePropertiesAudio
{
public:
	CPhysSurfacePropertiesAudio();
	
public:
	float m_reflectivity;
	float m_hardnessFactor;
	float m_roughnessFactor;
	float m_roughThreshold;
	float m_hardThreshold;
	float m_hardVelocityThreshold;
	float m_flStaticImpactVolume;
	float m_flOcclusionFactor;
};

class CPhysSurfaceProperties
{
public:
	CPhysSurfaceProperties();
	
public:
	CUtlString m_name;
	uint32 m_nameHash;
	uint32 m_baseNameHash;
	int32 m_nListIndex;
	int32 m_nBaseListIndex;
	bool m_bHidden;
	CUtlString m_description;
	CPhysSurfacePropertiesPhysics m_physics;
	CPhysSurfacePropertiesVehicle m_vehicleParams;
	CPhysSurfacePropertiesSoundNames m_audioSounds;
	CPhysSurfacePropertiesAudio m_audioParams;
};

class CHitBox
{
public:
	CHitBox();
	
public:
	CUtlString m_name;
	CUtlString m_sSurfaceProperty;
	CUtlString m_sBoneName;
	Vector m_vMinBounds;
	Vector m_vMaxBounds;
	float m_flShapeRadius;
	CUtlStringToken m_nBoneNameHash;
	int32 m_nGroupId;
	uint8 m_nShapeType;
	bool m_bTranslationOnly;
	uint32 m_CRC;
	Color m_cRenderColor;
	uint16 m_nHitBoxIndex;
	bool m_bForcedTransform;
	CTransform m_forcedTransform;
};

class CDefaultHitbox : public CHitBox
{
public:
	CDefaultHitbox();
	
public:
	char m_tempName[16];
	char m_tempBoneName[16];
	char m_tempSurfaceProp[16];
};

struct RnQueryShapeAttr_t
{
public:
	RnQueryShapeAttr_t();
	
	bool HasInteractsAsLayer( int nLayerIndex ) const;
	bool HasInteractsWithLayer( int nLayerIndex ) const;
	bool HasInteractsExcludeLayer( int nLayerIndex ) const;
	void EnableInteractsAsLayer( int nLayer );
	void EnableInteractsWithLayer( int nLayer );
	void EnableInteractsExcludeLayer( int nLayer );
	void DisableInteractsAsLayer( int nLayer );
	void DisableInteractsWithLayer( int nLayer );
	void DisableInteractsExcludeLayer( int nLayer );
	
public:
	// Which interaction layers do I interact or collide with? (e.g. I collide with LAYER_INDEX_CONTENTS_PASS_BULLETS because I am not a bullet)
	// NOTE: This is analogous to the "solid mask" or "trace mask" in source 1 (bit mask of CONTENTS_* or 1<<LAYER_INDEX_*)
	uint64 m_nInteractsWith;
	
	// Which interaction layers do I _not_ interact or collide with?  If my exclusion layers match m_nInteractsAs on the other object then no interaction happens.
	uint64 m_nInteractsExclude;
	
	// Which interaction layers do I represent? (e.g. I am a LAYER_INDEX_CONTENTS_PLAYER_CLIP volume)
	// NOTE: This is analogous to "contents" in source 1  (bit mask of CONTENTS_* or 1<<LAYER_INDEX_*)
	uint64 m_nInteractsAs;
	
	uint32 m_nEntityIdsToIgnore[2]; // this is the ID of the game entity which should be ignored
	uint32 m_nOwnerIdsToIgnore[2];	// this is the ID of the owner of the game entity which should be ignored
	uint16 m_nHierarchyIds[2];		// this is an ID for the hierarchy of game entities (used to disable collision among objects in a hierarchy)
	uint16 m_nIncludedDetailLayers;	// which detail layers the query can interact with, eg. can the query interact with the target's layer?
	uint8 m_nTargetDetailLayer;	// which detail layer the query is on, eg. can the target interact with the query's layer?
	uint8 m_nObjectSetMask; 		// set of RnQueryObjectSet bits
	uint8 m_nCollisionGroup; 		// one of the registered collision groups

	bool m_bHitSolid : 1; 							// if true, then query will hit solid
	bool m_bHitSolidRequiresGenerateContacts : 1; 	// if true, the FCOLLISION_FUNC_ENABLE_SOLID_CONTACT flag will be checked to hit solid
	bool m_bHitTrigger : 1; 						// if true, then query will hit tirgger
	bool m_bShouldIgnoreDisabledPairs : 1; 			// if true, then ignores if the query and shape entity IDs are in collision pairs
	bool m_bIgnoreIfBothInteractWithHitboxes : 1;	// if true, then ignores if both query and shape interact with LAYER_INDEX_CONTENTS_HITBOX
	bool m_bForceHitEverything : 1;					// if true, will hit any objects without any conditions
	bool m_unk101 : 1;								// AMNOTE: Set by default, the physics world's query filters never test it
};

struct RnQueryAttr_t : public RnQueryShapeAttr_t
{
};

class CTraceFilter : public RnQueryAttr_t
{
public:
	CTraceFilter( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT, bool bIterateEntities = true );

	CTraceFilter( 	CEntityInstance* pPassEntity, 
					CEntityInstance* pPassEntityOwner, 
					uint16 nHierarchyId,
					uint64 nInteractsWith = 0, 
					int nCollisionGroup = COLLISION_GROUP_DEFAULT, 
					bool bIterateEntities = true );
	
	CTraceFilter( 	CEntityInstance* pPassEntity1, 
					CEntityInstance* pPassEntity2, 
					CEntityInstance* pPassEntityOwner1, 
					CEntityInstance* pPassEntityOwner2, 
					uint16 nHierarchyId1,
					uint16 nHierarchyId2,
					uint64 nInteractsWith = 0, 
					int nCollisionGroup = COLLISION_GROUP_DEFAULT, 
					bool bIterateEntities = true );
	
	void SetPassEntity1( CEntityInstance* pEntity );
	void SetPassEntity2( CEntityInstance* pEntity );
	
	void SetPassEntityOwner1( CEntityInstance* pOwner );
	void SetPassEntityOwner2( CEntityInstance* pOwner );
	
	virtual ~CTraceFilter();
	virtual bool ShouldHitEntity( CEntityInstance* pEntity );
	
public:
	bool m_bIterateEntities; // if true then calls ShouldHitEntity for each hit entity
};

struct RnCollisionAttr_t
{
public:
	RnCollisionAttr_t();
	
	bool IsSolidContactEnabled() const;
	bool IsTraceAndQueryEnabled() const;
	bool IsTouchEventEnabled() const;
	bool IsSelfCollisionsEnabled() const;
	bool ShouldIgnoreForHitboxTest() const;
	bool IsTouchPersistsEnabled() const;

	bool HasInteractsAsLayer( int nLayerIndex ) const;
	bool HasInteractsWithLayer( int nLayerIndex ) const;
	bool HasInteractsExcludeLayer( int nLayerIndex ) const;
	void EnableInteractsAsLayer( int nLayer );
	void EnableInteractsWithLayer( int nLayer );
	void EnableInteractsExcludeLayer( int nLayer );
	void DisableInteractsAsLayer( int nLayer );
	void DisableInteractsWithLayer( int nLayer );
	void DisableInteractsExcludeLayer( int nLayer );
	
public:
	// Which interaction layers do I represent? (e.g. I am a LAYER_INDEX_CONTENTS_PLAYER_CLIP volume)
	// NOTE: This is analogous to "contents" in source 1  (bit mask of CONTENTS_* or 1<<LAYER_INDEX_*)
	uint64 m_nInteractsAs;
	
	// Which interaction layers do I interact or collide with? (e.g. I collide with LAYER_INDEX_CONTENTS_PASS_BULLETS because I am not a bullet)
	// NOTE: This is analogous to the "solid mask" or "trace mask" in source 1 (bit mask of CONTENTS_* or 1<<LAYER_INDEX_*)
	uint64 m_nInteractsWith;
	
	// Which interaction layers do I _not_ interact or collide with?  If my exclusion layers match m_nInteractsAs on the other object then no interaction happens.
	uint64 m_nInteractsExclude;
	
	uint32 m_nEntityId; 			// this is the ID of the game entity
	uint32 m_nOwnerId;  			// this is the ID of the owner of the game entity
	uint16 m_nHierarchyId; 			// this is an ID for the hierarchy of game entities (used to disable collision among objects in a hierarchy)
	uint16 m_nDetailLayerMask;
	uint8 m_nDetailLayerMaskType;
	uint8 m_nTargetDetailLayer;
	uint8 m_nCollisionGroup;		// one of the registered collision groups
	uint8 m_nCollisionFunctionMask;	// set of CollisionFunctionMask_t bits
};

class CGameTrace
{
public:
	CGameTrace();

	void Init();
	
	// Returns true if there was any kind of impact at all
	bool DidHit() const;
	
public:
	const CPhysSurfaceProperties *m_pSurfaceProperties;
	CEntityInstance *m_pEnt;
	// AMNOTE: The game's trace setup points it at its default hitbox (named "invalid_hitbox") instead of null, so it is
	// never null on traces the game fills
	const CHitBox *m_pHitbox;
	
	HPhysicsBody m_hBody;
	HPhysicsShape m_hShape;
	
	uint64 m_nContents;					// contents on other side of surface hit
	
	CTransform m_BodyTransform;
	RnCollisionAttr_t m_ShapeAttributes;
	
	Vector m_vStartPos; 				// start position
	Vector m_vEndPos; 					// final position
	Vector m_vHitNormal; 				// surface normal at impact
	Vector m_vHitPoint;					// exact hit point if m_bExactHitPoint is true, otherwise equal to m_vEndPos
	
	float m_flHitOffset;				// surface normal hit offset
	float m_flFraction;					// time completed, 1.0 = didn't hit anything
	
	int32 m_nCompoundId;				// the index of the primitive hit in a compound shape, -1 for other shapes
	int32 m_nTriangle;					// the index of the triangle that was hit
	int16 m_nHitboxBoneIndex; 			// the index of the hitbox bone that was hit
	
	RayType_t m_eRayType;
	
	bool m_bStartInSolid;				// if true, the initial point was in a solid area
	bool m_bExactHitPoint;				// if true, then m_vHitPoint is the exact hit point of the query and the shape
	
private:
	// No copy constructors allowed
	CGameTrace(const CGameTrace& vOther);
};


typedef CGameTrace trace_t;

//=============================================================================

class ITraceListData
{
public:
	virtual ~ITraceListData();

	virtual void Reset() = 0;
	virtual bool IsEmpty() = 0;
	// CanTraceRay will return true if the current volume encloses the ray
	// NOTE: The leaflist trace will NOT check this.  Traces are intersected
	// against the culled volume exclusively.
	virtual bool CanTraceRay( const Ray_t &ray ) = 0;
};
#endif // GAMETRACE_H

