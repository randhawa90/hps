//
//  main.cpp
//  illuminati_tipsy
//

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <assert.h>
using namespace std;

ifstream file;


enum MODE { PLACE = 1, REMOVE};
enum PLAYER { RED = 1, BLUE};
enum LIMIT { MAX = 1000000, MIN = -1000000};

struct state {
  unsigned int rod:31;
  unsigned long Rlow:64;
  unsigned long Rhigh:60;
  unsigned long Blow:64;
  unsigned long Bhigh:60;
  unsigned short Rremaining;
  unsigned short Bremaining;
  int Rfirst;
  int Rlast;
  int Blast;
};

static state board;

bool isEmpty(state test, int i) {
  return (test.rod & (1u << i)) == 0;
}

void getState(const char* fileName) {
  bzero(&board, sizeof(board));
  file.open(fileName);
  unsigned long  weight;
  int pos, player = 0;
  int rf = 1;
  board.Rremaining = 0xffff;
  board.Bremaining = 0xffff;
  while ( file >> pos >> weight >> player ) {
    pos += 15;
    if ( weight) {
      if (player == RED && rf == 1) {
        board.Rfirst = pos;
        rf = 0;
      }
      board.rod |= 1u << pos;
      if( player == RED) {
        if (pos < 16) {
          board.Rlow |= weight << (pos * 4) ;
        }else {
          board.Rhigh |= weight << ( ( pos - 16 ) * 4);
        }
        board.Rremaining &= ~(1u << weight);
        board.Rlast = pos;
      }else if (player == BLUE){
        if( pos < 16) {
          board.Blow |= weight << ( pos * 4);
        }else {
          board.Bhigh |= weight << ( ( pos - 16) * 4);
        }
        board.Bremaining &= ~(1u << weight);
        board.Blast = pos;
      }
    }
  }
  file.close();
}



int getWeightSingle(unsigned long bits, int idx) {
  int off = idx * 4;
  return (bits & (15ul << off))>> off;
}


int getWeight(state test, int i) {
  if ( i == 11) {
    return 3;
  }
  int weight = 0;
  if(i < 16) {
    weight = getWeightSingle(test.Rlow, i) | getWeightSingle(test.Blow, i);
  }else {
    weight = getWeightSingle(test.Rhigh, i - 16) | getWeightSingle(test.Bhigh, i - 16);
  }
  return weight;
}

int isBlue(state test, int i) {
  if (i < 16 && getWeightSingle(test.Blow, i)) return 1;
  if( i >= 16 && getWeightSingle(test.Bhigh, i-16) ) return 1;
  return 0;
}

int isRed(state test, int i) {
  if (i < 16 && getWeightSingle(test.Rlow, i)) return 1;
  if( i >= 16 && getWeightSingle(test.Rhigh, i-16) ) return 1;
  return 0;
}


int getLeftNumber(state test, int player) {
  int number = 0;
  for(int i = 0; i < 12; i ++ ) {
    if( isEmpty(test, i) == 0 ) {
      if( player == RED && isRed(test, i) ){
        number ++;
      }
      else if( player == BLUE && isBlue(test, i))
        number ++;
    }
  }
  return number;
}


int getMiddleNumber(state  test, int player) {
  int number = 0;
  for(int i = 12; i < 15; i ++ ) {
    if( isEmpty(test, i) == 0 ) {
      if( player == RED && isRed(test, i) ){
        number ++;
      }
      else if( player == BLUE && isBlue(test, i))
        number ++;
    }
  }
  return number;
}


int getRightNumber(state test, int player) {
  int number = 0;
  for(int i = 15; i < 31; i ++ ) {
    if( isEmpty(test, i) == 0 ) {
      if( player == RED && isRed(test, i) ){
        number ++;
      }
      else if( player == BLUE && isBlue(test, i))
        number ++;
    }
  }
  return number;
}

std::pair<int, int> diff_state(state s1, state s2) {
  /* s2 has to hold more weight than s1 */
  pair<int, int> move;
  for(int i = 0; i < 31; i ++ ) {
    if( isEmpty(s1, i) != isEmpty(s2, i)) {
      move.first = i;
      break;
    }
  }

  move.second = getWeight(s2, move.first);
  return move;
}

