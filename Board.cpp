#include <iostream>
#include <sstream>

#include "Board.h"

std::string Node::toString() const
{
    return std::to_string(x) + " " + std::to_string(y);
}

Node* Move::from() const
{
    return origin;
}

Node* Move::to() const
{
    return origin->neighbours[direction];
}

std::string Capture::toString() const
{
    return "Capturing " + std::to_string(capturedNodes.size()) + " nodes";
}

std::string Turn::toString() const
{
    std::string nextTurnString = nextTurn ? "\n" + nextTurn->toString() : "\n";
    return move->toString() + "\n" + capture->toString() + nextTurnString;
}

Board::Board()
{
}

Board::~Board()
{
}

void Board::parse(std::string boardContent)
{
    std::istringstream stream(boardContent);
    std::string str;

    std::vector<std::string> lines;
    while (getline(stream, str))
    {
        // Remove carriage return if a server with Windows line endings is used
        if (str[0] == '\r' && str.length() > 0)
        {
            str = str.substr(1);
        }
        lines.push_back(str);
    }

    for (int y = 0;; y++)
    {
        int inputRow = y * 2 + 1;
        if (inputRow >= lines.size())
        {
            break;
        }

        auto line = lines[inputRow];
        for (size_t x = 0; x < line.size(); x++)
        {
            if (!isPositionInBounds(x, y))
            {
                break;
            }

            int inputColumn = x * 2 + 2;
            char character = line[inputColumn];

            auto cell = getCell(x, y);

            // TODO: Assign this earlier?
            cell->x = x;
            cell->y = y;

            if (character == 'O')
            {
                cell->state = State::WHITE;
            }
            else if (character == '#')
            {
                cell->state = State::BLACK;
            }
            // else if (character == '*') // current cell -> needed?
            else
            {
                cell->state = State::EMPTY;
            }

            // TODO: only setup neighbour connections during first parsing phase @rene
            if (isPositionInBounds(x - 1, y - 1) && lines[inputRow - 1][inputColumn - 1] == '\\')
            {
                cell->neighbours[0] = getCell(x - 1, y - 1);
            }
            if (isPositionInBounds(x, y - 1) && lines[inputRow - 1][inputColumn] == '|')
            {
                cell->neighbours[1] = getCell(x, y - 1);
            }
            if (isPositionInBounds(x + 1, y - 1) && lines[inputRow - 1][inputColumn + 1] == '/')
            {
                cell->neighbours[2] = getCell(x + 1, y - 1);
            }
            if (isPositionInBounds(x - 1, y) && lines[inputRow][inputColumn - 1] == '-')
            {
                cell->neighbours[3] = getCell(x - 1, y);
            }
            if (isPositionInBounds(x + 1, y) && lines[inputRow][inputColumn + 1] == '-')
            {
                cell->neighbours[4] = getCell(x + 1, y);
            }
            if (isPositionInBounds(x - 1, y + 1) && lines[inputRow + 1][inputColumn - 1] == '/')
            {
                cell->neighbours[5] = getCell(x - 1, y + 1);
            }
            if (isPositionInBounds(x, y + 1) && lines[inputRow + 1][inputColumn] == '|')
            {
                cell->neighbours[6] = getCell(x, y + 1);
            }
            if (isPositionInBounds(x + 1, y + 1) && lines[inputRow + 1][inputColumn + 1] == '\\')
            {
                cell->neighbours[7] = getCell(x + 1, y + 1);
            }
        }
    }
}

Node* Board::getCell(int x, int y)
{
    if (!isPositionInBounds(x, y))
    {
        return nullptr;
    }
    return &cells[y][x];
}

bool Board::isPositionInBounds(int x, int y)
{
    return x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT;
}

// temp implementation for stdin input by user: TODO add logic implementation here
std::string Board::getPosition(Mode mode)
{
    if (mode == Mode::SELECT_STONE)
    {
        auto turns = findTurns(State::WHITE);
        std::cout << turns.size() << " available Turns: \n";
        for (auto turn : turns)
        {
            std::cout << turn.toString() << "\n";
        }
        std::cout << "select stone: ";
    }
    else if (mode == Mode::SELECT_MOVEMENT)
        std::cout << "select location: ";
    else if (mode == Mode::SELECT_CAPTURE)
        std::cout << "select capture: ";

    std::string input;
    // std::cin >> input; // does not parse whitespaces!
    std::getline(std::cin, input);

    // TODO: validate input (for user)
    return input;
}

