#include "csgostructs.hpp"
#include "../Helpers/Math.hpp"
#include "../Helpers/Utils.hpp"
#include <algorithm>

//increase it if valve added some funcs to baseentity :lillulmoa:
constexpr auto VALVE_ADDED_FUNCS = 0ull;

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

bool C_BaseEntity::IsPlayer()
{
	//index: 152
	//ref: "effects/nightvision"
	//sig: 8B 92 ? ? ? ? FF D2 84 C0 0F 45 F7 85 F6
	return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 158 + VALVE_ADDED_FUNCS)(this);
}

bool C_BaseEntity::IsLoot() {
	return GetClientClass()->m_ClassID == ClassId_CPhysPropAmmoBox ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropLootCrate ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropRadarJammer ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropWeaponUpgrade ||
		GetClientClass()->m_ClassID == ClassId_CDrone ||
		GetClientClass()->m_ClassID == ClassId_CDronegun ||
		GetClientClass()->m_ClassID == ClassId_CItem_Healthshot ||
		GetClientClass()->m_ClassID == ClassId_CItemCash || 
		GetClientClass()->m_ClassID == ClassId_CBumpMine;
}

bool C_BaseEntity::IsWeapon()
{
	//index: 160
	//ref: "CNewParticleEffect::DrawModel"
	//sig: 8B 80 ? ? ? ? FF D0 84 C0 74 6F 8B 4D A4
	return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 166 + VALVE_ADDED_FUNCS)(this);
}


bool C_BaseEntity::IsPlantedC4()
{
	return GetClientClass()->m_ClassID == ClassId_CPlantedC4;
}

bool C_BaseEntity::IsDefuseKit()
{
	return GetClientClass()->m_ClassID == ClassId_CBaseAnimating;
}

CCSWeaponInfo* C_BaseCombatWeapon::GetCSWeaponData()
{
	//return g_WeaponSystem->GetWpnData(this->m_Item().m_iItemDefinitionIndex());
	return CallVFunction<CCSWeaponInfo* (__thiscall*)(void*)>(this, 461)(this);

}

bool C_BaseCombatWeapon::HasBullets()
{
	return !IsReloading() && m_iClip1() > 0;
}

float& C_BaseViewModel::m_flCycle()
{
	static auto m_flCycle = Utils::find_in_datamap(GetPredDescMap(), "m_flCycle");
	return *(float*)(uintptr_t(this) + m_flCycle);
}

void C_BasePlayer::SetAngle2(Vector wantedang)
{

	if (!this)
		return;

	typedef void(__thiscall* SetAngleFn)(void*, const Vector&);
	static SetAngleFn SetAngle = (SetAngleFn)((DWORD)GetModuleHandleA("client.dll") + 0x1C2AC0);
	SetAngle(this, wantedang);
} // paste it on your Entity class

float& C_BaseViewModel::m_flAnimTime()
{
	static auto m_flAnimTime = Utils::find_in_datamap(GetPredDescMap(), "m_flAnimTime");
	return *(float*)(uintptr_t(this) + m_flAnimTime);
}

bool C_BaseCombatWeapon::CanFire()
{
	auto owner = this->m_hOwnerEntity().Get();
	if (!owner)
		return false;

	if (IsReloading() || m_iClip1() <= 0)
		return false;

	auto flServerTime = owner->m_nTickBase() * g_GlobalVars->interval_per_tick;

	if (owner->m_flNextAttack() > flServerTime)
		return false;


	return m_flNextPrimaryAttack() <= flServerTime;
}

bool C_BaseCombatWeapon::is_non_aim()
{
	if (!this) //-V704
		return true;

	auto idx = m_iItemDefinitionIndex();

	if (idx == WEAPON_C4 || idx == WEAPON_HEALTHSHOT)
		return true;

	if (IsKnife())
		return true;

	if (IsGrenade())
		return true;

	return false;
}

bool C_BaseCombatWeapon::IsGrenade()
{
	return GetCSWeaponData()->weaponType == WEAPONTYPE_GRENADE;
}

