#include "lab_m1/card-games/CardGames.h"

#include <cmath>

#include "lab_m1/card-games/layout.h"
#include "lab_m1/card-games/transform2D.h"

using namespace std;
using namespace m1;
using namespace gwent;

namespace {
const float pi = (float)M_PI;
const glm::mat3 identity(1);

const glm::vec3 white(0.95f, 0.95f, 0.95f);
const glm::vec3 black(0.03f, 0.03f, 0.03f);
const glm::vec3 grey(0.4f, 0.4f, 0.42f);
const glm::vec3 gold(0.9f, 0.75f, 0.25f);
const glm::vec3 red(0.85f, 0.15f, 0.15f);
const glm::vec3 plateColor(0.22f, 0.24f, 0.3f);
const glm::vec3 cardBackColor(0.35f, 0.1f, 0.12f);
const glm::vec3 highlightColor(0.2f, 0.5f, 1);

// Indexed by CardType.
const glm::vec3 cardColors[CardTypeCount] = {
    glm::vec3(0.55f, 0.2f, 0.75f),  glm::vec3(0.08f, 0.08f, 0.08f), glm::vec3(0.95f, 0.85f, 0.2f),
    glm::vec3(0.93f, 0.95f, 1),     glm::vec3(0.65f, 0.68f, 0.7f),  glm::vec3(0.3f, 0.45f, 0.6f),
    glm::vec3(0.95f, 0.55f, 0.1f),  glm::vec3(0.15f, 0.35f, 0.9f),  glm::vec3(0.8f, 0.12f, 0.12f),
    glm::vec3(0.15f, 0.65f, 0.25f), glm::vec3(0.95f, 0.6f, 0.75f),  glm::vec3(0.5f, 0.35f, 0.2f),
};

// Indexed by Leader.
const char *const leaderMeshes[LeaderCount] = {"circle", "diamond", "hexagon", "triangle", "star"};
const glm::vec3 leaderColors[LeaderCount] = {
    glm::vec3(0.2f, 0.4f, 0.9f),  glm::vec3(0.75f, 0.75f, 0.8f), glm::vec3(0.75f, 0.2f, 0.2f),
    glm::vec3(0.2f, 0.7f, 0.35f), glm::vec3(0.9f, 0.7f, 0.2f),
};

// Indexed by Lane.
const char *const laneMeshes[LaneCount] = {"triangle", "circle", "square"};
} // namespace

void CardGames::Draw(const string &mesh, const glm::mat3 &parent, glm::vec2 pos, glm::vec2 scale,
                     const glm::vec3 &color, float rotation) {
    RenderMesh2D(meshes[mesh],
                 parent * Translate(pos.x, pos.y) * Rotate(rotation) * Scale(scale.x, scale.y),
                 color);
}

void CardGames::DrawMenu() {
    for (int leader = 0; leader < LeaderCount; leader += 1) {
        glm::vec2 pos = MenuLeaderPos(leader);
        if (leader == selectedLeader) {
            DrawLeader(leader, pos, 1.15f * menuLeaderRadius, highlightColor);
        }
        DrawLeader(leader, pos, menuLeaderRadius, leaderColors[leader]);
    }
}

void CardGames::DrawBoard() {
    for (int who = 0; who < 2; who += 1) {
        DrawSide(who);
    }
    Draw("square", identity, glm::vec2(boardCenterX, screenH / 2), glm::vec2(laneSize.x, 4), gold);

    for (int who = 0; who < 2; who += 1) {
        const Side &side = sides[who];
        for (const vector<Card> &lane : side.lanes) {
            for (const Card &card : lane) {
                DrawCard(card);
            }
        }
        for (const Card &card : side.discard) {
            DrawCard(card);
        }
    }
    for (int who = Enemy; who >= Player; who -= 1) {
        const vector<Card> &hand = sides[who].hand;
        for (int i = 0; i < (int)hand.size(); i += 1) {
            if (who != Player || i != dragged) {
                DrawCard(hand[i]);
            }
        }
    }
    if (dragged >= 0) {
        DrawCard(sides[Player].hand[dragged]);
    }

    if (phase == RoundEnd) {
        DrawOutcome(1);
    } else if (phase == GameOver) {
        DrawOutcome(2);
    }
}

