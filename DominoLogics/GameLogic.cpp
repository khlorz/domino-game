#include "GameLogic.h"
#include "imgui_internal.h"

//-----------------------------------------------------------------------------------------------------------------------
// DominoTile CLASS
//-----------------------------------------------------------------------------------------------------------------------

constexpr DominoTile::DominoTile(uint16_t num1, uint16_t num2) :
	removed(false),
	left_number(num1),
	right_number(num2)
{}

constexpr uint16_t DominoTile::TileSum()
{
	return left_number + right_number;
}

constexpr bool DominoTile::IsDoubleNumber() const
{
	return left_number == right_number;
}

constexpr bool DominoTile::IsRemoved() const
{
	return removed;
}

constexpr uint16_t DominoTile::GetLeftNumber() const
{
	return left_number;
}

constexpr uint16_t DominoTile::GetRightNumber() const
{
	return right_number;
}

void DominoTile::ChangeCardRemoveState(bool r)
{
    this->removed = r;
}



//-----------------------------------------------------------------------------------------------------------------------
// Player CLASS
//-----------------------------------------------------------------------------------------------------------------------

Player::Player(const uint16_t& p_num)
{
    PlayerNumber = p_num;
}

uint16_t Player::GetPlayerNum() const
{
    return PlayerNumber;
}

std::vector<Domino2D*>& Player::GetPlayerCards()
{
    return PlayerCards;
}

bool Player::HasTheDouble(uint16_t num) const
{
    for (auto& DT : PlayerCards) {
        if (DT->IsDoubleNumber()) {
            if (DT->GetLeftNumber() == num) {
                return true;
            }
        }
    }
    return false;
}

bool Player::HasTheHighestCard(uint16_t num1, uint16_t num2) const
{
    for (auto& DT : PlayerCards) {
        if (!DT->IsDoubleNumber()) {
            if (DT->GetLeftNumber() == num1 && DT->GetRightNumber() == num2) {
                return true;
            }
        }
    }
    return false;
}

uint16_t Player::SumOfCards() const
{
    uint16_t sum = 0;

    for (auto Card : PlayerCards) {
        if (Card->IsRemoved()) {
            sum += (Card->GetLeftNumber() + Card->GetRightNumber());
        }
    }

    return sum;
}

uint16_t Player::NumberOfCards() const
{
    uint16_t numcard = 0;

    for (auto* Card : PlayerCards) {
        if (!Card->IsRemoved()) {
            numcard++;
        }
    }

    return numcard;
}

void Player::SetClickedCard(Domino2D* dcard)
{
    CurrentlyClickedCard = dcard;
}

Domino2D* Player::GetClickedCard()
{
    return CurrentlyClickedCard;
}

void Player::ClearCards()
{
    PlayerCards.clear();
}

void Player::SetPlayerNum(uint16_t p_num)
{
    PlayerNumber = p_num;
}

void Player::SetCard(Domino2D& card)
{
    PlayerCards.push_back(&card);
}

uint16_t Player::GetRemainingCards()
{
    uint16_t remaining_cards = 0;

    for (auto& C : PlayerCards) {
        if (!C->IsRemoved()) {
            remaining_cards++;
        }
    }

    return remaining_cards;
}



//-----------------------------------------------------------------------------------------------------------------------
// Domino2D CLASS
//-----------------------------------------------------------------------------------------------------------------------

// Functions for drawing the dots of the dominoes
namespace domino_dots
{
    // Draw the left side of the domino. Vertically, it's the upper side.
    static void leftDotNumber(uint16_t number, const ImVec2& pos, float scale, ImU32 color, bool vertical = false)
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        switch (number)
        {
        case 1:
            draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 10.5f), pos.y + (scale * 10.5f)), scale * 2.5f, color, 40);
            break;
        case 2:
            if (vertical) {
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 4.0f), pos.y + (scale * 4.0f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 17.0f), pos.y + (scale * 17.0f)), scale * 2.5f, color, 40);
            }
            else {
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 17.0f), pos.y + (scale * 4.0f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 4.0f), pos.y + (scale * 17.0f)), scale * 2.5f, color, 40);
            }
            break;
        case 3:
            if (vertical) {
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 4.0f), pos.y + (scale * 4.0f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 10.5f), pos.y + (scale * 10.5f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 17.0f), pos.y + (scale * 17.0f)), scale * 2.5f, color, 40);
            }
            else {
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 17.0f), pos.y + (scale * 4.0f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 10.5f), pos.y + (scale * 10.5f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 4.0f), pos.y + (scale * 17.0f)), scale * 2.5f, color, 40);
            }
            break;
        case 4:
            draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 4.0f), pos.y + (scale * 4.0f)), scale * 2.5f, color, 40);
            draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 17.0f), pos.y + (scale * 17.0f)), scale * 2.5f, color, 40);
            draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 17.0f), pos.y + (scale * 4.0f)), scale * 2.5f, color, 40);
            draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 4.0f), pos.y + (scale * 17.0f)), scale * 2.5f, color, 40);
            break;
        case 5:
            draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 4.0f), pos.y + (scale * 4.0f)), scale * 2.5f, color, 40);
            draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 17.0f), pos.y + (scale * 17.0f)), scale * 2.5f, color, 40);
            draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 10.5f), pos.y + (scale * 10.5f)), scale * 2.5f, color, 40);
            draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 17.0f), pos.y + (scale * 4.0f)), scale * 2.5f, color, 40);
            draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 4.0f), pos.y + (scale * 17.0f)), scale * 2.5f, color, 40);
            break;
        case 6:
            if (vertical) {
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 4.0f), pos.y + (scale * 4.0f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 4.0f), pos.y + (scale * 10.5f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 17.0f), pos.y + (scale * 17.0f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 17.0f), pos.y + (scale * 4.0f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 17.0f), pos.y + (scale * 10.5f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 4.0f), pos.y + (scale * 17.0f)), scale * 2.5f, color, 40);
            }
            else {
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 4.0f), pos.y + (scale * 4.0f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 10.5f), pos.y + (scale * 4.0f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 17.0f), pos.y + (scale * 17.0f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 17.0f), pos.y + (scale * 4.0f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 10.5f), pos.y + (scale * 17.0f)), scale * 2.5f, color, 40);
                draw_list->AddCircleFilled(ImVec2(pos.x + (scale * 4.0f), pos.y + (scale * 17.0f)), scale * 2.5f, color, 40);
            }
            break;
        default:
            break;
        }
    }
    // Draw the right side of the domino. Vertically, it's the lower side.
    static void rightDotNumber(uint16_t number, float leftWidth, const ImVec2& pos, float scale, ImU32 col, bool vertical = false)
    {
        auto new_pos = vertical ? ImVec2(pos.x, pos.y + leftWidth) : ImVec2(pos.x + leftWidth, pos.y);
        leftDotNumber(number, new_pos, scale, col, vertical);
    }
    // Draw the line between the left and right/up and down side.
    static void middleLine(float leftWidth, const ImVec2& pos, float scale, ImU32 col, bool vertical = false)
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        if (vertical) {
            draw_list->AddRectFilled(ImVec2((1.5f * scale) + pos.x, leftWidth - (scale * 0.55f) + pos.y), ImVec2(pos.x + (19.5f * scale), leftWidth + pos.y + (scale * 0.55f)), col, 0.0f);
        }
        else {
            draw_list->AddRectFilled(ImVec2(leftWidth - (scale * 0.55f) + pos.x, pos.y + (1.5f * scale)), ImVec2(leftWidth + pos.x + (scale * 0.55f), pos.y + (19.5f * scale)), col, 0.0f);
        }
    }
}

