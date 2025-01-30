#include "board.h"
#include "rotate.h"
#include "render.h"
#include <algorithm>

Board::Board(glm::vec3 pos, const std::string &res_prefix)
    : m_pos(pos)
{
    for (int y = 0; y < 8; ++y)
        for (int x = 0; x < 8; ++x)
            for (int z = 0; z < 8; ++z)
                m_board[y][x][z] = Piece{ .type = PieceType::NONE };

                /* if (y == 6) m_board[y][x][z] = Piece{ .type = PieceType::PAWN, .color = Color::WHITE }; */
                /* if (y == 1) m_board[y][x][z] = Piece{ .type = PieceType::PAWN, .color = Color::BLACK }; */

    for (int x = 0; x < 8; ++x)
    {
        for (int z = 0; z < 8; ++z)
        {
            m_board[6][x][z] = Piece{ .type = PieceType::PAWN, .color = Color::WHITE };
            m_board[1][x][z] = Piece{ .type = PieceType::PAWN, .color = Color::BLACK };

            bool rook = (x == 0 || x == 7) || (z == 0 || z == 7);
            if (rook)
            {
                m_board[0][x][z] = Piece{ .type = PieceType::ROOK, .color = Color::BLACK };
                m_board[7][x][z] = Piece{ .type = PieceType::ROOK, .color = Color::WHITE };
            }

            bool knight = (x == 1 || x == 6) || (z == 1 || z == 6);
            if (knight && !rook)
            {
                m_board[0][x][z] = Piece{ .type = PieceType::KNIGHT, .color = Color::BLACK };
                m_board[7][x][z] = Piece{ .type = PieceType::KNIGHT, .color = Color::WHITE };
            }

            bool bishop = (x == 2 || x == 5) || (z == 2 || z == 5);
            if (bishop && !knight && !rook)
            {
                m_board[0][x][z] = Piece{ .type = PieceType::BISHOP, .color = Color::BLACK };
                m_board[7][x][z] = Piece{ .type = PieceType::BISHOP, .color = Color::WHITE };
            }

            m_board[0][3][3] = Piece{ .type = PieceType::KING, .color = Color::BLACK };
            m_board[0][3][4] = Piece{ .type = PieceType::QUEEN, .color = Color::BLACK };
            m_board[0][4][3] = Piece{ .type = PieceType::QUEEN, .color = Color::BLACK };
            m_board[0][4][4] = Piece{ .type = PieceType::QUEEN, .color = Color::BLACK };

            m_board[7][3][3] = Piece{ .type = PieceType::KING, .color = Color::WHITE };
            m_board[7][3][4] = Piece{ .type = PieceType::QUEEN, .color = Color::WHITE };
            m_board[7][4][3] = Piece{ .type = PieceType::QUEEN, .color = Color::WHITE };
            m_board[7][4][4] = Piece{ .type = PieceType::QUEEN, .color = Color::WHITE };
        }
    }

    std::vector<std::string> pieces = {
        "pawn.obj", "rook.obj",
        "knight.obj", "bishop.obj",
        "king.obj", "queen.obj"
    };

    for (auto &p : pieces)
        m_models.emplace_back(Model(res_prefix + p));
}

Board::~Board()
{
}

void Board::render(uint32_t *scr, float *zbuf)
{
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            for (int z = 0; z < 8; ++z)
            {
                bool in_moves = std::find(m_moves.begin(), m_moves.end(), glm::ivec3(x, y, z)) != m_moves.end();
                PieceType type = in_moves ? m_board[m_selected.y][m_selected.x][m_selected.z].type : m_board[y][x][z].type;
                if (m_board[y][x][z].type != PieceType::NONE || in_moves)
                {
                    glm::vec3 pos = m_pos + glm::vec3(x - 3.5, y - 3.5, z - 3.5);
                    SDL_Color color = m_selected == glm::ivec3(x, y, z) ? SDL_Color{ 255, 0, 0 } :
                        (m_board[y][x][z].color == Color::WHITE ? SDL_Color{ 255, 255, 255 } : SDL_Color{ 50, 50, 50 });
                    if (in_moves) color = { 0, 255, 0 };
                    if (m_board[y][x][z].type != PieceType::NONE && in_moves)
                    {
                        color = { 255, 0, 255 };
                        type = m_board[y][x][z].type;
                    }
                    m_models[(int)type - 1].render(pos, m_rot, m_pos, color, scr, zbuf);
                }
            }
        }
    }
}

