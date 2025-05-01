#include "onecard_poker.hpp"

#include "open_spiel/algorithms/cfr.h"
#include "open_spiel/algorithms/tabular_exploitability.h"
#include "open_spiel/spiel.h"

#include <memory>
#include <print>

using namespace open_spiel;
using namespace open_spiel::algorithms;
using namespace open_spiel::onecard_poker;

void print_header() {
    std::print("Holding:    ");
    for (int i = 0; i < kDeckSize; i++)
        std::print("{:<7}", CardToString(i));
    std::println();
}

int main() {

    REGISTER_SPIEL_GAME(kGameType, [](const GameParameters&) {
        return std::make_shared<const OneCardGame>();
    });

    const double gap = 0.00004;

    auto game = LoadGame("onecard_poker");
    CFRSolver solver(*game);
    while (Exploitability(*game, *solver.AveragePolicy()) > gap)
        solver.EvaluateAndUpdatePolicy();

    auto ret = solver.TabularAveragePolicy();

    std::cout << "Exploitability : " << Exploitability(*game, *solver.AveragePolicy()) << std::endl;
    std::println();

    print_header();
    for (const auto& a : {"pass", "bet"}) {
        std::print("On {:4}:    ", a);
        for (int i = 0; i < kDeckSize; i++) {
            auto map = ret.GetStatePolicyAsMap(CardToString(i) + '_' + a);
            std::print("{:5.3f}  ", map[ActionType::BetCall]);
        }
        std::println();
    }

    std::println();

    print_header();
    for (const auto &a : {"", "_pass_bet"}) {
        std::print("{} round:  ", !strlen(a) ? "1st" : "2nd");
        for (int i = 0; i < kDeckSize; i++) {
            auto map = ret.GetStatePolicyAsMap(CardToString(i) + a);
            std::print("{:5.3f}  ", map[ActionType::BetCall]);
        }
        std::println();
    }
}
