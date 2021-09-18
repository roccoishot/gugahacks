#include "AnimFix.h"

AnimFix* g_Animfix = new AnimFix();

void AnimFix::UpdateFakeState()
{
	if ((!g_EngineClient->IsConnected() && !g_EngineClient->IsInGame()) || !g_LocalPlayer) {
		return;
	}

	if (!g_LocalPlayer->IsAlive()) {
		return;
	}

	static CBaseHandle* selfhandle = nullptr;
	static float spawntime = g_LocalPlayer->m_flSpawnTime();

	auto alloc = FakeAnimstate == nullptr;
	auto change = !alloc && selfhandle != &g_LocalPlayer->GetRefEHandle();
	auto reset = !alloc && !change && g_LocalPlayer->m_flSpawnTime() != spawntime;

	if (change) {
		memset(&FakeAnimstate, 0, sizeof(FakeAnimstate));
		selfhandle = (CBaseHandle*)&g_LocalPlayer->GetRefEHandle();
	}
	if (reset) {
		g_LocalPlayer->ResetAnimationState(FakeAnimstate);
		spawntime = g_LocalPlayer->m_flSpawnTime();
	}

	if (alloc || change) {
		FakeAnimstate = reinterpret_cast<CCSGOPlayerAnimState*>(g_pMemAlloc->Alloc(sizeof(CCSGOPlayerAnimState)));
		if (FakeAnimstate)
			g_LocalPlayer->CreateAnimationState(FakeAnimstate);
	}

	if (FakeAnimstate->m_iLastClientSideAnimationUpdateFramecount == g_GlobalVars->framecount)
		FakeAnimstate->m_iLastClientSideAnimationUpdateFramecount -= 1.f;

	g_LocalPlayer->GetEffects() |= 0x8;

	g_LocalPlayer->InvalidateBoneCache();

	AnimationLayer backup_layers[13];
	if (g_LocalPlayer->m_flOldSimulationTime() != g_LocalPlayer->m_flSimulationTime())
	{
		std::memcpy(backup_layers, g_LocalPlayer->GetAnimOverlays(),
			(sizeof(AnimationLayer) * g_LocalPlayer->GetNumAnimOverlays()));

		g_LocalPlayer->UpdateAnimationState(FakeAnimstate, g_FakeAngle); // update animstate
		g_LocalPlayer->SetAbsAngles(Vector(0, FakeAnimstate->m_flGoalFeetYaw, 0));
		g_LocalPlayer->GetAnimOverlay(12)->m_flWeight = FLT_EPSILON;
		g_LocalPlayer->SetupBones(fakematrix, 128, 0x7FF00, g_GlobalVars->curtime);// setup matrix
		//g_LocalPlayer->SetupBones(Hooks::fakelag_matrix, 128, 0x7FF00, g_GlobalVars->curtime);// setup matrix

		for (auto& i : fakematrix)
		{
			i[0][3] -= g_LocalPlayer->GetRenderOrigin().x;
			i[1][3] -= g_LocalPlayer->GetRenderOrigin().y;
			i[2][3] -= g_LocalPlayer->GetRenderOrigin().z;
		}


		std::memcpy(g_LocalPlayer->GetAnimOverlays(), backup_layers,
			(sizeof(AnimationLayer) * g_LocalPlayer->GetNumAnimOverlays()));
	}


	g_LocalPlayer->GetEffects() &= ~0x8;
}

void AnimFix::ApplyLocalPlayer()
{
	// same as above...
	if ((!g_EngineClient->IsConnected() && !g_EngineClient->IsInGame()) || !g_LocalPlayer) {
		return;
	}
	
	if (!g_LocalPlayer->IsAlive()) {
		return;
	}

	auto animstate = g_LocalPlayer->GetPlayerAnimState();
	if (!animstate)
		return;

	const auto backup_frametime = g_GlobalVars->frametime;
	const auto backup_curtime = g_GlobalVars->curtime;

	animstate->m_flGoalFeetYaw = g_VisualAngle.y;

	if (animstate->m_iLastClientSideAnimationUpdateFramecount == g_GlobalVars->framecount)
		animstate->m_iLastClientSideAnimationUpdateFramecount -= 1.f;

	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;
	g_GlobalVars->curtime = g_LocalPlayer->m_flSimulationTime();

	g_LocalPlayer->GetEFlags() &= ~0x1000;
	g_LocalPlayer->GetAbsVelocity() = g_LocalPlayer->GetVelocity();

	static float angle = animstate->m_flGoalFeetYaw;

	animstate->m_flFeetYawRate = 0.f;

	AnimationLayer backup_layers[13];
	if (g_LocalPlayer->m_flSimulationTime() != g_LocalPlayer->m_flOldSimulationTime())
	{
		std::memcpy(backup_layers, g_LocalPlayer->GetAnimOverlays(),
			(sizeof(AnimationLayer) * 13));

		ShouldUpdate = g_LocalPlayer->client_side_animation() = true;
		g_LocalPlayer->UpdateAnimationState(animstate, g_FakeAngle);
		g_LocalPlayer->UpdateClientSideAnimation();
		ShouldUpdate = g_LocalPlayer->client_side_animation() = false;

		angle = animstate->m_flGoalFeetYaw;

		std::memcpy(g_LocalPlayer->GetAnimOverlays(), backup_layers,
			(sizeof(AnimationLayer) * 13));
	}
	animstate->m_flGoalFeetYaw = angle;
	g_GlobalVars->curtime = backup_curtime;
	g_GlobalVars->frametime = backup_frametime;

	//if (Options::Menu.AntiAimTab.slide_walk.GetState() && Hooks::fake_walk)
	//	g_LocalPlayer->GetAnimOverlays()[6].m_flWeight = 0.0f;	// >_< premium
}