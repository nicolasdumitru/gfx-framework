#pragma once

#include <algorithm>

#include "utils/glm_utils.h"

namespace gwent {
// Declared in the order in which the effects are applied each turn.
// Frost, Fog and Rain line up with Melee, Ranged and Siege.
enum CardType {
    Scorch,
    Spy,
    ClearWeather,
    Frost,
    Fog,
    Rain,
    WarHorn,
    Shield,
    Damage,
    Heal,
    Medic,
    Decoy,
    CardTypeCount
};

enum Lane { Melee, Ranged, Siege, LaneCount, AnyLane = LaneCount };

enum Leader { Foltest, Emhyr, Eredin, Francesca, Radovid, LeaderCount };

const int maxCardValue = 10;

struct Card {
    CardType type;
    int baseValue;
    int value;
    Lane lane;

    glm::vec2 pos;
    glm::vec2 target;
    // 0 shows the face, 1 shows the back.
    float flip;
    float pulse;

    bool faceUp;
    bool playedThisTurn;
    bool scorched;
    bool doubled;

    Card(CardType type, int value, Lane lane, glm::vec2 pos)
        : type(type), baseValue(value), value(value), lane(lane), pos(pos), target(pos), flip(1),
          pulse(0), faceUp(false), playedThisTurn(false), scorched(false), doubled(false) {}

    void Reset() {
        value = baseValue;
        playedThisTurn = false;
        scorched = false;
        doubled = false;
    }
};

inline bool IsPermanent(CardType type) {
    return type == Shield || type == Damage || type == Heal;
}

inline bool IsWeather(CardType type) {
    return type == Frost || type == Fog || type == Rain;
}

inline int EffectiveValue(const Card &card) {
    return card.doubled ? std::min(maxCardValue, 2 * card.value) : card.value;
}

inline bool CanPlace(const Card &card, int lane) {
    return card.lane == AnyLane || card.lane == lane;
}
} // namespace gwent
