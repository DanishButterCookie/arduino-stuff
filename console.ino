#include <LiquidCrystal.h>

// Initialize the LCD with the numbers of the interface pins
LiquidCrystal lcd(9, 10, 11, 12, 13, 2);
unsigned long previousMillis = 0;
int interval = 500; // time between tones in milliseconds
bool playTone = false;
const int sfxPin = 5;

const int h1 = 8; // Define the heart pins
const int h2 = 7;
const int h3 = 6;

// Button pin
const int buttonPin = 4; // Button connected to D4
int lastButtonState = LOW; // Previous state of the button
// Joystick pins
const int joystickXPin = A0; // X-axis connected to A0
const int joystickYPin = A7; // Y-axis connected to A7

// Threshold for joystick sensitivity
const int threshold = 800;

// Number of rows and columns
const int rows = 16;
const int cols = 16;

// 2D array to hold the grid values
byte grid[rows][cols];

// Player position in the grid
int playerX = 1;
int playerY = 14;
int playerHp = 3;

// Enemy position in the grid
int enemy1X = 3; // Starting position for the enemy
int enemy1Y = 3;
bool enemy1 = true;
byte enemy1_sprite = byte(5);
 
// Enemy movement timing
unsigned long lastEnemyMoveTime = 0; // Last time the enemy moved
const unsigned long enemyMoveInterval = 1300; // Interval for enemy movement (in milliseconds)

// Camera settings
int cameraYOffset = 1; // Initial camera offset
bool cameraShiftedDown = false;

// Idle animation variables
unsigned long lastMoveTime = 0;
unsigned long idleInterval = 1000; // Interval for idle animation in milliseconds
bool isIdle = false;
bool idleState = false; // Toggle between idle1 and idle2
// Add this new variable at the top
unsigned long lastIdleToggleTime = 0; // Keeps track of the last idle animation toggle time
const unsigned long idleToggleInterval = 500; // Interval for switching idle states (in milliseconds)


void setup() {
    Serial.begin(9600);  // Start serial communication at 9600 baud
    lcd.begin(16, 2);
    pinMode(buttonPin, INPUT);
    
    // heart pins
    pinMode(h1, OUTPUT);
    pinMode(h2, OUTPUT);
    pinMode(h3, OUTPUT);
    pinMode(sfxPin, OUTPUT); // Set the buzzer pin as output
    createCustomCharacters();
    initializeGrid();
    playSoundEffect1();
    delay(200); // Wait 2 seconds
    playSoundEffect2();
    delay(200); // Wait 2 seconds
    
    playerX = 1; // Starting column for the player
    grid[playerY][playerX] = 0; // Set the player sprite (idle1)
    
    updateDisplay();
}

