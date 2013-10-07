/* ambulance scheduling algorithm */

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <climits>
#include <cstdlib>
#include <cassert>
#include <utility>

class Direction {
  public:
    static int NUM;
    enum dir { up = 0, left, upleft, upright, downleft, downright, right, down};
    static std::pair<int, int> getStep(int d) {
      std::pair< int, int> step;
      int x, y;
      switch(d) {
        case up:
          x = 0; y = -1; break;
        case down:
          x = 0; y = 1; break;
        case left:
          x = -1; y = 0; break;
        case right:
          x = 1; y = 0; break;
        case upleft:
          x = -1; y = -1; break;
        case upright:
          x = 1, y = -1;  break;
        case downleft:
          x = -1; y = 1; break;
        case downright:
          x = 1; y = 1;  break;
        default:
          x =-0; y = 0;
      }
      step.first = x;
      step.second = y;
      return step;
  }
  static int getRandomDir() {
    int r = rand() %  Direction::NUM;
    return r;
  }

  static int getOppositeDir(int r) {
    return Direction::NUM - 1 - r;
  }
};
int Direction::NUM = 8;

class Location {
  private:
    int x, y;
  public:
    Location(int x, int y) {
      this->x = x;
      this->y = y;
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
};

class Patient {
  private:
    Location l;
    int time;
    int group;
  public:
    enum State { WAIT, INCAR, DEAD, SAVED};
    Patient( int x, int y, int t) : l(x, y) {
      time = t;
      group = 0;
      saved = WAIT;
    }
    Patient ( Location loc, int t) : l(loc.getX(), loc.getY() ) {
      time = t;
      group = 0;
      saved = WAIT;
    }

    State saved;
    inline int getX() const { return l.getX();}
    inline int getY() const { return l.getY();}
    inline int getT() const { return time; };
    inline Location getL() const { return l; }
    inline void setT(int t) { time  = t; }
    inline int getG() const { return group; }
    inline void setG(int g) { group = g; }
    inline void setS(State s) { saved = s ;}
    inline State getS() const { return saved; }
    inline bool isSaved() const { return saved == SAVED; }
};


class Ambulance {
  private:
    static int ID;
    std::vector<int> patient_index;
    int num;
    int hospital;
    int id;
    Location l;
    int current_time;

  public:
    const static int MAX_PATIENT = 4;
    Ambulance(int hos) {
      hospital = hos;
      id = ID;
      ID ++;
      current_time = 0;
    }
    Ambulance(const Ambulance& a ) {
      hospital = a.hospital;
      id = a.id;
      current_time = a.current_time;
    }

    inline void setL(Location loc) { l = loc; }
    inline Location getL() const { return l; }
    inline int load(std::vector<Patient> & patients, int i) {
      patient_index.push_back(i);
      patients[i].setS(Patient::INCAR);
      num ++; 
      return 1;
    }

    inline void unload(std::vector<Patient> & patients) {
      for(int i = 0; i < num; i ++ ) {
        int idx = patient_index[i];
        patients[idx].setS(Patient::SAVED);
      }
      patient_index.clear();
      num --;
    }

    inline int getPatientNum() { return num ; }
    inline int getT() const { return current_time; }
    inline void setT(int time) { current_time = time; }
};

int Ambulance::ID = 0;

class Hospital{
  public:
    Hospital(int num, int idx) {
      nam = num;
      id = idx;
      for(int i = 0; i < num; i++ ){
        ambulances.push_back(Ambulance(id));
      }
    }
    inline void setL(Location loc ) {
      l = loc;
      for(int i = 0; i < ambulances.size();i ++) {
        ambulances[i].setL(loc);
      }
    }

    int getAmbulanceNum() {return nam; }
    Ambulance & getAmbulance(int i) {
      assert( i < nam );
      return ambulances[i];
    }

    inline Location getL() const { return l ;}
  private:
    Location l;
    int nam;
    int id;
    std::vector<Ambulance> ambulances;
};


class KMeansLocateHospitalHelper {
  private:
    static void getGravity(std::vector<Patient> & patients , std::vector<Location>& locs) {
      std::vector<int> nums;
      for(int i = 0; i < locs.size(); i ++) {
        locs[i].reset();
        nums[i] = 0;
      }
      for(int i = 0; i < patients.size() ; i ++ ) {
        int g = patients[i].getG();
        locs[g].setX(locs[g].getX() + patients[i].getX());
        locs[g].setY(locs[g].getY() + patients[i].getY());
        nums[g]++;
      }

      for(int i = 0; i < locs.size();i ++){
        locs[i].setX(locs[i].getX() / nums[i]);
        locs[i].setY(locs[i].getY() / nums[i]);
      }
    }


    static void getMaxLocation(std::vector<Patient> & patients,
                               std::vector<Location> & locs,
                               int num,
                               std::vector<Location> & max,
                               std::vector<Location> & min) {
      std::map<int, int> locs_num;
      for(int i = 0; i < patients.size(); i ++ ) {
        if( locs_num.find(patients[i].getG()) == locs_num.end()) {
          locs_num[patients[i].getG()] = 0;
        }
        locs_num[patients[i].getG()] ++;
      }

      int size = locs_num.size() - num;
      for(int i = 0; i < size; i++ ) {
        int idx = 0;
        int num = INT_MAX ;
        for(int j = 0; j < locs_num.size(); j ++ ) {
          if( locs_num[j] < num ) {
            idx = j;
            num = locs_num[j];
          }
        }

        min.push_back(locs[locs_num[idx]]);
        locs_num.erase(idx);
      }

      for(int i = 0; i < num; i ++){
        max.push_back(locs[locs_num[i]]);
      }
    }


