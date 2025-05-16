#define RIGHT_ENABLE 9
#define RIGHT_FORWARD 7
#define RIGHT_BACKWARD 8
#define LEFT_FORWARD 11
#define LEFT_BACKWARD 12
#define LEFT_ENABLE 10
#define MIDDLE_SENSOR 4
#define RIGHT_SENSOR 5
#define LEFT_SENSOR 6
#define SONAR_TRIGGER 1
#define SONAR_ECHO 2

enum class Motor {
  Right,
  Left,
};

enum class Sensor {
  Right,
  Middle,
  Left,
};

enum class Direction {
  Forward,
  Backward,
};

void setup() {
  init_pin_mode();

  digitalWrite(LED_BUILTIN, LOW);
  motor_direction(Direction::Forward);
  // task_one();
}

void loop() {
  // task_simple_track();
  task_avoid_near_stuff();
  task_complex_track();
}

void task_one() {
  int left_speed = 135;
  int right_speed = left_speed - 2;
  motor_direction(Direction::Forward);
  motor_speed(Motor::Left, left_speed);
  motor_speed(Motor::Right, right_speed);
  delay(2200);
  motor_speed(0);

  delay(150);

  motor_direction(Motor::Left, Direction::Backward);
  motor_speed(left_speed);
  delay(330);
  motor_speed(0);

  delay(1000);

  motor_direction(Motor::Left, Direction::Forward);
  motor_direction(Motor::Right, Direction::Backward);
  motor_speed(left_speed);
  delay(660);
  motor_speed(0);

  delay(1000);

  motor_speed(left_speed);
  delay(340);
  motor_speed(0);

  delay(1000);

  motor_direction(Direction::Forward);
  motor_speed(Motor::Right, left_speed);
  motor_speed(Motor::Left, right_speed);
  delay(2200);
  motor_speed(0);
}

typedef unsigned long utime;
typedef struct State {
  bool left;
  bool middle;
  bool right;
  utime last_left;
  utime last_middle;
  utime last_right;
} State;

State state = State{ false, false, false, millis(), millis(), millis() };

void task_simple_track() {
  bool middle = sensor_is_black(Sensor::Middle);
  bool right = sensor_is_black(Sensor::Right);
  bool left = sensor_is_black(Sensor::Left);

  if (middle) {
    if (left && !right) {
      motor_speed(Motor::Left, 0);
      motor_speed(Motor::Right, 95);
      delay(45);
    } else if (!left && right) {
      motor_speed(Motor::Left, 95);
      motor_speed(Motor::Right, 0);
      delay(45);
    } else if (!state.middle) {
      digitalWrite(LED_BUILTIN, HIGH);
      if (!state.left && state.right) {
        motor_speed(Motor::Left, 0);
        motor_speed(Motor::Right, 95);
        delay(45);
      } else if (state.left && !state.right) {
        motor_speed(Motor::Left, 95);
        motor_speed(Motor::Right, 0);
        delay(45);
      } else {
        motor_speed(Motor::Left, 85);
        motor_speed(Motor::Right, 85);
      }
      digitalWrite(LED_BUILTIN, LOW);
    } else {
      motor_speed(Motor::Left, 85);
      motor_speed(Motor::Right, 85);
    }
  } else if (left || right) {
    if (left && !right) {
      motor_speed(Motor::Left, 0);
      motor_speed(Motor::Right, 95);
      delay(60);
    } else if (!left && right) {
      motor_speed(Motor::Left, 95);
      motor_speed(Motor::Right, 0);
      delay(60);
    }
  } else {
    utime now = millis();
    utime last_black = min(min(state.last_middle, state.last_left), state.last_right);
    if (now - last_black >= 500) {
      motor_speed(0);
    } else {
      motor_speed(95);
    }
    // if (state.left && !state.right) {
    //   motor_speed(Motor::Left, 0);
    //   motor_speed(Motor::Right, 95);
    //   delay(80);
    // } else if (!state.left && state.right) {
    //   motor_speed(Motor::Left, 95);
    //   motor_speed(Motor::Right, 0);
    //   delay(80);
    // } else {
    //   motor_speed(Motor::Left, 80);
    //   motor_speed(Motor::Right, 80);
    // }
  }

  state.middle = middle;
  state.right = right;
  state.left = left;
  if (middle) {
    state.last_middle = millis();
  }
  if (left) {
    state.last_left = millis();
  }
  if (right) {
    state.last_right = millis();
  }
}

