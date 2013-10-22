#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <cfloat>
#include <iostream>

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
  Stone(int c, int x_, int y_): l ( x_, y_) {
    color = c;
  }
  Stone(const Stone& s) {
    l = s.l;
    color = s.color;
  }
  int color;
  Location l;
};

class Grid {
  public:
    static const int WIDTH = 1000;
    static const int HEIGHT = 1000;

    Grid() {
      stride = 40;
    }
    Grid(std::vector<Stone> & stones) {
      stride = 40;
      setColor(stones);
    }


    Location center() {
      return Location(WIDTH/2, HEIGHT/2);
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
        for(int i = 0; i < WIDTH; i += stride ){
          for(int  j = 0; j < HEIGHT; j += stride ){
            std::vector<Location> locs;
            locs.push_back(Location(i, j));
            locs.push_back(Location(i+stride-1, j));
            locs.push_back(Location(i, j+stride -1));
            locs.push_back(Location(i+ stride - 1, j+stride - 1));
            locs.push_back(Location(i + stride / 2, j + stride / 2));
            std::vector<int> colors;
            for(int n = 0; n < locs.size(); n ++) {
              Location l = locs[n];
              int c = _setcolor(l, stoneset);
              board[l.getX()][l.getY()] = c;
              colors.push_back(c);
            }

            int flag = 1;
            for(int n = 1; n < colors.size(); n ++) {
              if(colors[n] != colors[n - 1] ) {
                flag = 0;
                break;
              }
            }
            if( flag == 1) {
              int color = colors[0];
              for(int m = i; m < i + stride; m ++ ){
                for(int n = j; n < j + stride; n ++) {
                  board[m][n] = color;
                }
              }
            }else {
              for(int m = i; m < i + stride; m ++) {
                for(int n = j; n < j + stride; n ++) {
                  Location l(m, n);
                  int c = _setcolor(l, stoneset);
                  board[l.getX()][l.getY()] = c;
                }
              }
            }
          }
        }
      }
    }
    int getColor(int i, int j ) {
      return board[i][j];
    }


    std::map<int, int> getColorDist() {
      std::map< int, int> ret;
      for(int i = 0; i < WIDTH; i ++ ) {
        for(int j = 0; j < HEIGHT; j ++ ) {
          int c = board[i][j];
          if( ret.find(c) == ret.end() ) {
            ret[c] = 0;
          }
          ret[c] ++;
        }
      }
      return ret;
    }
  private:
    int _setcolor(Location l, std::map<int, std::vector<Stone> > & stoneset) {
      float pull = 0.0;
      int c;
      std::map<int , std::vector<Stone> >::iterator iter = stoneset.begin();
      for(; iter != stoneset.end(); iter ++){
        int sum = 0;
        for(int m = 0; m < iter->second.size(); m ++) {
          sum += 1.0 / l.getDS((iter->second)[m].l);
        }
        if( pull < sum){
          pull  = sum;
          c = iter->first;
        }
      }
      return c;
    }
    int board[WIDTH][HEIGHT];
    int stride;
};

Grid grid;


class GreedyVoronoiMove{
  private:
    static int stride;
  public:
    static Location move(Grid grid, std::vector<Stone> stone, int color) {
      int num = 0;
      int idx_i = 0;
      int idx_j = 0;

      if (stone.size() == 0 ) {
        return grid.center();
      }
      for(int i = 0; i < Grid::WIDTH; i += stride ) {
        for(int j = 0; j < Grid::HEIGHT; j += stride) {
          std::vector<Stone> pass = stone;
          Stone s(color, i + stride/2, j + stride/2);
          pass.push_back(s);
          grid.setColor(pass);
          std::map<int, int> ret = grid.getColorDist();
          if (ret[color] > num) {
            num = ret[color];
            idx_i = i;
            idx_j = j;
          }
        }
      }

      int fidx_i = idx_i;
      int fidx_j = idx_j;
      for(int i = idx_i; i < idx_i + stride;  i ++) {
        for(int j = idx_j; j < idx_j + stride; j ++ ) {
          Stone s(color, i, j);
          std::vector<Stone> pass = stone;
          pass.push_back(s);
          grid.setColor(pass);
          std::map<int, int> ret = grid.getColorDist();
          if (ret[color] > num) {
            num = ret[color];
            fidx_i = i;
            fidx_j = j;
          }
        }
      }
      Location l(fidx_i, fidx_j);
      return l;
    }
};

int GreedyVoronoiMove::stride = 40;


int fpeek(FILE* fin) {
  int c;
  c = fgetc(fin);
  ungetc(c, fin);
  return c;
}

int main(int argc, char ** argv) {
  if (argc != 2 && argc != 3) {
    fprintf(stderr, "Fuck!\n");
    return 0;
  }
  int color = atoi(argv[1]); 
  FILE * fin = stdin;
  if( argc == 3) {
    fin = NULL;
    char* filename = argv[2];

    if( (fin = fopen(filename, "r")) == NULL) {
      fprintf(stderr, "Fuck!\n");
      return 0;
    }
  }
  std::vector<Stone> stones;
  int c, x, y;
  while( 1 ) {
    int n = fscanf(fin, "(%d,%d,%d)", &c, &x, &y);
    if ( n != 3) {
      break;
    }
    printf("%d, %d, %d\n", c, x, y);
    stones.push_back(Stone(c, x, y));
    n = fgetc(fin);
    if( n !=  ',') {
      break;
    }
  }

  std::map<int, int> areas;
  while(1) {
    int n = fscanf(fin, "(%d,%d)", &c, &x);
    if( n != 2 ) {
      break;
    }
    printf("%d, %d\n", c, x);
    areas[c] = x;
    n = fpeek(fin);
    if( n != ',') {
      break;
    }
  }
  grid.setColor(stones);
  std::map<int, int> dist = grid.getColorDist(); 
  Location l = GreedyVoronoiMove::move(grid, stones, color); 
  std::cout << "(" << color << "," << l.getX() << "," << l.getY() << ")" << std::endl;
  return 0;
  
}
