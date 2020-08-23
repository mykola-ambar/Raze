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

#ifndef __exhumed_h__
#define __exhumed_h__

#include "compat.h"
#include "baselayer.h"
#include "v_text.h"
#include "printf.h"
#include "gamecvars.h"
#include "m_argv.h"
#include "gamecontrol.h"
#include "c_buttons.h"
#include <algorithm>
#include "tarray.h"
#include "zstring.h"
#include "filesystem.h"

BEGIN_PS_NS

// Order is that of EDuke32 by necessity because it exposes the key binds to scripting  by index instead of by name.
enum GameFunction_t
{
	gamefunc_Move_Forward,
	gamefunc_Move_Backward,
	gamefunc_Turn_Left,
	gamefunc_Turn_Right,
	gamefunc_Strafe,
	gamefunc_Fire,
	gamefunc_Open,
	gamefunc_Run,
	gamefunc_Alt_Fire,	// Duke3D, Blood
	gamefunc_Jump,
	gamefunc_Crouch,
	gamefunc_Look_Up,
	gamefunc_Look_Down,
	gamefunc_Look_Left,
	gamefunc_Look_Right,
	gamefunc_Strafe_Left,
	gamefunc_Strafe_Right,
	gamefunc_Aim_Up,
	gamefunc_Aim_Down,
	gamefunc_Weapon_1,
	gamefunc_Weapon_2,
	gamefunc_Weapon_3,
	gamefunc_Weapon_4,
	gamefunc_Weapon_5,
	gamefunc_Weapon_6,
	gamefunc_Weapon_7,
	gamefunc_Weapon_8,
	gamefunc_Weapon_9,
	gamefunc_Weapon_10,
	gamefunc_Inventory,
	gamefunc_Inventory_Left,
	gamefunc_Inventory_Right,
	gamefunc_TurnAround,
	gamefunc_SendMessage,
	gamefunc_Map,
	gamefunc_Shrink_Screen,
	gamefunc_Enlarge_Screen,
	gamefunc_Center_View,
	gamefunc_Holster_Weapon,
	gamefunc_Show_Opponents_Weapon,
	gamefunc_Map_Follow_Mode,
	gamefunc_See_Coop_View,
	gamefunc_Mouse_Aiming,
	gamefunc_Toggle_Crosshair,
	gamefunc_Next_Weapon,
	gamefunc_Previous_Weapon,
	gamefunc_Dpad_Select,
	gamefunc_Dpad_Aiming,
	gamefunc_Last_Weapon,
	gamefunc_Alt_Weapon,
	gamefunc_Third_Person_View,
	gamefunc_Toggle_Crouch,	// This is the last one used by EDuke32.
    NUM_ACTIONS,

};

#define kTimerTicks		120

#ifdef __WATCOMC__
void handleevents();
#endif

enum basepal_t {
    BASEPAL = 0,
    ANIMPAL,
    BASEPALCOUNT
};

void ExitGame();
void ShutDown(void);
void DebugOut(const char *fmt, ...);
int ExhumedMain(int argc, char *argv[]);

void FinishLevel();

void SetHiRes();

void BlackOut();

void DoGameOverScene();

int Query(short n, short l, ...);

extern unsigned char curpal[];

void TintPalette(int a, int b, int c);
//void MySetPalette(unsigned char *palette);
//void GetCurPal(unsigned char *palette);

void EraseScreen(int eax);

void RestorePalette();

void FadeIn();
void FadeOut(int bFadeMusic);

void mychangespritesect(int nSprite, int nSector);
void mydeletesprite(int nSprite);

void GrabPalette();

void mysetbrightness(char nBrightness);

void StartFadeIn();
int DoFadeIn();

void InitSpiritHead();

// TODO - relocate
void StatusMessage(int messageTime, const char *fmt, ...);

int DoSpiritHead();

void HandleAsync();

extern bool EndLevel;
extern int32_t g_commandSetup;
extern int32_t g_noSetup;

extern char sHollyStr[];

extern int localclock;

extern int moveframes;

extern int nNetPlayerCount;

extern int htimer;

extern int nNetTime;

extern short nTotalPlayers;

extern short nFontFirstChar;
extern short nBackgroundPic;
extern short nShadowPic;

extern short nCreaturesLeft;

extern int lLocalButtons;

extern short nEnergyTowers;

extern short nEnergyChan;

extern short nSpiritSprite;

extern short bInDemo;

extern short nFreeze;

extern short nCurBodyNum;
extern short nBodyTotal;

extern short bSnakeCam;

extern short levelnum;
//extern short nScreenWidth;
//extern short nScreenHeight;

extern short nMapMode;

extern short nButtonColor;

extern short nHeadStage;

extern short lastfps;

extern int flash;

extern short nLocalSpr;
extern short levelnew;

extern short nSnakeCam;

extern short bCoordinates;

extern short bHolly;

extern int totalmoves;

extern int lCountDown;

extern short bSlipMode;

extern int bVanilla;

#define POWERSLAVE  (g_gameType & GAMEFLAG_POWERSLAVE)
#define EXHUMED     (g_gameType & GAMEFLAG_EXHUMED)
#define ISDEMOVER   (g_gameType & GAMEFLAG_SHAREWARE)

extern double g_frameDelay;

enum {
    kPalNormal = 0,
    kPalNoDim,
    kPalTorch,
    kPalNoTorch,
    kPalBrite,
    kPalRedBrite,
    kPalGreenBrite,
    kPalNormal2,
    kPalNoDim2,
    kPalTorch2,
    kPalNoTorch2,
    kPalBrite2
};


class TextOverlay
{
	double nCrawlY;
	short nLeft[50];
	int nHeight;
    int lastclock;
	TArray<FString> screentext;


public:

	void Start(int starttime);
	void ComputeCinemaText();
	void ReadyCinemaText(uint16_t nVal);
	void DisplayText();
	bool AdvanceCinemaText(int totalclock);
};


extern char g_modDir[BMAX_PATH];

void G_LoadGroupsInDir(const char* dirname);
void G_DoAutoload(const char* dirname);
void DrawRel(int tile, double x, double y, int shade = 0);

// savegame.

int savegame(int nSlot);
int loadgame(int nSlot);

const uint32_t kSpiritX = 106;
const uint32_t kSpiritY = 97;
const uint32_t WorktileSize = kSpiritX * 2 * kSpiritY * 2;

struct SavegameHelper
{
    FString Name;
    TArray<std::pair<void*, size_t>> Elements;
    SavegameHelper(const char* name, ...);
    void Load();
    void Save();
};

#define SV(v) &v, sizeof(v)
#define SA(a) &a, sizeof(a)



struct GameInterface : ::GameInterface
{
    const char* Name() override { return "Exhumed"; }
    int app_main() override;
    bool GenerateSavePic() override;
    void DrawNativeMenuText(int fontnum, int state, double xpos, double ypos, float fontscale, const char* text, int flags) override;
    void MenuOpened() override;
    void MenuSound(EMenuSounds snd) override;
    void MenuClosed() override;
    void StartGame(FNewGameStartup& gs) override;
    FSavegameInfo GetSaveSig() override;
    void DrawMenuCaption(const DVector2& origin, const char* text) override;
    bool LoadGame(FSaveGameNode* sv) override;
    bool SaveGame(FSaveGameNode* sv) override;
    bool CanSave() override;
    ReservedSpace GetReservedScreenSpace(int viewsize) override { return { 0, 24 }; }

    FString statFPS() override;
    //GameStats getStats() override;
};


END_PS_NS

#endif
