// Chương trình này được viết để giao tiếp với BMS sử dụng IC bq78pl116 với 3 cell.
#include<Wire.h>
#define ADDRESS 0x0B
//Các hàm dùng trong chế độ SBData
void unlockSBD(void);
void writeSBD(byte, int);
int readSBD(byte);
void clearFuse(void);
void commit(void);
void relearnInitialize(void);
void enDisPowerPump(void);
void currentOffsetCalibration(void);
void currentGainCalibration(void);
void temperatureCalibration(void);
int readParameter(int);
void readVoltageSafetyParameters(void);
void readCurrentSafetyParameters(void);
void readTemperatureSafetyParameters(void);
void readSafetyParameters(void);
void readSafetyRegister(void);
void readFETStatus(bool);
void readSafetyStatus(bool);
void readPermanentDisableStatus(bool);
void readChargeStatus(bool);
void readCellStatus(bool);
void readDynamicParameters(void);
void writeSafetyParameter(void);
void readSBData(void);
void writeSBData(void);
//Các hàm dùng trong chế độ DFP
void DFPModeEnable(void);
void sendRequestDataFlashWrite(void);
void Request(unsigned int*, bool);
int Read(void);
void Write(int);
unsigned int Status(void);
void readConfigurationRegister(void);
int readDFP(unsigned int []);
void writeDFP(unsigned int [], int);
void writeConfigurationRegister(void);
void enDisSafetyRules(void);
void toggleWiredbit(void);
bool readWiredbit(void);
void exitDFP(void);
// Các hàm khác
byte CRCCalculation(byte*, byte);
void printMainMenu(void);
void printWriteSafetyParameterMenu(void);
void printWriteRegisterMenu(void);
void printEnDisSafeyRulesMenu(void);
void printEnDisPowerPumpMenu(void);
void printReadSafetyRegister(void);
void debugPEC(byte[], byte);
void printWiredbitStatus(void);
void printReadSafetyRegisterMenu(void);
int readSerial(void);
int readWord(byte);
void writeWord(byte, int);
void writeMySafetyParameters(void);
//Pack unlock
const unsigned int unlockCmd PROGMEM = 0x1A91;
//Cell overvoltage
const unsigned int COVThresold PROGMEM = 0x2400;
const unsigned int COVTime PROGMEM = 0x2300;
const unsigned int COVRecovery PROGMEM = 0x2401;
//Pack overvoltage
const unsigned int POVThresold PROGMEM = 0x2406;
const unsigned int POVTime PROGMEM = 0x2302;
const unsigned int POVRecovery PROGMEM =0x2407;
//Cell under voltage
const unsigned int CUVThresold PROGMEM = 0x2404;
const unsigned int CUVTime PROGMEM = 0x2301;
const unsigned int CUVRecovery PROGMEM = 0x2405;
//Pack undervoltage
const unsigned int PUVThresold PROGMEM = 0x2408;
const unsigned int PUVTime PROGMEM = 0x2303;
const unsigned int PUVRecovery PROGMEM = 0x2409;
//Cell extrem undervoltage
const unsigned int EUVThresold PROGMEM = 0x2416;
const unsigned int EUVTime PROGMEM = 0x230F;
const unsigned int EUVRecovery PROGMEM = 0x2417;
//OC charge tier 1
const unsigned int OCCT1Thresold PROGMEM = 0x240A;
const unsigned int OCCT1Time PROGMEM = 0x2305;
const unsigned int OCCT1Recovery PROGMEM = 0x2304;
//OC discharge tier 1
const unsigned int OCDCT1Thresold PROGMEM = 0x240B;
const unsigned int OCDCT1Time PROGMEM = 0x2307;
const unsigned int OCDCT1Recovery PROGMEM = 0x2306;
//Over current max attemps
const unsigned int OCMAttemp PROGMEM = 0x240F;
//Hardware short circuit
const unsigned int HSCThresold PROGMEM = 0x2414;
const unsigned int HSCTime PROGMEM = 0x2415;
const unsigned int HSCRecovery PROGMEM = 0x230E;
//Hardware short circuit max attemps
const unsigned int HSCMAttemp PROGMEM = 0x240E;
//Over temperature charge
const unsigned int OTCThresold PROGMEM = 0x2418;
const unsigned int OTCTime PROGMEM = 0x2310;
const unsigned int OTCRecovery PROGMEM = 0x2419;
//Charge suspend temperature high
const unsigned int CSHTemp PROGMEM = 0x2437;
const unsigned int CSHTRecovery PROGMEM = 0x2438;
//Charge inhibit temperature high
const unsigned int CIHTemp PROGMEM = 0x2430;
const unsigned int CIHTRecovery PROGMEM = 0x2431;
//Over temperature discharge
const unsigned int OTDThresold PROGMEM = 0x241A;
const unsigned int OTDTime PROGMEM = 0x2311;
const unsigned int OTDRecovery = 0x241B;
//Charge suspend temperature low
const unsigned int CSLTemp PROGMEM = 0x2435;
const unsigned int CSLTRecovery PROGMEM = 0x2436;
//Charge inhibit temperature low
const unsigned int CILTemp PROGMEM = 0x242E;
const unsigned int CILTRecovery PROGMEM = 0x242F;
//Discharge under temperature
const unsigned int DUTThresold PROGMEM = 0x2472;
const unsigned int DUTTime PROGMEM = 0x2326;
const unsigned int DUTRecovery PROGMEM = 0x2473;
//Dynamic parameters
//Lưu ý: 0x3C, 0x3D, 0x3E, 0x4B, 0x4C và 0x4D được map sang SBData bằng phần mềm bqWizard
const unsigned int V1 PROGMEM = 0x3C; // Điện áp cell 1
const unsigned int V2 PROGMEM = 0x3D; // Điện áp cell 2
const unsigned int V3 PROGMEM = 0x3E; // Điện áp cell 3
const unsigned int T1 PROGMEM = 0x4B; // Nhiệt độ cell 1
const unsigned int T2 PROGMEM = 0x4C; // Nhiệt độ cell 2
const unsigned int T3 PROGMEM = 0x4D; // Nhiệt độ cell 3
const unsigned int I PROGMEM = 0x0A; // Dòng qua pin
const unsigned int V PROGMEM = 0x09; // Điện áp pin
//Configuration registers
//Lưu ý: 
//Phần tử 0: file
//Phần tử 1: chunk
//Phần tử 2: Số chunk
const unsigned int hardwareConfiguration [3] = {0x09, 0x0048, 0x0001};
const unsigned int algorithmEnable [3] = {0x09, 0x0049, 0x0001};
const unsigned int systemControl [3] = {0x09, 0x0076, 0x0001};
//
const unsigned int requestDataFlashWrite [3] = {0x1B, 0x0000, 0x0002};
//Safety status register
//Lưu ý: Các thanh ghi này được map sang SBData bằng phần mềm bqWizard
const unsigned int FETStaus PROGMEM = 0x3F;
const unsigned int Alert0 PROGMEM = 0x40;
const unsigned int Status0 PROGMEM = 0x41;
const unsigned int Alert1 PROGMEM = 0x42;
const unsigned int Status1 PROGMEM = 0x43;
const unsigned int permanentAlert PROGMEM = 0x44;
const unsigned int permanentStatus PROGMEM = 0x45;
const unsigned int chargeAlert PROGMEM = 0x46;
const unsigned int chargeStatus PROGMEM = 0x47;
const unsigned int cell1Status PROGMEM = 0x48;
const unsigned int cell2Status PROGMEM = 0x49;
const unsigned int cell3Status PROGMEM = 0x4A;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  digitalWrite(SDA, LOW);
  digitalWrite(SCL, LOW);
  // Giới hạn tốc độ I2C ở 100KHz
  Wire.setClock(100000);
  printMainMenu();
}

