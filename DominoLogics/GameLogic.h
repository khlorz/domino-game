#pragma once

#include "imgui.h"
#include <vector>
#include <random>
#include <chrono>
#include <array>

enum AIDifficulty_
{
	AIDifficulty_Random,
	AIDifficulty_Normal,
	AIDifficulty_Hard,
	AIDifficulty_GigaBrain
};

enum TileOrientation_
{
	TileOrientation_Horizontal = 0,
	TileOrientation_Vertical   = 1
};

enum TileDropPosition_
{
	TileDropPosition_Left  = 0,
	TileDropPosition_LeftDown = 2,
	TileDropPosition_Up    = 0, // The same as the left number
	TileDropPosition_Right = 1,
	TileDropPosition_RightDown = 3,
	TileDropPosition_Down  = 1  // The same as the right number
};

//-----------------------------------------------------------------------------------------------------------------------
// DominoTile CLASS
//-----------------------------------------------------------------------------------------------------------------------

class DominoTile
{
protected:
	bool     removed;
	uint16_t left_number;
	uint16_t right_number;

public:
	DominoTile() = default;
	constexpr DominoTile(uint16_t num1, uint16_t num2);

	constexpr bool     IsDoubleNumber() const;
	constexpr bool     IsRemoved() const;
	constexpr uint16_t TileSum();
	constexpr uint16_t GetLeftNumber() const;
	constexpr uint16_t GetRightNumber() const;

	// Change the card status of whether it's already removed from the player's hand or not
	void ChangeCardRemoveState(bool r);
	
};

template<size_t S>
using DominoTileArr        = std::array<DominoTile, S>;
using DominoTileVec        = std::vector<DominoTile>;
using DominoTileVecPointer = std::vector<DominoTile*>;



//-----------------------------------------------------------------------------------------------------------------------
// Domino2D CLASS
//-----------------------------------------------------------------------------------------------------------------------

class Domino2D : public DominoTile
{
protected:
	// Static members
	inline static float  TileHeight;
	inline static float  TileWidth;
	inline static float  TileScale;
	inline static float  TileLeftWidth;
	inline static ImVec2 TileVecSize;
	inline static ImU32  TileColor;
	inline static ImU32  DotColor;

private:
	// Non-static members
	ImVec2   TilePos;
	bool     MirrorTile;
	int      TileOrientation;
	bool     left_connectable;
	bool     right_connectable;

	bool TileButton(const char* label, bool OverrideMirror, ImGuiButtonFlags flags = 0) const;
	bool ConnectDominoOnRight(Domino2D& d);
	bool ConnectDominoOnLeft(Domino2D& d);

public:
	Domino2D() = default;
	Domino2D(uint16_t leftnumber, uint16_t rightnumber);
	~Domino2D() = default;

	// Static Functions
	static void    InitiateDominoStatics(float tile_h, ImU32 tile_col, ImU32 dot_col);
	static void    InitiateDominoStatics(float tile_h, const ImVec4& tile_col, const ImVec4& dot_col);
	static void    ChangeColors(ImU32 tile_col, ImU32 dot_col);
	static void    ChangeColors(const ImVec4& tile_col, const ImVec4& dot_col);
	static void    ChangeSize(float tile_h);
	static ImU32&  GetTileColor();
	static ImU32&  GetDotColor();
	static ImVec2& GetDimensions();

	// Non-static Functions
	void CreateTile(uint16_t leftnumber, uint16_t rightnumber);
	void ChangePosition(float plus_x, float plus_y);
	ImVec2 GetPos() const;
	uint16_t GetTileNumber() const;

	// Set the domino parameters as the first domino [Centered, Horizontal(if not double number) or Vertical(if double number)]
	bool SetAsFirstDomino();
	// Connect the domino to a connectee domino depending on what tile number position (left or right)
	bool ConnectDomino(Domino2D& DominoConnectee, int pos);
	// Test the domino if it will connect with the connectee. Returns true if it connects for anyway it can
	bool PseudoConnectDomino(Domino2D& DominoConnectee) const;
	// Query if the left side of the tile is connectable
	bool IsLeftConnectable() const;
	// Query if the right side of the tile is connectable
	bool IsRightConnectable() const;
	// Change the card status of whether the left side of the tile is connectable or not
	void ChangeLeftNumConnection(bool connectable);
	// Change the card status of whether the right side of the tile is connectable or not
	void ChangeRightNumConnection(bool connectable);
	// Reset the parameters of the domino
	void ResetParameters();

