#include <cmath>
#include <vector>
#include <map>
#include <cfloat>

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
    inline float getD(const Location & l) const {
      return std::sqrt((x - l.x) * ( x- l.x) + (y - l.y) * (y -l.y));
    }
    inline float getDS(const Location &l)  const {
      return (x -l.x) * (x -l.x) + (y - l.y ) * ( y - l.y);
    }
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
  Stone(const Stone& s) {
    l = s.l;
    color = s.color;
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
      if ( stones.size() == 1) {
        int c = stones[0].color;
        for(int i = 0; i < WIDTH; i ++) {
          for(int j = 0; j < HEIGHT; j ++){
            board[i][j] = c;
          }
        }
      }else {
        std::map<int, std::vector<Stone> > stoneset;
        for(int i = 0; i < stones.size(); i ++){
          int c = stones[i].color;
          if( stoneset.find(c) == stoneset.end()) {
            std::vector<Stone> sset;
            sset.push_back(stones[i]);
            stoneset[c] = sset;
          }else {
            stoneset[c].push_back(stones[i]);
          }
        }
        for(int i = 0; i < WIDTH; i ++ ){
          for(int  j =0; j < HEIGHT; j ++ ){
            Location l(i, j);
            float pull = FLT_MAX;
            int c;
            std::map<int , std::vector<Stone> >::iterator iter = stoneset.begin();
            for(; iter != stoneset.end(); iter ++){
              int sum = 0;
              for(int m = 0; m < iter->second.size(); m ++) {
                sum += 1.0 / l.getDS((iter->second)[m].l)
              }
              if( pull > sum){
                pull  = sum;
                c = iter->first;
              }
            }
            board[i][j] = c;
          }
        }
      }
    }

    int getColor(int i, int j ) {
      return board[i][j];
    }
  private:
    int board[WIDTH][HEIGHT];
};

Grid grid;
