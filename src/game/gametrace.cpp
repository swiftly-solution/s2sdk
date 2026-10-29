#include "public/game/gametrace.h"

CPhysSurfacePropertiesPhysics::CPhysSurfacePropertiesPhysics()
{
	m_friction = 0.0f;
	m_elasticity = 0.0f;
	m_density = 0.0f;
	m_thickness = 0.1f;
	m_softContactFrequency = 0.0f;
	m_softContactDampingRatio = 0.0f;
}

CPhysSurfacePropertiesVehicle::CPhysSurfacePropertiesVehicle()
{
	m_wheelDrag = 0.0f;
	m_wheelFrictionScale = 0.0f;
}

CPhysSurfacePropertiesSoundNames::CPhysSurfacePropertiesSoundNames()
{
	m_meleeImpact = "";
	m_pushOff = "";
	m_skidStop = "";
}

CPhysSurfacePropertiesAudio::CPhysSurfacePropertiesAudio()
{
	m_reflectivity = 0.0f;
	m_hardnessFactor = 0.0f;
	m_roughnessFactor = 0.0f;
	m_roughThreshold = 0.0f;
	m_hardThreshold = 0.0f;
	m_hardVelocityThreshold = 0.0f;
	m_flStaticImpactVolume = 0.0f;
	m_flOcclusionFactor = 0.0f;
}

CPhysSurfaceProperties::CPhysSurfaceProperties()
{
	m_bHidden = false;
}

CHitBox::CHitBox()
{
	m_vMinBounds.Init();
	m_vMaxBounds.Init();
	m_nGroupId = HITGROUP_GENERIC;
	m_nShapeType = HITBOX_SHAPE_HULL;
	m_bTranslationOnly = false;
	m_CRC = 0;
	m_cRenderColor.SetColor( 255, 255, 255, 255 );
	m_nHitBoxIndex = 0;
	m_bForcedTransform = false;
	m_forcedTransform.SetToIdentity();
}

CDefaultHitbox::CDefaultHitbox()
{
	V_strncpy( m_tempName, "invalid_hitbox", sizeof( m_tempName ) );
	V_strncpy( m_tempBoneName, "invalid_bone", sizeof( m_tempBoneName ) );
	V_strncpy( m_tempSurfaceProp, "default", sizeof( m_tempSurfaceProp ) );
	
	m_name = m_tempName;
	m_sSurfaceProperty = m_tempSurfaceProp;

	m_sBoneName = m_tempBoneName;
	m_nBoneNameHash = CUtlStringToken( m_tempBoneName );

	m_cRenderColor.SetColor( 0, 0, 0, 0 );
}

RnQueryShapeAttr_t::RnQueryShapeAttr_t()
{
	m_nInteractsWith = 0;
	m_nInteractsExclude = 0;
	m_nInteractsAs = 0;
	
	m_nEntityIdsToIgnore[0] = INVALID_EHANDLE_INDEX;
	m_nEntityIdsToIgnore[1] = INVALID_EHANDLE_INDEX;
	
	m_nOwnerIdsToIgnore[0] = INVALID_EHANDLE_INDEX;
	m_nOwnerIdsToIgnore[1] = INVALID_EHANDLE_INDEX;
	
	m_nHierarchyIds[0] = 0;
	m_nHierarchyIds[1] = 0;
	m_nIncludedDetailLayers = (uint16)-1;
	m_nTargetDetailLayer = 0;
	m_nObjectSetMask = RNQUERY_OBJECTS_ALL;
	m_nCollisionGroup = COLLISION_GROUP_ALWAYS;
	
	m_bHitSolid = true;
	m_bHitSolidRequiresGenerateContacts = false;
	m_bHitTrigger = false;
	m_bShouldIgnoreDisabledPairs = true;
	m_bIgnoreIfBothInteractWithHitboxes = false;
	m_bForceHitEverything = false;
	m_unk101 = true;
}

bool RnQueryShapeAttr_t::HasInteractsAsLayer( int nLayerIndex ) const
{ return ( m_nInteractsAs & ( 1ull << nLayerIndex ) ) != 0; }

bool RnQueryShapeAttr_t::HasInteractsWithLayer( int nLayerIndex ) const
{ return ( m_nInteractsWith & ( 1ull << nLayerIndex ) ) != 0; }

