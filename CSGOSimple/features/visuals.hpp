#pragma once

#include "../singleton.hpp"
#include <stdio.h>
#include <string>
#include <iostream>
#include "../render.hpp"
#include "../helpers/math.hpp"
#include "../valve_sdk/csgostructs.hpp"

class Visuals : public Singleton<Visuals>
{
	friend class Singleton<Visuals>;

	CRITICAL_SECTION cs;

	Visuals();
	~Visuals();
public:
	class Player
	{
	public:
		struct
		{
			C_BasePlayer* pl;
			bool          is_enemy;
			bool          is_visible;
			Color         clr;
			Vector        head_pos;
			Vector        feet_pos;
			RECT          bbox;
		} ctx;

		bool Begin(C_BasePlayer * pl);
		void RenderBox(C_BaseEntity* pl);
		void RenderName(C_BaseEntity* pl);
		void RenderWeaponName(C_BaseEntity* pl);
		void sexdick(C_BasePlayer* ent);
		void RenderHealth(C_BaseEntity* pl);
		void RenderArmour();
		void RenderSnapline();
		void Skemlanton();
		void BruhTubb();
		void Drugs();
	};
	void RenderWeapon(C_BaseCombatWeapon* ent);
	void RenderDefuseKit(C_BaseEntity* ent);
	void RenderPlantedC4(C_BaseEntity* ent);
	void RenderItemEsp(C_BaseEntity* ent);
	void ThirdPerson();
public:
	void AddToDrawList();
	void Render();
};