glm::ivec3 Board::raycast(int mx, int my)
{
    float ha = ((float)mx / SCRSIZE) - .5f;
    float va = ((float)my / SCRSIZE) - .5f;
    float px = std::sin(ha);
    float py = std::sin(va);
    glm::vec3 dir = glm::normalize(glm::vec3(px, py, 1.f));

    float nearest = INFINITY;
    glm::ivec3 res(-1.f);
    bool selected_possible_move = false;

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            for (int z = 0; z < 8; ++z)
            {
                bool in_moves = std::find(m_moves.begin(), m_moves.end(), glm::ivec3(x, y, z)) != m_moves.end();
                PieceType type = in_moves ? m_board[m_selected.y][m_selected.x][m_selected.z].type : m_board[y][x][z].type;
                if (m_board[y][x][z].type != PieceType::NONE || in_moves)
                {
                    glm::vec3 pos = m_pos + glm::vec3(x - 3.5, y - 3.5, z - 3.5);

                    float t;
                    if (m_models[(int)type - 1].ray_intersect(dir, pos, m_rot, m_pos, &t) && t < nearest)
                    {
                        nearest = t;
                        res = glm::ivec3(x, y, z);
                        selected_possible_move = in_moves;
                    }
                }
            }
        }
    }

    if (selected_possible_move)
    {
        m_board[res.y][res.x][res.z] = m_board[m_selected.y][m_selected.x][m_selected.z];
        m_board[m_selected.y][m_selected.x][m_selected.z].type = PieceType::NONE;
        m_selected = glm::ivec3(-1.f);
        m_turn = m_turn == Color::WHITE ? Color::BLACK : Color::WHITE;
        detect_check(m_turn);
        return glm::ivec3(-1.f);
    }

    return res;
}

void Board::zoom(float dz)
{
    m_pos.z += dz;
    if (m_pos.z < 10.f)
        m_pos.z = 10.f;
}

void Board::rotate(glm::vec3 rot)
{
    m_rot += rot;
}

void Board::select(glm::ivec3 coord)
{
    if (coord == glm::ivec3(-1) || at(coord).color == m_turn)
    {
        m_selected = coord;
        m_moves = possible_moves(m_selected);
    }
}

void Board::make_move(glm::ivec3 src, glm::ivec3 dst) {
    m_board[dst.y][dst.x][dst.z] = m_board[src.y][src.x][src.z];
    m_board[src.y][src.x][src.z].type = PieceType::NONE;
    m_turn = m_turn == Color::WHITE ? Color::BLACK : Color::WHITE;
    detect_check(m_turn);
}

void Board::rook_moves(std::vector<glm::ivec3> &moves, glm::ivec3 coord, Piece p)
{
    auto func = [&](glm::ivec3 pos) {
        if (at(pos).type != PieceType::NONE)
        {
            if (at(pos).color != p.color)
                moves.emplace_back(pos);
            return false;
        }

        moves.emplace_back(pos);
        return true;
    };

    for (int y = coord.y + 1; y < 8; ++y)
    {
        glm::ivec3 pos(coord.x, y, coord.z);
        if (!func(pos)) break;
    }

    for (int y = coord.y - 1; y >= 0; --y)
    {
        glm::ivec3 pos(coord.x, y, coord.z);
        if (!func(pos)) break;
    }

    for (int x = coord.x + 1; x < 8; ++x)
    {
        glm::ivec3 pos(x, coord.y, coord.z);
        if (!func(pos)) break;
    }

    for (int x = coord.x - 1; x >= 0; --x)
    {
        glm::ivec3 pos(x, coord.y, coord.z);
        if (!func(pos)) break;
    }

    for (int z = coord.z + 1; z < 8; ++z)
    {
        glm::ivec3 pos(coord.x, coord.y, z);
        if (!func(pos)) break;
    }

    for (int z = coord.z - 1; z >= 0; --z)
    {
        glm::ivec3 pos(coord.x, coord.y, z);
        if (!func(pos)) break;
    }
}

