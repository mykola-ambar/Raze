#pragma once

bool System_WantGuiCapture();	// During playing this tells us whether the game must be paused due to active GUI elememts.

#include <stdint.h>
#include "vectors.h"
#include "engineerrors.h"
#include "stats.h"
#include "packet.h"
#include "inputstate.h"

class FSerializer;

struct GameStats
{
	int kill, tkill;
	int secret, tsecret;
	int timesecnd;
	int frags;
};

struct FNewGameStartup
{
	int Episode;
	int Level;
	int Skill;
};

struct FSavegameInfo
{
	const char *savesig;
	int minsavever;
	int currentsavever;
};

struct ReservedSpace
{
	int top;
	int statusbar;
};

enum EMenuSounds : int;
struct MapRecord;

extern cycle_t drawtime, actortime, thinktime, gameupdatetime;

struct GameInterface
{
	virtual const char* Name() { return "$"; }
	virtual ~GameInterface() {}
	virtual bool GenerateSavePic() { return false; }
	virtual void app_init() = 0;
	virtual void loadPalette();
	virtual void clearlocalinputstate() {}
	virtual void UpdateScreenSize() {}
	virtual void FreeLevelData();
	virtual void FreeGameData() {}
	virtual void PlayHudSound() {}
	virtual GameStats getStats() { return {}; }
	virtual void MainMenuOpened() {}
	virtual void MenuOpened() {}
	virtual void MenuClosed() {}
	virtual void MenuSound(EMenuSounds snd) {}
	virtual bool CanSave() { return true; }
	virtual void CustomMenuSelection(int menu, int item) {}
	virtual bool StartGame(FNewGameStartup& gs) { return false; }
	virtual FSavegameInfo GetSaveSig() { return { "", 0, 0}; }
	virtual double SmallFontScale() { return 1; }
	virtual bool SaveGame() { return true; }
	virtual bool LoadGame() { return true; }
	virtual void SerializeGameState(FSerializer& arc) {}
	virtual void DrawPlayerSprite(const DVector2& origin, bool onteam) {}
	virtual void QuitToTitle() {}
	virtual void SetAmbience(bool on) {}
	virtual FString GetCoordString() { return "'stat coord' not implemented"; }
	virtual void ExitFromMenu() { throw CExitEvent(0); }
	virtual ReservedSpace GetReservedScreenSpace(int viewsize) { return { 0, 0 }; }
	virtual void GetInput(InputPacket* packet, ControlInfo* const hidInput) {}
	virtual void UpdateSounds() {}
	virtual void ErrorCleanup() {}
	virtual void Startup() {}
	virtual void DrawBackground() {}
	virtual void Render() {}
	virtual void Ticker() {}
	virtual int GetPlayerChecksum(int pnum) { return 0x12345678 + pnum; }
	virtual const char *CheckCheatMode() { return nullptr; }
	virtual const char* GenericCheat(int player, int cheat) = 0;
	virtual void NextLevel(MapRecord* map, int skill) {}
	virtual void NewGame(MapRecord* map, int skill) {}
	virtual void LevelCompleted(MapRecord* map, int skill) {}
	virtual bool DrawAutomapPlayer(int x, int y, int z, int a) { return false; }
	virtual void SetTileProps(int tile, int surf, int vox, int shade) {}
	virtual fixed_t playerHorizMin() { return IntToFixed(-200); }
	virtual fixed_t playerHorizMax() { return IntToFixed(200); }
	virtual int playerKeyMove() { return 0; }
	virtual void WarpToCoords(int x, int y, int z, int a, int h) {}
	virtual void ToggleThirdPerson() { }
	virtual void SwitchCoopView() { Printf("Unsupported command\n"); }
	virtual void ToggleShowWeapon() { Printf("Unsupported command\n"); }

	virtual FString statFPS()
	{
		FString output;

		output.AppendFormat("Actor think time: %.3f ms\n", actortime.TimeMS());
		output.AppendFormat("Total think time: %.3f ms\n", thinktime.TimeMS());
		output.AppendFormat("Game Update: %.3f ms\n", gameupdatetime.TimeMS());
		output.AppendFormat("Draw time: %.3f ms\n", drawtime.TimeMS());

		return output;
	}


};

extern GameInterface* gi;


void ImGui_Begin_Frame();

