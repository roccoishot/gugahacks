#pragma once

#include "../Misc/CUserCmd.hpp"

#define MULTIPLAYER_BACKUP 150

class bf_write;
class bf_read;

class CInput
{
public:
    std::byte            pad0 [ 0xC ];            // 0x0
    bool                bTrackIRAvailable;        // 0xC
    bool                bMouseInitialized;        // 0xD
    bool                bMouseActive;            // 0xE
    std::byte            pad1 [ 0x9A ];            // 0xF
    bool                m_fCameraInThirdPerson;    // 0xA9
    std::byte            pad2 [ 0x2 ];            // 0xAA
    Vector                vecCameraOffset;        // 0xAC
    std::byte            pad3 [ 0x38 ];            // 0xB8
    CUserCmd*            m_pCommands;            // 0xF0
    CVerifiedUserCmd*    m_pVerifiedCommands;    // 0xF4

    CUserCmd* GetUserCmd(int sequence_number)
    {
        return &m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
    }

    CVerifiedUserCmd* GetVerifiedUserCmd(int sequence_number)
    {
        return &m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];
    }
};