const std::vector<Turn> Board::findTurns(State movingState)
{
    std::vector<Turn> turns;

    // iterate over board
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            auto cell = getCell(x, y);
            // if we find a stone of my color
            if (cell->state == movingState)
            {
                // iterate over neighbors
                for (int i = 0; i < 8; i++)
                {
                    auto neighbour = cell->neighbours[i];

                    // if neighbor node is empty
                    if (neighbour != nullptr && neighbour->state == State::EMPTY)
                    {
                        // this is a possible turn!

                        Move* move = new Move(cell, i);
                        Capture* captureForward = new Capture(getCapturesInDirection(*move, false));
                        Capture* captureBackward = new Capture(getCapturesInDirection(*move, true));

                        turns.emplace_back(Turn(move, captureForward));
                        turns.emplace_back(Turn(move, captureBackward));
                    }
                }
            }
        }
    }

    return turns;
}

// returns movable pieces for desired color, and integer denoting in which direction it can move
// this means pieces can occur twice in the returned vector if they can move in two or more directions
const std::vector<Move> Board::findMoves(State movingState)
{
    // if we have no moving piece its a normal turn else we continue moving that piece
    return movingPiece != nullptr ? findContinuingMoves() : findFirstMoves(movingState);
}

const std::vector<Move> Board::findFirstMoves(State movingState)
{
    std::vector<Move> nonCapturingMoves;
    std::vector<Move> capturingMoves;

    // iterate over board
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            auto cell = getCell(x, y);
            // if we find a stone of my color
            if (cell->state == movingState)
            {
                // iterate over neighbors
                for (int i = 0; i < 8; i++)
                {
                    auto neighbour = cell->neighbours[i];
                    // if neighbor node is empty
                    if (neighbour != nullptr && neighbour->state == State::EMPTY)
                    {
                        Move move(cell, i);
                        // check if move captures and add to corresponding vector
                        if (getBestCaptures(move).size() >= 1)
                        {
                            capturingMoves.push_back(move);
                        }
                        else
                        {
                            nonCapturingMoves.push_back(move);
                        }
                    }
                }
            }
        }
    }
    // capture is mandatory thus only return those moves if available
    return capturingMoves.size() > 0 ? capturingMoves : nonCapturingMoves;
}

const std::vector<Move> Board::findContinuingMoves()
{
    std::vector<Move> nonCapturingMoves;
    std::vector<Move> capturingMoves;

    // iterate over neighbors of current moving piece
    for (int i = 0; i < 8; i++)
    {
        auto neighbour = movingPiece->neighbours[i];

        // if neighbor is empty
        if (neighbour != nullptr && neighbour->state == State::EMPTY)
        {
            Move move(movingPiece, i);
            // check for captures and add to corresponding vector
            if (getBestCaptures(move).size() >= 1)
            {
                capturingMoves.push_back(move);
            }
            else
            {
                nonCapturingMoves.push_back(move);
            }
        }
    }
    // capture is mandatory thus only return those moves if available
    return capturingMoves.size() > 0 ? capturingMoves : nonCapturingMoves;
}

const std::vector<Node*> Board::getCapturesInDirection(const Move& move, bool reverse_direction)
{
    std::vector<Node*> captures;

    State myState = move.from()->state;

    int direction;
    Node* currentNeighbour;

    if (reverse_direction)
    {
        direction = 7 - move.direction;
        currentNeighbour = move.from()->neighbours[direction];
    }
    else
    {
        direction = move.direction;
        currentNeighbour = move.to()->neighbours[direction];
    }

    // if there is a field, and on that field is a stone of the other color count up
    while (currentNeighbour != nullptr && currentNeighbour->state != State::EMPTY && currentNeighbour->state != myState)
    {
        captures.emplace_back(currentNeighbour);

        // continue with next neighbor
        currentNeighbour = currentNeighbour->neighbours[direction];
    }

    return captures;
}

const std::vector<Node*> Board::getBestCaptures(const Move& move)
{
    auto captures_forwards = getCapturesInDirection(move, false);
    auto captures_backwards = getCapturesInDirection(move, true);

    return captures_forwards.size() > captures_backwards.size() ? captures_forwards : captures_backwards;
}

std::string Move::toString() const
{
    return "From " + from()->toString() + " to " + to()->toString();
}
