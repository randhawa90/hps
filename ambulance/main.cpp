/* ambulance scheduling algorithm */

#include <iostream>
#include <string>

class Location {
  private:
    int x, y;
  public:
    Location(int x, int y) {
      this->x = x;
      this->y = y;
    }
    Location() {
      x = -1;
      y = -1;
    }
    inline const int getX() { return x; }
    inline const int getY() { return y; }
    inline void setX(int x) { this->x = x;}
    inline void setY(int y) { this->y = y;}
};

class Patient {
  private:
    Location l;
    int time;
  public:
    Peopple( int x, int y, int t) : l(x, y) {
      time = t;
    }
    Patient ( Location loc, int t) : l(loc.getX(), loc.getY() ) {
      time = t;
    }
    inline const int getX() { return l.getX();}
    inline const int getY() { return l.getY();}
    inline const int getT() { return time; };
    inline void setT(int t) { time  = t; }
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
  public:
    static float locate(std::vector<Patient> & patients, std::vector<Hospital> & hospitals) {
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
