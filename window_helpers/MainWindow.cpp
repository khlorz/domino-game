#include "MainWindow.h"
#include "imgui_internal.h"

static uint16_t CombineTwoDigits(uint16_t a, uint16_t b) {
    return (a * 10) + b;
}

static bool ButtonWithPosition(const char* label, const ImVec2& b_pos, const ImVec2& b_size = ImVec2(0, 0));

void MainWindow::RenderWindow()
{
    MainMenuBar();

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_Always);
    if (!ImGui::Begin("Main Body", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_HorizontalScrollbar)) {
        ImGui::End();
        return;
    }
    
    // Game Start screen
    // The point where you initialize the important objects before starting the game
    if (!GameStart) {
        if (ImGui::BeginChild("BigWindow")) {
            // Initialize the important objects
            const auto& sliderpos = (ImGui::GetWindowContentRegionMax() / 2.0f) - ImVec2(90.0f, 80.0f);
            ImGui::SetCursorPos(sliderpos);
            ImGui::PushItemWidth(150.0f);
            ImGui::SliderInt("Number Of Players", &NumberOfPlayer, 4, 8);
            if (GameStartButton()) {
                GameStart = true;
                RestartGame();
            }

            ImGui::EndChild();
        }

        ImGui::End();
        return;
    }

    // Game End screen
    if (GameEnd) {
        const auto& textpos = (ImGui::GetWindowContentRegionMax() / 2.0f) - ImVec2(50.0f, 120.0f);
        ImGui::SetCursorPos(textpos);
        ImGui::Text("Player %d Wins!", dvars::GameState.GetWinnerNumber());

        const auto& sliderpos = (ImGui::GetWindowContentRegionMax() / 2.0f) - ImVec2(90.0f, 80.0f);
        ImGui::SetCursorPos(sliderpos);
        ImGui::PushItemWidth(150.0f);
        ImGui::SliderInt("Number Of Players", &NumberOfPlayer, 4, 8);

        if (GameStartButton()) {
            RestartGame();
        }
        ImGui::End();
        return;
    }
    
    static const ImVec2 topside_size = ImVec2(ImGui::GetContentRegionAvail().x, viewport->WorkSize.y * 3.0f / 4.1f );
    if (ImGui::BeginChild("MainGameBoard", topside_size, false, ImGuiWindowFlags_NoScrollbar)) {
        ImGui::Text("Current Player Turn: %d", dvars::GameState.GetCurrentTurn() + 1);
       
        RenderGameBoard();
        if (RenderDropOptions()) {
            GameEnd = dvars::GameState.CheckGameState();
            dvars::GameState.TurnAdvance();
            //ShowPassButton = dvars::GameState.GetCurrentTurn() == 0 ? dvars::GameState.CurrentPlayerCanAttack() : false;
        }
        if (RenderPassButton()) {
            ShowPassButton = false;
            dvars::GameState.PassCurrentTurn();
            dvars::GameState.SetNoClickedCard();
            GameEnd = dvars::GameState.CheckGameState();
            //ShowPassButton = dvars::GameState.GetCurrentTurn() == 0 ? dvars::GameState.CurrentPlayerCanAttack() : false;
        }

        AIAttacks();

        ImGui::EndChild();
    }

    if (ImGui::BeginChild("PlayerCards", ImVec2(ImGui::GetContentRegionAvail().x / 1.50f, ImGui::GetContentRegionAvail().y), true)) {
        ImGui::Text("Your Cards");

        RenderPlayerDominoes();

        ImGui::EndChild();
    }
    ImGui::SameLine();
    if (ImGui::BeginChild("OtherInfo", ImVec2(0, 0), true)) {
        this->OtherInfoChildWindow();

        ImGui::EndChild();
    }

    ImGui::End();
}

