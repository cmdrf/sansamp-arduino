#include <U8g2lib.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

#define PIN_POT_PREAMP A1
#define PIN_POT_BUZZ A7
#define PIN_POT_PUNCH A5
#define PIN_POT_CRUNCH A4
#define PIN_POT_DRIVE A0
#define PIN_POT_LOW A6
#define PIN_POT_HIGH A2
#define PIN_POT_LEVEL A3

#define PIN_CTRL_DATA 3
#define PIN_CTRL_RST 4
#define PIN_CTRL_CLK 5

#define PIN_BYPASS 2

#define PIN_UP 9
#define PIN_DOWN 8
#define PIN_SAVE 7

class Button
{
public:
  Button(int pin) : mPin(pin) {}

  bool triggered()
  {
    bool newState = (digitalRead(mPin) == LOW);
    unsigned long m = millis();
    if(!mState && newState)
    {
      // press
      mState = true;
      mPressedMillis = m;
      return true;
    }
    else if(mState && newState && m - mPressedMillis > 800)
    {
      // start repeat
      mRepeating = true;
      mPressedMillis = m;
      return true;
    }
    else if(mState && !newState && m - mPressedMillis > 50)
    {
      // release
      mState = false;
      mRepeating = false;
      return false;
    }
    else if(mState && mRepeating && m - mPressedMillis > 100)
    {
      // repeat
      mPressedMillis = m;
      return true;
    }
    return false;
  }

private:
  int mPin = 0;
  bool mState = false;
  bool mRepeating = false;
  unsigned long mPressedMillis = 0;
};

Button upButton(PIN_UP), downButton(PIN_DOWN), saveButton(PIN_SAVE);

uint8_t currentInput[8] = {0};
uint8_t currentOutput[8] = {0};

const char preampString[] PROGMEM = "Preamp";
const char buzzString[] PROGMEM = "Buzz";
const char punchString[] PROGMEM = "Punch";
const char crunchString[] PROGMEM = "Crunch";
const char driveString[] PROGMEM = "Drive";
const char lowString[] PROGMEM = "Low";
const char highString[] PROGMEM = "High";
const char levelString[] PROGMEM = "Level";

const char* const potStrings[] PROGMEM = {
  preampString,
  buzzString,
  punchString,
  crunchString,
  driveString,
  lowString,
  highString, 
  levelString
};

const char preset00Name[] PROGMEM = "Bypass";
const char preset01Name[] PROGMEM = "Plexi";
const char preset02Name[] PROGMEM = "Vintage";
const char preset03Name[] PROGMEM = "Schenker";
const char preset04Name[] PROGMEM = "JMP-1";
const char preset05Name[] PROGMEM = "High Gain";
const char preset06Name[] PROGMEM = "Bluesbreaker";
const char preset07Name[] PROGMEM = "Hendrix";
const char preset08Name[] PROGMEM = "Van Halen 1";
const char preset09Name[] PROGMEM = "Classic Clean";
const char preset10Name[] PROGMEM = "Stock";
const char preset11Name[] PROGMEM = "B.B. King";
const char preset12Name[] PROGMEM = "Stevie Ray";
const char preset13Name[] PROGMEM = "Funk";
const char preset14Name[] PROGMEM = "Champ";
const char preset15Name[] PROGMEM = "Twin";
const char preset16Name[] PROGMEM = "Super Bright";
const char preset17Name[] PROGMEM = "Classic Rock";
const char preset18Name[] PROGMEM = "Super Clean";
const char preset19Name[] PROGMEM = "Jazz";
const char preset20Name[] PROGMEM = "Mark 1";
const char preset21Name[] PROGMEM = "Metallica";
const char preset22Name[] PROGMEM = "Santana";
const char preset23Name[] PROGMEM = "Clean";
const char preset24Name[] PROGMEM = "Rectifier";
const char preset25Name[] PROGMEM = "Triaxis";
const char preset26Name[] PROGMEM = "Lead";
const char preset27Name[] PROGMEM = "Rhythm";
const char preset28Name[] PROGMEM = "Too Much Gain";
const char preset29Name[] PROGMEM = "Mutant";
const char preset30Name[] PROGMEM = "SVT";
const char preset31Name[] PROGMEM = "Bassman";
const char preset32Name[] PROGMEM = "Jazz";
const char preset33Name[] PROGMEM = "Metal";
const char preset34Name[] PROGMEM = "Slap";
const char preset35Name[] PROGMEM = "King's X";
const char preset36Name[] PROGMEM = "Yes";
const char preset37Name[] PROGMEM = "Lead";
const char preset38Name[] PROGMEM = "Doug Wimbish";
const char preset39Name[] PROGMEM = "Crimson";
const char preset40Name[] PROGMEM = "Fuzz Face";
const char preset41Name[] PROGMEM = "Big Muff Pi";
const char preset42Name[] PROGMEM = "Pignose";
const char preset43Name[] PROGMEM = "MXR+";
const char preset44Name[] PROGMEM = "Tele Simulator";
const char preset45Name[] PROGMEM = "American Woman";
const char preset46Name[] PROGMEM = "Pantera";
const char preset47Name[] PROGMEM = "Hiwatt";
const char preset48Name[] PROGMEM = "AC30 Queen";
const char preset49Name[] PROGMEM = "Speaker Simulator";

