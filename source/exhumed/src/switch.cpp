//-------------------------------------------------------------------------
/*
Copyright (C) 2010-2019 EDuke32 developers and contributors
Copyright (C) 2019 sirlemonhead, Nuke.YKT
This file is part of PCExhumed.
PCExhumed is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License version 2
as published by the Free Software Foundation.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
//-------------------------------------------------------------------------
#include "ns.h"
#include "aistuff.h"
#include "exhumed.h"
#include "engine.h"
#include "player.h"
#include "sound.h"
#include <string.h>
#include <assert.h>

BEGIN_PS_NS

struct Link
{
    int8_t v[8];
};


struct Switch
{
    short nWaitTimer;
    short nWait;
    short nChannel;
    short nLink;
    short nRunPtr;
    short nSector;
    short nRun2;
    short nWall;
    short nRun3;
    uint16_t nKeyMask;
};

TArray<Link> LinkMap;
TArray<Switch> SwitchData;

FSerializer& Serialize(FSerializer& arc, const char* keyname, Link& w, Link* def)
{
    arc.Array(keyname, w.v, 8);
    return arc;
}

FSerializer& Serialize(FSerializer& arc, const char* keyname, Switch& w, Switch* def)
{
    if (arc.BeginObject(keyname))
    {
        arc("waittimer", w.nWaitTimer)
            ("wait", w.nWait)
            ("channel", w.nChannel)
            ("link", w.nLink)
            ("runptr", w.nRunPtr)
            ("sector", w.nSector)
            ("run2", w.nRun2)
            ("wall", w.nWall)
            ("run3", w.nRun3)
            ("keymask", w.nKeyMask)
            .EndObject();
    }
    return arc;
}

void SerializeSwitch(FSerializer& arc)
{
    arc("switch", SwitchData)
        ("linkmap", LinkMap);
}

void InitLink()
{
    LinkMap.Clear();
}

int BuildLink(int nCount, ...)
{
    va_list list;
    va_start(list, nCount);

    unsigned LinkCount = LinkMap.Reserve(1);

    for (int i = 0; i < 8; i++)
    {
        int ebx;

        if (i >= nCount)
        {
            ebx = -1;
        }
        else
        {
            ebx = va_arg(list, int);
        }

        LinkMap[LinkCount].v[i] = (int8_t)ebx;
    }
    va_end(list);

    return LinkCount;
}

void InitSwitch()
{
    SwitchData.Clear();
}

int BuildSwReady(int nChannel, short nLink)
{
    if (nLink < 0) {
        I_Error("Too many switch readys!\n");
        return -1;
    }
    unsigned SwitchCount = SwitchData.Reserve(1);
    SwitchData[SwitchCount].nChannel = nChannel;
    SwitchData[SwitchCount].nLink = nLink;
    return SwitchCount | 0x10000;
}

void FuncSwReady(int a, int, int nRun)
{
    short nSwitch = RunData[nRun].nVal;
    assert(nSwitch >= 0 && nSwitch < (int)SwitchData.Size());

    int nMessage = a & 0x7F0000;

    short nChannel = SwitchData[nSwitch].nChannel;
    short nLink = SwitchData[nSwitch].nLink;

    switch (nMessage)
    {
        case 0x10000:
            return;

        case 0x30000:
        {
            assert(sRunChannels[nChannel].c < 8);
            int8_t nVal = LinkMap[nLink].v[sRunChannels[nChannel].c];
            if (nVal >= 0) {
                runlist_ChangeChannel(nChannel, nVal);
            }

            break;
        }

        default:
            return;
    }
}

int BuildSwPause(int nChannel, int nLink, int ebx)
{
    for (unsigned i = 0; i < SwitchData.Size(); i++)
    {
        if (SwitchData[i].nChannel == nChannel && SwitchData[i].nWait != 0) {
            return i | 0x20000;
        }
    }

    if (nLink < 0) {
        I_Error("Too many switches!\n");
        return -1;
    }
    unsigned SwitchCount = SwitchData.Reserve(1);

    SwitchData[SwitchCount].nChannel = nChannel;
    SwitchData[SwitchCount].nLink = nLink;
    SwitchData[SwitchCount].nWait = ebx;
    SwitchData[SwitchCount].nRunPtr = -1;

    return SwitchCount | 0x20000;
}

void FuncSwPause(int a, int, int nRun)
{
    short nSwitch = RunData[nRun].nVal;
    assert(nSwitch >= 0 && nSwitch < (int)SwitchData.Size());

    int nMessage = a & 0x7F0000;

    short nChannel = SwitchData[nSwitch].nChannel;
    short nLink = SwitchData[nSwitch].nLink;

    switch (nMessage)
    {
        default:
            return;

        case 0x10000:
        {
            if (SwitchData[nSwitch].nRunPtr >= 0)
            {
                runlist_SubRunRec(SwitchData[nSwitch].nRunPtr);
                SwitchData[nSwitch].nRunPtr = -1;
            }

            return;
        }

        case 0x20000:
        {
            SwitchData[nSwitch].nWaitTimer--;
            if (SwitchData[nSwitch].nWaitTimer <= 0)
            {
                runlist_SubRunRec(SwitchData[nSwitch].nRunPtr);
                SwitchData[nSwitch].nRunPtr = -1;

                assert(sRunChannels[nChannel].c < 8);
                assert(nLink < 1024);

                runlist_ChangeChannel(nChannel, LinkMap[nLink].v[sRunChannels[nChannel].c]);
            }

            return;
        }

        case 0x30000:
        {
            assert(sRunChannels[nChannel].c < 8);

            if (LinkMap[nLink].v[sRunChannels[nChannel].c] < 0) {
                return;
            }

            if (SwitchData[nSwitch].nRunPtr >= 0) {
                return;
            }

            SwitchData[nSwitch].nRunPtr = runlist_AddRunRec(NewRun, RunData[nRun].nMoves);

            int eax;

            if (SwitchData[nSwitch].nWait <= 0)
            {
                eax = 100;
            }
            else
            {
                eax = SwitchData[nSwitch].nWait;
            }

            SwitchData[nSwitch].nWaitTimer = eax;
            return;
        }
    }
}

int BuildSwStepOn(int nChannel, int nLink, int nSector)
{
    if (nLink < 0 || nSector < 0)
        I_Error("Too many switches!\n");
    unsigned nSwitch = SwitchData.Reserve(1);

    SwitchData[nSwitch].nChannel = nChannel;
    SwitchData[nSwitch].nLink = nLink;
    SwitchData[nSwitch].nSector = nSector;
    SwitchData[nSwitch].nRun2 = -1;

    return nSwitch | 0x30000;
}

void FuncSwStepOn(int a, int, int nRun)
{
    short nSwitch = RunData[nRun].nVal;
    assert(nSwitch >= 0 && nSwitch < (int)SwitchData.Size());

    short nLink = SwitchData[nSwitch].nLink;
    short nChannel = SwitchData[nSwitch].nChannel;
    short nSector = SwitchData[nSwitch].nSector;

    assert(sRunChannels[nChannel].c < 8);

    int8_t var_14 = LinkMap[nLink].v[sRunChannels[nChannel].c];

    int nMessage = a & 0x7F0000;

    switch (nMessage)
    {
        default:
            return;

        case 0x10000:
        {
            if (SwitchData[nSwitch].nRun2 >= 0)
            {
                runlist_SubRunRec(SwitchData[nSwitch].nRun2);
                SwitchData[nSwitch].nRun2 = -1;
            }

            if (var_14 >= 0)
            {
                SwitchData[nSwitch].nRun2 = runlist_AddRunRec(sector[nSector].lotag - 1, RunData[nRun].nMoves);
            }

            return;
        }

        case 0x50000:
        {
            if (var_14 != sRunChannels[nChannel].c)
            {
                short nWall = sector[nSector].wallptr;
                PlayFXAtXYZ(StaticSound[nSwitchSound], wall[nWall].x, wall[nWall].y, sector[nSector].floorz, nSector);

                assert(sRunChannels[nChannel].c < 8);

                runlist_ChangeChannel(nChannel, LinkMap[nLink].v[sRunChannels[nChannel].c]);
            }
        }

        return;
    }

}

int BuildSwNotOnPause(int nChannel, int nLink, int nSector, int ecx)
{
    if (nLink < 0 || nSector < 0)
        I_Error("Too many switches!\n");
    unsigned nSwitch = SwitchData.Reserve(1);
    SwitchData[nSwitch].nChannel = nChannel;
    SwitchData[nSwitch].nLink    = nLink;
    SwitchData[nSwitch].nWait  = ecx;
    SwitchData[nSwitch].nSector  = nSector;
    SwitchData[nSwitch].nRunPtr  = -1;
    SwitchData[nSwitch].nRun2  = -1;

    return nSwitch | 0x40000;
}

void FuncSwNotOnPause(int a, int, int nRun)
{
    short nSwitch = RunData[nRun].nVal;
    assert(nSwitch >= 0 && nSwitch < (int)SwitchData.Size());

    int nMessage = a & 0x7F0000;

    short nChannel = SwitchData[nSwitch].nChannel;
    short nLink = SwitchData[nSwitch].nLink;

    switch (nMessage)
    {
        default:
            return;

        case 0x10000:
        {
            if (SwitchData[nSwitch].nRun2 >= 0)
            {
                runlist_SubRunRec(SwitchData[nSwitch].nRun2);
                SwitchData[nSwitch].nRun2 = -1;
            }

            if (SwitchData[nSwitch].nRunPtr >= 0)
            {
                runlist_SubRunRec(SwitchData[nSwitch].nRunPtr);
                SwitchData[nSwitch].nRunPtr = -1;
            }

            return;
        }

        case 0x20000:
        {
            SwitchData[nSwitch].nWaitTimer -= 4;
            if (SwitchData[nSwitch].nWaitTimer <= 0)
            {
                assert(sRunChannels[nChannel].c < 8);

                runlist_ChangeChannel(nChannel, LinkMap[nLink].v[sRunChannels[nChannel].c]);
            }

            return;
        }

        case 0x30000:
        {
            assert(sRunChannels[nChannel].c < 8);

            if (LinkMap[nLink].v[sRunChannels[nChannel].c] >= 0)
            {
                if (SwitchData[nSwitch].nRunPtr < 0)
                {
                    SwitchData[nSwitch].nRunPtr = runlist_AddRunRec(NewRun, RunData[nRun].nMoves);

                    short nSector = SwitchData[nSwitch].nSector;

                    SwitchData[nSwitch].nWaitTimer = SwitchData[nSwitch].nWait;
                    SwitchData[nSwitch].nRun2 = runlist_AddRunRec(sector[nSector].lotag - 1, RunData[nRun].nMoves);
                }
            }

            return;
        }

        case 0x50000:
        {
            SwitchData[nSwitch].nWaitTimer = SwitchData[nSwitch].nWait;
            return;
        }
    }
}

int BuildSwPressSector(int nChannel, int nLink, int nSector, int keyMask)
{
    if (nLink < 0 || nSector < 0)
        I_Error("Too many switches!\n");
    unsigned nSwitch = SwitchData.Reserve(1);
    SwitchData[nSwitch].nChannel = nChannel;
    SwitchData[nSwitch].nLink = nLink;
    SwitchData[nSwitch].nSector = nSector;
    SwitchData[nSwitch].nKeyMask = keyMask;
    SwitchData[nSwitch].nRun2 = -1;

    return nSwitch | 0x50000;
}

void FuncSwPressSector(int a, int, int nRun)
{
    short nSwitch = RunData[nRun].nVal;
    assert(nSwitch >= 0 && nSwitch < (int)SwitchData.Size());

    int nMessage = a & 0x7F0000;

    short nChannel = SwitchData[nSwitch].nChannel;
    short nLink = SwitchData[nSwitch].nLink;
    short nPlayer = a & 0xFFFF;

    switch (nMessage)
    {
        default:
            return;

        case 0x10000:
        {
            if (SwitchData[nSwitch].nRun2 >= 0)
            {
                runlist_SubRunRec(SwitchData[nSwitch].nRun2);
                SwitchData[nSwitch].nRun2 = -1;
            }

            assert(sRunChannels[nChannel].c < 8);

            if (LinkMap[nLink].v[sRunChannels[nChannel].c] < 0) {
                return;
            }

            short nSector = SwitchData[nSwitch].nSector;

            SwitchData[nSwitch].nRun2 = runlist_AddRunRec(sector[nSector].lotag - 1, RunData[nRun].nMoves);
            return;
        }

        case 0x40000:
        {
            if ((PlayerList[nPlayer].keys & SwitchData[nSwitch].nKeyMask) == SwitchData[nSwitch].nKeyMask)
            {
                runlist_ChangeChannel(nChannel, LinkMap[nLink].v[sRunChannels[nChannel].c]);
            }
            else
            {
                if (SwitchData[nSwitch].nKeyMask)
                {
                    short nSprite = PlayerList[nPlayer].nSprite;
                    PlayFXAtXYZ(StaticSound[nSwitchSound], sprite[nSprite].x, sprite[nSprite].y, 0, sprite[nSprite].sectnum, CHANF_LISTENERZ);

                    StatusMessage(300, "YOU NEED THE KEY FOR THIS DOOR");
                }
            }
        }
    }
}

int BuildSwPressWall(short nChannel, short nLink, short nWall)
{
    if (nLink < 0 || nWall < 0) {
        I_Error("Too many switches!\n");
    }
    unsigned SwitchCount = SwitchData.Reserve(1);
    SwitchData[SwitchCount].nChannel = nChannel;
    SwitchData[SwitchCount].nLink = nLink;
    SwitchData[SwitchCount].nWall = nWall;
    SwitchData[SwitchCount].nRun3 = -1;

    return SwitchCount | 0x60000;
}

void FuncSwPressWall(int a, int, int nRun)
{
    short nSwitch = RunData[nRun].nVal;
    assert(nSwitch >= 0 && nSwitch < (int)SwitchData.Size());

    short nChannel = SwitchData[nSwitch].nChannel;
    short nLink = SwitchData[nSwitch].nLink;

    // TEMP
//	assert(nLink < 1024);
//	assert(nChannel < 8);

    int nMessage = a & 0x7F0000;

    switch (nMessage)
    {
        default:
            return;

        case 0x30000:
        {
            if (SwitchData[nSwitch].nRun3 >= 0)
            {
                runlist_SubRunRec(SwitchData[nSwitch].nRun3);
                SwitchData[nSwitch].nRun3 = -1;
            }

            if (LinkMap[nLink].v[sRunChannels[nChannel].c] >= 0)
            {
                short nWall = SwitchData[nSwitch].nWall;
                SwitchData[nSwitch].nRun3 = runlist_AddRunRec(wall[nWall].lotag - 1, RunData[nRun].nMoves);
            }

            return;
        }

        case 0x40000:
        {
            int8_t cx = LinkMap[nLink].v[sRunChannels[nChannel].c];

            runlist_ChangeChannel(nChannel, LinkMap[nLink].v[sRunChannels[nChannel].c]);

            if (cx < 0 || LinkMap[nLink].v[cx] < 0)
            {
                runlist_SubRunRec(SwitchData[nSwitch].nRun3);
                SwitchData[nSwitch].nRun3 = -1;
            }

            short nWall = SwitchData[nSwitch].nWall;
            short nSector = SwitchData[nSwitch].nSector; // CHECKME - where is this set??

            PlayFXAtXYZ(StaticSound[nSwitchSound], wall[nWall].x, wall[nWall].y, 0, nSector, CHANF_LISTENERZ);

            return;
        }
    }
}
END_PS_NS
