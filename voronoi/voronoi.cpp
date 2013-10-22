#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cassert>
#include <vector>
#include <map>
#include <cfloat>
#include <iostream>
#include <sys/time.h>

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

    bool operator==(const Location & o) {
      return o.x == x && o.y == y;
    }
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


std::map<int, std::vector<Stone> > splitStone(std::vector<Stone> stones) {
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
  return stoneset;
}

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

    Location getTile(Location l) {
      int x = l.getX();
      int y = l.getY();

      return Location(x/stride*stride , y/stride*stride);
    }

    Location center() {
      return Location(WIDTH/2, HEIGHT/2);
    }
    void setColor(std::vector<Stone> & stones) {
      if ( stones.size() == 1) {
        int c = stones[0].color;
        _setPatchColorSingle(0, 0, WIDTH, HEIGHT, c);
      }else {
        std::map<int, std::vector<Stone> > stoneset = splitStone(stones);
        for(int i = 0; i < WIDTH; i += stride ){
          for(int  j = 0; j < HEIGHT; j += stride ){
            std::vector<Location> locs = getMark(i, j);
            std::vector<int> colors;
            for(int n = 0; n < locs.size(); n ++) {
              Location l = locs[n];
              int c = _setcolor(l, stoneset);
              colors.push_back(c);
            }

            if( isIdentical(colors)) {
              _setPatchColorSingle(i, j, stride, stride, colors[0]);
            }else {
              _setPatchColor(i, j, stride, stride, stoneset);
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
      double pull = 0.0;
      int c = 1;
      std::map<int , std::vector<Stone> >::iterator iter = stoneset.begin();
      for(; iter != stoneset.end(); iter ++){
        double sum = 0;
        for(int m = 0; m < iter->second.size(); m ++) {
          sum += 1.0l / l.getDS((iter->second)[m].l);
        }
        if( pull < sum){
          pull  = sum;
          c = iter->first;
        }
      }
      return c;
    }


    std::vector<Location> getMark(int start_i, int start_j) {
      std::vector<Location> locs;
      int i = start_i;
      int j = start_j;
      locs.push_back(Location(i, j));
      locs.push_back(Location(i+stride-1, j));
      locs.push_back(Location(i, j+stride -1));
      locs.push_back(Location(i+ stride - 1, j+stride - 1));
      locs.push_back(Location(i + stride / 2, j + stride / 2));
      return locs;
    }


    bool isIdentical(std::vector<int> &colors) const {
      for(int n = 1; n < colors.size(); n ++) {
        if(colors[n] != colors[n - 1] ) {
          return false;
        }
      }
      return true;
    }

    void _setPatchColorSingle(int start_i, int start_j, int width, int height, int c) {
      for(int i = start_i; i < start_i + width; i ++) {
        for(int j = start_j; j < start_j + height; j ++){
          board[i][j] = c;
        }
      }
    }

    void _setPatchColor(int start_i, int start_j, int width, int height,
        std::map<int, std::vector<Stone> > & stoneset ) {
      for(int i = start_i; i < start_i + width; i ++ ) {
        for(int j = start_j; j < start_j + height; j ++ ) {
            Location l(i, j);
            int c = _setcolor(l, stoneset);
            board[i][j] = c;
        }
      }

    }
    int board[WIDTH][HEIGHT];
    int stride;
};

Grid grid;


class GreedyVoronoiMove{
  private:
    static int stride;
    static std::vector<Location> getAllTile(int width, int height) {
      std::vector<Location> locs;
      for(int i = 0; i < Grid::WIDTH; i += stride ) {
        for(int j = 0; j < Grid::HEIGHT; j += stride) {
          locs.push_back(Location(i, j));
        }
      }
      return locs;
    }

    static std::vector<Location> getStoneTile(std::vector<Stone> & stones, Grid grid) {
      std::vector<Location> locs;
      for(int i = 0; i < stones.size(); i ++ ) {
        locs.push_back(grid.getTile(stones[i].l));
      }
      return locs;
    }

  public:
    static Location move(Grid grid, std::vector<Stone> stone, int color) {
      int num = 0;
      int idx_i = 0;
      int idx_j = 0;

      if (stone.size() == 0 ) {
        return grid.center();
      }
      std::map<int, std::vector<Stone> > stoneset = splitStone(stone);
      std::vector<Stone> onestones;

      if(color == 1) {
        onestones = stoneset[2];
      }else {
        onestones = stoneset[1];
      }

      //std::vector<Location> locs = getAllTile(Grid::WIDTH, Grid::HEIGHT);
      std::vector<Location> locs = getStoneTile(onestones, grid);
      for(int n = 0; n < locs.size() ; n ++ ) {
        int i = locs[n].getX();
        int j = locs[n].getY();
        std::vector<Stone> pass = stone;
        Stone s(color, i + stride/2, j + stride/2);
        //std::cout << s.l << std::endl;
        pass.push_back(s);
        grid.setColor(pass);
        std::map<int, int> ret = grid.getColorDist();
        if (ret[color] > num) {
          num = ret[color];
          idx_i = i;
          idx_j = j;
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


class ReversedMove {
  public:
    static Location move(Grid, std::vector<Stone> stones, int color ) {
      std::map<int, std::vector<Stone> > stoneset = splitStone(stones);
      Location l = findReversedLocation(stoneset, Grid::WIDTH, Grid::HEIGHT, color);
      return l;
    }

  private:
    static Location findReversedLocation(std::map<int, std::vector<Stone> >& stoneset , int width, int height, int color) {
      //assert(stoneset.size() == 2);
      assert(color == 2);
      std::vector<Stone> & firststones = stoneset[1];
      std::vector<Stone> & secondstones = stoneset[2];

      for(int i = 0; i < firststones.size(); i ++ ) {
        Location l = makeReversed(firststones[i].l, width, height);
        int j;
        for(j = 0; j < secondstones.size(); j ++ ) {
          if (secondstones[j].l == l)
            break;
        }
        if( j ==  secondstones.size()) {
          return l;
        }
      }
    }

    static Location makeReversed(Location l, int width, int height) {
      Location r(width - l.getX(), height - l.getY());
      if ( r == l) {
        return Location(0, 0);
      }else {
        return r;
      }
    }
};

class SymmetricMove {
  public:
    static Location move(Grid, std::vector<Stone> stones, int color ) {
      std::map<int, std::vector<Stone> > stoneset = splitStone(stones);
      Location l = findSymmetricLocation(stoneset, Grid::WIDTH, Grid::HEIGHT, color);
      return l;
    }

  private:
    static Location findSymmetricLocation(std::map<int, std::vector<Stone> >& stoneset , int width, int height, int color) {
      //assert(stoneset.size() == 2);
      assert(color == 2);
      std::vector<Stone> & firststones = stoneset[1];
      std::vector<Stone> & secondstones = stoneset[2];

      for(int i = 0; i < firststones.size(); i ++ ) {
        Location l = makeSymmetric(firststones[i].l, width, height);
        int j;
        for(j = 0; j < secondstones.size(); j ++ ) {
          if (secondstones[j].l == l)
            break;
        }
        if( j ==  secondstones.size()) {
          return l;
        }
      }
    }

    static Location makeSymmetric(Location l, int width, int height) {
      Location r(l.getY(), l.getX());
      if ( r == l) {
        return Location(0, 0);
      }else {
        return r;
      }
    }
};



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
    areas[c] = x;
    n = fgetc(fin);
    if( n != ',') {
      break;
    }
  }
  grid.setColor(stones);
  std::map<int, int> dist = grid.getColorDist(); 
  struct timeval tv1;
  struct timeval tv2;
  gettimeofday(&tv1, NULL);
  Location l = GreedyVoronoiMove::move(grid, stones, color); 
  //Location l = SymmetricMove::move(grid, stones, color);

  //gettimeofday(&tv2, NULL);
  //printf ("Total time = %f seconds\n",
  //         (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +
  //         (double) (tv2.tv_sec - tv1.tv_sec));
  std::cout << l.getX() << " " << l.getY() << std::endl;
  return 0;
}