void Board::bishop_moves(std::vector<glm::ivec3> &moves, glm::ivec3 coord, Piece p)
{
    auto func = [&](glm::ivec3 pos) {
        if (at(pos).type != PieceType::NONE)
        {
            if (at(pos).color != p.color)
                moves.emplace_back(pos);
            return false;
        }

        moves.emplace_back(pos);
        return true;
    };

    std::array<bool, 8> cont;
    cont.fill(true);
    for (int y = coord.y - 1; y >= 0; --y)
    {
        int dy = y - coord.y;
        if (cont[0]) if (!func(glm::ivec3(coord.x + dy, y, coord.z + dy))) cont[0] = false;
        if (cont[1]) if (!func(glm::ivec3(coord.x - dy, y, coord.z + dy))) cont[1] = false;
        if (cont[2]) if (!func(glm::ivec3(coord.x + dy, y, coord.z - dy))) cont[2] = false;
        if (cont[3]) if (!func(glm::ivec3(coord.x - dy, y, coord.z - dy))) cont[3] = false;
        if (cont[4]) if (!func(glm::ivec3(coord.x, y, coord.z - dy))) cont[4] = false;
        if (cont[5]) if (!func(glm::ivec3(coord.x, y, coord.z + dy))) cont[5] = false;
        if (cont[6]) if (!func(glm::ivec3(coord.x + dy, y, coord.z))) cont[6] = false;
        if (cont[7]) if (!func(glm::ivec3(coord.x - dy, y, coord.z))) cont[7] = false;

        bool con = false;
        for (size_t i = 0; i < cont.size(); ++i)
            if (cont[i]) con = true;

        if (!con) break;
    }

    cont.fill(true);
    for (int y = coord.y + 1; y < 8; ++y)
    {
        int dy = y - coord.y;
        if (cont[0]) if (!func(glm::ivec3(coord.x + dy, y, coord.z + dy))) cont[0] = false;
        if (cont[1]) if (!func(glm::ivec3(coord.x - dy, y, coord.z + dy))) cont[1] = false;
        if (cont[2]) if (!func(glm::ivec3(coord.x + dy, y, coord.z - dy))) cont[2] = false;
        if (cont[3]) if (!func(glm::ivec3(coord.x - dy, y, coord.z - dy))) cont[3] = false;
        if (cont[4]) if (!func(glm::ivec3(coord.x, y, coord.z - dy))) cont[4] = false;
        if (cont[5]) if (!func(glm::ivec3(coord.x, y, coord.z + dy))) cont[5] = false;
        if (cont[6]) if (!func(glm::ivec3(coord.x + dy, y, coord.z))) cont[6] = false;
        if (cont[7]) if (!func(glm::ivec3(coord.x - dy, y, coord.z))) cont[7] = false;

        bool con = false;
        for (size_t i = 0; i < cont.size(); ++i)
            if (cont[i]) con = true;

        if (!con) break;
    }
}

