// Reproducing Geoffrey J. Gordon results on One-card poker
// https://www.cs.cmu.edu/~ggordon/poker/

#ifndef OPEN_SPIEL_GAMES_ONE_CARD_POKER_H_
#define OPEN_SPIEL_GAMES_ONE_CARD_POKER_H_

#include <memory>
#include <string>

#include "open_spiel/spiel.h"

namespace open_spiel {
namespace onecard_poker {

    // Most of the code's logic can't cope with 3+ players
    // This is just to avoid magic 2's everywhere
    // So don't modify it expecting it to solve a new variant
    constexpr int kPlayersSize = 2;

    // This, however, you can change!
    // Choosing 3 here is equivalent to 2-player Kuhn Poker.
    constexpr int kDeckSize = 13;

    constexpr int kAnte = 1;

    const GameType kGameType{
        .short_name = "onecard_poker",
        .long_name =  "One-card poker",
        .dynamics = GameType::Dynamics::kSequential,
        .chance_mode = GameType::ChanceMode::kExplicitStochastic,
        .information = GameType::Information::kImperfectInformation,
        .utility = GameType::Utility::kZeroSum,
        .reward_model = GameType::RewardModel::kTerminal,
        .max_num_players = kPlayersSize,
        .min_num_players = kPlayersSize,
        .provides_information_state_string = true,
        .provides_information_state_tensor = false,
        .provides_observation_string = false,
        .provides_observation_tensor = false,
        .parameter_specification = {},
        .provides_factored_observation_string = false,
    };

    enum ActionType {
        PassFold = 0,
        BetCall = 1
    };

    auto CardToString(Action card) -> std::string;


    class OneCardState : public State {
    public:
        explicit OneCardState(std::shared_ptr<const Game> game);
        OneCardState(const OneCardState&) = default;

        auto CurrentPlayer() const -> Player override;
        auto LegalActions() const -> std::vector<Action> override;
        auto ActionToString(Player player, Action move) const -> std::string override;
        auto ToString() const -> std::string override;
        auto IsTerminal() const -> bool override;
        auto Returns() const -> std::vector<double> override;
        auto InformationStateString(Player player) const -> std::string override;
        auto Clone() const -> std::unique_ptr<State> override;
        void UndoAction(Player player, Action move) override;
        auto ChanceOutcomes() const -> ActionsAndProbs override;

    protected:
        void DoApplyAction(Action move) override;
    };

    class OneCardGame : public Game {
    public:
        explicit OneCardGame() : Game(kGameType, {}) {}
        auto NewInitialState() const -> std::unique_ptr<State> override { return std::make_unique<OneCardState>(shared_from_this()); }
        auto NumDistinctActions() const -> int override            { return  2; }
        auto MaxChanceOutcomes() const -> int override             { return  kDeckSize; }
        auto NumPlayers() const -> int override                    { return  kPlayersSize; }
        auto MinUtility() const -> double override                 { return  2; };
        auto MaxUtility() const -> double override                 { return -2; };
        auto UtilitySum() const -> absl::optional<double> override { return  0; }
        auto MaxGameLength() const -> int override                 { return  3; }
        auto MaxChanceNodesInHistory() const -> int override       { return  2; }
    };

} // namespace one_card_poker
} // namespace open_spiel

#endif // OPEN_SPIEL_GAMES_ONE_CARD_POKER_H_
