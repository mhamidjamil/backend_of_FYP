
void setup()
{
  Serial.begin(115200);
}
void loop()
{
  String tempStr;
  if (Serial.available() > 0)
  {
    // data_input = 1;
    while (Serial.available())
    {
      tempStr = Serial.readStringUntil('#');
    }
    if (tempStr.length() > 10)
      DataManager(tempStr);
    // Serial.println(tempStr);

    // R_temperature = Serial.parseFloat();
    // network->firestoreDataUpdate(temperature, 0);
  }
}
void DataManager(String tempstr)
{
  // perform operation on given string, and assign to different variables
  // b'25.18!Supine position!C:0.00,R:0.00,V:0.0000!0.00!'
  // 25.18 temperature here and will be stored in filterd_temperature variable
  // Supine position will be stored in position variable
  // C:0.00,R:0.00,V:0.0000 will be stored in filterd_Conductance,
  // filterd_Resistance, filterd_Conductance_voltage 0.00 will be stored in
  // filterd_snore_voltages ? data to be managed in struct b'25.18!Supine
  // position!C:0.00,R:0.00,V:0.0000!0.00!' int data_input=0;
  
  // Serial.println("Working on : " + tempstr);

  float filterd_temperatur = 0;

  String position = "";

  float filterd_Conductance = 0;
  float filterd_Resistance = 0.0;
  float filterd_Conductance_voltage = 0.0;

  float filterd_snore_voltages = 0.0;

  // ? data manager variables end
  int first_exclamation = tempstr.indexOf('!');
  int second_exclamation = tempstr.indexOf('!', first_exclamation + 1);
  int third_exclamation = tempstr.indexOf('!', second_exclamation + 1);
  int fourth_exclamation = tempstr.indexOf('!', third_exclamation + 1);

  // Serial.println("1st ! sppoted at position : " + String(first_exclamation));
  // Serial.println("2nd ! sppoted at position : " + String(second_exclamation));
  // Serial.println("3rd ! sppoted at position : " + String(third_exclamation));

  String temp = tempstr.substring(2, first_exclamation);
  filterd_temperatur = temp.toFloat();

  Serial.println("filterd_temperatur: " + String(filterd_temperatur));

  position = tempstr.substring(first_exclamation + 1, second_exclamation);
  Serial.println("position: " + position);

  String temp1 = tempstr.substring(second_exclamation + 1, third_exclamation);
  int first_coma = temp1.indexOf(',');
  int second_coma = temp1.indexOf(',', first_coma + 1);
  int third_coma = temp1.indexOf(',', second_coma + 1);

  String temp2 = temp1.substring(0, first_coma);
  filterd_Conductance = temp2.toFloat();
  Serial.println("filterd_Conductance: " + String(filterd_Conductance));

  String temp3 = temp1.substring(first_coma + 1, second_coma);
  filterd_Resistance = temp3.toFloat();
  Serial.println("filterd_Resistance: " + String(filterd_Resistance));

  String temp4 = temp1.substring(second_coma + 1, third_exclamation);
  filterd_Conductance_voltage = temp4.toFloat();
  Serial.println("filterd_Conductance_voltage: " +
                 String(filterd_Conductance_voltage));

  String temp5 = tempstr.substring(third_exclamation + 1, fourth_exclamation);
  filterd_snore_voltages = temp5.toFloat();
  Serial.println("filterd_snore_voltages: " + String(filterd_snore_voltages));

  // String temp5 = tempstr.substring(third_exclamation + 1,);
  // filterd_snore_voltages = temp5.toFloat();
  // Serial.println("filterd_snore_voltages: " + String(filterd_snore_v
}