std::vector<int> getPosAvail(state test) {
  std::vector<int> pos;
  for(int i = 0; i < 31; i ++ ) {
    if( isEmpty(test, i)) 
      pos.push_back(i);
  }
  return pos;
}
std::vector<int> getBlockAvail(state test, int player) {
  std::vector<int> blocks;
  unsigned short r = test.Rremaining;
  if(player == BLUE)
    r = test.Bremaining;

  for(int i = 1;i <= 12; i++ ) {
    if ( r & (1u << i) )
      blocks.push_back(i);
  }

  return blocks;
}



void place(state * test, int player, int i, int weight) {
    test->rod |= (1u << i);
    if( player == RED) {
      if(i < 16)  test->Rlow |= ((unsigned long)weight) << (i * 4);
      else test->Rhigh |= ((unsigned long )weight) << ((i -16) * 4);
      test->Rremaining &= ~(1u << weight);
    }else {
      if(i < 16) test->Blow |= ((unsigned long )weight) << (i * 4);
      else test->Bhigh |= ((unsigned long)weight) << ((i -16) * 4);
      test->Bremaining &= ~(1u << weight);
    }
}

int remove(state* test, int player, int i) {
  int weight = getWeight(*test, i);
  test->rod &= ~(1u << i);
  if( player == RED ) {
    if(i < 16)  test->Rlow &= ~(15ul << (i * 4));
    else test->Rhigh &= ~(15ul << ((i -16) * 4));
    test->Rremaining |= 1u << weight;
  }else {
    if(i < 16)  test->Blow &= ~(15ul << (i * 4));
    else test->Bhigh &= ~(15ul << ((i -16) * 4));
    test->Bremaining |= 1u << weight;
  }

  return weight;
}



pair<int, int> getTorque(state test) {
  pair<int, int> torque;
  torque.first = 0;
  torque.second = 0;
  for (int i = 0; i < 31; i++) {
    if (test.rod & 1u<<i) {
      int weight = getWeight(test, i);
      torque.first -= (i - 12) * weight;
      torque.second -= (i - 14) * weight;
    }
  }
  // gravity
  torque.first -=  3 * 3;
  torque.second -= 1 * 3;
  return torque;
}



std::vector<pair<int, int> > getMoveAvail(state test, int player) {
  std::vector<pair<int, int> > moves;
  pair<int, int> move;
  std::vector<int> positions = getPosAvail(test);
  std::vector<int> blocks = getBlockAvail(test, player);
  for(int i = 0; i < positions.size() ; i ++ ) {
    for(int j = 0; j < blocks.size(); j ++ ){
      int pos = positions[i];
      int weight = blocks[j];
      state s = test;
      place(&s, player, pos, weight);
      pair<int, int> t = getTorque(s);
      if( t.first * t.second <= 0) {
        move.first = pos;
        move.second = weight;
        moves.push_back(move);
      }
    }
  }
  return moves;
}
void printState(state test) {
  for(int i = 0; i < 31; i ++) {
    printf("%4d", i-15);
  }
  printf("\n");
  int blocks[31] = {0};
  int weight;
  for(int i = 0; i < 31;i ++ ) {
    if(test.rod & (1u << i) ) {
      weight = getWeight(test, i);
      printf("%4d", weight);
      if( isRed(test, i) )
        blocks[i] = 1;
      else if( isBlue(test, i))
        blocks[i] = 2;
    }else {
      printf("    ");
    }
  }
  printf("\n");
  for(int i = 0; i < 31; i ++ ) {
    if( blocks[i] == 1) {
      printf("   R");
    }else if( blocks[i] == 2) {
      printf("   B");
    }else {
      printf("    ");
    }
  }
  printf("\n");
  printf("Red Blocks:\n");
  for(int i = 1; i <= 12;  ++ i ) {
    if( test.Rremaining & (1u << i))  {
      printf("%d ", i);
    }
  }
  printf("\nBlue Blocks:\n");
  for(int i = 1; i <= 12;  ++ i ) {
    if( test.Bremaining & (1u << i))  {
      printf("%d ", i);
    }
  }
  printf("\n");
  pair<int, int> t = getTorque(test);
  std::cout << t.first << " " << t.second << std::endl;
}





class StateTree {
  public:
    StateTree(state s, int player, int step = 2)  {
      _sbegin = new StateNode(s, player, 0);
      _player = player;
      _step = step;
    }
    ~StateTree() {
      std::cout << StateNode::count << std::endl;
      delete _sbegin;
    }
    void constructTree() {
      constructTreeRec(_sbegin);
    }
    pair<int,int> pick() {
      float value = pruning(_sbegin, MIN, MAX);
      printState(_final);
      return diff_state( _sbegin->_s, _final);
   }
  private:
    class StateNode {
      public:
        StateNode(state s, int player, int level) {
          _s  = s;
          _player = player;
          _level = level;
          _value = 0;
          count ++;
        }