void task_complex_track() {
  bool middle = sensor_is_black(Sensor::Middle);
  bool right = sensor_is_black(Sensor::Right);
  bool left = sensor_is_black(Sensor::Left);
  bool near = is_near_cm(10);

  if (middle) {
    if (left && !right) {
      motor_speed(Motor::Left, 0);
      motor_speed(Motor::Right, 90);
      delay(35);
    } else if (!left && right) {
      motor_speed(Motor::Left, 90);
      motor_speed(Motor::Right, 0);
      delay(35);
    } else if (!left && !right) {
      motor_speed(85);
      delay(35);
    } else {  // all black
      if (state.left && !state.right) {
        motor_direction(Direction::Backward);
        motor_speed(Motor::Left, 90);
        motor_speed(Motor::Right, 80);
        delay(30);
        motor_direction(Direction::Forward);
      } else if (!state.left && state.right) {
        motor_direction(Direction::Backward);
        motor_speed(Motor::Left, 80);
        motor_speed(Motor::Right, 90);
        delay(30);
        motor_direction(Direction::Forward);
      } else {
        motor_direction(Direction::Backward);
        motor_speed(85);
        delay(30);
        motor_direction(Direction::Forward);
      }
    }
  } else if (left || right) {
    if (left && !right) {
      motor_speed(Motor::Left, 0);
      motor_speed(Motor::Right, 85);
      delay(35);
    } else if (!left && right) {
      motor_speed(Motor::Left, 85);
      motor_speed(Motor::Right, 0);
      delay(35);
    } else {
      motor_direction(Direction::Backward);
      motor_speed(85);
      delay(35);
      motor_direction(Direction::Forward);
    }
  } else {
    // TODO: check if time duration check is needed
    digitalWrite(LED_BUILTIN, HIGH);
    if (state.last_middle > state.last_left && state.last_middle > state.last_right) {
      // if (
      //   state.last_middle > state.last_left
      //   && state.last_middle > state.last_right
      //   && min(state.last_middle - state.last_left, state.last_middle - state.last_right) > 150) {  // > 0.15s
      motor_speed(85);
      delay(30);
    } else if (state.last_left > state.last_right) {
      // } else if (state.last_left > state.last_right && state.last_left - state.last_right > 0) {
      motor_direction(Direction::Backward);
      motor_speed(Motor::Left, 90);
      motor_speed(Motor::Right, 85);
      delay(35);
      motor_direction(Direction::Forward);
    } else if (state.last_left < state.last_right) {
      // } else if (state.last_left < state.last_right && state.last_right - state.last_left > 0) {
      motor_direction(Direction::Backward);
      motor_speed(Motor::Left, 85);
      motor_speed(Motor::Right, 90);
      delay(35);
      motor_direction(Direction::Forward);
    } else {
      motor_direction(Direction::Backward);
      motor_speed(85);
      delay(35);
      motor_direction(Direction::Forward);
    }
    digitalWrite(LED_BUILTIN, LOW);
  }

  state.middle = middle;
  state.right = right;
  state.left = left;
  if (middle) {
    state.last_middle = millis();
  }
  if (left) {
    state.last_left = millis();
  }
  if (right) {
    state.last_right = millis();
  }
}

void task_avoid_near_stuff() {
  if (!is_near_cm(30)) {
    // motor_direction(Direction::Forward);
    // motor_speed(100);
    return;
  }
  digitalWrite(LED_BUILTIN, HIGH);
  motor_speed(Motor::Left, 0);
  motor_speed(Motor::Right, 100);
  delay(280);
  motor_speed(Motor::Left, 105);
  motor_speed(Motor::Right, 80);
  delay(1250);
  digitalWrite(LED_BUILTIN, LOW);

  state.last_middle = millis();
}

bool is_near_cm(unsigned long distance) {
  digitalWrite(SONAR_TRIGGER, HIGH);
  delayMicroseconds(20);
  digitalWrite(SONAR_TRIGGER, LOW);
  unsigned long max_duration = distance * 2 * 1000 * 1000 / 100 / 350;
  unsigned long pulse = pulseIn(SONAR_ECHO, HIGH, 5714);  // max: 1 m
  return pulse != 0 && pulse < max_duration;
}