std::vector<glm::ivec3> Board::possible_moves(glm::ivec3 coord, bool ignore_check)
{
    Piece p = m_board[coord.y][coord.x][coord.z];
    std::vector<glm::ivec3> moves;

    switch (p.type)
    {
    case PieceType::PAWN:
    {
        // Forward moves
        int dy = p.color == Color::WHITE ? -1 : 1;
        glm::ivec3 pos = coord;
        int y = pos.y;
        pos.y += dy;

        if (at(pos).type == PieceType::NONE)
            moves.emplace_back(pos);

        if ((y == 1 && p.color == Color::BLACK) || (y == 6 && p.color == Color::WHITE))
        {
            pos.y += dy;

            if (at(pos).type == PieceType::NONE)
                moves.emplace_back(pos);
        }

        // Take moves
        for (int x = std::max(pos.x - 1, 0); x <= std::min(pos.x + 1, 7); ++x)
        {
            for (int z = std::max(pos.z - 1, 0); z <= std::min(pos.z + 1, 7); ++z)
            {
                if (x == pos.x && z == pos.z) continue;
                glm::ivec3 pos = glm::ivec3(x, y + dy, z);
                if (at(pos).type != PieceType::NONE && at(pos).color != p.color)
                    moves.emplace_back(pos);
            }
        }
    } break;
    case PieceType::ROOK:
    {
        rook_moves(moves, coord, p);
    } break;
    case PieceType::BISHOP:
    {
        bishop_moves(moves, coord, p);
    } break;
    case PieceType::QUEEN:
    {
        rook_moves(moves, coord, p);
        bishop_moves(moves, coord, p);
    } break;
    case PieceType::KING:
    {
        for (int y = std::max(coord.y - 1, 0); y <= std::min(coord.y + 1, 7); ++y)
        {
            for (int x = std::max(coord.x - 1, 0); x <= std::min(coord.x + 1, 7); ++x)
            {
                for (int z = std::max(coord.z - 1, 0); z <= std::min(coord.z + 1, 7); ++z)
                {
                    glm::ivec3 pos(x, y, z);
                    if (pos == coord) continue;

                    if (at(pos).type == PieceType::NONE || (at(pos).type != PieceType::NONE && at(pos).color != p.color))
                        moves.emplace_back(pos);
                }
            }
        }
    } break;
    case PieceType::KNIGHT:
    {
        for (int y = std::max(coord.y - 2, 0); y <= std::min(coord.y + 2, 7); ++y)
        {
            for (int x = std::max(coord.x - 2, 0); x <= std::min(coord.x + 2, 7); ++x)
            {
                for (int z = std::max(coord.z - 2, 0); z <= std::min(coord.z + 2, 7); ++z)
                {
                    if (std::abs(y - coord.y) * std::abs(x - coord.x) * std::abs(z - coord.z) == 2)
                    {
                        glm::ivec3 pos(x, y, z);
                        if (at(pos).type == PieceType::NONE || (at(pos).type != PieceType::NONE && at(pos).color != p.color))
                            moves.emplace_back(pos);

                    }
                }
            }
        }
    } break;
    default: break;
    }

    if (!ignore_check/* && m_check != Color::NONE*/)
    {
        std::vector<glm::ivec3> legal_moves;

        for (auto &m : moves)
        {
            // Simulate move and check if king is still in check
            Piece orig = at(m);
            at(m) = p;
            PieceType type = at(coord).type;
            at(coord).type = PieceType::NONE;

            Color check = m_check;
            detect_check(m_turn);
            if (m_check == Color::NONE)
                legal_moves.emplace_back(m);
            m_check = check;

            at(m) = orig;
            at(coord).type = type;
        }

        moves = legal_moves;
    }

    return moves;
}

Piece &Board::at(glm::ivec3 coord)
{
    return m_board[coord.y][coord.x][coord.z];
}

void Board::detect_check(Color c)
{
    std::vector<glm::ivec3> moves;
    glm::ivec3 king_pos;

    for (int y = 0; y < 8; ++y)
        for (int x = 0; x < 8; ++x)
            for (int z = 0; z < 8; ++z)
            {
                glm::ivec3 pos(x, y, z);
                if (at(pos).color != c)
                {
                    std::vector<glm::ivec3> tmp = possible_moves(pos, true);
                    moves.insert(moves.end(), tmp.begin(), tmp.end());
                }

                if (at(pos).type == PieceType::KING && at(pos).color == c)
                    king_pos = pos;
            }

    if (std::find(moves.begin(), moves.end(), king_pos) != moves.end())
        m_check = c;
    else
        m_check = Color::NONE;
}

