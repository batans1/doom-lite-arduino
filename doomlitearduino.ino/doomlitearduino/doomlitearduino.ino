#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int buttonPin = 7;
const int buzzerPin = 6;

int lives = 3;
int score = 0;
bool gameOver = false;

// Музика 
const int musicNotes[] = {440, 494, 523, 392}; 
const int noteCount = 4;
int noteIndex = 0;
unsigned long lastNoteTime = 0;

// Спрайтове
byte skull[8]  = {B01110, B10101, B11111, B11011, B11111, B01110, B01010, B10001};
byte alien[8]  = {B00100, B01110, B10101, B11111, B11111, B01110, B01010, B10001};
byte zombie[8] = {B01110, B11111, B11011, B11111, B10101, B01110, B10101, B01010};
byte bomb[8]   = {B00100, B00100, B01110, B11111, B11111, B01110, B01110, B00100};

const int reactionTimes[] = {800, 600, 400, 0}; // За череп;извънземно;зомби;бомба

void setup() {
  lcd.begin(16, 2);
  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  lcd.createChar(0, skull);
  lcd.createChar(1, alien);
  lcd.createChar(2, zombie);
  lcd.createChar(3, bomb);

  showTitleScreen();
}

void loop() {
  if (gameOver) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GAME OVER");
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    lcd.print(score);
    deathSound();
    delay(3000);
    resetGame();
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enemy incoming...");
  int waitTime = random(1000, 3000);
  unsigned long waitStart = millis();

  // Watch for false start
  while (millis() - waitStart < waitTime) {
    if (digitalRead(buttonPin) == HIGH) {
      delay(50); while (digitalRead(buttonPin) == HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("False Start!");
      failSound();
      lives = 0;
      gameOver = true;
      return;
    }
  }

  int enemyType = random(0, 4);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ENEMY!");
  lcd.setCursor(7, 0);
  lcd.write(byte(enemyType));

  bool reacted = false;
  unsigned long startTime = millis();

  if (enemyType == 3) {
    while (millis() - startTime < 1000) {
      if (digitalRead(buttonPin) == HIGH) {
        delay(50); while (digitalRead(buttonPin) == HIGH);
        reacted = true;
        break;
      }
    }
    lcd.clear();
    if (reacted) {
      lives--;
      failSound();
      lcd.setCursor(0, 0);
      lcd.print("BOOM! Wrong!");
    } else {
      score++;
      successSound();
      lcd.setCursor(0, 0);
      lcd.print("Nice! You waited");
    }
  } else {
    int limit = reactionTimes[enemyType];
    while (millis() - startTime < limit) {
      if (digitalRead(buttonPin) == HIGH) {
        delay(50); while (digitalRead(buttonPin) == HIGH);
        reacted = true;
        break;
      }
    }

    lcd.clear();
    if (reacted) {
      score++;
      successSound();
      lcd.setCursor(0, 0);
      lcd.print("HIT!");
    } else {
      lives--;
      failSound();
      lcd.setCursor(0, 0);
      lcd.print("TOO SLOW!");
    }
  }

  lcd.setCursor(0, 1);
  lcd.print("Lives:");
  lcd.print(lives);
  lcd.print(" Score:");
  lcd.print(score);
  delay(1500);

  if (lives <= 0) {
    gameOver = true;
  }
}

void showTitleScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DOOM LITE");
  lcd.setCursor(0, 1);
  lcd.print("Press to start");

  noteIndex = 0;
  lastNoteTime = millis();

  // Loop за background mузиката
  while (digitalRead(buttonPin) == LOW) {
    unsigned long now = millis();
    if (now - lastNoteTime > 300) {
      tone(buzzerPin, musicNotes[noteIndex], 200);
      noteIndex = (noteIndex + 1) % noteCount;
      lastNoteTime = now;
    }
  }

  delay(50);
  while (digitalRead(buttonPin) == HIGH);
}

void resetGame() {
  lives = 3;
  score = 0;
  gameOver = false;
  showTitleScreen();
}

// Саунд ефекти
void successSound() {
  tone(buzzerPin, 1000, 150);
  delay(150);
}

void failSound() {
  tone(buzzerPin, 200, 300);
  delay(300);
}

void deathSound() {
  tone(buzzerPin, 400, 150);
  delay(200);
  tone(buzzerPin, 250, 250);
  delay(300);
}
