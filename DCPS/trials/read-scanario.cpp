#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct {
   int duration;
   double steering;
   double motor;
} TRIPLET; 

static TRIPLET tdata[100];

static char scenario[]=":1,2,3:4,5,6:7,8.9,10.11:8,-12.13,-14.15:999,888.77,-66.33:"; 

int read_scenario(char *ps)
{

   int index=0;

   while(sscanf(ps, ":%d,%lf,%lf", &tdata[index].duration, &tdata[index].steering, &tdata[index].motor) != EOF) {
      ps = strchr(ps+1, ':');
      index++;
   }
   return index;
}

int main(int argc, char *argv[])
{
   int i, index;
   index = read_scenario(scenario);

   for(i=0; i < index; i++){
      printf("%d, %lf, %lf \n", tdata[i].duration, tdata[i].steering, tdata[i].motor);
      //sleep(1);
   }

}
