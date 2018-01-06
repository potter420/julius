
#include "AllWindows.h"

#include "../Graphics.h"
#include "../Widget.h"

#include "../Data/CityInfo.h"
#include "../Data/Screen.h"
#include "../Data/State.h"

#include "core/lang.h"
#include "core/string.h"
#include "game/file.h"
#include "game/mission.h"
#include "game/tutorial.h"
#include "scenario/criteria.h"
#include "scenario/property.h"
#include "sound/music.h"
#include "sound/speech.h"

static void startMission(int param1, int param2);
static void briefingBack(int param1, int param2);

static const int backgroundGraphicOffset[] = {
	0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 0
};
static const int campaignHasChoice[] = {
	0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};
struct CampaignSelection {
	int xPeaceful;
	int yPeaceful;
	int xMilitary;
	int yMilitary;
};
static const struct CampaignSelection campaignSelection[12] = {
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{292, 182, 353, 232},
	{118, 202, 324, 286},
	{549, 285, 224, 121},
	{173, 109, 240, 292},
	{576, 283, 19, 316},
	{97, 240, 156, 59},
	{127, 300, 579, 327},
	{103, 35, 410, 109},
	{191, 153, 86, 8},
	{200, 300, 400, 300},
};

static const int goalOffsetsX[] = {32, 288, 32, 288, 288, 288};
static const int goalOffsetsY[] = {95, 95, 117, 117, 73, 135};

static ImageButton imageButtonStartMission = {
	0, 0, 27, 27, ImageButton_Normal, 92, 56, startMission, Widget_Button_doNothing, 1, 0, 1
};
static ImageButton imageButtonBackToSelection = {
	0, 0, 31, 20, ImageButton_Normal, 90, 8, briefingBack, Widget_Button_doNothing, 0, 0, 1
};

static int focusButton = 0;

static struct {
	int choice;
} data;

void UI_MissionStart_show()
{
	int select = 1;
	if (UI_Window_getId() == Window_MissionSelection) {
		select = 0;
	}
	if (!campaignHasChoice[scenario_campaign_rank()]) {
		select = 0;
	}
	if (select) {
		data.choice = 0;
		UI_Window_goTo(Window_MissionSelection);
	} else {
		UI_Intermezzo_show(Intermezzo_MissionBriefing, Window_MissionBriefingInitial, 1000);
		Data_State.missionBriefingShown = 0;
	}
}

void UI_MissionStart_Selection_drawBackground()
{
	int xOffset = Data_Screen.offset640x480.x;
	int yOffset = Data_Screen.offset640x480.y;
	int rank = scenario_campaign_rank();
	
	Graphics_drawFullScreenImage(image_group(GROUP_SELECT_MISSION_BACKGROUND));
	Graphics_drawImage(image_group(GROUP_SELECT_MISSION) +
		backgroundGraphicOffset[rank],
		xOffset, yOffset);
	Widget_GameText_draw(144, 1 + 3 * rank, xOffset + 20, yOffset + 410, FONT_LARGE_BLACK);
	if (data.choice) {
		Widget_GameText_drawMultiline(144, 1 + 3 * rank + data.choice,
			xOffset + 20, yOffset + 440, 560, FONT_NORMAL_BLACK);
	} else {
		Widget_GameText_drawMultiline(144, 0,
			xOffset + 20, yOffset + 440, 560, FONT_NORMAL_BLACK);
	}
}

static int isMouseHit(const mouse *m, int x, int y, int size)
{
	return
		x <= m->x && m->x < x + size &&
		y <= m->y && m->y < y + size;
}

void UI_MissionStart_Selection_drawForeground()
{
	int xOffset = Data_Screen.offset640x480.x;
	int yOffset = Data_Screen.offset640x480.y;

	if (data.choice > 0) {
		Widget_Button_drawImageButtons(xOffset + 580, yOffset + 410, &imageButtonStartMission, 1);
	}

	int rank = scenario_campaign_rank();
	int xPeaceful = xOffset + campaignSelection[rank].xPeaceful - 4;
	int yPeaceful = yOffset + campaignSelection[rank].yPeaceful - 4;
	int xMilitary = xOffset + campaignSelection[rank].xMilitary - 4;
	int yMilitary = yOffset + campaignSelection[rank].yMilitary - 4;
	int graphicId = image_group(GROUP_SELECT_MISSION_BUTTON);
	if (data.choice == 0) {
		Graphics_drawImage(focusButton == 1 ? graphicId + 1 : graphicId, xPeaceful, yPeaceful);
		Graphics_drawImage(focusButton == 2 ? graphicId + 1 : graphicId, xMilitary, yMilitary);
	} else if (data.choice == 1) {
		Graphics_drawImage(focusButton == 1 ? graphicId + 1 : graphicId + 2, xPeaceful, yPeaceful);
		Graphics_drawImage(focusButton == 2 ? graphicId + 1 : graphicId, xMilitary, yMilitary);
	} else {
		Graphics_drawImage(focusButton == 1 ? graphicId + 1 : graphicId, xPeaceful, yPeaceful);
		Graphics_drawImage(focusButton == 2 ? graphicId + 1 : graphicId + 2, xMilitary, yMilitary);
	}
}

