#include "MainWindow.h"
#include "imgui_internal.h"
#include "imgui_markdown.h"

namespace ImGui
{
static void QuestionMark(const std::string_view& str)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(400.0f);
        ImGui::TextUnformatted(str.data());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
}

static bool ButtonWithPosition(const char* label, const ImVec2& b_pos, const ImVec2& b_size = ImVec2(0, 0));

void MainWindow::RenderWindow()
{
    this->MainMenuBar();

    const ImGuiIO& io = ImGui::GetIO();
    if (io.KeyCtrl && ImGui::IsKeyPressed(71, false))                           OpenGameLogs = !OpenGameLogs;   // CTRL + G
    if (io.KeyCtrl && ImGui::IsKeyPressed(72, false))                           OpenHelp     = !OpenHelp;       // CTRL + H
    if (io.KeyCtrl && ImGui::IsKeyPressed(79, false))                           OpenOptions = !OpenOptions;     // CTRL + O
    if (GameStart && !GameEnd && io.KeyCtrl && ImGui::IsKeyPressed(82, false))  this->RestartGame();            // CTRL + R

    if (OpenGameLogs) this->GameLogWindow();    // Opens the Game Logs window
    if (OpenHelp)     this->HelpWindow();       // Opens the Help window
    if (OpenOptions)  this->OptionWindow();     // Opens the Options window

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_Always);
    if (!ImGui::Begin("Main Body", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
        ImGui::End();
        return;
    }
    
    // Game Start screen
    if (this->GameStartWindow()) {
        ImGui::End();
        return;
    }

    // Game End screen
    if (this->GameEndWindow()) {
        ImGui::End();
        return;
    }
    
    // Main Game window
    this->OngoingGameWindow();

    ImGui::End();
}

//------------------------------------------------------------------------
// WINDOW FUNCTIONS
//------------------------------------------------------------------------

void MainWindow::MainMenuBar()
{
    if (!ImGui::BeginMainMenuBar()) {
        ImGui::EndMainMenuBar();
        return;
    }

    if (ImGui::BeginMenu("Game")) {
        if (ImGui::MenuItem("Change Game Parameters", NULL, false, GameStart && !GameEnd)) {
            GameStart = false;
        }
        if (ImGui::MenuItem("Restart", "Ctrl + R", false, GameStart && !GameEnd)) {
            RestartGame();
        }
        ImGui::MenuItem("Game Log", "Ctrl + G", &OpenGameLogs);
        ImGui::MenuItem("Options", "Ctrl + O", &OpenOptions);
        ImGui::MenuItem("Help", "Ctrl + H", &OpenHelp);
        ImGui::MenuItem("Exit", "Alt + F4", &CloseWindowBool);
        ImGui::EndMenu();
    }

    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("FPS: %0.2f", io.Framerate);

    ImGui::EndMainMenuBar();
}

void MainWindow::GameStartOptions()
{
    // Position and Size debug widgets
    //static float x_pos1 = 80.0f; // 70
    //ImGui::DragFloat("xpos1", &x_pos1);
    //static float x_pos2 = 90.0f; // 102
    //ImGui::DragFloat("xpos2", &x_pos2);
    //static float sz = 170.0f; // 200
    //ImGui::DragFloat("xpos3", &sz);

    const auto& textpos = (ImGui::GetWindowContentRegionMax() / 2.0f) - ImVec2(70.0f, 100.0f);
    ImGui::SetCursorPos(textpos);
    ImGui::Text("Number of players: %d", NumberOfPlayer);

    const auto& sliderpos = (ImGui::GetWindowContentRegionMax() / 2.0f) - ImVec2(102.0f, 80.0f);
    ImGui::SetCursorPos(sliderpos);
    ImGui::PushItemWidth(200.0f);
    ImGui::SliderInt("##NumberOfPlayers", &NumberOfPlayer, 4, 8, "", ImGuiSliderFlags_AlwaysClamp);

    if (GameStartButton()) {
        RestartGame();
    }
}

bool MainWindow::GameStartWindow()
{
    if (GameStart) {
        return false;
    }

    // Render the widgets for game options
    this->GameStartOptions();

    return true;
}

bool MainWindow::GameEndWindow()
{
    // Game End screen
    if (!GameEnd) {
        return false;
    }

    // Render winner text
    const auto& textpos = (ImGui::GetWindowContentRegionMax() / 2.0f) - ImVec2(50.0f, 120.0f);
    ImGui::SetCursorPos(textpos);
    ImGui::Text("Player %d Wins!", dvars::GameState.GetWinnerNumber());

    // Render the widgets for game options
    this->GameStartOptions();

    return true;
}