bool C_BaseCombatWeapon::IsGun()
{
	switch (GetCSWeaponData()->weaponType)
	{
	case WEAPONTYPE_C4:
		return false;
	case WEAPONTYPE_GRENADE:
		return false;
	case WEAPONTYPE_KNIFE:
		return false;
	case WEAPONTYPE_UNKNOWN:
		return false;
	default:
		return true;
	}
}

bool C_BaseCombatWeapon::IsMashineGun()
{
	return GetCSWeaponData()->weaponType == WEAPONTYPE_MACHINEGUN;
}

bool C_BaseCombatWeapon::IsSMG()
{
	switch (GetCSWeaponData()->weaponType)
	{
	case WEAPONTYPE_SUBMACHINEGUN:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsZeus()
{
	if (this->m_Item().m_iItemDefinitionIndex() == WEAPON_TASER)
	{
		return true;
	}
	return false;
}

bool C_BaseCombatWeapon::IsAuto()
{
	auto id = this->m_iItemDefinitionIndex();
	if (id == WEAPON_G3SG1 || id == WEAPON_SCAR20)
		return true;
	return false;
}

bool C_BaseCombatWeapon::IsShotgun()
{
	auto id = this->m_iItemDefinitionIndex();
	if (id == WEAPON_MAG7 || id == WEAPON_SAWEDOFF || id == WEAPON_XM1014 || id == WEAPON_NOVA)
		return true;
	return false;
}

bool C_BaseCombatWeapon::IsAutomaticGun()
{
	switch (GetCSWeaponData()->weaponType)
	{
	case WEAPONTYPE_RIFLE:
		return true;
	case WEAPONTYPE_SUBMACHINEGUN:
		return true;
	case WEAPONTYPE_SHOTGUN:
		return true;
	case WEAPONTYPE_MACHINEGUN:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsBallistic()
{
	auto id = this->m_iItemDefinitionIndex();
	return (id == WEAPON_AUG || id == WEAPON_SG556);
}

bool C_BaseCombatWeapon::IsKnife()
{
	if (this->m_Item().m_iItemDefinitionIndex() == WEAPON_TASER) return false;
	return GetCSWeaponData()->weaponType == WEAPONTYPE_KNIFE;
}

bool C_BaseCombatWeapon::can_double_tap()
{
	if (!this) //-V704
		return false;

	if (is_non_aim())
		return false;

	auto idx = m_iItemDefinitionIndex();

	if (idx == WEAPON_TASER || idx == WEAPON_REVOLVER || idx == WEAPON_SSG08 || idx == WEAPON_AWP || idx == WEAPON_XM1014 || idx == WEAPON_NOVA || idx == WEAPON_SAWEDOFF || idx == WEAPON_MAG7)
		return false;

	return true;
}

bool C_BaseCombatWeapon::IsRifle()
{
	switch (GetCSWeaponData()->weaponType)
	{
	case WEAPONTYPE_RIFLE:
		return true;
	case WEAPONTYPE_SUBMACHINEGUN:
		return true;
	case WEAPONTYPE_SHOTGUN:
		return true;
	case WEAPONTYPE_MACHINEGUN:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsPistol()
{
	switch (GetCSWeaponData()->weaponType)
	{
	case WEAPONTYPE_PISTOL:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsSniper()
{
	switch (GetCSWeaponData()->weaponType)
	{
	case WEAPONTYPE_SNIPER_RIFLE:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsReloading()
{
	static auto inReload = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
	return *(bool*)((uintptr_t)this + inReload);
}

float C_BaseCombatWeapon::GetInaccuracy()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 483 + VALVE_ADDED_FUNCS)(this);
}

float C_BaseCombatWeapon::GetSpread()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 453 + VALVE_ADDED_FUNCS)(this);
}

void C_BaseCombatWeapon::UpdateAccuracyPenalty()
{
	CallVFunction<void(__thiscall*)(void*)>(this, 484 + VALVE_ADDED_FUNCS)(this);
}

CUtlVector<IRefCounted*>& C_BaseCombatWeapon::m_CustomMaterials()
{	static auto inReload = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "83 BE ? ? ? ? ? 7F 67") + 2) - 12;
	return *(CUtlVector<IRefCounted*>*)((uintptr_t)this + inReload);
}

bool* C_BaseCombatWeapon::m_bCustomMaterialInitialized()
{
	static auto currentCommand = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "C6 86 ? ? ? ? ? FF 50 04") + 2);
	return (bool*)((uintptr_t)this + currentCommand);
}

CUserCmd*& C_BasePlayer::m_pCurrentCommand()
{
	static auto currentCommand = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "89 BE ? ? ? ? E8 ? ? ? ? 85 FF") + 2);
	return *(CUserCmd**)((uintptr_t)this + currentCommand);
}