    static bool cluster(std::vector<Patient> &patients, std::vector<Location> & locs) {
      bool changed = false;
      for(int i = 0; i < patients.size(); i ++ ){
        Location l = patients[i].getL();
        int dis = INT_MAX;
        int g;
        for(int j = 0; j < locs.size(); j ++){
          int d = l.getD(locs[j]);
          if( d < dis ) {
            dis = d;
            g = j;
          }
        }
        if( patients[i].getG() != g)
          changed = true;
        patients[i].setG(g);
      }
      return changed;
    }
  public:
    static float locate(std::vector<Patient> & patients, std::vector<Hospital> & hospitals) {
      int num = hospitals.size();
      std::vector<Location> locs;
      getGravity(patients, locs);
      std::vector<Location> tmps = locs;
      while(locs.size() != num) {
        if( num >= 2 * locs.size()) {
          locs.clear();

          for(int i = 0; i < tmps.size(); i ++ ) {
            int dir1 = Direction::getRandomDir();
            int dir2 = Direction::getOppositeDir(dir1);
            Location l1 = tmps[i].move(Direction::getStep(dir1));
            Location l2 = tmps[i].move(Direction::getStep(dir2));

            locs.push_back(l1);
            locs.push_back(l2); }
        }else {
          int need = num - locs.size();
          std::vector<Location> maxlocs;
          std::vector<Location> minlocs;
          getMaxLocation(patients, locs, need,  maxlocs, minlocs);

          locs.clear();
          for(int i = 0; i < minlocs.size(); i ++){
            locs.push_back(minlocs[i]);
          }
          for(int i = 0; i < maxlocs.size(); i ++ ) {
            int dir1 = Direction::getRandomDir();
            int dir2 = Direction::getOppositeDir(dir1);
            Location l1 = tmps[i].move(Direction::getStep(dir1));
            Location l2 = tmps[i].move(Direction::getStep(dir2));

            locs.push_back(l1);
            locs.push_back(l2);
          }
        }

        cluster(patients, locs);
      }


      while(1) {
        getGravity(patients, locs);
        bool changed = cluster(patients, locs);
        if( changed == false) break;
      }

      for(int i = 0; i < num; i ++) {
        hospitals[i].setL(locs[i]);
      }
      return 0;
    }
};


class GreedyScheduler{
  private:
    static int greedyPickPatient(std::vector<Patient> &patients, Ambulance& am , int& time ) {
      assert( am.getPatientNum() < Ambulance::MAX_PATIENT);
      Location l = am.getL();
      int emin = INT_MAX;
      int idx = -1;
      for(int i = 0; i < patients.size(); i ++) {
        if(patients[i].getS() == Patient::WAIT) {
          int e = l.getD(patients[i].getL()) + patients[i].getT();
          if( e < emin) {
            emin = e;
            idx = i;
          }
        }
      }
      time = emin + 1;
      return idx;
    }


    static void decreaseTime(std::vector<Patient> & patients, int time) {
      for(int i = 0; i < patients.size(); i ++) {
        int t = patients[i].getT();
        if( t - time < 0) {
          patients[i].setS(Patient::DEAD);
        }
        else {
          patients[i].setT(t -time);
        }
      }
    }

    static bool gameover(std::vector<Patient> &patients) {
      for(int i = 0; i < patients.size(); i ++) {
        if( patients[i].getS() == Patient::WAIT) {
          return false;
        }
      }
      return true;
    }
  public:
    static int run(std::vector<Patient> & patients, std::vector<Hospital> & hospitals) {
      for(int k = 0; k < hospitals.size() ; k ++ ) {
        for(int j = 0; j < hospitals[k].getAmbulanceNum() ; j ++ ) {
          if ( gameover(patients) == true)
            goto end;
          Ambulance & am = hospitals[k].getAmbulance(j);
          int time = am.getT();
          decreaseTime(patients, time);
          int i = 0;
          for(; i < Ambulance::MAX_PATIENT; i ++){
            int idx = greedyPickPatient(patients, am, time);
            if( idx ==  -1) 
              break;
            am.load(patients, idx);
            am.setL(patients[idx].getL());
            decreaseTime(patients, time);
          }
          if( i == 0) continue;
          time = am.getL().getD(hospitals[0].getL());
          time += am.getPatientNum();
          am.setT(time);
          am.unload(patients);
          am.setL(hospitals[k].getL());
          decreaseTime(patients, time);
        }
      }
end:
      int count = 0;
      for(int i = 0; i < patients.size(); i ++ ) {
        if( patients[i].isSaved()) count ++;
      }
      return count;
    }
};

int main() {
  std::string file = "test";
  std::ifstream ifs(file.c_str());

  std::vector<Patient> patients;
  std::vector<Hospital> hospitals;
  int x, y, t;
  for(int i = 0; i < 300 ;i ++ ) {
    ifs >> x >> y >> t;
    Patient p(x, y, t);
    patients.push_back(p);
  }
  int nam;
  for(int i = 0; i < 5; i ++ ) {
    ifs >> nam;
    Hospital h(nam, i);
    hospitals.push_back(h);
  }

  KMeansLocateHospitalHelper::locate(patients, hospitals);
  int nubmer = GreedyScheduler::run(patients, hospitals);
  return 0;
}
