//
//  main.cpp
//  illuminati_tipsy
//

#include <iostream>
#include <fstream>

using namespace std;

ifstream file;

struct state {
  unsigned int rod:31;
  unsigned long Rlow:64;
  unsigned long Rhigh:60;
  unsigned long Blow:64;
  unsigned long Bhigh:60;
  short Rremaining:12;
  short Bremaining:12;
};
state board;


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

void getState(char* fileName) {
  file.open(fileName);
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
  closeFile();
}

pair<unsigned int, unsigned int> getTorque(state test) {
  pair<unsigned int, unsigned int> torque;
  torque.first = 0;
  torque.second = 0;
  torque.first -= 9;
  torque.second -= 3;
  for (int i = 0; i < 31; i++) {
    if (test.rod & 1u<<i) {
      int weight = 0;
      if (i < 16) {
        weight = (test.Rlow & (15 << (i*4))) | (test.Blow & (15 << (i*4)));
        torque.first += ((i - 12)*weight);
        torque.second += ((i - 14)*weight);
      }
      else {
        weight = (test.Rhigh & (15 << ((i - 16)*4))) | (test.Bhigh & (15 << ((i - 16)*4)));
        torque.first -= ((i - 15 + 3)*weight);
        torque.second -= ((i - 15 + 1)*weight);
      }
    }
  }
  return torque;
}

int main(int argc, const char * argv[])
{
  
  // insert code here...
  std::cout << "H";
  return 0;
}