int C_BasePlayer::GetNumAnimOverlays()
{
	return *(int*)((DWORD)this + 0x2990);
}

AnimationLayer *C_BasePlayer::GetAnimOverlays()
{
	return *(AnimationLayer**)((DWORD)this + 0x2990);
}

AnimationLayer *C_BasePlayer::GetAnimOverlay(int i)
{
	if (i < 15)
		return &GetAnimOverlays()[i];
	return nullptr;
}

int C_BasePlayer::GetSequenceActivity(int sequence)
{
	auto hdr = g_MdlInfo->GetStudiomodel(this->GetModel());

	if (!hdr)
		return -1;

	// sig for stuidohdr_t version: 53 56 8B F1 8B DA 85 F6 74 55
	// sig for C_BaseAnimating version: 55 8B EC 83 7D 08 FF 56 8B F1 74 3D
	// c_csplayer vfunc 242, follow calls to find the function.
	// Thanks @Kron1Q for merge request
	static auto get_sequence_activity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 53 8B 5D 08 56 8B F1 83"));

	return get_sequence_activity(this, hdr, sequence);
}

CCSGOPlayerAnimState* C_BasePlayer::GetPlayerAnimState() {
	return *reinterpret_cast<CCSGOPlayerAnimState**>(reinterpret_cast<void*>(uintptr_t(this) + 0x9960));
}

void C_BasePlayer::UpdateAnimationState()
{
	if (!this) //-V704
		return;

	auto animstate = GetPlayerAnimState();

	if (!animstate)
		return;

	if (animstate->m_iLastClientSideAnimationUpdateFramecount >= g_GlobalVars->framecount) //-V614
		animstate->m_iLastClientSideAnimationUpdateFramecount = g_GlobalVars->framecount - 1;

	using Fn = void(__thiscall*)(void*);
	call_virtual<Fn>(this, 223)(this);
}

void C_BasePlayer::UpdateAnimationState(CCSGOPlayerAnimState* state, QAngle angle) {
	static auto update_anim_state = Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");

	if (!update_anim_state)
		return;

	__asm
	{
		push 0
	}

	__asm
	{
		mov ecx, state

		movss xmm1, dword ptr[angle + 4]
		movss xmm2, dword ptr[angle]

		call update_anim_state
	}
}

void C_BasePlayer::ResetAnimationState(CCSGOPlayerAnimState *state)
{
	using ResetAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*);
	static auto ResetAnimState = (ResetAnimState_t)Utils::PatternScan(GetModuleHandleA("client.dll"), "56 6A 01 68 ? ? ? ? 8B F1");
	if (!ResetAnimState)
		return;

	ResetAnimState(state);
}

void C_BasePlayer::CreateAnimationState(CCSGOPlayerAnimState *state)
{
	using CreateAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*, C_BasePlayer*);
	static auto CreateAnimState = (CreateAnimState_t)Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46");
	if (!CreateAnimState)
		return;

	CreateAnimState(state, this);
}

Vector C_BasePlayer::GetEyePos()
{
	return m_vecOrigin() + m_vecViewOffset();
}

player_info_t C_BasePlayer::GetPlayerInfo()
{
	player_info_t info;
	g_EngineClient->GetPlayerInfo(EntIndex(), &info);
	return info;
}

bool C_BasePlayer::valid(bool check_team, bool check_dormant)
{
	if (!this) //-V704
		return false;

	if (!g_LocalPlayer)
		return false;

	if (!IsPlayer())
		return false;

	if (!IsAlive())
		return false;

	if (IsDormant() && check_dormant)
		return false;

	if (check_team && IsEnemy() == false)
		return false;

	return true;
}