const char* const presetNames[50] PROGMEM = {
  preset00Name, preset01Name, preset02Name, preset03Name, preset04Name,
  preset05Name, preset06Name, preset07Name, preset08Name, preset09Name,
  preset10Name, preset11Name, preset12Name, preset13Name, preset14Name,
  preset15Name, preset16Name, preset17Name, preset18Name, preset19Name,
  preset20Name, preset21Name, preset22Name, preset23Name, preset24Name,
  preset25Name, preset26Name, preset27Name, preset28Name, preset29Name,
  preset30Name, preset31Name, preset32Name, preset33Name, preset34Name,
  preset35Name, preset36Name, preset37Name, preset38Name, preset39Name,
  preset40Name, preset41Name, preset42Name, preset43Name, preset44Name,
  preset45Name, preset46Name, preset47Name, preset48Name, preset49Name
};

char buffer[30];    // make sure this is large enough for the largest string it must hold

// http://www.popeye-x.com/tech/SansAmp_PSA-1_presets.htm
const PROGMEM uint8_t factoryPresets[50 * 8] = {
  0, 0, 0, 0, 0, 0, 0, 0,
// Marshall knobs
//  --------------------PREAMP--BUZZ-PUNCH-CRUNCH-DRIVE--LOW--HIGH-LEVEL
/*01 Plexi*/               255,   63,  243,  181,  243,  170,  155,   80,
/*02 Vintage*/             255,  255,  255,  255,   83,  255,  246,   27,
/*03 Schenker*/            255,    0,  255,  115,  227,  179,  147,   78,
/*04 JMP-1*/               251,   81,   80,  108,  243,  119,  186,   60,
/*05 High Gain*/           252,   63,  243,  181,  243,  170,  155,   80,
/*06 Bluesbreaker*/        186,  194,  193,  227,  140,   54,  155,   75,
/*07 Hendrix     */        247,  255,  156,  240,  231,   92,  152,   72,
/*08 Van Halen 1 */        255,   37,  148,  236,  227,  198,  173,   68,
/*09 Classic Clean*/       123,  129,  126,  250,    0,  255,  186,  228,

// Fender knobs

//--------------------PREAMP--BUZZ-PUNCH-CRUNCH-DRIVE--LOW--HIGH-LEVEL
/*10 Stock       */      118,  215,    0,  237,   75,  144,  190,  240,
/*11 B.B. King   */      176,  127,   52,  238,  240,   16,   67,  157,
/*12 Stevie Ray  */      239,  197,   36,  245,  239,   80,  150,   91,
/*13 Funk        */      126,  197,    4,  245,   85,  192,  255,  188,
/*14 Champ       */      255,   71,  206,  116,  224,    6,  122,  149,
/*15 Twin        */      211,  200,    0,  239,  181,   68,  196,  105,
/*16 Super Bright*/      120,  241,    0,  255,  181,  119,  231,  128,
/*17 Classic Rock*/      128,  217,   19,  255,  197,  117,  191,  127,
/*18 Super Clean */       47,  159,    0,  255,    0,  151,  120,  255,
/*19 Jazz        */      128,  129,  127,    0,  129,  129,  128,  199,

// Mesa/Boogie knobs

//--------------------PREAMP--BUZZ-PUNCH-CRUNCH-DRIVE--LOW--HIGH-LEVEL
/*20 Mark 1       */     251,  255,  190,  127,  244,  107,  177,   51,
/*21 Metallica    */     255,  255,  255,  127,  222,  160,  198,   53,
/*22 Santana      */     255,  228,  184,  216,  197,   30,  135,   68,
/*23 Clean        */     137,  140,   16,  231,    9,  156,  203,  208,
/*24 Rectifier    */     255,  255,  234,  214,  214,  242,  230,   35,
/*25 Triaxis      */     247,  255,  235,  231,  189,  186,  154,   61,
/*26 Lead         */     250,  255,  243,  236,  213,   20,  176,   89,
/*27 Rhythm       */     238,  255,  128,  235,   78,  202,  202,   33,
/*28 Too Much Gain*/     255,  255,  243,  243,  212,   82,  156,   69,
/*29 Mutant       */     255,  255,  123,  255,  181,  129,  181,   55,

// Bass knobs

//--------------------PREAMP--BUZZ-PUNCH-CRUNCH-DRIVE--LOW--HIGH-LEVEL
/*30 SVT              */  70,  140,    0,  254,  201,  228,  193,  129,
/*31 Bassman          */  47,  255,   22,  255,   79,   17,   85,  237,
/*32 Jazz             */ 162,  237,   22,   25,   79,   92,  199,  182,
/*33 Metal            */ 237,  236,    0,  249,  186,  188,  210,   33,
/*34 Slap             */  14,  244,    0,  249,  112,  188,  236,  159,
/*35 King's X         */ 255,  255,    0,  249,   16,  238,  236,   26,
/*36 Yes              */ 238,  235,   10,  249,   24,  215,  243,   26,
/*37 Lead             */ 128,  128,  221,  248,  128,  239,  227,   48,
/*38 Doug Wimbish     */ 250,  239,  255,  240,  125,  221,  210,   34,
/*39 Crimson          */ 255,  225,    0,  241,  243,  201,  234,   19,

// Miscellaneous knobs-

//--------------------PREAMP--BUZZ-PUNCH-CRUNCH-DRIVE--LOW--HIGH-LEVEL
/*40 Fuzz Face        */ 255,  255,   25,    0,  244,   79,  255,   35,
/*41 Big Muff Pi      */ 239,  255,  240,    0,  208,   65,  251,   37,
/*42 Pignose          */ 255,  130,  255,  209,  178,    0,    0,  175,
/*43 MXR+             */ 255,  144,  129,  152,  212,    0,   71,  122,
/*44 Tele Simulator   */ 255,   13,    0,  242,  244,   96,  145,   81,
/*45 American Woman   */ 255,  252,  236,  235,  224,  117,  147,   88,
/*46 Pantera          */ 255,  255,  255,  255,   84,  255,  246,   27,
/*47 Hiwatt           */ 255,   25,  177,  255,  143,   68,  166,   70,
/*48 AC30 Queen       */ 249,   41,  254,  158,  217,  157,  203,   60,
/*49 Speaker Simulator*/  49,  129,  128,  128,    0,  128,  239,  237
};

