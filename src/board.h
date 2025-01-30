#pragma once
#include "model.h"

enum class PieceType
{
    NONE = 0,
    PAWN = 1,
    ROOK = 2,
    KNIGHT = 3,
    BISHOP = 4,
    KING = 5,
    QUEEN = 6
};

enum class Color
{
    WHITE,
    BLACK,
    NONE
};

struct Piece
{
    PieceType type{ PieceType::NONE };
    Color color{ Color::WHITE };
};

class Board
{
public:
    Board(glm::vec3 pos, const std::string &res_prefix);
    ~Board();

    void render(uint32_t *scr, float *zbuf);
    glm::ivec3 raycast(int mx, int my);

    void zoom(float dz);
    void rotate(glm::vec3 rot);

    void select(glm::ivec3 coord);
    std::vector<glm::ivec3> possible_moves(glm::ivec3 coord, bool ignore_check = false);

    void make_move(glm::ivec3 src, glm::ivec3 dst);

    void detect_check(Color c);

    Color turn() const { return m_turn; }
    Color in_check() const { return m_check; }

    Piece &at(glm::ivec3 coord);
    void rook_moves(std::vector<glm::ivec3> &moves, glm::ivec3 coord, Piece p);
    void bishop_moves(std::vector<glm::ivec3> &moves, glm::ivec3 coord, Piece p);

    glm::vec3 m_pos, m_rot;
    std::vector<Model> m_models;
    // YXZ
    Piece m_board[8][8][8];

    glm::ivec3 m_selected{ -1 };
    std::vector<glm::ivec3> m_moves;

    Color m_turn{ Color::WHITE };
    Color m_check{ Color::NONE };
};

