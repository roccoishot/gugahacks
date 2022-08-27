#pragma once

#include "../valve_sdk/csgostructs.hpp"
#include <d3d9.h>
#include "helpers/detourhook.h"
namespace index
{
  //constexpr auto EmitSound1 = 5;
  //constexpr auto EmitSound2 = 6;
  //constexpr auto EndScene = 42;
  //constexpr auto Reset = 16;

  //constexpr auto DispatchUserMessage = 38;
  //constexpr auto PaintTraverse = 41;
  //constexpr auto CreateMove = 22;
  //constexpr auto PlaySound = 82;
  //constexpr auto FrameStageNotify = 37;
  //constexpr auto DrawModelExecute = 21;
  //constexpr auto DoPostScreenSpaceEffects = 44;
  //constexpr auto SvCheatsGetBool = 13;
  //constexpr auto IsPlayingDemo = 82;
 // constexpr auto GetDemoPlaybackParameters = 218;
  //constexpr auto SendMessageGC = 0;
  //constexpr auto RetrieveMessageGC = 2;
  //constexpr auto OverrideView = 18;
  //constexpr auto FireEvent = 9;
  //constexpr auto LockCursor = 67;
  //constexpr auto ClientModeCreateMove = 24;
  //constexpr auto ListLeavesInBox = 6;
  constexpr auto SuppressLists = 16;
  constexpr auto PostEntityPacketReceived = 20 / 4;
  constexpr auto ReadPacketEntities = 64;
}

namespace Hooks
{
  void Initialize( );
  void Shutdown( );

 //inline  CDetourHook direct3d_hook;
 //inline  CDetourHook direct3d_hook1;
 //inline CDetourHook vguipanel_hook;
//inline CDetourHook vguisurf_hook;
//inline CDetourHook mdlrender_hook;
//inline CDetourHook viewrender_hook;  
//inline  CDetourHook hlclient_hook;
typedef bool(__thiscall* sendnetmsg_fn)(void*, INetMessage* msg, bool reliable, bool voice);
//inline sendnetmsg_fn original_sendnetmsg = nullptr;
//inline  CDetourHook sound_hook;
//inline  CDetourHook  clientmode_hook;
//inline  CDetourHook  clientmode_hook1;
//inline  CDetourHook  clientmode_hook2;
//inline  CDetourHook engineclient_hook;
//inline  CDetourHook filesystem_hook;
//inline  CDetourHook filesystem_hook1;
//inline  CDetourHook sv_cheats;
//inline  CDetourHook gameevents_hook;
//inline recv_prop_hook* sequence_hook;
//inline  CDetourHook bsp_query_hook;
//inline  CDetourHook hk_netchannel;
inline  CDetourHook  partition_hook;//
inline  CDetourHook prediction_hook;//
inline  CDetourHook clientstate_hook;//
 using EndScene = long( __stdcall * )( IDirect3DDevice9* );
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  using Reset = long( __stdcall * )( IDirect3DDevice9*, D3DPRESENT_PARAMETERS* );
  using CreateMove = void( __thiscall* )( IBaseClientDLL*, int, float, bool );
  using CreateMoveClientMode = bool( __thiscall* )( IClientMode*, float, CUserCmd* );
  using DispatchUserMessage = bool(__fastcall*)(void*, void*, int, unsigned int, unsigned int, const void*);
  using PaintTraverse = void( __thiscall* )( IPanel*, vgui::VPANEL, bool, bool );
  using EmitSound1 = void( __thiscall* )( void*, IRecipientFilter&, int, int, const char*, unsigned int, const char*, float, int, float, int, int, const Vector*, const Vector*, void*, bool, float, int, int );

  using FrameStageNotify = void( __thiscall* )(IBaseClientDLL*,  ClientFrameStage_t );
  //using PlaySound = void( __thiscall* )( ISurface*, const char* name );
  using LockCursor_t = void( __thiscall* )( ISurface* );
  using DrawModelExecute = void( __thiscall* )( void*, void*, DrawModelInfo_t*, matrix3x4_t*, float*, float*, Vector&, int );
  using FireEvent = bool( __thiscall* )( IGameEventManager2*, IGameEvent* pEvent );
  using DoPostScreenEffects = int( __thiscall* )( IClientMode*, int );
  using OverrideView = void( __thiscall* )( IClientMode*, CViewSetup* );
  //using IsPlayingDemo = bool ( __thiscall* ) ( );
  using GetDemoPlaybackParameters = void* ( __thiscall* ) ( );
  using PostEntityPacketReceived = void( __thiscall* )( void* );
  using ReadPacketEntities = void( __thiscall* )( void*, void* );

  using SuppressLists = bool( __thiscall* )( void*, int, bool );
  
  //bool __fastcall hkDispatchUserMessage(void* ecx, void* edx, int type, unsigned int a3, unsigned int length, const void* msg_data);
  //long __stdcall hkEndScene( IDirect3DDevice9* device );
 // long __stdcall hkReset( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters );
 // bool __stdcall hkCreateMove( float input_sample_frametime, CUserCmd* cmd );
 // void __stdcall hkPaintTraverse( vgui::VPANEL panel, bool forceRepaint, bool allowForce );
 // void __stdcall hkEmitSound1( IRecipientFilter & filter, int iEntIndex, int iChannel, const char * pSoundEntry, unsigned int nSoundEntryHash, const char * pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector * pOrigin, const Vector * pDirection, void * pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk );
 // void __stdcall hkFrameStageNotify(ClientFrameStage_t stage);
 // void __stdcall hkPlaySound( const char* name );
 // void __fastcall hkDrawModelExecute(void* _this, int, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld);
 // void __stdcall hkOverrideView( CViewSetup * vsView );
 // void __stdcall hkLockCursor( );
  //int  __stdcall hkDoPostScreenEffects( int a1 );
  //bool __fastcall hkSvCheatsGetBool( PVOID pConVar, void* edx );
  //bool __stdcall hkIsPlayingDemo( );
 // void* __stdcall hkGetDemoPlaybackParameters( );
  //EGCResults __fastcall hkRetrieveMessageGC(void* ecx, void* edx, uint32* punMsgType, void* pubDest, uint32 cubDest, uint32_t* pcubMsgSize);
  //EGCResults __fastcall hkSendMessageGC(void* ecx, void* edx, uint32_t unMsgType, const void* pubData, uint32_t cubData);
 // bool __stdcall hkFireEvent( IGameEvent* pEvent );
  //void hkRecvProxy( const CRecvProxyData* pData, void* entity, void* output );
  //int __fastcall hkListLeavesInBox(void* bsp, void* edx, Vector& mins, Vector& maxs, unsigned short* pList, int listMax);
  void __stdcall hkSuppressLists( int a2, bool a3 );
 // bool _fastcall send_net_msg(void* ecx, void* edx, INetMessage* msg, bool reliable, bool voice);
  void __fastcall hkPostEntityPacketReceived( void* pPrediction, void* pEDX );
  void __fastcall hkReadPacketEntities( void* pClientState, void* pEDX, void* pEntInfo );
}