void loop()
{
  static unsigned long lastMillis = 0;
  static byte mainMenuSel;
  while(Serial.available() == 0);
  mainMenuSel = readSerial();
  switch (mainMenuSel)
  {
    case 1:
      unlockSBD();
      printMainMenu();
      break;
    case 2:
      while(readSerial() == 0) // Chờ nhấn bất kỳ phím nào để thoát
      {
        if(millis() - lastMillis >= 1000) // Cập nhật một lần mỗi giây
        {
          lastMillis = millis();
          readDynamicParameters();
        }
      }
      printMainMenu();
      break;
    case 3:
      readSafetyParameters();
      printMainMenu();
      break;
    case 4:
      readSafetyRegister();
      printMainMenu();
      break;
    case 5:
      readSBData();
      printMainMenu();
      break;
    case 6:
      writeSBData();
      printMainMenu();
      break;
    case 7:
      writeSafetyParameter();
      printMainMenu();
      break;
    case 8:
      readConfigurationRegister();
      printMainMenu();
      break;
    case 9:
      writeConfigurationRegister();
      printMainMenu();
      break;
    case 10:
      toggleWiredbit();
      printMainMenu();
      break;
    case 11:
      enDisSafetyRules();
      printMainMenu();
      break;
    case 12:
      clearFuse();
      printMainMenu();
      break;
    case 13:
      relearnInitialize();
      printMainMenu();
      break;
    case 14:
      enDisPowerPump();
      printMainMenu();
      break;
    case 15:
      currentOffsetCalibration();
      printMainMenu();
      break;
    case 16:
      currentGainCalibration();
      printMainMenu();
      break;
    case 17:
      temperatureCalibration();
      printMainMenu();
      break;
    // case 18:
    //   writeMySafetyParameters();
    //   printMainMenu();
    default:
      Serial.println(F("Invalid"));
      printMainMenu();
  }
}

//Các hàm dùng trong chế độ SBData
void unlockSBD()
{
  writeSBD(0x80, unlockCmd);
}

void writeSBD(byte command, int value)
{
	writeWord(command, value);
}

int readSBD(byte command)
{
	return readWord(command);
}

void clearFuse()
{
  writeSBD(0x80, 0x200B);
}

void commit()
{
  writeSBD(0x80, 0x2003);
}

void relearnInitialize()
{
  writeSBD(0x80, 0x2007);
}

void enDisPowerPump()
{
  byte menuSel = 0;
  printEnDisPowerPumpMenu();
  while(Serial.available() == 0);
  menuSel = readSerial();
  switch (menuSel)
  {
    case 1:
      DFPModeEnable();
      writeDFP(systemControl, 0x0000);
      exitDFP();
      break;
    case 2:
      DFPModeEnable();
      writeDFP(systemControl, 0x8000);
      exitDFP();
      break;
    default:
      Serial.println(F("Invalid"));
  }
}