void loop() {
    int currentButtonState = digitalRead(buttonPin); // Read the current state of the button
    // Read joystick values
    int joystickX = analogRead(joystickXPin);
    int joystickY = analogRead(joystickYPin);
    unsigned long currentMillis = millis();

    // Check if it's time to play the next tone
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        playTone = !playTone;

        if (playTone) {
            tone(sfxPin, 1000, 100); // Play a tone of 1000Hz for 100ms
        } else {
            noTone(sfxPin); // Stop the tone
        }
    }
    
    // Check joystick input for player movement
    if (joystickX > threshold+200) {
        movePlayerDown(); // Move player down if joystick pushed down
        lastMoveTime = millis();
        isIdle = false;
        delay(100); // Delay to see the effect of movement
    } else if (joystickX < (1023 - threshold+200)) {
        movePlayerUp(); // Move player up if joystick pushed up
        lastMoveTime = millis();
        isIdle = false;
        delay(100); // Delay to see the effect of movement
    }

    if (joystickY > threshold) {
        movePlayerLeft(); // Move player left if joystick pushed left
        lastMoveTime = millis();
        isIdle = false;
        delay(100); // Delay to see the effect of movement
    } else if (joystickY < (1023 - threshold)) {
        movePlayerRight(); // Move player right if joystick pushed right
        lastMoveTime = millis();
        isIdle = false;
        delay(100); // Delay to see the effect of movement
    }

    // Handle idle animation if no movement is detected
    if (millis() - lastMoveTime > idleInterval && !isIdle) {
        isIdle = true; // Player is now idle
    }

    if (isIdle) {
        playIdleAnimation();
    }

    // Move the enemy toward the player at a slower rate
    if (millis() - lastEnemyMoveTime >= enemyMoveInterval && enemy1 == true) {
        moveEnemy();
        lastEnemyMoveTime = millis(); // Update the last enemy move time
    }
    if (isPlayerOnEnemy()) {
        enemy1X = 0;
        enemy1Y = 0;
        enemy1_sprite = byte(5);
        enemy1 = false;
        playerHp -= 1;
        Serial.println(playerHp);
    }
    int buttonState = digitalRead(buttonPin); // Read the state of the button

    // Check for a rising edge
    if (currentButtonState == HIGH && lastButtonState == LOW) {
        if (isPlayerAdjacentToEnemy()) {
          enemy1X = 0;
          enemy1Y = 0;
          enemy1_sprite = byte(5);
          enemy1 = false;
        }
    }
    lastButtonState = currentButtonState; // Store the current state for the next loop
    if (playerHp == 3) {
      digitalWrite(h1, HIGH);
      digitalWrite(h2, HIGH);
      digitalWrite(h3, HIGH);
    }
    else if (playerHp == 2) {
      digitalWrite(h1, LOW);
      digitalWrite(h2, HIGH);
      digitalWrite(h3, HIGH);
    }
    else if (playerHp == 1) {
      digitalWrite(h1, LOW);
      digitalWrite(h2, LOW);
      digitalWrite(h3, HIGH);
    }
    else {
      digitalWrite(h1, LOW);
      digitalWrite(h2, LOW);
      digitalWrite(h3, LOW);
    }
    delay(100); // Small delay for stability
}

void createCustomCharacters() {
    // Define custom characters here
    byte customChar1[8] = {B01110, B01110, B01110, B11111, B11111, B01110, B01010, B01010}; // idle1 (player)
    byte customChar2[8] = {B00000, B01110, B01110, B01110, B11111, B11111, B01110, B01010}; // idle2
    byte customChar3[8] = {B11111, B10101, B11011, B10101, B11011, B10101, B11011, B11111}; // wall
    byte customChar4[8] = {B00000, B00000, B00000, B01010, B10101, B10001, B01010, B00100}; // empty heart
    byte customChar5[8] = {B00000, B00000, B00000, B01010, B11111, B11111, B01110, B00100}; // full heart
    byte customChar6[8] = {B00000, B01010, B01010, B01110, B11111, B01110, B01110, B01010}; // enemy
    byte customChar7[8] = {B11111, B11100, B11000, B10000, B10000, B11000, B11100, B11111}; // door (enter from its right)
    byte customChar8[8] = {B11111, B00111, B00011, B00001, B00001, B00011, B00111, B11111}; // door (enter from its left)

    // Create custom characters
    lcd.createChar(0, customChar1);
    lcd.createChar(1, customChar2);
    lcd.createChar(2, customChar3);
    lcd.createChar(3, customChar4);
    lcd.createChar(4, customChar5);
    lcd.createChar(5, customChar6);
    lcd.createChar(6, customChar7);
    lcd.createChar(7, customChar8);
}

// Function to initialize the grid
void initializeGrid() {
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (y == 0 || y == rows - 1 || x == 0 || x == cols - 1) {
                grid[y][x] = 2; // Set walls around the edges (index 2)
            } else {
                grid[y][x] = 4; // Set all other cells to empty heart character (index 4)
            }
        }
    }

    // You can customize the grid layout here
    grid[3][5] = 2;
    grid[5][5] = 2;
    grid[4][6] = 2;
    grid[4][4] = 2;
    grid[14][0] = 6;
    grid[1][15] = 7;
}

// Function to update the display
void updateDisplay() {
    lcd.clear();
    // Print the current view of the grid on the LCD
    for (int y = 0; y < 2; y++) { // Display only 2 rows
        for (int x = 0; x < 16; x++) {
            // Ensure we don't go out of bounds when displaying the grid
            if (y + playerY - cameraYOffset < rows && x < cols) {
                lcd.setCursor(x, y); // Set cursor for the row
                // Check if the position is the player's current position
                if (y + playerY - cameraYOffset == playerY && x == playerX) {
                    lcd.write(byte(grid[playerY][playerX])); // Display the player sprite
                } else if (y + playerY - cameraYOffset == enemy1Y && x == enemy1X) {
                    lcd.write(enemy1_sprite); // Display the enemy sprite
                } else {
                    // Write a space for empty cells or the character for the grid
                    if (grid[y + playerY - cameraYOffset][x] == 4) {
                        lcd.print(" "); // Print space for empty heart
                    } else {
                        lcd.write(byte(grid[y + playerY - cameraYOffset][x])); // Display the character
                    }
                }
            }
        }
    }
}