void CardGames::DrawSide(int who) {
    const Side &side = sides[who];
    // Mirrors the panel elements of the enemy around the middle of the board.
    float away = who == Player ? -1.0f : 1.0f;

    Draw("square", identity, glm::vec2(boardCenterX, handY[who]), glm::vec2(laneSize.x, 96),
         glm::vec3(0.12f, 0.13f, 0.16f));

    for (int lane = 0; lane < LaneCount; lane += 1) {
        glm::vec3 color =
            who == Player ? glm::vec3(0.17f, 0.24f, 0.18f) : glm::vec3(0.26f, 0.17f, 0.17f);
        if (HasWeather(who, lane)) {
            color = glm::vec3(0.4f, 0.55f, 0.7f);
        }
        if (who == Player && dragged >= 0 && CanPlace(side.hand[dragged], lane)) {
            color = glm::vec3(0.35f, 0.6f, 0.3f);
        }
        glm::vec2 center = LaneCenter(who, lane);
        Draw("square", identity, center, laneSize - glm::vec2(0, 6), color);
        Draw(laneMeshes[lane], identity, glm::vec2(boardLeft + 22, center.y), glm::vec2(12), grey);
    }

    int deckLayers = ((int)side.deck.size() + 3) / 4;
    for (int i = 0; i < deckLayers; i += 1) {
        glm::vec2 pos = deckPos[who] + glm::vec2(2.0f * i);
        Draw("square", identity, pos, cardSize + glm::vec2(4), black);
        DrawCardBack(Translate(pos.x, pos.y));
    }
    Draw("square", identity, discardPos[who], cardSize + glm::vec2(4),
         glm::vec3(0.12f, 0.13f, 0.16f));

    glm::vec3 leaderColor = side.leaderUsed ? grey : leaderColors[side.leader];
    DrawLeader(side.leader, leaderPos[who], leaderRadius, leaderColor);
    for (int i = 0; i < 2; i += 1) {
        glm::vec2 pos = leaderPos[who] + glm::vec2(75, 36 * i - 18);
        Draw("diamond", identity, pos, glm::vec2(13), i < side.crystals ? red : grey);
    }
    if (side.hornPending) {
        Draw("triangle", identity, leaderPos[who] + glm::vec2(110, 0), glm::vec2(10),
             cardColors[WarHorn], pi / 2);
    }

    for (int i = 0; i < side.health; i += 1) {
        glm::vec2 pos = leaderPos[who] + glm::vec2(13.0f * i - 60, 76 * away);
        Draw("square", identity, pos, glm::vec2(10, 16), red);
    }
    for (int i = 0; i < side.shield; i += 1) {
        glm::vec2 pos = leaderPos[who] + glm::vec2(13.0f * i - 60, 98 * away);
        Draw("square", identity, pos, glm::vec2(10, 12), cardColors[Shield]);
    }
}

void CardGames::DrawCard(const Card &card) {
    float turn = cos(card.flip * pi);
    float size = 1 + 0.25f * card.pulse;
    glm::mat3 model = Translate(card.pos.x, card.pos.y) * Scale(fabs(turn) * size, size);

    Draw("square", model, glm::vec2(0), cardSize + glm::vec2(4), black);
    if (turn < 0) {
        DrawCardBack(model);
        return;
    }
    Draw("square", model, glm::vec2(0), cardSize, card.scorched ? grey : cardColors[card.type]);
    DrawSymbol(card.type, model);

    for (int i = 0; i < EffectiveValue(card); i += 1) {
        glm::vec2 pos(-23, 33 - 7.0f * i);
        Draw("square", model, pos, glm::vec2(6), black);
        Draw("square", model, pos, glm::vec2(4), card.doubled ? gold : white);
    }
    for (int lane = 0; lane < LaneCount; lane += 1) {
        glm::vec2 pos(22, 9 - 9.0f * lane);
        Draw("circle", model, pos, glm::vec2(4), black);
        Draw("circle", model, pos, glm::vec2(3),
             CanPlace(card, lane) ? glm::vec3(0.4f, 0.95f, 1) : plateColor);
    }
}

void CardGames::DrawCardBack(const glm::mat3 &model) {
    Draw("square", model, glm::vec2(0), cardSize, cardBackColor);
    Draw("diamond", model, glm::vec2(0), glm::vec2(22, 30), gold);
    Draw("diamond", model, glm::vec2(0), glm::vec2(15, 21), cardBackColor);
}

