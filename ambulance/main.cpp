/* ambulance scheduling algorithm */

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <climits>
#include <cfloat>
#include <cstdlib>
#include <cassert>
#include <utility>
#include <algorithm>

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
  out << l.x << " " << l.y;
  return out;
}
class Patient {
  private:
    static int ID;
    Location l;
    int time;
    int current_time;
    int group;
    int id;
    int saved_by;
  public:
    enum State { WAIT, INCAR, DEAD, SAVED};
    Patient( int x, int y, int t) : l(x, y) {
      time = t;
      current_time = time;
      group = 0;
      saved = WAIT;
      id = ID ++;
    }
    Patient ( Location loc, int t) : l(loc.getX(), loc.getY() ) {
      time = t;
      current_time = time;
      group = 0;
      saved = WAIT;
      id = ID ++;
    }

    State saved;
    inline int getX() const { return l.getX();}
    inline int getY() const { return l.getY();}
    inline int getT() const { return current_time; };
    inline int decreaseT(int t) { current_time -= t;}
    inline void restore() { current_time = time; }
    inline Location getL() const { return l; }
    inline int getG() const { return group; }
    inline void setG(int g) { group = g; }
    inline void setS(State s) { saved = s ;}
    inline void save(int id) { saved_by = id, setS(SAVED);}
    inline State getS() const { return saved; }
    inline bool isSaved() const { return saved == SAVED; }
    friend std::ostream& operator<< ( std::ostream& out, const Patient & p);
};