Domino2D::Domino2D(uint16_t leftnumber, uint16_t rightnumber)
{
    CreateTile(leftnumber, rightnumber);
}

//---------------------------
// STATIC FUNCTIONS
//---------------------------

void Domino2D::InitiateDominoStatics(float tile_h, ImU32 tile_col, ImU32 dot_col)
{
    TileHeight = tile_h;
    TileWidth = (TileHeight * 2) + 1;
    TileScale = TileHeight / 21.0f;
    TileLeftWidth = TileHeight + 1;
    TileVecSize = ImVec2(TileWidth, TileHeight);
    TileColor = tile_col;
    DotColor = dot_col;
}

void Domino2D::InitiateDominoStatics(float tile_h, const ImVec4& tile_col, const ImVec4& dot_col)
{
    InitiateDominoStatics(tile_h, ImGui::ColorConvertFloat4ToU32(tile_col), ImGui::ColorConvertFloat4ToU32(dot_col));
}

void Domino2D::ChangeColors(ImU32 tile_col, ImU32 dot_col)
{
    TileColor = tile_col;
    DotColor = dot_col;
}

void Domino2D::ChangeColors(const ImVec4& tile_col, const ImVec4& dot_col)
{
    TileColor = ImGui::ColorConvertFloat4ToU32(tile_col);
    DotColor = ImGui::ColorConvertFloat4ToU32(dot_col);
}

void Domino2D::ChangeSize(float tile_h)
{
    TileHeight = tile_h;
    TileWidth = (TileHeight * 2) + 1;
    TileScale = TileHeight / 21.0f;
    TileLeftWidth = TileHeight + 1;
    TileVecSize = ImVec2(TileWidth, TileHeight);
}

ImU32& Domino2D::GetTileColor()
{
    return TileColor;
}

ImU32& Domino2D::GetDotColor()
{
    return DotColor;
}

ImVec2& Domino2D::GetDimensions()
{
    return TileVecSize;
}

//---------------------------
// NON-STATIC FUNCTIONS
//---------------------------

void Domino2D::CreateTile(uint16_t leftnumber, uint16_t rightnumber)
{
    MirrorTile        = false;
    removed           = false;
    left_number       = leftnumber;
    right_number      = rightnumber;
    left_connectable  = true;
    right_connectable = true;
    TileOrientation   = TileOrientation_Horizontal;
    TilePos           = ImVec2(0, 0);
}

ImVec2 Domino2D::GetPos() const
{
    return TilePos;
}

uint16_t Domino2D::GetTileNumber() const
{
    return (left_number * 10) + right_number;
}

bool Domino2D::SetAsFirstDomino()
{
    const ImVec2& window = ImGui::GetWindowContentRegionMax();
    if (this->IsDoubleNumber()) {
        TilePos = (window / 2.0f) - ImVec2(TileHeight / 2.0f, TileWidth / 2.0f) + ImVec2(0.0f, 20.0f);
        TileOrientation = TileOrientation_Vertical;
    }
    else {
        TilePos = (window / 2.0f) - ImVec2(TileWidth / 2.0f, TileHeight / 2.0f) + ImVec2(0.0f, 20.0f);
        TileOrientation = TileOrientation_Horizontal;
    }

    removed = true;
    return true;
}

void Domino2D::ChangeLeftNumConnection(bool connectable)
{
    this->left_connectable = connectable;
}

void Domino2D::ChangeRightNumConnection(bool connectable)
{
    this->right_connectable = connectable;
}

bool Domino2D::IsLeftConnectable() const
{
    return left_connectable;
}

bool Domino2D::IsRightConnectable() const
{
    return right_connectable;
}

void Domino2D::ResetParameters()
{
    TilePos           = ImVec2(0, 0);
    MirrorTile        = false;
    left_connectable  = true;
    right_connectable = true;
    removed           = false;
    TileOrientation   = TileOrientation_Horizontal;
}

void Domino2D::ChangePosition(float plus_x, float plus_y)
{
    TilePos.x += plus_x;
    TilePos.y += plus_y;
}

