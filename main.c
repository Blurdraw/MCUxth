#include <REGX52.H>
#include "xth.h"

void bh()
{
  while(1)
  {
		P0_0 = 0;
		delay(500);
		P0_1 = 0;
		delay(500);
		P0_2 = 0;
		delay(500);
		P0_0 = 1;
		P0_1 = 1;
		P0_2 = 1;
		delay(500);
  }
}
void ch()
{
	while (1)
  {
		P0_3 = 0;
		delay(1000);
		P0_4 = 0;
		delay(1000);
		P0_5 = 0;
		delay(1000);
		P0_3 = 1;
		P0_4 = 1;
		P0_5 = 1;
  }
}

void main_app()
{
	create_task(ch);
	bh();
}