const char knobs00Name[] PROGMEM = "Marshall";
const char knobs01Name[] PROGMEM = "Fender";
const char knobs02Name[] PROGMEM = "Mesa";
const char knobs03Name[] PROGMEM = "Bass";
const char knobs04Name[] PROGMEM = "Misc";
const char knobs05Name[] PROGMEM = "Custom";

const char* const knobsNames[6] PROGMEM = {
  knobs00Name, knobs01Name, knobs02Name, knobs03Name, knobs04Name, knobs05Name
};

U8G2_SSD1306_128X64_NONAME_1_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 10, /* dc=*/ 6);
int currentProgram = 0;
bool gameActive = false;
bool saving = false;

void readPots(uint8_t pots[8])
{
  pots[0] = analogRead(PIN_POT_PREAMP) >> 2;
  pots[1] = analogRead(PIN_POT_BUZZ) >> 2;
  pots[2] = analogRead(PIN_POT_PUNCH) >> 2;
  pots[3] = analogRead(PIN_POT_CRUNCH) >> 2;
  pots[4] = analogRead(PIN_POT_DRIVE) >> 2;
  pots[5] = analogRead(PIN_POT_LOW) >> 2;
  pots[6] = analogRead(PIN_POT_HIGH) >> 2;
  pots[7] = analogRead(PIN_POT_LEVEL) >> 2;
}