void MainWindow::OngoingGameWindow()
{
    static const auto& viewport = ImGui::GetMainViewport();
    static const ImVec2 topside_size = ImVec2(ImGui::GetContentRegionAvail().x, viewport->WorkSize.y * 3.0f / 4.1f);
    if (ImGui::BeginChild("MainGameBoard", topside_size, false, ImGuiWindowFlags_NoScrollbar)) {
        this->BoardWindow();

        ImGui::EndChild();
    }

    if (ImGui::BeginChild("PlayerCards", ImVec2(ImGui::GetContentRegionAvail().x / 1.50f, ImGui::GetContentRegionAvail().y), true)) {
        this->PlayerWindow();

        ImGui::EndChild();
    }

    ImGui::SameLine();
    if (ImGui::BeginChild("OtherInfo", ImVec2(0, 0), true)) {
        this->OtherInfoChildWindow();

        ImGui::EndChild();
    }
}

void MainWindow::BoardWindow()
{
    auto& dgs = dvars::GameState;
    ImGui::Text("Current Player Turn: %d", dgs.GetCurrentTurn() + 1);

    RenderGameBoard();

    if (RenderDropOptions()) {
        GameEnd = dgs.CheckGameState();
        dgs.TurnAdvance();
        //ShowPassButton = dvars::GameState.GetCurrentTurn() == 0 ? dvars::GameState.CurrentPlayerCanAttack() : false;
    }

    if (RenderPassButton()) {
        ShowPassButton = false;
        dgs.PassCurrentTurn();
        dgs.SetNoClickedCard();
        GameEnd = dgs.CheckGameState();
        //ShowPassButton = dvars::GameState.GetCurrentTurn() == 0 ? dvars::GameState.CurrentPlayerCanAttack() : false;
    }

    // The function for the attacking AI
    AIAttacks();
}

void MainWindow::PlayerWindow()
{
    ImGui::Text("Your Cards");

    RenderPlayerDominoes();
}

void MainWindow::OtherInfoChildWindow()
{
    auto& dgs = dvars::GameState;

    static const auto w_sz = ImVec2(ImGui::GetContentRegionAvail().x / 1.90f, ImGui::GetContentRegionAvail().y);
    if (ImGui::BeginChild("OtherInfoWindow", w_sz)) {
        ImGui::Text("Other Info");
        static const char* PLabel[] = { "PC##2", "PC##3", "PC##4", "PC##5", "PC##6", "PC##7", "PC##8" };
        for (uint16_t i = 1; i < dgs.GetNumberOfPlayers(); i++) {
            ImGui::Text("Player %d Remaining Cards: %d", i + 1, dgs.GetPlayerData(i).GetRemainingCards());
        }

        ImGui::EndChild();
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

    ImGui::SameLine();
    dgs.RenderCurrentTurnLog();
}

bool MainWindow::CloseWindow()
{
    return CloseWindowBool;
}

void MainWindow::GameLogWindow()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(250.0f, 300.0f), ImGuiCond_Always);
    if (!ImGui::Begin("Game Logs", &OpenGameLogs, ImGuiWindowFlags_NoResize)) {
        ImGui::End();
        return;
    }

    dvars::GameState.RenderGameLogs();

    ImGui::End();
}

void MainWindow::OptionWindow()
{
    if (!OpenOptions) {
        return;
    }
    static bool once = true;
    static ImVec2 size(400.0f, 100.0f);
    if (once) {
        once = false;
        ImGui::OpenPopup("Options");
        const ImVec2& center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(295.0f, 78.0f), ImGuiCond_Appearing);
    }

    if (!ImGui::BeginPopupModal("Options", &OpenOptions, ImGuiWindowFlags_NoResize)) {
        once = true;
        return;
    }

    static bool PlayerOneFirstBool = false;
    if (ImGui::Checkbox("Player One Always First", &PlayerOneFirstBool)) {
        dvars::GameState.SetPlayerOneAsFirstTurn(PlayerOneFirstBool);
    }
    ImGui::SameLine();
    ImGui::QuestionMark("You are always the first turn every start of the game regardless of the game rules");

    ImGui::AlignTextToFramePadding();
    ImGui::PushItemWidth(100.0f);
    ImGui::SliderFloat("AI Attack Speed:", &this->AIAttackSpeed, 0.25f, 2.50f, "");
    ImGui::SameLine();
    ImGui::Text("%0.2f", this->AIAttackSpeed);
    ImGui::SameLine();
    ImGui::QuestionMark("Will only change how fast the AI will attack in its turn in case you want a fast game or a slow game.");

    ImGui::EndPopup();
}