void CardGames::DrawSymbol(CardType type, const glm::mat3 &model) {
    // The plate under the symbol tells the category apart: round for permanent cards, a diamond
    // for temporary ones.
    if (IsPermanent(type)) {
        Draw("circle", model, glm::vec2(0), glm::vec2(17), plateColor);
    } else {
        Draw("diamond", model, glm::vec2(0), glm::vec2(20), plateColor);
    }

    switch (type) {
    case Damage:
        Draw("square", model, glm::vec2(0, 2), glm::vec2(4, 20), white);
        Draw("square", model, glm::vec2(0, -5), glm::vec2(14, 3), white);
        break;
    case Heal:
        Draw("square", model, glm::vec2(0), glm::vec2(6, 20), white);
        Draw("square", model, glm::vec2(0), glm::vec2(20, 6), white);
        break;
    case Shield:
        Draw("square", model, glm::vec2(0, 7), glm::vec2(18, 8), white);
        Draw("triangle", model, glm::vec2(0, -2.2f), glm::vec2(10.4f), white, pi);
        break;
    case Spy:
        Draw("circle", model, glm::vec2(0), glm::vec2(13, 8), white);
        Draw("circle", model, glm::vec2(0), glm::vec2(4), black);
        break;
    case Frost:
        for (int i = 0; i < 3; i += 1) {
            Draw("square", model, glm::vec2(0), glm::vec2(3, 24), white, i * pi / 3);
        }
        break;
    case Fog:
        for (int i = -1; i <= 1; i += 1) {
            Draw("square", model, glm::vec2(0, 7 * i), glm::vec2(20 - 6 * abs(i), 4), white);
        }
        break;
    case Rain:
        for (int i = -1; i <= 1; i += 1) {
            Draw("square", model, glm::vec2(7 * i, 0), glm::vec2(3, 14), white, -0.4f);
        }
        break;
    case ClearWeather:
        for (int i = 0; i < 4; i += 1) {
            Draw("square", model, glm::vec2(0), glm::vec2(2.5f, 26), gold, i * pi / 4);
        }
        Draw("circle", model, glm::vec2(0), glm::vec2(7), gold);
        break;
    case WarHorn:
        Draw("triangle", model, glm::vec2(-2, 0), glm::vec2(8, 12), white, pi / 2);
        Draw("square", model, glm::vec2(5, 0), glm::vec2(3, 18), white);
        break;
    case Scorch:
        Draw("triangle", model, glm::vec2(0, 2), glm::vec2(7, 11), cardColors[WarHorn]);
        Draw("circle", model, glm::vec2(0, -4), glm::vec2(7), cardColors[WarHorn]);
        Draw("circle", model, glm::vec2(0, -4), glm::vec2(3.5f), gold);
        break;
    case Medic:
        Draw("circle", model, glm::vec2(0), glm::vec2(12), white);
        Draw("square", model, glm::vec2(0), glm::vec2(5, 16), red);
        Draw("square", model, glm::vec2(0), glm::vec2(16, 5), red);
        break;
    case Decoy:
        Draw("circle", model, glm::vec2(0), glm::vec2(12), white);
        Draw("circle", model, glm::vec2(0), glm::vec2(8), plateColor);
        Draw("circle", model, glm::vec2(0), glm::vec2(4), white);
        break;
    default: break;
    }
}

void CardGames::DrawLeader(int leader, glm::vec2 pos, float radius, const glm::vec3 &color) {
    Draw(leaderMeshes[leader], identity, pos, glm::vec2(radius), color);
}

void CardGames::DrawOutcome(float size) {
    glm::mat3 model = Translate(screenW / 2, screenH / 2) * Scale(size, size);
    Draw("square", model, glm::vec2(0), glm::vec2(268, 128), gold);

    switch (outcome) {
    case Win:
        Draw("square", model, glm::vec2(0), glm::vec2(260, 120), glm::vec3(0.12f, 0.45f, 0.2f));
        Draw("star", model, glm::vec2(0), glm::vec2(45), gold);
        break;
    case Loss:
        Draw("square", model, glm::vec2(0), glm::vec2(260, 120), glm::vec3(0.5f, 0.12f, 0.12f));
        Draw("square", model, glm::vec2(0), glm::vec2(14, 90), white, pi / 4);
        Draw("square", model, glm::vec2(0), glm::vec2(14, 90), white, -pi / 4);
        break;
    case Tie:
        Draw("square", model, glm::vec2(0), glm::vec2(260, 120), grey);
        Draw("square", model, glm::vec2(0, 14), glm::vec2(80, 12), white);
        Draw("square", model, glm::vec2(0, -14), glm::vec2(80, 12), white);
        break;
    }
}