void UI_MissionStart_Selection_handleMouse(const mouse *m)
{
	int xOffset = Data_Screen.offset640x480.x;
	int yOffset = Data_Screen.offset640x480.y;

    int rank = scenario_campaign_rank();
    int xPeaceful = xOffset + campaignSelection[rank].xPeaceful - 4;
    int yPeaceful = yOffset + campaignSelection[rank].yPeaceful - 4;
    int xMilitary = xOffset + campaignSelection[rank].xMilitary - 4;
    int yMilitary = yOffset + campaignSelection[rank].yMilitary - 4;
    focusButton = 0;
    if (isMouseHit(m, xPeaceful, yPeaceful, 44)) {
        focusButton = 1;
    }
    if (isMouseHit(m, xMilitary, yMilitary, 44)) {
        focusButton = 2;
    }

	if (m->right.went_up) {
		UI_MissionStart_show();
	}
	if (data.choice > 0) {
		if (Widget_Button_handleImageButtons(xOffset + 580, yOffset + 410, &imageButtonStartMission, 1, 0)) {
			return;
		}
	}
	if (m->left.went_up) {
		if (isMouseHit(m, xPeaceful, yPeaceful, 44)) {
			scenario_set_campaign_mission(game_mission_peaceful());
			data.choice = 1;
			UI_Window_requestRefresh();
			sound_speech_play_file("wavs/fanfare_nu1.wav");
		}
		if (isMouseHit(m, xMilitary, yMilitary, 44)) {
			scenario_set_campaign_mission(game_mission_military());
			data.choice = 2;
			UI_Window_requestRefresh();
			sound_speech_play_file("wavs/fanfare_nu5.wav");
		}
	}
}

void UI_MissionStart_Briefing_init()
{
	Widget_RichText_reset(0);
}

void UI_MissionStart_Briefing_drawBackground()
{
	if (!Data_State.missionBriefingShown) {
		Data_State.missionBriefingShown = 1;
		if (!game_file_start_scenario(scenario_name())) {
            UI_Window_goTo(Window_City);
            return;
		}
	}
	
	int textId = 200 + scenario_campaign_mission();
	int xOffset = Data_Screen.offset640x480.x + 16;
	int yOffset = Data_Screen.offset640x480.y + 32;
	
	Widget_Panel_drawOuterPanel(xOffset, yOffset, 38, 27);
	Widget_Text_draw(lang_get_message(textId)->title.text, xOffset + 16, yOffset + 16, FONT_LARGE_BLACK, 0);
	Widget_Text_draw(lang_get_message(textId)->subtitle.text, xOffset + 16, yOffset + 46, FONT_NORMAL_BLACK, 0);

	Widget_GameText_draw(62, 7, xOffset + 360, yOffset + 401, FONT_NORMAL_BLACK);
	if (UI_Window_getId() == Window_MissionBriefingInitial && campaignHasChoice[scenario_campaign_rank()]) {
		Widget_GameText_draw(13, 4, xOffset + 50, yOffset + 403, FONT_NORMAL_BLACK);
	}
	
	Widget_Panel_drawInnerPanel(xOffset + 16, yOffset + 64, 33, 5);
	Widget_GameText_draw(62, 10, xOffset + 32, yOffset + 72, FONT_NORMAL_WHITE);
	int goalIndex = 0;
	if (scenario_criteria_population_enabled()) {
		int x = goalOffsetsX[goalIndex];
		int y = goalOffsetsY[goalIndex];
		goalIndex++;
		Widget_Panel_drawSmallLabelButton(xOffset + x, yOffset + y, 15, 1);
		int width = Widget_GameText_draw(62, 11, xOffset + x + 8, yOffset + y + 3, FONT_NORMAL_RED);
		Widget_Text_drawNumber(scenario_criteria_population(), '@', " ",
			xOffset + x + 8 + width, yOffset + y + 3, FONT_NORMAL_RED);
	}
	if (scenario_criteria_culture_enabled()) {
		int x = goalOffsetsX[goalIndex];
		int y = goalOffsetsY[goalIndex];
		goalIndex++;
		Widget_Panel_drawSmallLabelButton(xOffset + x, yOffset + y, 15, 1);
		int width = Widget_GameText_draw(62, 12, xOffset + x + 8, yOffset + y + 3, FONT_NORMAL_RED);
		Widget_Text_drawNumber(scenario_criteria_culture(), '@', " ",
			xOffset + x + 8 + width, yOffset + y + 3, FONT_NORMAL_RED);
	}
	if (scenario_criteria_prosperity_enabled()) {
		int x = goalOffsetsX[goalIndex];
		int y = goalOffsetsY[goalIndex];
		goalIndex++;
		Widget_Panel_drawSmallLabelButton(xOffset + x, yOffset + y, 15, 1);
		int width = Widget_GameText_draw(62, 13, xOffset + x + 8, yOffset + y + 3, FONT_NORMAL_RED);
		Widget_Text_drawNumber(scenario_criteria_prosperity(), '@', " ",
			xOffset + x + 8 + width, yOffset + y + 3, FONT_NORMAL_RED);
	}
	if (scenario_criteria_peace_enabled()) {
		int x = goalOffsetsX[goalIndex];
		int y = goalOffsetsY[goalIndex];
		goalIndex++;
		Widget_Panel_drawSmallLabelButton(xOffset + x, yOffset + y, 15, 1);
		int width = Widget_GameText_draw(62, 14, xOffset + x + 8, yOffset + y + 3, FONT_NORMAL_RED);
		Widget_Text_drawNumber(scenario_criteria_peace(), '@', " ",
			xOffset + x + 8 + width, yOffset + y + 3, FONT_NORMAL_RED);
	}
	if (scenario_criteria_favor_enabled()) {
		int x = goalOffsetsX[goalIndex];
		int y = goalOffsetsY[goalIndex];
		goalIndex++;
		Widget_Panel_drawSmallLabelButton(xOffset + x, yOffset + y, 15, 1);
		int width = Widget_GameText_draw(62, 15, xOffset + x + 8, yOffset + y + 3, FONT_NORMAL_RED);
		Widget_Text_drawNumber(scenario_criteria_favor(), '@', " ",
			xOffset + x + 8 + width, yOffset + y + 3, FONT_NORMAL_RED);
	}
	int immediateGoalText = tutorial_get_immediate_goal_text();
	if (immediateGoalText) {
		int x = goalOffsetsX[2];
		int y = goalOffsetsY[2];
		goalIndex++;
		Widget_Panel_drawSmallLabelButton(xOffset + x, yOffset + y, 31, 1);
		Widget_GameText_draw(62, immediateGoalText, xOffset + x + 8, yOffset + y + 3, FONT_NORMAL_RED);
	}
	
	Widget_Panel_drawInnerPanel(xOffset + 16, yOffset + 152, 33, 15);
	
	Widget_RichText_setFonts(FONT_NORMAL_WHITE, FONT_NORMAL_RED);
	Widget_RichText_init(lang_get_message(textId)->content.text,
		xOffset + 48, yOffset + 152, 31, 15, 0);

	Graphics_setClipRectangle(xOffset + 19, yOffset + 155, 522, 234);
	Widget_RichText_draw(lang_get_message(textId)->content.text,
		xOffset + 32, yOffset + 164, 496, 14, 0);
	Graphics_resetClipRectangle();
}