void currentOffsetCalibration(void)
{
  unlockSBD();
  writeSBD(0x80, 0x2000);
  Serial.println(F("Calibrating"));
  while(readSBD(0x80) != 0x0000) // Chờ IC đang trong quá trình hiệu chỉnh
  {
    delay(3);
  }
  commit();
}

void currentGainCalibration()
{
  int current = 0;
  Serial.println(F("Enter current (-32000mA to 32000mA):"));
  while(Serial.available() == 0);
  current = readSerial();
  if(current < -32000 || current > 32000)
  {
    Serial.println(F("Current out of range"));
    return;
  }
  unlockSBD();
  writeSBD(0x81, current);
  writeSBD(0x80, 0x2001);
  Serial.println(F("Calibrating"));
  while(readSBD(0x80) != 0x0000) // Chờ IC đang trong quá trình hiệu chỉnh
  {
    delay(3);
  }
  commit();
}

void temperatureCalibration()
{
  int temperature = 0;
  Serial.println(F("Enter temperature (290K to 303K):"));
  while(Serial.available() == 0);
  temperature = readSerial() * 10;
  if(temperature < 2900 || temperature > 3030)
  {
    Serial.println(F("Temperature out of range"));
    return;
  }
  unlockSBD();
  writeSBD(0x81, temperature);
  writeSBD(0x80, 0x2002);
  Serial.println(F("Calibrating"));
  while(readSBD(0X80) != 0x0000) // Chờ IC đang trong quá trình hiệu chỉnh
  {
    delay(3);
  }
  commit();
}

int readParameter(int index)
{
  writeSBD(0x80, index);
  return readSBD(0x81);
}

void readVoltageSafetyParameters()
{
  struct safetyParameters
  {
    char name [5][4] = {{"COV"}, {"POV"}, {"CUV"}, {"PUV"}, {"EUV"}};
    int thresold [5] = {readParameter(COVThresold), readParameter(POVThresold), readParameter(CUVThresold), readParameter(PUVThresold), readParameter(EUVThresold)};
    int time[5] = {readParameter(COVTime), readParameter(POVTime), readParameter(CUVTime), readParameter(PUVTime), readParameter(EUVTime)};
    int recovery[5] = {readParameter(COVRecovery), readParameter(POVRecovery), readParameter(CUVRecovery), readParameter(PUVRecovery), readParameter(EUVRecovery)};
  }voltageSafetyParameters;
  Serial.println(F("*Voltage Safety Parameters:"));
  Serial.println(F("Name\tThres\tTime\tRecov"));
  for(byte i = 0; i < 5; i++)  
  {
    Serial.print(voltageSafetyParameters.name[i]);
    Serial.print('\t');
    if(voltageSafetyParameters.thresold[i] == 0)
        Serial.print('\t');
    else
    {
      Serial.print(voltageSafetyParameters.thresold[i]);
      Serial.print('\t');
    }
    if(voltageSafetyParameters.time[i] == 0)
      Serial.print('\t');
    else
    {
      Serial.print(voltageSafetyParameters.time[i]);
      Serial.print('\t');
    }
    if(voltageSafetyParameters.recovery[i] == 0)    
      Serial.println();
    else  
      Serial.println(voltageSafetyParameters.recovery[i]);
  }
}

void readCurrentSafetyParameters()
{
  struct safetyParameters
  {
    char name [5][6] = {{"OCC"}, {"OCD"}, {"OCAt"}, {"HSC"}, {"HSCAt"}};
    int thresold [5] = {readParameter(OCCT1Thresold), readParameter(OCDCT1Thresold), readParameter(OCMAttemp), readParameter(HSCThresold), readParameter(HSCMAttemp)};
    int time [5] = {readParameter(OCCT1Time), readParameter(OCDCT1Time), 0, readParameter(HSCTime), 0};
    int recovery [5] = {readParameter(OCCT1Recovery), readParameter(OCDCT1Recovery), 0, readParameter(HSCRecovery), 0};
  }currentSafetyParameters;
  Serial.println(F("*Current Safety Parameters:"));
  Serial.println(F("Name\tThres\tTime\tRecov"));
  for(byte i = 0; i < 5; i++)  
  {
      Serial.print(currentSafetyParameters.name[i]);
      Serial.print('\t');
      if(currentSafetyParameters.thresold[i] == 0)
        Serial.print('\t');
      else
      {
        Serial.print(currentSafetyParameters.thresold[i]);
        Serial.print('\t');
      }
      if(currentSafetyParameters.time[i] == 0)
        Serial.print('\t');
      else
      {
        Serial.print(currentSafetyParameters.time[i]);
        Serial.print('\t');
      }
      if(currentSafetyParameters.recovery[i] == 0)    
        Serial.println();
      else  
        Serial.println(currentSafetyParameters.recovery[i]);
  }
}