bool Domino2D::ConnectDominoOnLeft(Domino2D& DominoConnectee)
{
    const uint16_t& ConnecteeLeftNumber = DominoConnectee.MirrorTile ? DominoConnectee.GetRightNumber() : DominoConnectee.GetLeftNumber();

    if (this->GetRightNumber() == ConnecteeLeftNumber) {
        this->MirrorTile = false;
        this->ChangeRightNumConnection(false);
    }
    else if (this->GetLeftNumber() == ConnecteeLeftNumber) {
        this->MirrorTile = true;
        this->ChangeLeftNumConnection(false);
    }
    else {
        return false;
    }

    if (DominoConnectee.IsDoubleNumber()) {
        if (dvars::GameState.LeftSideSize() == 5) {
            DominoConnectee.TilePos.x = DominoConnectee.TilePos.x - TileHeight;
            DominoConnectee.TilePos.y = DominoConnectee.TilePos.y + (TileWidth / 2.0f) - (TileHeight / 2.0f);
            DominoConnectee.TileOrientation = TileOrientation_Horizontal;
            TilePos.x = DominoConnectee.TilePos.x - TileHeight;
            TilePos.y = DominoConnectee.TilePos.y - TileHeight;
            TileOrientation = TileOrientation_Vertical;
        }
        else if (dvars::GameState.LeftSideSize() == 6) {
            TilePos.x = DominoConnectee.TilePos.x;
            TilePos.y = DominoConnectee.TilePos.y - TileWidth;
            TileOrientation = TileOrientation_Vertical;
        }
        else if (dvars::GameState.LeftSideSize() == 7) {
            TilePos.x = DominoConnectee.TilePos.x;
            TilePos.y = DominoConnectee.TilePos.y - TileHeight;
            TileOrientation = TileOrientation_Horizontal;
        }
        else if (dvars::GameState.LeftSideSize() == 8) {
            TilePos.x = DominoConnectee.TilePos.x + TileWidth;
            TilePos.y = DominoConnectee.TilePos.y;
            TileOrientation = TileOrientation_Horizontal;
        }
        else if (DominoConnectee.TileOrientation == TileOrientation_Horizontal) {
            TilePos.x = DominoConnectee.TilePos.x + (TileWidth / 2.0f) - (TileHeight / 2.0f);
            TilePos.y = DominoConnectee.TilePos.y - TileWidth;
            TileOrientation = TileOrientation_Vertical;
        }
        else {
            if (dvars::GameState.LeftSideSize() > 8) {
                TilePos.x = DominoConnectee.TilePos.x + TileHeight;
                TilePos.y = DominoConnectee.TilePos.y + (TileWidth / 2.0f) - (TileHeight / 2.0f);
            }
            else {
                TilePos.x = DominoConnectee.TilePos.x - TileWidth;
                TilePos.y = DominoConnectee.TilePos.y + (TileWidth / 2.0f) - (TileHeight / 2.0f);
            }
            TileOrientation = TileOrientation_Horizontal;
        }
    }
    else if (this->IsDoubleNumber()) {
        if (dvars::GameState.LeftSideSize() == 5) {
            TilePos.x = DominoConnectee.TilePos.x - TileHeight;
            TilePos.y = DominoConnectee.TilePos.y - (TileWidth / 2.0f);
            TileOrientation = TileOrientation_Vertical;
        }
        else if (dvars::GameState.LeftSideSize() == 6) {
            TilePos.x = DominoConnectee.TilePos.x;
            TilePos.y = DominoConnectee.TilePos.y - TileWidth;
            TileOrientation = TileOrientation_Vertical;
        }
        else if (dvars::GameState.LeftSideSize() == 7) {
            TilePos.x = DominoConnectee.TilePos.x;
            TilePos.y = DominoConnectee.TilePos.y - TileHeight;
            TileOrientation = TileOrientation_Horizontal;
        }
        else if (DominoConnectee.TileOrientation == TileOrientation_Horizontal) {
            if (dvars::GameState.LeftSideSize() > 7) {
                TilePos.x = DominoConnectee.TilePos.x + TileWidth;
                TilePos.y = DominoConnectee.TilePos.y - (TileWidth / 2.0f) + (TileHeight / 2.0f);
            }
            else {
                TilePos.x = DominoConnectee.TilePos.x - TileHeight;
                TilePos.y = DominoConnectee.TilePos.y - (TileWidth / 2.0f) + (TileHeight / 2.0f);
            }
            TileOrientation = TileOrientation_Vertical;
        }
        else {
            TilePos.x = DominoConnectee.TilePos.x - (TileWidth / 2.0f) + (TileHeight / 2.0f);
            TilePos.y = DominoConnectee.TilePos.y - TileHeight;
            TileOrientation = TileOrientation_Horizontal;
        }
    }
    else {
        if (dvars::GameState.LeftSideSize() == 5) {
            TilePos.x = DominoConnectee.TilePos.x - TileHeight;
            TilePos.y = DominoConnectee.TilePos.y - (TileWidth / 2.0f);
            TileOrientation = TileOrientation_Vertical;
        }
        else if (dvars::GameState.LeftSideSize() == 6) {
            TilePos.x = DominoConnectee.TilePos.x;
            TilePos.y = DominoConnectee.TilePos.y - TileWidth;
            TileOrientation = TileOrientation_Vertical;
        }
        else if (dvars::GameState.LeftSideSize() == 7) {
            TilePos.x = DominoConnectee.TilePos.x;
            TilePos.y = DominoConnectee.TilePos.y - TileHeight;
            TileOrientation = TileOrientation_Horizontal;
        }
        else if (DominoConnectee.TileOrientation == TileOrientation_Vertical) {
            TilePos.x = DominoConnectee.TilePos.x;
            TilePos.y = DominoConnectee.TilePos.y - TileWidth;
            TileOrientation = TileOrientation_Vertical;
        }
        else {
            if (dvars::GameState.LeftSideSize() > 7) {
                TilePos.x = DominoConnectee.TilePos.x + TileWidth;
                TilePos.y = DominoConnectee.TilePos.y;
            }
            else {
                TilePos.x = DominoConnectee.TilePos.x - TileWidth;
                TilePos.y = DominoConnectee.TilePos.y;
            }
            TileOrientation = TileOrientation_Horizontal;
        }
    }

    this->removed = true;
    DominoConnectee.MirrorTile ? DominoConnectee.ChangeRightNumConnection(false) : DominoConnectee.ChangeLeftNumConnection(false);
    return true;
}