        ~StateNode() {
          for(int i = 0; i < _choices.size(); i ++ ) {
            StateNode* s = _choices[i];
            delete s;
          }
        }
        state _s;
        int _player;
        int _level;
        float _value;
        static int count;
        std::vector<StateNode * > _choices;
    };
    StateNode * _sbegin;
    int _step;
    int _player;
    state _final;

    float pruning(StateNode* p, float alpha, float beta) {
      if( p->_level == 2 * _step - 1) {
        return p->_value;
      }
      float value;
      for(int i =0; i < p->_choices.size();i ++) {
        if(p->_player == _player) {
          value = pruning(p->_choices[i], alpha, beta);
          if( value > alpha && p->_level == 0){
            _final = p->_choices[i]->_s;
            alpha = value;
          }
          if (alpha >= beta) {
            return alpha;
          }
        }
        else {
          value = pruning(p->_choices[i], alpha, beta);
          if( value < beta && p->_level == 0) {
            _final = p->_choices[i]->_s;
            beta = value;
          }
          if( alpha >= beta) {
            return beta;
          }
        }
      }

      return p->_player == _player ? alpha : beta;
    }
    void constructTreeRec(StateNode *p) {
      int level = p->_level;
      if(level == 2 * _step - 1){
        assert( p->_player != _player);
        int op = RED+BLUE - p->_player;
        int l = getLeftNumber(p->_s, op);
        int r = getRightNumber(p->_s, op);
        float value = 1.0 / (abs(l - r) + 0.01);
        pair<int, int> t = getTorque(p->_s);
        value += 1.0 / (abs(t.first + t.second) + 0.1);

        std::vector<int> blocks = getBlockAvail(p->_s, op);
        if( blocks.size() > 6) {
          int sum = 0;
          for(int i = 0; i < blocks.size() ;i ++ ) {
            sum += blocks[i];
          }
          value += 10.0 / sum;
        }
        p->_value = value;
        return;
      }
      int player = p->_player;
      std::vector<pair<int, int> > moves = getMoveAvail(p->_s, player);
      for(int i  = 0; i < moves.size(); i ++ ){
        int pos = moves[i].first;
        int weight = moves[i].second;
        state next = p->_s;
        place(&next, player, pos, weight);
        StateNode * nextNode = new StateNode(next, RED + BLUE - player, level + 1);
        p->_choices.push_back(nextNode);
        constructTreeRec(nextNode);
      }
    }
};

int StateTree::StateNode::count = 0;

