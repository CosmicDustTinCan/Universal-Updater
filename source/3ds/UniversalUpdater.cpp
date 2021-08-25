/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2021 Universal-Team
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "UniversalUpdater.hpp"
#include <dirent.h>


/*
	Initialize everything as needed.
*/
void UU::Initialize() {
	romfsInit();
	gfxInitDefault();
	Gui::init();
	hidSetRepeatParameters(20, 8);

	/* Create Directories. */
	mkdir("sdmc:/3ds", 0x777);
	mkdir("sdmc:/3ds/Universal-Updater", 0x777);
	mkdir("sdmc:/3ds/Universal-Updater/stores", 0x777);
	mkdir("sdmc:/3ds/Universal-Updater/shortcuts", 0x777);
	
	this->GData = std::make_unique<GFXData>();
	this->CData = std::make_unique<ConfigData>();
	this->TData = std::make_unique<ThemeData>();
	this->Store = std::make_unique<UniStore>("romfs:/test.unistore", "test.unistore");

	this->_Tabs = std::make_unique<Tabs>();
	this->TList = std::make_unique<TopList>();
};


/*
	Scan the key input.
*/
void UU::ScanInput() {
	hidScanInput();
	this->Down = hidKeysDown();
	this->Repeat = hidKeysDownRepeat();
	hidTouchRead(&this->T);
};


/*
	Draws Universal-Updater's UI.
*/
void UU::Draw() {
	Gui::clearTextBufs();
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

	this->GData->DrawTop();

	/* Ensure it isn't a nullptr. */
	if (this->Store) {
		Gui::DrawStringCentered(0, 3, TEXT_LARGE, TEXT_WHITE, this->Store->GetUniStoreTitle(), 390);
		this->TList->Draw();
		this->_Tabs->DrawTop();

	} else {
		Gui::DrawStringCentered(0, 3, TEXT_LARGE, TEXT_WHITE, "Invalid UniStore", 390);
	}

	this->GData->DrawBottom();
	this->_Tabs->DrawBottom();

	C3D_FrameEnd(0);
};


/*
	Main Handler of the app. Handle Input and display stuff here.
*/
int UU::Handler() {
	this->Initialize();

	while(aptMainLoop() && !this->Exiting) {
		gspWaitForVBlank();

		this->Draw();
		this->ScanInput();

		/* Handle Top List if possible. */
		if (this->_Tabs->HandleTopScroll()) this->TList->Handler();
		this->_Tabs->Handler(); // Tabs are always handled.
	}

	this->CData->Sav();
	Gui::exit();
	gfxExit();
	romfsExit();

	return 0;
};


bool UU::Touched(const Structs::ButtonPos Pos) const {
	return ((this->T.px >= Pos.x && this->T.px <= (Pos.x + Pos.w)) && (this->T.py >= Pos.y && this->T.py <= (Pos.y + Pos.h)));
};