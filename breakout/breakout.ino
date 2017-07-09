#include <MicroView.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 48

class Brick {
public:
  static const uint8_t width=4;
  static const uint8_t height=2;
  
  uint8_t x;
  uint8_t y;
  Brick(uint8_t x, uint8_t y) : x{x}, y{y} {}

  void draw() {
    uView.rect(this->x, this->y, 4, 2);
    uView.rect(this->x, this->y, Brick::width, Brick::height);
  }
};

class Bat {
public:
  uint8_t x;
  uint8_t y;
  static const uint8_t width=6;
  static const uint8_t height=2;

  Bat(uint8_t x, uint8_t y) : x{x}, y{y} {}

  void left() {
    this->x++;
  }
  void right() {
    this->x--;
  }
  
  void draw() {
    uView.rect(this->x, this->y, Bat::width, Bat::height);
  }
};

class Ball {
public:
  uint8_t x;
  uint8_t y;
  uint8_t xdir;
  uint8_t ydir;
  
  Ball(uint8_t x, uint8_t y) : x{x}, y{y}, xdir{1}, ydir{1} {}

  void update() {
    this->x += this->xdir;
    this->y += this->ydir;
  }

  void draw() {
    uView.rect(this->x, this->y, 2, 2);
  }
};

class Scene {
public:
  void frame();
};

class GameScene {
public:
  Bat player;
  Ball ball;
  static const uint8_t max_bricks=40;
  Brick *bricks[GameScene::max_bricks];

  GameScene() : player(SCREEN_WIDTH/2 - Bat::width/2, 45), ball(SCREEN_WIDTH/2 - 1, 30) {
      // Prepare rows of bricks
      // Padding is like this
      //  Screen Padding
      //   |    Brick padding
      //   |      |
      //   v      v
      // |  *****  ****
      uint8_t x, y;
      const uint8_t brick_xpadding = 2; 
      const uint8_t brick_screen_xpadding = 2;
      const uint8_t brick_ypadding = 2;
      const uint8_t brick_screen_ypadding =0;
      for (y = 0; y < 4; y++) {
        uint8_t bricky = y * (Brick::height + brick_ypadding) + brick_ypadding/2 + brick_screen_ypadding;
        for (x=0; x< 10; x++) {
          uint8_t brickx = x*(Brick::width + brick_xpadding) + brick_xpadding/2 + brick_screen_xpadding;
          this->bricks[y*10 + x] = new Brick(brickx, bricky);
        }
      }
  }
  
  void left() {}
  void right() {}
  void button() {}

  void update() {
    // Ball bouncing
    this->ball.update();
    if (this->ball.x > SCREEN_WIDTH-2 || this->ball.x < 1) {
      this->ball.xdir = -this->ball.xdir;
    }
    if (this->ball.y < 1) {
      this->ball.ydir = -this->ball.ydir;
    }
    if (this->ball.y > SCREEN_HEIGHT-2) {
      this->ball.ydir = -this->ball.ydir;
      //this->game_over();
      //return;
    }
    // Collisions!

  }

  void draw() {
    this->ball.draw();
  }

  void game_over() {

  }
};

GameScene *scene;

void setup() {
	uView.begin();		// begin of MicroView
	uView.clear(ALL);	// erase hardware memory inside the OLED controller
	uView.display();	// display the content in the buffer memory, by default it is the MicroView logo
  scene = new GameScene();
}

void loop() {
	uView.clear(PAGE);	// erase the memory buffer, when next uView.display() is called, the OLED will be cleared.
  scene->update();
  scene->draw();
  uView.display();	// display the content in the buffer memory, by default it is the MicroView logo
  delay(50);
}