void readTemperatureSafetyParameters()
{
  struct safetyParameters
  {
    const char name [7][5] = {{"OTC"}, {"OTD"}, {"CIHT"}, {"CILT"}, {"CSHT"}, {"CSLT"}, {"DUT"}};
    int thresold [7] = {readParameter(OTCThresold), readParameter(OTDThresold), readParameter(CIHTemp), readParameter(CILTemp), readParameter(CSHTemp), readParameter(CSLTemp), readParameter(DUTThresold)};
    int time [7] = {readParameter(OTCTime), readParameter(OTCTime), 0, 0, 0, 0, readParameter(DUTTime)};
    int recovery [7] = {readParameter(OTCRecovery), readParameter(OTDRecovery), readParameter(CIHTRecovery), readParameter(CILTRecovery), readParameter(CSHTRecovery), readParameter(CSLTRecovery), readParameter(DUTRecovery)};  
  }temperatureSafetyParameters;
  Serial.println(F("*Temperature Safety Parameters:"));
  Serial.println(F("Name\tThres\tTime\tRecov"));
  for(byte i = 0; i < 7; i++)  
  {
    Serial.print(temperatureSafetyParameters.name[i]);
    Serial.print('\t');
    if(temperatureSafetyParameters.thresold[i] == 0)
        Serial.print('\t');
    else
    {
      Serial.print(temperatureSafetyParameters.thresold[i]);
      Serial.print('\t');
    }
    if(temperatureSafetyParameters.time[i] == 0)
      Serial.print('\t');
    else
    {
      Serial.print(temperatureSafetyParameters.time[i]);
      Serial.print('\t');
    }
    if(temperatureSafetyParameters.recovery[i] == 0)    
      Serial.println();
    else  
      Serial.println(temperatureSafetyParameters.recovery[i]);
  }
}

void readSafetyParameters()
{
  Serial.println(F("|**********Safety parameters*********|"));
  unlockSBD();
  readVoltageSafetyParameters();
  readCurrentSafetyParameters();
  readTemperatureSafetyParameters();
}

void readSafetyRegister()
{
  byte menuSel = 0;
  printReadSafetyRegisterMenu();
  while(Serial.available() == 0);
  menuSel = readSerial();
  switch (menuSel)
  {
    case 1: 
      readFETStatus(false);
      break;
    case 2:
      readSafetyStatus(false);
      break;
    case 3:
      readPermanentDisableStatus(false);
      break;
    case 4:
      readChargeStatus(false);
      break;
    case 5:
      readCellStatus(false);
      break;
    default:
      Serial.println(F("Invalid"));
  }
}

void readFETStatus(bool printSetbit)
{
  unsigned int FETStatusRegister = readSBD(FETStaus);
	const char FETStatusBitName [3][5] = {{"DFET"},
							                          {"CFET"},
							                          {"PFET"}};
  Serial.println(F("*FET Status:"));
  if(printSetbit == false || (printSetbit == true && (FETStatusRegister & 0x0007) != 0x0007))
	{
    Serial.println(F("Name\tStatus"));
    for(unsigned int i = 0, n = 1; i < 3; i++, n<<=1)
    {
      if(printSetbit == false || (printSetbit == true && ((FETStatusRegister & n) == 0)))
      {
        Serial.print(FETStatusBitName[i]);
        Serial.print('\t');
        (FETStatusRegister & n) != 0? Serial.println(true) : Serial.println(false);
      }
    }
  }
}

void readSafetyStatus(bool printSetbit)
{
  unsigned int safetyAlertRegister = readSBD(Alert0);
  unsigned int safetyStatusRegister = readSBD(Status0);
  unsigned int safetyAlert1Register = readSBD(Alert1);
  unsigned int safetyStatus1Register = readSBD(Status1);
  const char safetyStatusAlertBitname [17][5] = {{"HOCD"}, {"HOCC"}, {"HSC"}, {"EUV"}, {"HWDG"}, {"COV"}, {"BOT"}, {"CUV"},
                                                  {"POV"}, {"PUV"}, {"OCC2"}, {"OCD2"}, {"OCC"}, {"OCD"}, {"OTC"}, {"OTD"},
                                                  {"DUT"}};
  Serial.println(F("*Safety status:"));
  if(printSetbit == false || (printSetbit == true && (((safetyAlertRegister & 0xFFFF) != 0 || (safetyStatusRegister & 0xFFFF) != 0 || (safetyAlert1Register & 0x0002) != 0) || (safetyStatus1Register & 0x0002) != 0)))
  {
    Serial.println(F("Name\tAlert\tStatus"));
    for(unsigned int i = 0, n = 1; i < 16; i++, n<<=1)
    {
      if(printSetbit == false || (printSetbit == true && ((safetyAlertRegister & n) != 0 || (safetyStatusRegister & n) != 0)))
      {
        Serial.print(safetyStatusAlertBitname[i]);
        Serial.print('\t');
        (safetyAlertRegister & n) != 0 ? Serial.print(true) : Serial.print(false);
        Serial.print('\t');
        (safetyStatusRegister & n) != 0 ? Serial.println(true) : Serial.println(false);
      }
    }
    if(printSetbit == false || (printSetbit == true && ((safetyAlert1Register & 0x0002) != 0 || (safetyStatus1Register & 0x0002) != 0)))
    {
      Serial.print(safetyStatusAlertBitname[16]);
      Serial.print('\t');
      (safetyAlert1Register & 0x0002) != 0 ? Serial.print(true) : Serial.print(false);
      Serial.print('\t');
      (safetyStatus1Register & 0x0002) != 0 ? Serial.println(true) : Serial.println(false);
    }
  }
}

