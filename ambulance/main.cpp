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
#include <sstream>
#include <sys/time.h>

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
  out << "(" << l.x << "," << l.y << ")";
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
    State saved;
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
    Patient(const Patient & p )
        : l(p.l), time(p.time), current_time(p.current_time),
        group(p.group), id(p.id), saved_by(p.saved_by), saved(p.saved)
    {
    }
    inline int getX() const { return l.getX();}
    inline int getY() const { return l.getY();}
    inline int getT() const { return current_time; };
    inline int getST() const { return time; }
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
  if( p.isSaved() )
    out << "Patient: id = " << p.id << " location = " << p.l << " saved by ambulance " << p.saved_by;
  else
    out << "Patient: id = " << p.id << " lecation = " << p.l << " group by " << p.group;
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
      l = a.l;
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
    inline int getID() const { return id; }
    std::vector<int> getPatientIndex() const { return patient_index; }
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

    Hospital(const Hospital & h)
        :l( h.l ), nam(h.nam), id(h.id), ambulances(h.ambulances)
    {
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

    static std::vector<int> getNumPatient(std::vector<Patient> & patients) {
      std::map<int, int> locs_num;
      for(int i = 0; i < patients.size(); i ++ ) {
        if( locs_num.find(patients[i].getG()) == locs_num.end()) {
          locs_num[patients[i].getG()] = 0;
        }
        locs_num[patients[i].getG()] ++;
      }
      std::vector<int> nums(locs_num.size(), 0);
      std::map<int, int>::iterator iter;
      for(iter = locs_num.begin(); iter != locs_num.end(); iter++){
        nums[iter->first] = iter->second;
      }
      return nums;
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
          float d = l.getD(locs[j]) * 1.0 * sum / weights[j];
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
    static std::vector<Location> locate(std::vector<Patient> & patients, std::vector<Hospital> & hospitals) {
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
        while(1) {
          bool changed = cluster(patients, locs, weights);
          if( changed == false) break;
          getGravity(patients, locs);
        }
      }
      
      for(int i = 0; i < hospitals.size(); i ++) {
        weights[i] = hospitals[i].getAmbulanceNum();
      }
      while(1) {
        bool changed = cluster(patients, locs, weights);
        if( changed == false) break;
        getGravity(patients, locs);
      }
      return locs;
    }
};


