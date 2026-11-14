#pragma once

#include "utils/glm_utils.h"

// Everything is laid out in a fixed 1280x720 scene, whatever the window size is.
namespace gwent {
const float screenW = 1280;
const float screenH = 720;

const glm::vec2 cardSize(56, 76);

const float boardLeft = 220;
const float boardRight = 1060;
const float boardBottom = 108;
const float boardCenterX = (boardLeft + boardRight) / 2;
const float laneHeight = 84;
const glm::vec2 laneSize(boardRight - boardLeft, laneHeight);

// Indexed by side: player first, enemy second.
const float handY[2] = {52, 668};
const glm::vec2 deckPos[2] = {glm::vec2(1115, 160), glm::vec2(1115, 560)};
const glm::vec2 discardPos[2] = {glm::vec2(1205, 160), glm::vec2(1205, 560)};
const glm::vec2 leaderPos[2] = {glm::vec2(80, 240), glm::vec2(80, 480)};
const float leaderRadius = 36;

const float menuLeaderRadius = 70;

inline glm::vec2 MenuLeaderPos(int leader) {
    return glm::vec2(240 + 200 * leader, screenH / 2);
}

// Melee is the lane next to the middle of the board on both halves.
inline glm::vec2 LaneCenter(int side, int lane) {
    int row = side == 0 ? 2 - lane : 3 + lane;
    return glm::vec2(boardCenterX, boardBottom + laneHeight * (row + 0.5f));
}

inline bool InRect(glm::vec2 point, glm::vec2 center, glm::vec2 size) {
    glm::vec2 min = center - size / 2.0f;
    glm::vec2 max = center + size / 2.0f;
    return min.x <= point.x && point.x <= max.x && min.y <= point.y && point.y <= max.y;
}
} // namespace gwent
