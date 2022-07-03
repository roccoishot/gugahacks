#include "rageborto.hh"
#include "../helpers/math.hpp"
#include "../Globals.h"
#include "autowall.hpp"

void rage::ragebot( CUserCmd* cmd ) {

	auto local = g_LocalPlayer;

	if ( !local || !local->IsAlive( ) )
		return;

	auto weapon = local->m_hActiveWeapon( );

	if ( !weapon || !weapon->CanFire( ) )
		return;

	target opp = get_target( );

	int min_dmg = get_mid_dmg( );

	if ( !opp.ent || opp.damage < min_dmg )
		return;

	autostop( cmd );

	QAngle ang = Math::CalcAngle( local->GetEyePos( ), opp.pos ) - Globals::aim_punch_old * g_CVar->FindVar( "weapon_recoil_scale" )->GetFloat( );

	if ( hitchance(opp.ent, weapon, ang, get_hitchance()) ) {

		cmd->viewangles = Math::CalcAngle( local->GetEyePos( ), opp.pos );

		if ( weapon->GetCSWeaponData( )->bFullAuto )
			cmd->buttons |= IN_ATTACK;
		else if ( cmd->command_number % 2 )
			cmd->buttons |= IN_ATTACK;
		else
			cmd->buttons &= IN_ATTACK;

	}


}
/*
struct closer {
	inline bool operator( )( const C_BasePlayer& pll1, const C_BasePlayer& pll2 ) {
		auto pl1 = pll1;
		auto pl2 = pll2;

		return ( g_LocalPlayer->GetEyePos( ).DistTo( pl1.GetEyePos( ) ) < g_LocalPlayer->GetEyePos( ).DistTo( pl2.GetEyePos( ) ) );
	}
};*/

target rage::get_target( ) { 

	auto local = g_LocalPlayer;

	std::vector< C_BasePlayer* > valids;

	for ( int i = 1; i < g_EntityList->GetMaxEntities( ); i++ ) {

		auto pl = C_BasePlayer::GetPlayerByIndex( i );

		if ( !pl || !pl->IsPlayer( ) || !pl->IsAlive( ) || pl->IsDormant( ) || pl->IsTeammate( ) )
			continue;

		valids.push_back( pl );

	}

	//std::sort( valids.front( ), valids.back( ), closer( ) );

	target tg;

	for ( auto& pl : valids ) {

		for ( int i = 0; i < 18; i++ ) {

			auto hitbox_pos = pl->GetHitboxPos( i );

			int dmg = Autowall::GetDamage( hitbox_pos );

			if ( dmg > tg.damage ) {

				tg.ent = pl;
				tg.pos = hitbox_pos; // maybe do sum more shit
				tg.damage = dmg;

			}

		}

		// var min dmg
		if ( tg.damage > 50 )
			return tg;

	}

	return tg;

}

bool rage::hitchance( C_BasePlayer* pEnt, C_BaseCombatWeapon* pWeapon, QAngle Angle, int chance ) {
	static float Seeds = 256.f;

	Vector forward, right, up;

	Math::AngleVectors( Angle, forward, right, up );

	int Hits = 0, neededHits = ( Seeds * ( chance / 100.f ) );

	float weapSpread = pWeapon->GetSpread( ), weapInaccuracy = pWeapon->GetInaccuracy( );

	bool Return = false;

	for ( int i = 0; i < Seeds; i++ ) {
		float Inaccuracy = Math::random_float( 0.f, 1.f ) * weapInaccuracy;
		float Spread     = Math::random_float( 0.f, 1.f ) * weapSpread;

		Vector spreadView( ( cos( Math::random_float( 0.f, 2.f * M_PI ) ) * Inaccuracy ) + ( cos( Math::random_float( 0.f, 2.f * M_PI ) ) * Spread ),
		                   ( sin( Math::random_float( 0.f, 2.f * M_PI ) ) * Inaccuracy ) + ( sin( Math::random_float( 0.f, 2.f * M_PI ) ) * Spread ),
		                   0 ),
			direction;
		direction = Vector( forward.x + ( spreadView.x * right.x ) + ( spreadView.y * up.x ),
		                    forward.y + ( spreadView.x * right.y ) + ( spreadView.y * up.y ),
		                    forward.z + ( spreadView.x * right.z ) + ( spreadView.y * up.z ) )
		                .Normalize( );

		QAngle viewanglesSpread;
		Vector viewForward;

		Math::VectorAngles( direction, up, viewanglesSpread );
		Math::Normalize3( viewanglesSpread );

		Math::AngleVectors( viewanglesSpread, viewForward );
		viewForward.NormalizeInPlace( );

		viewForward = g_LocalPlayer->GetEyePos( ) + ( viewForward * pWeapon->GetCSWeaponData( )->flRange );

		trace_t Trace;
		Ray_t ray;

		ray.Init( g_LocalPlayer->GetEyePos( ), viewForward );
		g_EngineTrace->ClipRayToEntity( ray, MASK_SHOT | CONTENTS_GRATE, pEnt, &Trace );


		if ( Trace.hit_entity == pEnt )
			Hits++;

		if ( ( ( Hits / Seeds ) * 100.f ) >= chance ) {
			Return = true;
			break;
		}

		if ( ( Seeds - i + Hits ) < neededHits )
			break;
	}

	return Return;
}

void rage::autostop( CUserCmd* cmd ) {

	auto velocity = g_LocalPlayer->m_vecVelocity( );

	if ( velocity.Length2D( ) > 20.0f ) {
		Vector direction;
		QAngle real_view;

		Math::vector_angles( velocity, direction );
		g_EngineClient->GetViewAngles( &real_view );

		direction.y = real_view.yaw - direction.y;

		Vector forward;
		Math::angle_vectors( direction, forward );

		static auto cl_forwardspeed = g_CVar->FindVar( XorStr( "cl_forwardspeed" ) );
		static auto cl_sidespeed    = g_CVar->FindVar( XorStr( "cl_sidespeed" ) );

		auto negative_forward_speed = -cl_forwardspeed->GetFloat( );
		auto negative_side_speed    = -cl_sidespeed->GetFloat( );

		auto negative_forward_direction = forward * negative_forward_speed;
		auto negative_side_direction    = forward * negative_side_speed;

		cmd->forwardmove = negative_forward_direction.x;
		cmd->sidemove    = negative_side_direction.y;
	} else {
		auto speed = 0.1f;

		if ( cmd->buttons & IN_DUCK )
			speed *= 2.94117647f;

		static auto switch_move = false;

		if ( switch_move )
			cmd->sidemove += speed;
		else
			cmd->sidemove -= speed;

		switch_move = !switch_move;
	}

}

int rage::get_hitchance( ) { 
	return 90;
}

int rage::get_mid_dmg( ) {
	return 50;
}