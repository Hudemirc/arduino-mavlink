
#include "/home/hudemirc/Arduino/libraries/mavlink/include/common/mavlink.h"       


unsigned long previousMillisMAVLink = 0;     // will store last time MAVLink was transmitted and listened
unsigned long next_interval_MAVLink = 1000;  // next interval to count
const int num_hbs = 60;                      // # of heartbeats to wait before activating STREAMS from Pixhawk. 60 = one minute.
int num_hbs_pasados = num_hbs; 




void setup() {
        Serial3.begin(57600);
        Serial.begin(57600);
        
}

void loop() {
        /* The default UART header for your MCU */ 
    int sysid = 1;                   ///< ID 20 for this airplane
    int compid = 158;     ///< The component sending the message is the IMU, it could be also a Linux process
    int type = MAV_TYPE_QUADROTOR;   ///< This system is an airplane / fixed wing
 
// Define the system type, in this case an airplane
    uint8_t system_type = MAV_TYPE_GENERIC;
    uint8_t autopilot_type = MAV_AUTOPILOT_INVALID;
 
    uint8_t system_mode = MAV_MODE_PREFLIGHT; ///< Booting up
    uint32_t custom_mode = 0;                 ///< Custom mode, can be defined by user/adopter
    uint8_t system_state = MAV_STATE_STANDBY; ///< System ready for flight
    // Initialize the required buffers
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
 
// Pack the message
    mavlink_msg_heartbeat_pack(1,compid, &msg, type, autopilot_type, system_mode, custom_mode, system_state);
// Copy the message to the send buffer
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
 
// Send the message with the standard UART send function
// uart0_send might be named differently depending on
// the individual microcontroller / library in use.
     unsigned long currentMillisMAVLink = millis();
  if (currentMillisMAVLink - previousMillisMAVLink >= next_interval_MAVLink) {
    // Timing variables
    previousMillisMAVLink = currentMillisMAVLink;

    Serial3.write(buf, len);

    //Mav_Request_Data();
    num_hbs_pasados++;
    if(num_hbs_pasados>=num_hbs) {
      // Request streams from Pixhawk
      Mav_Request_Data();
      num_hbs_pasados=0;
    }
  }
     comm_receive();
    
 //                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               Serial.println("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy");
}

void comm_receive() {
 
       mavlink_message_t msg;
  mavlink_status_t status;
 
  // COMMUNICATION THROUGH EXTERNAL UART PORT (XBee serial)
 
  while(Serial3.available() > 0 ) 
  {
    uint8_t c = Serial3.read();
    // Try to get a new message
    if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
      // Handle message
        Serial.println(msg.msgid);
      switch(msg.msgid)
      {
              case MAVLINK_MSG_ID_ATTITUDE:
              {
                mavlink_attitude_t altitude;
                mavlink_msg_attitude_decode(&msg,&altitude);
                
        
               
//                  Serial.print("yaw:");
//                  Serial.print(altitude.yaw);
              }
              break;
              case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
              {
                mavlink_global_position_int_t gps;
                mavlink_msg_global_position_int_decode(&msg,&gps);
//                Serial.print("  vx:");
//                Serial.println(gps.vx);   
              }         
              break;
      case MAVLINK_MSG_ID_COMMAND_LONG:
        // EXECUTE ACTION
        break;
      default:
        //Do nothing
        break;
      }
    }
 
    // And get the next one
  }
}
void Mav_Request_Data()
{
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  // STREAMS that can be requested
  /*
   * Definitions are in common.h: enum MAV_DATA_STREAM
   *   
   * MAV_DATA_STREAM_ALL=0, // Enable all data streams
   * MAV_DATA_STREAM_RAW_SENSORS=1, /* Enable IMU_RAW, GPS_RAW, GPS_STATUS packets.
   * MAV_DATA_STREAM_EXTENDED_STATUS=2, /* Enable GPS_STATUS, CONTROL_STATUS, AUX_STATUS
   * MAV_DATA_STREAM_RC_CHANNELS=3, /* Enable RC_CHANNELS_SCALED, RC_CHANNELS_RAW, SERVO_OUTPUT_RAW
   * MAV_DATA_STREAM_RAW_CONTROLLER=4, /* Enable ATTITUDE_CONTROLLER_OUTPUT, POSITION_CONTROLLER_OUTPUT, NAV_CONTROLLER_OUTPUT.
   * MAV_DATA_STREAM_POSITION=6, /* Enable LOCAL_POSITION, GLOBAL_POSITION/GLOBAL_POSITION_INT messages.
   * MAV_DATA_STREAM_EXTRA1=10, /* Dependent on the autopilot
   * MAV_DATA_STREAM_EXTRA2=11, /* Dependent on the autopilot
   * MAV_DATA_STREAM_EXTRA3=12, /* Dependent on the autopilot
   * MAV_DATA_STREAM_ENUM_END=13,
   * 
   * Data in PixHawk available in:
   *  - Battery, amperage and voltage (SYS_STATUS) in MAV_DATA_STREAM_EXTENDED_STATUS
   *  - Gyro info (IMU_SCALED) in MAV_DATA_STREAM_EXTRA1
   */

  // To be setup according to the needed information to be requested from the Pixhawk
  const int  maxStreams = 1;
  const uint8_t MAVStreams[maxStreams] = {MAV_DATA_STREAM_ALL};
  const uint16_t MAVRates[maxStreams] = {0x05};
    
  for (int i=0; i < maxStreams; i++) {
    /*
     * mavlink_msg_request_data_stream_pack(system_id, component_id, 
     *    &msg, 
     *    target_system, target_component, 
     *    MAV_DATA_STREAM_POSITION, 10000000, 1);
     *    
     * mavlink_msg_request_data_stream_pack(uint8_t system_id, uint8_t component_id, 
     *    mavlink_message_t* msg,
     *    uint8_t target_system, uint8_t target_component, uint8_t req_stream_id, 
     *    uint16_t req_message_rate, uint8_t start_stop)
     * 
     */
    mavlink_msg_request_data_stream_pack(2, 200, &msg, 1, 0, MAVStreams[i], MAVRates[i], 1);
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    Serial3.write(buf, len);
  }
}
       
       
       