bool C_BasePlayer::IsAlive()
{
	return m_lifeState() == LIFE_ALIVE;
}

void C_BasePlayer::SetVAngles(QAngle angles)
{
	static auto deadflag = NetvarSys::Get().GetOffset("DT_BasePlayer", "deadflag");
	*(QAngle*)((DWORD)this + deadflag + 0x4) = angles;
}

bool C_BasePlayer::IsFlashed()
{
	static auto m_flFlashMaxAlpha = NetvarSys::Get().GetOffset("DT_CSPlayer", "m_flFlashMaxAlpha");
	return *(float*)((uintptr_t)this + m_flFlashMaxAlpha - 0x8) > 200.0;
}
bool C_BasePlayer::IsTeammate()
{
	return !Utils::IsDangerZone() && this != g_LocalPlayer && this->m_iTeamNum() == g_LocalPlayer->m_iTeamNum();
}
bool C_BasePlayer::IsEnemy()
{
	const bool is_dangerzone = Utils::IsDangerZone();

	if (is_dangerzone && g_LocalPlayer->m_nSurvivalTeam() == -1)
		return true;

	if (is_dangerzone)
		return g_LocalPlayer->m_nSurvivalTeam() != m_nSurvivalTeam();

	return g_LocalPlayer->m_iTeamNum() != m_iTeamNum();
}
bool C_BasePlayer::HasC4()
{
	static auto fnHasC4
		= reinterpret_cast<bool(__thiscall*)(void*)>(
			Utils::PatternScan(GetModuleHandleW(L"client.dll"), "56 8B F1 85 F6 74 31")
			);

	return fnHasC4(this);
}

uint32_t& C_BasePlayer::m_iMostRecentModelBoneCounter()
{
	static auto invalidate_bone_cache = Utils::PatternScan2("client.dll", "80 3D ?? ?? ?? ?? ?? 74 16 A1 ?? ?? ?? ?? 48 C7 81");
	static auto most_recent_model_bone_counter = *(uintptr_t*)(invalidate_bone_cache + 0x1B);

	return *(uint32_t*)((uintptr_t)this + most_recent_model_bone_counter);
}

float& C_BasePlayer::m_flLastBoneSetupTime()
{
	static auto invalidate_bone_cache = Utils::PatternScan2("client.dll", "80 3D ?? ?? ?? ?? ?? 74 16 A1 ?? ?? ?? ?? 48 C7 81");
	static auto last_bone_setup_time = *(uintptr_t*)(invalidate_bone_cache + 0x11);

	return *(float*)((uintptr_t)this + last_bone_setup_time);
}

void C_BasePlayer::invalidate_bone_cache()
{
	if (!this) //-V704
		return;

	m_flLastBoneSetupTime() = -FLT_MAX;
	m_iMostRecentModelBoneCounter() = UINT_MAX;
}


void C_BasePlayer::set_abs_origin(const Vector& origin)
{
	if (!this) //-V704
		return;

	using Fn = void(__thiscall*)(void*, const Vector&);
	static auto fn = reinterpret_cast<Fn>(Utils::PatternScan2("client.dll", "55 8B EC 83 E4 F8 51 53 56 57 8B F1"));

	return fn(this, origin);
}

