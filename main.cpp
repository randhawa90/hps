//
//  main.cpp
//  illuminati_tipsy
//

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

ifstream file;
/*
char peekFile() {
  return file.peek();
}

void closeFile() {
  file.close();
}
int parseInt(){
  int pos = 0;
  char* num = new char[4];
  while (isdigit(file.peek()) || file.peek() == '-') {
    num[pos++] = file.get();
  }
  num[pos] = '\0';
  return atoi(num);
}

void stepOverDelimiters(){
  while (isblank(file.peek()) || file.peek() == '\n') {
    file.get();
  }
}
*/


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

int isEmpty(state test, int i) {
  return test.rod & (1u << i);
}

void getState(const char* fileName) {
  bzero(&board, sizeof(board));
  file.open(fileName);
  /*
  for (int i = 0; i < 31; i++) {
    parseInt();
    stepOverDelimiters();
    unsigned int weight = parseInt();
    stepOverDelimiters();
    unsigned int player = parseInt();
    stepOverDelimiters();
    if (weight) {
      board.rod |= 1u << i;
      if (player == 1) {
        if (i < 16) {
          board.Rlow |= weight << (i*4);
        }
        else {
          board.Rhigh |= weight << ((i - 16)*4);
        }
      }
      else {
        if (i < 16) {
          board.Blow |= weight << (i*4);
        }
        else {
          board.Bhigh |= weight << ((i - 16)*4);
        }
      }
    }
  }
  */
  unsigned long  weight;
  int pos, player = 0;
  int rf = 1;
  board.Rremaining = 0xffff;
  board.Bremaining = 0xffff;
  while ( file >> pos >> weight >> player ) {
    pos += 15;
    if ( weight) {
      if (player == 1 && rf == 1) {
        board.Rfirst = pos;
        rf = 0;
      }
      board.rod |= 1u << pos;
      if( player == 1) {
        if (pos < 16) {
          board.Rlow |= weight << (pos * 4) ;
        }else {
          board.Rhigh |= weight << ( ( pos - 16 ) * 4);
        }
        board.Rremaining &= ~(1u << weight);
        board.Rlast = pos;
      }else if (player == 2){
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
}

pair<int, int> getTorque(state test) {
  pair<int, int> torque;
  torque.first = 0;
  torque.second = 0;
  for (int i = 0; i < 31; i++) {
    if (test.rod & 1u<<i) {
      int weight = 0;
      /*
      if (i < 16) {
        weight = (test.Rlow & (15 << (i*4))) | (test.Blow & (15 << (i*4)));
        torque.first += ((i - 12)*weight);
        torque.second += ((i - 14)*weight);
      }else if{
        weight = (test.Rhigh & (15 << ((i - 16)*4))) | (test.Bhigh & (15 << ((i - 16)*4)));
        torque.first -= ((i - 15 + 3)*weight);
        torque.second -= ((i - 15 + 1)*weight);
      }
      */

      weight = getWeight(test, i);
      torque.first -= (i - 12) * weight;
      torque.second -= (i - 14) * weight;
    }
  }
  // gravity
  torque.first -=  3 * 3;
  torque.second -= 1 * 3;
  return torque;
}




void place(state * test, int player, int i, int weight) {
    test->rod |= 1u << i;
    if( player == 0 ) {
      if(i < 16)  test->Rlow |= weight << (i * 4);
      else test->Rhigh |= weight << ((i -16) * 4);
    }else {
      if(i < 16) test->Blow |= weight << (i * 4);
      else test->Bhigh |= weight << ((i -16) * 4);
    }
}

int remove(state* test, int player, int i) {
  int weight = getWeight(*test, i);
  test->rod &= ~(1u << i);
  if( player == 0 ) {
    if(i < 16)  test->Rlow &= ~(15ul << (i * 4));
    else test->Rhigh &= ~(15ul << ((i -16) * 4));
  }else {
    if(i < 16)  test->Blow &= ~(15ul << (i * 4));
    else test->Bhigh &= ~(15ul << ((i -16) * 4));
  }

  return weight;
}

/*
std::vector<int> getPosAvail(state test) {
  std::vector<int> pos;
  for(int i = 0; i < 
}
*/
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
  printState(board);
  pair<int, int> t = getTorque(board);
  std::cout << t.first << " " << t.second << endl;
  return 0;
}

