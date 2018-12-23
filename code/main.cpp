#include <iostream>
#include <list>
#include <vector>
#include <unordered_set>
#include <set>
#include <unordered_map>

const unsigned long long  FinalState = 1147797409030816545; //1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 0
const unsigned long long Decoder[] =
{
    0x000000000000000F, 0x00000000000000F0, 0x0000000000000F00, 0x000000000000F000,
    0x00000000000F0000, 0x0000000000F00000, 0x000000000F000000, 0x00000000F0000000,
    0x0000000F00000000, 0x000000F000000000, 0x00000F0000000000, 0x0000F00000000000,
    0x000F000000000000, 0x00F0000000000000, 0x0F00000000000000, 0xF000000000000000
};
const unsigned long long Encoder[] =
{
    0xFFFFFFFFFFFFFFF0, 0xFFFFFFFFFFFFFF0F, 0xFFFFFFFFFFFFF0FF, 0xFFFFFFFFFFFF0FFF,
    0xFFFFFFFFFFF0FFFF, 0xFFFFFFFFFF0FFFFF, 0xFFFFFFFFF0FFFFFF, 0xFFFFFFFF0FFFFFFF,
    0xFFFFFFF0FFFFFFFF, 0xFFFFFF0FFFFFFFFF, 0xFFFFF0FFFFFFFFFF, 0xFFFF0FFFFFFFFFFF,
    0xFFF0FFFFFFFFFFFF, 0xFF0FFFFFFFFFFFFF, 0xF0FFFFFFFFFFFFFF, 0x0FFFFFFFFFFFFFFF
};

class GameState
{
public:
    GameState(): position(0) {};

    inline GameState(const GameState &gamestate):
        position(gamestate.position), emptyCell(gamestate.emptyCell), parent(gamestate.parent), h(gamestate.h){};

    inline GameState &operator=(const GameState &gamestate)
    {
        h = gamestate.h;
        position = gamestate.position;
        emptyCell = gamestate.emptyCell;
        parent = gamestate.parent;
        return *this;
    }

    friend bool operator<(const GameState &L, const GameState &R)
    {
        return true;
    }

    inline void Decode(int place, unsigned char value)
    {
        position = (position & Encoder[place]) | (static_cast<unsigned long long>(value) << (place << 2));
    }
    inline unsigned char Encode(int place) const
    {
        return static_cast<unsigned char>((position & Decoder[place]) >> (place << 2));
    }

    int Manhatten() const;
    inline GameState Move(char dir) const;
    bool CheckSolution() const;

    unsigned long long getPosition() const { return position;}
    int getEmptyCell() const {return emptyCell;}
    int getHeur() const {return h;}
    long getParent() const {return parent;}

    void setPosition(unsigned long long _position)  { position = _position; };
    void setParent(long _parent) { parent = _parent; };
    void setemptyCell(int _emptycell) { emptyCell = _emptycell; };
    void setHeur(int _heur) { h = _heur; };

private:
    unsigned long long position;
    int emptyCell;
    int h;
    long parent;
};

int GameState::Manhatten() const
{
    int Manhatten = 0;
    for (int i = 0; i < 16; i++)
    {
        if (Encode(i) == 0)
        {
            continue;
        }
        Manhatten += 87 * (abs(i % 4 - (Encode(i) - 1) % 4) + abs(i / 4 - (Encode(i) - 1) / 4));
    }
    return Manhatten;
}

bool GameState::CheckSolution() const
{
    int inversions = 0;
    for (int i = 0; i < 16; i++)
    {
        for (int j = i + 1; j < 16; j++)
        {
            if (Encode(i) != 0 and Encode(j) != 0 and Encode(i) > Encode(j))
            {
                inversions++;
            }
        }
    }
    bool t1 = (inversions % 2 == 0);
    bool t2 = (emptyCell / 4 % 2 != 0);
    if (t1 == t2)
    {
        return true;
    }
    return false;
}