bool RnQueryShapeAttr_t::HasInteractsExcludeLayer( int nLayerIndex ) const
{ return ( m_nInteractsExclude & ( 1ull << nLayerIndex ) ) != 0; }

void RnQueryShapeAttr_t::EnableInteractsAsLayer( int nLayer )
{ m_nInteractsAs |= ( 1ull << nLayer ); }

void RnQueryShapeAttr_t::EnableInteractsWithLayer( int nLayer )
{ m_nInteractsWith |= ( 1ull << nLayer ); }

void RnQueryShapeAttr_t::EnableInteractsExcludeLayer( int nLayer )
{ m_nInteractsExclude |= ( 1ull << nLayer ); }

void RnQueryShapeAttr_t::DisableInteractsAsLayer( int nLayer )
{ m_nInteractsAs &= ~( 1ull << nLayer ); }

void RnQueryShapeAttr_t::DisableInteractsWithLayer( int nLayer )
{ m_nInteractsWith &= ~( 1ull << nLayer ); }

void RnQueryShapeAttr_t::DisableInteractsExcludeLayer( int nLayer )
{ m_nInteractsExclude &= ~( 1ull << nLayer ); }

CTraceFilter::CTraceFilter( uint64 nInteractsWith, int nCollisionGroup, bool bIterateEntities )
{
	m_nInteractsWith = nInteractsWith;
	m_nCollisionGroup = nCollisionGroup;
	m_bIterateEntities = bIterateEntities;
}

CTraceFilter::CTraceFilter( 	CEntityInstance* pPassEntity, 
				CEntityInstance* pPassEntityOwner, 
				uint16 nHierarchyId,
				uint64 nInteractsWith, 
				int nCollisionGroup, 
				bool bIterateEntities )
{
	SetPassEntity1( pPassEntity );
	
	SetPassEntityOwner1( pPassEntityOwner );

	m_nHierarchyIds[0] = nHierarchyId;
	
	m_nInteractsWith = nInteractsWith;
	m_nCollisionGroup = nCollisionGroup;
	m_bIterateEntities = bIterateEntities;
}

CTraceFilter::CTraceFilter( 	CEntityInstance* pPassEntity1, 
				CEntityInstance* pPassEntity2, 
				CEntityInstance* pPassEntityOwner1, 
				CEntityInstance* pPassEntityOwner2, 
				uint16 nHierarchyId1,
				uint16 nHierarchyId2,
				uint64 nInteractsWith, 
				int nCollisionGroup, 
				bool bIterateEntities )
{
	SetPassEntity1( pPassEntity1 );
	SetPassEntity2( pPassEntity2 );
	
	SetPassEntityOwner1( pPassEntityOwner1 );
	SetPassEntityOwner2( pPassEntityOwner2 );
	
	m_nHierarchyIds[0] = nHierarchyId1;
	m_nHierarchyIds[1] = nHierarchyId2;
	
	m_nInteractsWith = nInteractsWith;
	m_nCollisionGroup = nCollisionGroup;
	m_bIterateEntities = bIterateEntities;
}

void CTraceFilter::SetPassEntity1( CEntityInstance* pEntity )
{ m_nEntityIdsToIgnore[0] = pEntity ? pEntity->GetRefEHandle().ToInt() : INVALID_EHANDLE_INDEX; }

void CTraceFilter::SetPassEntity2( CEntityInstance* pEntity )
{ m_nEntityIdsToIgnore[1] = pEntity ? pEntity->GetRefEHandle().ToInt() : INVALID_EHANDLE_INDEX; }

void CTraceFilter::SetPassEntityOwner1( CEntityInstance* pOwner )
{ m_nOwnerIdsToIgnore[0] = pOwner ? pOwner->GetRefEHandle().ToInt() : INVALID_EHANDLE_INDEX; }

void CTraceFilter::SetPassEntityOwner2( CEntityInstance* pOwner )
{ m_nOwnerIdsToIgnore[1] = pOwner ? pOwner->GetRefEHandle().ToInt() : INVALID_EHANDLE_INDEX; }

CTraceFilter::~CTraceFilter()
{}

bool CTraceFilter::ShouldHitEntity( CEntityInstance* pEntity )
{ return true; }