pair<int, int> smarter_tip(state test, int mode , int player) {
  if ( mode == PLACE) { // for placeing blocks
    if (player == RED) {
      // try best to distribute the blocks
      int remain = 1; //getBlockAvail(test, RED).size();
      StateTree tree(test, RED, remain>=2? 2:1);
      tree.constructTree();
      pair<int, int> move = tree.pick();
      return move;
    }else {
      // try best to force red places his blocks onto one side
      int l = getLeftNumber(test, RED);
      int r = getRightNumber(test, RED);
      int torque = 1;
      if( r > l) {
        torque = 0;
      }
      std::vector<pair<int, int> > moves = getMoveAvail(test, player);
      int m = MAX;
      int p = -1;
      int w;
      for(int i = 0; i < moves.size(); i ++) {
        int pos = moves[i].first;
        int weight = moves[i].second;
        if( pos == -1){  p = pos; w = weight; }

        state next = test;
        place(&next, BLUE, pos, weight);

        pair<int , int> t = getTorque(next);

        if( torque == 1) {
          if( t.second < m ) {
            m = t.second;
            p = pos;
            w = weight;
          }
        }
        else {
          if( abs(t.first) < m) {
            m = abs(t.first);
            p = pos;
            w = weight;
          }
        }
      }
      pair<int, int> move;
      move.first = p;
      move.second = w;
      return move;
    }
  }

  else { // for remove blocks

  }
}
void greedy_tip(state test, int mode, int player) {
  if( mode == 1) {  // for plaing blocks
    // greedy algorithm, pick block from 1 up to 12
    int i;
    for(i = 0; i < 12; i ++) {
      if(player == 0 ) {
        if(test.Rremaining & (1 << i)) {
          break;
        }
      }else {
        if(test.Bremaining & (1 << i)) {
          break;
        }
      }
    }

    // greedy algorithm, pick a spot to make some torque as close to 0 as possible
    int weight = i;
    int pos = -1;
    int m = 1000000;
    for(i = 0;i < 31; i ++ ) {
      if(!test.rod & (1u <<i)) {
        //put the block on this spot
        place(&test, player, i, weight);
        pair <int, int> torque;
        torque = getTorque(test);
        if( pos == -1 ) {
          pos = i;
        }
        if( torque.first * torque.second >= 0) {
          if(min ( abs(torque.first), abs(torque.second) ) < m ) {
            m = min ( abs(torque.first), abs(torque.second) );
            pos = i;
          }
        }
        // move back
        remove(&test, player, i);
      }
    }
    if( m != 1000000) {
      std::cout << pos <<  " "  << weight << std::endl;
    }else { // we can make any move
      std::cout << pos <<  " " << weight << std::endl;
    }
  }else { // for removing blocks
    if( player == 0) { // red player, can take every block
      //greedy algorithm, remove a block to make either torque as close to 0 as possible
      int pos = -1;
      int m = 1000000;

      int i;
      for(i = 0; i < 31; i ++) {
        if( test.rod & (1u << i)) {
          if( pos == -1) pos = i;
          int weight = remove(&test, player, i);
          pair <int, int> torque;
          torque = getTorque(test);
          if( torque.first * torque.second >= 0){
            if(min ( abs(torque.first), abs(torque.second) ) < m ) {
              m = min ( abs(torque.first), abs(torque.second) );
              pos = i;
            }
          }
          place(&test, player, i, weight);
        }
      }
      if( m == 1000000) { // we lose
        std::cout << pos << " " << getWeight(test, pos) << std::endl;
      }
      else { // we remove a block from positon pos
        std::cout << pos << " " << getWeight(test, pos) << std::endl;
      }
    }else { // blue player, could only take blue blocks
      //greedy algorithm, remove a block to make torques' abs values as close to each other as possible
      int pos = -1;
      int m = 1000000;

      int i;
      for(i = 0; i < 31;i ++ ) {
        if ( test.rod & (1u << i) && isBlue(test, i) ) {
          if(pos == -1) pos = i;
          int weight = remove(&test, player, i);
          pair<int, int> torque = getTorque(test);
          if(torque.first * torque.second >= 0) {
            if( abs( torque.first + torque.second) < m) {
              m = abs(torque.first + torque.second);
              pos = i;
            }
          }
          place(&test, player, i, weight);
        }
      }

      if(m == 1000000) {
        std::cout << pos << " " << getWeight(test, pos) << std::endl;
      }else {
        std::cout << pos << " " << getWeight(test, pos) << std::endl;
      }
    }
  }
}


int main(int argc, const char * argv[])
{
  int player, mode;
  float remaining_time;
  mode = atoi(argv[1]);
  player = atoi(argv[2]);
  remaining_time = atof(argv[3]);

  std::string filename = "board.txt";
  getState(filename.c_str());

  /*
  //printState(board);
  pair<int, int> t = getTorque(board);
  std::cout << t.first << " " << t.second << endl;
  state next = board;
  std::cout << "Red place 1 on -7 " << std::endl;
  place(&next, RED, -7 + 15, 1);
  //printState(next);
  t = getTorque(next);
  std::cout << t.first << " " << t.second << endl;
  pair<int , int> move = diff_state(board, next);
  std::cout << move.first  << " " << move.second<< std::endl;
  std::cout << "Red remove 5" << std::endl;
  int weight = remove(&board, RED, 5 + 15);
  std::cout << "Remove " << weight << " from 5" << std::endl; 
  printState(board);
  */
  printState(board);
  pair<int, int> move = smarter_tip(board, mode, player);
  place(&board, player, move.first, move.second);
  printState(board);
  /*
  std::vector< pair<int, int> > moves = getMoveAvail(board, RED);
  for(int i = 0; i < moves.size(); i++ ){
    int pos = moves[i].first;
    int weight = moves[i].second;
    state s = board;
    std::cout << "Place " << weight << " at " << pos - 15 << std::endl;
    place(&s, RED, pos, weight);
    printState(s);
    pair<int, int> t = getTorque(s);
    std::cout << t.first << " " << t.second << std::endl;
  }
  */
  return 0;
}

