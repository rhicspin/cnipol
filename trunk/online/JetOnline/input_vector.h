typedef struct hitData{
  int strip;
  int amp;
  int time;
  int tmax;
  int intg;
  int amp2;
  int time2;
  int tmax2;
  int intg2;
  double amp3;
  double time3;
  double tmax3;
  double intg3;
  double f_amp;
  double f_time;
  double f_tmax;
  double f_intg;
}hitData;

typedef struct eventData{
  int event_num;
  int rev;
  int bunch;
  int hit_num;
  int hjet_count;
  int target_inf;
  vector< hitData> hit;

}eventData;

vector< eventData > v_event;
      
eventData test_event;
hitData   hit_test;

int bunch_typ=120;

