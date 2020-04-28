/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "file_formats.h"

//*********************************************************
//****************Sctructure initalizers*******************
//*********************************************************

saveCharState FileFormats::CreateSavCharacterState()
{
    saveCharState newData;

    newData.id          = 1;
    newData.health      = 0;
    newData.itemID      = 0;
    newData.mountID     = 0;
    newData.mountType   = 0;
    newData.state       = 1;

    return newData;
}


GamesaveData FileFormats::CreateGameSaveData()
{
    GamesaveData newData;

    newData.meta.RecentFormatVersion = 0;

    newData.lives = 3;
    newData.coins = 0;
    newData.points = 0;
    newData.totalStars = 0;

    newData.last_hub_warp = 0;

    newData.musicID = 0;
    newData.musicFile = "";

    newData.worldPosX = 0;
    newData.worldPosY = 0;
    newData.gameCompleted = false;

    newData.characterStates.push_back(CreateSavCharacterState());
    newData.currentCharacter.push_back(1);

    return newData;
}
