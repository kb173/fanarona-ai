#include <string>
#include <array>

const int BOARD_WIDTH = 9;
const int BOARD_HEIGHT = 5;

enum class State : char {
  EMPTY,
  WHITE,
  BLACK,
};

struct Node {
  State state = State::EMPTY;
  std::array<Node*, 8> neighbours;
};

class Board {
public:
  Board();
  ~Board();

  void parse(std::string boardContent);

  Node cells[BOARD_HEIGHT][BOARD_WIDTH];
};