void UI_MissionStart_BriefingInitial_drawForeground()
{
	int xOffset = Data_Screen.offset640x480.x + 16;
	int yOffset = Data_Screen.offset640x480.y + 32;

	Widget_RichText_drawScrollbar();
	Widget_Button_drawImageButtons(xOffset + 500, yOffset + 394, &imageButtonStartMission, 1);
	if (campaignHasChoice[scenario_campaign_rank()]) {
		Widget_Button_drawImageButtons(xOffset + 10, yOffset + 396, &imageButtonBackToSelection, 1);
	}
}

void UI_MissionStart_BriefingReview_drawForeground()
{
	int xOffset = Data_Screen.offset640x480.x + 16;
	int yOffset = Data_Screen.offset640x480.y + 32;

	Widget_RichText_drawScrollbar();
	Widget_Button_drawImageButtons(xOffset + 500, yOffset + 394, &imageButtonStartMission, 1);
}

void UI_MissionStart_BriefingInitial_handleMouse(const mouse *m)
{
	int xOffset = Data_Screen.offset640x480.x + 16;
	int yOffset = Data_Screen.offset640x480.y + 32;

	if (Widget_Button_handleImageButtons(xOffset + 500, yOffset + 394, &imageButtonStartMission, 1, 0)) {
		return;
	}
	if (campaignHasChoice[scenario_campaign_rank()]) {
		if (Widget_Button_handleImageButtons(xOffset + 10, yOffset + 396, &imageButtonBackToSelection, 1, 0)) {
			return;
		}
	}
	Widget_RichText_handleScrollbar(m);
}

void UI_MissionStart_BriefingReview_handleMouse(const mouse *m)
{
	int xOffset = Data_Screen.offset640x480.x + 16;
	int yOffset = Data_Screen.offset640x480.y + 32;

	if (Widget_Button_handleImageButtons(xOffset + 500, yOffset + 394, &imageButtonStartMission, 1, 0)) {
		return;
	}
	Widget_RichText_handleScrollbar(m);
}

static void briefingBack(int param1, int param2)
{
	if (UI_Window_getId() == Window_MissionBriefingInitial) {
		sound_speech_stop();
		UI_Window_goTo(Window_MissionSelection);
	}
}

static void startMission(int param1, int param2)
{
	switch (UI_Window_getId()) {
		case Window_NewCareerDialog:
		case Window_MissionSelection:
			UI_MissionStart_show();
			break;
		default:
			sound_speech_stop();
			sound_music_reset();
			UI_Window_goTo(Window_City);
			Data_CityInfo.missionSavedGameWritten = 0;
			break;
	}
	UI_Window_requestRefresh();
}