	// Renders the tile textures and dots and all it needs to become a domino
	bool RenderTile(const char* label, bool OverrideMirror, ImGuiButtonFlags flags = 0) const;
	// Renders the tile as a drop option (a bit transparent version of the normal domino) 
	bool RenderTransparentTile(const char* label, bool OverrideMirror, ImGuiButtonFlags flags = 0) const;
	// Render the tile independently
	bool RenderTileIndependently(const char* label, const ImVec2& sz, const ImVec2& position, ImGuiButtonFlags flags = 0) const;

	constexpr bool operator == (const Domino2D& other) const;
};


//-----------------------------------------------------------------------------------------------------------------------
// Player CLASS
//-----------------------------------------------------------------------------------------------------------------------

class Player
{
protected:
	uint16_t               PlayerNumber;
	Domino2D*              CurrentlyClickedCard = nullptr; // The card that is about to be used
	std::vector<Domino2D*> PlayerCards;

public:
	Player() = default;
	Player(const uint16_t& p_num);
	uint16_t GetPlayerNum() const;

	bool HasTheDouble(uint16_t num) const;
	bool HasTheHighestCard(uint16_t num1, uint16_t num2) const;
	void SetClickedCard(Domino2D* dcard);
	void ClearCards();
	void SetPlayerNum(uint16_t p_num);
	void SetCard(Domino2D& card);
	uint16_t SumOfCards() const;
	uint16_t NumberOfCards() const;
	Domino2D* GetClickedCard();
	std::vector<Domino2D*>& GetPlayerCards();
	uint16_t GetRemainingCards();

};

//-----------------------------------------------------------------------------------------------------------------------
// PlayerDomino2D CLASS
//-----------------------------------------------------------------------------------------------------------------------

class PlayerDomino2D : public Player
{
private:
	inline static ImU32* TileColor;
	inline static ImU32* DotColor;
	inline static ImU32  ClickedColor;
	inline static ImVec2 TileSize;
	inline static float  scale;
	inline static float  HalfWidth;
	inline static ImVec2 TilePositioning;

public:
	PlayerDomino2D() {}
	PlayerDomino2D(uint16_t player_number);

	static void InitializeDominoParameters();
	void ClearDominoes();
	bool InitializePlayerDomino();
	bool RenderPlayerDominoes();

	// DEBUG
	void ChangeTilePositioning(float _x, float _y);
	ImVec2 GetTilePosition();

private:
	bool PlayerDominoButton(Domino2D& DT, const char* label, const ImVec2& position, ImGuiButtonFlags flags = 0);

};

template<size_t S>
using PlayerArr = std::array<PlayerDomino2D, S>;
using PlayerVec = std::vector<PlayerDomino2D>;



class GameBoardDominoes
{
protected:
	Domino2D* FirstDomino = nullptr;           // The starting domino
	std::vector<Domino2D*> LeftSideDominoes;   // The connecting dominoes from the left of FirstDomino
	std::vector<Domino2D*> RightSideDominoes;  // The connecting dominoes from the right of FirstDomino

	void AddFirstDomino(Domino2D& d);
	void AddLeftDomino(Domino2D& d);
	void AddRightDomino(Domino2D& d);
	void ClearBoardDominoes();

public:
	bool      NoDominoesYet()          const ;
	bool      EmptyLeftSideDominoes()  const ;
	bool      EmptyRightSideDominoes() const ;
	void      RenderBoardDominoes();
	size_t    LeftSideSize() const ;
	size_t    RightSideSize() const ;
	Domino2D* GetFirstDomino();
	Domino2D* GetLatestLeftSideDomino();
	Domino2D* GetLatestRightSideDomino();

};

class DominoAI
{
private:
	Player* PlayerData     = nullptr;   // Human player data
	Player* AIData         = nullptr;   // AI data
	int     AIDifficulty;

public:
	DominoAI() = default;
	DominoAI(Player* p_data, Player* ai_data, uint16_t ai_difficulty);
	~DominoAI() = default;