void MainWindow::OtherInfoChildWindow()
{
    auto& dgs = dvars::GameState;

    ImGui::BeginGroup();
    ImGui::Text("Other Info");
    static const char* PLabel[] = { "PC##2", "PC##3", "PC##4", "PC##5", "PC##6", "PC##7", "PC##8" };
    for (uint16_t i = 1; i < dgs.GetNumberOfPlayers(); i++) {
        ImGui::Text("Player %d Remaining Cards: %d", i + 1, dgs.GetPlayerData(i).GetRemainingCards());
    }
    ImGui::EndGroup();
    
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

    ImGui::SameLine();
    dgs.RenderCurrentTurnLog();
}

void MainWindow::MainMenuBar()
{
    if (!ImGui::BeginMainMenuBar()) {
        ImGui::EndMainMenuBar();
        return;
    }

    if (ImGui::BeginMenu("Game")) {
        ImGui::MenuItem("Exit", NULL, &CloseWindowBool);
        if (ImGui::MenuItem("Restart", NULL, false, GameStart)) {
            RestartGame();
        }
        if (ImGui::MenuItem("Game Log")) {

        }
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

bool MainWindow::CloseWindow()
{
    return CloseWindowBool;
}

//------------------------------------------------------------------------
// RENDERING FUNCTIONS
//------------------------------------------------------------------------

void MainWindow::RenderGameBoard()
{
    if (!dvars::GameState.NoDominoesYet()) {
        dvars::GameState.RenderBoardDominoes();
        return;
    }

    if (FirstDominoButton()) {
        auto clicked_card = dvars::GameState.GetPlayerSelectedCard(0);
        if (clicked_card == nullptr) {
            return;
        }
        dvars::GameState.AddBoardDominoes(*clicked_card, TileDropPosition_Left);
        dvars::GameState.TurnAdvance();
        dvars::GameState.SetNoClickedCard();
        return;
    }
}

bool MainWindow::RenderDropOptions()
{
    auto RenderDropLambda = [this](const char* label, const Domino2D& DropOption, const Domino2D& TempConnectee, Domino2D* Connectee, int domino_pos) {
        if (!DropOption.RenderTransparentTile(label, domino_pos == TileDropPosition_Left ? dvars::GameState.LeftSideSize() >= 7 : dvars::GameState.RightSideSize() >= 7)) {
            return false;
        }

        auto clicked_card = dvars::GameState.GetPlayerSelectedCard(0);
        *clicked_card = DropOption;                                    // Copy the parameters to the card that will connect
        dvars::GameState.AddBoardDominoes(*clicked_card, domino_pos);  // Add the card to the board dominoes(dominoes that are already put down)
        *Connectee = TempConnectee;                                    // Copy the parameters of the connectee card
        dvars::GameState.SetNoClickedCard();                           // Set the clicked card to nullptr
        return true;
    };

    if (ShowDropOptions.first) {
        if (RenderDropLambda("LeftDropOption", DropOptions.first, TemporaryConnectee.first, ConnecteePointer.first, TileDropPosition_Left)) {
            ShowDropOptions.first = false;
            ShowDropOptions.second = false;
            return true;
        }
    }

    if (ShowDropOptions.second) {
        if (RenderDropLambda("RightDropOption", DropOptions.second, TemporaryConnectee.second, ConnecteePointer.second, TileDropPosition_Right)) {
            ShowDropOptions.first = false;
            ShowDropOptions.second = false;
            return true;
        }
    }

    return false;
}

void MainWindow::RenderPlayerDominoes()
{
    auto& dgs = dvars::GameState;
    ImGui::BeginDisabled(dgs.GetCurrentTurn() != 0);
    if (dgs.RenderPlayerDominoes()) {
        auto* clicked_card = dgs.GetPlayerSelectedCard(0);
        if (clicked_card == nullptr || dgs.NoDominoesYet()) {
            ShowDropOptions.first  = false;
            ShowDropOptions.second = false;
            ImGui::EndDisabled();
            return;
        }

        DropOptions.first  = *clicked_card;
        DropOptions.second = *clicked_card;
        
        TemporaryConnectee.first  = dgs.EmptyLeftSideDominoes() ? *dgs.GetFirstDomino() : *dgs.GetLatestLeftSideDomino();
        ConnecteePointer.first    = dgs.EmptyLeftSideDominoes() ? dgs.GetFirstDomino() : dgs.GetLatestLeftSideDomino();
        ShowDropOptions.first     = DropOptions.first.ConnectDomino(TemporaryConnectee.first, TileDropPosition_Left);

        TemporaryConnectee.second = dgs.EmptyRightSideDominoes() ? *dgs.GetFirstDomino() : *dgs.GetLatestRightSideDomino();
        ConnecteePointer.second   = dgs.EmptyRightSideDominoes() ? dgs.GetFirstDomino() : dgs.GetLatestRightSideDomino();
        ShowDropOptions.second    = DropOptions.second.ConnectDomino(TemporaryConnectee.second, TileDropPosition_Right);
    }
    ImGui::EndDisabled();
}

//------------------------------------------------------------------------
// GAME RELATED FUNCTIONS
//------------------------------------------------------------------------

void MainWindow::RestartGame()
{
    GameEnd = false;
    ShowPassButton = false;
    ShowDropOptions.first = false;
    ShowDropOptions.second = false;
    dvars::GameState.ResetGameState();
    dvars::GameState.InitializeGame(NumberOfPlayer, AIDifficulty_Random);
}

void MainWindow::GameLogWindow()
{
}

//------------------------------------------------------------
// CUSTOM BUTTONS
//------------------------------------------------------------

bool MainWindow::RenderPassButton()
{
    if (!ShowPassButton) {
        return false;
    }

    ImGui::SameLine();
    constexpr ImVec2 size(141.0f, 70.0f);
    constexpr const char* label("PASS!!!");
    const ImVec2 pos = ImVec2(ImGui::GetWindowContentRegionMax().x / 2, 320.0f) - ImVec2(size.x / 2.0f, 0);
    return ButtonWithPosition(label, pos, size);
}

bool MainWindow::GameStartButton()
{
    constexpr ImVec2 size(101.0f, 50.0f);
    constexpr const char* label("GAME START!!!");
    const ImVec2 pos = (ImGui::GetWindowContentRegionMax() / 2.0f) - ImVec2(size.x / 2.0f, size.y / 2.0f);
    return ButtonWithPosition(label, pos, size);
}

bool MainWindow::FirstDominoButton()
{
    constexpr ImVec2 size(101.0f, 50.0f);
    constexpr const char* label("FIRST DOMINO");
    const ImVec2 pos = (ImGui::GetWindowContentRegionMax() / 2.0f) - ImVec2(size.x / 2.0f, size.y / 2.0f) + ImVec2(0.0f, 20.0f);
    return ButtonWithPosition(label, pos, size);
}

void MainWindow::AIAttacks()
{
    if (dvars::GameState.GetCurrentTurn() == 0 || GameEnd) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    static float ai_attack_time = 0.0f;
    static float max_attack_time = 2.5f;

    if (ai_attack_time < max_attack_time) {
        ai_attack_time += io.DeltaTime;
        return;
    }

    auto& dgs = dvars::GameState;
    if (!dgs.AIAttackFunc()) {
        dgs.PassCurrentTurn();
        GameEnd = dgs.CheckGameState();
    }
    else {
        GameEnd = dgs.CheckGameState();
        dgs.TurnAdvance();
    }
    ShowPassButton = dgs.GetCurrentTurn() == 0 && !dgs.CurrentPlayerCanAttack();
    ai_attack_time = 0;
}

static bool ButtonWithPosition(const char* label, const ImVec2& b_pos, const ImVec2& b_size)
{
    using namespace ImGui;
    ImGuiButtonFlags flags = 0;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    //ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(b_size, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
    ImVec2 pos = b_pos;

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
        flags |= ImGuiButtonFlags_Repeat;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

    if (g.LogEnabled)
        LogSetNextTextDecoration("[", "]");
    RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

    // Automatically close popups
    //if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
    //    CloseCurrentPopup();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}


