#include "bot.h"
#include <bits/stdc++.h>
using namespace std;
using ll=long long;
#define sig(x) (x<0?-1:1)
#define sz(x) ((int)size(x))
#define all(x) begin(x),end(x)
#define all1(x) begin(x)+1,end(x)
template <typename T> using vec=vector<T>;
template <typename T> bool ckmin(T &a, T b) {return b<a ? a=b, true : false;}
template <typename T> bool ckmax(T &a, T b) {return b>a ? a=b, true : false;}

double static_eval(Board &s) {
    map<PieceType, double> value;
    value[PieceType::BISHOP] = 3;
    value[PieceType::KING] = 1000;
    value[PieceType::KNIGHT] = 3;
    value[PieceType::PAWN] = 1;
    value[PieceType::QUEEN] = 9;
    value[PieceType::ROOK] = 5;
    value[PieceType::NONE] = 0;

    double res=0;
    for (int x=0; x<8; ++x) {
        for (int y=0; y<8; ++y) {
            for (int z=0; z<8; ++z) {
                glm::ivec3 pos(x,y,z);
                res += value[s.at(pos).type] * (s.at(pos).color==Color::WHITE ? 1 : -1);
            }
        }
    }

    return res;
}


double minimax(Board &s, int d, int &vis) {
    vis++;
    if (vis%10==0) {
        printf("minimax %d visited\n", vis);
    }

    // gen moves
    vec<act_t> moves;
    for (int x=0; x<8; ++x) {
        for (int y=0; y<8; ++y) {
            for (int z=0; z<8; ++z) {
                glm::ivec3 pos(x,y,z);
                vec<glm::ivec3> dst = s.possible_moves(pos);
                for (glm::ivec3 p : dst) {
                    moves.push_back({pos,p});
                }
            }
        }
    }

    // leaf
    if (d==0 || empty(moves)) return static_eval(s);

    // white or black
    if (s.turn()==Color::WHITE) {
        double mx=-1e9;
        for (act_t &a:moves) {
            Board sp=s;
            sp.make_move(a.src, a.dst);
            double eval = minimax(sp, d-1, vis);

            mx=max(mx, eval);
        }

        return mx;
    } else { // black
        double mn=1e9;
        for (act_t &a:moves) {
            Board sp=s;
            sp.make_move(a.src, a.dst);
            double eval = minimax(sp, d-1, vis);

            mn=min(mn, eval);
        }

        return mn;
    }
}

act_t best_move(Board &s, int d) {
    // gen moves
    vec<act_t> moves;
    for (int x=0; x<8; ++x) {
        for (int y=0; y<8; ++y) {
            for (int z=0; z<8; ++z) {
                glm::ivec3 pos(x,y,z);
                vec<glm::ivec3> dst = s.possible_moves(pos);
                for (glm::ivec3 p : dst) {
                    moves.push_back({pos,p});
                }
            }
        }
    }

    auto st=chrono::steady_clock::now();

    act_t best=moves[0];
    double best_ev;
    int vis=0;

    if (s.turn()==Color::WHITE) {
        double mx=-1e9;
        for (act_t a : moves) {
            Board sp=s;
            sp.make_move(a.src, a.dst);
            if (ckmax(mx, minimax(sp, d, vis))) {
                best=a;
                best_ev=mx;
            }
        }
    } else { // black
        double mn=1e9;
        for (act_t a : moves) {
            Board sp=s;
            sp.make_move(a.src, a.dst);
            if (ckmin(mn, minimax(sp, d, vis))) {
                best=a;
                best_ev=mn;
            }
        }
    }

    int dur=chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now()-st).count();

    printf("visited %d states, took %.2fs\n", vis, (double)dur/1000);
    printf("eval %.2f\n", best_ev);
    return best;
}