class GreedyScheduler{
  private:
    static int greedyPickPatient(std::vector<Patient> &patients, std::vector<Hospital> & hospitals,  Ambulance& am , int& time ) {
      assert( am.getPatientNum() < Ambulance::MAX_PATIENT);
      Location l = am.getL();
      int stime = am.getShortestTime(patients);
      int emin = INT_MAX;
      int idx = -1;
      for(int i = 0; i < patients.size(); i ++) {
        //if( patients[i].getG() != am.getHospitalId()) continue;
        int ss = std::min(patients[i].getT(), stime);
        int hid = patients[i].getG();
        Location lp = patients[i].getL();
        Location lh = hospitals[hid].getL();
        if(patients[i].getS() == Patient::WAIT && l.getD(lp)  +  lp.getD(lh) + 2 <= ss) {
          int e = l.getD(patients[i].getL()) + patients[i].getT() / 4;
          //int e = l.getD(lp);
          if( e < emin) {
            emin = e;
            idx = i;
          }
        }
      }
      if( idx != -1)
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
        if(!patients[i].isSaved()) 
          patients[i].setS(Patient::WAIT);
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
    static int run(std::vector<Patient> patients, std::vector<Hospital> hospitals, int verbose) {
      int count = 0;
      while(1) {
        int saved = 0;
        for(int k = 0; k < hospitals.size(); k ++) {
          for(int j = 0; j < hospitals[k].getAmbulanceNum() ; j ++ ) {
            restoreTime(patients);
            if ( gameover(patients) == true)
              goto end;
            Ambulance & am = hospitals[k].getAmbulance(j);
            int time = am.getT();
            decreaseTime(patients, time);
            int run_time = time;
            int i = 0;
            for(; i < Ambulance::MAX_PATIENT; i ++){
              int idx = greedyPickPatient(patients, hospitals, am, time);
              if( idx ==  -1) 
                break;
              saved = 1;
              am.load(patients, idx);
              am.setL(patients[idx].getL());
              decreaseTime(patients, time);
              run_time += time;
            }
            if( i == 0) continue;
            count += i;
            if( verbose == 1) 
                std::cout << "ambulance " << am.getID();
            std::vector<int> idxs = am.getPatientIndex();
            for(int i = 0; i < idxs.size(); i ++) {
              int id = idxs[i];
              //std::cout << " " << id << " (" << patients[id].getG() << "," << patients[id].getX() << "," << patients[id].getY() << "," << patients[id].getST()
              if( verbose == 1) 
                std::cout << " " << id << " (" << patients[id].getX() << "," << patients[id].getY() << "," << patients[id].getST()
                        << ");";
            }
            int idx = idxs.back();
            int hid = patients[idx].getG();
            Location hl = hospitals[hid].getL();
            time = am.getL().getD(hl) + 1;
            run_time += time;
            am.setT(run_time);
            am.unload(patients);
            am.setL(hl);
            if( verbose)
                std::cout << am.getL() << std::endl;
            decreaseTime(patients, time);
          }
        }
        if( saved == 0) break;
      }
end:
      //int count = 0;
      std::map<int , int> num_dead;
      num_dead[0] = 0;
      num_dead[1] = 0;
      num_dead[2] = 0;
      num_dead[3] = 0;
      num_dead[4] = 0;
      for(int i = 0; i < patients.size(); i ++ ) {
        //std::cout << patients[i] << std::endl;
        if( patients[i].isSaved()) {
          //count ++;
        } else {
          num_dead[patients[i].getG()] ++;
        }
      }

     /* 
      std::map<int, int>::iterator iter = num_dead.begin();
      for(; iter != num_dead.end(); iter ++) {
        std::cout << iter->first << " " << iter->second << std::endl;
      }
      */
      return count;
    }
};

void better_locate(std::vector<Patient> & patients, std::vector<Hospital> & hospitals, std::vector<Location> &locs){
    std::cout << "hospitals ";
    for(int i = 0; i < locs.size(); i ++ ){
        int d = INT_MAX;
        int idx;
        Location ll = locs[i];
        for(int j = 0; j < patients.size(); j ++) {
            int e = patients[j].getL().getD(ll);
            if( e < d) {
                d = e; idx = j;
            }
        }
        hospitals[i].setL(patients[idx].getL());
        std::cout << " " << i << " " << patients[idx].getL() << ";";
    }
    std::cout << std::endl;
}

std::vector< std::vector<Location> > gen_loc(std::vector<Location> & locs) {
    std::vector< std::vector< Location> > multlocs;
    for(int i = 0; i < locs.size(); i++){
        std::vector< Location > lls;
        Location l = locs[i];
        lls.push_back(l.move(0, -1));
        lls.push_back(l.move(0, 1));
        lls.push_back(l.move(1, 0));
        lls.push_back(l.move(-1, 0));
        lls.push_back(l.move(0, 0));
        
        multlocs.push_back(lls);
    }
    std::vector< std::vector< Location> > retlocs;
    std::vector<Location> slocs;
    for(int a = 0; a < 5; a ++) {
        slocs.push_back(multlocs[0][a]);
        for(int b = 0; b < 5; b++ ){
            slocs.push_back(multlocs[1][b]);
            for(int c = 0; c < 5; c++ ){
                slocs.push_back(multlocs[2][c]);
                for(int d = 0; d < 5; d++ ){
                    slocs.push_back(multlocs[3][d]);
                    for(int e = 0; e < 5; e++ ){
                        slocs.push_back(multlocs[4][e]);
                        retlocs.push_back(slocs);
                        slocs.pop_back();
                    }
                    slocs.pop_back();
                }
                slocs.pop_back();
            }
            slocs.pop_back();
       }
        slocs.pop_back();
    }
    assert(retlocs.size() == 3125);
    return retlocs;
}

void set_locs(std::vector<Hospital> & hospitals, std::vector< Location > & locs, int verbose) {
    if( verbose)
        std::cout << "hospitals ";
    for(int i = 0; i< locs.size(); i ++) {
        hospitals[i].setL(locs[i]);
        if( verbose)
            std::cout << " " << i << " " << hospitals[i].getL() << ";";
    }
    if( verbose)
        std::cout << std::endl;
}

void printout_locs(std::vector<Location> locs){
    for(int i = 0; i< locs.size(); i ++) {
        std::cout << locs[i] << std::endl;
    }
}

int main() {
  std::string file = "input";
  std::ifstream ifs(file.c_str());

  std::vector<Patient> patients;
  std::vector<Hospital> hospitals;
  int x, y, t;
  char c;
  std::string tmp;
  getline(ifs, tmp);
  for(int i = 0; i < 300 ;i ++ ) {
    ifs >> x >> c >>  y >> c >> t;
    //std::cout << x << " " << y << " " << t << std::endl;
    Patient p(x, y, t);
    patients.push_back(p);
  }

  while( 1) {
    ifs >> c;
    if( !isspace(c) )
      break;
  }
  getline(ifs,tmp);
  int nam;
  for(int i = 0; i < 5; i ++ ) {
    ifs >> nam;
    Hospital h(nam, i);
    hospitals.push_back(h);
  }
  struct timeval tv1;
  struct timeval tv2;
  gettimeofday(&tv1, NULL);
  std::vector< Location > locs = KMeansLocateHospitalHelper::locate(patients, hospitals);
  printout_locs(locs);
  //better_locate(patients, hospitals, locs);
  std::vector< std::vector< Location> > retlocs = gen_loc(locs);
  int d = 0;
  int id = 0;
  std::vector< Location> best_loc;
  for(int i = 0; i < retlocs.size(); i ++) {
    set_locs(hospitals, retlocs[i], 0);
    printout_locs(retlocs[i]); 
    int number = GreedyScheduler::run(patients, hospitals, 0);
    std::cout << number << " saved" << std::endl;
    if( number > d) {
        id = i;
        d = number;
        best_loc = retlocs[i];
    }
  }
  //set_locs(hospitals, best_loc, 1);
  //set_locs(hospitals, locs, 1);
  set_locs(hospitals, best_loc, 1);
  int number = GreedyScheduler::run(patients, hospitals, 1);
  gettimeofday(&tv2, NULL);
  printf ("Total time = %f seconds\n",
           (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +
           (double) (tv2.tv_sec - tv1.tv_sec));
  //std::cout << number << " patients been saved" << std::endl;
  return 0;
}