void readPermanentDisableStatus(bool printSetbit)
{
  unsigned int permanentDisableAlertRegister = readSBD(permanentAlert);
  unsigned int permanentDisableStatusRegister = readSBD(permanentStatus);
  const char permanentDisableAlertStatusBitName [15][6] = {{"SOV"}, {"SOTC"}, {"SOTD"}, {"CIM"}, {"CFETF"}, {"DFETF"}, {"OTS"}, {"CMF"},
                                                            {"VLAN"}, {"SOCC"}, {"SOCD"}, {"ROR"}, {"IGRR"}, {"IGR"}, {"FBF"}};
  Serial.println(F("*Permanent disable status:"));
  if(printSetbit == false || (printSetbit == true && ((permanentDisableAlertRegister & 0xFFFE) != 0 || (permanentDisableStatusRegister & 0xFFFE) != 0)))
  {
    Serial.println(F("Name\tAlert\tStatus"));
    for(unsigned int i = 0, n = 2; i < 15; i++, n<<=1)
    {
      if(printSetbit == false || (printSetbit == true && ((permanentDisableAlertRegister & n) != 0 || (permanentDisableStatusRegister & n) != 0)))
      {
        Serial.print(permanentDisableAlertStatusBitName[i]);
        Serial.print('\t');
        (permanentDisableAlertRegister & n) != 0 ? Serial.print(true) : Serial.print(false);
        Serial.print('\t');
        (permanentDisableStatusRegister & n) != 0 ? Serial.println(true) : Serial.println(false);
      }
    }
  }
}

void readChargeStatus(bool printSetbit)
{
  unsigned int chargeAlertRegister = readSBD(chargeAlert);
  unsigned int chargeStatusRegister = readSBD(chargeStatus);
  const char chargeAlertStatusBitname [10][5] = {{"CSLT"}, {"CSHT"}, {"CILT"}, {"CIHT"}, {"PV"}, {"PT"}, {"CT"}, {"PTO"},
                                                  {"CC"}, {"DC"}};
  Serial.println(F("*Charge status:"));
  if(printSetbit == false || (printSetbit == true && ((chargeAlertRegister & 0xFFC0) != 0 || (chargeStatusRegister & 0xFFC0) != 0)))
  {
    Serial.println(F("Name\tAlert\tStatus"));
    for(unsigned int i = 0, n = 64; i < 10; i++, n<<=1)
    {
      if(printSetbit == false || (printSetbit == true && ((chargeAlertRegister & n) != 0 || (chargeStatusRegister & n) != 0)))
      {
        Serial.print(chargeAlertStatusBitname[i]);
        Serial.print('\t');
        (chargeAlertRegister & n) != 0 ? Serial.print(true) : Serial.print(false);
        Serial.print('\t');
        (chargeStatusRegister & n) != 0 ? Serial.println(true) : Serial.println(false);
      }
    }
  }
}

void readCellStatus(bool printSetbit)
{
  unsigned int cell1StatusRegister = readSBD(cell1Status);
  unsigned int cell2StatusRegister = readSBD(cell2Status);
  unsigned int cell3StatusRegister = readSBD(cell3Status);
  const char cellStatusBitName [6][6] = {{"North"}, {"South"}, {"OV"}, {"UV"}, {"OT"}, {"UT"}};
  Serial.println(F("*Cell status:"));
  if(printSetbit == false || (printSetbit == true && ((cell1StatusRegister & 0x003F) != 0 || (cell2StatusRegister & 0x003F) != 0 || (cell3StatusRegister & 0x003F) != 0)))
  {
    Serial.println(F("Name\tCell1\tCell2\tCell3"));
    for(unsigned int i = 0, n = 1; i < 6; i++, n<<=1)
    {
      if(printSetbit == false || (printSetbit == true && ((cell1StatusRegister & n) != 0 || (cell2StatusRegister & n) != 0 || (cell3StatusRegister & n) != 0)))
      {
        Serial.print(cellStatusBitName[i]);
        Serial.print('\t');
        (cell1StatusRegister & n) != 0 ? Serial.print(true) : Serial.print(false);
        Serial.print('\t');
        (cell2StatusRegister & n) != 0 ? Serial.print(true) : Serial.print(false);
        Serial.print('\t');
        (cell3StatusRegister & n) != 0 ? Serial.println(true) : Serial.println(false);
      }
    }
  }
}

void readDynamicParameters()
{
  int DynamicParameters [8] = {readSBD(V1), readSBD(V2), readSBD(V3),
                                (readSBD(T1) / 10) - 272, (readSBD(T2) / 10) -272 , (readSBD(T3) / 10) - 272,
                                readSBD(V), readSBD(I)};
  Serial.println(F("|*********Dynamic parameters*********|"));
  for(byte i = 0; i < sizeof(DynamicParameters) / 2; i++)
  {
    if(i == 0)
      Serial.println(F("V1\tV2\t\V3"));
    else if(i == 3)
      Serial.println(F("T1\tT2\t\T3"));
    else if(i == 6)
      Serial.println(F("V\tI"));
    Serial.print(DynamicParameters[i]);
    if(i == 2 || i == 5 || i == 7)
      Serial.println();
    else
      Serial.print('\t');
  }
  readFETStatus(true);
  readSafetyStatus(true);
  readPermanentDisableStatus(true);
  readChargeStatus(true);
  readCellStatus(true);
}

void readSBData()
{
  byte SBDCommand = 0;
  Serial.println(F("Enter command:"));
  while(Serial.available() == 0);
  SBDCommand = readSerial();
  if((SBDCommand >= 0x00 && SBDCommand <= 0x03) || (SBDCommand >= 0x08 && SBDCommand <= 0x1C) || (SBDCommand >= 0x3C && SBDCommand <= 0x81))
    Serial.println(readWord(SBDCommand));
  else
    Serial.println(F("Invalid command"));
}

