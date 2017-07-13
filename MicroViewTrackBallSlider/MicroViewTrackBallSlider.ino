/*
	MicroView Arduino Library
	Copyright (C) 2014 GeekAmmo

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <MicroView.h>

MicroViewWidget *widget[2];		// declaring an array of 2 MicroViewWidget

#define UP_PIN    2
#define DOWN_PIN  3
#define LEFT_PIN  5
//#define RIGHT_PIN 4
#define BTN_PIN   6

int ud = 0; // up down value
int lr = 0; // left right value

void setup() {
	uView.begin();		// init and start MicroView
	uView.clear(PAGE);	// erase the memory buffer, when next uView.display() is called, the OLED will be cleared.
	widget[0] = new MicroViewSlider(0,0,0,100);		// declare widget0 as a Slider at x=0, y=0, min=0, max=100
	widget[1] = new MicroViewSlider(0,30,0,100);	// declare widget0 as a Slider at x=0, y=30, min=0, max=200
  pinMode(UP_PIN, INPUT);
  pinMode(DOWN_PIN, INPUT);
  pinMode(LEFT_PIN, INPUT);
//  pinMode(RIGHT_PIN, INPUT);
  pinMode(BTN_PIN, INPUT);
  uView.display();
}

void loop() {
  int mouseLeft =   pulseIn(LEFT_PIN, HIGH,     20000);
//  int mouseRight =  pulseIn(RIGHT_PIN, HIGH,  20000);
  int mouseUp =     pulseIn(UP_PIN, HIGH,       20000);
  int mouseDown =   pulseIn(DOWN_PIN, HIGH,     20000);
  if (mouseUp > 0 && ud < 100) {
    ud ++;
  }
  else if (mouseDown > 0 && ud > 0) {
    ud --;
  }
  if (mouseLeft > 0) {
    lr = lr + 1;
    lr = lr % 100;
  }
  if(mouseUp > 0 || mouseLeft > 0 || mouseDown > 0) {
    widget[0]->setValue(lr);
    widget[1]->setValue(ud);
  	uView.display();
  }
}
