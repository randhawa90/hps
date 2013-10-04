/* ambulance scheduling algorithm */

#include <iostream>
#include <string>
#include <map>
#include <climits>

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
    inline void reset() { x = 0; y = 0; }
};

class Patient {
  private:
    Location l;
    int time;
    int groups;
  public:
    Peopple( int x, int y, int t) : l(x, y) {
      time = t;
      group = 0;
    }
    Patient ( Location loc, int t) : l(loc.getX(), loc.getY() ) {
      time = t;
      group = 0;
    }
    inline int getX() const { return l.getX();}
    inline int getY() const { return l.getY();}
    inline int getT() const { return time; };
    inline Location getLoc() const { return l; }
    inline void setT(int t) { time  = t; }
    inline int getG() const { return group; }
    inline void setG(int g) { group = g; }
};


class Hospital{
  public:
    Hospital(int num) {
      nam = num;
    }

    inline void setL(Location loc ) { l = loc; }
    inline void setL(int x, int y) { l.setX(x); l.setY(y); }
  private:
    Location l;
    int nam;
}


class KMeansLocateHospitalHelper {
  private:
    static void getGravity(std::<Patient> & patients , std::vector<Location> locs) {
      std::vector<int> nums;
      for(int i = 0; i < locs.size(); i ++) {
        locs[i].reset();
        nums[i] = 0;
      }
      for(int i = 0; i < patients.size() ; i ++ ) {
        int g = patients[i].getG();
        locs[g].setX(locs[g].getX() + patients[i].getX());
        locs[g].setY(locs[g].getY() + patients[i].getY());
        num[g]++;
      }

      for(int i = 0; i < locs.size();i ++){
        locs[i].setX(locs[i].getX() / num[i]);
        locs[i].setY(locs[i].getY() / num[i]);
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
        for(int j = 0; j < locs_num; j ++ ) {
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


    static bool cluster(std::vector<Patient> &patients, std::vecote<Location> & locs) {
      bool changed = false;
      for(int i = 0; i < patients.size(); i ++ ){
        Location l = patients[i].getLoc();
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
      while(locs.size() != num) {
        if( num >= 2 * locs.size()) {
          std::vector<Location> tmps = locs;
          locs.clear();

          for(int i = 0; i < tmps.size(); i ++ ) {
            Location l1 = tmps[i].move(-1, -1);
            Location l2 = tmps[i].move(1, 1);

            locs.push_back(l1);
            locs.push_back(l2);
          }
        }else {
          int need = num - groups.size();
          std::vector<Location> maxlocs;
          std::vector<Location> minlocs;
          getMaxLocation(patient, locs, need,  maxlocs, minlocs);

          locs.clear();
          for(int i = 0; i < minlocs.size(); i ++){
            locs.push_back(minlocs[i]);
          }
          for(int i = 0; i < maxlocs.size(); i ++ ) {
            Location l1 = tmps[i].move(-1, -1);
            Location l2 = tmps[i].move(1, 1);

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


class DijkstraScheduler {
  public:
    static int run(std::vector<Patient> & patients, std::vector<Hospital> & hospitals) {
      return 0;
    }
}

int main() {
  std::string file = "test";
  ifstream ifs(file);

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
    Hospital h(nam);
    hospitals.push_back(h);
  }

  KMeansLocateHospitalHelper::locate(patients, hospitals);
  int nubmer = DijkstraScheduler::run(patieents, hospitals);
  return 0;
}
