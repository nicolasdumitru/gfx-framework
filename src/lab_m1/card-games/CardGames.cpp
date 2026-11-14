#include "lab_m1/card-games/CardGames.h"

#include <algorithm>

#include "lab_m1/card-games/layout.h"
#include "lab_m1/card-games/shapes.h"

using namespace std;
using namespace m1;
using namespace gwent;

namespace {
const int maxHealth = 15;
const int startingHand = 10;
const int crystalCount = 2;

struct DeckEntry {
    CardType type;
    int value;
    Lane lane;
};

const DeckEntry deckList[] = {
    {Damage, 3, Melee},  {Damage, 2, Melee},  {Damage, 2, Ranged},        {Damage, 2, Ranged},
    {Damage, 3, Siege},  {Damage, 1, Siege},  {Heal, 2, Melee},           {Heal, 1, Ranged},
    {Heal, 2, Siege},    {Shield, 2, Melee},  {Shield, 2, Ranged},        {Shield, 1, Siege},
    {Spy, 1, AnyLane},   {Spy, 1, AnyLane},   {Scorch, 2, AnyLane},       {Frost, 2, Melee},
    {Fog, 2, Ranged},    {Rain, 2, Siege},    {ClearWeather, 1, AnyLane}, {WarHorn, 2, AnyLane},
    {Decoy, 1, AnyLane}, {Medic, 1, AnyLane},
};

void LayoutRow(vector<Card> &cards, glm::vec2 center, float width) {
    int count = (int)cards.size();
    float spacing = cardSize.x + 8;
    if (count > 1) {
        spacing = min(spacing, (width - cardSize.x) / (count - 1));
    }
    for (int i = 0; i < count; i += 1) {
        cards[i].target = center + glm::vec2(spacing * (i - (count - 1) / 2.0f), 0);
    }
}

void AnimateCard(Card &card, float deltaTime) {
    card.pos += (card.target - card.pos) * min(1.0f, 10 * deltaTime);
    float flipStep = 3 * deltaTime;
    card.flip += glm::clamp((card.faceUp ? 0.0f : 1.0f) - card.flip, -flipStep, flipStep);
    card.pulse = max(0.0f, card.pulse - 2.5f * deltaTime);
}
} // namespace

void CardGames::Init() {
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, screenW, 0, screenH, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    AddMeshToList(CreateSquare("square"));
    AddMeshToList(CreatePolygon("circle", 40));
    AddMeshToList(CreatePolygon("hexagon", 6));
    AddMeshToList(CreatePolygon("diamond", 4));
    AddMeshToList(CreatePolygon("triangle", 3));
    AddMeshToList(CreateStar("star", 5, 0.45f));

    rng.seed(random_device()());
    phase = Menu;
    selectedLeader = -1;
    dragged = -1;
}