void writeSBData()
{
  byte SBDCommand = 0;
  int SBDValue = 0;
  Serial.println(F("Enter command:"));
  while(Serial.available() == 0);
  SBDCommand = readSerial();
  if((SBDCommand >= 0x00 && SBDCommand <= 0x03) || (SBDCommand >= 0x3C && SBDCommand <= 0x81))
  {
    Serial.println(F("Enter value:"));
    while(Serial.available() == 0);
    SBDValue = readSerial();
  }
  else
  {
    Serial.println(F("Invalid command"));
    return;
  }
  writeSBD(SBDCommand, SBDValue);
}

void writeSafetyParameter()
{
  unsigned int index = 0;
  int SafetyParameterValue = 0;
  byte menuSel = 0;
  printWriteSafetyParameterMenu();
  while(Serial.available() == 0);
  menuSel = readSerial();
  switch(menuSel)
  {
    case 1:
      Serial.println(F("Enter index:"));
      while(Serial.available() == 0);
      index = readSerial();
      if(index < 0x2100 || index > 0x2127)
      {
        Serial.println(F("Invalid index"));
        return;
      }
      break;
    case 2:
      Serial.println(F("Enter index:"));
      while(Serial.available() == 0);
      index = readSerial();
      if(index < 0x2200 || index > 0x2277)
      {
        Serial.println(F("Invalid index"));
        return;
      }
      break;
    default:
      Serial.println(F("Invalid"));
      return;
  }
  Serial.println(F("Enter value:"));
  while(Serial.available() == 0);
  SafetyParameterValue = readSerial();
  writeSBD(0x81, SafetyParameterValue);
  writeSBD(0x80, index);
}

// Các hàm dùng trong chế độ DFP
void DFPModeEnable()
{
  writeSBD(0x00, 0x44BB); // Ghi 0x44BB vào 0x00 để mở khoá
  writeSBD(0x00, 0xC01A); // Ghi 0xC01A vào 0x00 để đọc thanh ghi System flags
  if((readSBD(0x00) & 0x0200) != 0) // Kiểm tra bit 9 thanh ghi System flags phải được xoá
    Serial.println(F("Unlock pack error"));
  writeSBD(0x00, 0x55AA); // Ghi 0x55AA vào 0x00 để vào chế độ DFP
  if(readSBD(0x00) != 0x55AA) // Nếu đọc 0x00 mà trả về 0x55AA thì vào thành công
  {
    Serial.println(F("DFP mode enable error"));
  }
}

void sendRequestDataFlashWrite()
{
  Request(requestDataFlashWrite, true);
  if(Status() != 0x0000)
  {
    Serial.println(F("Request data flash write error"));
    return;
  }
  byte wrtData [8] = {ADDRESS << 1, 0x3E, 0x04, 0x0D, 0x00, 0x00, 0x00, CRCCalculation(wrtData, sizeof(wrtData) - 1)};
  Wire.beginTransmission(ADDRESS);
  for(byte i = 1; i < sizeof(wrtData) - 1; i++)
    Wire.write(wrtData[i]);
  Wire.endTransmission(true);
  if(Status() != 0x0000)
    Serial.println(F("Request data flash write error"));
}

void Request(unsigned int *Register, bool RW) 
{
  byte wrtData [10] = {ADDRESS << 1, 0x3C, 0x06, RW == true? 0x01 : 0x00, Register[0], Register[1] & 0x00FF, (Register[1] & 0xFF00) >> 8, Register[2] & 0x00FF, (Register[2] & 0xFF00) >> 8, CRCCalculation(wrtData, sizeof(wrtData) - 1)};
  Wire.beginTransmission(ADDRESS);
  for(byte i = 1; i <= sizeof(wrtData) - 1; i++)
    Wire.write(wrtData[i]);
  Wire.endTransmission(true);
}

int Read()
{
  byte rcvData [7] = {ADDRESS << 1, 0x3F, (ADDRESS << 1) | 1, 0x00, 0x00, 0x00, 0x00};
  int rcvData_int = 0;
  byte i = 3;
  Wire.beginTransmission(ADDRESS);
  Wire.write(0x3F);
  Wire.endTransmission(false);

  Wire.requestFrom(ADDRESS, 4, true);
  while(Wire.available() > 0)
  {
    rcvData[i] = Wire.read();
    i++;
  }
  if(CRCCalculation(rcvData, sizeof(rcvData)) == 0x00)
  {
    rcvData_int = rcvData[5];
    rcvData_int = rcvData_int << 8;
    rcvData_int = rcvData_int | rcvData[4];
    return rcvData_int;
  }
  else
    return 0xFFFF;
}

void Write(int value)
{
  byte wrtData [6] = {ADDRESS << 1, 0x3E, 0x02, value & 0x00FF, (value & 0xFF00) >> 8, CRCCalculation(wrtData, sizeof(wrtData) - 1)};
  Wire.beginTransmission(ADDRESS);
  for(byte i = 1; i <= sizeof(wrtData) - 1; i++)
    Wire.write(wrtData[i]);
  Wire.endTransmission(true);
}

unsigned int Status()
{
  return readWord(0x3D);
}

