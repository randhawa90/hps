class Location {
  private:
    int x, y;
  public:
    Location(int x, int y) {
      this->x = x;
      this->y = y;
    }
    Location(const Location & l) {
      x = l.x; y = l.y;
    }
    Location() {
      reset();
    }
    inline int getX() const { return x; }
    inline int getY() const { return y; }
    inline void setX(int x) { this->x = x;}
    inline void setY(int y) { this->y = y;}
    inline int getD(const Location & l) const { return abs(x - l.x) + abs(y - l.y) ; }
    inline Location move(int x, int y) {
      Location l(this->x + x, this->y + y);
      return l;
    }
    inline Location move(std::pair<int,int> step) {
      return move(step.first, step.second);
    }
    inline void reset() { x = 0; y = 0; }
    friend std::ostream& operator<< ( std::ostream& out, const Location& l);
};

std::ostream& operator<<(std::ostream& out, const Location & l) {
  out << "(" << l.x << "," << l.y << ")";
  return out;
}



struct Stone{
  Stone(int c, int x_, int y_): l ( x, y) {
    color = c;
  }
  int color;
  Location l;
}

class Grid {
  public:
    static const int WIDTH = 1000;
    static const int HEIGHT = 1000;
    
    Grid() {
    }
    Grid(std::vector<Stone> & stones) {
      setColor(stones);
    }

    void setColor(std::vector<Stone> & stones) {
    }
  private:
    int board[WIDTH][HEIGHT];
};

Grid grid;