bool Domino2D::ConnectDominoOnRight(Domino2D& DominoConnectee)
{
    const uint16_t& RightNumber          = this->GetRightNumber();
    const uint16_t& LeftNumber           = this->GetLeftNumber();
    const uint16_t& ConnecteeRightNumber = DominoConnectee.MirrorTile ? DominoConnectee.GetLeftNumber() : DominoConnectee.GetRightNumber();

    if (RightNumber == ConnecteeRightNumber) {
        MirrorTile = true;
        this->ChangeRightNumConnection(false);
    }
    else if (LeftNumber == ConnecteeRightNumber) {
        MirrorTile = false;
        this->ChangeLeftNumConnection(false);
    }
    else {
        return false;
    }

    if (DominoConnectee.IsDoubleNumber()) {
        if (dvars::GameState.RightSideSize() == 5) {
            DominoConnectee.TilePos.x = DominoConnectee.TilePos.x;
            DominoConnectee.TilePos.y = DominoConnectee.TilePos.y + (TileWidth / 2.0f) - (TileHeight / 2.0f);
            DominoConnectee.TileOrientation = TileOrientation_Horizontal;
            TilePos.x = DominoConnectee.TilePos.x + TileWidth;
            TilePos.y = DominoConnectee.TilePos.y;
            TileOrientation = TileOrientation_Vertical;
        }
        else if (dvars::GameState.RightSideSize() == 6) {
            TilePos.x = DominoConnectee.TilePos.x;
            TilePos.y = DominoConnectee.TilePos.y + TileWidth;
            TileOrientation = TileOrientation_Vertical;
        }
        else if (dvars::GameState.RightSideSize() == 7) {
            TilePos.x = DominoConnectee.TilePos.x - (TileWidth / 2.0f);
            TilePos.y = DominoConnectee.TilePos.y + TileWidth;
            TileOrientation = TileOrientation_Horizontal;
        }
        else if (dvars::GameState.RightSideSize() == 8) {
            TilePos.x = DominoConnectee.TilePos.x - TileWidth;
            TilePos.y = DominoConnectee.TilePos.y;
            TileOrientation = TileOrientation_Horizontal;
        }
        else if (DominoConnectee.TileOrientation == TileOrientation_Horizontal) {
            TilePos.x = DominoConnectee.TilePos.x + (TileWidth / 2.0f) - (TileHeight / 2.0f);
            TilePos.y = DominoConnectee.TilePos.y + TileHeight;
            TileOrientation = TileOrientation_Vertical;
        }
        else {
            if (dvars::GameState.LeftSideSize() > 8) {
                TilePos.x = DominoConnectee.TilePos.x - TileWidth;
                TilePos.y = DominoConnectee.TilePos.y + (TileWidth / 2.0f) - (TileHeight / 2.0f);
            }
            else {
                TilePos.x = DominoConnectee.TilePos.x + TileHeight;
                TilePos.y = DominoConnectee.TilePos.y + (TileWidth / 2.0f) - (TileHeight / 2.0f);
            }
            TileOrientation = TileOrientation_Horizontal;
        }
    }
    else if (this->IsDoubleNumber()) {
        if (dvars::GameState.RightSideSize() == 5) {
            TilePos.x = DominoConnectee.TilePos.x + TileWidth;
            TilePos.y = DominoConnectee.TilePos.y;
            TileOrientation = TileOrientation_Vertical;
        }
        else if (dvars::GameState.RightSideSize() == 6) {
            TilePos.x = DominoConnectee.TilePos.x;
            TilePos.y = DominoConnectee.TilePos.y + TileWidth;
            TileOrientation = TileOrientation_Vertical;
        }
        else if (dvars::GameState.RightSideSize() == 7) {
            TilePos.x = DominoConnectee.TilePos.x - (TileWidth / 2.0f);
            TilePos.y = DominoConnectee.TilePos.y + TileWidth;
            TileOrientation = TileOrientation_Horizontal;
        }
        else if (DominoConnectee.TileOrientation == TileOrientation_Horizontal) {
            if (dvars::GameState.RightSideSize() > 7) {
                TilePos.x = DominoConnectee.TilePos.x - TileHeight;
                TilePos.y = DominoConnectee.TilePos.y - (TileWidth / 2.0f) + (TileHeight / 2.0f);
            }
            else {
                TilePos.x = DominoConnectee.TilePos.x + TileWidth;
                TilePos.y = DominoConnectee.TilePos.y - (TileWidth / 2.0f) + (TileHeight / 2.0f);
            }
            TileOrientation = TileOrientation_Vertical;
        }
        else {
            TilePos.x = DominoConnectee.TilePos.x - (TileWidth / 2.0f) + (TileHeight / 2.0f);
            TilePos.y = DominoConnectee.TilePos.y + TileWidth;
            TileOrientation = TileOrientation_Horizontal;
        }
    }
    else {
        if (dvars::GameState.RightSideSize() == 5) {
            TilePos.x = DominoConnectee.TilePos.x + TileWidth;
            TilePos.y = DominoConnectee.TilePos.y;
            TileOrientation = TileOrientation_Vertical;
        }
        else if (dvars::GameState.RightSideSize() == 6) {
            TilePos.x = DominoConnectee.TilePos.x;
            TilePos.y = DominoConnectee.TilePos.y + TileWidth;
            TileOrientation = TileOrientation_Vertical;
        }
        else if (dvars::GameState.RightSideSize() == 7) {
            TilePos.x = DominoConnectee.TilePos.x - (TileWidth / 2.0f);
            TilePos.y = DominoConnectee.TilePos.y + TileWidth;
            TileOrientation = TileOrientation_Horizontal;
        }
        else if (DominoConnectee.TileOrientation == TileOrientation_Vertical) {
            TilePos.x = DominoConnectee.TilePos.x;
            TilePos.y = DominoConnectee.TilePos.y + TileWidth;
        }
        else {
            if (dvars::GameState.RightSideSize() > 7) {
                TilePos.x = DominoConnectee.TilePos.x - TileWidth;
                TilePos.y = DominoConnectee.TilePos.y;
            }
            else {
                TilePos.x = DominoConnectee.TilePos.x + TileWidth;
                TilePos.y = DominoConnectee.TilePos.y;
            }
            TileOrientation = TileOrientation_Horizontal;
        }
    }
    // Changes the current tile status to removed and the connectee's corresponding tile unconnectable
    this->removed = true;
    DominoConnectee.MirrorTile ? DominoConnectee.ChangeLeftNumConnection(false) : DominoConnectee.ChangeRightNumConnection(false);
    return true;
}

bool Domino2D::ConnectDomino(Domino2D& DominoConnectee, int drop_pos)
{
    const auto& connectable = drop_pos == TileDropPosition_Left ?  (!DominoConnectee.MirrorTile ? DominoConnectee.IsLeftConnectable() : DominoConnectee.IsRightConnectable()) :
                                                                   (DominoConnectee.MirrorTile ? DominoConnectee.IsLeftConnectable() : DominoConnectee.IsRightConnectable());
    if (!connectable) {
        return false;
    }

    if (drop_pos == TileDropPosition_Left) {
        return ConnectDominoOnLeft(DominoConnectee);
    }

    if (drop_pos >= TileDropPosition_Right) {
        return ConnectDominoOnRight(DominoConnectee);
    }

    return false;
}

bool Domino2D::PseudoConnectDomino(Domino2D& DominoConnectee) const
{
    if (!DominoConnectee.IsLeftConnectable() && !DominoConnectee.IsRightConnectable()) {
        return false;
    }

    const bool& leftconnect = DominoConnectee.MirrorTile ? DominoConnectee.IsRightConnectable() : DominoConnectee.IsLeftConnectable();
    const bool& rightconnect = DominoConnectee.MirrorTile ? DominoConnectee.IsLeftConnectable() : DominoConnectee.IsRightConnectable();
    const uint16_t& RightNumber = this->GetRightNumber();
    const uint16_t& LeftNumber  = this->GetLeftNumber();
    const uint16_t& ConnecteeLeftNumber  = DominoConnectee.MirrorTile ? DominoConnectee.GetRightNumber() : DominoConnectee.GetLeftNumber();
    const uint16_t& ConnecteeRightNumber = DominoConnectee.MirrorTile ? DominoConnectee.GetLeftNumber()  : DominoConnectee.GetRightNumber();

    //if (DominoConnectee.TileNumbers.IsDoubleNumber()) [[unlikely]] {
    //    connect1 = RightNumber == ConnecteeLeftNumber || LeftNumber == ConnecteeRightNumber;
    //}

    bool connect1 = (RightNumber == ConnecteeLeftNumber  && leftconnect)  || (LeftNumber == ConnecteeRightNumber && rightconnect);
    bool connect2 = (RightNumber == ConnecteeRightNumber && rightconnect) || (LeftNumber == ConnecteeLeftNumber  && leftconnect);

    return connect1 || connect2;
}