// Function to move the enemy toward the player
void moveEnemy() {
    int deltaX = playerX - enemy1X;
    int deltaY = playerY - enemy1Y;

    // Determine the direction to move
    if (abs(deltaX) > abs(deltaY)) {
        if (deltaX > 0 && enemy1X < cols - 1 && grid[enemy1Y][enemy1X + 1] != 2) {
            enemy1X++; // Move right
        } else if (deltaX < 0 && enemy1X > 0 && grid[enemy1Y][enemy1X - 1] != 2) {
            enemy1X--; // Move left
        }
    } else {
        if (deltaY > 0 && enemy1Y < rows - 1 && grid[enemy1Y + 1][enemy1X] != 2) {
            enemy1Y++; // Move down
        } else if (deltaY < 0 && enemy1Y > 0 && grid[enemy1Y - 1][enemy1X] != 2) {
            enemy1Y--; // Move up
        }
    }

    // Update the display after moving the enemy
    updateDisplay();
}

// Player movement functions
void movePlayerDown() {
    if (playerY < rows - 1 && grid[playerY + 1][playerX] != 2) {
        grid[playerY][playerX] = 4; // Set previous position to empty heart
        playerY++;
        grid[playerY][playerX] = 0; // Set new position to player (idle1)
        updateDisplay();
    }
}

void movePlayerUp() {
    if (playerY > 0 && grid[playerY - 1][playerX] != 2) {
        grid[playerY][playerX] = 4; // Set previous position to empty heart
        playerY--;
        grid[playerY][playerX] = 0; // Set new position to player (idle1)
        updateDisplay();
    }
}

void movePlayerLeft() {
    if (playerX > 0 && grid[playerY][playerX - 1] != 2) {
        grid[playerY][playerX] = 4; // Set previous position to empty heart
        playerX--;
        grid[playerY][playerX] = 0; // Set new position to player (idle1)
        updateDisplay();
    }
}

void movePlayerRight() {
    if (playerX < cols - 1 && grid[playerY][playerX + 1] != 2) {
        grid[playerY][playerX] = 4; // Set previous position to empty heart
        playerX++;
        grid[playerY][playerX] = 0; // Set new position to player (idle1)
        updateDisplay();
    }
}

// Function to handle idle animation
void playIdleAnimation() {
    // Check if the specified interval has passed since the last toggle
    if (millis() - lastIdleToggleTime >= idleToggleInterval) {
        if (idleState) {
            grid[playerY][playerX] = 1; // Set player to idle2
        } else {
            grid[playerY][playerX] = 0; // Set player to idle1
        }
        idleState = !idleState; // Toggle the state
        lastIdleToggleTime = millis(); // Update the last toggle time
        updateDisplay(); // Update the display with the new state
    }
}

// Function to check if the player is adjacent to the enemy
bool isPlayerAdjacentToEnemy() {
    // Check if the player is directly above, below, left, or right of the enemy
    if ((playerX == enemy1X && abs(playerY - enemy1Y) == 1) ||  // Same column, row difference of 1
        (playerY == enemy1Y && abs(playerX - enemy1X) == 1)) {  // Same row, column difference of 1
        return true;
    }
    return false;
}
// Function to check if the player and enemy share the same coordinates
bool isPlayerOnEnemy() {
    return (playerX == enemy1X && playerY == enemy1Y);
}
void playSoundEffect1() {
    tone(sfxPin, 440, 500); // Play a 440 Hz sound for 500 ms
    delay(500);                // Wait for the sound to finish
    noTone(sfxPin);        // Stop the sound
}

void playSoundEffect2() {
    tone(sfxPin, 523, 500); // Play a 523 Hz sound for 500 ms
    delay(500);                // Wait for the sound to finish
    noTone(sfxPin);        // Stop the sound
}