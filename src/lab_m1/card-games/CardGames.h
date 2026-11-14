#pragma once

#include <functional>
#include <random>
#include <string>
#include <vector>

#include "components/simple_scene.h"
#include "lab_m1/card-games/Card.h"

namespace m1 {
class CardGames : public gfxc::SimpleScene {
public:
    void Init() override;

private:
    enum { Player, Enemy };
    enum Phase { Menu, Deal, PlayerTurn, EnemyMove, EnemyFlip, Resolve, RoundEnd, GameOver };
    enum Outcome { Win, Loss, Tie };

    struct Side {
        int health;
        int shield;
        int crystals;
        int leader;
        bool leaderUsed;
        bool hornPending;
        std::vector<gwent::Card> deck;
        std::vector<gwent::Card> hand;
        std::vector<gwent::Card> discard;
        std::vector<gwent::Card> lanes[gwent::LaneCount];
    };

    void FrameStart() override;
    void Update(float deltaTimeSeconds) override;

    void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
    void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
    void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;

    void StartGame(int leader);
    void StartRound(int draws);
    void DrawFromDeck(int who);
    void ToHand(int who, gwent::Card card);
    void Discard(int who, gwent::Card card);
    void PlayCard(int who, int handIndex, int lane, int decoyTarget);
    void StartEnemyMove();
    void UseLeader(int who);

    void UpdatePhase(float deltaTime);
    void ResolveNextStage();
    bool ApplyEffects(gwent::CardType type);
    void ApplyEffect(int who, const gwent::Card &card);
    void ExpireWeather();
    void SweepDestroyed();
    void EndRound();

    void LayoutCards();
    void AnimateCards(float deltaTime);
    void ForEachBoardCard(const std::function<void(gwent::Card &)> &visit);
    bool HasWeather(int who, int lane) const;
    glm::vec2 ToScene(int mouseX, int mouseY) const;

    void Draw(const std::string &mesh, const glm::mat3 &parent, glm::vec2 pos, glm::vec2 scale,
              const glm::vec3 &color, float rotation = 0);
    void DrawMenu();
    void DrawBoard();
    void DrawSide(int who);
    void DrawCard(const gwent::Card &card);
    void DrawCardBack(const glm::mat3 &model);
    void DrawSymbol(gwent::CardType type, const glm::mat3 &model);
    void DrawLeader(int leader, glm::vec2 pos, float radius, const glm::vec3 &color);
    void DrawOutcome(float size);

    Side sides[2];
    Phase phase;
    Outcome outcome;
    float timer;
    int pendingDraws;
    int stage;
    int selectedLeader;
    // Index into the player's hand, or -1.
    int dragged;
    glm::vec2 cursor;
    std::mt19937 rng;
};
} // namespace m1