void ctrlWrite(int value0, int value1)
{
  digitalWrite(PIN_CTRL_DATA, LOW); // Stack
  digitalWrite(PIN_CTRL_CLK, HIGH);
  digitalWrite(PIN_CTRL_CLK, LOW);

  for(int i = 7; i >= 0; --i)
  {
    digitalWrite(PIN_CTRL_DATA, value0 & (1 << i) ? HIGH : LOW);
    digitalWrite(PIN_CTRL_CLK, HIGH);
    digitalWrite(PIN_CTRL_CLK, LOW);
  }

  for(int i = 7; i >= 0; --i)
  {
    digitalWrite(PIN_CTRL_DATA, value1 & (1 << i) ? HIGH : LOW);
    digitalWrite(PIN_CTRL_CLK, HIGH);
    digitalWrite(PIN_CTRL_CLK, LOW);
  }
}

void writePots(uint8_t pots[8])
{
  digitalWrite(PIN_CTRL_RST, HIGH);
  ctrlWrite(pots[5], pots[7]);
  ctrlWrite(pots[3], pots[6]);
  ctrlWrite(pots[2], pots[4]);
  ctrlWrite(pots[0], pots[1]);
  digitalWrite(PIN_CTRL_RST, LOW);
}

void printSmallCaps(const char* str)
{
  for(const char* it = str; *it != 0; ++it)
  {
    if(isUpperCase(*it))
    {
      u8g2.setFont(u8g2_font_crox5h_tr);
      u8g2.print(*it);
    }
    else
    {
      u8g2.setFont(u8g2_font_crox3hb_tr);
      u8g2.print(char(toupper(*it)));
    }
  }
}

// the setup routine runs once when you press reset:
void setup()
{
  Serial.begin(31250);

  pinMode(PIN_CTRL_DATA, OUTPUT);
  pinMode(PIN_CTRL_RST, OUTPUT);
  pinMode(PIN_CTRL_CLK, OUTPUT);
  pinMode(PIN_BYPASS, OUTPUT);
  
  digitalWrite(PIN_BYPASS, HIGH);

  pinMode(PIN_UP, INPUT);
  pinMode(PIN_DOWN, INPUT);
  pinMode(PIN_SAVE, INPUT);

  u8g2.begin();
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 20);
    printSmallCaps("SansAmp");
    u8g2.setCursor(0, 40);
    printSmallCaps("PSA-1");
  } while ( u8g2.nextPage() );

  readPots(currentInput);
  writePots(currentInput);
}

bool fuzzyCompare(uint8_t v0, uint8_t v1)
{
  if(v0 == v1)
    return true;
  if(v0 > 0 && v0 - 1 == v1)
    return true;
  if(v1 > 0 && v0 == v1 - 1)
    return true;
  if(v0 < 255 && v0 + 1 == v1)
    return true;
  if(v1 < 255 && v0 == v1 + 1)
    return true;
  return false;
}

float potValueToAngle(uint8_t value)
{
  return value * 0.01847 - 3.9269;
}

