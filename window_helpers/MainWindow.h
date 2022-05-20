#pragma once

#include <iostream>
#include "imgui.h"
#include "GameLogic.h"

enum AiDifficulty_
{
	AiDifficulty_Easy       = 0,
	AiDifficulty_Hard       = 1,
	AiDifficulty_ReallyHard = 2,
	AiDifficulty_GiveUp     = 3
};

class MainWindow
{
private:
	bool     CloseWindowBool = false;
	bool     GameStart       = false;
	bool     GameEnd         = false;
	bool     ShowPassButton  = false;
	bool     OpenGameLogs    = false;
	bool     OpenOptions     = false;
	bool     OpenHelp        = false;
	int      NumberOfPlayer  = 4;
	float    AIAttackSpeed   = 1.25f;
	std::pair<bool, bool>           ShowDropOptions;
	std::pair<Domino2D, Domino2D>   TemporaryConnectee;
	std::pair<Domino2D, Domino2D>   DropOptions;
	std::pair<Domino2D*, Domino2D*> ConnecteePointer;

	void MainMenuBar();

	// UI Domino Functions
	void RenderGameBoard();
	bool GameStartButton();
	bool FirstDominoButton();
	//void AddDominoes(Domino2D* d, int left_or_right);
	//void RenderDominoes();
	void RenderPlayerDominoes();
	bool RenderPassButton();
	void RestartGame();
	void AIAttacks();
	bool RenderDropOptions();
	void OtherInfoChildWindow();
	void GameLogWindow();
	void GameStartOptions();
	bool GameStartWindow();
	bool GameEndWindow();
	void OngoingGameWindow();
	void BoardWindow();
	void PlayerWindow();
	void HelpWindow();
	void OptionWindow();
public:
	MainWindow()  = default;
	~MainWindow() = default;

	void RenderWindow();
	bool CloseWindow();
};