int Patient::ID = 0;
std::ostream& operator<< ( std::ostream& out, const Patient & p) {
  out << "Patient: id = " << p.id << " location = (" << p.l << ") saved by ambulance " << p.saved_by;
  return out;
}


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
      num = 0;
    }
    Ambulance(const Ambulance& a ) {
      hospital = a.hospital;
      id = a.id;
      current_time = a.current_time;
      num = a.num;
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
        patients[idx].save(id);
      }
      patient_index.clear();
      num  = 0;
    }

    inline int getPatientNum() { return num ; }
    inline int getT() const { return current_time; }
    inline void setT(int time) { current_time = time; }
    inline int getHospitalId() { return hospital; }

    int getShortestTime(std::vector<Patient> & patients) const { 
      int s = INT_MAX;
      for(int i = 0; i < patient_index.size();i ++) {
        int id = patient_index[i];
        if( patients[id].getT() < s)
          s = patients[id].getT();
      }
      return s;
    }
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

    bool compare(Hospital& h) {
      return nam > h.nam;
    }
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
        nums.push_back(0);
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
        std::map<int, int>::iterator iter = locs_num.begin();
        for(; iter != locs_num.end(); iter++) {
          if( iter->first < num ) {
            idx = iter->first;
            num = iter->second;
          }
        }

        min.push_back(locs[idx]);
        locs_num.erase(idx);
      }
      std::map<int , int>::iterator iter = locs_num.begin();
      for(; iter != locs_num.end(); iter ++) {
        max.push_back(locs[iter->first]);
      }
    }


    static bool cluster(std::vector<Patient> &patients, std::vector<Location> & locs, std::vector<int>& weights) {
      bool changed = false;
      int sum = 0;
      for(int i = 0; i < weights.size(); i ++) {
        sum += weights[i];
      }

      for(int i = 0; i < patients.size(); i ++ ){
        Location l = patients[i].getL();
        float dis = FLT_MAX;
        int g;
        for(int j = 0; j < locs.size(); j ++){
          float d = l.getD(locs[j]) * 1.0 * weights[j] / sum;
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
      locs.push_back(Location());
      getGravity(patients, locs);
      std::vector<int> weights(1,1);
      while(locs.size() != num) {
        if( num >= 2 * locs.size()) {
          std::vector<Location> tmps = locs;
          locs.clear();
          weights.clear();

          for(int i = 0; i < tmps.size(); i ++ ) {
            int dir1 = Direction::getRandomDir();
            int dir2 = Direction::getOppositeDir(dir1);
            Location l1 = tmps[i].move(Direction::getStep(dir1));
            Location l2 = tmps[i].move(Direction::getStep(dir2));

            locs.push_back(l1);
            locs.push_back(l2); 
            weights.push_back(1);
            weights.push_back(1);
          }
        }else {
          int need = num - locs.size();
          std::vector<Location> maxlocs;
          std::vector<Location> minlocs;
          getMaxLocation(patients, locs, need,  maxlocs, minlocs);

          locs.clear();
          weights.clear();
          for(int i = 0; i < minlocs.size(); i ++){
            locs.push_back(minlocs[i]);
            weights.push_back(1);
          }
          for(int i = 0; i < maxlocs.size(); i ++ ) {
            int dir1 = Direction::getRandomDir();
            int dir2 = Direction::getOppositeDir(dir1);
            Location l1 = maxlocs[i].move(Direction::getStep(dir1));
            Location l2 = maxlocs[i].move(Direction::getStep(dir2));

            locs.push_back(l1);
            locs.push_back(l2);
            weights.push_back(1);
            weights.push_back(1);
          }
        }

        cluster(patients, locs, weights);
        getGravity(patients, locs);
      }
      for(int i = 0; i < hospitals.size(); i ++) {
        weights[i] = hospitals[i].getAmbulanceNum();
      }
      while(1) {
        bool changed = cluster(patients, locs, weights);
        if( changed == false) break;
        getGravity(patients, locs);
      }
      for(int i = 0; i < num; i ++) {
        hospitals[i].setL(locs[i]);
      }
      return 0;
    }
};


class GreedyScheduler{
  private:
    static int greedyPickPatient(std::vector<Patient> &patients, std::vector<Hospital> & hospitals,  Ambulance& am , int& time ) {
      assert( am.getPatientNum() < Ambulance::MAX_PATIENT);
      Location l = am.getL();
      Location lh = hospitals[am.getHospitalId()].getL();
      int stime = am.getShortestTime(patients);
      int emin = INT_MAX;
      int idx = -1;
      for(int i = 0; i < patients.size(); i ++) {
        if( patients[i].getG() != am.getHospitalId()) continue;
        int ss = std::min(patients[i].getT(), stime);
        Location lp = patients[i].getL();
        if(patients[i].getS() == Patient::WAIT && l.getD(lp)  +  lp.getD(lh) + 1 + am.getPatientNum() + 1 <= ss) {
          //int e = l.getD(patients[i].getL()) + patients[i].getT();
          int e = l.getD(lp);
          if( e < emin) {
            emin = e;
            idx = i;
          }
        }
      }
      time = l.getD(patients[idx].getL()) + 1;
      return idx;
    }


    static void decreaseTime(std::vector<Patient> & patients, int time) {
      for(int i = 0; i < patients.size(); i ++) {
        int t = patients[i].getT();
        if( patients[i].isSaved()) continue;
        if( t - time < 0) {
          patients[i].setS(Patient::DEAD);
        }
        else {
          patients[i].decreaseT(time);
        }
      }
    }
    static void restoreTime(std::vector<Patient> & patients) {
      for(int i = 0 ; i < patients.size();i ++) {
        patients[i].restore();
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
    static bool compare(Hospital h1, Hospital h2) {
      return h1.compare(h2);
    }
  public:
    static int run(std::vector<Patient> & patients, std::vector<Hospital> & hospitals) {
      for(int k = 0; k < hospitals.size() ; k ++ ) {
        for(int j = 0; j < hospitals[k].getAmbulanceNum() ; j ++ ) {
          restoreTime(patients);
          if ( gameover(patients) == true)
            goto end;
          Ambulance & am = hospitals[k].getAmbulance(j);
          int time = am.getT();
          decreaseTime(patients, time);
          int run_time = 0;
          int i = 0;
          for(; i < Ambulance::MAX_PATIENT; i ++){
            int idx = greedyPickPatient(patients, hospitals, am, time);
            if( idx ==  -1) 
              break;
            am.load(patients, idx);
            am.setL(patients[idx].getL());
            decreaseTime(patients, time);
            run_time += time;
          }
          if( i == 0) continue;
          time = am.getL().getD(hospitals[0].getL());
          time += am.getPatientNum();
          run_time += time;
          am.setT(run_time);
          am.unload(patients);
          am.setL(hospitals[k].getL());
          decreaseTime(patients, time);
        }
      }
end:
      int count = 0;
      for(int i = 0; i < patients.size(); i ++ ) {
        if( patients[i].isSaved()) {
          std::cout << patients[i] << std::endl;
          count ++;
        }
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
  char c;
  for(int i = 0; i < 300 ;i ++ ) {
    ifs >> x >> c >>  y >> c >> t;
    //std::cout << x << " " << y << " " << t << std::endl;
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
  int number = GreedyScheduler::run(patients, hospitals);
  std::cout << number << " patients been saved" << std::endl;
  return 0;
}