int readWord(byte command)
{
  byte rcvData [6] = {ADDRESS << 1, command, (ADDRESS << 1) | 1, 0x00, 0x00, 0x00};
	int rcvData_int = 0;
  byte i = 3;
  Wire.beginTransmission(ADDRESS);
  Wire.write(command);
  Wire.endTransmission(false);

	Wire.requestFrom(ADDRESS, 3, true);
	while(Wire.available() > 0)
	{
	  rcvData[i] = Wire.read();
    i++;
	}
	if(CRCCalculation(rcvData, sizeof(rcvData)) == 0x00)
  {
    rcvData_int = rcvData[4];
    rcvData_int = rcvData_int << 8;
    rcvData_int = rcvData_int | rcvData[3];
    return rcvData_int;
  }
  else
    return 0xFFFF;
}

void writeWord(byte command, int value)
{
  byte wrtData [5] = {ADDRESS << 1, command, value & 0x00FF, (value & 0xFF00) >> 8, CRCCalculation(wrtData, sizeof(wrtData) - 1)};
  Wire.beginTransmission(ADDRESS);
  for(byte i = 1; i <= sizeof(wrtData) - 1; i++)
    Wire.write(wrtData[i]);
  Wire.endTransmission(true);
}

void readConfigurationRegister()
{
  Serial.println(F("|********Configuration register******|"));
  DFPModeEnable();
  Serial.print(F("Algorithm enable register: "));
  Serial.println((unsigned int)readDFP(algorithmEnable), HEX);
  Serial.print(F("Hardware configuration register: "));
  Serial.println((unsigned int)readDFP(hardwareConfiguration), HEX);
  Serial.print(F("System control register: "));
  Serial.println((unsigned int)readDFP(systemControl), HEX);
  exitDFP();
}

int readDFP(unsigned int Register[])
{
  Request(Register, false);
  if(Status() != 0x0000)
  {
    Serial.println(F("Read request error"));
    return 0xFFFF;
  }
  return Read();  
}

void writeDFP(unsigned int Register[], int value)
{
  sendRequestDataFlashWrite();
  Request(Register, true);
  if(Status() != 0x0000)
  {
    Serial.println(F("Request write error"));
    return;
  }
  Write(value);
  if(Status() != 0x0000)
    Serial.println(F("Write error"));
}

void writeConfigurationRegister()
{
  byte menuSel = 0;
  unsigned int Register [3];
  unsigned int registerValue = 0;
  printWriteRegisterMenu();
  while(Serial.available() == 0);
  menuSel = readSerial();
  switch(menuSel)
  {
    case 1:
      memcpy(Register, hardwareConfiguration, sizeof(hardwareConfiguration));
      break;
    case 2:
      memcpy(Register, algorithmEnable, sizeof(algorithmEnable));
      break;
    case 3:
      memcpy(Register, systemControl, sizeof(systemControl));
      break;
    default:
      Serial.println(F("Invalid register"));
      return;
  }
  Serial.println(F("Enter value:"));
  while(Serial.available() == 0);
  registerValue = readSerial();
  DFPModeEnable();
  writeDFP(Register, registerValue);
  exitDFP();
}

void enDisSafetyRules()
{
  byte menuSel = 0;
  unsigned int AERValue = 0;
  unsigned int AERNewValue = 0;
  printEnDisSafeyRulesMenu();
  while(Serial.available() == 0);
  menuSel = readSerial();
  switch(menuSel)
  {
    case 1:
      DFPModeEnable();
      AERValue = readDFP(algorithmEnable);
      AERNewValue = AERValue & 0x7FFF;
      writeDFP(algorithmEnable, AERNewValue);
      exitDFP();
      break;
    case 2:
      DFPModeEnable();
      AERValue = readDFP(algorithmEnable);
      AERNewValue = AERValue | 0x8000;
      writeDFP(algorithmEnable, AERNewValue);
      exitDFP();
      break;
    default:
      Serial.println(F("Invalid"));
  }
}

void toggleWiredbit()
{
  DFPModeEnable();
  unsigned int AERValue = readDFP(algorithmEnable);
  unsigned int AERNewValue = 0;
  if((AERValue & 0x0008) == 0)
  {
    AERNewValue = AERValue | 0x0008;
    writeDFP(algorithmEnable, AERNewValue);
  }
  else
  {
    AERNewValue = AERValue & 0xFFF7;
    writeDFP(algorithmEnable, AERNewValue);
  }
  exitDFP();
}

bool readWiredbit()
{
	DFPModeEnable();
	if((readDFP(algorithmEnable) & 0x0008) != 0)
	{
		exitDFP();
		return true;
  }
	else
  {
		exitDFP();
		return false;
	}
}

void exitDFP()
{
  writeSBD(0x00, 0x0000);
}
// Other functions
byte CRCCalculation(byte *data, byte numByte)
{
  const byte generator = 0x07;
  byte crc = 0;
  for(byte i = 0; i < numByte; i++)
  {
    crc ^= data[i];
    for (byte j = 0; j < 8; j++)
    {
      if ((crc & 0x80) != 0)
        crc = (crc << 1) ^ generator;
      else
        crc <<= 1;
    }
  }
  return crc;
}

