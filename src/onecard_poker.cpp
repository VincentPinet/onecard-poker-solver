#include "onecard_poker.hpp"

#include <algorithm>
#include <ranges>
#include <utility>

namespace open_spiel {
namespace onecard_poker {

    auto CardToString(Action card) -> std::string {
        if (card == 12) return "A";
        if (card == 11) return "K";
        if (card == 10) return "Q";
        if (card ==  9) return "J";
        if (card ==  8) return "T";
        return std::to_string(card + 2);
    }

    OneCardState::OneCardState(std::shared_ptr<const Game> game)
        : State(game)
    {}

    auto OneCardState::CurrentPlayer() const -> int {
        if (IsTerminal()) return kTerminalPlayerId;
        if (history_.size() < kPlayersSize) return kChancePlayerId;
        return history_.size() % kPlayersSize;
    }

    auto OneCardState::LegalActions() const -> std::vector<Action> {
        if ( IsTerminal())   return {};
        if (!IsChanceNode()) return {ActionType::PassFold, ActionType::BetCall};

        auto available = [&](auto e) { return !history_.size() || history_[0].action != e; };
        return std::views::iota(0, kDeckSize)
             | std::views::filter(available)
             | std::ranges::to<std::vector<Action>>();
    }

    auto OneCardState::ActionToString(Player player, Action move) const -> std::string {
        if (player == kChancePlayerId)    return "Dealt" + CardToString(move);
        if (move == ActionType::BetCall)  return "BetCall";
        if (move == ActionType::PassFold) return "CheckFold";
        std::unreachable();
    }

    auto OneCardState::ToString() const -> std::string {
        return history_
             | std::views::transform([](auto e) { return std::to_string(e.action); })
             | std::views::join
             | std::ranges::to<std::string>();
    }

    auto OneCardState::IsTerminal() const -> bool {
        if (history_.size() < 4) return false;
        if (history_.size() == 4 &&
            history_[2].action == ActionType::PassFold &&
            history_[3].action == ActionType::BetCall)
            return false;

        return true;
    }

    auto OneCardState::Returns() const -> std::vector<double> {
        if (!IsTerminal()) return std::vector<double>(num_players_, 0.0);

        bool has_called = false;
        bool has_bet = false;
        Player winner = kInvalidPlayer;
        for (int i = kPlayersSize; i < history_.size(); i++) {
            // NON SHOWDOWN
            if (has_bet && history_[i].action == ActionType::PassFold)
                winner = (i + 1) % 2;

            has_called |= history_[i].action == ActionType::BetCall && has_bet;
            has_bet    |= history_[i].action == ActionType::BetCall;
        }

        // SHOWDOWN
        if (winner == kInvalidPlayer)
            winner = history_[1].action > history_[0].action;

        const double wager = kAnte + (has_bet && has_called);
        return {Player{0} == winner ? wager : -wager, Player{1} == winner ? wager : -wager};
    }

    auto OneCardState::InformationStateString(Player player) const -> std::string {
        std::string res{};

        if (history_.size() > player)
            res += CardToString(history_[player].action);

        bool has_bet = false;
        for (int i = kPlayersSize; i < history_.size(); i++) {
            switch (history_[i].action) {
                case ActionType::PassFold: res += has_bet ? "_fold" : "_pass"; break;
                case ActionType::BetCall:  res += has_bet ? "_call" : "_bet" ; break;
            }
            has_bet |= history_[i].action == ActionType::BetCall;
        }

        return res;
    }

    std::unique_ptr<State> OneCardState::Clone() const {
        return std::make_unique<OneCardState>(*this);
    }

    void OneCardState::UndoAction(Player player, Action move) {
        history_.pop_back();
        move_number_--;
    }

    auto OneCardState::ChanceOutcomes() const -> ActionsAndProbs {
        auto available = [&](auto e) { return !history_.size() || history_[0].action != e; };
        auto to_proba  = [&](auto e) -> std::pair<Action, double> {
            return {e, 1.0 / (kDeckSize - history_.size())};
        };

        return std::views::iota(0, kDeckSize)
             | std::views::filter(available)
             | std::views::transform(to_proba)
             | std::ranges::to<ActionsAndProbs>();
    }

    void OneCardState::DoApplyAction(Action move) {
        // Somehow I have nothing to do here
    }

} // namespace one_card_poker
} // namespace open_spiel