inline void AStar(const GameState &StartingState, std::list<char> &answers)
{
    GameState vertex;
    GameState gamestate;
    std::set<std::pair<int, GameState>> SetGameStates;
    SetGameStates.insert(std::make_pair(0, StartingState));
    std::unordered_map<unsigned long long, std::pair<char, unsigned long long>> parents;
    parents.emplace(StartingState.getPosition(), std::make_pair(0, 0));
    while(true)
    {
        vertex = SetGameStates.begin()->second;
        std::pair<int, GameState> First (std::make_pair(SetGameStates.begin()->first, SetGameStates.begin()->second));
        SetGameStates.erase(SetGameStates.begin());
        if (vertex.getEmptyCell() > 3)
        {
            gamestate = vertex.Move('U');
            if(parents.find(gamestate.getPosition()) == parents.end())
            {
                parents.emplace(gamestate.getPosition(), std::make_pair('D', vertex.getPosition()));
                if (gamestate.getPosition() == FinalState)
                {
                    break;
                }
                SetGameStates.insert(std::make_pair(gamestate.getParent() + gamestate.getHeur(), gamestate));
            }
        }
        if (vertex.getEmptyCell() % 4 != 0)
        {
            gamestate = vertex.Move('L');
            if (parents.find(gamestate.getPosition()) == parents.end())
            {
                parents.emplace(gamestate.getPosition(), std::make_pair('R', vertex.getPosition()));
                if (gamestate.getPosition() == FinalState)
                {
                    break;
                }
                SetGameStates.insert(std::make_pair(gamestate.getParent() + gamestate.getHeur(), gamestate));
            }
        }
        if (vertex.getEmptyCell() < 12)
        {
            gamestate = vertex.Move('D');
            if (parents.find(gamestate.getPosition()) == parents.end())
            {
                parents.emplace(gamestate.getPosition(), std::make_pair('U', vertex.getPosition()));
                if (gamestate.getPosition() == FinalState)
                {
                    break;
                }
                SetGameStates.insert(std::make_pair(gamestate.getParent() + gamestate.getHeur(), gamestate));
            }
        }
        if (vertex.getEmptyCell() % 4 != 3)
        {
            gamestate = vertex.Move('R');
            if (parents.find(gamestate.getPosition()) == parents.end())
            {
                parents.emplace(gamestate.getPosition(), std::make_pair('L', vertex.getPosition()));
                if (gamestate.getPosition() == FinalState)
                {
                    break;
                }
                SetGameStates.insert(std::make_pair(gamestate.getParent() + gamestate.getHeur(), gamestate));
            }
        }
    }
    unsigned long long u = FinalState;
    std::pair<char, unsigned long long> curMove;
    while (u != StartingState.getPosition())
    {
        curMove = parents[u];
        answers.push_front(curMove.first);
        u = curMove.second;
    }
}

inline GameState GameState::Move(char dir) const
{
    GameState answer(*this);
    switch(dir)
    {
        case 'R':
        {
            int cell = emptyCell + 1;
            unsigned char c = Encode(cell);
            answer.h -= 1.5 * (abs((cell) % 4 - (c - 1) % 4) + abs(cell / 4 - (c - 1) / 4));
            answer.Decode(cell, 0);
            answer.Decode(emptyCell, c);
            answer.h += 1.5 * (abs((emptyCell) % 4 - (c - 1) % 4) + abs(emptyCell / 4 - (c - 1) / 4));
            answer.emptyCell = cell;
            answer.parent++;
            return answer;
        }
        case 'L':
        {
            int cell = emptyCell - 1;
            unsigned char c = Encode(cell);
            answer.h -= 1.5 * (abs((cell) % 4 - (c - 1) % 4) + abs(cell / 4 - (c - 1) / 4));
            answer.Decode(cell, 0);
            answer.Decode(emptyCell, c);
            answer.h += 1.5 * (abs((emptyCell) % 4 - (c - 1) % 4) + abs(emptyCell / 4 - (c - 1) / 4));
            answer.emptyCell = cell;
            answer.parent++;
            return answer;
        }
        case 'D':
        {
            int cell = emptyCell + 4;
            unsigned char c = Encode(cell);
            answer.h -= 1.5 * (abs((cell) % 4 - (c - 1) % 4) + abs(cell / 4 - (c - 1) / 4));
            answer.Decode(cell, 0);
            answer.Decode(emptyCell, c);
            answer.h += 1.5 * (abs((emptyCell) % 4 - (c - 1) % 4) + abs(emptyCell / 4 - (c - 1) / 4));
            answer.emptyCell = cell;
            answer.parent++;
            return answer;
        }
        case 'U':
        {
            int cell = emptyCell - 4;
            unsigned char c = Encode(cell);
            answer.h -= 1.5 * (abs((cell) % 4 - (c - 1) % 4) + abs(cell / 4 - (c - 1) / 4));
            answer.Decode(cell, 0);
            answer.Decode(emptyCell, c);
            answer.h += 1.5 * (abs((emptyCell) % 4 - (c - 1) % 4) + abs(emptyCell / 4 - (c - 1) / 4));
            answer.emptyCell = cell;
            answer.parent++;
            return answer;
        }
    }
}

int main() {
    GameState StartingState;
    int num;

    for(int i = 0; i < 16; i++)
    {
        std::cin >> num;
        StartingState.Decode(i, static_cast<unsigned char>(num));
        if (num == 0)
        {
            StartingState.setemptyCell(i);
        }
    }
    StartingState.setParent(0);
    StartingState.setHeur(StartingState.Manhatten());

    std::list<char> answer;
    if(StartingState.getPosition() == FinalState)
    {
        std::cout << '0';
    }
    else
    if(StartingState.CheckSolution())
    {
        AStar(StartingState, answer);
        std::cout << answer.size() << std::endl;

        for (std::list<char>::iterator i = answer.begin(); i != answer.end(); i++)
        {
            std::cout << *i;
        }
    }
    else
    {
        std::cout << "-1";
    }
    return 0;
}