void CardGames::FrameStart() {
    glClearColor(0.07f, 0.08f, 0.1f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Every mesh sits at z = 0, so what is drawn later has to cover what was drawn earlier.
    glDisable(GL_DEPTH_TEST);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void CardGames::Update(float deltaTimeSeconds) {
    if (phase == Menu) {
        DrawMenu();
        return;
    }

    UpdatePhase(deltaTimeSeconds);
    LayoutCards();
    AnimateCards(deltaTimeSeconds);
    DrawBoard();
}

void CardGames::StartGame(int leader) {
    for (int who = 0; who < 2; who += 1) {
        Side &side = sides[who];
        side = Side();
        side.crystals = crystalCount;
        side.leader = who == Player ? leader : (int)(rng() % LeaderCount);
        side.leaderUsed = false;
        for (const DeckEntry &entry : deckList) {
            side.deck.push_back(Card(entry.type, entry.value, entry.lane, deckPos[who]));
        }
        shuffle(side.deck.begin(), side.deck.end(), rng);
    }
    StartRound(startingHand);
}

void CardGames::StartRound(int draws) {
    for (int who = 0; who < 2; who += 1) {
        Side &side = sides[who];
        for (vector<Card> &lane : side.lanes) {
            for (const Card &card : lane) {
                Discard(who, card);
            }
            lane.clear();
        }
        side.health = maxHealth;
        side.shield = 0;
        side.hornPending = false;
    }
    pendingDraws = draws;
    timer = 0;
    phase = Deal;
}

void CardGames::DrawFromDeck(int who) {
    vector<Card> &deck = sides[who].deck;
    if (deck.empty()) {
        return;
    }
    ToHand(who, deck.back());
    deck.pop_back();
}

void CardGames::ToHand(int who, Card card) {
    card.Reset();
    card.faceUp = who == Player;
    sides[who].hand.push_back(card);
}

void CardGames::Discard(int who, Card card) {
    card.Reset();
    card.faceUp = true;
    sides[who].discard.push_back(card);
}

void CardGames::PlayCard(int who, int handIndex, int lane, int decoyTarget) {
    Side &side = sides[who];
    Card card = side.hand[handIndex];
    side.hand.erase(side.hand.begin() + handIndex);

    if (side.hornPending) {
        card.value = min(maxCardValue, 2 * card.value);
        side.hornPending = false;
    }
    card.playedThisTurn = true;

    if (decoyTarget < 0) {
        side.lanes[lane].push_back(card);
        return;
    }
    swap(card, side.lanes[lane][decoyTarget]);
    ToHand(who, card);
}

void CardGames::StartEnemyMove() {
    Side &enemy = sides[Enemy];
    if (!enemy.leaderUsed && rng() % 4 == 0) {
        UseLeader(Enemy);
    }
    if (enemy.hand.empty()) {
        stage = 0;
        timer = 0;
        phase = Resolve;
        return;
    }

    int index = rng() % enemy.hand.size();
    const Card &card = enemy.hand[index];
    int lane = card.lane == AnyLane ? rng() % LaneCount : card.lane;
    int decoyTarget = -1;
    if (card.type == Decoy) {
        vector<int> occupied;
        for (int i = 0; i < LaneCount; i += 1) {
            if (!enemy.lanes[i].empty()) {
                occupied.push_back(i);
            }
        }
        if (!occupied.empty()) {
            lane = occupied[rng() % occupied.size()];
            decoyTarget = rng() % enemy.lanes[lane].size();
        }
    }
    PlayCard(Enemy, index, lane, decoyTarget);
    timer = 0.7f;
    phase = EnemyMove;
}

void CardGames::UseLeader(int who) {
    Side &own = sides[who];
    Side &opponent = sides[1 - who];

    switch (own.leader) {
    case Foltest:
        ExpireWeather();
        SweepDestroyed();
        break;
    case Emhyr: {
        vector<int> hidden;
        for (int i = 0; i < (int)opponent.hand.size(); i += 1) {
            if (!opponent.hand[i].faceUp) {
                hidden.push_back(i);
            }
        }
        shuffle(hidden.begin(), hidden.end(), rng);
        for (int i = 0; i < min(3, (int)hidden.size()); i += 1) {
            opponent.hand[hidden[i]].faceUp = true;
        }
        break;
    }
    case Eredin:
        for (Card &card : own.lanes[Melee]) {
            card.doubled = true;
        }
        break;
    case Francesca: {
        vector<Card> &melee = opponent.lanes[Melee];
        if (melee.empty()) {
            return;
        }
        auto strongest = max_element(melee.begin(), melee.end(), [](const Card &a, const Card &b) {
            return EffectiveValue(a) < EffectiveValue(b);
        });
        Discard(1 - who, *strongest);
        melee.erase(strongest);
        break;
    }
    case Radovid:
        for (Card &card : own.lanes[Siege]) {
            card.value = min(maxCardValue, 2 * card.value);
        }
        break;
    }
    own.leaderUsed = true;
}

void CardGames::UpdatePhase(float deltaTime) {
    timer -= deltaTime;

    switch (phase) {
    case Deal:
        if (timer > 0) {
            break;
        }
        if (pendingDraws == 0) {
            phase = PlayerTurn;
            break;
        }
        DrawFromDeck(Player);
        DrawFromDeck(Enemy);
        pendingDraws -= 1;
        timer = 0.15f;
        break;
    case PlayerTurn:
        if (sides[Player].hand.empty()) {
            StartEnemyMove();
        }
        break;
    case EnemyMove:
        if (timer > 0) {
            break;
        }
        ForEachBoardCard([](Card &card) {
            if (card.playedThisTurn) {
                card.faceUp = true;
            }
        });
        timer = 0.6f;
        phase = EnemyFlip;
        break;
    case EnemyFlip:
        if (timer <= 0) {
            stage = 0;
            phase = Resolve;
        }
        break;
    case Resolve:
        if (timer <= 0) {
            ResolveNextStage();
        }
        break;
    case RoundEnd:
        if (timer > 0) {
            break;
        }
        if (sides[Player].crystals > 0 && sides[Enemy].crystals > 0) {
            StartRound(1);
            break;
        }
        outcome = sides[Player].crystals > 0 ? Win : sides[Enemy].crystals > 0 ? Loss : Tie;
        phase = GameOver;
        break;
    default: break;
    }
}

void CardGames::ResolveNextStage() {
    while (stage < CardTypeCount) {
        bool applied = ApplyEffects((CardType)stage);
        stage += 1;
        if (sides[Player].health == 0 || sides[Enemy].health == 0) {
            EndRound();
            return;
        }
        if (applied) {
            timer = 0.5f;
            return;
        }
    }

    ForEachBoardCard([](Card &card) {
        card.playedThisTurn = false;
        card.scorched = false;
        card.doubled = false;
    });
    if (sides[Player].hand.empty() && sides[Enemy].hand.empty()) {
        EndRound();
    } else {
        phase = PlayerTurn;
    }
}

bool CardGames::ApplyEffects(CardType type) {
    struct Source {
        int who;
        Card *card;
    };
    vector<Source> sources;
    for (int who = 0; who < 2; who += 1) {
        for (vector<Card> &lane : sides[who].lanes) {
            for (Card &card : lane) {
                if (card.type == type && !card.scorched) {
                    sources.push_back({who, &card});
                }
            }
        }
    }

    // The pointers stay valid because no effect adds or removes board cards; cards that run out
    // of value are only collected by SweepDestroyed below.
    for (const Source &source : sources) {
        ApplyEffect(source.who, *source.card);
    }
    for (const Source &source : sources) {
        source.card->pulse = 1;
        if (!IsPermanent(type)) {
            source.card->value -= 1;
        }
    }
    SweepDestroyed();
    return !sources.empty();
}

void CardGames::ApplyEffect(int who, const Card &card) {
    Side &own = sides[who];
    Side &opponent = sides[1 - who];
    int value = EffectiveValue(card);

    switch (card.type) {
    case Scorch:
        for (vector<Card> &lane : opponent.lanes) {
            for (Card &other : lane) {
                if (other.playedThisTurn && EffectiveValue(other) >= value) {
                    other.scorched = true;
                }
            }
        }
        break;
    case Spy: {
        if (opponent.hand.empty()) {
            break;
        }
        int index = rng() % opponent.hand.size();
        ToHand(who, opponent.hand[index]);
        opponent.hand.erase(opponent.hand.begin() + index);
        break;
    }
    case ClearWeather: ExpireWeather(); break;
    case Frost:
    case Fog:
    case Rain:
        for (Card &other : opponent.lanes[card.lane]) {
            other.value -= 1;
        }
        break;
    case WarHorn: own.hornPending = true; break;
    case Shield: own.shield = min(maxHealth, own.shield + value); break;
    case Damage: {
        int absorbed = min(opponent.shield, value);
        opponent.shield -= absorbed;
        opponent.health = max(0, opponent.health - (value - absorbed));
        break;
    }
    case Heal: own.health = min(maxHealth, own.health + value); break;
    case Medic: {
        // Medics are left in the pile so that two of them cannot keep reviving each other.
        vector<int> revivable;
        for (int i = 0; i < (int)own.discard.size(); i += 1) {
            if (own.discard[i].type != Medic) {
                revivable.push_back(i);
            }
        }
        if (revivable.empty()) {
            break;
        }
        int index = revivable[rng() % revivable.size()];
        ToHand(who, own.discard[index]);
        own.discard.erase(own.discard.begin() + index);
        break;
    }
    default: break;
    }
}

void CardGames::ExpireWeather() {
    ForEachBoardCard([](Card &card) {
        if (IsWeather(card.type)) {
            card.value = 0;
        }
    });
}

void CardGames::SweepDestroyed() {
    for (int who = 0; who < 2; who += 1) {
        for (vector<Card> &lane : sides[who].lanes) {
            for (size_t i = 0; i < lane.size();) {
                if (lane[i].value > 0) {
                    i += 1;
                    continue;
                }
                Discard(who, lane[i]);
                lane.erase(lane.begin() + i);
            }
        }
    }
}

void CardGames::EndRound() {
    int player = sides[Player].health;
    int enemy = sides[Enemy].health;
    if (player <= enemy) {
        sides[Player].crystals -= 1;
    }
    if (enemy <= player) {
        sides[Enemy].crystals -= 1;
    }
    outcome = player > enemy ? Win : player < enemy ? Loss : Tie;
    timer = 1.8f;
    phase = RoundEnd;
}

void CardGames::LayoutCards() {
    for (int who = 0; who < 2; who += 1) {
        Side &side = sides[who];
        LayoutRow(side.hand, glm::vec2(boardCenterX, handY[who]), laneSize.x - 20);
        for (int lane = 0; lane < LaneCount; lane += 1) {
            // Shifted right to leave room for the lane marker.
            LayoutRow(side.lanes[lane], LaneCenter(who, lane) + glm::vec2(20, 0), laneSize.x - 70);
        }
        for (Card &card : side.discard) {
            card.target = discardPos[who];
        }
    }
}

void CardGames::AnimateCards(float deltaTime) {
    for (Side &side : sides) {
        for (Card &card : side.hand) {
            AnimateCard(card, deltaTime);
        }
        for (Card &card : side.discard) {
            AnimateCard(card, deltaTime);
        }
    }
    ForEachBoardCard([deltaTime](Card &card) { AnimateCard(card, deltaTime); });

    if (dragged >= 0) {
        sides[Player].hand[dragged].pos = cursor;
    }
}

void CardGames::ForEachBoardCard(const function<void(Card &)> &visit) {
    for (Side &side : sides) {
        for (vector<Card> &lane : side.lanes) {
            for (Card &card : lane) {
                visit(card);
            }
        }
    }
}

bool CardGames::HasWeather(int who, int lane) const {
    for (const Card &card : sides[1 - who].lanes[lane]) {
        if (card.type == Frost + lane && !card.scorched) {
            return true;
        }
    }
    return false;
}

glm::vec2 CardGames::ToScene(int mouseX, int mouseY) const {
    // The cursor is reported in screen coordinates, which on a scaled display are not the
    // framebuffer pixels that GetResolution() returns.
    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    return glm::vec2(mouseX * screenW / width, (height - mouseY) * screenH / height);
}

void CardGames::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    cursor = ToScene(mouseX, mouseY);
}

void CardGames::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
    if (!IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        return;
    }
    cursor = ToScene(mouseX, mouseY);

    switch (phase) {
    case Menu:
        for (int leader = 0; leader < LeaderCount; leader += 1) {
            if (!InRect(cursor, MenuLeaderPos(leader), glm::vec2(2 * menuLeaderRadius))) {
                continue;
            }
            if (selectedLeader == leader) {
                StartGame(leader);
            } else {
                selectedLeader = leader;
            }
            break;
        }
        break;
    case PlayerTurn: {
        const Side &player = sides[Player];
        if (!player.leaderUsed && InRect(cursor, leaderPos[Player], glm::vec2(2 * leaderRadius))) {
            UseLeader(Player);
            break;
        }
        // Cards further right are drawn on top when the hand overlaps.
        for (int i = (int)player.hand.size() - 1; i >= 0; i -= 1) {
            if (InRect(cursor, player.hand[i].pos, cardSize)) {
                dragged = i;
                break;
            }
        }
        break;
    }
    case GameOver:
        selectedLeader = -1;
        phase = Menu;
        break;
    default: break;
    }
}

void CardGames::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
    if (!IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) || dragged < 0) {
        return;
    }
    cursor = ToScene(mouseX, mouseY);
    int index = dragged;
    dragged = -1;

    Side &player = sides[Player];
    const Card &card = player.hand[index];
    for (int lane = 0; lane < LaneCount; lane += 1) {
        if (!CanPlace(card, lane) || !InRect(cursor, LaneCenter(Player, lane), laneSize)) {
            continue;
        }
        int decoyTarget = -1;
        if (card.type == Decoy) {
            for (int i = 0; i < (int)player.lanes[lane].size(); i += 1) {
                if (InRect(cursor, player.lanes[lane][i].pos, cardSize)) {
                    decoyTarget = i;
                }
            }
        }
        PlayCard(Player, index, lane, decoyTarget);
        StartEnemyMove();
        return;
    }
}