	void SetAIData(Player* p_data, Player* ai_data);
	void SetDifficulty(uint16_t ai_difficulty);
	void AIAttack();

private:
	// Will randomize the order of cards and use the foremost usable card to attack from left to right
	void RandomCompute();
	void NormalCompute();
	void HardCompute();
	void GigaBrainCompute();

};



//-----------------------------------------------------------------------------------------------------------------------
// DominoLogs CLASS
//-----------------------------------------------------------------------------------------------------------------------
enum LogMove_
{
	LogMove_TurnPassed,
	LogMove_TurnAttack
};
using LogMove = uint16_t;

class DominoLogs
{
private:
	struct AttackLog
	{
		const Domino2D* DominoUsed = nullptr;
		uint16_t PlayerNumber;
		LogMove PlayerMove;

		AttackLog() = default;
		AttackLog(const Domino2D* d, uint16_t player_number, LogMove player_move) :
			DominoUsed(d), PlayerNumber(player_number), PlayerMove(player_move)
		{}
	};
	std::vector<AttackLog> LogData;

public:
	DominoLogs() = default;

	void AddLog(const Domino2D* d, uint16_t player_number, LogMove player_move);
	void ClearLog();
	void RenderLog(int CurrentTurn = -1);

};


// Domino Game State
class DominoGameStructure : public GameBoardDominoes
{
private:
	bool              GameInitialized;
	DominoLogs        GameLog;
	DominoAI          AIPlayerLogic;
	PlayerArr<8>      Players;
	uint16_t          CurrentTurn;
	uint16_t          NumberOfTurns;
	uint16_t          NumberOfCards;
	uint16_t          NumberOfPlayers;
	uint16_t          PlayerWinner;
	uint16_t          FirstTurn;
	uint16_t          LastTurn;
	uint16_t          NumberOfPasses;

public:
	DominoGameStructure();
	~DominoGameStructure() = default;

	bool            InitializeGame(const uint16_t& number_of_players, const uint16_t& ai_difficulty, const bool& change_player = false);
	bool            CheckGameState();
	void            SetNoClickedCard();
	uint16_t        GetWinnerNumber() const;
	uint16_t        GetCurrentTurn() const;
	Domino2D*       GetPlayerSelectedCard(uint16_t player_number);
	PlayerDomino2D& GetPlayerData(uint16_t pnum);

	bool     CurrentPlayerCanAttack();
	void     PassCurrentTurn();
	void     TurnAdvance();
	void     ResetGameState();
	bool     RenderPlayerDominoes();
	void     AddBoardDominoes(Domino2D& D, int left_or_right);
	bool     AIAttackFunc();
	void     AddGameLogs(const Domino2D* d, uint16_t player_number, LogMove player_move);
	void     RenderGameLogs();
	void     RenderCurrentTurnLog();
	uint16_t GetNumberOfPlayers() const;

private:
	// For distributing cards to the players
	void DistributeCards();
	// For finding the first player turn
	void FindFirstPlayerTurn();
	// For finding the winner in a stalemate by finding the lowest sum of all cards of a player
	void FindTheLowestSum();
	// Clear player dominoes
	void ClearPlayerDominoes();
	// Clear the game logs
	void ClearGameLogs();
};

// Global Domino Variables
namespace dvars
{
// The game's dominoes. 27 dominoes in total
inline std::array<Domino2D, 27> GameDominoes = { 
												  Domino2D(1, 0), Domino2D(2, 0), Domino2D(3, 0), Domino2D(4, 0), Domino2D(5, 0), Domino2D(6, 0),
												  Domino2D(1, 1), Domino2D(2, 1), Domino2D(3, 1), Domino2D(4, 1), Domino2D(5, 1), Domino2D(6, 1),
												  Domino2D(2, 2), Domino2D(3, 2), Domino2D(4, 2), Domino2D(5, 2), Domino2D(6, 2),
												  Domino2D(3, 3), Domino2D(4, 3), Domino2D(5, 3), Domino2D(6, 3),
												  Domino2D(4, 4), Domino2D(5, 4), Domino2D(6, 4),
												  Domino2D(5, 5), Domino2D(6, 5),
												  Domino2D(6, 6)
												};

inline DominoGameStructure GameState;

}


