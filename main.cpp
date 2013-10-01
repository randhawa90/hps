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

int getRedNumber(state test) {
  int number = 0;
  for(int i = 0; i < 31; i ++) {
    if ( ! isEmpty(test, i) && isRed(test, i)) {
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

std::vector<int> getEmptyPos(state test) {
  std::vector<int> pos;
  for(int i = 0; i < 31; i ++ ) {
    if( isEmpty(test, i))
      pos.push_back(i);
  }
  return pos;
}


std::vector<int> getPlacedPos(state test) {
  std::vector<int> positions;
  for(int i = 0; i < 31; i ++ ) {
    if( !isEmpty(test, i) ) {
      positions.push_back(i);
    }
  }
  return positions;
}

std::vector<int> getRedPlacedPos(state test) {
  std::vector<int> pos;
  for(int i = 0; i < 31; i ++ ) {
    if( ! isEmpty(test, i) && !isBlue(test, i) ) {
      pos.push_back(i);
    }
  }
  return pos;
}

std::vector<int> getRemainingBlock(state test, int player) {
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
  std::vector<int> positions = getEmptyPos(test);
  std::vector<int> blocks = getRemainingBlock(test, player);
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


std::vector<pair < int, int> > getRemoveAvail(state test, int player) {
  std::vector< pair<int, int> > moves;
  pair<int, int> move;
  std::vector< int > pos = getPlacedPos(test);
  if( player == RED) {
    int red = getRedNumber(test);
    if( red != 0) {
      std::vector<int> tmp;
      for(int i = 0; i < pos.size(); i ++ ) {
        int p = pos[i];
        if( !isBlue(test, p)) {
          tmp.push_back(p);
        }
      }
      pos = tmp;
    }
  }


  for(int i = 0; i < pos.size(); i ++ ) {
    int p = pos[i];
    int w = getWeight(test, p);
    state s = test;
    remove(&s, player, p);
    pair<int, int> t = getTorque(s);

    if( t.first * t.second <= 0) {
      move.first = p;
      move.second = w;
      moves.push_back(move);
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



pair<int, int> getRandomMove(state test, int player, int mode) {
  pair<int, int> move;
  int w;
  int p;
  if( mode == PLACE) {
    w = getRemainingBlock(test, player)[0];
    p = getEmptyPos(test)[0];
  }else {
    int r = getRedNumber(test);
    if (player == RED && r != 0) {
      p = getRedPlacedPos(test)[0];
      w = getWeight(test, p);
    }else {
      p = getPlacedPos(test)[0];
      w = getWeight(test, p);
    }
  }
  move.first = p;
  move.second = w;
  return move;
}


class StateTree {
  public:
    StateTree(state s, int player, int mode  = PLACE, int step = 2)  {
      _sbegin = new StateNode(s, player, 0);
      _player = player;
      _step = step;
      _mode = mode;
      _final = s;
    }
    ~StateTree() {
      delete _sbegin;
    }
    void constructTree() {
      constructTreeRec(_sbegin);
    }
    pair<int,int> pick() {
      float value = pruning(_sbegin, MIN, MAX);
      if( _final.rod == _sbegin->_s.rod) {
        return getRandomMove(_sbegin->_s, _player, _mode);
      }
      if( _mode == PLACE)
        return diff_state( _sbegin->_s, _final);
      else
        return diff_state(_final, _sbegin->_s);
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
    int _mode;

    float pruning(StateNode* p, float alpha, float beta) {
      if( p->_choices.size() == 0) {
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
      if(level == _step){
        int r = getRedNumber(p->_s);
        if(_mode == PLACE || (_player == RED && r > 0)) {
          assert( p->_player != _player);
          int op = RED+BLUE - p->_player;
          int l = getLeftNumber(p->_s, op);
          int r = getRightNumber(p->_s, op);
          float value = 10.0 / (abs(l - r) + 0.01);
          pair<int, int> t = getTorque(p->_s);
          value += 1.0 / (abs(t.first + t.second) + 0.1);

          std::vector<int> blocks = getRemainingBlock(p->_s, op);
          if(_mode == PLACE &&  blocks.size() > 6) {
            int sum = 0;
            for(int i = 0; i < blocks.size() ;i ++ ) {
              sum += blocks[i];
            }
            value += 1.0 / sum;
          }
          p->_value = value;
        }else {
          // remove things, try to maxinum the steps I could take
          p->_value = getRemoveAvail(p->_s, p->_player).size();
        }
        return;
      }
      int player = p->_player;
      std::vector<pair<int, int> > moves;
      if( _mode == PLACE)
        moves = getMoveAvail(p->_s, player);
      else
        moves = getRemoveAvail(p->_s, player);
      for(int i  = 0; i < moves.size(); i ++ ){
        int pos = moves[i].first;
        int weight = moves[i].second;
        state next = p->_s;
        if( _mode == PLACE)
          place(&next, player, pos, weight);
        else
          remove(&next, player, pos);
        StateNode * nextNode = new StateNode(next, RED + BLUE - player, level + 1);
        p->_choices.push_back(nextNode);
        constructTreeRec(nextNode);
      }
      if( moves.size() == 0) {
          if( p->_player != _player) {
            p->_value = MAX;
          }else {
            p->_value = MIN;
          }
      }
    }
};

int StateTree::StateNode::count = 0;


pair<int, int> brute_force_pick(state test, int player, int mode, int flag, std::vector<pair<int, int> > & moves){
  int m = MAX;
  int p = -1;
  int w;
  for(int i = 0; i < moves.size(); i ++) {
    int pos = moves[i].first;
    int weight = moves[i].second;
    if( p == -1){  p = pos; w = weight; }

    state next = test;
    if( mode == PLACE)
      place(&next, player, pos, weight);
    else
      remove(&next, player, pos);

    pair<int , int> t = getTorque(next);

    if( flag == 1) {
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

pair<int, int> smarter_tip(state test, int mode , int player) {
  if ( mode == PLACE) { // for placeing blocks
    if (player == RED) {
      // try best to distribute the blocks
      StateTree tree(test, RED, mode, 1);
      tree.constructTree();
      pair<int, int> move = tree.pick();
      return move;
    }else {
      // try best to force red places his blocks onto one side
      int l = getLeftNumber(test, RED);
      int r = getRightNumber(test, RED);
      int torque = 1;
      if( r > l) {
        torque = 2;
      }
      std::vector<pair<int, int> > moves = getMoveAvail(test, player);
      return brute_force_pick(test, player, mode, torque, moves);
    }
  }

  else { // for remove blocks
    if( player == RED ) {
      // still try to balance the numbers of left and right
      int red = getRedNumber(test);
      if (red > 0){
        StateTree tree(test, player, mode, 1);
        tree.constructTree();
        return tree.pick();
      }else {
        int r = getPlacedPos(test).size();
        StateTree tree(test, player, mode, 2);
        tree.constructTree();
        return tree.pick();
      }
    }else {
      int red = getRedNumber(test);
      if( red == 0) {
        // There is no red blocks on board, fair play
        // create a tree to do a-b pruning
        int r = getPlacedPos(test).size();
        StateTree tree(test, player, mode, r);
        tree.constructTree();
        pair<int, int> move = tree.pick();
        return move;
      }else {
        int l = getLeftNumber(test, RED);
        int r = getRightNumber(test, RED);
        int flag = 0;
        if( r == 0) {
          flag = 1;
        }else if( l == 0){
          flag = 2;
        }

        std::vector<pair<int, int> > moves = getRemoveAvail(test, BLUE);
        if( flag > 0) {
          return brute_force_pick(test, player, mode, flag, moves);
        }else {
          flag = 1;
          if( l < r)
            flag = 2;
          std::vector<pair<int, int> > redMoves;
          pair<int, int> move;
          for(int i = 0; i < moves.size() ; i ++) {
            int pos = moves[i].first;
            if ((flag == 1 && pos < 15)  || (flag == 2 && pos > 11)) continue;
            if( isRed(test, pos) ) {
              move.first = pos;
              move.second = getWeight(test, pos);
              redMoves.push_back(move);
            }
          }

          if( redMoves.size() != 0) {
            return redMoves[0];
          }else {
            return brute_force_pick(test, player, mode, flag, moves);
          }
        }
      }
    }
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
  printState(board);
  state next = board;
  std::cout << "Red Remove -15" << std::endl;
  int weight = remove(&next, RED, 14);
  printState(next);
  pair<int , int> move = diff_state(board, next);
  std::cout << move.first  << " " << move.second<< std::endl;
  std::cout << "Red remove 5" << std::endl;
  int weight = remove(&board, RED, 5 + 15);
  std::cout << "Remove " << weight << " from 5" << std::endl; 
  printState(board);
  */
  printState(board);
  pair<int, int> move = smarter_tip(board, mode, player);
  if( mode == PLACE) {
    std::cout << "Place " << move.second << " at " << move.first - 15 << endl;
    place(&board, player, move.first, move.second);
  }
  else {
    std::cout << "Remoe " << move.second << " from " << move.first - 15 << std::endl;
    remove(&board, player, move.first);
  }
  printState(board);
  /*
  std::vector< pair<int, int> > moves = getRemoveAvail(board, RED);
  for(int i = 0; i < moves.size(); i++ ){
    int pos = moves[i].first;
    int weight = moves[i].second;
    state s = board;
    std::cout << "Remove " << weight << " at " << pos - 15 << std::endl;
    remove(&s, RED, pos);
    printState(s);
  }
  */
  return 0;
}