void checkPots()
{
  uint8_t pots[8];
  readPots(pots);

  for(int i = 0; i < 8; ++i)
  {
    if(!fuzzyCompare(pots[i], currentInput[i]))
    {
      strcpy_P(buffer, (char*)pgm_read_word(&(potStrings[i]))); // Necessary casts and dereferencing, just copy.
      const int potCenterX = 100;
      const int potCenterY = 36;
      const float angle = potValueToAngle(pots[i]); 
      const int lineEndX = cos(angle) * 25 + potCenterX;
      const int lineEndY = sin(angle) * 25 + potCenterY;
      const uint8_t presetValue = pgm_read_word_near(factoryPresets + currentProgram * 8 + i);
      const float presetAngle = potValueToAngle(presetValue);
      const int presetX = cos(presetAngle) * 27 + potCenterX;
      const int presetY = sin(presetAngle) * 27 + potCenterY;
      u8g2.firstPage();
      do {
        u8g2.setCursor(0, 20);
        printSmallCaps(buffer);
        u8g2.setCursor(0, 60);
        u8g2.setFont(u8g2_font_crox5hb_tn);
        u8g2.print(pots[i]);
        u8g2.drawCircle(potCenterX, potCenterY, 25);
        u8g2.drawLine(102, 36, lineEndX, lineEndY);
        u8g2.drawDisc(presetX, presetY, 2);
      } while ( u8g2.nextPage() );

      currentInput[i] = pots[i];
      currentOutput[i] = pots[i];
      writePots(currentOutput);
      break;
    }
  }
}

void drawProgramSelector(int program)
{
  int knobSet = constrain(program / 10, 0, 5);
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_crox5hb_tn);
    u8g2.setCursor(0, 25);
    u8g2.print(program);
    if(program != 0)
    {
      strcpy_P(buffer, (char*)pgm_read_word(&(knobsNames[knobSet]))); // Necessary casts and dereferencing, just copy.
      u8g2.setCursor(40, 25);
      printSmallCaps(buffer);
    }
    if(program < 50)
    {
      u8g2.setCursor(0, 60);
      strcpy_P(buffer, (char*)pgm_read_word(&(presetNames[program]))); // Necessary casts and dereferencing, just copy.
      printSmallCaps(buffer);
    }
    else
    {
      for(int i = 0; i < 8; ++i)
      {
        const int centerX = 8 + i * 16;
        const int centerY = 56;
        const float angle = potValueToAngle(currentOutput[i]); 
        const int lineEndX = cos(angle) * 7 + centerX;
        const int lineEndY = sin(angle) * 7 + centerY;
        u8g2.drawDisc(centerX, centerY, 7);
        u8g2.setDrawColor(0);
        u8g2.drawLine(centerX + 1, centerY + 1, lineEndX + 1, lineEndY + 1);
        u8g2.drawLine(centerX + 1, centerY, lineEndX + 1, lineEndY);
        u8g2.drawLine(centerX, centerY + 1, lineEndX, lineEndY + 1);
        u8g2.drawLine(centerX, centerY, lineEndX, lineEndY);
        u8g2.drawLine(centerX, centerY, lineEndX, lineEndY);
        u8g2.setDrawColor(1);
      }
    }
  } while ( u8g2.nextPage() );
}

void checkProgram(bool upTriggered, bool downTriggered, bool saveTriggered)
{
  int program = currentProgram;
  
  if(upTriggered)
    program++;
  if(downTriggered)
    program--;
  
  if(program != currentProgram)
  {
    program = constrain(program, 0, 99);
    currentProgram = program;

    if(program < 50)
    {
      for(int i = 0; i < 8 ; ++i)
        currentOutput[i] = pgm_read_word_near(factoryPresets + program * 8 + i);
    }
    else
    {
      for(int i = 0; i < 8 ; ++i)
        currentOutput[i] = EEPROM.read((program - 50) * 8 + i);
    }
    writePots(currentOutput);

    digitalWrite(PIN_BYPASS, (program == 0) ? LOW : HIGH);

    drawProgramSelector(program);
  }

  if(saveTriggered)
  {
    saving = true;
    if(currentProgram < 50)
      currentProgram += 50;
  }
}

void save(bool upTriggered, bool downTriggered, bool saveTriggered)
{
  if(upTriggered)
    currentProgram++;
  if(downTriggered)
    currentProgram--;
  currentProgram = constrain(currentProgram, 50, 99);

  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_crox5hb_tn);
    u8g2.setCursor(0, 25);
    u8g2.print(currentProgram);
    u8g2.setCursor(40, 25);
    printSmallCaps(saveTriggered ? "Saved!" : "Save");
  } while ( u8g2.nextPage() );

  if(saveTriggered)
  {
    for(int i = 0; i < 8 ; ++i)
      EEPROM.write((currentProgram - 50) * 8 + i, currentOutput[i]);
    delay(1000);
    saving = false;
    drawProgramSelector(currentProgram);
  }
}