void test_if_near(unsigned long cm) {
  if (is_near_cm(cm)) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void test_if_middle_is_black() {
  if (sensor_is_black(Sensor::Middle)) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void test_go_when_not_black() {
  if (sensor_is_black(Sensor::Middle)) {
    digitalWrite(LED_BUILTIN, HIGH);
    motor_speed(0);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    motor_speed(150);
  }
}

void test_z_path() {
  digitalWrite(LED_BUILTIN, HIGH);
  motor_speed(Motor::Left, 0);
  motor_speed(Motor::Right, 150);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);

  motor_speed(Motor::Left, 150);
  motor_speed(Motor::Right, 150);
  delay(500);

  digitalWrite(LED_BUILTIN, HIGH);
  motor_speed(Motor::Left, 150);
  motor_speed(Motor::Right, 0);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);

  motor_speed(Motor::Left, 150);
  motor_speed(Motor::Right, 150);
  delay(500);
}

void test_idk() {
  // delay(900);
  // motor_direction(Direction::Backward);
  // delay(900);
  // motor_direction(Direction::Forward);
  /*
	delay(550);
	motor_speed(0);
	delay(100);
	motor_speed(Motor::Left, 95);
	motor_speed(Motor::Right, 0);
	delay(500);
	motor_speed(0);
	delay(100);
	motor_speed(90);

	delay(400);
	motor_speed(0);
	delay(100);
	motor_speed(Motor::Left, 95);
	motor_speed(Motor::Right, 0);
	delay(500);
	motor_speed(0);
	delay(100);
	motor_speed(90);
	*/
  // test_go_when_not_black();
  // test_z_path();
  // test_if_middle_is_black();
  // test_simple_track();
}

int speed(float s) {
  int min = 80;
  int max = 255;
  return s * (max - min) + min;
}

void motor_direction(Direction d) {
  switch (d) {
    case Direction::Forward:
      digitalWrite(RIGHT_FORWARD, HIGH);
      digitalWrite(LEFT_FORWARD, HIGH);
      digitalWrite(RIGHT_BACKWARD, LOW);
      digitalWrite(LEFT_BACKWARD, LOW);
      break;
    case Direction::Backward:
      digitalWrite(RIGHT_FORWARD, LOW);
      digitalWrite(LEFT_FORWARD, LOW);
      digitalWrite(RIGHT_BACKWARD, HIGH);
      digitalWrite(LEFT_BACKWARD, HIGH);
      break;
  }
}
void motor_direction(Motor motor, Direction d) {
  int forward_pin;
  int backward_pin;
  switch (motor) {
    case Motor::Right:
      forward_pin = RIGHT_FORWARD;
      backward_pin = RIGHT_BACKWARD;
      break;
    case Motor::Left:
      forward_pin = LEFT_FORWARD;
      backward_pin = LEFT_BACKWARD;
      break;
  }

  switch (d) {
    case Direction::Forward:
      digitalWrite(forward_pin, HIGH);
      digitalWrite(backward_pin, LOW);
      break;
    case Direction::Backward:
      digitalWrite(forward_pin, LOW);
      digitalWrite(backward_pin, HIGH);
      break;
  }
}

// INFO: value of speed should between 0 and 255
void motor_speed(int speed) {
  motor_speed(Motor::Right, speed);
  motor_speed(Motor::Left, speed);
}
// INFO: value of speed should between 0 and 255
void motor_speed(Motor motor, int speed) {
  switch (motor) {
    case Motor::Right:
      analogWrite(RIGHT_ENABLE, speed);
      break;
    case Motor::Left:
      analogWrite(LEFT_ENABLE, speed);
      break;
  }
}

bool sensor_is_black(Sensor sensor) {
  int pin;
  switch (sensor) {
    case Sensor::Right:
      pin = RIGHT_SENSOR;
      break;
    case Sensor::Middle:
      pin = MIDDLE_SENSOR;
      break;
    case Sensor::Left:
      pin = LEFT_SENSOR;
      break;
  }
  return digitalRead(pin) == HIGH;
}

void init_pin_mode() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RIGHT_ENABLE, OUTPUT);
  pinMode(RIGHT_FORWARD, OUTPUT);
  pinMode(RIGHT_BACKWARD, OUTPUT);
  pinMode(LEFT_FORWARD, OUTPUT);
  pinMode(LEFT_BACKWARD, OUTPUT);
  pinMode(LEFT_ENABLE, OUTPUT);
  pinMode(MIDDLE_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(SONAR_TRIGGER, OUTPUT);
  pinMode(SONAR_ECHO, INPUT);
}