bool Domino2D::TileButton(const char* label, bool OverrideMirror, ImGuiButtonFlags flags) const
{
    using namespace ImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const bool Mirror = OverrideMirror ? !MirrorTile : MirrorTile;

    ImVec2 pos = TilePos;
    ImVec2 size = CalcItemSize(TileOrientation == TileOrientation_Vertical ? ImVec2(TileVecSize.y, TileVecSize.x) : TileVecSize, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
        flags |= ImGuiButtonFlags_Repeat;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    //const ImU32 col = held && hovered ? GetColorU32(ImGuiCol_ButtonActive) : (hovered ? GetColorU32(ImGuiCol_ButtonHovered) : TileColor);
    const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, col, true, 3.0f * TileScale);

    if (g.LogEnabled)
        LogSetNextTextDecoration("[", "]");

    domino_dots::leftDotNumber(Mirror ? right_number : left_number, pos, TileScale, DotColor, TileOrientation);
    domino_dots::middleLine(TileLeftWidth, pos, TileScale, DotColor, TileOrientation);
    domino_dots::rightDotNumber(Mirror ? left_number : right_number, TileLeftWidth, pos, TileScale, DotColor, TileOrientation);

    // Automatically close popups
    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
    //    CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

bool Domino2D::RenderTile(const char* label, bool OverrideMirror, ImGuiButtonFlags flags) const
{
    ImGui::PushStyleColor(ImGuiCol_Button, TileColor);
    if (TileButton(label, OverrideMirror, flags)) {
        ImGui::PopStyleColor();
        return true;
    }
    ImGui::PopStyleColor();
    return false;
}

bool Domino2D::RenderTransparentTile(const char* label, bool OverrideMirror, ImGuiButtonFlags flags) const
{
    using namespace ImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = TilePos;
    ImVec2 size = CalcItemSize(TileOrientation == TileOrientation_Vertical ? ImVec2(TileVecSize.y, TileVecSize.x) : TileVecSize, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
        flags |= ImGuiButtonFlags_Repeat;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    constexpr ImU32 tile_transparent_color = 2197815295;
    constexpr ImU32 dot_transparent_color = 2181038080;
    const bool Mirror = OverrideMirror ? !MirrorTile : MirrorTile;

    // Render
    const ImU32 col = held && hovered ? GetColorU32(ImGuiCol_ButtonActive) : (hovered ? GetColorU32(ImGuiCol_ButtonHovered) : tile_transparent_color);
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, col, true, 3.0f * TileScale);

    if (g.LogEnabled)
        LogSetNextTextDecoration("[", "]");

    domino_dots::leftDotNumber(Mirror ? right_number : left_number, pos, TileScale, dot_transparent_color, TileOrientation);
    domino_dots::middleLine(TileLeftWidth, pos, TileScale, dot_transparent_color, TileOrientation);
    domino_dots::rightDotNumber(Mirror ? left_number : right_number, TileLeftWidth, pos, TileScale, dot_transparent_color, TileOrientation);

    // Automatically close popups
    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
    //    CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

bool Domino2D::RenderTileIndependently(const char* label, const ImVec2& sz, const ImVec2& position, ImGuiButtonFlags flags) const
{
    using namespace ImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = position;
    if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
        pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
    ImVec2 size = CalcItemSize(sz, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
        flags |= ImGuiButtonFlags_Repeat;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    //const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    const ImU32 col = held && hovered ? GetColorU32(ImGuiCol_ButtonActive) : (hovered ? GetColorU32(ImGuiCol_ButtonHovered) : TileColor);

    const float scale = size.y / 21.0f;
    const float halfwidth = size.y + 1;

    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, col, true, 3.0f * scale);

    if (g.LogEnabled)
        LogSetNextTextDecoration("[", "]");


    domino_dots::leftDotNumber(left_number, pos, scale, DotColor, TileOrientation_Horizontal);
    domino_dots::middleLine(halfwidth, pos, scale, DotColor, TileOrientation_Horizontal);
    domino_dots::rightDotNumber(right_number, halfwidth, pos, scale, DotColor, TileOrientation_Horizontal);

    // Automatically close popups
    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
    //    CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

constexpr bool Domino2D::operator == (const Domino2D& DT) const
{
    return (left_number == DT.left_number && right_number == DT.right_number) || (right_number == DT.left_number && left_number == DT.right_number);
}

constexpr bool Domino2D::operator == (const DominoTile& other) const
{
    return (left_number == other.GetLeftNumber() && right_number == other.GetRightNumber()) || (right_number == other.GetLeftNumber() && left_number == other.GetRightNumber());
}




//-----------------------------------------------------------------------------------------------------------------------
// PlayerDomino2D CLASS
//-----------------------------------------------------------------------------------------------------------------------

PlayerDomino2D::PlayerDomino2D(uint16_t player_number)
{
    this->PlayerNumber = player_number;
}

bool PlayerDomino2D::InitializePlayerDomino()
{
    if (this->PlayerNumber >= 8) {
        return false;
    }

    return true;
}

void PlayerDomino2D::InitializeDominoParameters()
{
    TileColor       = &Domino2D::GetTileColor();
    DotColor        = &Domino2D::GetDotColor();
    ClickedColor    = 3432598886;
    TileSize        = ImVec2(141.0f, 70.0f);
    scale           = 70.0f / 21.0f;
    HalfWidth       = 71.0f;
    TilePositioning = ImVec2(23.0f, 540.0f);

}

bool PlayerDomino2D::PlayerDominoButton(Domino2D& DT, const char* label, const ImVec2& position, ImGuiButtonFlags flags)
{
    using namespace ImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = position;
    ImVec2 size = CalcItemSize(TileSize, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
        flags |= ImGuiButtonFlags_Repeat;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    //const ImU32 col = held && hovered ? GetColorU32(ImGuiCol_ButtonActive) : (hovered ? GetColorU32(ImGuiCol_ButtonHovered) : (DT.Clicked ? ClickedColor : *TileColor));
    bool tile_selected = CurrentlyClickedCard == &DT;
    const ImU32 col = (held && hovered) ? GetColorU32(ImGuiCol_ButtonActive) : hovered ? GetColorU32(ImGuiCol_ButtonHovered) : (tile_selected ? ClickedColor : GetColorU32(ImGuiCol_Button));

    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, col, true, 3.0f * scale);

    if (g.LogEnabled)
        LogSetNextTextDecoration("[", "]");

    domino_dots::leftDotNumber(DT.GetLeftNumber(), pos, scale, *DotColor);
    domino_dots::middleLine(HalfWidth, pos, scale, *DotColor);
    domino_dots::rightDotNumber(DT.GetRightNumber(), HalfWidth, pos, scale, *DotColor);

    // Automatically close popups
    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
    //    CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;

    return true;
}

void PlayerDomino2D::ClearDominoes()
{
    PlayerCards.clear();
}

bool PlayerDomino2D::RenderPlayerDominoes()
{
    bool clicked    = false;
    auto& dgs       = dvars::GameState;
    ImVec2 position = TilePositioning;
    bool first_tile_state = dgs.IsThereAChangeInPlayer() && dgs.GetNumberOfTurns() == 0;

    for (size_t iter = 0; auto DT : PlayerCards) {
        if (!DT->IsRemoved()) {
            static const char* Label[] = { "PDB##1", "PDB##2", "PDB##3", "PDB##4", "PDB##5" };

            ImGui::SameLine();
            ImGui::BeginDisabled(first_tile_state && dgs.GetFirstTurnTile() != *DT); // Disable the tile in the first turn if it's not the first tile that needs to be used
            ImGui::PushStyleColor(ImGuiCol_Button, *TileColor);
            if (PlayerDominoButton(*DT, Label[iter], position)) {
                clicked = true;
                CurrentlyClickedCard = CurrentlyClickedCard == nullptr ? DT : (CurrentlyClickedCard == DT ? nullptr : DT);
            }
            if (ImGui::IsItemHovered() && first_tile_state && dgs.GetFirstTurnTile() == *DT) {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(150.0f);
                ImGui::TextUnformatted("FIRST TURN TILE USE");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
            ImGui::PopStyleColor();
            ImGui::EndDisabled();
        }

        position = ImVec2(position.x + 152.0f, position.y);
        iter++;
    }

    return clicked;
}

// FOR DEBUGGING PURPOSES
#ifdef _DEBUG
void PlayerDomino2D::ChangeTilePositioning(float _x, float _y)
{
    TilePositioning = ImVec2(_x, _y);
}

ImVec2 PlayerDomino2D::GetTilePosition()
{
    return TilePositioning;
}
#endif // DEBUG




//-----------------------------------------------------------------------------------------------------------------------
// GameBoardDominoes CLASS
//-----------------------------------------------------------------------------------------------------------------------

void GameBoardDominoes::AddFirstDomino(Domino2D& d)
{
    FirstDomino = &d;
}

void GameBoardDominoes::AddLeftDomino(Domino2D& d)
{
    LeftSideDominoes.push_back(&d);
}

void GameBoardDominoes::AddRightDomino(Domino2D& d)
{
    RightSideDominoes.push_back(&d);
}

bool GameBoardDominoes::NoDominoesYet() const
{
    return FirstDomino == nullptr;
}

Domino2D* GameBoardDominoes::GetLatestLeftSideDomino()
{
    return LeftSideDominoes.back();
}

Domino2D* GameBoardDominoes::GetLatestRightSideDomino()
{
    return RightSideDominoes.back();
}

Domino2D* GameBoardDominoes::GetFirstDomino()
{
    return FirstDomino;
}

bool GameBoardDominoes::EmptyLeftSideDominoes() const
{
    return LeftSideDominoes.empty();
}

bool GameBoardDominoes::EmptyRightSideDominoes() const
{
    return RightSideDominoes.empty();
}

void GameBoardDominoes::RenderBoardDominoes()
{
    static const char* LeftDominoLabel[] = { "a##ls", "b##ls", "c##ls", "d##ls", "e##ls", "f##ls",
                                              "g##ls", "h##ls", "i##ls", "j##ls", "k##ls", "l##ls",
                                              "m##ls", "n##ls", "o##ls", "p##ls", "q##ls", "r##ls", "s##ls", "t##ls" };
    static const char* RightDominoLabel[] = { "a##rs", "b##rs", "c##rs", "d##rs", "e##rs", "f##rs",
                                              "g##rs", "h##rs", "i##rs", "j##rs", "k##rs", "l##rs",
                                              "m##rs", "n##rs", "o##rs", "p##rs", "q##rs", "r##rs", "s##rs", "t##rs" };

    // Render the first domino. The center most domino
    FirstDomino->RenderTile("First Domino", false);

    // Render the left side dominoes
    for (int i = 0; auto & DT : LeftSideDominoes) {
        ImGui::SameLine();
        DT->RenderTile(LeftDominoLabel[i], i > 6);
        i++;
    }

    // Render the right side dominoes
    for (int i = 0; auto & DT : RightSideDominoes) {
        ImGui::SameLine();
        DT->RenderTile(RightDominoLabel[i], i > 6);
        i++;
    }
}

void GameBoardDominoes::ClearBoardDominoes()
{
    FirstDomino = nullptr;
    LeftSideDominoes.clear();
    RightSideDominoes.clear();
}

size_t GameBoardDominoes::LeftSideSize() const
{
    return LeftSideDominoes.size();
}

size_t GameBoardDominoes::RightSideSize() const
{
    return RightSideDominoes.size();
}



//-----------------------------------------------------------------------------------------------------------------------
// DominoAI CLASS
//-----------------------------------------------------------------------------------------------------------------------

DominoAI::DominoAI(Player* p_data, Player* ai_data, uint16_t ai_difficulty)
{
    this->SetAIData(p_data, ai_data);
    this->SetDifficulty(ai_difficulty);
}

void DominoAI::SetAIData(Player* p_data, Player* ai_data)
{
    this->AIData = ai_data;
    this->PlayerData = p_data;
}

void DominoAI::SetDifficulty(uint16_t ai_difficulty)
{
    this->AIDifficulty = ai_difficulty;
}

void DominoAI::AIAttack()
{
    if (this->FirstTurnAIAttack()) {
        return;
    }

    switch (this->AIDifficulty)
    {
    case AIDifficulty_Random:    this->RandomCompute();    return;
    case AIDifficulty_Normal:    this->NormalCompute();    return;
    case AIDifficulty_Hard:      this->HardCompute();      return;
    case AIDifficulty_GigaBrain: this->GigaBrainCompute(); return;
    default: return;
    }
}

bool DominoAI::FirstTurnAIAttack()
{
    if (!dvars::GameState.IsThereAChangeInPlayer() || dvars::GameState.GetNumberOfTurns() != 0) {
        return false;
    }

    auto& dgs = dvars::GameState;
    auto& ai_cards = this->AIData->GetPlayerCards();
    for (auto& ai_card : ai_cards) {
        if (*ai_card != dgs.GetFirstTurnTile()) {
            continue;
        }

        ai_card->SetAsFirstDomino();
        dgs.AddBoardDominoes(*ai_card, TileDropPosition_Left);
        break;
    }

    return true;
}

void DominoAI::RandomCompute()
{
    auto& dgs = dvars::GameState;
    auto& ai_cards = AIData->GetPlayerCards();
    const auto& seed = std::chrono::steady_clock::now().time_since_epoch().count();
    std::mt19937_64 rng(seed);
    std::shuffle(ai_cards.begin(), ai_cards.end(), rng);

    for (auto ai_card : ai_cards) {
        if (ai_card->IsRemoved()) {
            continue;
        }
        if (dgs.NoDominoesYet()) {
            ai_card->SetAsFirstDomino();
            dgs.AddBoardDominoes(*ai_card, TileDropPosition_Left);
            return;
        }
        if (ai_card->ConnectDomino(dgs.EmptyRightSideDominoes() ? *dgs.GetFirstDomino() : *dgs.GetLatestRightSideDomino(), TileDropPosition_Right)) {
            dgs.AddBoardDominoes(*ai_card, TileDropPosition_Right);
            return;
        }
        // Will use the first usable card and will attack on the left side
        if (ai_card->ConnectDomino(dgs.EmptyLeftSideDominoes() ? *dgs.GetFirstDomino() : *dgs.GetLatestLeftSideDomino(), TileDropPosition_Left)) {
            dgs.AddBoardDominoes(*ai_card, TileDropPosition_Left);
            return;
        }
        // And if not possible, then on the right
        // Otherwise, continue with loop and repeat
    }
}

void DominoAI::NormalCompute()
{

}

void DominoAI::HardCompute()
{

}

void DominoAI::GigaBrainCompute()
{

}



//-----------------------------------------------------------------------------------------------------------------------
// DominoLogs CLASS
//-----------------------------------------------------------------------------------------------------------------------

void DominoLogs::AddLog(const Domino2D* d, uint16_t player_number, LogMove player_move)
{
    LogData.push_back(AttackLog(d, player_number, player_move));
}

void DominoLogs::ClearLog()
{
    LogData.clear();
}

void DominoLogs::RenderLog(int NumberOfTurn)
{
    if (NumberOfTurn == -1) {
        // Render all logs
        static const auto domino_sz = ImVec2(121.0f, 60.0f);
        std::string DominoLabels = "DT##0";

        for (size_t i = 0; auto & logs : LogData) {
            logs.PlayerMove == LogMove_TurnAttack ? ImGui::Text("Turn %d: Player %d attacked with:", i, logs.PlayerNumber) : ImGui::Text("Turn %d: Player %d passed", i, logs.PlayerNumber);
            if (logs.DominoUsed != nullptr) {
                const auto& domino_pos = ImGui::GetCurrentWindow()->DC.CursorPos;
                logs.DominoUsed->RenderTileIndependently(DominoLabels.c_str(), domino_sz, domino_pos);
            }

            i++;
            ImGui::Separator();
        }
        return;
    }

    ImGui::BeginGroup(); {
        constexpr ImVec2 LogDominoSize = ImVec2(141.0f, 70.0f);
        const auto& CurrentLog = LogData[NumberOfTurn];
        if (CurrentLog.DominoUsed == nullptr) {
            ImGui::Text("Player %d passed!", CurrentLog.PlayerNumber);
            ImGui::EndGroup();
            return;
        }

        ImGui::Text("Player %d attacked with:", CurrentLog.PlayerNumber);
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        const ImVec2& pos = window->DC.CursorPos;
        ImGui::BeginDisabled(); {
            CurrentLog.DominoUsed->RenderTileIndependently("Current Log", LogDominoSize, pos);
        } ImGui::EndDisabled();
    } ImGui::EndGroup();
}



//-----------------------------------------------------------------------------------------------------------------------
// DominoGameStructure CLASS
//-----------------------------------------------------------------------------------------------------------------------
static void ResetGameDominoes()
{
    for (auto& GD : dvars::GameDominoes) {
        GD.ResetParameters();
    }
}

static void ShuffleGameDominoes()
{
    ResetGameDominoes();
    auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    std::shuffle(dvars::GameDominoes.begin(), dvars::GameDominoes.end(), rng);
}

DominoGameStructure::DominoGameStructure() :
    Players({
            PlayerDomino2D(0),
            PlayerDomino2D(1),
            PlayerDomino2D(2),
            PlayerDomino2D(3),
            PlayerDomino2D(4),
            PlayerDomino2D(5),
            PlayerDomino2D(6),
            PlayerDomino2D(7)
        }),
    NumberOfPlayers(0),
    NumberOfTurns(0)
{}

bool DominoGameStructure::InitializeGame(const uint16_t & number_of_players, const uint16_t& ai_difficulty, const bool& change_player)
{
    // Check if the number of players is eligible for the game
    if (number_of_players < 4 || number_of_players > 8) {
        GameInitialized = false;
        return false;
    }

    // Set the AI difficulty
    this->AIPlayerLogic.SetDifficulty(ai_difficulty);

    // In domino, whenever there is a change in player (a player leaves, a player joins, a player leaves but another joins at the same time), the first player will change
    ChangeInPlayers = NumberOfPlayers != number_of_players || change_player;
    NumberOfPlayers = number_of_players;
    NumberOfCards   = number_of_players == 4 ? 5 : (number_of_players > 6 ? 3 : 4);

    this->DistributeCards();

    if (PlayerOneAlwaysFirst) {
        CurrentTurn = 0;
        ChangeInPlayers = false;
    }
    else {
        if (ChangeInPlayers) 
            this->FindFirstPlayerTurn();
        else                 
            CurrentTurn = PlayerWinner;
    }

    GameInitialized = true;
    return true;
}

void DominoGameStructure::SetPlayerOneAsFirstTurn(bool enable)
{
    this->PlayerOneAlwaysFirst = enable;
}

bool DominoGameStructure::CheckGameState()
{
    // Check if the current player already won
    if (Players[CurrentTurn].NumberOfCards() == 0) {
        PlayerWinner = CurrentTurn;
        return true;
    }

    if (NumberOfPasses == NumberOfPlayers + 1) {
        FindTheLowestSum();
        return true;
    }

    return false;
}

bool DominoGameStructure::IsThereAChangeInPlayer()
{
    return ChangeInPlayers;
}

DominoTile DominoGameStructure::GetFirstTurnTile() const
{
    return FirstTurnTileAttack;
}

void DominoGameStructure::PassCurrentTurn()
{
    this->AddGameLogs(nullptr, CurrentTurn + 1, LogMove_TurnPassed);
    TurnAdvance();
    NumberOfPasses++;
}

PlayerDomino2D& DominoGameStructure::GetPlayerData(uint16_t pnum)
{
    return Players[pnum];
}

uint16_t DominoGameStructure::GetCurrentTurn() const
{
    return CurrentTurn;
}

void DominoGameStructure::SetNoClickedCard()
{
    Players[0].SetClickedCard(nullptr);
}

void DominoGameStructure::DistributeCards()
{
    int end_domino = 0;
    ShuffleGameDominoes();

    for (int p_idx = 0; p_idx < NumberOfPlayers; p_idx++) {
        auto& CurrentPlayer = Players[p_idx]; // Reference for the current player for readability
        CurrentPlayer.ClearCards();

        int starting_domino = end_domino;		// Create the starting domino index and the end domino index for distribution
        end_domino += NumberOfCards;

        for (int current_domino = starting_domino; current_domino < end_domino; current_domino++) {
            CurrentPlayer.SetCard(dvars::GameDominoes[current_domino]);
        }
        CurrentPlayer.InitializePlayerDomino();
    }
}

void DominoGameStructure::FindFirstPlayerTurn()
{
    // Finding the player that has the double number starting from 6 to 1
    for (int i = 6; i > 0; i--) {
        for (int p = 0; p < NumberOfPlayers; p++) {
            if (Players[p].HasTheDouble(i)) {
                CurrentTurn = FirstTurn = p;
                FirstTurnTileAttack = DominoTile(i, i);
                return;
            }
        }
    }

    // If there is no one who has the double number, find the player who has the highest card 
    for (int i = 6; i > 0; i--) {
        for (int j = i - 1; i >= 0; j--) {
            for (int p = 0; p < NumberOfPlayers; p++) {
                if (Players[p].HasTheHighestCard(i, j)) {
                    CurrentTurn = FirstTurn = p;
                    FirstTurnTileAttack = DominoTile(i, j);
                    return;
                }
            }
        }
    }
}

void DominoGameStructure::FindTheLowestSum()
{
    uint16_t lowest_sum = 100;
    uint16_t current_winner = 10;
    uint16_t current_sum;

    for (int i = 0; i < NumberOfPlayers; i++) {
        current_sum = Players[i].SumOfCards();

        if (current_sum < lowest_sum) {
            lowest_sum = current_sum;
            current_winner = i;
        }
        else if (current_sum == lowest_sum) [[unlikely]] {
            current_winner = Players[i].NumberOfCards() < Players[current_winner].NumberOfCards() ? i : (i > FirstTurn && current_winner > FirstTurn) ? current_winner : i;
        }
    }

    PlayerWinner = current_winner;
}

Domino2D* DominoGameStructure::GetPlayerSelectedCard(uint16_t player_number)
{
    return Players[player_number].GetClickedCard();
}

bool DominoGameStructure::RenderPlayerDominoes()
{
    return Players[0].RenderPlayerDominoes();
}

void DominoGameStructure::ClearPlayerDominoes()
{
    for (auto& P : Players) {
        P.ClearDominoes();
    }
}

void DominoGameStructure::TurnAdvance()
{
    NumberOfTurns++;
    CurrentTurn++;
    if (CurrentTurn == NumberOfPlayers){
        CurrentTurn = 0;
    }
}

bool DominoGameStructure::CurrentPlayerCanAttack()
{
    const auto& P = Players[CurrentTurn].GetPlayerCards();

    for (auto PC : P) {
        if (!PC->IsRemoved()) {
            if (PC->PseudoConnectDomino(EmptyLeftSideDominoes() ? *GetFirstDomino() : *GetLatestLeftSideDomino())) {
                return true;
            }
            if (PC->PseudoConnectDomino(EmptyRightSideDominoes() ? *GetFirstDomino() : *GetLatestRightSideDomino())) {
                return true;
            }
        }
    }

    return false;
}

void DominoGameStructure::AddBoardDominoes(Domino2D& D, int left_or_right)
{
    // If it's empty, then it's the first turn of the game, then set the domino for first domino placement
    if (NoDominoesYet()) {
        D.SetAsFirstDomino();
        AddFirstDomino(D);
    }
    else if (left_or_right == TileDropPosition_Left) {
        AddLeftDomino(D);
    }
    else if (left_or_right == TileDropPosition_Right) {
        AddRightDomino(D);
    }

    // Log the current move
    this->AddGameLogs(&D, CurrentTurn + 1, LogMove_TurnAttack);
    // Because there is an added domino on board, then the number of passes shall reset
    NumberOfPasses = 0;
}

uint16_t DominoGameStructure::GetWinnerNumber() const
{
    return PlayerWinner + 1;
}

void DominoGameStructure::ResetGameState()
{
    // Clear the board dominoes
    this->ClearBoardDominoes();
    // Clear the players' dominoes
    this->ClearPlayerDominoes();
    // Clear game logs
    this->ClearGameLogs();
    // Reset dominoes states
    ResetGameDominoes();
    NumberOfTurns  = 0;
    NumberOfCards  = 0;
    NumberOfPasses = 0;
}

uint16_t DominoGameStructure::GetNumberOfPlayers() const
{
    return NumberOfPlayers;
}

bool DominoGameStructure::AIAttackFunc()
{
    if (!NoDominoesYet() && !this->CurrentPlayerCanAttack()) {
        return false;
    }

    const auto& ai_num = CurrentTurn;
    AIPlayerLogic.SetAIData(&this->GetPlayerData(0), &this->GetPlayerData(ai_num));
    AIPlayerLogic.AIAttack();
    return true;
}

void DominoGameStructure::AddGameLogs(const Domino2D* d, uint16_t player_number, LogMove player_move)
{
    GameLog.AddLog(d, player_number, player_move);
}

void DominoGameStructure::ClearGameLogs()
{
    GameLog.ClearLog();
}

void DominoGameStructure::RenderGameLogs()
{
    GameLog.RenderLog(-1);
}

void DominoGameStructure::RenderCurrentTurnLog()
{
    if (NumberOfTurns == 0) {
        ImGui::Text("NO MOVES YET");
        return;
    }

    GameLog.RenderLog(NumberOfTurns - 1);
}

uint16_t DominoGameStructure::GetNumberOfTurns() const
{
    return NumberOfTurns;
}







