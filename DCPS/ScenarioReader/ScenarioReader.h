#ifndef	SCENARIOREADER_H
#define SCENARIOREADER_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_SCENARIO_ITEMS (100)
#define ERROR_MAX_ITEM (-2)
#define ERROR_NO_ITEM  (-3)

typedef struct {
   int duration;
   float steering;
   float  motor;
} TRIPLET; 

class ScenarioReader
{

public:

ScenarioReader();
~ScenarioReader();

//parse scenario string
int ScenarioParse(char *scenario);
//get next decoded item
int ScenarioGetItem(int *duration, float *steering, float * motor);

private:

// number of items found in the current scenario
int items_found;

// next item to deliver
int next_item;

// table of decoded  scenario items
TRIPLET tdata[MAX_SCENARIO_ITEMS];

};
#endif
