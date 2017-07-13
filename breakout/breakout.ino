#include <MicroView.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 48

#define UP_PIN    2
#define DOWN_PIN  3
#define LEFT_PIN  5
//#define RIGHT_PIN 4
#define BTN_PIN   6

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
    if(this->x < SCREEN_WIDTH - 4) {
      this->x+=4;
    }
  }
  void right() {
    if(this->x > 4) {
      this->x-=4; 
    }
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
  static const uint8_t ballystart=30;
  static const uint8_t max_bricks=40;

  Brick *bricks[GameScene::max_bricks];

  GameScene() : player(SCREEN_WIDTH/2 - Bat::width/2, 45), ball(SCREEN_WIDTH/2 - 1, ballystart) {
      for (int n=0; n<GameScene::max_bricks; n++) {
        this->bricks[n] = NULL;
      }

      this->reset();
  }

  void reset() {
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
          if(this->bricks[y*10+x] == NULL) {
            this->bricks[y*10 + x] = new Brick(brickx, bricky);
          }
        }
      }
      this->ball.x = SCREEN_WIDTH/2 - 1;
      this->ball.y = ballystart;
  }

  // We will use MouseUp and Down for this game... And orient the trackball accordingly.
  void controls(int mouseLeft, int mouseRight) {
    if (mouseLeft) {
      this->player.left();
    }
    else if(mouseRight) {
      this->player.right();
    }
  }

  void collisions() {
    // Is it at the right height for the bat?
    int ballxmax = max(this->ball.x + this->ball.xdir, this->ball.x);
    int ballxmin = min(this->ball.x + this->ball.xdir, this->ball.x);
    int ballymax = max(this->ball.y + this->ball.ydir, this->ball.y);
    int ballymin = min(this->ball.y + this->ball.ydir, this->ball.y);
    if(ballymax > 43 && 
      ballxmax > this->player.x && ballxmin < this->player.x + Bat::width) {
        this->ball.ydir = -1;
      }
    if(ballymin < 15) {
      for(int n=0; n<GameScene::max_bricks; n++) {
        Brick * current=this->bricks[n];
        if(current) {
          if(this->ball.x >= current->x && this->ball.x <= (current->x + Brick::width) &&
            ballymin >= current->y && ballymin <= current->y + 2) {
              // BALL brick collision
              delete(current);
              this->bricks[n] = NULL;
              this->ball.ydir = -this->ball.ydir;
          }
        }
      }
    }
  }

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
      this->reset();
    }
    // Collisions!
    this->collisions();
  }

  void draw() {
    this->ball.draw();
    this->player.draw();
    for(int i=0; i<GameScene::max_bricks; i++) {
      if (this->bricks[i]) {
        this->bricks[i]->draw();
      }
    }
    
  }

  void game_over() {

  }
};

class TrackDir {
  int positive_pin;
  int negative_pin;
  int positive_pin_old;
  int negative_pin_old;

public:
  TrackDir(int positive_pin, int negative_pin) : 
    positive_pin{positive_pin}, 
    negative_pin{negative_pin}, 
    positive_pin_old{0},
    negative_pin_old{0} {
      pinMode(positive_pin, INPUT);
      pinMode(negative_pin, INPUT);
    }

  int read() {
    int new_value = digitalRead(this->positive_pin);
    if(new_value != this->positive_pin_old) {
      this->positive_pin_old = new_value;
      return 1;
    }
    new_value = digitalRead(this->negative_pin);
    if(new_value != this->negative_pin_old){
      this->negative_pin_old = new_value;
       return -1;
    }
    return 0;
  }
};

GameScene *scene;
TrackDir *updown;

void setup() {
	uView.begin();		// begin of MicroView
	uView.clear(ALL);	// erase hardware memory inside the OLED controller
	uView.display();	// display the content in the buffer memory, by default it is the MicroView logo
  pinMode(LEFT_PIN, INPUT);
//  pinMode(RIGHT_PIN, INPUT);
  pinMode(BTN_PIN, INPUT);
  updown = new TrackDir(UP_PIN, DOWN_PIN);
  scene = new GameScene();
}

int draw_frame = 1;
void loop() {
  int moved = updown->read();
  scene->controls(moved < 0, moved > 0);
  draw_frame --;

  if(draw_frame == 0) {
    uView.clear(PAGE);	// erase the memory buffer, when next uView.display() is called, the OLED will be cleared.
    scene->update();
    scene->draw();
    uView.display();
    draw_frame = 30;
  }
  delay(1);
}