void printMainMenu()
{
  Serial.println(F("|**************bq78pl116*************|"));
  printWiredbitStatus();
  Serial.println(F("1. Unlock"));
  Serial.println(F("2. Read dynamic parameters"));
  Serial.println(F("3. Read safety parameters"));
  Serial.println(F("4. Read safety and status register"));
  Serial.println(F("5. Read SBData"));
  Serial.println(F("6. Write SBData"));
  Serial.println(F("7. Write saftey parameter"));
  Serial.println(F("8. Read configuration register"));
  Serial.println(F("9. Write configuration register"));
  Serial.println(F("10. Toggle Wired bit"));
  Serial.println(F("11. En/Dis Safety rules"));
  Serial.println(F("12. Clear Fuse"));
  Serial.println(F("13. Relearn/Initialize"));
  Serial.println(F("14. En/Dis PowerPump"));
  Serial.println(F("15. Current zero offset calibration"));
  Serial.println(F("16. Current gain calibration"));
  Serial.println(F("17. Temperature offset calibration"));
}

void printWriteSafetyParameterMenu()
{
  Serial.println(F("|*******Write saftey parameter*******|"));
  Serial.println(F("1. Write timer"));
  Serial.println(F("2. Write thresold"));
}

void printWriteRegisterMenu()
{
  Serial.println(F("|****Write configuration register****|"));
  Serial.println(F("1. Hardware configuration register"));
  Serial.println(F("2. Algorithm enable register"));
  Serial.println(F("3. System control register"));
}

void printEnDisSafeyRulesMenu()
{
  
  Serial.println(F("|********En/Dis safety rules*********|"));
  Serial.println(F("1. Enable"));
  Serial.println(F("2. Disable"));
}

void printEnDisPowerPumpMenu()
{
  Serial.println(F("|**********En/Dis Powerpump**********|"));
  Serial.println(F("1. Enable"));
  Serial.println(F("2. Disable"));
}

void printWiredbitStatus()
{
  if(readWiredbit() == true)
    Serial.println(F("Wired bit: Set"));
  else
    Serial.println(F("Wired bit: Clear"));
}

void printReadSafetyRegisterMenu()
{
  Serial.println(F("|*********Read safety status*********|"));
  Serial.println(F("1. FET status"));
  Serial.println(F("2. Safety status"));
  Serial.println(F("3. Permanent disable status"));
  Serial.println(F("4. Charge status"));
  Serial.println(F("5. Cell status"));
}

void debugPEC(byte dataToDebug[], byte dataToDebugSize)
{
  Serial.print(F("PEC: "));
  for(int i = 0; i < dataToDebugSize; i++)
  {
    Serial.print(dataToDebug[i], HEX);
  }
  Serial.println();
}

int readSerial()
{
  int serialValue = 0;
  while(Serial.available() > 0)
  {
    serialValue = Serial.parseInt();
  }
  return serialValue;
}

// void writeMySafetyParameters()
// {
//   const int safetyParameters [43][2] PROGMEM = {{0x2200, 4250},//COV Thresold
//                                 {0x2100, 1}, // COV Time
//                                 {0x2201, 4100}, //COV Recovery
//                                 {0x2206, 12750}, //POV Thresold
//                                 {0x2102, 1}, //POV Time
//                                 {0x2207, 12300}, //POV Recovery
//                                 {0x2204, 3000}, //CUV Thresold
//                                 {0x2101, 2}, //CUV Time
//                                 {0x2205, 3100}, //CUV Recovery
//                                 {0x2208, 9000}, //PUV Thresold
//                                 {0x2103, 2}, //PUV Time
//                                 {0x2209, 9300}, //PUV Recovery
//                                 {0x2216, 2700}, //EUV Thresold
//                                 {0x210F, 5}, //EUV Time
//                                 {0x2217, 2900}, //EUV Recovery
//                                 {0x220A, 1200}, //OCC Thresold
//                                 {0x2105, 1}, //OCC Time
//                                 {0x2104, 6}, //OCC Recovery
//                                 {0x220B, -7000}, //OCD Thresold
//                                 {0x2107, 1}, //OCD Time
//                                 {0x2106, 6}, //OCD Recovery
//                                 {0x220F, 0}, //OC max attemps
//                                 {0x2214, 47}, //HSC Thresold 10A
//                                 {0x2215, 1}, //HSC Time
//                                 {0x210E, 10}, //HSC Recovery
//                                 {0x220E, 0}, //HSC max attemps
//                                 {0x2218, 3121}, //OTC Thresold
//                                 {0x2110, 2}, //OTC Time
//                                 {0x2219, 3021}, //OTC Recovery
//                                 {0x2237, 3221}, //CSTH Thresold
//                                 {0x2238, 3121}, //CSTH Recovery  
//                                 {0x2230, 3121}, //CITH Thresold
//                                 {0x2231, 3021}, //CITH Recovery
//                                 {0x221A, 3321}, //OTD Thresold
//                                 {0x2111, 2}, //OTD Time
//                                 {0x221B, 3221}, //OTD Recovery
//                                 {0x2235, 2721}, //CSTL Thresold
//                                 {0x2236, 2771}, //CSTL Recovery
//                                 {0x222E, 2721}, //CITL Thresold
//                                 {0x222F, 2771}, //CITL Recovery
//                                 {0x2272, 2521}, //DUT Thresold
//                                 {0x2126, 2}, //DUT Time
//                                 {0x2273, 2671}, //DUT Recovery
//                               };
//   unlockSMBus();                                                          
//   for(byte i = 0; i < 43; i++)
//   {
//     writeSBD(0x81, safetyParameters[i][1]);
//     writeSBD(0x80, safetyParameters[i][0]);
//   }
// }