bool C_BasePlayer::setup_bones_fixed(matrix3x4_t* matrix, int mask)
{
	if (!this) //-V704
		return false;

	auto setuped = false;

	auto backup_value = *(uint8_t*)((uintptr_t)this + 0x274);
	*(uint8_t*)((uintptr_t)this + 0x274) = 0;

	auto backup_effects = m_fEffects();
	m_fEffects() |= 8;

	auto animstate = GetPlayerAnimState();
	auto previous_weapon = animstate ? animstate->m_pLastBoneSetupWeapon : nullptr;

	if (previous_weapon)
		animstate->m_pLastBoneSetupWeapon = animstate->m_pActiveWeapon;

	auto backup_abs_origin = abs_origin();

	if (this != g_LocalPlayer)
		set_abs_origin(m_vecOrigin());

	invalidate_bone_cache();

#if RELEASE
	if (mask == BONE_USED_BY_ANYTHING)
		SetupBones(matrix, MAXSTUDIOBONES, mask, m_flSimulationTime());
	else
	{
		CSetupBones setup_bones;

		setup_bones.m_animating = this;
		setup_bones.m_vecOrigin = m_vecOrigin();
		setup_bones.m_animLayers = get_animlayers();
		setup_bones.m_pHdr = m_pStudioHdr();
		setup_bones.m_nAnimOverlayCount = animlayer_count();
		setup_bones.m_angAngles = Vector(0.0f, animstate ? animstate->m_flGoalFeetYaw : 0.0f, 0.0f);
		setup_bones.m_boneMatrix = matrix;

		memcpy(setup_bones.m_flPoseParameters, &m_flPoseParameter(), 24 * sizeof(float));

		auto weapon = m_hActiveWeapon().Get();
		auto world_model = weapon ? weapon->m_hWeaponWorldModel().Get() : nullptr;

		if (world_model)
			memcpy(setup_bones.m_flWorldPoses, &world_model->m_flPoseParameter(), 24 * sizeof(float));

		setup_bones.m_boneMask = mask;

		Vector position[MAXSTUDIOBONES];
		Quaternion q[MAXSTUDIOBONES];

		setup_bones.m_vecBones = position;
		setup_bones.m_quatBones = q;
		setup_bones.m_flCurtime = m_flSimulationTime();

		setup_bones.setup();

		if (m_CachedBoneData().Base() != m_BoneAccessor().m_pBones)
			memcpy(m_BoneAccessor().m_pBones, setup_bones.m_boneMatrix, m_CachedBoneData().Count() * sizeof(matrix3x4_t));

		setup_bones.fix_bones_rotations();
	}
#else
	SetupBones(matrix, matrix ? MAXSTUDIOBONES : -1, mask, m_flSimulationTime());
#endif

	if (this != g_LocalPlayer)
		set_abs_origin(backup_abs_origin);

	if (previous_weapon)
		animstate->m_pLastBoneSetupWeapon = previous_weapon;

	m_fEffects() = backup_effects;
	*(uint8_t*)((uintptr_t)this + 0x274) = backup_value;

	return setuped;
}

Vector C_BasePlayer::GetHitboxPos(int hitbox_id)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
		auto studio_model = g_MdlInfo->GetStudiomodel(GetModel());
		if (studio_model) {
			auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
			if (hitbox) {
				auto
					min = Vector{},
					max = Vector{};

				Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
				Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

				return (min + max) / 2.0f;
			}
		}
	}
	return Vector{};
}

mstudiobbox_t* C_BasePlayer::GetHitbox(int hitbox_id)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
		auto studio_model = g_MdlInfo->GetStudiomodel(GetModel());
		if (studio_model) {
			auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
			if (hitbox) {
				return hitbox;
			}
		}
	}
	return nullptr;
}

bool C_BasePlayer::GetHitboxPos(int hitbox, Vector &output)
{
	if (hitbox >= HITBOX_MAX)
		return false;

	const model_t *model = this->GetModel();
	if (!model)
		return false;

	studiohdr_t *studioHdr = g_MdlInfo->GetStudiomodel(model);
	if (!studioHdr)
		return false;

	matrix3x4_t matrix[MAXSTUDIOBONES];
	if (!this->SetupBones(matrix, MAXSTUDIOBONES, 0x100, 0))
		return false;

	mstudiobbox_t *studioBox = studioHdr->GetHitboxSet(0)->GetHitbox(hitbox);
	if (!studioBox)
		return false;

	Vector min, max;

	Math::VectorTransform(studioBox->bbmin, matrix[studioBox->bone], min);
	Math::VectorTransform(studioBox->bbmax, matrix[studioBox->bone], max);

	output = (min + max) * 0.5f;

	return true;
}

Vector C_BasePlayer::GetBonePos(int bone)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];
	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, 0.0f)) {
		return boneMatrix[bone].at(3);
	}
	return Vector{};
}

bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, int hitbox)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;
	Vector endpos;

	if (player->valid(false, true))
	endpos = player->GetHitboxPos(hitbox);

	ray.Init(GetEyePos(), endpos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, const Vector& pos)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	ray.Init(GetEyePos(), pos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	if (player->valid(false, true))
		return tr.hit_entity == player || tr.fraction > 0.97f;
	else
		return false;
}

void C_BasePlayer::UpdateClientSideAnimation()
{
	if (!this) //-V704
		return;

	auto animstate = GetPlayerAnimState();

	if (!animstate)
		return;

	if (animstate->m_iLastClientSideAnimationUpdateFramecount >= g_GlobalVars->framecount) //-V614
		animstate->m_iLastClientSideAnimationUpdateFramecount = g_GlobalVars->framecount - 1;

	using Fn = void(__thiscall*)(void*);
	call_virtual<Fn>(this, 224)(this);
}

matrix3x4_t C_BasePlayer::GetBoneMatrix(int BoneID)
{
	matrix3x4_t matrix;

	auto offset = *reinterpret_cast<uintptr_t*>(uintptr_t(this) + 0x2698);
	if (offset)
		matrix = *reinterpret_cast<matrix3x4_t*>(offset + 0x30 * BoneID);

	return matrix;
}

uint32_t& C_BasePlayer::m_fEffects()
{
	static auto m_fEffects = Utils::find_in_datamap(GetPredDescMap(), XorStr("m_fEffects"));
	return *(uint32_t*)(uintptr_t(this) + m_fEffects);
}

uint32_t& C_BasePlayer::m_iEFlags()
{
	static auto m_iEFlags = Utils::find_in_datamap(GetPredDescMap(), XorStr("m_iEFlags"));
	return *(uint32_t*)(uintptr_t(this) + m_iEFlags);
}

Vector C_BasePlayer::abs_origin()
{
	return CallVFunction<Vector& (__thiscall*)(void*)>(this, 10)(this);
}
void C_BasePlayer::InvalidateBoneCache()
{
	static DWORD addr = (DWORD)Utils::PatternScan(GetModuleHandleA("client.dll"), "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81");

	*(int*)((uintptr_t)this + 0xA30) = g_GlobalVars->framecount; //we'll skip occlusion checks now
	*(int*)((uintptr_t)this + 0xA28) = 0;//clear occlusion flags

	unsigned long g_iModelBoneCounter = **(unsigned long**)(addr + 10);
	*(unsigned int*)((DWORD)this + 0x2924) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
	*(unsigned int*)((DWORD)this + 0x2690) = (g_iModelBoneCounter - 1); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
}

int C_BasePlayer::m_nMoveType()
{
	return *(int*)((uintptr_t)this + 0x25C);
}

QAngle* C_BasePlayer::GetVAngles()
{
	static auto deadflag = NetvarSys::Get().GetOffset("DT_BasePlayer", "deadflag");
	return (QAngle*)((uintptr_t)this + deadflag + 0x4);
}
void C_BaseAttributableItem::SetModelIndex(int modelIndex)
{
	return CallVFunction<void(__thiscall*)(void*, int)>(this, 75)(this, modelIndex);
}
void C_BaseEntity::setModelIndex(int modelIndex)
{
	return CallVFunction<void(__thiscall*)(void*, int)>(this, 75)(this, modelIndex);
}
void C_BaseAttributableItem::SetGloveModelIndex(int modelIndex)
{
	return CallVFunction<void(__thiscall*)(void*, int)>(this, 75)(this, modelIndex);
}

void C_BaseEntity::set_abs_angles(const Vector& angle)
{
	if (!this) //-V704
		return;

	using Fn = void(__thiscall*)(void*, const Vector&);
	static auto fn = reinterpret_cast<Fn>(Utils::PatternScan2("client.dll", "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"));

	return fn(this, angle);
}

void C_BaseViewModel::SendViewModelMatchingSequence(int sequence)
{
	return CallVFunction<void(__thiscall*)(void*, int)>(this, 247 + VALVE_ADDED_FUNCS)(this, sequence);
}

float_t C_BasePlayer::m_flSpawnTime()
{
	return *(float_t*)((uintptr_t)this + 0xA370);
}