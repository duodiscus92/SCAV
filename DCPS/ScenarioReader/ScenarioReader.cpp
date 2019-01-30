#include "ScenarioReader.h"

ScenarioReader::ScenarioReader()
{
   items_found =  next_item = 0;
}
ScenarioReader::~ScenarioReader(){}

int ScenarioReader::ScenarioParse(char *ps)
{
   int status;
   if(items_found >= MAX_SCENARIO_ITEMS)
      return  ERROR_MAX_ITEM;
   while((status = sscanf(ps, ":%d,%f,%f",
	 &tdata[items_found].duration,
	 &tdata[items_found].steering,
	 &tdata[items_found].motor)) != EOF) {
      ps = strchr(ps+1, ':');
      items_found++;
   }
   return status;
}

int ScenarioReader::ScenarioGetItem(int *duration, float *steering, float *motor)
{
   if (next_item < items_found){
      *duration = tdata[next_item].duration;
      *steering = tdata[next_item].steering;
      *motor = tdata[next_item].motor;
      next_item++;
      return 0;
   }
   return ERROR_NO_ITEM;
}


static char scenario[]=":1,2,3:4,5,6:7,8.9,10.11:8,-12.13,-14.15:999,888.77,-66.33:"; 

#ifdef STAND_ALONE
// Test
int main(int argc, char *argv[])
{
   int duration; float steering, motor;
   ScenarioReader myreader;

   myreader.ScenarioParse(scenario);

   while(!myreader.ScenarioGetItem(&duration, &steering, &motor) ) {
      printf("%d, %lf, %lf \n", duration, steering, motor);
      sleep(1);
   }
}
#endif