void checkCheat(bool upTriggered, bool downTriggered, bool saveTriggered)
{
  static int cheatState = 0;
  static bool lastLevelLeft = false;
  static bool lastLevelRight = false;
  bool levelLeft = (currentInput[7] < 50);
  bool levelRight = (currentInput[7] > 200);
  bool leftTriggered = false;
  bool rightTriggered = false;
  if(levelLeft && !lastLevelLeft)
    leftTriggered = true;
  if(levelRight && !lastLevelRight)
    rightTriggered = true;
  lastLevelLeft = levelLeft;
  lastLevelRight = levelRight;
  if(upTriggered || downTriggered || saveTriggered || leftTriggered || rightTriggered)
  {
    if(cheatState == 0 && upTriggered)
      cheatState = 1;
    else if(cheatState == 1 && upTriggered)
      cheatState = 2;
    else if(cheatState == 2 && downTriggered)
      cheatState = 3;
    else if(cheatState == 3 && downTriggered)
      cheatState = 4;
    else if(cheatState == 4 && leftTriggered)
      cheatState = 5;
    else if(cheatState == 5 && leftTriggered)
      cheatState = 6;
    else if(cheatState == 6 && rightTriggered)
      cheatState = 7;
    else if(cheatState == 7 && rightTriggered)
      cheatState = 8;
    else if(cheatState == 8 && saveTriggered)
      cheatState = 9;
    else if(cheatState == 9 && saveTriggered)
      cheatState = 10;
    else
      cheatState = 0;

    if(cheatState == 10)
      gameActive = true;
  }
}

void sansamp()
{
  bool upTriggered = upButton.triggered();
  bool downTriggered = downButton.triggered();
  bool saveTriggered = saveButton.triggered();
  if(saving)
    save(upTriggered, downTriggered, saveTriggered);
  else
  {
    checkPots();
    checkProgram(upTriggered, downTriggered, saveTriggered);
  }
  checkCheat(upTriggered, downTriggered, saveTriggered);
}

void game()
{
  uint8_t pots[8];
  readPots(pots);

  int leftPaddlePos = constrain(pots[0] / 4 - 8, 0, 48);
  int rightPaddlePos = constrain(pots[7] / 4 - 8, 0, 48);
  static int ballPosX = 64;
  static int ballPosY = 32;
  static int ballVelX = 1;
  static int ballVelY = 1;
  static int player1Score = 0;
  static int player2Score = 0;

  ballPosX += ballVelX;
  ballPosY += ballVelY;

  if(ballPosY < 1)
    ballVelY = 1;
  else if(ballPosY > 62)
    ballVelY = -1;

  if(ballPosX == 4 && ballPosY > leftPaddlePos - 3 && ballPosY < leftPaddlePos + 19)
      ballVelX = 1;
  else if(ballPosX == 123 && ballPosY > rightPaddlePos - 3 && ballPosY < rightPaddlePos + 19)
      ballVelX = -1;
  else if(ballPosX > 148)
  {
    ballPosX = 64;
    ballPosY = 32;
    ballVelX = -1;
    ballVelY = -1;
    player1Score++;
  }
  else if(ballPosX < -20)
  {
    ballPosX = 64;
    ballPosY = 32;
    ballVelX = 1;
    ballVelY = 1;
    player2Score++;
  }
     
  u8g2.firstPage();
  u8g2.setFont(u8g2_font_profont12_tn);
  do {
    u8g2.drawBox(0, leftPaddlePos, 3, 16);  
    u8g2.drawBox(125, rightPaddlePos, 3, 16);
    u8g2.drawDisc(ballPosX, ballPosY, 2);

    u8g2.setCursor(40, 10);
    u8g2.print(player1Score);
    u8g2.setCursor(80, 10);
    u8g2.print(player2Score);
  } while ( u8g2.nextPage() );
}

// the loop routine runs over and over again forever:
void loop()
{
  if(gameActive)
    game();
  else
    sansamp();
}

void serialEvent()
{
  int incomingByte = Serial.read();
  Serial.write(incomingByte);
}