RnCollisionAttr_t::RnCollisionAttr_t()
{
	m_nInteractsAs = 0;
	m_nInteractsWith = 0;
	m_nInteractsExclude = 0;
	m_nEntityId = 0;
	m_nOwnerId = INVALID_EHANDLE_INDEX;
	m_nHierarchyId = 0;
	m_nCollisionGroup = COLLISION_GROUP_ALWAYS;
	m_nCollisionFunctionMask = FCOLLISION_FUNC_DEFAULT;
}

bool RnCollisionAttr_t::IsSolidContactEnabled() const
{ return ( m_nCollisionFunctionMask & FCOLLISION_FUNC_ENABLE_SOLID_CONTACT ) != 0; }

bool RnCollisionAttr_t::IsTraceAndQueryEnabled() const
{ return ( m_nCollisionFunctionMask & FCOLLISION_FUNC_ENABLE_TRACE_QUERY ) != 0; }

bool RnCollisionAttr_t::IsTouchEventEnabled() const
{ return ( m_nCollisionFunctionMask & FCOLLISION_FUNC_ENABLE_TOUCH_EVENT ) != 0; }

bool RnCollisionAttr_t::IsSelfCollisionsEnabled() const
{ return ( m_nCollisionFunctionMask & FCOLLISION_FUNC_ENABLE_SELF_COLLISIONS ) != 0; }

bool RnCollisionAttr_t::ShouldIgnoreForHitboxTest() const
{ return ( m_nCollisionFunctionMask & FCOLLISION_FUNC_IGNORE_FOR_HITBOX_TEST ) != 0; }

bool RnCollisionAttr_t::IsTouchPersistsEnabled() const
{ return ( m_nCollisionFunctionMask & FCOLLISION_FUNC_ENABLE_TOUCH_PERSISTS ) != 0; }

bool RnCollisionAttr_t::HasInteractsAsLayer( int nLayerIndex ) const
{ return ( m_nInteractsAs & ( 1ull << nLayerIndex ) ) != 0; }

bool RnCollisionAttr_t::HasInteractsWithLayer( int nLayerIndex ) const
{ return ( m_nInteractsWith & ( 1ull << nLayerIndex ) ) != 0; }

bool RnCollisionAttr_t::HasInteractsExcludeLayer( int nLayerIndex ) const
{ return ( m_nInteractsExclude & ( 1ull << nLayerIndex ) ) != 0; }

void RnCollisionAttr_t::EnableInteractsAsLayer( int nLayer )
{ m_nInteractsAs |= ( 1ull << nLayer ); }

void RnCollisionAttr_t::EnableInteractsWithLayer( int nLayer )
{ m_nInteractsWith |= ( 1ull << nLayer ); }

void RnCollisionAttr_t::EnableInteractsExcludeLayer( int nLayer )
{ m_nInteractsExclude |= ( 1ull << nLayer ); }

void RnCollisionAttr_t::DisableInteractsAsLayer( int nLayer )
{ m_nInteractsAs &= ~( 1ull << nLayer ); }

void RnCollisionAttr_t::DisableInteractsWithLayer( int nLayer )
{ m_nInteractsWith &= ~( 1ull << nLayer ); }

void RnCollisionAttr_t::DisableInteractsExcludeLayer( int nLayer )
{ m_nInteractsExclude &= ~( 1ull << nLayer ); }

CGameTrace::CGameTrace()
{
	Init();
}

void CGameTrace::Init()
{
	m_pSurfaceProperties = NULL;
	m_pEnt = NULL;
	m_pHitbox = NULL;
	m_hBody = NULL;
	m_hShape = NULL;
	m_nContents = 0;
	m_BodyTransform.SetToIdentity();
	m_vHitNormal.Init();
	m_vHitPoint.Init();
	m_flHitOffset = 0.0f;
	m_flFraction = 1.0f;
	m_nCompoundId = -1;
	m_nTriangle = -1;
	m_nHitboxBoneIndex = -1;
	m_eRayType = RAY_TYPE_LINE;
	m_bStartInSolid = false;
	m_bExactHitPoint = false;
}

bool CGameTrace::DidHit() const
{ 
	return m_flFraction < 1 || m_bStartInSolid; 
}

ITraceListData::~ITraceListData()
{}
