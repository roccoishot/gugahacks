#pragma once 

#include "IClientNetworkable.hpp"
#include "IClientRenderable.hpp"
#include "IClientUnknown.hpp"
#include "IClientThinkable.hpp"

struct SpatializationInfo_t;

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
    virtual void Release(void) = 0;
};

#pragma pack(push, 1)
class CCSWeaponInfo { //xSeeker
public:
    char pad_0000[4]; //0x0000
    char* ConsoleName; //0x0004
    char pad_0008[12]; //0x0008
    int iMaxClip1; //0x0014
    char pad_0018[12]; //0x0018
    int iMaxClip2; //0x0024
    char pad_0028[4]; //0x0028
    char* szWorldModel; //0x002C
    char* szViewModel; //0x0030
    char* szDropedModel; //0x0034
    char pad_0038[4]; //0x0038
    char* N00000984; //0x003C
    char pad_0040[56]; //0x0040
    char* szEmptySound; //0x0078
    char pad_007C[4]; //0x007C
    char* szBulletType; //0x0080
    char pad_0084[4]; //0x0084
    char* szHudName; //0x0088
    char* szWeaponName; //0x008C
    char pad_0090[60]; //0x0090
    int weaponType; //0x00CC
    int iWeaponPrice; //0x00D0
    int iKillAward; //0x00D4
    char* szAnimationPrefex; //0x00D8
    float flCycleTime; //0x00DC
    float flCycleTimeAlt; //0x00E0
    float flTimeToIdle; //0x00E4
    float flIdleInterval; //0x00E8
    bool bFullAuto; //0x00EC
    char pad_00ED[3]; //0x00ED
    int iDamage; //0x00F0
    float flArmorRatio; //0x00F4
    int iBullets; //0x00F8
    char fixshonax[4]; //just here...................
    float flPenetration; //0x00FC
    float flFlinchVelocityModifierLarge; //0x0100
    float flFlinchVelocityModifierSmall; //0x0104
    // char fixshonax[4]; 
    float flRange; //0x0108
    float flRangeModifier; //0x010C
    char pad_0110[28]; //0x0110
    int iCrosshairMinDistance; //0x012C
    float fMaxSpeed; //0x0130
    float fMaxSpeedAlt; //0x0134
    char pad_0138[4]; //0x0138
    float flSpread; //0x013C
    float flSpreadAlt; //0x0140
    float fInaccuracyCrouch; //0x0144
    float fInaccuracyCrouchAlt; //0x0148
    float fInaccuracyStand; //0x014C
    float fInaccuracyStandAlt; //0x0150
    float fInaccuracyJumpIntial; //0x0154
    float fInaccaurcyJumpApex;
    float fInaccuracyJump; //0x0158
    float fInaccuracyJumpAlt; //0x015C
    float fInaccuracyLand; //0x0160
    float fInaccuracyLandAlt; //0x0164
    float fInaccuracyLadder; //0x0168
    float fInaccuracyLadderAlt; //0x016C
    float fInaccuracyFire; //0x0170
    float fInaccuracyFireAlt; //0x0174
    float fInaccuracyMove; //0x0178
    float fInaccuracyMoveAlt; //0x017C
    float fInaccuracyReload; //0x0180
    int iRecoilSeed; //0x0184
    float flRecoilAngle; //0x0188
    float flRecoilAngleAlt; //0x018C
    float flRecoilVariance; //0x0190
    float flRecoilAngleVarianceAlt; //0x0194
    float flRecoilMagnitude; //0x0198
    float flRecoilMagnitudeAlt; //0x019C
    float flRecoilMagnatiudeVeriance; //0x01A0
    float flRecoilMagnatiudeVerianceAlt; //0x01A4
    float flRecoveryTimeCrouch; //0x01A8
    float flRecoveryTimeStand; //0x01AC
    float flRecoveryTimeCrouchFinal; //0x01B0
    float flRecoveryTimeStandFinal; //0x01B4
    int iRecoveryTransititionStartBullet; //0x01B8
    int iRecoveryTransititionEndBullet; //0x01BC
    bool bUnzoomAfterShot; //0x01C0
    char pad_01C1[31]; //0x01C1
    char* szWeaponClass; //0x01E0
    char pad_01E4[56]; //0x01E4
    float flInaccuracyPitchShift; //0x021C
    float flInaccuracySoundThreshold; //0x0220
    float flBotAudibleRange; //0x0224
    char pad_0228[12]; //0x0228
    bool bHasBurstMode; //0x0234
};
#pragma pack(pop)

class IWeaponSystem
{
    virtual void unused0() = 0;
    virtual void unused1() = 0;
public:
    virtual CCSWeaponInfo* GetWpnData(unsigned ItemDefinitionIndex) = 0;
};