void MainWindow::HelpWindow()
{
    const ImVec2& center = ImGui::GetMainViewport()->GetWorkCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500.0f, 350.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 10.0f);
    if (!ImGui::Begin("Help", &OpenHelp, ImGuiWindowFlags_NoResize)) {
        ImGui::End();
        return;
    }

    static ImGui::MarkdownConfig mdconfig;
    static const std::string gamerulesstr =
        "Game Rules:\n"
        "___\n"
        "1.) Everytime a new game starts or there is a change in number of players, whoever has the highest double tile number will be first turn player."
        " In case no one has any double tile number, the player who has the highest numbered card will be first (i.e, 6|5 then 6|4 then 6|3 and so on)."
        " In any case, the player who has the certain card is required to attack with that card first.\n"
        "2.) After a game, whoever wins the previous game will be the first turn player in the next game until there is a change in number of players(refer to 1)\n"
        "3.) You can't pass your turn if you have a tile that could be used for the turn.\n"
        "4.) The first player to empty his deck is the winner. In the event that there are no possible attacks for all players, "
        "The player with the lowest total sum of all cards followed by having less remaining card "
        "followed by being the player with the earlier turn will be the winner.\n"
        "  * Example 1: \n"
        "     Player 2: Sum of cards = 3\n"
        "           Number of remaining cards = 1\n"
        "           Order of turn = 4th turn\n"
        "     Player 3: Sum of cards = 10\n"
        "           Number of remaining cards = 1\n"
        "           Order of turn = 1st turn\n"
        "     Winner: Player 2\n"
        "  * Example 2: \n"
        "     Player 2: Sum of cards = 4\n"
        "           Number of remaining cards = 2\n"
        "           Order of turn = 2nd turn\n"
        "     Player 4: Sum of cards = 4\n"
        "           Number of remaining cards = 1\n"
        "           Order of turn = 4th turn\n"
        "     Winner: Player 4\n"
        "  * Example 3: \n"
        "     Player 3: Sum of cards = 4\n"
        "           Number of remaining cards = 2\n"
        "           Order of turn = 5th turn\n"
        "     Player 5: Sum of cards = 4\n"
        "           Number of remaining cards = 2\n"
        "           Order of turn = 2nd turn\n"
        "     Winner: Player 5\n"
        "";
    static const std::string howtoplaystr =
        "How To Play:\n"
        "___\n"
        "  * You are always Player 1.\n"
        "  * On your turn, click on any domino on your deck. When it's not your turn, your deck will be disabled.\n"
        "  * Drop options will be shown if there are any possible attacks on the board from the domino you have clicked. \n"
        "  * \"PASS!!!\" button will be shown everytime you can't attack on your turn. \n"
        "  * You can restart the game by pressing CTRL + R or the Restart in Game from the main menu bar. However, the number of players and AI difficulty will still be the same."
        " Click \"Change Game Parameters\" instead if you want to change the number of players or AI difficulty. This will also restart the game"
        "";

    if (ImGui::BeginTabBar("HelpTabs")) {
        if (ImGui::BeginTabItem("Game Rules")) {
            ImGui::Markdown(gamerulesstr.c_str(), gamerulesstr.length(), mdconfig);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("How To Play")) {
            ImGui::Markdown(howtoplaystr.c_str(), howtoplaystr.length(), mdconfig);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::PopStyleVar();
    ImGui::End();
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
        auto& dgs = dvars::GameState;
        auto clicked_card = dgs.GetPlayerSelectedCard(0);
        if (clicked_card == nullptr || (dgs.GetNumberOfTurns() == 0 && dgs.IsThereAChangeInPlayer() && dgs.GetFirstTurnTile() != *clicked_card)) {
            return;
        }
        dgs.AddBoardDominoes(*clicked_card, TileDropPosition_Left);
        dgs.TurnAdvance();
        dgs.SetNoClickedCard();
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
    ImGui::BeginDisabled(dgs.GetCurrentTurn() != 0 || ShowPassButton);
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
    
    if (ShowPassButton && ImGui::IsWindowHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(250.0f);
        ImGui::TextUnformatted("YOU HAVE NO POSSIBLE ATTACKS!\nPRESS THE 'PASS!!!' BUTTON!!!");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

//------------------------------------------------------------------------
// GAME RELATED FUNCTIONS
//------------------------------------------------------------------------

void MainWindow::RestartGame()
{
    ShowPassButton = false;
    ShowDropOptions.first = false;
    ShowDropOptions.second = false;
    dvars::GameState.ResetGameState();
    dvars::GameState.InitializeGame(NumberOfPlayer, AIDifficulty_Random, !GameEnd);
    GameStart = true;
    GameEnd   = false;
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
    const char* label(GameEnd ? "PLAY AGAIN!!!" : "GAME START!!!");
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

    if (ai_attack_time < this->AIAttackSpeed) {
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


