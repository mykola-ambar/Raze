//-------------------------------------------------------------------------
/*
Copyright (C) 2010-2019 EDuke32 developers and contributors
Copyright (C) 2019 Nuke.YKT

This file is part of NBlood.

NBlood is free software; you can redistribute it and/or
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
#include "ns.h"	// Must come before everything else!

#include "build.h"
#include "mmulti.h"
#include "common_game.h"
#include "levels.h"
#include "map2d.h"
#include "view.h"
#include "v_2ddrawer.h"
#include "v_draw.h"
#include "statusbar.h"

BEGIN_BLD_NS

void sub_2541C(int x, int y, int z, short a)
{
    int tmpydim = (xdim * 5) / 8;
    renderSetAspect(65536, divscale16(tmpydim * 320, xdim * 200));
    int nCos = z*sintable[(0-a)&2047];
    int nSin = z*sintable[(1536-a)&2047];
    int nCos2 = mulscale16(nCos, yxaspect);
    int nSin2 = mulscale16(nSin, yxaspect);
    for (int i = 0; i < numsectors; i++)
    {
        if (gFullMap || show2dsector[i])
        {
            int nStartWall = sector[i].wallptr;
            int nEndWall = nStartWall+sector[i].wallnum;
            int nZCeil = sector[i].ceilingz;
            int nZFloor = sector[i].floorz;
            walltype *pWall = &wall[nStartWall];
            for (int j = nStartWall; j < nEndWall; j++, pWall++)
            {
                int nNextWall = pWall->nextwall;
                if (nNextWall < 0)
                    continue;
                if (sector[pWall->nextsector].ceilingz == nZCeil && sector[pWall->nextsector].floorz == nZFloor
                    && ((wall[nNextWall].cstat | pWall->cstat) & 0x30) == 0)
                    continue;
                if (gFullMap || show2dsector[pWall->nextsector])
                    continue;
                int wx = pWall->x-x;
                int wy = pWall->y-y;
                int cx = xdim<<11;
                int x1 = cx+dmulscale16(wx, nCos, -wy, nSin);
                int cy = ydim<<11;
                int y1 = cy+dmulscale16(wy, nCos2, wx, nSin2);
                walltype *pWall2 = &wall[pWall->point2];
                wx = pWall2->x-x;
                wy = pWall2->y-y;
                int x2 = cx+dmulscale16(wx, nCos, -wy, nSin);
                int y2 = cy+dmulscale16(wy, nCos2, wx, nSin2);
                renderDrawLine(x1,y1,x2,y2,24);
            }
        }
    }
    int nPSprite = gView->pSprite->index;
    for (int i = 0; i < numsectors; i++)
    {
        if (gFullMap || show2dsector[i])
        {
            for (int nSprite = headspritesect[i]; nSprite >= 0; nSprite = nextspritesect[nSprite])
            {
                spritetype *pSprite = &sprite[nSprite];
                if (nSprite == nPSprite)
                    continue;
                if (pSprite->cstat&32768)
                    continue;
            }
        }
    }
    for (int i = 0; i < numsectors; i++)
    {
        if (gFullMap || show2dsector[i])
        {
            int nStartWall = sector[i].wallptr;
            int nEndWall = nStartWall+sector[i].wallnum;
            walltype *pWall = &wall[nStartWall];
            int nNWall = -1;
            int x1, y1, x2 = 0, y2 = 0;
            for (int j = nStartWall; j < nEndWall; j++, pWall++)
            {
                int nNextWall = pWall->nextwall;
                if (nNextWall >= 0)
                    continue;
                if (!tilesiz[pWall->picnum].x || !tilesiz[pWall->picnum].y)
                    continue;
                if (nNWall == j)
                {
                    x1 = x2;
                    y1 = y2;
                }
                else
                {
                    int wx = pWall->x-x;
                    int wy = pWall->y-y;
                    x1 = (xdim<<11)+dmulscale16(wx, nCos, -wy, nSin);
                    y1 = (ydim<<11)+dmulscale16(wy, nCos2, wx, nSin2);
                }
                nNWall = pWall->point2;
                walltype *pWall2 = &wall[nNWall];
                int wx = pWall2->x-x;
                int wy = pWall2->y-y;
                x2 = (xdim<<11)+dmulscale16(wx, nCos, -wy, nSin);
                y2 = (ydim<<11)+dmulscale16(wy, nCos2, wx, nSin2);
                renderDrawLine(x1,y1,x2,y2,24);
            }
        }
    }
    videoSetCorrectedAspect();

    for (int i = connecthead; i >= 0; i = connectpoint2[i])
    {
        if (automapFollow || gView->nPlayer != i)
        {
            PLAYER *pPlayer = &gPlayer[i];
            spritetype *pSprite = pPlayer->pSprite;
            int px = pSprite->x-x;
            int py = pSprite->y-y;
            int pa = (pSprite->ang-a)&2047;
            if (i == gView->nPlayer)
            {
                px = 0;
                py = 0;
                pa = 0;
            }
            int x1 = dmulscale16(px, nCos, -py, nSin);
            int y1 = dmulscale16(py, nCos2, px, nSin2);
            if (i == gView->nPlayer || gGameOptions.nGameType == 1)
            {
                int nTile = pSprite->picnum;
                int ceilZ, ceilHit, floorZ, floorHit;
                GetZRange(pSprite, &ceilZ, &ceilHit, &floorZ, &floorHit, (pSprite->clipdist<<2)+16, CLIPMASK0, PARALLAXCLIP_CEILING|PARALLAXCLIP_FLOOR);
                int nTop, nBottom;
                GetSpriteExtents(pSprite, &nTop, &nBottom);
                int nScale = mulscale((pSprite->yrepeat+((floorZ-nBottom)>>8))*z, yxaspect, 16);
                nScale = ClipRange(nScale, 8000, 65536<<1);
				// Players on automap
				double x = xdim/2. + x1 / double(1<<12);
				double y = ydim/2. + y1 / double(1<<12);
				// This very likely needs fixing later
				DrawTexture(twod, tileGetTexture(nTile, true), x, y, DTA_FullscreenScale, FSMode_Fit320x200, DTA_ViewportX, windowxy1.x, DTA_ViewportY, windowxy1.y,
							DTA_ViewportWidth, windowxy2.x - windowxy1.x+1, DTA_ViewportHeight, windowxy2.y - windowxy1.y+1, DTA_Alpha, (pSprite->cstat&2? 0.5:1.), TAG_DONE);
            }
        }
    }
}

void CViewMap::sub_25C38(int _x, int _y, int _angle, short zoom)
{
    x = _x;
    y = _y;
    angle = _angle;
    nZoom = zoom;
    forward = 0;
    turn = 0;
    strafe = 0;
}

void CViewMap::sub_25C74(void)
{
    int tm = 0;
    if (windowxy1.x > 0)
    {
        setViewport(Hud_Stbar);
        tm = 1;
    }
    // only clear the actual window.
    twod->AddColorOnlyQuad(windowxy1.x, windowxy1.y, (windowxy2.x + 1) - windowxy1.x, (windowxy2.y + 1) - windowxy1.y, 0xff000000);
    renderDrawMapView(x,y,nZoom>>2,angle);
    sub_2541C(x,y,nZoom>>2,angle);
    if (tm)
        setViewport(hud_size);
}

void CViewMap::sub_25DB0(spritetype *pSprite)
{
    nZoom = gZoom;
    if (automapFollow)
    {
        x = pSprite->x;
        y = pSprite->y;
        angle = pSprite->ang;
    }
    else
    {
        angle += FixedToInt(turn)>>3;
        x += mulscale24(forward>>8, Cos(angle));
        y += mulscale24(forward>>8, Sin(angle));
        x -= mulscale24(strafe>>8, Cos(angle+512));
        y -= mulscale24(strafe>>8, Sin(angle+512));
        forward = 0;
        strafe = 0;
        turn = 0;
    }
    sub_25C74();
}

void CViewMap::sub_25E84(int *_x, int *_y)
{
    if (_x)
        *_x = x;
    if (_y)
        *_y = y;
}


CViewMap gViewMap;


END_BLD_NS
