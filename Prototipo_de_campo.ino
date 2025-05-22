                       /*--------------------------------------------------------------
                        | Projeto: Sistema de automação para Hidroponia.              |
                        |                                                             |
                        | Objetivo: Controlar irrigação, obter informação e status    |
                        | do sistema próprio ocntrole por Telegram.                   |
                        |                                                             |
                        | Versão de placa ESP32 2.0.17.                               |           
                        |                                                             |
                        | ESP32 - Doit Esp32 Devkit V1                                |
                        |                                                             |
                        | Desenvolvido por: Robson Eduardo                            |
                        |                                                             |
                        | Início do projeto: 01/04/2023                               |
                        --------------------------------------------------------------*/
      
// ------------------------------------------- DERETIVAS BIBLIOTECAS -------------------------------------------
 #include <WiFi.h> 
 #include <WiFiClientSecure.h> 
 #include <WiFiClient.h>
 #include <WiFiManager.h>  // Versão 2.0.17
 #include <WiFiUDP.h>
 #include <UniversalTelegramBot.h>  // Versão 1.3.0
 #include <ArduinoJson.h> // Versão 6.21.5
 #include <DNSServer.h>
 #include <ESPmDNS.h> 
 #include <NTPClient.h>  // Versão 3.2.1
 #include <Update.h> // Versão 3.1.1
 #include <Preferences.h>    
 #include <OneWire.h>  // Versão 2.3.7
 #include <DallasTemperature.h>  // Versão 3.9.0
 #include <LiquidCrystal_I2C.h>  // Versão 1.1.4
 #include <CheckDS18B20.h> // Versão 1.0.3
 #include <RTClib.h>  // Versão 2.1.4
 #include <Wire.h> 
 #include <HTTPClient.h>  
 #include <ESPAsyncWebServer.h>  // Versão 3.1.0
 #include <AsyncTCP.h>
 #include <esp_ota_ops.h>
 #include <esp_partition.h> 

 #define CURRENT_VERSION "0.0.4"

 //#define BOTtoken "5673470044:AAGMcP0VuxIXx3UE-5Nj3lRjpRIYFJGDCFg"  // Meu Telegram.
 //#define CHAT_ID  "1929532977"

 //#define BOTtoken "5967070334:AAHjxoGPddkj9z5-bULEnMxtpnc8WsfoF1s"  // Telegram da mãe.
 //#define CHAT_ID  "6032999590"

 //#define BOTtoken "5971093302:AAHe94yUjsbuw6yjiR4SSzPve3wSAorXBRQ"  // Telegram Ricardo.
 //#define CHAT_ID  "457355158"

 //#define BOTtoken "5984399756:AAEv1ND_l9fc0YQuhKY0ztIOWsFmTOOy8bE"  // Telegram Nene.
 //#define CHAT_ID  "1484669195"

//--------------------------------------------- FIM DAS BIBLIOTECAS -------------------------------------------- 

// ------------------- Mapa de pinos ESP32 - lado esquerdo - olhado o ESP32 de frente. -------------------------

  #define boia_alta           36  // Boia alta ligado na GPIO36.
  #define boia_baixa          39  // Boia baixa ligado na GPIO39.
  #define bt_set              35  // Botão Set ligado na GPIO35.
  //#define ADC_PIN           34  // Conversor ADC.
  #define led_stat_wifi       32  // Led de status do wi-fi ligado na GPIO32.
  #define led_stat_rele       33  // Led de status do rele ligado na GPIO33.
  #define rele_reser_1        25  // rele reserva 1.                                     foi alterado GPIO25.
  //#define DHT11_PIN         26  // Pino ligado a DHT11.
  #define ADC_PIN             26
  #define led_stat_func       18  // Led de status de funcionamento ligado na GPIO27. era 27
  #define led_ciclo_15X30     14  // Status do ciclo 15X30 minutos ligado na GPIO14.     foi alterado para GPIO14.
  #define rele_irriga         13  // Rele que liga bomba de irrigação ligado na GPIO13.
  #define rele_reser_2        23  // rele reserva 2.                                     foi alterado GPIO26.
  #define I2C_SCL             22  // Define o pino GPIO22 como I2C_SCL.
  //#define HC_06_TX           1  // Define o pino Serial TX como HC_06_TX.
  //#define HC_06_RX           3  // Define o pino Serial RX como HC_06_RX.
  #define I2C_SDA             21  // Define o pino GPIO22 como I2C_SDA.
  #define buzzer              27  // Buzzer  ligado na GPIO27. era 19
  #define led_ciclo_15X45     19  // Status do ciclo 15X45 minutos ligado na GPIO19.      
  //#define led_ciclo_15X45   18  // Status do ciclo 15X45 minutos ligado na GPIO18.     foi alterado para GPIO19.
  #define DBS1820pin          17  // Pino ligado a DBS1820.
  #define led_stat_erro_func  16  // Led de status de erro ligado na GPIO16.       
  #define rele_val             4  // Rele que abre a válvula ligado na GPIO4.
  //#define DHTTYPE DHT11         // Define o tipo de sensor usado.
  #define touchPin             2  // Define o uso do pino 2 para touch.  

  // Defina constantes para os tipos de reinicialização (simulando um enum)

  #define BOOT_NONE         0
  #define BOOT_GENERIC      1
  #define BOOT_RESET_WIFI   2
  #define BOOT_TELEGRAM     3
  #define BOOT_RESTORE      4  
  #define BOOT_UPGRADE      5
  
 hw_timer_t *Timer4_Cfg = nullptr;
// -------------------------------------------Fim dos mapas de pinos -------------------------------------------

// --------------------------------------------- VARIAVÉIS STRINGS ---------------------------------------------
    
  String header;
  String chat_id;
  String T_horario;
  String horario;
  String minutos;
  String start_flags;
  String temperatura;
  String humidade;
  String temp_ambi_18B20;
  String temp_nutri_18B20;
  String temp_diss_18B20;
  String counter_autom;
  String situacao_dia;
  String get_addr;
  String get_addr1;
  String get_addr2;
  String get_addre;  
  String get_addre1;
  String get_addre2;
  String varHex;
  String varHex1;
  String varHex2;
  String varHexa;
  String varHexa1;
  String varHexa2;     

  String dig_comp_h_aux;
  String dig_comp_m_aux;
  String dig_comp_s_aux;  

  String val_aux_cor_amb;  
  String val_aux_cor_nut;
  String val_aux_cor_dis;
  String aux_v_cor_nutri;
  String aux_val_cor_dis; 
  String aux_val_cor_ale;   

  String modi_clic_tele_1;
  String modi_clic_tele_2;
  String modi_clic_tele_3;
  String modi_clic_tele_4;
  String modi_clic_tele_5;
  String modi_clic_tele_6;
  String modi_clic_tele_7;
  String modi_clic_tele_8;

  //String cl_data_1;
  //String cl_data_2;  

  String yes_schedule30;

  String rese_prog;
  //String block_sist;
  //String rese_manual;  
  //String bomba_on;
  //String open_val;
  //String rese_crit;
  //String no_des_mod_auto;
  //String repo_agua;
  //String swit_primver;
  //String swit_outinv;
  //String swit_estacao_2;
  //String back_mod_auto;
  //String yes_schedule45;
  //String sorry;
  //String on_of_irri;
  //String alert_repo_agua;
  //String block_val;
  //String cred_del;
  //String on_of_all_ale;
  //String troca_bat;  
  //String core_temp;
  //String incr_decr_temp;
  String cont_reset_aux;
  //String star_cor_temp;
  //String vacoambi;
  //String vaconutr;
  //String vacodiss;
  String temp_corr;  
  //String ambien_C;
  //String nutrien_C;
  //String dissip_C;
  String teste_senso;
  //String hora_cont_1;
  //String hora_cont_2;
  //String test_18B20;  

  String local_ip;       
  String subnet;     
  String gateway;
  String macaddr;
  String SSID;
  String RSSI_i;
  String BSSIDstr;
  String psk_senha;
  String DNS_0;

  String rede_conex;
  //String on_of_dren_1 PROGMEM;

  //String ale_ambi;

  String time_show; 
  //String T_hora_aux;
 // String T_minuto_aux;
  //String T_segundo_aux;
  //String voltts;
  
  String Bem_vindo;
  //String sensor_1;
  //String sensor_2;
  //String sensor_3;
  String temper;
  
  String text;
  String from_name; 
  
  //String aux_stat_cmd;  
  //String time_hours; 

  String remoteVersion = "";

// ---------------------------------------------- FIM DAS STRINGS ----------------------------------------------

// ---------------------------------------------- VARIAVÉIS BYTES ----------------------------------------------

  byte coluna = 20;
  byte linha = 4;
  byte pol;
  byte barra_pro [] PROGMEM = {B11111, B00000, B11111, B11111, B11111, B11111, B00000, B11111};  // LCD barra de progresso.
  byte sml_aleg  [] PROGMEM = {B11011, B00000, B11011, B11011, B00100, B10001, B01110, B00000};  // LCD sorriso alegre.
  byte sml_tris  [] PROGMEM = {B11011, B00000, B11011, B11011, B00100, B00000, B01110, B10001};  // LCD sorriso triste.
  byte bord      [] PROGMEM = {B11111, B00000, B00000, B00000, B00000, B00000, B00000, B11111};  // LCD borda.
  byte bord_esq  [] PROGMEM = {B00011, B00010, B00010, B00010, B00010, B00010, B00010, B00011};  // LCD borda esquerda.
  byte bord_dir  [] PROGMEM = {B11000, B01000, B01000, B01000, B01000, B01000, B01000, B11000};  // LCD borda direita.

  byte spinnerChars[4][8] = {
                            { B00000, B00100, B01110, B11111, B01110, B00100, B00000, B00000 }, // cruz cheia
                            { B00000, B00010, B00110, B11111, B00110, B00010, B00000, B00000 }, // barra inclinada
                            { B00000, B00100, B00100, B11111, B00100, B00100, B00000, B00000 }, // linha vertical
                            { B00000, B01000, B01100, B11111, B01100, B01000, B00000, B00000 }  // outra inclinação
                            };

  byte temp_ambi  [8] = {};  
  byte temp_nutri [8] = {};
  byte temp_diss  [8] = {};
     
  byte ds18;   
  byte bords;
  byte bar_prog;  

  byte conti = 0;
  byte conter_boia = 0; 
  byte cont_buzz = 0;     

  byte ii;  
  byte p; 
  byte w;
  byte m; 

  byte i;
  byte addr [8];  
  byte addr1[8]; 
  byte addr2[8];
  byte pio = 0;  
  byte counter_blackluz = 0;    
  byte counter_wait_oper = 0; 
  byte valor_nomi = 0;
  byte counter_cmd_activ = 0;

            // Posição     0   1   2   3   4   5   6   7   8   9  10
  byte minuto[] PROGMEM = {0, 15, 30, 45, 22, 10, 40, 55, 25, 59, 5};  
  byte hora[]   PROGMEM = {18, 0};

  byte counter_corr_temp = 0;    

  DeviceAddress temp_ambi_exib;
  DeviceAddress temp_nutri_exib; 
  DeviceAddress temp_diss_exib;
  DeviceAddress sensorEnderecos[3];

  const byte BORDA_ESQUERDA_CHAR = 0;
  const byte BORDA_CENTRO_CHAR = 1;
  const byte BORDA_DIREITA_CHAR = 2;
  const byte BARRA_PROGRESSO_CHAR = 3;
  const byte BARRA_INICIO_COLUNA = 12;
  const byte BARRA_FIM_COLUNA = 19;
  const byte PONTO_ATIVACAO_LED_BUZZER = 16;

  byte waitConnSensor = 0;

// ----------------------------------------------- FIM DOS BYTES ----------------------------------------------- 

// --------------------------------------------- VARIAVEIS INTEIRAS --------------------------------------------
  
  const uint8_t outputPins[] = {
                                 led_stat_wifi,       // GPIO32
                                 led_stat_rele,       // GPIO33
                                 rele_reser_1,        // GPIO25
                                 led_stat_func,       // GPIO18
                                 led_ciclo_15X30,     // GPIO14
                                 rele_irriga,         // GPIO13
                                 rele_reser_2,        // GPIO23
                                 buzzer,              // GPIO27
                                 led_ciclo_15X45,     // GPIO19
                                 led_stat_erro_func,  // GPIO16
                                 rele_val             // GPIO04
                               };

  const uint8_t inputPins[] =  {
                                 boia_alta,  // GPIO36
                                 boia_baixa, // GPIO39
                                 bt_set      // GPIO35                          
                               };
 
  int pendingBoot = BOOT_NONE;
  int counter_init_black = 0; 
  int vapor = 0;
  int counter_1 = 0;
  int counter_temps = 0;  
  int counter_alert_sens_ambi = 0;  
  int counter_alert_sens_nutri = 0; 
  int counter_alert_sens_diss = 0; 
  int counter_blackout = 0; 

  int T_hora = 0;
  int T_minuto = 0;
  int T_segundo = 0;

  int botRequestDelay = 750;
  int numNewMessages =  0;

  int cont0 = 0;
  //int status_0 = 0;//*******
  //int status_1 = 0;//*******
  int cont_reset = 1;//******
  int posi = 0;
  //int cont_conex = 0;//******

  int counter_hora = 0;
  //int counter_situ = 0;//****
  int counter_trava_alert = 0;
  int counter_seca = 0;
  int counter_auto = 0;
  int counter_timeout = 0;
  int counter_bip_wifi = 0;
  int counter_flag_temp = 0;
  int counter_dry = 0;
  int counter_start = 0;
  int aux_start = 0; 
  int counter_oo_all_ale = 0;
  int counter_adj_timer = 0;    

  //int threshold = 30;//***********
  //int touchValue = 0; //***********

  int inc_pos_sensor = 1;
  //int number_device = 0;//*********
  short int G;
  short int button_state = 1;      
  //short int last_button_state;//*****************
  //short int cont_wifi = 0;//******************
  short int cont_0 = 0;
  //short int cont_irrig = 0;//********************
  //short int cont_recall_auto = 0;//****************
  //short int cont_stat_30 = 0;//*********************
  //short int cont_stat_45 = 0;//********************  

  //int botRequestDelay0 = 1000;
  //unsigned long int tempoInicio = 0;
  //unsigned long int tempoBotao = 0;
  unsigned long int temp_ant_0 PROGMEM = 0;
  unsigned long int lastTimeBotRan PROGMEM = 0;  
  //unsigned long int lastTimeBotRan0 PROGMEM = 0;
  const unsigned long WIFI_RESET_HOLD_TIME = 8000; // Tempo para segurar para resetar WiFi (ex: 5 segundos)  
  const int maxItems = 7;  // Número máximo de itens

  unsigned long previousMillis = 0;
  const long interval = 850;  // Intervalo de 1 segundo
  int currentItem = 0;
  const int LOG_SIZE = 75;  // Número máximo de linhas no log
  const unsigned long DELAY_BORDA = 1200;
  const unsigned long DELAY_PREENCHIMENTO = 520;
  const unsigned long DELAY_FINAL = 1200;  
  int logIndex = 0; 
  const uint8_t BUZZER_DELAY_MS PROGMEM = 100; // Tempo de delay para o buzzer  

  unsigned long start = millis();

// ----------------------------------------------- FIM DASINTEIRAS --------------------------------------------- 
  
// ---------------------------------------------- VARIAVEIS FLOATS ---------------------------------------------
  
  float ambiC = 0;  
  float nutriC = 0;  
  float dissC = 0;  

  float sta_val_cor_amb = 0.0; 
  float sta_val_cor_nut = 0.0;

  float temp_ref_ambi  = 34.00;
  float temp_ref_nutri = 34.00;
  float temp_ref_diss  = 34.00;

  float t_minima_ambi = 18.0;
  float t_minima_nutr = 18.0;
  float t_minima_diss = 35.0;
  
  float temp_ambi_C  = 0;
  float temp_nutri_C = 0;
  float temp_diss_C  = 0;
  float f = 0; 

  float v_corr_ambi = 0;
  float v_corr_nutr = 0;
  float v_corr_diss = 0;

  float v_corr_amb_maxi = 0;
  float v_corr_nut_maxi = 0;        
  float v_corr_amb_mini = 0;
  float v_corr_nut_mini = 0;  
  
  float auxil_tempC_addr  = 0;
  float auxil_tempC_addr1 = 0;   
  float auxil_tempC_addr2 = 0;


  float aux_v_corr_ambi = 0;
  float aux_v_corr_nutr = 0;
  float aux_v_corr_diss = 0;

  float auxAmbCorr_maxi = 0;
  float auxAmbCorr_mini = 0;

  float auxNutCorr_maxi = 0;
  float auxNutCorr_mini = 0;

  const float TEMP_READ_ERROR = -127.0;
  const float TEMP_INIT_ERROR = 85.0; 

  float aux_AmbMin = 0.0;

  float savedCorAmb = 0;
  float savedCorNut = 0;
  float savedCorDis = 0;
  float savedAmbMax = 0;
  float savedNutMax = 0;
  float savedAmbMin = 0;
  float savedNutMin = 0;

  float savedAdjCorAmb = 0;
  float savedAdjCorNut = 0;
  float savedAdjCorDis = 0;
  float savedAdjAmbMax = 0;
  float savedAdjNutMax = 0;
  float savedAdjAmbMin = 0;
  float savedAdjNutMin = 0;

  float aux_ambiC = 0;
  float aux_nutriC = 0;
  float aux_dissC = 0;

// ----------------------------------------------- FIM DAS FLOATS ---------------------------------------------- 

// ----------------------------------------------- VARIAVEIS BOOL ----------------------------------------------
 
  bool aux_modi_ciclo = true; 
  bool on_off = true;

  //bool estado_0 = false; 
  bool estado_1 = false; 
  bool estado_2 = false; 
  bool estado_3 = false;

  bool trava_15X15_on   = true;
  bool trava_15X15_off  = false;

  bool trava_15X45_on   = true;
  bool trava_15X45_off  = false;

  bool trava_res_esp    = false;
  bool trava_modi_ciclo = false;

  //bool bip_100        = true;
  bool flag             = true;                     
  bool flag_decisao     = false;
  bool flag_wait_0      = true;
  bool flag_wait_1      = true;
  bool flag_trava       = true;
  bool flag_cert        = true; 
  bool flag_toma_dec    = true; 
  //bool start_irri_0     = false;
  //bool start_irri_1     = false;
  //bool start_irri_2     = false;   
  //bool status_rest_0    = false;
  bool rese_manu        = false; 
  bool rese_fabrica     = false;  
  bool stat_on_irrig    = true;
  bool off_sound        = true;

  bool on_off_bip_irri  = true;
  bool on_off_bip_nutr  = true;
  bool on_off_all_bip   = true;
  bool reset_esp        = false;
  bool buzzer_reset     = false;
  bool modo_auto        = true; 
  bool modo_manual      = false;
  //bool on_off           = true;
  bool on_off_geral     = true;

  bool start_agend      = false;
  bool start_touch      = true;

  //bool flag_grav_ambi   = true;
  //bool flag_grav_nutri  = false;

  bool testa_18B20      = true;
  bool flag_conf_sens   = true;
  bool active_light     = true;

  //bool flag_counter_touch = false; 
  //bool flag_backlight_off  = false; 
  //bool flag_alert_temps = false;

  bool rese             = false;
  bool seca             = false;
  bool dry              = true;
  bool repor            = false;
  bool pronto_now       = false;
  bool vapori           = true;
  bool vaporiz          = true;
  //bool flag_led_blink   = false;
  bool trava_blink      = false;
  bool trava_cont_hora  = false;

  //bool flag_all_blink   = false;
  //bool flag_nutri       = false;

  bool flag_sim_abra    = false; 
  //bool black_auxi     = true;
  bool flag_blklight    = true;
  bool flag_counter_blackluz = false;
  //bool RestartTriggered = false;  
  bool flag_init_backlight = false;

  bool flag_pisc_red = false;
  
  bool flag_pisc_red_0 = false;  
  bool flag_pisc_red_1 = false;  
  bool flag_pisc_red_2 = false;  
  bool flag_pisca_wifi = false;  

  bool counter_alert_ambi_0  = false;
  bool counter_alert_nutri_0 = false;
  bool counter_alert_diss_0  = false;  
  
  bool flag_cont_blackout    = true;
  bool flag_on_irrig         = false;
  bool flag_blackout         = false;
  bool flag_trava_blackout0  = true;
  bool flag_trava_blackout1  = true;
  bool flag_trava_blackout2  = true;
  bool flag_trava_blackout3  = true;
  bool flag_trava_blackout4  = true;
  bool flag_trava_blackout5  = true; 
  bool flag_trava_blackout6  = true;    

  bool blackout_15X45        = true;
  bool blackout_15X30        = false;     
  
  bool lock_senso_ambi       = true;
  bool lock_senso_nutr       = true;
  bool lock_senso_diss       = true;

  bool lock_seca_rese       = true;
  //bool bloc_of_sen_amb      = true;
  bool bloc_of_sen_nut      = true;
  bool bloc_of_sen_dis      = true;      

  bool flag_onlygrava_ambi  = true;
  bool flag_onlygrava_nutri = true;
  bool flag_onlygrava_diss  = true;    //***************************** parei aqui 30/10/2024.   

  bool grav_flash_ambi      = false;
  bool grav_flash_nutr      = false;
  bool grav_flash_diss      = false;

  bool flashDevDisAmb       = true;
  bool flashDevDisNut       = true;
  bool flashDevDisDis       = true;

  bool flashDevConAmb0      = false;
  bool flashDevConAmb1      = false;
  bool flashDevConNut0      = false;
  bool flashDevConNut1      = false;
  bool flashDevConDis0      = false;
  bool flashDevConDis1      = false;

  bool flag_rec_flash_0     = true;
  bool flag_rec_flash_1     = true;
  bool flag_rec_flash_2     = true;
  bool flag_rec_flash_3     = true;
  //bool flag_rec_flash_4     = true;
  //bool flag_rec_flash_5     = true;

  bool up_led_30            = false; // original true 
  bool up_led_45            = true;

  bool flash_18_06hs_0      = true;
  bool flash_18_06hs_1      = true;

  bool anomalia         = false;
  bool on_backlig       = false;
  bool aux_cont0        = true;
  bool aux_but_stat     = true;
  bool trava_rese       = false;  
  //bool on_off_bomba     = false;
  bool start_clear      = false;
  bool resetar          = false;       
  bool block_flash_0    = true;
  bool block_empty      = true;
  bool block_dry        = true;
  bool buzzer_00        = false;

  bool flag_wait_oper   = false;
  bool wait_laco_0      = false;
  bool wait_laco_1      = true;
  bool switch_LCD       = true;
  bool bomb_on          = false;
  bool scheduled        = true;
  bool stop_led_30      = false;
  bool stop_led_45      = false;
  bool to_change_15X30  = true;
  bool veryf_boias      = false;
  bool cor_temp_ambi    = false;
  bool cor_temp_nutr    = false;
  bool cor_temp_diss    = false;
  bool maxi_temp_ambi   = false;
  bool maxi_temp_nutr   = false;

  bool mini_temp_ambi   = false;
  bool mini_temp_nutr   = false;

  bool start_corr_temp  = false;
  bool stat_corr_temp   = true;      //---------------------------------------- 

  bool flag_clear_IP    = false;
  bool situa            = false;
  bool flag_temp_18B20  = true;
  bool flag_alert_ambi  = true;
  bool flag_alert_nutri = true;
  bool flag_alert_diss  = true;
 
  bool forceBoot_0      = false;
  bool forceBoot_1      = false;
  bool forceBoot_2      = false;
  bool forceBoot_3      = false;
  bool forceBoot_gene   = false;
  bool set_time_cur     = false;

  bool flag_3600        = false;
  bool flag_10800       = false;
  bool flag_18000       = false;
  bool flag_25200       = false;
  bool flag_32400       = false;  
  
  bool flag_15X15       = true; 
  bool flag_15X45       = false;
  bool flag_10X20       = false;
  //bool status_led_temp  = false;
  bool trava_alert      = true;
  bool flag_seca        = false;
  bool flag_stat_cic30  = false;
 
  
  bool flag_count_auto  = false;
  bool recall_auto      = false;
  bool led_pause_auto   = false;
  //bool flag_wifi_status = false;
  bool flag_bip_wifi    = false;

  bool flag_conta_hora    = true;
  //bool on_off_alert_nutri = false;  
  bool flag_alerta_nutri  = true;   
  bool flag_repor         = false; 
  bool flag_pos_fill      = false;   
  bool flag_bac_pwr_1 = false;  
  bool flag_bac_pwr_2 = false; 
  bool block_full         = true;

  bool trava_alert_temp   = true;
  //bool flag_counter_temp  = true;

  bool lcd_wait_nutri     = false;
  //bool flag_decisao_lcd   = true;
  bool flag_encher_lcd    = true;
  
  bool flag_clear_lcd_info = true;
  bool block_lcd_info      = true;
  bool flag_alert_wifi_lcd = true;

  bool flag_temp_led_rele  = false;
  bool counter_flag_wait_0 = true;
  //bool counter_flag_wait_1 = false;
  
  bool restaure         = true;
  bool modo_trab        = true;
  bool stat_bomba       = false;
  bool aux_stat_bomba   = false;
  bool modo_work        = false;
  //bool stat_led_conex   = false;

  bool block_on_00  = false;
  bool block_off_00 = false;

  bool block_on_15  = false;
  bool block_off_15 = false;

  bool block_on_45  = false;
  bool block_off_45 = false;

  bool block_on_30  = false;
  bool block_off_30 = false;  
  bool estadoBotao; 

  bool aux_dec_repor   = false;
  bool open_valvu      = false;
  bool no_pronto       = true;
  bool on_valvu        = false;
  bool open_valvu_off  = false;
  bool flag_back_pow_2 = false; 
  bool back_parci_lcd  = false;
  bool fill_reser      = false;
  bool repor_aux_1     = false; 
  bool repor_aux_2     = false;
  bool level_crit_boia = false;
  bool memo_lcd        = true;
  bool dry_lcd         = false;
  bool switch_estacao  = true;

  bool active_aler_nutr = false;
  bool block_all_sys    = true;
  bool flag_back_pow_1  = false;
  bool block_cmd_0      = true;
  bool block_cmd_1      = true;
  bool aux_reset_esp    = true; 
  bool switc_outinver   = true;
  bool atual_primavera  = false;
  bool atual_outoinver  = true;
  bool led_verao        = true;
  bool led_inver        = false; 
  bool reset_force      = false;
  bool aspers           = false;
  bool refrig           = false;
  bool activ_aler_ambi  = true;
  bool activ_aler_nutr  = true;
  bool activ_aler_diss  = true;   
  bool normal_temp_amb  = false;
  bool normal_temp_nut  = false;
  bool sensor_atuan_amb = false;
  bool sensor_atuan_nut = false;
  bool desat_seca       = true;
  bool aux_pronto       = false;
  bool onoffall_bip     = true;
  bool lcd_token_id     = true;
  bool cont_cmd         = false;
  bool cmd_active       = true;
  bool selct_min_outinv = false;
  bool disa_aler_sens   = true;

  bool sensor_amb_normal = true;
  bool sensor_nutr_normal = true;
  bool sensor_diss_normal = true;

  bool aux_falha_ambi = true;
  bool aux_falha_nutr = true;
  bool aux_falha_diss = true;
  bool primeiraExecucao = true;
  bool sensorAmbienteOK = true;
  bool sensorNutricaoOK = true;
  bool flag_pisc_red_0_anterior = false;
  bool flag_pisc_red_1_anterior = false; 

  // Variáveis para controle individual de alertas (além da trava_alert_temp)
  bool alerta_ambi_ativo = true;    // Alerta do ambiente ativo?
  bool alerta_nutri_ativo = true;   // Alerta da nutrição ativo?
  bool alerta_diss_ativo = true;    // Alerta do dissipador ativo?
  bool activeContLockCmd = false;
  bool openValv          = false;
  bool res               = false;
  bool liber_yes_del     = false;
  bool exitPressButton   = true;
  bool ledTelegram       = false;
  bool led_update        = false;
  bool rtcOk             = false;
  bool activeAlertVoid   = false;
  bool aux_sucesso;
  bool initStatusLogged  = false;
  bool switchUpdata      = true;
  
// ------------------------------------------------ FIM DAS BOOL -----------------------------------------------

// ----------------------------------------------- VARIAVEIS CHAR ----------------------------------------------

  //const char* validUser = "administrador";  // Usado quando a tela de login é solicitada.
  //const char* validPassword = "admin@1973"; // Usado quando a tela de login é solicitada. 

  const char* ntpServer = "pool.ntp.org";
  const long gmtOffset_sec = -10800; // Horário de Brasília (GMT-3) em segundos
  const long daylightOffset_sec = 0;

  char tempTask = 0;
  char aux_tentativa = 0;
  
  char aux1_minut_on  = 0;  
  char aux2_minut_on  = 30;                    
  char aux3_minut_off = 15;
  char aux4_minut_off = 45; 
  char aux5_minut_off = 15;

  //const char* token = nullptr;
  //const char* id = nullptr;

  char tokenBuffer[50] = {0};
  char idBuffer[15] = {0};

  UniversalTelegramBot* bot = nullptr; 
  void (*sendMessage)(String, String, String);

  const char MSG_IP_PREFIX[] PROGMEM = "IP:";
  const char MSG_NO_CONNECTION[] PROGMEM = "CONEXAO PERDIDA!"; 
  const char loginPage[] PROGMEM = R"rawliteral(
                                <!DOCTYPE html>
                                <html lang="pt-BR">
                                <head>
                                    <meta charset="UTF-8">
                                    <meta name="viewport" content="width=device-width, initial-scale=1.0">
                                    
                                    <title>Login - Schidro</title>
                                    <style>
                                        body {
                                            font-family: 'Arial', sans-serif;
                                            background: linear-gradient(135deg, #65a7dd, #ffffff);
                                            display: flex;
                                            justify-content: center;
                                            align-items: center;
                                            height: 100vh;
                                            margin: 0;
                                        }
                                        .container {
                                            background-color: #ffffff;
                                            border-radius: 10px;
                                            box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
                                            width: 100%;
                                            max-width: 400px;
                                            padding: 30px;
                                            text-align: center;
                                        }
                                        h1 {
                                            color: #333333;
                                            font-size: 24px;
                                            margin-bottom: 20px;
                                        }
                                        input[type="text"],
                                        input[type="password"] {
                                            width: 100%;
                                            padding: 10px;
                                            margin: 10px 0;
                                            border: 1px solid #cccccc;
                                            border-radius: 5px;
                                            font-size: 16px;
                                            box-sizing: border-box; /* Adicionado para consistência */
                                        }
                                        button {
                                            padding: 10px 20px;
                                            background-color: #65a7dd;
                                            border: none;
                                            border-radius: 5px;
                                            color: white;
                                            font-size: 16px;
                                            cursor: pointer;
                                            transition: background-color 0.3s ease;
                                            width: 100%; /* Adicionado para consistência */
                                            margin-top: 10px; /* Adicionado para espaçamento */
                                            box-sizing: border-box; /* Adicionado para consistência */
                                        }
                                        button:hover {
                                            background-color: #4a8ecb;
                                        }
                                        .error-message {
                                            color: red;
                                            font-size: 14px;
                                            margin-top: 15px; /* Ajustado */
                                            min-height: 1.2em; /* Reserva espaço mesmo vazio */
                                        }
                                    </style>
                                </head>
                                <body>
                                    <div class="container">
                                        <h1>Login</h1>
                                        <form id="loginForm" autocomplete="off">
                                            <input type="text" id="username" placeholder="Usuário" required autocomplete="username">
                                            <input type="password" id="password" placeholder="Senha" required autocomplete="new-password">
                                            <button type="submit">Entrar</button>
                                        </form>
                                        <div id="error-message" class="error-message"></div>
                                    </div>

                                    <script>
                                        const loginForm = document.getElementById('loginForm');
                                        const usernameInput = document.getElementById('username');
                                        const passwordInput = document.getElementById('password');
                                        const errorMessageDiv = document.getElementById('error-message');

                                        // --- NOVO CÓDIGO PARA LIMPAR CAMPOS ---
                                        // Executa quando a página é carregada ou exibida (incluindo via botão Voltar)
                                        function clearFormFields() {
                                            if (usernameInput) {
                                                usernameInput.value = '';
                                            }
                                            if (passwordInput) {
                                                passwordInput.value = '';
                                            }
                                            if (errorMessageDiv) {
                                                errorMessageDiv.textContent = '';
                                                // errorMessageDiv.style.display = 'none'; // Esconde se não houver erro
                                            }
                                          // Opcional: Foca no campo de usuário
                                          // if(usernameInput) {
                                          //    usernameInput.focus();
                                          // }
                                        }

                                        // Usa o evento 'pageshow' que é mais confiável para bfcache do que 'load'
                                        window.addEventListener('pageshow', function(event) {
                                            clearFormFields();
                                        });
                                        // --- FIM DO NOVO CÓDIGO ---


                                        // Código original para tratar o submit (mantido)
                                        loginForm.addEventListener('submit', function(e) {
                                            e.preventDefault();
                                            errorMessageDiv.textContent = ''; // Limpa erro anterior

                                            const username = usernameInput.value;
                                            const password = passwordInput.value;

                                            fetch('/login', { // Endpoint de login no ESP32
                                                method: 'POST',
                                                headers: { 'Content-Type': 'application/json' },
                                                body: JSON.stringify({ username, password }),
                                            })
                                            .then(response => {
                                                if (response.ok) {
                                                    // Login OK, redireciona para o menu
                                                    window.location.href = '/menu'; // Ou a página principal pós-login
                                                } else {
                                                    // Login falhou, tenta obter mensagem de erro ou usa padrão
                                                    return response.text().then(text => {
                                                        throw new Error(text || 'Usuário ou senha inválidos.');
                                                    });
                                                }
                                            })
                                            .catch(error => {
                                                errorMessageDiv.textContent = error.message;
                                                // errorMessageDiv.style.display = 'block';
                                            });
                                        });
                                    </script>
                                </body>
                                </html>
                                )rawliteral";  
  
  const char menu[] PROGMEM = R"rawliteral(
                              <!DOCTYPE html>
                              <html lang="pt-BR">
                              <head>
                                  <meta charset="UTF-8" />
                                  <meta name="viewport" content="width=device-width, initial-scale=1" />
                                  <title>Menu Principal - Schidro</title>
                                  <style>
                                      body {
                                          font-family: Arial, sans-serif;
                                          background: linear-gradient(135deg, #65a7dd, #ffffff);
                                          display: flex;
                                          justify-content: center;
                                          align-items: center;
                                          height: 100vh;
                                          margin: 0;
                                      }
                                      .container {
                                          background-color: #fff;
                                          border-radius: 10px;
                                          box-shadow: 0 4px 10px rgba(0,0,0,0.2);
                                          width: 100%;
                                          max-width: 400px;
                                          padding: 30px;
                                          text-align: center;
                                          box-sizing: border-box;
                                      }
                                      h1 {
                                          color: #333;
                                          font-size: 24px;
                                          margin-bottom: 20px;
                                      }
                                      .button {
                                          display: block;
                                          width: 100%;
                                          padding: 10px 20px;
                                          margin: 10px 0;
                                          background-color: #65a7dd;
                                          border: none;
                                          border-radius: 5px;
                                          color: white;
                                          font-size: 16px;
                                          text-decoration: none;
                                          cursor: pointer;
                                          transition: background-color 0.3s ease;
                                          box-sizing: border-box;
                                      }
                                      .button:hover {
                                          background-color: #4a8ecb;
                                      }
                                  </style>
                              </head>
                              <body>
                                  <div class="container">
                                      <h1>Menu Principal</h1>
                                      <a href="/upload" class="button">Upload de Firmware</a>
                                      <a href="/telegram" class="button">Configuração do Telegram</a>
                                      <a href="/monitor" class="button">Monitor Serial</a>
                                      <a href="/status" class="button">Status da Rede</a>
                                      <a href="/sobre" class="button">Sobre</a>
                                  </div>
                              </body>
                              </html>
                              )rawliteral"; // pronto

  const char Stat_rede[] PROGMEM = R"rawliteral(
                                  <!DOCTYPE html>
                                  <html lang="pt-BR">
                                  <head>
                                      <meta charset="UTF-8" />
                                      <meta name="viewport" content="width=device-width, initial-scale=1" />
                                      <title>Sobre e Rede - Schidro</title>
                                      <style>
                                          body {
                                              font-family: Arial, sans-serif;
                                              background: linear-gradient(135deg, #65a7dd, #ffffff);
                                              display: flex;
                                              justify-content: center;
                                              align-items: center;
                                              height: 100vh;
                                              margin: 0;
                                              padding: 20px;
                                              box-sizing: border-box;
                                          }

                                          .container {
                                              background-color: #fff;
                                              border-radius: 10px;
                                              box-shadow: 0 4px 10px rgba(0,0,0,0.2);
                                              width: 100%;
                                              max-width: 500px;
                                              padding: 30px;
                                              text-align: left;
                                              box-sizing: border-box;
                                          }

                                          h1 {
                                              color: #333;
                                              font-size: 24px;
                                              margin-bottom: 20px;
                                              text-align: center;
                                          }

                                          .network-info p {
                                              color: #555;
                                              font-size: 16px;
                                              margin-bottom: 10px;
                                              word-wrap: break-word;
                                          }

                                          .network-info p strong {
                                              color: #333;
                                              display: inline-block;
                                              width: 180px;
                                              margin-right: 5px;
                                          }

                                          .note {
                                              font-size: 12px;
                                              color: #888;
                                              margin-top: 15px;
                                              text-align: center;
                                          }

                                          .button-group {
                                              display: flex;
                                              justify-content: flex-end;
                                              gap: 10px;
                                              margin-top: 20px;
                                          }

                                          .button {
                                              display: inline-block;
                                              padding: 10px 20px;
                                              background-color: #65a7dd;
                                              border: none;
                                              border-radius: 5px;
                                              color: white;
                                              font-size: 16px;
                                              cursor: pointer;
                                              text-align: center;
                                              transition: background-color 0.3s ease;
                                          }

                                          .button:hover {
                                              background-color: #4a8ecb;
                                          }
                                      </style>
                                  </head>
                                  <body>
                                      <div class="container">
                                          <h1>Informações de Rede</h1>
                                          <div class="network-info">
                                              <p><strong>SSID:</strong> {{SSID}}</p>
                                              <p><strong>IP Local:</strong> {{LOCAL_IP}}</p>
                                              <p><strong>Máscara de Rede:</strong> {{SUBNET}}</p>
                                              <p><strong>Gateway:</strong> {{GATEWAY}}</p>
                                              <p><strong>DNS:</strong> {{DNS}}</p>
                                              <p><strong>MAC Address:</strong> {{MACADDRESS}}</p>
                                              <p><strong>Seu IP de Internet:</strong> {{PUBLIC_IP}}</p>
                                              <p><strong>Intensidade Sinal (RSSI):</strong> {{RSSI}} dBm</p>
                                          </div>
                                          <p class="note">Obs.: O IP Internet requer conexão e pode demorar a aparecer ou falhar se o serviço externo estiver indisponível.</p>
                                          <div class="button-group">
                                              <button class="button" onclick="window.location.href='/'">Voltar</button>
                                          </div>
                                      </div>
                                  </body>
                                  </html>
                                  )rawliteral"; // pronto
  
  const char uploadPage[] PROGMEM = R"rawliteral(
                                    <!DOCTYPE html>
                                    <html lang="pt-BR">
                                    <head>
                                        <meta charset="UTF-8" />
                                        <meta name="viewport" content="width=device-width, initial-scale=1.0" />
                                        <title>Atualização OTA - Schidro</title>
                                        <style>
                                            body {
                                                font-family: Arial, sans-serif;
                                                background: linear-gradient(135deg, #65a7dd, #ffffff);
                                                display: flex;
                                                justify-content: center;
                                                align-items: center;
                                                height: 100vh;
                                                margin: 0;
                                            }

                                            .container {
                                                background-color: #fff;
                                                border-radius: 10px;
                                                box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
                                                width: 100%;
                                                max-width: 400px;
                                                padding: 30px;
                                                text-align: center;
                                                box-sizing: border-box;
                                            }

                                            h1 {
                                                color: #333;
                                                font-size: 24px;
                                                margin-bottom: 20px;
                                            }

                                            input[type="file"] {
                                                margin-bottom: 20px;
                                                width: 100%;
                                            }

                                            .button {
                                                display: inline-block;
                                                padding: 10px 20px;
                                                background-color: #65a7dd;
                                                border: none;
                                                border-radius: 5px;
                                                color: white;
                                                font-size: 16px;
                                                cursor: pointer;
                                                transition: background-color 0.3s ease;
                                                margin-top: 10px;
                                                text-align: center;
                                            }

                                            .button:hover {
                                                background-color: #4a8ecb;
                                            }

                                            #progress-bar-container {
                                                width: 100%;
                                                background-color: #cfcfcf;
                                                border-radius: 5px;
                                                margin-top: 20px;
                                                overflow: hidden;
                                            }

                                            #progress-bar {
                                                width: 0%;
                                                height: 20px;
                                                background-color: #65a7dd;
                                                text-align: center;
                                                line-height: 20px;
                                                color: white;
                                            }

                                            .error-message {
                                                color: red;
                                                font-size: 14px;
                                                margin-top: 10px;
                                            }

                                            .button-group {
                                                display: flex;
                                                justify-content: space-between;
                                                gap: 10px;
                                                margin-top: 20px;
                                            }
                                        </style>
                                    </head>
                                    <body>
                                        <div class="container">
                                            <h1>Atualização OTA</h1>
                                            <form id="upload_form">
                                                <input type="file" id="firmware" required><br>
                                                <div class="button-group">
                                                    <button type="submit" class="button">Atualizar</button>
                                                    <button type="button" class="button" onclick="window.location.href='/'">Voltar</button>
                                                </div>
                                            </form>
                                            <div id="progress-bar-container">
                                                <div id="progress-bar">0%</div>
                                            </div>
                                            <div id="error-message" class="error-message"></div>
                                        </div>

                                        <script>
                                            document.getElementById('upload_form').addEventListener('submit', function(e) {
                                                e.preventDefault();

                                                const fileInput = document.getElementById('firmware');
                                                const file = fileInput.files[0];
                                                if (!file) {
                                                    alert('Por favor, selecione um arquivo.');
                                                    return;
                                                }

                                                const formData = new FormData();
                                                formData.append('firmware', file);

                                                const xhr = new XMLHttpRequest();
                                                xhr.open('POST', '/update', true);

                                                xhr.upload.onprogress = function(event) {
                                                    if (event.lengthComputable) {
                                                        const percentComplete = (event.loaded / event.total) * 100;
                                                        const progressBar = document.getElementById('progress-bar');
                                                        progressBar.style.width = percentComplete + '%';
                                                        progressBar.textContent = Math.round(percentComplete) + '%';
                                                    }
                                                };

                                                xhr.onload = function() {
                                                    const progressBar = document.getElementById('progress-bar');
                                                    if (xhr.status === 200) {
                                                        progressBar.textContent = 'Upload concluído!';
                                                    } else {
                                                        const errorMessage = xhr.responseText || 'Erro desconhecido.';
                                                        document.getElementById('error-message').textContent = 'Erro ao enviar o arquivo: ' + errorMessage;
                                                        progressBar.style.width = '0%';
                                                        progressBar.textContent = '0%';
                                                    }
                                                };

                                                xhr.onerror = function() {
                                                    document.getElementById('error-message').textContent = 'Erro de conexão com o servidor.';
                                                    const progressBar = document.getElementById('progress-bar');
                                                    progressBar.style.width = '0%';
                                                    progressBar.textContent = '0%';
                                                };

                                                xhr.send(formData);
                                            });
                                        </script>
                                    </body>
                                    </html>
                                    )rawliteral"; // pronto
  
  const char PAGE_Telegram[] PROGMEM = R"rawliteral(
                                      <!DOCTYPE html>
                                      <html lang="pt-BR">
                                      <head>
                                          <meta charset="UTF-8" />
                                          <meta name="viewport" content="width=device-width, initial-scale=1.0" />
                                          <title>Configuração Universal Bot Telegram</title>
                                          <style>
                                              body {
                                                  font-family: Arial, sans-serif;
                                                  background: linear-gradient(135deg, #65a7dd, #ffffff);
                                                  display: flex;
                                                  justify-content: center;
                                                  align-items: center;
                                                  height: 100vh;
                                                  margin: 0;
                                              }

                                              .container {
                                                  background-color: #fff;
                                                  border-radius: 10px;
                                                  box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
                                                  width: 100%;
                                                  max-width: 500px;
                                                  padding: 30px;
                                                  box-sizing: border-box;
                                              }

                                              h1 {
                                                  color: #333;
                                                  font-size: 24px;
                                                  margin-bottom: 20px;
                                                  text-align: center;
                                              }

                                              form label {
                                                  display: block;
                                                  margin-bottom: 5px;
                                                  font-weight: bold;
                                                  color: #555;
                                              }

                                              form input[type="text"] {
                                                  padding: 10px;
                                                  margin-bottom: 15px;
                                                  width: 100%;
                                                  border: 1px solid #ccc;
                                                  border-radius: 5px;
                                                  font-size: 16px;
                                                  box-sizing: border-box;
                                              }

                                              .button-group {
                                                  display: flex;
                                                  justify-content: space-between;
                                                  gap: 10px;
                                                  margin-top: 20px;
                                              }

                                              .button {
                                                  display: inline-block;
                                                  padding: 10px 20px;
                                                  background-color: #65a7dd;
                                                  border: none;
                                                  border-radius: 5px;
                                                  color: white;
                                                  font-size: 16px;
                                                  cursor: pointer;
                                                  transition: background-color 0.3s ease;
                                              }

                                              .button:hover {
                                                  background-color: #4a8ecb;
                                              }

                                              .error-message {
                                                  color: red;
                                                  font-size: 14px;
                                                  margin-top: 10px;
                                                  text-align: center;
                                              }
                                          </style>
                                      </head>
                                      <body>
                                          <div class="container">
                                              <h1>Configuração Universal Bot Telegram</h1>
                                              <form method="POST" action="/save">
                                                  <label for="token">Token do Bot:</label>
                                                  <input type="text" id="token" name="token" maxlength="50" required>

                                                  <label for="id">ID do Bot:</label>
                                                  <input type="text" id="id" name="id" maxlength="15" required>

                                                  <div class="button-group">
                                                      <input type="submit" class="button" value="Salvar">
                                                      <button type="button" class="button" onclick="window.location.href='/'">Voltar</button>
                                                  </div>
                                              </form>
                                              <div id="error-message" class="error-message"></div>
                                          </div>
                                      </body>
                                      </html>
                                      )rawliteral"; // pronto
  
  const char monitorSerial[] = R"rawliteral(
                              <!DOCTYPE html>
                              <html lang="pt-BR">
                              <head>
                                <meta charset="UTF-8" />
                                <meta name="viewport" content="width=device-width, initial-scale=1.0" />
                                <title>Monitor Serial - Schidro</title>
                                <style>
                                  body {
                                    font-family: Arial, sans-serif;
                                    background: linear-gradient(135deg, #65a7dd, #ffffff);
                                    display: flex;
                                    justify-content: center;
                                    align-items: center;
                                    height: 100vh;
                                    margin: 0;
                                    color: #e6e6e6;
                                  }

                                  .container {
                                    width: 90vw;
                                    max-width: 1200px;
                                    height: 90vh;
                                    background-color: #ffffff;
                                    border-radius: 10px;
                                    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
                                    padding: 30px;
                                    text-align: center;
                                    display: flex;
                                    flex-direction: column;
                                  }

                                  h1 {
                                    color: #333;
                                    font-size: 24px;
                                    margin-bottom: 20px;
                                  }

                                  #log {
                                    flex-grow: 1;
                                    background: #111;
                                    color: #e6e6e6;
                                    font-family: monospace;
                                    border: 1px solid #0f0;
                                    border-radius: 5px;
                                    padding: 10px;
                                    overflow-y: auto;
                                    overflow-x: auto;
                                    white-space: pre-wrap;
                                    text-align: left;
                                  }

                                  .button {
                                    padding: 12px 24px;
                                    margin-top: 15px;
                                    font-size: 18px;
                                    background-color: #65a7dd;
                                    border: none;
                                    border-radius: 5px;
                                    color: white;
                                    cursor: pointer;
                                    transition: background-color 0.3s ease;
                                    width: fit-content;
                                  }

                                  .button:hover {
                                    background-color: #4a8ecb;
                                  }

                                  .button-container {
                                    display: flex;
                                    justify-content: center;
                                    gap: 10px;
                                    flex-wrap: wrap;
                                  }
                                </style>
                              </head>
                              <body>
                                <div class="container">
                                  <h1>🔌 Monitor Serial SCHidro 💦</h1>
                                  <div id="log">Conectando...</div>
                                  <div class="button-container">
                                    <button class="button" onclick="clearLog()">🧹 Limpar Log</button>
                                    <button class="button" onclick="window.location.href='/'">🏠 Voltar</button>
                                    <button class="button" onclick="togglePause()" id="pauseBtn">⏸️ Pausar</button>
                                  </div>
                                </div>

                                <script>
                                  window.onload = function () {
                                    let ws;
                                    let isPaused = false;

                                    const logDiv = document.getElementById('log');
                                    const pauseBtn = document.getElementById("pauseBtn");

                                    function connectWebSocket() {
                                      
                                    ws = new WebSocket(`ws://${window.location.hostname}/ws`);                                      

                                    ws.onopen = () => 
                                      {
                                        logDiv.innerHTML += "<br>✅ Conectado ao WebSocket<br><br>";
                                        ws.send("getLog");
                                      };

                                    ws.onmessage = (event) => 
                                      {
                                        if (!isPaused) 
                                          {
                                            logDiv.innerHTML += event.data + "<br>";
                                            logDiv.scrollTop = logDiv.scrollHeight;
                                          }
                                      };

                                      ws.onerror = (err) => {
                                        logDiv.innerHTML += "<br>❌ Erro na conexão WebSocket";
                                        console.error("WebSocket erro", err);
                                      };

                                      ws.onclose = () => {
                                        logDiv.innerHTML += "<br>🔌 WebSocket desconectado. Tentando reconectar...";
                                        setTimeout(connectWebSocket, 500); // aumentado para evitar reconexões agressivas
                                      };
                                    }

                                    function togglePause() {
                                      isPaused = !isPaused;
                                      if (isPaused) {
                                        logDiv.innerHTML += "<br>⏸️ Pausado.";
                                        pauseBtn.innerHTML = "▶️ Continuar";
                                      } else {
                                        logDiv.innerHTML += "<br>▶️ Lendo porta serial.<br><br>";
                                        pauseBtn.innerHTML = "⏸️ Pausar";
                                      }
                                    }

                                    function clearLog() {
                                      logDiv.innerHTML = '';
                                    }

                                    window.togglePause = togglePause;
                                    window.clearLog = clearLog;

                                    connectWebSocket();
                                  }
                                </script>
                              </body>
                              </html>
                              )rawliteral"; // pronto
            

  
  const char PAGE_Save_Success[] PROGMEM = R"rawliteral(
                                          <!DOCTYPE html>
                                          <html lang="pt-BR">
                                          <head>
                                            <meta charset="UTF-8" />
                                            <meta name="viewport" content="width=device-width, initial-scale=1.0" />
                                            <title>Sucesso</title>
                                            <style>
                                              body {
                                                font-family: Arial, sans-serif;
                                                background: linear-gradient(135deg, #65a7dd, #ffffff);
                                                display: flex;
                                                justify-content: center;
                                                align-items: center;
                                                height: 100vh;
                                                margin: 0;
                                              }

                                              .container {
                                                background-color: #ffffff;
                                                border-radius: 10px;
                                                box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
                                                width: 100%;
                                                max-width: 500px;
                                                padding: 30px;
                                                box-sizing: border-box;
                                                text-align: center;
                                              }

                                              h1 {
                                                color: #333;
                                                font-size: 24px;
                                                margin-bottom: 20px;
                                              }

                                              p {
                                                font-size: 16px;
                                                color: #555;
                                                margin-bottom: 20px;
                                              }

                                              .button {
                                                display: inline-block;
                                                padding: 10px 20px;
                                                background-color: #65a7dd;
                                                border: none;
                                                border-radius: 5px;
                                                color: white;
                                                font-size: 16px;
                                                text-decoration: none;
                                                text-align: center;
                                                cursor: pointer;
                                                transition: background-color 0.3s ease;
                                              }

                                              .button:hover {
                                                background-color: #4a8ecb;
                                              }
                                            </style>
                                          </head>
                                          <body>
                                            <div class="container">
                                              <h1>✅ Dados salvos com sucesso!</h1>
                                              <p>As configurações foram armazenadas corretamente.</p>
                                              <a href="/telegram" class="button">Voltar</a>
                                            </div>
                                          </body>
                                          </html>
                                          )rawliteral"; // pronto

  const char PAGE_Save_Fail[] PROGMEM = R"rawliteral(
                                        <!DOCTYPE html>
                                        <html lang="pt-BR">
                                        <head>
                                          <meta charset="UTF-8" />
                                          <meta name="viewport" content="width=device-width, initial-scale=1.0" />
                                          <title>Erro ao Salvar</title>
                                          <style>
                                            body {
                                              font-family: Arial, sans-serif;
                                              background: linear-gradient(135deg, #65a7dd, #ffffff);
                                              display: flex;
                                              justify-content: center;
                                              align-items: center;
                                              height: 100vh;
                                              margin: 0;
                                            }

                                            .container {
                                              background-color: #ffffff;
                                              border-radius: 10px;
                                              box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
                                              width: 100%;
                                              max-width: 500px;
                                              padding: 30px;
                                              box-sizing: border-box;
                                              text-align: center;
                                            }

                                            h1 {
                                              color: #cc0000;
                                              font-size: 24px;
                                              margin-bottom: 20px;
                                            }

                                            p {
                                              font-size: 16px;
                                              color: #555;
                                              margin-bottom: 20px;
                                            }

                                            .button {
                                              display: inline-block;
                                              padding: 10px 20px;
                                              background-color: #f44336;
                                              border: none;
                                              border-radius: 5px;
                                              color: white;
                                              font-size: 16px;
                                              text-decoration: none;
                                              text-align: center;
                                              cursor: pointer;
                                              transition: background-color 0.3s ease;
                                            }

                                            .button:hover {
                                              background-color: #d32f2f;
                                            }
                                          </style>
                                        </head>
                                        <body>
                                          <div class="container">
                                            <h1>❌ Erro: Faltam parâmetros!</h1>
                                            <p>Verifique se todos os campos foram preenchidos corretamente.</p>
                                            <a href="/telegram" class="button">Voltar</a>
                                          </div>
                                        </body>
                                        </html>
                                        )rawliteral"; 

  const char host[] = "schidro"; 
  const char conex_good[] PROGMEM  = "Conexão foi bem sucedida com o dispositivo WI-FI!";
  const char start_hidro[] PROGMEM = "Schidro inicializado com sucesso!";
  const char system_disa[] PROGMEM = "Sistema está pausado, comando cancelado!";
  const char valvuClose[] PROGMEM  = "Vávula já se encontra fechada.";
  const char cmd_invalid[] PROGMEM = "Comando incorreto, entre o comando válido!\n\n ou cancele ->     /cancelar";
  const char troca_bat_now[] PROGMEM = "Atenção! A bateria está com a tensão ruim, necessita troca. Siga o manual de instruções.\n\n"
                               "Inicie a troca ->     /troca_bat";  

  const char rese_manual_0[] PROGMEM = "Você iniciou o reset manual, isso fará com que seu dispositivo reinicie. Você tem certeza do reset?\n\n"
                                 "Se sim ->      /sim_reset"; 


  const char clean_creden[] PROGMEM = "Não encontrei nenhum pedido para limpar credenciais da rede.\n\n"
                                      "Limpar credenciais ->        /del_creden";                             

  const char rese_manual_1[] PROGMEM = "⛔ Você não pode reiniciar o dispositivo agora! "
                               "Sua irrigação está ativa nesse momento! " 
                               "Seja paciente e aguarde o término da irrigação.";  

  const char back_mod_auto[] PROGMEM = "⚠️ Você esqueceu de ativar o modo automático! Não se preocupe estou retomando o controle, ativando...";
  const char rese_crit_0[] PROGMEM = "O reservatório foi drenado por motivo de força maior com sucesso! "
                             "Conclua seu objetivo pelo qual o reservatório foi drenado.\n\n"
                             "⚠️ OBS.: Se o motivo da drenagem foi uma manutenção ou uma limpeza. "
                             "Agora desligue o dispositivo de controle da rede elétrica! 🔌\n\n"
                             "🤔 Caso desista da limpeza ou da manutenção, OK! Mas o dispositivo deve ser reiniciado.\n\n" 
                             "Reiniciar ->         /reset";

  const char rese_crit_1[] PROGMEM = "Notei que foi executado uma drernagem total do reservatório. "
                             "O procedimento agora é reiniciar o sistema repondo a água.\n\n" 
                             "Para repor ->        /repor_agua ";

  const char rese_crit_2[] PROGMEM = "⚠️ Reservatório com nível crítico! Fique tranquilo, "
                             "vou usar a reserva de segurança, " 
                             "mas de tempos em tempos vou alerta você para repor a água do reservatório.\n\n"
                             "Comece a repor ->       /repor_agua"; 

  const char repo_agua[] PROGMEM = "OK! Repondo a água 💧, aguarde...\n\n"
                           "Fique tranquilo eu avisarei quando o reservatório estiver cheio. "
                           "Após repor a água será necessário repor os nutrientes e calibrar os parâmetros da solução.";

  const char nutr_wait[] PROGMEM = "Reservatório no nível máximo! Sistema pausado! "
                           "Sua nutrição foi comprometida quando abriu a válvula, "
                           "agora você precisa repor e ajustar sua nutrição. " 
                           "Após terminar de repor nutrientes, " 
                           "inicie o sistema apertando o botão SET no painel.";

  const char bom_work[] PROGMEM = "👍 Bom trabalho! Tudo pronto para cuidar do seu cultivo! Agora é comigo.";
  const char alert_repo_agua[] PROGMEM = "Seu reservatório está cheio, não há necessidade de repor a água. Comando cancelado! ";
  const char open_val[] PROGMEM = "Tem certeza em repor a água? Seu reservatório está parcialmente vazio! "
                          "Se estiver ciente dessa decisão, vá em frente, a válvula não pode ser fechado na condição parcial. "                                
                          "Não se preocupe, eu aviso quando o reservatório estiver cheio.\n\n "
                          "⚠️ Muito importante lembrar que ao abrir a válula, a solução perderá os parâmetros e terá que ser corrigida.\n\n "
                          "Confirme ->    /sim_repor    ou    /cancelar\n"; 

  const char bomba_on[] PROGMEM = "Você não pode repor a água do reservatório!. Irrigação está em andamento. Aguarde a irrigação terminar! Comando cancelado!";
  const char onoff_bipirri_0[] PROGMEM = "OK! Desativando aviso sonoro da irrigação!";
  const char onoff_bipirri_1[] PROGMEM = "OK! Ativando aviso sonoro da irrigação!";
  const char onoff_bipnutr_0[] PROGMEM = "OK! Desativando aviso sonoro da reposição de nutrientes!";
  const char onoff_bipnutr_1[] PROGMEM = "OK! Ativando aviso sonoro da reposição de nutrientes!"; 
  const char esp_reset_0[] PROGMEM = "OK! Desativando o reset programado!";
  const char esp_reset_1[] PROGMEM = "OK! Ativando o reset programado, aguarde...";
  const char esp_reset_2[] PROGMEM = "Pronto! Reset programado foi ativado com sucesso.";
  const char reset_buzzer_0[] PROGMEM = "OK! Desativando o aviso sonoro do reset programado!";
  const char reset_buzzer_1[] PROGMEM = "OK! Ativando o aviso sonoro do reset programado!";
  const char bip_all_onoff_0[] PROGMEM = "Desativando todo os avisos sonoros, menos os alertas!";
  const char bip_all_onoff_1[] PROGMEM = "Ativando todo os avisos sonoros, menos os alertas que já estão ativados!";
  const char cred_del[] PROGMEM = "As credenciais de rede serão deletadas e seu dispositivo será reinicializado, "
                          "após o reinicio entre com as credenciais do Wi-Fi.\n\n"
                          "Confirme ->   /sim_delete    ou    /cancelar"; 

  const char cred_del_0[] PROGMEM = "As credenciais de rede serão deletadas e seu dispositivo será reinicializado, "
                          "após o reinicio entre com as credenciais do Wi-Fi.\n\n"
                          "Obs.: Credenciais deletadas pelo painel.";

  const char amb_sen_of_block_0[] PROGMEM = "O alerta do sensor do ambiente foi desativado temporariamente.";
  const char amb_sen_of_block_1[] PROGMEM = "O alerta do sensor do ambiente já foi desativado."; 
  const char nut_sen_of_bloc_0[] PROGMEM = "O alerta do sensor da nutrição foi desativado.";
  const char nut_sen_of_bloc_1[] PROGMEM = "O alerta do sensor da nutrição já foi desativado.";
  const char dis_sen_of_bloc_0[] PROGMEM = "O alerta do sensor o dissipador foi desativado.";
  const char dis_sen_of_bloc_1[] PROGMEM = "O alerta do sensor do dissipador já foi desativado.";
  const char cl_data_1[] PROGMEM = "Você iniciou a restauração de fábrica! "
                           "Lembre-se que todas as configurações serão perdidas menos os sensores e os parâmetros do Telegram.\n\n "                           
                           "Tem certeza da restauração de fabrica?\n\nSe sim, selecione ->    /sim_restaure ";

  const char cl_data_2[] PROGMEM = "Deletando o Token e ID do Telegram, e em seguida reiniciando...,"
                                   " vai ser preciso inserir os dados do Telegram novamente."
                                   " Acessando a página -> http://schidro.local -> Botão ""Configuração do Telegram""  \n\n"
                                   "Delete executado pelo painel."; 

  const char yes_restaure_0[] PROGMEM = "OK! Restaurando agora, iniciando no...";
  const char yes_restaure_1[] PROGMEM = "Não encontrei nenhum pedido de restauração de dados. Comando cancelado!";
  const char ready_now_0[] PROGMEM = "OK! Reiniciando seu sistema...";
  const char ready_now_1[] PROGMEM = "Não encontro nenhum pedido de reposição de nutriente.";
  const char sensor_delet[] PROGMEM = "Deletando sensores, aguarde...";
  const char sensor_delet_suc[] PROGMEM = "Sensores deletados com sucesso.";
  const char teleg_delet[] PROGMEM = "Deletando Token e ID de configuração do Telegram!\n\n"
                             "⚠️ ATENÇÃO! Após deletar os parâmetros não será mais "
                             "possível obter status ou até mesmo enviar comandos. "
                             "Para restabelecer o Bot Telegram faça um reset manual,"
                             " após reiniciar carregue a página de login http://schidro.local,"
                             " vá na página de configuração do Telegram entre com ID do seu bot Telegram.";                                                        

  const char telegr_delet_suc[] PROGMEM = "Parâmetros do Telegram deletados com sucesso.";
  const char descr_onoff_0[] PROGMEM = "Descrição do teste do sensor foi ativada.";
  const char descr_onoff_1[] PROGMEM = "Descrição do teste do sensor foi desativada.";
  const char restour[] PROGMEM= "Restaurado! O sistema será reinicializado pra concluir a restauração... ";
  const char onoff_bcklight_0[] PROGMEM = "O backlight está no modo fixo 💡.";
  const char onoff_bcklight_1[] PROGMEM = "O backlight está no modo temporário.";
  const char troca_bat[] PROGMEM = "OK! Você está trocando a bateria 🔋 do RTC,  para executar a troca com sucesso, será necessário executar alguns passos:\n\n"
                           "PASSO 1: Primeiro desligue o sistema de controle da energia elétrica.\n\n"
                           "PASSO 2: Troque a bateria do RTC por uma nova.\n\n"
                           "PASSO 3: Religue seu dispositivo de controle a energia elétrica.\n\n"
                           "Após reiniciar sistema o RTC será atualizado automaticamente através do NTP, "
                           "  para que o NPT esteja atualizado,  seu dispositivo de controle deve estar conectado a internet.";

  const char test_18B20_0[] PROGMEM = "Todo sensor DS18B20 possui um número de série único com 64bit,"
                              " os 8 bits menos significativos do código ROM contêm o código da família 1-Wire do DS18B20: 28h."
                              " Isso significa se o serial do sensor conter o 28h ou 40 em decimal,"
                              " ele é original, caso contrário você foi enganado, não é original."
                              " Caso não queira ver esse texto novamente toque no comando.\n\n"
                              "Quer desativar ->   /on_off_descri\n\n"
                              "Temos como testar os sensores do dispositivo através do comando.\n\n"
                              "Quer testar ->   /test_sensor\n\n"
                              "O comando retornará os seriais dos sensores e visualmente você poderá identificar.\n";

  const char test_18B20_1[] PROGMEM = "Descrição do sensor DS18B20 está desativada.\n\n"
                              "Quer ativar ->   /on_off_descri.\n\n";

  const char swit_primver_0[] PROGMEM = "Você está optando pela irrigação de primavera e verão, "
                                "sabia decisão! Na primavera e verão o clima fica mais quente, assim seu cultivo necessitará de mais água.\n\n"
                                "A partir de agora a irrigação diurna é: 15X15 minutos."
                                "A partir de agora a irrigação noturna é: 15X45 minutos."
                                " Aguarde, configurando...\n\n"; 

  const char swit_primver_1[] PROGMEM = "A estação já está em primavera e verão!";
  const char swit_outinv_0[] PROGMEM = "Você está optando pela irrigação de outono e inverno, "
                               "sabia decisão! Na irrigação de outono e inverno o clima fica mais frio, assim seu cultivo necessita menos água e também economizará energia elétrica.\n\n"
                               "A partir de agora a irrigação diurna é: 10X20 minutos.\n\n"
                               "Agora você decidirá qual será o tempo de irrigação noturna no inverno!\n\n";

  const char swit_outinv_1[] PROGMEM = "A estação já está em outono e inverno!"; 
  const char swit_outinv_2[] PROGMEM = "A irrigação noturna atual é: "
                               "5 minutos irrigando e 55 minutos pausado.\n\n"
                               "Pode decidir ->      /min_5      /min_10      /min_15\n\n";

  const char swit_outinv_3[] PROGMEM = "A irrigação noturna atual é: "
                                       "10 minutos irrigando e 50 minutos pausado.\n\n"
                                       "Pode, decidir ->      /min_5      /min_10      /min_15\n\n";

  const char swit_outinv_4[] PROGMEM = "A irrigação noturna atual é: "
                               "15 minutos irrigando e 45 minutos pausado.\n\n"
                               "Pode, decidir ->      /min_5      /min_10      /min_15\n\n";  

  const char search_irri_0[] PROGMEM = "Você optou por 5 minutos irrigando e 55 pausado.";
  const char search_irri_1[] PROGMEM = "Você optou por 10 minutos irrigando e 50 pausado.";
  const char search_irri_2[] PROGMEM = "Você optou por 15 minutos irrigando e 45 pausado.";
  const char search_irri_3[] PROGMEM = " Aguarde, configurando...";
  const char core_temp[] PROGMEM = "Você está preste a setar a temperatura ou alerta dos sensores.\n"
                           "Diga! Qual parâmetro você quer setar?\n\n"
                           "Selecione ->    /temp_ambi    /temp_nutr    /temp_diss\n\n"
                           "Temp. máxima ->    /t_maxi_ambi     /t_maxi_nutr\n\n" 
                           "Temp. mínima ->    /t_mini_ambi     /t_mini_nutr\n\n"
                           "Se preferir setar os parametros nominal da:\n\n"
                           "Temperatura ambiente ->    /back_temp_ambi\n\n"
                           "Temperatura nutrição ->    /back_temp_nutr\n\n"
                           "Temperatura dissipador ->    /back_temp_diss\n\n"
                           "Temperatura máxima ambiente ->    /back_maxi_ambi\n\n"
                           "Temperatura mínima ambiemte ->    /back_mini_ambi\n\n"
                           "Temperatura máxima nutrição ->    /back_maxi_nutr\n\n"
                           "Temperatura mínima nutrição ->    /back_mini_nutr\n\n"
                           "Setar todos os ajuste de uma vez ->    /back_all_temp" 
                           "     ou     /cancelar";                                 

  const char star_cor_temp_7[] PROGMEM = "Não encontrei nenhum pedido de correção das temperaturas, inicie a correção ->    /set_tempera  .";
  const char Command_0[] PROGMEM = "A segue os comandos abaixo:\n\n"
                           "/pausa_sistema - Ativa ou desativa totalmente o sistema.\n"
                           "/on_off_auto - Ativa ou desativa modo automático.\n"                                                
                           "/on_off_irrig - Liga e desliga a bomba de irrigação.\n"
                           "/on_off_drenar - Usado para esvaziar o reservatório.\n"
                           "/repor_agua - Use para repor a água do reservatório.\n"                                
                           "/off_repor - Interrompe a reposição da água.\n"
                           "/on_off_bip_nutr - Ativa ou desativa aviso sonoro da reposição nutrientes.\n"
                           "/on_off_bip_irrig - Ativa ou desativa aviso sonoro da irrigação.\n"
                           "/on_off_reset_prog - Ativa ou desativa o reset programado para 14 dias.\n"
                           "/on_off_bip_res_prog - Ativa o aviso sonoro do reset programado.\n"                
                           "/on_off_all_bip - Ativa ou desativa todos os aviso sonoros exceto os alertas.\n"
                           "/status_conex - Verifica se há conexão com internet.\n"
                           "/del_creden - Limpar dados da credencial do Wi-fi.\n"
                           "/reset - Reinicia o dispositivo.\n"                                           
                           "/clean_data - Inicia restauração de fábrica.\n"                
                           "/tempera - Retorna status das temperaturas.\n" 
                           "/del_sensor - Deleta todos os sensores.\n"
                           "/del_telegram - Deleta informações do Telegram como Token e ID.\n"
                           "/on_off_descri - Desativar descrição dos sensores.\n"  
                           "/test_sensor - Verifica se os sensores são originais.\n"
                           "/on_off_backlight - Coloca o backlight do LCD em modo temporário ou fixo.\n\n" 
                           "Para mais comandos ->       /continue";

  const char Command_1[] PROGMEM = "/troca_bat - Traz a descrição do sensor DBS18B20.\n"
                           "/show_time - Mostra horários dos protocolos NPT e RTC.\n"
                           "/show_DS18B20 - Mostra descrição do sensor DS18B20.\n"
                           "/modo_primaverao - Configura para estações primavera e verão.\n"
                           "/modo_outoinver - Configura para estações outono e inverno.\n"
                           "/set_tempera - Corrige a temperatura dos sensores.\n"                            
                           "/status_cmd - Retorna o status de todo sistema.\n"
                           "/cancelar - Tem a função de cancelar comandos.\n\n"
                           "Fim dos comandos.\n\n "
                           "BOM CULTIVO!.\n\n";

  const char txt_cont_cmd[] PROGMEM = "Fim dos comandos! Caso queira rever do inicio\n\n"
                              "Para rever ->        /start";                                                          

  const char block_sist[] PROGMEM = "Sistema está pausado! \n\n"
                            "A partir de agora seu cultivo não está sob os cuidados do sistema!"
                            " Então, está por sua conta e risco!";  

  const char pause_sistem[] PROGMEM = "⛔ Você não pode pausar agora! "
                              "Sua irrigação está ativa nesse momento! " 
                              "Seja paciente e aguarde o término da irrigação.";

  const char auto_onoff[] PROGMEM = "⛔ Você não pode ativar ou desativar o controle automático agora! "
                           "Sua irrigação está ativa nesse momento! "                          
                           "Seja paciente e aguarde o término da irrigação.";

  const char on_of_irri[] PROGMEM = "Irrigação está em modo automático, comando cancelado! Você precisa desativar o modo automático.\n\nDesative ->   /on_off_auto";
  const char on_of_dren_1[] PROGMEM = "Você não pode drenar o reservatório, em andamento! Aguarde a irrigação terminar, comando cancelado!";
  const char on_of_dren_2[] PROGMEM = "Reservatório no nível crítico, Por isso a bomba não pode ser ligada!";
  const char mens_min_outinv[] PROGMEM = "Não encontrei nenhum pedido de mudança de estação. Comando cancelado.";  
  char count_button PROGMEM = 0;

  char line_0[] PROGMEM = "SEU RESERV. NAO ESTA";
  char line_1[] PROGMEM = "TOTALMENTE VAZIO MES-";
  char line_2[] PROGMEM = "MO ASSIM REPOR AGUA";
  char line_3[] PROGMEM = "OK! REPONDO...";  

  char dry_line_0[] PROGMEM = "ATENCAO!";
  char dry_line_1[] PROGMEM = "O RESERVATORIO SERA";
  char dry_line_2[] PROGMEM = "DRENADO. AGUARDE...";
  char dry_line_3[] PROGMEM = "SISTEMA PAUSADO!";

  const char* versionURL = "https://raw.githubusercontent.com/robedua/OTA/refs/heads/main/version.txt";                            
  const char* firmwareURL = "https://raw.githubusercontent.com/robedua/OTA/refs/heads/main/Prototipo_de_campo.ino.bin";

  String logBuffer[LOG_SIZE];

  volatile bool novaVersaoDisponivel = false;
  String mensagemUpdate = "";

// ------------------------------------------------ FIM DAS CHAR ----------------------------------------------- 

// -------------------------------------------------- OBJETOS --------------------------------------------------
   struct SensorInfo 
      {
        bool conectadoAnterior;
        bool temperaturaNormalAnterior;
        bool alertaAtivo;
        float ultimaTemperatura;
      };

      SensorInfo ambienteInfo = {true, true, false, 0};
      SensorInfo nutricaoInfo = {true, true, false, 0};
      SensorInfo dissipadorInfo = {true, true, false, 0};  

   struct SensorControl 
      {
        bool alertasAtivos;
        String comandoOff;
      };

      SensorControl ambienteCtrl = {true, "/off_sensor_ambi"};
      SensorControl nutricaoCtrl = {true, "/off_sensor_nutri"};
      SensorControl dissipadorCtrl = {true, "/off_sensor_diss"};
   
   Preferences Memory;
   Preferences Sensores;
   Preferences Flash;
   Preferences Telegram;  

   AsyncWebServer server(80);       // Porta padrão   
   AsyncWebSocket ws("/ws");        // Caminho do WebSocket   

   //WebServer server(80);
   WiFiClientSecure client;
   //UniversalTelegramBot bot(BOTtoken, client);  
   WiFiManager wifiManager;
   WiFiUDP udp;
   //NTPClient ntp(udp, "a.st1.ntp.br", -3 * 3600, 180000);  //Cria um objeto "NTP" com as configurações. 
   NTPClient ntp(udp, ntpServer, gmtOffset_sec, daylightOffset_sec); 
   OneWire oneWire(DBS1820pin);
   DallasTemperature sensor(&oneWire);  
   LiquidCrystal_I2C lcd(0x27, coluna, linha);    
   RTC_DS3231 rtc; 
   
   using namespace CheckDS18B20;
// ---------------------------------------------- FIM DOS OBJETOS ----------------------------------------------

// -------------------------------------------- PROTOTIPO DE FUNÇÔES -------------------------------------------

    void blinkWifiLED();
    void soundAlertBuzzer();
    void clearIpInfoOnLCD();
    void displayIpInfoOnLCD();
    void adjustTemperature(float& correctionValue, float baseValue, const char* memoryKey, String description, int temporiza);
    void string_inc_decr(const char* text);    
    void loadValuesFromFlash(); 
    void saveValuesToFlash(const String& newToken, const String& newId); 
    void handleRoot(AsyncWebServerRequest *request);    
    void handlerStatusRede(AsyncWebServerRequest *request); 
    void handleLogin();    
    void handleMenu(AsyncWebServerRequest *request);    
    void handleUpload(AsyncWebServerRequest *request);    
    void handleUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);    
    void handleSave(AsyncWebServerRequest *request);    
    void string_dinamica(const char *mensagem);       
    void on_irrig_15X15();
    void off_irrig_15X15();
    void on_irrig_15X45();
    void off_irrig_15X45();    
    void handleNewMessages(int numNewMessages);    
    void mills_newmess();
    void all_led_on_off(bool led_);   
    void blinkIpTeekgram();    
    void wait_nutri();
    void status_wifi();            
    void reseta_ESP(); 
    void restore();
    void seca_reserva();  
    void cont_hora(); 
    void temperaturas();    
    void lcd_info();    
    void falha_sensor();
    void local_ender(); 
    void configurarSensor(byte *endereco, const char* label, const char* nomeMemoria, float &tempAux, uint8_t posSensorEsperada);   
    void test_sensores();
    void status_conex();
    void black_light_activ(); 
    void task_time(void* Parameters);
    void blackout(); 
    void pushReset();    
    void on_flag_blackout();   
    void off_flag_blackout(); 
    void read_flash();   
    void descr_DS18B20(); 
    void barprog_senso();  
    void salvarEstadoFlash();    
    void scanI2CBus();
    void desativarTodosAlertas();    
    void salvarEstadoAlertas();   
    void printAddress(const char* nome, const uint8_t* address);  
    void testarSensor(const String& tipo, byte* temp_sensor, String& resultado);
    String formatarEndereco(byte* temp_sensor);
    String getPublicIP();
    String formatWithSign(float value, int decimals = 2); 
    String formatCorrectionStatus(const char* sensorName, float correctionValue);
    String getLogAsText();
    void a_buzzer(byte num_vezes, int tempo_ativ, int tempo_inat); // Prototipo buzzer (n vezes, temp ativo, tem inativo)  
    bool blockInvald();
    void logToWeb(const String& msg);
    void baseTimer(void* Parameters)
        {
          for (;;)
             {           
              counter_trava_alert ++; 

              cont_buzz ++;

              counter_1 ++;
              if (counter_1 == 3)  // Faz o led de status piscar indicando que o sistema está em funcionamento.
                {
                  estado_3 = !estado_3;
                  digitalWrite(led_stat_func, estado_3); 
                  counter_1 = 0;
                }

              if (trava_blink == true)  // Habilita ou desabilita o modo piscante do led quando reservatório vazio.
                {
                  estado_1 = !estado_1;
                  
                  digitalWrite(led_stat_erro_func, estado_1); // led vermelho.
                }

              conter_boia ++;

              if (conter_boia == 10) // Temporização para verificarr o estados das boias.
                {
                  veryf_boias = true;              
                }

              if (trava_cont_hora == true)  // Habilita ou desabilita o alerta quando o recipiente está vazio.
                {
                  counter_hora ++; 

                  if (counter_hora == 3600)  flag_3600  = true;
                  if (counter_hora == 10800) flag_10800 = true;
                  if (counter_hora == 18000) flag_18000 = true;
                  if (counter_hora == 25200) flag_25200 = true;
                  if (counter_hora == 32400) 
                    {
                      flag_32400 = true;
                      counter_hora = 0;
                    }
                }

              counter_temps ++;
              if (counter_temps == 300)
                {
                  flag_temp_18B20 = true;              
                  counter_temps = 0;
                }
              
              if (counter_alert_ambi_0 == true)  // Ativa contador do alerta do sensor ambiente. 
                {
                  counter_alert_sens_ambi ++;  // Contador de tempo quando falha o sensor.

                  if (counter_alert_sens_ambi == 180)   
                    {
                      flag_alert_ambi = true;
                      //counter_alert_sens_ambi = 0;                  
                    } 
                }

              if (counter_alert_nutri_0 == true)  // Ativa contador do alerta do sensor nutrição.
                {
                  counter_alert_sens_nutri ++;  // Contador de tempo quando falha o sensor.

                  if (counter_alert_sens_nutri == 180)   
                    {
                      flag_alert_nutri = true;
                      //counter_alert_sens_nutri = 0;                  
                    } 
                }

              if (counter_alert_diss_0 == true)  // Ativa contador do alerta do ensor dissipador. 
                {
                  counter_alert_sens_diss ++;  // Contador de tempo quando falha o sensor.

                  if (counter_alert_sens_diss == 180)   
                    {
                      flag_alert_diss = true;
                      //counter_alert_sens_diss = 0;                  
                    } 
                } 

              if (counter_trava_alert == 10800) 
                {
                  trava_alert = true;
                } 

              if (flag_seca == true)  
                {
                  counter_seca ++;              

                  if (counter_seca == 5)
                    {
                      digitalWrite(rele_reser_2, LOW);    
                      digitalWrite(led_stat_rele, LOW);      

                      flag_seca = false;
                      //vaporiz = true;
                      counter_seca = 0;                  
                    }
                } 

              if (flag_count_auto == true)
                {
                  counter_auto ++;

                  counter_autom = (120 - counter_auto) / 60;

                  led_pause_auto = !led_pause_auto;
                  
                  digitalWrite(led_stat_erro_func, led_pause_auto);
                
                  //if (counter_auto == 3600) 
                  if (counter_auto == 120)
                    {                  
                      //flag_count_auto = false;
                      recall_auto = true;
                      
                      digitalWrite(led_stat_erro_func, LOW); 
                      digitalWrite(led_stat_rele, LOW);
                      digitalWrite(rele_irriga, LOW);                 

                      //counter_auto = 0;
                    }
                }
                
                if (flag_pisca_wifi == true) //  Flag responsável por alerta visual por falta de conexão com dispsitivo.
                  {
                    estado_2 = !estado_2;

                    digitalWrite(led_stat_wifi, estado_2); 
                    digitalWrite(led_stat_erro_func, estado_2); 

                    counter_bip_wifi ++; 

                    if (counter_bip_wifi == 300)  // 300 segundos equivale 5 minutos.
                      {
                        flag_bip_wifi = true;

                        counter_bip_wifi = 0;
                      }                               
                  }

                if (active_aler_nutr == true)
                  {
                    cont_0 ++;
                    
                    if (cont_0 == 900)  // Controla contagem do aviso para repor ingridientes. valor original 900.
                      { 
                        counter_flag_wait_0 = true; 
        
                        cont_0 = 0;
                      }
                  }

                if (flag_temp_led_rele == true)
                  {              
                    counter_flag_temp ++;

                    if (counter_flag_temp == 180)
                      {              
                        flag_temp_led_rele = false;
                        digitalWrite(led_stat_rele, LOW);
                        counter_flag_temp = 0;
                      } 
                  }
                
                if (flag_blklight == true) //  condição que verefica se o timer do backlight foi ativa.
                  {    
                    counter_blackluz ++;
                    
                    if (counter_blackluz == 30) 
                      {
                        counter_blackluz = 0;
                        flag_counter_blackluz = true;                    
                        flag_blklight = false;                   
                      }
                  }

                counter_init_black ++;
                if (counter_init_black == 1200)
                  {                 
                    flag_init_backlight = true;
                    counter_init_black = 0;
                  } 

                if (flag_pisc_red_0 == true || flag_pisc_red_1 == true || flag_pisc_red_2 == true || flag_pisc_red == true || ledTelegram == true || led_update == true)    
                  {
                    situa = !situa;
                    digitalWrite(led_stat_erro_func, situa); 
                  } 

                if (flag_cont_blackout == true)
                  {
                    counter_blackout ++;

                    if (counter_blackout == 15)
                      {                    
                        flag_on_irrig = true;
                        flag_cont_blackout = false;       
                        counter_blackout = 0;                                 
                      }
                  }

                if (flag_wait_oper == true) // Aguardando ativação no laço que aguarda o operador repor a agua.
                  {
                    counter_wait_oper ++;

                    if (counter_wait_oper == 13) 
                      { 
                        switch_LCD = !switch_LCD;

                        counter_wait_oper = 0;
                      }
                  } 

                if (start_corr_temp == true)  
                  {
                    counter_corr_temp ++;
                    
                    if (counter_corr_temp == 180)  // Conta 90 segundos e desativa o comando "/set_tempera". Para que inicie novamente.
                      {
                        start_corr_temp = false;
                        counter_corr_temp = 0;
                      }
                  }

                if (trava_alert_temp == false)
                  {
                    counter_oo_all_ale ++;
                          
                    if (counter_oo_all_ale == 7200) 
                      {
                        trava_alert_temp = true;

                        counter_oo_all_ale = 0;
                      } 
                  }

                  counter_adj_timer ++;

                  if (counter_adj_timer == 3600)
                    {
                      // Atualiza o horário do NTP
                      ntp.update();

                      unsigned long epochTime = ntp.getEpochTime();

                      Serial.print("Hora do NTP: ");
                      Serial.println(epochTime);

                      logToWeb(" - Hora do NTP: ");

                      DateTime ntpDateTime(epochTime);

                      // Ajusta o DS3231 com a hora do NTP
                      rtc.adjust(ntpDateTime);

                      Serial.println("Horário do DS3231 ajustado com sucesso!"); 
                      logToWeb(" - Horário do DS3231 ajustado com sucesso!");

                    }
                    
                  if (activeContLockCmd == true)
                    {
                      counter_cmd_activ ++;

                      if (counter_cmd_activ == 45)
                        {
                          cmd_active = true;

                          counter_cmd_activ = 0;
                          activeContLockCmd = false;
                          block_cmd_0 = true;
                          block_cmd_1 = true;
                        } 
                    }                    

                  vTaskDelay(pdMS_TO_TICKS(1000)); 

            }                  
        }    

    void readHtml(void* Parameters)
        {
          for (;;)
             {
                if (WiFi.status() == WL_CONNECTED)
                  {                
                    //server.handleClient(); // Processa as requisições do servidor       apagar se der certo

                    //webSocket.loop();     apagar se der certo
                  }

               vTaskDelay(pdMS_TO_TICKS(10));     
             }
        }               

    void lerBotoes(void* Parameters) 
        {  
          for (;;)
             {   
               if (digitalRead(bt_set) == LOW) 
                 {
                   // O botão está pressionado (conectando o pino 35 ao GND)
                   lcd.backlight();

                   on_backlig = true;  // Ativa o contador.
                 }

                   vTaskDelay(pdMS_TO_TICKS(10));
               }
        }

    void task_time(void* Parameters)           // Função responsável por mantar a hora em dia. 
        {
          for (;;)
            {  
              if (WiFi.status() == WL_CONNECTED) 
                {
                  ntp.forceUpdate(); // Forçar atualização do horário NTP 
                        
                  T_horario = ntp.getFormattedTime(); // Obter o horário atual do NTP

                  T_hora = ntp.getHours();
                  T_minuto = ntp.getMinutes();
                  T_segundo = ntp.getSeconds();
                } 
                else 
                  {
                    if (rtc.begin()) 
                      { 
                        DateTime now = rtc.now();

                        T_hora = now.hour();                        
                        T_minuto = now.minute();
                        T_segundo = now.second();

                        Serial.print(String(T_hora) + ":" + String(T_minuto) + ":" + String(T_segundo));
                        logToWeb(String(T_hora) + ":" + String(T_minuto) + ":" + String(T_segundo));
                        
                        char Tx_horario[9]; // Buffer para armazenar o horário no formato "HH:MM:SS"

                        sprintf(Tx_horario, "%02d:%02d:%02d", T_hora, T_minuto, T_segundo);

                        T_horario = Tx_horario;

                        tempTask ++;

                        if (tempTask == 360) 
                          {
                            Serial.println("Horário atualizado via RTC: " + String(Tx_horario));
                            tempTask = 0;
                          }                     
                      } 
                      else 
                        {
                          Serial.println("Falha ao iniciar o RTC!");
                        }
                    }                      
                vTaskDelay(pdMS_TO_TICKS(10000)); 
            }
        }  

    void read_flash_sensor();   
    void force_boot();
    void exibirTemperaturaLCD(byte linha, const char* nome, float temperatura); 
    char* montarMensagemConexao();
    void executeAction(int item);
    void pressButton();
    void waitNonBlocking(unsigned long durationMs);
    void mostrarMensagemTentandoConectar(int tentativa);
    void mostrarMensagemFalhaTemporaria();
    void mostrarMensagemFalhaFinal();
    void mostrarMensagemConectado();
    void blinkIpTelegram();    
    void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    void configConexWifi();
    void estabilishConnec();
    void timesStart();    
    void enviarMensagem(const char* mensagem);
    void logInitStatus();

    bool enviarMensagemSeguro(const String& mensagem) 
        {
          if (bot == nullptr) return false;

          bool sucesso = bot->sendMessage(idBuffer, mensagem, "");
          aux_sucesso = sucesso;

          if (sucesso) 
            {
              Serial.println("Telegram: Mensagem enviada com sucesso!");
              return true;
            } 
            else 
              {
                Serial.println("Telegram: Falha ao enviar mensagem.");
                return false;
              }
        }

    void checkFirmware();
    void verificarAtualizacao();
    /*void taskVerificaUpdate(void *pvParameters) 
        {
          for (;;) 
             {
               if (WiFi.status() == WL_CONNECTED) 
                 {
                   verificarAtualizacao(); 
                 }
    
                  //vTaskDelay(10800000 / portTICK_PERIOD_MS); // a cada 3h
                  //vTaskDelay(3600000 / portTICK_PERIOD_MS); // a cada 1h
                  vTaskDelay(1800000 / portTICK_PERIOD_MS); // a cada 30 minutos                               
                  //vTaskDelay(300000 / portTICK_PERIOD_MS); // para testes: a cada 5 minutos
             }
        }*/
    
    String obterVersaoRemota();

// ---------------------------------------- FIM DO PROTOTIPO DE FUNÇÔES ----------------------------------------
  
void setup() 
    {
      Serial.begin(115200);
      delay(150);      

      Serial.println(F(" ---------------------------- INICIANDO ----------------------------")); 

      // -------------------- Configuração de pinos --------------------
      for (uint8_t sd = 0; sd < sizeof(inputPins) / sizeof(inputPins[0]); sd++) 
         {
           pinMode(inputPins[sd], INPUT);
         }

      for (uint8_t i = 0; i < sizeof(outputPins) / sizeof(outputPins[0]); i++) 
         {
           pinMode(outputPins[i], OUTPUT);
           digitalWrite(outputPins[i], LOW);
         }

      pushReset();

     // -------------------- Carregando da memória flash --------------------
     read_flash();
     read_flash_sensor();
     loadValuesFromFlash();

     // -------------------- Inicializa LCD --------------------
     lcd.init();     
     lcd.backlight();
     delay(300);

     WiFi.mode(WIFI_STA);  // Garante modo estação

     delay(100);

     configConexWifi();    // Aqui o WiFiManager assume tudo

     Serial.println("Wi-Fi conectado com IP: " + WiFi.localIP().toString());

     // -------------------- Inicializa mDNS --------------------
     if (!MDNS.begin(host)) 
       {
         Serial.println("Erro ao configurar serviço mDNS! Use IP direto: " + WiFi.localIP().toString());
       } 
       else
         {
           Serial.println("MDNS iniciado: http://" + String(host) + ".local/");
         }

      // -------------------- Informações do ESP --------------------
      Serial.printf("Versão do ESP-IDF: %s\n", esp_get_idf_version());

      // -------------------- Inicializa sensor DS18B20 --------------------

         /*---------------------------------------
          |         Resolução do DS18B20         |
          ----------------------------------------
          |      Resolução     |  Tempo de conv. |
          ----------------------------------------
          |  9-Bit | 0.5 ºC    |       150ms     |
          | 10-Bit | 0.25 ºC   |       300ms     |
          | 11-Bit | 0.125 ºC  |       600ms     |
          | 12-Bit | 0.0625 ºC |      1200ms     |
          ---------------------------------------*/
      
      sensor.begin();
      delay(500);

      for (uint8_t i = 0; i < sensor.getDeviceCount(); i++) 
         {
           DeviceAddress addr;
           
           if (sensor.getAddress(addr, i)) 
             {
               sensor.setResolution(addr, 10);  // ou 9, 10, 11
             }
         }

      // -------------------- Configura rotas Web --------------------
      server.on("/", HTTP_GET, handleMenu);
      server.on("/upload", HTTP_GET, handleUpload);
      server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {}, handleUpdate);
      server.on("/telegram", HTTP_GET, handleRoot);
      server.on("/save", HTTP_POST, handleSave);
      server.on("/status", HTTP_GET, handlerStatusRede);
      server.on("/monitor", HTTP_GET, [](AsyncWebServerRequest *request) 
            {
              request->send(200, "text/html", monitorSerial);
            });
      server.on("/serial", HTTP_GET, [](AsyncWebServerRequest *request) 
            {
              request->send(200, "text/plain", getLogAsText());
            });

      ws.onEvent(onWebSocketEvent);
      server.addHandler(&ws);
      server.begin();

      // -------------------- Endereços dos sensores --------------------
      local_ender();

      // -------------------- Log Web --------------------
      logToWeb(" - Wi-Fi conectado: " + WiFi.localIP().toString() + ".\n\n");

      // -------------------- Criação das tasks que não dependem de RTC/NTP --------------------
      xTaskCreatePinnedToCore(baseTimer, "baseTimer", 2048, NULL, 0, NULL, 1);
      xTaskCreatePinnedToCore(readHtml, "readHtml", 4096, NULL, 2, NULL, 1);
      xTaskCreatePinnedToCore(lerBotoes, "lerBotoes", 2048, NULL, 3, NULL, 1);
      //xTaskCreatePinnedToCore(taskVerificaUpdate, "TaskVerificaUpdate", 4096, NULL, 4,  NULL, 1);

      // -------------------- RTC --------------------
      if (rtc.begin()) 
        {
          rtcOk = true;
        } 
        else 
          {
            Serial.println("Erro ao iniciar RTC");
          }

      // -------------------- Aguardando token/ID do Telegram --------------------
      lcd.clear();

      while (strlen(tokenBuffer) == 0 && strlen(idBuffer) == 0) 
           {
             Serial.println("Aguardando configuração do token e ID do bot...");

             if (lcd_token_id) lcd_token_id = false;

             lcd.setCursor(6, 0); lcd.print("ATENCAO!");
             lcd.setCursor(0, 1); lcd.print("INSIRA O TOKEN E ID");
             lcd.setCursor(0, 2); lcd.print("DO TELEGRAM.");
             lcd.setCursor(0, 3); lcd.print("AGUARDANDO");

             for (byte numPoints = 0; numPoints < 4; numPoints++) 
                {
                  lcd.setCursor(10, 3); lcd.print("    ");
                  lcd.setCursor(10, 3);

                  for (byte i = 0; i <= numPoints; i++) lcd.print(".");
                  delay(650);
                }

            delay(100);
           }

     for (int i = 0; i < 4; i++) lcd.createChar(i, spinnerChars[i]);

     // -------------------- Conecta com Telegram --------------------
     if (strlen(tokenBuffer) > 0 && strlen(idBuffer) > 0 && WiFi.status() == WL_CONNECTED) 
       {
         #ifdef ESP32
            client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
         #endif

         bot = new UniversalTelegramBot(tokenBuffer, client);
         enviarMensagem(conex_good);

         lcd.clear();
         lcd.setCursor(0, 0); lcd.print("AGUARDANDO RESPOSTA");
         lcd.setCursor(0, 1); lcd.print("DO TELEGRAM         ");

         byte frame = 0;
         unsigned long animStart = millis();
         const unsigned long animDuration = 10000;
         bool sucessoTelegram = false;

        while (millis() - animStart < animDuration) 
            {
              lcd.setCursor(0, 2); lcd.print("CARREGANDO ");
              lcd.write(frame);
              frame = (frame + 1) % 4;

              if (bot != nullptr) 
                {
                  sucessoTelegram = bot->sendMessage(idBuffer, "🔧 Iniciando sistema! Aguarde...", "");
                }

              if (sucessoTelegram) break;

              delay(1000);
              yield();
            }

        if (sucessoTelegram) 
          {
            lcd.setCursor(0, 2); lcd.print("TELEGRAM OK!        ");
            lcd.setCursor(0, 3); lcd.print("SISTEMA PRONTO.     ");
          } 
          else 
            {
              lcd.setCursor(0, 2); lcd.print("FALHA NA CONEXAO.   ");
              lcd.setCursor(0, 3); lcd.print("VERIFIQUE A REDE.   ");
              Serial.println("Falha ao conectar com Telegram.");
            }
       }

       // -------------------- Buzzer e boas-vindas --------------------
       a_buzzer(1, 1500, 500);
       lcd.clear(); 

       // -------------------- NTP e ajuste de hora --------------------
       ntp.begin();
       ntp.forceUpdate();

       if (set_time_cur == true) 
         {
           set_time_cur = false;

           Memory.begin("espaco_1", false);
           Memory.putBool("set_time_cur", set_time_cur);
           Memory.end();

           ntp.forceUpdate();
           unsigned long epochTime = ntp.getEpochTime();

           Serial.print("Hora do NTP: ");
           Serial.println(epochTime);           
           logToWeb("Epoch time (NTP): " + String(epochTime));

           DateTime ntpDateTime(epochTime);
           rtc.adjust(ntpDateTime);

           Serial.println("Horário do DS3231 ajustado com sucesso!");
           logToWeb("Horário do DS3231 ajustado com sucesso!");
         }

      // -------------------- Agora sim: cria a task_time --------------------
      xTaskCreatePinnedToCore(task_time, "task_time", 2048, NULL, 1, NULL, 1);

      // -------------------- Início da irrigação --------------------
      enviarMensagem(start_hidro);

      lcd.clear();
      lcd.setCursor(4, 1); lcd.print("BOM CULTIVO!");
      delay(2500);

      Serial.println("-------------------------- Fim do Setup -----------------------------");
    }    

void loop()
    {
      if (!initStatusLogged) 
        {
          logInitStatus();  // Função que faz todos os logs de inicialização
          initStatusLogged = true;  // Só roda uma vez
        }

      if (novaVersaoDisponivel) 
        {
          bot->sendMessage(idBuffer, mensagemUpdate, "Markdown");
          novaVersaoDisponivel = false;
        }
      
      //logToWeb(" - Entrei na função lcd_info()");
      /*Serial.println();
      Serial.print(F("Entrei na função lcd_info() ------------------------------------------------------------------------------------------------------------"));
      Serial.println();*/
      lcd_info();            // Função de imprime dados da temperatura no LCD 20X4.
      /*Serial.println();
      Serial.print("Saindo da função lcd_info() ------------------------------------------------------------------------------------------------------------");
      Serial.println();*/
      //logToWeb(" - Entrei na função lcd_info()\n");

      /*logToWeb(" - Entrei na função status_wifi()");
      Serial.println();
      Serial.print(F("Entrei na função status_wifi() ------------------------------------------------------------------------------------------------------------"));
      Serial.println();*/
      status_wifi();         // Função que verifica se há conexão fisica entre ESP32 e o dispositivo sem fio. 
      /*Serial.println();
      Serial.print(F("Saindo da função status_wifi() ------------------------------------------------------------------------------------------------------------"));
      Serial.println();
      logToWeb(" - Saindo da função status_wifi()\n");*/ 
      
      /*logToWeb(" - Entrei na função mills_newmess()");      
      Serial.println();
      Serial.print(F("Entrei na função mills_newmess() ------------------------------------------------------------------------------------------------------------"));
      Serial.println();*/ 
      mills_newmess();       // Função que verifica se está chegando algum comando do Telegram.
      //tratarComandosTelegram();
      /*Serial.println();
      Serial.print(F("Saindo da função mills_newmess() ------------------------------------------------------------------------------------------------------------"));
      Serial.println();
      Serial.println();
      logToWeb(" - Saindo da função mills_newmess()\n");*/

      verificarAtualizacao(); // Verifica se há atualizações de firmware.
      
      if (on_off_geral == true) 
        {
          Serial.println("######################################################## INÍCIO DO LOOP 1 #######################################################"); 

          /*logToWeb(" - Entrei na função blackout()"); 
          Serial.println();
          Serial.print(F("Entrei na função blackout() ------------------------------------------------------------------------------------------------------------"));
          Serial.println();*/
          blackout();
          /*Serial.println();
          Serial.print(F("Saindo da função blackout() ------------------------------------------------------------------------------------------------------------"));
          Serial.println();
          logToWeb(" - Saindo da função blackout()\n");*/                   

          /*logToWeb(" - Entrei na função seca_reserva()");
          Serial.println();
          Serial.print("Entrei na função seca_reserva() ------------------------------------------------------------------------------------------------------------");
          Serial.println();*/      
          seca_reserva();        // Função que seca o recipiente manualamente.
          /*Serial.println();
          Serial.print("Saindo da função seca_reserva() ------------------------------------------------------------------------------------------------------------");
          Serial.println();
          logToWeb(" - Saindo da função seca_reserva()\n");*/

          /*logToWeb(" - Entrei na função reseta_ESP()");
          Serial.println();
          Serial.print("Entrei na função reseta_ESP() ------------------------------------------------------------------------------------------------------------");
          Serial.println();*/
          reseta_ESP();          // Função que reinicia o sistema.LCD 20X4.
          /*Serial.println();
          Serial.print("Saindo da função reseta_ESP() ------------------------------------------------------------------------------------------------------------");
          Serial.println();
          logToWeb(" - Saindo da função reseta_ESP()\n");*/

          /*logToWeb(" - Entrei na função black_light_activ()");
          Serial.println();
          Serial.print("Entrei na função black_light_activ() ------------------------------------------------------------------------------------------------------------");
          Serial.println();*/
          black_light_activ();   // Função que ativa ou desativa temporizador do backlight.  
          /*Serial.println();
          Serial.print("Saindo da função black_light_activ() ------------------------------------------------------------------------------------------------------------");
          Serial.println();
          logToWeb(" - Saindo da função black_light_activ()\n");*/

          /*logToWeb(" - Entrei na função falha_sensor()");
          Serial.println();
          Serial.print("Entrei na função falha_sensor() ------------------------------------------------------------------------------------------------------------");
          Serial.println();*/
          falha_sensor();        // Função de verifica a temperatatura dos sensores para emitir um alerta. 
          /*Serial.println();
          Serial.print("Saindo da função falha_sensor() ------------------------------------------------------------------------------------------------------------");
          Serial.println();
          logToWeb(" - Saindo da função falha_sensor()\n");
          Serial.println();*/

          pressButton();
          blinkIpTelegram();

          if (aux_stat_bomba == true)  // Condição que verifica se houve que de energia ou reset no modo manual no momento da irrigação.
            {
              aux_stat_bomba = false;

              digitalWrite(rele_irriga, HIGH);
              digitalWrite(led_stat_rele, HIGH);
            }              

            digitalWrite(led_ciclo_15X45, led_verao);
            digitalWrite(led_ciclo_15X30, led_inver);
            
            cont0 = 0;  //  Zera o contador 

          if (flag_init_backlight == true)
            {
              flag_init_backlight = false;
              lcd.init(); 
            }               

            force_boot();                 

          if (flag_back_pow_1 == true)  //  Condição guardado na memoria para retorno da energia, ESVAZIAR.
            {  
                digitalWrite(rele_reser_2, dry);
                digitalWrite(led_stat_rele, dry);  
      
                flag_back_pow_1 = false;

                Memory.begin("espaco_1", false); // Abre a memória para gravação.                
                Memory.putBool("dry", dry);
                Memory.putBool("flag_back_pow_1", flag_back_pow_1);                    
                Memory.end();                       
            }

          if (flag_back_pow_2 == true)  //  Condição guardado na memoria para retorno da energia, ABRE VÁLVULA!
            {  
                digitalWrite(rele_val, flag_sim_abra);
                digitalWrite(led_stat_rele, flag_sim_abra);  
      
                flag_bac_pwr_2 = false;

                Memory.begin("espaco_1", false); // Abre a memória para gravação.                
                Memory.putBool("flag_sim_abra", flag_sim_abra);
                Memory.putBool("flag_bac_pwr_2", flag_bac_pwr_2);                    
                Memory.end();                       
            }
            
          if (recall_auto == true)
            {
              recall_auto = false;              
              modo_trab = true;
              modo_auto = true;
              modo_manual = false; // Indica ao comando "/status_cmd" o status atual.
              modo_work = false;   // Reativa o controle automatico.
              flag_count_auto = false; // Desativa contador.
              counter_auto = 0;              

              Memory.begin("espaco_1", false); // Abre a memória para gravação. 
              Memory.putBool("modo_trab", modo_trab);
              Memory.putBool("modo_auto", modo_auto);
              Memory.putBool("modo_manual", modo_manual);
              Memory.putBool("modo_work", modo_work);
              Memory.putBool("modo_work", modo_work);
              Memory.putBool("flag_count_auto", flag_count_auto);
              Memory.end(); 

              digitalWrite(led_stat_rele, LOW);  

              if (on_off_all_bip == true) a_buzzer(3, 200, 200);

              string_dinamica(back_mod_auto);              

              //bot->sendMessage(idBuffer, F("Pronto! Ativado."), "");   
              enviarMensagem("Pronto! Ativado.");                                                                
            }             
          
          if (veryf_boias == true)
            { 
                conter_boia = 0;  // Zera contador que verificar boias.
                veryf_boias = false; 
                
                if (((digitalRead(boia_baixa) == LOW) && (digitalRead(boia_alta) == LOW)) && flag_pos_fill == true) // Verifica nivel de enchimento parcial.
                  {  
                    desat_seca = false;  // Avisa para a função seca reserva que o reservatorio está vazio. 
                    
                    flag_sim_abra = false; 
                    no_pronto = true; 
                                  
                    block_empty = true;  // Reativa lcd_info. 
                    
                    digitalWrite(rele_val, flag_sim_abra);
                    digitalWrite(led_stat_rele, flag_sim_abra);

                    flag_repor = false;  // Ativa status do "/status comand".

                    Memory.begin("espaco_1",false);                             
                    Memory.putBool("block_empty", block_empty);               
                    Memory.putBool("flag_sim_abra", flag_sim_abra);
                    Memory.putBool("no_pronto", no_pronto);  
                    Memory.end();             

                    lcd.clear();

                    wait_nutri();
                  }
                
                if (((digitalRead(boia_baixa) == HIGH) && (digitalRead(boia_alta) == HIGH)) || (fill_reser == true)) // Verifica se o reservatório está vazio.
                  {   
                    fill_reser = true; // Garante que o sistema perceba que o reservatiro precisa ser enchido.

                    if ((repor_aux_1 == false) && (repor_aux_2 == false)) level_crit_boia = true;
                    
                    counter_dry = 0;  // Zera contado do dreno do reservatório.   

                    dry = false;

                    desat_seca = false;  // Avisa para a função seca reserva que o reservatorio está vazio. 

                    aux_dec_repor = true;  // Flag que mostra ao comando /repor_agua a situação que foi drenado o reservatório.              

                    block_empty = true;  // Libera o lcd info.       

                    block_lcd_info = false; // Reabilita o lcd info.

                    if (block_flash_0 == true)  // Permite a gravar na flash uma unica vez.
                      {
                        block_flash_0 = false;
                        
                        Memory.begin("espaco_1", false);  // Abre a memória para gravação.                  
                        Memory.putInt("counter_dry", counter_dry);
                        Memory.putBool("block_empty", block_empty );
                        Memory.putBool("block_lcd_info", block_lcd_info);  
                        Memory.putBool("aux_dec_repor", aux_dec_repor); 
                        Memory.putBool("fill_reser", fill_reser);                   
                        Memory.end();
                      }

                      digitalWrite(rele_reser_2, dry);  // Garante que a bomba secadora esteja desligada.
                      digitalWrite(led_stat_rele, LOW);
                      //trava_blink = true;  // Flag que ativa todos os leds piscante indicando reservátorio nivel baixo.
                      trava_cont_hora = true;  // Flag que ativa o aviso de hora em hora.

                      if (on_off_all_bip == true) a_buzzer(3, 200, 200);

                    if (flag) 
                      { 
                        if (repor_aux_1 == true)
                          {  
                            flag_decisao = false; 

                            string_dinamica(rese_crit_0);                                
                            
                            repor_aux_2 = true;    

                            Memory.begin("espaco_1", false);                                 
                            Memory.putBool("repor_aux_2", repor_aux_2);
                            Memory.putBool("memo_lcd", memo_lcd);
                            Memory.putBool("flag_decisao", flag_decisao);
                            Memory.end();                     

                            repor_aux_2 = false;
                          }

                        if (repor_aux_2 == true) // Só entra aqui na reinicialização.
                          {   
                            string_dinamica(rese_crit_1); 

                            no_pronto = false;
                          }    

                        if (level_crit_boia == true)
                          {
                            string_dinamica(rese_crit_2);

                            Memory.begin("espaco_1", false);
                            Memory.putBool("level_crit_boia", level_crit_boia);
                            Memory.end();                                                 
                          }                    

                          Memory.begin("espaco_1", false);                          
                          Memory.putBool("dry", dry);
                          Memory.end();                          

                        while (digitalRead(boia_alta) == HIGH) // Laço esperando decisão de repor a água, esperando pelo comando "repor_agua".
                             { 
                              if (dry == true) flag_wait_oper = false;  // Destiva contador para para alterenar texto entre lcd info e aviso de reservatório baixo.                         
                              else flag_wait_oper = true;  // Ativa contador para para alterenar texto entre lcd info e aviso de reservatório baixo. 

                                if (digitalRead(rele_irriga) == LOW) //  Verifica se a bomba de irrigação está desligada.   
                                  {  
                                  off_irrig_15X15();
                                  off_irrig_15X45();                            
                                  }                          
                              
                                if (switch_LCD == false)
                                  {
                                  if (wait_laco_0 == false)                              
                                    {
                                      wait_laco_0 = true; // Trava a condição acima.
                                      lcd.clear(); 
                                      wait_laco_1 = true;
                                      block_empty = true;
                                    }  

                                    lcd_info();  // Mostra status da temperturas.
                                    status_wifi(); // Mostra status do wifi.
                                  }

                                if (switch_LCD == true)
                                  {
                                  if (wait_laco_1 == true)                              
                                    {
                                      wait_laco_1 = false; // Trava a condição acima. 
                                      lcd.clear();
                                      wait_laco_0 = false; // Libera a condição acima.
                                    }                                
                                  
                                  if (repor_aux_1 == true)
                                    { 
                                      lcd.setCursor(6, 0);
                                      lcd.print("ATENCAO!");

                                      lcd.setCursor(0, 2);
                                      lcd.print("O RESERVATORIO FOI");

                                      lcd.setCursor(0, 3);
                                      lcd.print("DRENADO COM SUCESSO"); 

                                      // Nesse ponto o dispositivo foi desconectado da rede eletrica.
                                    }

                                  if (repor_aux_2 == true) // Só entra aqui na reinicialização.
                                    { 
                                      lcd.setCursor(6, 0);
                                      lcd.print("ATENCAO!");

                                      lcd.setCursor(0, 1);
                                      lcd.print("REINICIANDO APOS UMA");

                                      //lcd.setCursor(0, 2);
                                      //lcd.print("DRENAGEM. AGORA REI-"); 

                                      lcd.setCursor(0, 2);
                                      lcd.print("DRENAGEM. REPONHA A"); 

                                      lcd.setCursor(0, 3);
                                      lcd.print("AGUA E INICIALIZE..."); 
                                    } 
                                    
                                  if (level_crit_boia == true) 
                                    {
                                      lcd.setCursor(6, 0);
                                      lcd.print("ATENCAO!");

                                      lcd.setCursor(0, 1);
                                      lcd.print("RESERVAT. NO NIVEL");

                                      lcd.setCursor(0, 2);
                                      lcd.print("CRITICO! FIQUE CALMO");
                    
                                      lcd.setCursor(0, 3);
                                      lcd.print("USANDO A RESERVA!");
                                    } 

                                    wait_laco_0 = false;
                                  }

                                if (forceBoot_1 == true) // Condição que avalia se houve arrependiemnto na limpeza ou manutenção.
                                  {
                                    forceBoot_1 = false;
                                    
                                    enviarMensagem("OK! Reiniciando agora, aguarde...");
                                    //bot->sendMessage(idBuffer, F("OK! Reiniciando agora, aguarde..."), "");   

                                    //delay(550);

                                    String aux_tyr;

                                    for (byte tyr = 0; tyr < 3; tyr ++)
                                      {
                                        aux_tyr = String(tyr);

                                        enviarMensagem(aux_tyr.c_str());   
                                        logToWeb(aux_tyr);  

                                        delay(250);
                                      }

                                   // delay(1250);

                                    lcd.clear();        

                                    lcd.setCursor(0,3);
                                    lcd.print("REINICIANDO");

                                    digitalWrite(led_stat_erro_func, LOW);
                                    delay(1250);   

                                    for (byte points = 11; points < 16; points ++) // Cria o efeito dos pontos (........).
                                       {                   
                                         lcd.setCursor(points, 3);
                                         lcd.print(".");

                                         delay(250);

                                         if (points == 15) ESP.restart();               
                                       }
                                      
                                      //ESP.restart();
                                  } 
                              
                                  mills_newmess(); // Verificando se algum comando foi digitado, esperando confirmação operador.

                                  trava_blink = true;  // Ativa o led blink alertando o operador pela decisão de repor a água. 

                                    if ((T_horario >= "06:00:00") && (T_horario <= "17:59:59")) // Controla o intervalo de hora das 7 a 18, aguardando reposição da agua do reservátorio.
                                      {
                                        if (repor == false)
                                          {
                                            on_irrig_15X15();   // Função responsável pelo ciclo de irrigação 15X15 minutos. 
                                            off_irrig_15X15();  // Função responsável por encerrar irrigação intervalo de 15 por 15 minutos.
                                          }
                                      } 
                    
                                    if (((T_horario >= "18:00:00") && (T_horario <= "23:59:59")) || ((T_horario >= "00:00:00") && (T_horario <= "05:59:59"))) // Controla a irrigação em 15 minutos ligado e 45 minutos desligado.
                                      {
                                        if (repor == false)
                                          {
                                            on_irrig_15X45();   // Função responsável pelo ciclo de irrigação 15X45minutos. 
                                            off_irrig_15X45();  // Função responsável por encerrar irrigação intervalo de 15 por 45 minutos.
                                          }
                                      }                                         
                                                           

                                  black_light_activ(); // Função que acende o backlight por tempo determinado.      
                                  
                              while ((digitalRead(boia_alta) == HIGH) && flag_decisao == true)  // Aguardando o recipiente encher.
                                   { 
                                     if ((flag_decisao == true) && (on_valvu == true)) // Religa a erposição após cair a energia ou um reset. 
                                       {
                                         digitalWrite(rele_val, on_valvu);  
                                         digitalWrite(led_stat_rele, on_valvu);
                                       } 
                                                                            
                                       flag_wait_oper = false; // Desativa contador do laço de resposta do operador.

                                       counter_wait_oper = 0; // Zera contador que alterna texto do lcd info e aviso de reservatório com nivel baixo.

                                      if (flag_encher_lcd == true)
                                        {
                                          flag_encher_lcd = false;

                                          lcd.clear();
                                        } 

                                        wait_laco_0 = false; // Volta as flags para condição inicial.
                                        wait_laco_1 = true;  // Volta as flags para condição inicial.                              

                                        lcd.setCursor(5,0);
                                        lcd.print("EXCELENTE!");

                                        lcd.setCursor(0,1);                               
                                        lcd.print("A AGUA DO RESERV.");
                                        lcd.setCursor(0,2);
                                        lcd.print("ESTA SENDO REPOSTA");  //esta sendo reposta

                                        lcd.setCursor(0,3);
                                        lcd.print("POR FAVOR AGUARDE...");  //esta sendo reposta

                                        mills_newmess(); // Verificando se algum comando foi digitado, esperando confirmação operador.                                     

                                      if (digitalRead(boia_alta) == LOW)  // Verifica se o reservatório encheu por completo.
                                        {
                                          digitalWrite(rele_val, LOW);
                                          //digitalWrite(led_stat_rele, LOW);

                                          no_pronto = true;  // Volta a permitir o usar do comando /pronto enquanto repõe a água.
                                            
                                          //counter_flag_wait_0 = true;
                                          //counter_flag_wait_1 = false;

                                          flag_repor = false; // Desativa no status comando.

                                          flag_wait_0 = true;
                                          flag_wait_1 = false;
                                          lcd_wait_nutri = true;
                                            
                                          wait_nutri(); // Função responsável verificarr se a nutrição foi respota.

                                          level_crit_boia = false;
                                          aux_dec_repor = false;

                                          //critic_level = true;  // Reativa mensagem de nivel critico

                                          repor_aux_1 = false;
                                          repor_aux_2 = false;

                                          flag_decisao = false;
                                          on_valvu  = false;
                                            
                                          trava_cont_hora = false;  // Desativa contador da função "cont_hora()". 
                                          flag_conta_hora = false;  // Desativa função "cont_hora()".
                                          counter_hora = 0; 

                                          block_empty = true; // Reativa LCD info.                                           
                                          modo_work = false; // Reativa irrigação após encher  o reservatorio e inserir nutrientes.  

                                          Memory.begin("espaco_1", false);                                    
                                          Memory.putBool("on_valvu", on_valvu);
                                          Memory.putBool("modo_work", modo_work);
                                          Memory.putBool("fill_reser", fill_reser);
                                          Memory.putBool("level_crit_boia", level_crit_boia);
                                          Memory.end();
                                        }                                                               
                                    } 
                                    cont_hora();
                             }
                            
                             flag = false;   
                             dry = false;
                      } 
                      else flag = true;                    
                          
                      flag = true;  
                  }
            }                      

          if (modo_work == false) 
            {
                if ((T_horario >= "06:00:00") && (T_horario <= "17:59:59"))  // Controla o intervalo de hora das 7 a 18.             
                  {
                    if (flag_trava) 
                      {
                        on_irrig_15X15();
                        off_irrig_15X15();
                      }
                  }

                if (((T_horario >= "18:00:00") && (T_horario <= "23:59:59")) || ((T_horario >= "00:00:00") && (T_horario <= "05:59:59")))  // Controla a irrigação em 15 minutos ligado e 45 minutos desligado.
                  {
                    if (flag_trava) 
                      {                       
                        scheduled = true;  // Reativa o comando "modi_ciclo".
                                                        
                        on_irrig_15X45();   // Função responsável por ligar ciclo de irrigação 15X45 minutos.
                        off_irrig_15X45();  // Função responsável por desligar o ciclo de irrigação 15X45 minutos.

                        if (flag_rec_flash_1 == true)  // Faz com que grave uma unica vez na flash.
                          {
                            flag_rec_flash_1 = false;                               
                          }                          
                      }
                  }
                  
                  block_all_sys = true;           
                      
            }// Fim da condição que verfica o bloqueio total do sistema

          if (back_parci_lcd == true)
            {
                back_parci_lcd = false;

                Memory.begin("espaco_1", false);
                Memory.putBool("back_parci_lcd", back_parci_lcd);
                Memory.end();

                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print(line_0);

                lcd.setCursor(0, 1);
                lcd.print(line_1);
                lcd.setCursor(0, 2);
                lcd.print(line_2);

                lcd.setCursor(0, 3);
                lcd.print(line_3);
            }

          if (dry_lcd == true)
            {              
                dry_lcd = false; 

                Memory.begin("espaco_1", false); 
                Memory.putBool("dry_lcd", dry_lcd);
                Memory.end();            

                lcd.clear();

                lcd.setCursor(6, 0);
                lcd.print(dry_line_0);

                lcd.setCursor(0, 1);
                lcd.print(dry_line_1);

                lcd.setCursor(0, 2);
                lcd.print(dry_line_2);

                lcd.setCursor(0, 3);
                lcd.print(dry_line_3);
            }

            Serial.println("########################################################## FIM DO LOOP 1 ########################################################");
            Serial.println(" ");
        }        
        
      if (on_off_geral == false) 
        {
          Serial.println("INICIO DO LOOP 2 !---------------------------------------");           

          if (block_all_sys == true)
            {
              block_all_sys = false;
              lcd.clear();
            }
              
            lcd.backlight(); // Acende backlight.

            on_backlig = true;  // Ativa o contador.                
                        
            lcd.setCursor(2, 0);
            lcd.print("SISTEMA PAUSADO!"); 

            lcd.setCursor(2, 2);
            lcd.print("TODAS AS FUNCOES");
                        
            lcd.setCursor(3, 3);
            lcd.print("ESTAO PAUSADAS"); 

            all_led_on_off(LOW);      // Função que acende ou apaga todos os leds.
              
            delay(2000);

            all_led_on_off(HIGH);      // Função que acende ou apaga todos os leds. 

            if (cont_buzz == 300)
              {
                if (on_off_all_bip == true) a_buzzer(1, 2000, 0);

                cont_buzz = 0;
              }

              Serial.println("FIM DO LOOP 2 !---------------------------------------");                 
        } 
    } // Fim de loop

//-------------------------------------------REPOSITÓRIO DAS FUNÇÕES -------------------------------------------

void status_wifi()      // Função destatus do Wi-Fi. 
    {
      if (WiFi.status() != WL_CONNECTED) 
        { // Verifica se a WiFi está desconectada
          blinkWifiLED(); // Ativa o piscar do LED WiFi

          if (flag_alert_wifi_lcd) 
            { // Se ainda não foi exibido o alerta no LCD
              flag_alert_wifi_lcd = false;
              block_lcd_info = true;
            }

            flag_clear_IP = true; // Marca para limpar o IP no LCD

            if (block_empty) 
              { // Exibe mensagem de conexão perdida no LCD
                lcd.setCursor(0, 3);
                lcd.print(MSG_IP_PREFIX);

                lcd.setCursor(4, 3);
                lcd.print(MSG_NO_CONNECTION);
              }

            if (flag_bip_wifi) 
              { // Emite alerta sonoro de falta de conexão
                soundAlertBuzzer();
              }
        } 
        else 
          { // WiFi conectada
            if (flag_clear_IP) 
              { // Limpa o LCD se necessário
                clearIpInfoOnLCD();
              }

            if (block_empty) 
              { // Exibe informações do IP no LCD
                lcd_info(); // Supondo que esta função exiba outras informações no LCD
                displayIpInfoOnLCD();
              }
        }
    }

void blinkWifiLED()      // Função que pisca todos os leds. 
    {
      flag_pisca_wifi = true;
    }

void soundAlertBuzzer()      // Função que aciona buzzer.
    {
      for (byte i = 0; i < 12; i++) 
         { // Loop do buzzer sem conexão física ao dispositivo
           digitalWrite(buzzer, !digitalRead(buzzer)); // Alterna o estado do buzzer
           delay(BUZZER_DELAY_MS);
         }

         flag_bip_wifi = false; // Desativa o alerta sonoro após tocar
    }

void clearIpInfoOnLCD()      // Função que limpa o LCD.
    {
      lcd.clear();
      flag_clear_IP = false;
    }

void displayIpInfoOnLCD()      // Função responsável por mostar IP no LCD.
    {
      lcd.setCursor(0, 3);
      lcd.print(MSG_IP_PREFIX);

      lcd.setCursor(4, 3);
      lcd.print(WiFi.localIP());

      flag_pisca_wifi = false;
      counter_timeout = 0;
    }

void on_irrig_15X15()      // Função responsável por iniciar irrigação intervalo de 15 em 15 minutos.
    {
      if (repor == false)
        { 
          flag_15X45 = false;

          if (((T_minuto == aux1_minut_on) || (T_minuto == aux2_minut_on)) && (trava_15X15_on == true)) // condição que liga em minuto 00 e 30.
            { 
              bomb_on = true;
              
              trava_15X15_on = false;
              trava_15X15_off = true;                            
              
              stat_on_irrig = !stat_on_irrig;  // Ativa e desativa a irrigação não controlada.

              lock_seca_rese = true; // Ativa seca reservatório.

              Memory.begin("espaco_1", false); // Abre a memória para gravação. // Abre a memória para gravação.
              Memory.putBool("trava_15X15_on", trava_15X15_on);
              Memory.putBool("trava_15X15_off", trava_15X15_off);
              Memory.putBool("lock_seca_rese", lock_seca_rese);
              Memory.putBool("bomb_on", bomb_on);            
              Memory.end(); 

              on_flag_blackout();  // Grava o status da irrigação (ligada ou desligada antes da queda de energia).        

              if (WiFi.status() == WL_CONNECTED)
                {
                  char mensagem[64];
                  snprintf(mensagem, sizeof(mensagem), "Ligou no horário: %s\n", T_horario);
                  enviarMensagem(mensagem); 

                  //bot->sendMessage(idBuffer, String(F("Ligou no horário: ")) + T_horario + "\n", "");
                  logToWeb(" - Ligou no horário: " + String(T_horario) + "\n");

                } else
                    {
                      logToWeb(" - Ligou no horário: " + String(T_horario) + "\n");
                      Serial.println(" - Ligou no horário: " + String(T_horario) + "\n");
                    }  
                                          
              digitalWrite(rele_irriga, HIGH);  // Liga bomba para irrigação.                
              digitalWrite(led_stat_rele, HIGH);  // Liga led status do rele bomba. 
              
              off_sound = true; // Ativa aviso sonoro após desligamento.
              
              if ((on_off_bip_irri == true) && (on_off_all_bip == true)) a_buzzer(1, 500, 500); 

              lcd.createChar(0, sml_aleg); 
              lcd.setCursor(19, 3);          
              lcd.write(0);                                    
            }
        }
    }

void off_irrig_15X15()      // Função responsável por encerrar irrigação intervalo de 15 em 15 minutos.
    {
      if (repor == false)
        {
          if (((T_minuto == aux3_minut_off) || (T_minuto == aux4_minut_off)) && (trava_15X15_off == true))
            {
              bomb_on = false;
              trava_15X15_off = false;
              trava_15X15_on = true; 
              
              Memory.begin("espaco_1", false); // Abre a memória para gravação.
              Memory.putBool("trava_15X15_on", trava_15X15_on);
              Memory.putBool("trava_15X15_off", trava_15X15_off);  
              Memory.putBool("flag_blackout", flag_blackout);              
              Memory.putBool("bomb_on", bomb_on);
              Memory.end(); 

              off_flag_blackout();// Grava o status da irrigação (ligada ou desligada antes da queda de energia). 

              if (WiFi.status() == WL_CONNECTED)
                {
                  char mensagem[64];
                  snprintf(mensagem, sizeof(mensagem), "Ligou no horário: %s\n", T_horario);
                  enviarMensagem(mensagem);
                  //bot->sendMessage(idBuffer, String(F("Desligou no horário: ")) + T_horario + "\n", "");
                  logToWeb(" - Desligou no horário: " + String(T_horario) + "\n");

                } else
                    {
                      logToWeb(" - Desligou no horário: " + String(T_horario) + "\n");
                      Serial.println(" - Desligou no horário: " + String(T_horario) + "\n");
                    }  

              digitalWrite(rele_irriga, LOW);            
              digitalWrite(led_stat_rele, LOW);

              //off_sound = true; // Ativa aviso sonoro após desligamento.

              if ((on_off_bip_irri == true) && (on_off_all_bip == true)) a_buzzer(2, 500, 500);

              lcd.createChar(0, sml_tris);  
              lcd.setCursor(19, 3);            
              lcd.write(0);         
            } 
        }     
    }

void on_irrig_15X45()      // Função responsável por iniciar irrigação intervalo de 15 por 45 minutos.
    {
      if (repor == false)
        {
          if ((T_minuto == minuto[0]) && (trava_15X45_on == true))
            {
              stat_on_irrig = !stat_on_irrig;  // Ativa e desativa a irrigação não controlada.

              bomb_on = true;              

              trava_15X45_on = false;
              trava_15X45_off = true;
              
              flag_15X15 = false;
              flag_10X20 = false;

              flag_stat_cic30 = false; 

              lock_seca_rese = true; //  Ativa seca reservatório

              digitalWrite(rele_irriga, HIGH);  // Liga bomba para irrigação.                
              digitalWrite(led_stat_rele, HIGH); 
              //digitalWrite(led_ciclo_15X45, HIGH);

              Memory.begin("espaco_1", false); // Abre a memória para gravação.              
              Memory.putBool("trava_15X45_on", trava_15X45_on);
              Memory.putBool("trava_15X45_off", trava_15X45_off); 
              Memory.putBool("lock_seca_rese", lock_seca_rese); 
              Memory.putBool("bomb_on", bomb_on);         
              Memory.end(); 

              on_flag_blackout();  // Grava o status da irrigação (ligada ou desligada antes da queda de energia).                             
                        
              if (WiFi.status() == WL_CONNECTED)
                {
                  char mensagem[64];
                  snprintf(mensagem, sizeof(mensagem), "Ligou no horário: %s\n", T_horario);
                  enviarMensagem(mensagem); 
                  
                  //bot->sendMessage(idBuffer, String(F(, "");
                  logToWeb(" - Ligou no horário: " + String(T_horario) + "\n");

                } else
                    {
                      char mensagem[64];
                      snprintf(mensagem, sizeof(mensagem), "Ligou no horário: %s\n", T_horario);
                      enviarMensagem(mensagem);
                      
                      logToWeb(" - Ligou no horário: " + String(T_horario) + "\n");
                      Serial.println(" - Ligou no horário: " + String(T_horario) + "\n");
                    }  
                                          
              digitalWrite(rele_irriga, HIGH);  // Liga bomba para irrigação.                
              digitalWrite(led_stat_rele, HIGH);              
              
              lcd.createChar(0, sml_aleg);  
              lcd.setCursor(19, 3);           
              lcd.write(0);

              if ((on_off_bip_irri == true) && (on_off_all_bip == true)) a_buzzer(1, 500, 500); 
            }
        }
    }

void off_irrig_15X45()      // Função responsável por encerrar irrigação intervalo de 15 por 45 minutos.
    {
      if (repor == false)
        {
          if ((T_minuto == aux5_minut_off) && (trava_15X45_off == true)) 
            {
              bomb_on = false;
              trava_15X45_off = false;
              trava_15X45_on = true; 

              Memory.begin("espaco_1", false); // Abre a memória para gravação.
              Memory.putBool("trava_15X45_off", trava_15X45_off);
              Memory.putBool("trava_15X45_on", trava_15X45_on);     
              Memory.putBool("bomb_on", bomb_on);         
              Memory.end();

              off_flag_blackout();  // Grava o status da irrigação (ligada ou desligada antes da queda de energia). 

              if (WiFi.status() == WL_CONNECTED)
                {
                  char mensagem[64];
                  snprintf(mensagem, sizeof(mensagem), "Ligou no horário: %s\n", T_horario);
                  enviarMensagem(mensagem);
                  
                  //bot->sendMessage(idBuffer, String(F("Desligou no horário: ")) + T_horario + "\n", "");
                  logToWeb(" - Desligou no horário: " + String(T_horario) + "\n");

                } else 
                    {
                      logToWeb(" - Desligou no horário: " + String(T_horario) + "\n");
                      Serial.println(" - Desligou no horário: " + String(T_horario) + "\n");
                    }

              digitalWrite(rele_irriga, LOW);  
              digitalWrite(led_stat_rele, LOW);
              
              lcd.createChar(0, sml_tris);  
              lcd.setCursor(19, 3);            
              lcd.write(0);
              
              if ((on_off_bip_irri == true) && (on_off_all_bip == true)) a_buzzer(2, 500, 500);                     
                                            
            }  
        }
    }

void mills_newmess()      // Função de tempo, verifica se chegou algum comando do Telegram.
    {
      // Verifica se já passou o tempo necessário desde a última execução
      if (millis() - lastTimeBotRan > botRequestDelay)  
        {    
          if (bot == nullptr) 
            {
              Serial.println("Bot ainda não foi inicializado.");
              logToWeb(" - Bot ainda não foi inicializado.");

              return; // Sai da função se o bot não foi inicializado      
            }

          int numNewMessages = bot->getUpdates(bot->last_message_received + 1);        

          if (numNewMessages > 0) 
            {
              Serial.println("Obtendo resposta");      
              handleNewMessages(numNewMessages);  // Passa o número de novas mensagens      
              lastTimeBotRan = millis();
            }
        }       
    }

void handleNewMessages(int numNewMessages)      // Função responsável por verificar novos comando Enviados pelo Telegram.
    { 
      Serial.println("handleNewMessages");
      Serial.println(String(numNewMessages));

      //logToWeb(String(numNewMessages) + "\n");

      for (int i = 0; i < numNewMessages; i++) 
        {
          // ID de bate-papo do solicitante
          chat_id = String(bot->messages[i].chat_id);

          // Verificação de segurança
          if (chat_id != idBuffer)
            {
              // Evita flood de mensagens para desconhecidos
              static unsigned long lastUnauthorized = 0;
              if (millis() - lastUnauthorized > 10000) // só responde a cada 10s
                {
                  enviarMensagem("❌ Usuário não autorizado.");
                  
                  //bot->sendMessage(chat_id, F(), "");
                  lastUnauthorized = millis();
                }
                continue;
            }
        
          text = bot->messages[i].text;
          Serial.println(text);
          logToWeb(" - " + text + "\n");
         
          from_name = bot->messages[i].from_name;        

          if (text.equals("/start"))
            {
              char situacao_dia[15];

              if ((T_horario >= "06:00:00") && (T_horario <= "11:59:59"))
                {
                  strcpy(situacao_dia, "Bom dia ");
                }
                else if ((T_horario >= "12:00:00") && (T_horario <= "17:59:59"))
                    {
                      strcpy(situacao_dia, "Boa tarde ");
                    }
                    else
                      {
                        strcpy(situacao_dia, "Boa noite ");
                      }

              // Convertendo from_name para const char*
              const char* from_name_char = from_name.c_str();

              // Monta a mensagem final
              int tamanhoTotal = strlen(situacao_dia) + strlen("e bem vindo, ") + strlen(from_name_char) + strlen(" ao controle SCHidro para hidropônia.\n\n") +
                                 strlen("Através do Telegram você poderá controlar e obter status do sistema através de comandos. \n\n") +
                                 strlen("Para conhecer os comandos toque ou digite:    /comandos.\n\n") + 1;

              char* Bem_vindo = (char*)malloc(tamanhoTotal);

              if (Bem_vindo == nullptr)
                {
                  return;
                }

              strcpy(Bem_vindo, situacao_dia);
              strcat(Bem_vindo, "e bem vindo, ");
              strcat(Bem_vindo, from_name_char); // Usando a variável convertida
              strcat(Bem_vindo, " ao controle SCHidro para hidropônia.\n\n");
              strcat(Bem_vindo, "Através do Telegram você poderá controlar e obter status do sistema através de comandos. \n\n");
              strcat(Bem_vindo, "Para conhecer os comandos toque ou digite:    /comandos.\n\n");

              //bot->sendMessage(idBuffer, , "");
              enviarMensagem(Bem_vindo);

              free(Bem_vindo);
            }

          else if (text.equals("/comandos")) // comando ok
              {
                if (block_cmd_0 == true) // Reativa os comandos. 
                  { 
                    string_dinamica(Command_0); 

                    cont_cmd = true;
                    
                  } else enviarMensagem(cmd_invalid);//bot->sendMessage(idBuffer, cmd_invalid, "");
              }           

          else if (text.equals("/continue"))
              {
                if (block_cmd_0 == true) // Reativa os comandos. 
                  {                 
                    if (cont_cmd == true)
                      {
                        cont_cmd = false;

                        string_dinamica(Command_1);

                      } else string_dinamica(txt_cont_cmd); 

                  } else enviarMensagem(cmd_invalid);//bot->sendMessage(idBuffer, cmd_invalid, "");                            
              } 

          else if (text.equals("/pausa_sistema"))// comando ok
              {
                if (block_cmd_0 == true)  
                  {                
                    if (digitalRead(rele_irriga) == LOW) //  Verifica se a irrigação está desligada.
                      {
                        on_off = !on_off;                        

                        if (on_off == false) 
                          {
                            lcd.backlight(); // Acende backlight.

                            on_off_geral = false; // Quem vai desativar todo os outros comandos na pausa do sistema.

                            on_backlig = true;  // Ativa o contador. 
                                
                            block_lcd_info = false; // Bloqueia função lcd_info.

                            block_full = false;  // Bloqueio secundário que bloqueia a função lcd_info.
                                
                            lcd.clear();
                            lcd.setCursor(0, 0);
                            lcd.print("SISTEMA DESATIVADO"); 

                            lcd.setCursor(2, 2);
                            lcd.print("TODAS AS FUNCOES");
                                
                            lcd.setCursor(2, 3);
                            lcd.print("ESTAO DESATIVADAS");

                            string_dinamica(block_sist); 

                            digitalWrite(led_stat_func, LOW);
                            digitalWrite(led_stat_erro_func, LOW);
                            digitalWrite(rele_irriga, LOW);
                            digitalWrite(led_stat_rele, LOW);
                          }

                        if (on_off == true) 
                          {
                            block_lcd_info = true; // Desbloqueia função lcd_info.
                            block_full = true;  // Desbloqueio secundário que bloqueia a função lcd_info.
                            flag_clear_lcd_info = true;                        
                                
                            //bot->sendMessage(idBuffer, F("Sistema ativado, pronto para cuidar do seu cultivo!"), "");   
                            enviarMensagem("Sistema ativado, pronto para cuidar do seu cultivo!");

                            on_off_geral = true;

                            digitalWrite(led_ciclo_15X45, LOW);
                            digitalWrite(led_ciclo_15X30, LOW);
                            digitalWrite(led_stat_rele, LOW);
                            digitalWrite(led_stat_erro_func, LOW);
                          }

                          Memory.begin("espaco_1", false); // Abre a memória para gravação.
                          Memory.putBool("on_off_geral", on_off_geral);
                          Memory.putBool("block_lcd_info", block_lcd_info);
                          Memory.putBool("block_full", block_full);
                          Memory.putBool("on_off", on_off);
                          Memory.end();
                      }
                      else 
                        {
                          if (cmd_active == false)
                            {
                              string_dinamica(cmd_invalid);
                            } 
                            else string_dinamica(pause_sistem);
                        }

                  } else enviarMensagem(cmd_invalid);  //bot->sendMessage(idBuffer, cmd_invalid, "");
              }
       
          else if (text.equals("/on_off_auto"))   
              {
                if (!blockInvald()) return; // Verifica se tem comandos atuando.

                        if (digitalRead(rele_irriga) == LOW) //  Verifica se a irrigação está desligada.
                          {
                            modo_trab = !modo_trab;

                            //block_cmd_0 = false; // Trava comandos.
                            //block_cmd_1 = false; // Trava comandos.
                            //counter_cmd_activ = 0; // Zera variavel para começar a contagem.

                            if (modo_trab == false)
                              {       
                                a_buzzer(2, 200, 200);

                                enviarMensagem("Desativando modo automático, aguarde...");
                               // bot->sendMessage(idBuffer, F("Desativando modo automático, aguarde..."), "");  

                                enviarMensagem("Pronto! Desativado.\n\n.O sistema deixou de controlar o cultivo a partir de agora.");                                       
                               // bot->sendMessage(idBuffer, F("Pronto! Desativado.\n\n.O sistema deixou de controlar o cultivo a partir de agora."), "");   

                                modo_manual = true; // Mostra ao comando status a situação do status.
                                modo_work = true;
                                flag_count_auto = true;  // Ativa o contador lá no timer.
                                counter_auto = 0;                         
                              }

                            if (modo_trab == true)
                              {
                                a_buzzer(2, 200, 200);

                                enviarMensagem("Ativando modo automático, aguarde...");
                                //bot->sendMessage(idBuffer, F("Ativando modo automático, aguarde..."), "");   

                                enviarMensagem("Pronto! Ativado.");                                      
                                //bot->sendMessage(idBuffer, F("Pronto! Ativado."), "");   

                                modo_manual = false;
                                modo_work = false;
                                flag_count_auto = false;
                                counter_auto = 0;

                                digitalWrite(led_stat_rele, LOW);
                              }  
                                    
                              Memory.begin("espaco_1", false); // Abre a memória para gravação.                 
                              Memory.putBool("modo_trab", modo_trab);
                              Memory.putBool("modo_manual", modo_manual);
                              Memory.putBool("modo_work", modo_work);
                              Memory.putBool("flag_count_auto", flag_count_auto);
                              Memory.end(); 
                          }
                          else string_dinamica(auto_onoff);                                                    
              } 

          else if (text.equals("/on_off_irrig")) 
              {
                if (!blockInvald()) return; // Verifica se tem comandos atuando.

                        if (modo_work == true) 
                          {
                            stat_bomba = !stat_bomba;

                             //block_cmd_0 = false; // Trava comandos.
                             //block_cmd_1 = false; // Trava comandos.
                             //counter_cmd_activ = 0; // Zera variavel para começar a contagem.

                            if (stat_bomba == false)
                              {                                                        
                                digitalWrite(rele_irriga, stat_bomba); 
                                digitalWrite(led_stat_rele, stat_bomba);

                                enviarMensagem("OK! Irrigação finalizada...");
                                //bot->sendMessage(idBuffer, F("OK! Irrigação finalizada..."), "");

                                off_sound = true;                            
                              }
                            
                            if (stat_bomba == true)
                              {
                                aux_stat_bomba = true;
                                
                                digitalWrite(rele_irriga, stat_bomba);  // Liga bomba para irrigação. 
                                digitalWrite(led_stat_rele, stat_bomba);

                                enviarMensagem("OK! Irrigação manual iniciada...");
                                //bot->sendMessage(idBuffer, F("OK! Irrigação manual iniciada..."), ""); 

                                off_sound = true;
                              }   
                          }
                          else string_dinamica(on_of_irri);
                       

                      Memory.begin("espaco_1", false);
                      Memory.putBool("stat_bomba", stat_bomba);
                      Memory.putBool("aux_stat_bomba", aux_stat_bomba);
                      Memory.end();
                                    
              }

          else if (text.equals("/on_off_drenar"))    // comando ok
              {
                if (!blockInvald()) return;

                        if (digitalRead(boia_baixa) == LOW)
                          {
                            if (bomb_on == false)
                              {
                                dry = !dry; // Seta para HIGH.
                                vapori = true;

                                lcd.backlight(); // Acende backlight.

                                //block_cmd_0 = false; // Trava comandos.
                                //block_cmd_1 = false; // Trava comandos.
                                //counter_cmd_activ = 0; // Zera variavel para começar a contagem.

                                on_backlig = true;  // Ativa o contador. 

                                if (dry == true) 
                                  {                        
                                    //flag_blklight = true; // Ativa o backlight temporario.  

                                    dry_lcd = true; // Ativa escrita no lcd após reinicar o dispositivo.
                                      
                                    lcd.clear();

                                    lcd.setCursor(6, 0);
                                    lcd.print(dry_line_0);

                                    lcd.setCursor(0, 1);
                                    lcd.print(dry_line_1);

                                    lcd.setCursor(0, 2);
                                    lcd.print(dry_line_2);

                                    lcd.setCursor(0, 3);
                                    lcd.print(dry_line_3);

                                    modo_work = true; // Desativa a irrigação.

                                    block_empty = false;  // Trava lcd info

                                    on_valvu = false; // Não deixa o a reposição de agua ligar.

                                    repor_aux_1 = true; // Habilta alerta de texto do telegram.

                                    // drena_reser = true; // Faz troca do alerta quando reservatorio estiver vazio.   deletar todos ->  drena_reser                               
                                      
                                    digitalWrite(rele_reser_2, dry); //  Liga rele reserva 2 (Bomba de drenagem).
                                    digitalWrite(led_stat_rele,dry); //  Liga led de status (amarelo).

                                    flag_back_pow_1 = true;  //  Flag que indica se algo está na memória.

                                    enviarMensagem("A drenagem do reservatório foi iniciada...");                                    
                                    //bot->sendMessage(idBuffer, F("A drenagem do reservatório foi iniciada..."), "");         

                                    block_empty = false;  // Trava lcd info caso haja arrependimento. 
                                  }

                                if (dry == false) 
                                  {                           
                                    lcd.clear();
                                        
                                    digitalWrite(rele_reser_2, dry);  //  Desliga rele reserva (Bomba de drenagem).
                                    digitalWrite(led_stat_rele, dry); //  Desliga led de status (amarelo).

                                    flag_back_pow_1 = false; 
                                    
                                    enviarMensagem("A drenagem do reservatório parou...");
                                    //bot->sendMessage(idBuffer, F("A drenagem do reservatório parou..."), "");   

                                    block_empty = true;  // Libera lcd info caso haja arrependimento.       

                                    block_lcd_info = true; // Reabilita o lcd info. 

                                    modo_work = false; // Reativa a irrigação.               
                                  }
                              }
                              else string_dinamica(on_of_dren_1);
                          }
                          else string_dinamica(on_of_dren_2);                           
                          
                          Memory.begin("espaco_1", false); // Abre a memória para gravação.                
                          Memory.putBool("flag_back_pow_1", flag_back_pow_1);
                          Memory.putBool("dry", dry);
                          Memory.putBool("dry_lcd", dry_lcd);                             
                          Memory.putBool("modo_work", modo_work);                           
                          Memory.putBool("block_empty", block_empty);
                          //Memory.putBool("repor_aux_1", repor_aux_1);
                          Memory.putString("dry_line_0", dry_line_0);   
                          Memory.putString("dry_line_1", dry_line_1); 
                          Memory.putString("dry_line_2", dry_line_2); 
                          Memory.putString("dry_line_3", dry_line_3);              
                          Memory.end(); 
              
                          flag_back_pow_1 = false;                           
              }

          else if (text.equals("/repor_agua"))   // comando ok
              {
                if (!blockInvald()) 
                  { 
                    open_valvu = false; // Desabilita a confirmação do comando /sim_abra.
                    return;
                  }
                
                        if (digitalRead(rele_irriga) == LOW) //  Verifica se a bomba de irrigação está desligada.   
                            {  
                              //block_cmd_0 = false; // Trava comandos.
                              //block_cmd_1 = false; // Trava comandos.
                              //counter_cmd_activ = 0; // Zera variavel para começar a contagem.

                              if ((digitalRead(boia_alta) == LOW) && (digitalRead(boia_baixa) == LOW))                         
                                { 
                                  string_dinamica(alert_repo_agua);
                                  openValv = true;
                                }
                              
                              if (((digitalRead(boia_alta) == HIGH) && (digitalRead(boia_baixa) == LOW)) && aux_dec_repor == false)
                                {  
                                  //block_cmd_0 = false; 
                                  
                                  open_valvu = true; // Habilita a confirmação do comando /sim_abra.

                                  openValv = true; // Avisa ao usuario que a valvula já está fechado.
                                    
                                  flag_trava = false;

                                  string_dinamica(open_val);    

                                  flag_trava = true;                                                                                                                                                
                                }                            
                              
                              if (aux_dec_repor == true) // Verifica se o reservatorio está vazio.
                                {
                                  flag_encher_lcd = true;  // Ativa condição para limpar LCD.

                                  openValv = true;  // Avisa ao usuario que a valvula já está fechado.
                                  
                                  flag_temp_led_rele = true; // Abre o contador do led do rele por 3 minutos. 

                                  flag_repor = true;  // Ativa status do "/status comand".   

                                  no_pronto = false;  // Proibe de usar o comando /pronto enquanto repõe a água.                         

                                  on_valvu = true; // Liga a valvula para repor a agua.

                                  flag_decisao = true; // Vai liberar a codição onde está aguardando a decisão. 

                                  counter_flag_wait_0 = true;                               

                                  digitalWrite(rele_val, on_valvu);  
                                  digitalWrite(led_stat_rele, on_valvu); 

                                  modo_work = true; // Trava totalmente a irrigação enquanto é reposta a agua.    

                                  Memory.begin("espaco_1", false);
                                  Memory.putBool("on_valvu", on_valvu);
                                  Memory.putBool("flag_decisao", flag_decisao); 
                                  Memory.putBool("modo_work", modo_work);                                                               
                                  Memory.end();  

                                  repor = true;  // Flag que garante que o sistema não executará nenhuma irrigação. 

                                  string_dinamica(repo_agua);                                   
                                }                           
                            }
                            else string_dinamica(bomba_on);
              }                           

          else if (text.equals("/off_repor"))    // comando ok
              {
                if (!blockInvald()) return;

                        if (openValv == true)
                          {                        
                            //block_cmd_0 = false; // Trava comandos.
                            //block_cmd_1 = false; // Trava comandos.
                            //counter_cmd_activ = 0; // Zera variavel para começar a contagem.
                            
                            flag_sim_abra = false;

                            flag_trava = true;
                            digitalWrite(rele_val, flag_sim_abra);
                            digitalWrite(led_stat_rele, flag_sim_abra);

                            enviarMensagem("OK! Fechando válvula!");
                            //bot->sendMessage(idBuffer, F("OK! Fechando válvula!"), "");

                            counter_flag_wait_0 = false;                        

                            flag_wait_0 = false;
                            flag_wait_1 = true;

                            flag_decisao = true;

                            wait_nutri();

                            flag_bac_pwr_2 = false;

                          } else string_dinamica(valvuClose);

                          Memory.begin("espaco_1", false);  // Abre a memória para gravação.
                          Memory.putBool("flag_sim_abra", flag_sim_abra);
                          Memory.putBool("flag_bac_pwr_2", flag_bac_pwr_2);
                          Memory.end();
              }

          else if (text.equals("/on_off_bip_irrig"))    // comando ok
              {
                if (!blockInvald()) return;

                        //block_cmd_0 = false; // Trava comandos.
                        //block_cmd_1 = false; // Trava comandos.
                        //counter_cmd_activ = 0; // Zera variavel para começar a contagem.
                        
                        on_off_bip_irri = !on_off_bip_irri;

                        if (!on_off_bip_irri)
                          {
                            string_dinamica(onoff_bipirri_0);
                          }
                        
                        if (on_off_bip_irri)
                          {
                            string_dinamica(onoff_bipirri_1);
                          }                                  
                          Memory.begin("espaco_1", false); // Abre a memória para gravação.                
                          Memory.putBool("on_off_bip_irri", on_off_bip_irri); 
                          Memory.end();
              }   

          else if (text.equals("/on_off_bip_nutr"))    // comando ok
              {
                if (!blockInvald()) return;

                        //block_cmd_0 = false; // Trava comandos.
                        //block_cmd_1 = false; // Trava comandos.
                        //counter_cmd_activ = 0; // Zera variavel para começar a contagem.
                        
                        on_off_bip_nutr = !on_off_bip_nutr;

                        if (on_off_bip_nutr == false)
                          {
                            string_dinamica(onoff_bipnutr_0); 
                          }

                        if (on_off_bip_nutr == true)
                          {
                            string_dinamica(onoff_bipnutr_1);
                          }

                        Memory.begin("espaco_1", false); // Abre a memória para gravação.                 
                        Memory.putBool("on_off_bip_nutr", on_off_bip_nutr);
                        Memory.end();
              } 
          
          else if (text.equals("/on_off_reset_prog"))   // comando ok  
              {
                 if (!blockInvald()) return;
                                                              
                        //block_cmd_0 = false; // Trava comandos.
                        //block_cmd_1 = false; // Trava comandos.
                        //counter_cmd_activ = 0; // Zera variavel para começar a contagem.
                        
                        reset_esp = !reset_esp;                                          
                                
                        if (reset_esp == false)
                          {
                            string_dinamica(esp_reset_0);

                            cont_reset = 1; // Valor padrão 15.                                                                                                                
                          }

                          if (reset_esp == true)
                            {
                              string_dinamica(esp_reset_1); 
                              delay(1200);
                              string_dinamica(esp_reset_2);    
                            }

                            Memory.begin("espaco_1", false); // Abre a memória para gravação.                   
                            Memory.putBool("reset_esp", reset_esp);                            
                            Memory.end();                         
              }

          else if (text.equals("/on_off_bip_res_prog"))     // comando ok
              {
                if (!blockInvald()) return;
                                        
                        //block_cmd_0 = false; // Trava comandos.
                        //block_cmd_1 = false; // Trava comandos.
                        //counter_cmd_activ = 0; // Zera variavel para começar a contagem.                        
                        
                        buzzer_reset = !buzzer_reset;

                        if (!buzzer_reset)
                          {
                            string_dinamica(reset_buzzer_0);
                          }
                      
                        if (buzzer_reset)
                          {
                            string_dinamica(reset_buzzer_1);
                          }

                          Memory.begin("espaco_1", false); // Abre a memória para gravação.                
                          Memory.putBool("buzzer_reset", buzzer_reset);
                          Memory.end();                             
              }

          else if (text.equals("/on_off_all_bip"))
              {
                if (!blockInvald()) return;
                                        
                        //block_cmd_0 = false; // Trava comandos.
                        //block_cmd_1 = false; // Trava comandos.
                        //counter_cmd_activ = 0; // Zera variavel para começar a contagem.

                        onoffall_bip = !onoffall_bip;

                        if (onoffall_bip == false) 
                          {
                            on_off_all_bip = false;

                            string_dinamica(bip_all_onoff_0);
                          }

                        if (onoffall_bip == true) 
                          {
                            on_off_all_bip = true;

                            string_dinamica(bip_all_onoff_1);
                          } 

                        Memory.begin("espaco_1", false);
                        Memory.putBool("on_off_all_bip", on_off_all_bip);
                        Memory.end();
              }

          else if (text.equals("/status_conex"))    // comando ok
              {
                if (!blockInvald()) return;
                                         
                        //block_cmd_0 = false; // Trava comandos.
                        //block_cmd_1 = false; // Trava comandos.
                        //counter_cmd_activ = 0; // Zera variavel para começar a contagem.
                        
                        if (WiFi.status() == WL_CONNECTED) 
                          { 
                            char* mensagemConexao = montarMensagemConexao();

                            enviarMensagem(mensagemConexao);
                            //bot->sendMessage(idBuffer, mensagemConexao, ""); 

                            free(mensagemConexao); // Libera a memória alocada   
                          } 
              }               
          
          else if ((text.equals("/del_creden")) || (text.equals("/sim_delete"))) // Comando com sub comando    // comando ok
              {
                if (!blockInvald()) return;
                                         
                        //block_cmd_0 = false; // Trava comandos.
                        //block_cmd_1 = false; // Trava comandos.
                        //counter_cmd_activ = 0; // Zera variavel para começar a contagem.

                        if (text.equals("/del_creden"))
                          { 
                            string_dinamica(cred_del);

                            liber_yes_del = true; 
                          }

                        if (text.equals("/sim_delete"))
                          {
                            if (liber_yes_del == true)
                              {
                                forceBoot_0 = true;

                              } else enviarMensagem(clean_creden);//bot->sendMessage(idBuffer, clean_creden, "");
                          }              
              }

          else if ((text.equals("/reset")) || (text.equals("/sim_reset"))) // Comando com sub comando    // comando ok
              {
                if (digitalRead(rele_irriga) == LOW) //  Verifica se a irrigação está desligada.
                  {                
                    if (text.equals("/reset"))
                      {
                        string_dinamica(rese_manual_0);
                      }                      

                    if (text.equals("/sim_reset")) forceBoot_1 = true;
                  }
                  else 
                    { 
                      string_dinamica(rese_manual_1);                       
                    }               
              } 

          else if ((text.equals("/off_sensor_ambi")) || (text.equals("/off_sensor_nutri")) || (text.equals("/off_sensor_diss")) || (text.equals("/off_all_alerts")))// Sub comando 
              {
                if (on_off_geral == true)
                  {
                    if (block_cmd_0 == true) // Reativa os comandos. 
                      {                    
                        temperaturas();                     
                
                      if (!on_off_geral) 
                        {
                          string_dinamica(system_disa);
                          return;  // Sai antecipadamente se o sistema estiver desativado
                        }               
                          
                      if (text.equals("/off_sensor_ambi")) 
                        {
                          if (!alerta_ambi_ativo) 
                            {
                              enviarMensagem("❌ Alerta do ambiente já está desativado!");
                              //bot->sendMessage(idBuffer, "❌ Alerta do ambiente já está desativado!", "");
                            } 
                            else 
                              {
                                alerta_ambi_ativo = false;

                                enviarMensagem("🔕 Alertas do ambiente desativados.");
                                //bot->sendMessage(idBuffer, "🔕 Alertas do ambiente desativados.", "");
                                salvarEstadoAlertas();  // Atualiza a memória
                              }
                        }
                        else if (text.equals("/off_sensor_nutri")) 
                            {
                              if (!alerta_nutri_ativo) 
                                {
                                  enviarMensagem("❌ Alerta da nutrição já está desativado!");
                                  //bot->sendMessage(idBuffer, "❌ Alerta da nutrição já está desativado!", "");
                                } 
                                else 
                                  {
                                    alerta_nutri_ativo = false;
                                    enviarMensagem("🔕 Alertas da nutrição desativados.");
                                    //bot->sendMessage(idBuffer, "🔕 Alertas da nutrição desativados.", "");
                                    salvarEstadoAlertas();
                                  }
                            }
                            else if (text.equals("/off_sensor_diss")) 
                                {
                                  if (!alerta_diss_ativo) 
                                    {                              
                                      enviarMensagem("❌ Alerta do dissipador já está desativado!");
                                      //bot->sendMessage(idBuffer, "❌ Alerta do dissipador já está desativado!", "");
                                    } 
                                    else 
                                      {
                                        alerta_diss_ativo = false;
                                        enviarMensagem("🔕 Alertas do dissipador desativados.");
                                        //bot->sendMessage(idBuffer, "🔕 Alertas do dissipador desativados.", "");
                                        salvarEstadoAlertas();
                                      }
                                }
                                else if (text.equals("/off_all_alerts")) 
                                    {
                                      if (!alerta_ambi_ativo && !alerta_nutri_ativo && !alerta_diss_ativo) 
                                        {
                                          enviarMensagem("❌ Todos os alertas já estão desativados!");
                                          //bot->sendMessage(idBuffer, "❌ Todos os alertas já estão desativados!", "");
                                        } 
                                        else 
                                          {
                                            desativarTodosAlertas();

                                            enviarMensagem("🔕 TODOS os alertas foram desativados.");
                                            // bot->sendMessage(idBuffer, "🔕 TODOS os alertas foram desativados.", "");
                                          }
                                    }

                      } else enviarMensagem(cmd_invalid);   //bot->sendMessage(idBuffer, cmd_invalid, "");                  
                                            
                  } else string_dinamica(system_disa);  
              }        

          else if (text.equals("/clean_data")) // parei aqui
              {
                string_dinamica(cl_data_1);
                                                               
                rese_fabrica = true; // Ativa subcomando /sim_restaure.
              }        
          
          else if (text.equals("/sim_restaure")) // Sub comando
              {
                if (rese_fabrica == true) 
                  {
                    string_dinamica(yes_restaure_0);                   
                    
                    forceBoot_2 = true;                    

                  } else string_dinamica(yes_restaure_1); 
              }

          else if (text.equals("/tempera"))    // comando ok 
              {
                if (on_off_geral == true)
                  {
                    if (block_cmd_0 == true) // Reativa os comandos. 
                      {                    
                        temperaturas();

                      } else enviarMensagem(cmd_invalid);//bot->sendMessage(idBuffer, cmd_invalid, ""); 

                  } else string_dinamica(system_disa);                   
              } 

          else if (text.equals("/pronto"))  // Comando para uso exclusivo do programador.
              {
                if (!blockInvald()) return;
                                        
                        if (aux_pronto == true)
                          {                
                            pronto_now = true;

                            digitalWrite(rele_reser_2, LOW);
                            digitalWrite(led_stat_rele, LOW);

                            string_dinamica(ready_now_0);                        

                            block_lcd_info = true;
                            flag_clear_lcd_info = true;  
                          }
                          else string_dinamica(ready_now_1);
              }         

          else if (text.equals("/del_sensor"))    // comando ok
              {
                if (!blockInvald()) return;
                                         
                        string_dinamica(sensor_delet);
                          
                        Sensores.begin("sensor_5", false);  
                        Sensores.clear(); // Para limpar todas as preferências no namespace aberto (não exclui o namespace). 
                        Sensores.end();  // Fecha nome space.

                        lcd.clear();
                        lcd.setCursor(6, 0);
                        lcd.print("ATENCAO!");

                        lcd.setCursor(1, 2);
                        lcd.print("DELETANDO SENSORES");  

                        lcd.setCursor(0, 3);
                        lcd.print("AGUARDE");               

                        // Desenha as bordas da barra de progresso /// desfazer até    áqui
                        lcd.createChar(0, bord_esq);
                        lcd.setCursor(8, 3);
                        lcd.write(0);                        

                        for (bords = 9; bords < 19; bords ++)
                           {
                             lcd.createChar(1, bord);
                             lcd.setCursor(bords, 3);
                             lcd.write(1);         
                           }
                           bords = 0;

                           lcd.createChar(2, bord_dir);
                           lcd.setCursor(19, 3);
                           lcd.write(2); 

                           delay(1200); 

                        for (bar_prog = 9; bar_prog < 19; bar_prog ++)
                           {
                             lcd.createChar(3, barra_pro);
                             lcd.setCursor(bar_prog, 3);
                             lcd.write(03);

                             if (bar_prog < 9)
                               {
                                 temp_ambi[bar_prog] = 0;
                                 Serial.print(temp_ambi[bar_prog]);

                                 temp_nutri[bar_prog] = 0;
                                 Serial.print(temp_nutri[bar_prog]);

                                 temp_diss[bar_prog] = 0;
                                 Serial.print(temp_diss[bar_prog]); 
                               }

                               Serial.println("");

                               delay(400); 
                           }

                           bar_prog = 0;
                           flag_clear_lcd_info = true;

                           flag_conf_sens = true;  // Reativa a configuração dos sensores.

                           Sensores.begin("sensor_5", false);
                           Sensores.putBool("flag_conf_sens", flag_conf_sens);
                           Sensores.end();

                           string_dinamica(sensor_delet_suc);
                            
                           lcd.clear();

                           disa_aler_sens = false;  // Desativa os sensores quando for deletados e evitas alertas desnecessarios.

                           forceBoot_gene = true;  
              }          

          else if (text.equals("/del_telegram"))    // comando ok
              {
                if (!blockInvald()) return;
                                         
                        string_dinamica(teleg_delet); 

                        lcd.clear();
                        lcd.setCursor(6, 0);
                        lcd.print("ATENCAO!");

                        lcd.setCursor(0, 1);
                        lcd.print("DELETANDO TOKEN E ID");

                        lcd.setCursor(0, 2);
                        lcd.print("DO TELEGRAM."); 

                        lcd.setCursor(0, 3);
                        lcd.print("AGUARDE");               

                        // Desenha as bordas da barra de progresso /// desfazer até    áqui
                        lcd.createChar(0, bord_esq);
                        lcd.setCursor(8, 3);
                        lcd.write(0);                        

                        for (bords = 9; bords < 19; bords ++)
                           {
                             lcd.createChar(1, bord);
                             lcd.setCursor(bords, 3);
                             lcd.write(1);         
                           }
                           bords = 0;

                           lcd.createChar(2, bord_dir);
                           lcd.setCursor(19, 3);
                           lcd.write(2); 

                           delay(1200); 

                        for (bar_prog = 9; bar_prog < 19; bar_prog ++)
                           {
                             lcd.createChar(3, barra_pro);
                             lcd.setCursor(bar_prog, 3);
                             lcd.write(03);

                             delay(400);                              
                            }                         

                        delay(1200);

                        string_dinamica(telegr_delet_suc);

                        memset(tokenBuffer, 0, sizeof(tokenBuffer)); // Limpa o tokenBuffer
                        memset(idBuffer, 0, sizeof(idBuffer));       // Limpa o idBuffer

                        Telegram.begin("token_id", false);  
                        Telegram.clear(); // Para limpar todas as preferências no namespace aberto (não exclui o namespace). 
                        Telegram.end();   // Fecha namespace.

                        delay(500);

                        lcd.clear();
              }

          else if (text.equals("/on_off_descri"))   // comando ok
              {
                if (!blockInvald()) return;
                                        
                        testa_18B20 = !testa_18B20; // Inverte o estado da flag.                
                        
                        Memory.begin("espaco_1", false); // Abre a memória para gravação.                
                        Memory.putBool("testa_18B20", testa_18B20);                
                        Memory.end();

                        if (testa_18B20 == true) string_dinamica(descr_onoff_0); 
                        if (testa_18B20 == false) string_dinamica(descr_onoff_1); 

                        Memory.end();
                        Serial.println("");   
              }

          else if (text.equals("/test_sensor"))    // comando ok
              {
                if (!blockInvald()) return;
                                        
                 test_sensores();  
              } 

          else if (text.equals("/on_off_backlight"))
              {
                if (!blockInvald()) return;

                Memory.begin("espaco_1", false);
                active_light = Memory.getBool("active_light", active_light);
                Memory.end();

                active_light = !active_light;

                if (active_light) 
                  {
                    lcd.setBacklight(LOW);  // Desliga o backlight físico
                    enviarMensagem("💡 Backlight: Modo Temporário.");
                    logToWeb("💡 Backlight: Modo Temporário.");

                    //bot->sendMessage(idBuffer, F("💡 Backlight: Modo Temporário."), "");
                  } else 
                      {
                        lcd.setBacklight(HIGH);  // Liga o backlight físico
                        enviarMensagem("💡 Backlight: Modo Fixo.");
                        logToWeb("💡 Backlight: Modo Fixo.");
                        //bot->sendMessage(idBuffer, F("💡 Backlight: Modo Fixo."), "");

                        counter_blackluz = 0; // Zera o contador do backlight.
                      }

                Memory.begin("espaco_1", false);
                Memory.putBool("active_light", active_light);
                Memory.end();

                //lcd.clear();

                block_empty = true;
              }

          else if (text.equals("/troca_bat"))    // comando ok 
              {
                if (!blockInvald()) return;
                                        
                        string_dinamica(troca_bat);
                          
                        set_time_cur = true;  // Ativa a troca da bateria.

                        Memory.begin("espaco_1", false); // Abre a memória para gravação.
                        Memory.putBool("set_time_cur", set_time_cur);
                        Memory.end();  
              } 

          else if (text.equals("/show_time")) 
              { 
                if (!blockInvald()) return;

                ntp.update();

                String formattedTime = ntp.getFormattedTime();  // Formato: HH:MM:SS

                String time_show = "Os horários recuperados são:\n\n";
                time_show += "Protocolo NTP: " + formattedTime + ".\n";

                DateTime now = rtc.now();

                char rtcTime[9];  // Buffer para armazenar o horário no formato "HH:MM:SS"
                sprintf(rtcTime, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());

                // Adicionar horário do RTC à mensagem
                time_show += "Protocolo RTC: " + String(rtcTime) + ".";

                if (bot != nullptr)  // Proteção contra panico.
                  {
                    bot->sendMessage(idBuffer, time_show, "");
                  }                       
              }

          else if (text.equals("/show_DS18B20"))    // comando ok  
              {
                if (!blockInvald()) return;
                                        
                descr_DS18B20();   
              }

          else if (text.equals("/modo_primaverao"))    // comando ok 
              { 
                if (!blockInvald()) return;
                                                            
                        if (atual_primavera == true)
                          {
                            atual_primavera = false; // Desativa condição acima.
                            //atual_outoinver = true; // Reativa condição do modo outono e inverno.

                            switc_outinver = true;

                            switch_estacao = true; // Flag que muda status do modo estação da irrigação.

                            led_verao = true;
                            led_inver = false; 
                            
                            //up_led_45 = true;  
                            //up_led_30 = false;                   

                            digitalWrite(led_ciclo_15X45, led_verao);
                            digitalWrite(led_ciclo_15X30, led_inver);  

                            flag_15X15 = true;
                            flag_10X20 = false; 
                            flag_15X45 = false;

                            aux1_minut_on = minuto[0];  // Liga irrigação no minuto 0.
                            aux2_minut_on = minuto[2];  // Liga irrigação no minuto 30.

                            aux3_minut_off = minuto[1];  // Desliga irrigação no minuto 15.
                            aux4_minut_off = minuto[3];  // Desliga irrigação no minuto 45.
                            
                            aux5_minut_off = minuto[1];  // Desliga irrigação no minuto 15. 

                            string_dinamica(swit_primver_0);

                            delay(2500);


                            enviarMensagem("Pronto, configurado!");
                            //bot->sendMessage(idBuffer, F("Pronto, configurado!"), ""); 

                          } else string_dinamica(swit_primver_1);
                  
                    Memory.begin("espaco_1", false);                   
                    Memory.putBool("switch_estacao", switch_estacao);                     
                    Memory.putBool("atual_primavera", atual_primavera);
                    Memory.putChar("aux1_minut_on", aux1_minut_on);
                    Memory.putChar("aux2_minut_on", aux2_minut_on);
                    Memory.putChar("aux3_minut_off", aux3_minut_off);                     
                    Memory.putChar("aux4_minut_off", aux4_minut_off);
                    Memory.putChar("aux5_minut_off", aux5_minut_off);
                    Memory.putBool("led_verao", led_verao);
                    Memory.putBool("led_inver", led_inver);                    
                    Memory.end();
              }

          else if (text.equals("/modo_outoinver"))    // comando ok 
              { 
                if (!blockInvald()) return;
                                        
                        if (atual_outoinver == true)
                          {                            
                            block_cmd_0 = false; // Trava comandos geral.
                            block_cmd_1 = false; // Trava comandos incrementação.
                            counter_cmd_activ = 0; // Zera variavel para começar a contagem.

                            selct_min_outinv = false; // Desativa seleção caso operador peça o comando fora do contexto.

                            atual_primavera = true; // Reativa condição do modo primavera verão.
                            
                            switch_estacao = false; // Flag que muda status do modo estação da irrigação.

                            led_verao = false;
                            led_inver = true;                        

                            digitalWrite(led_ciclo_15X45, led_verao);
                            digitalWrite(led_ciclo_15X30, led_inver);

                            flag_15X15 = false;
                            flag_10X20 = true; 
                            flag_15X45 = false;  

                            aux3_minut_off = minuto[5];  // Desliga irrigação diurna no minuto 10 - Intervalo de 10X20.
                            aux4_minut_off = minuto[6];  // Desliga irrigação diurna no minuto 40 - Intervalo de 10X20.
                            aux5_minut_off = minuto[5];  // Desliga irrigação noturna no minuto 10 - Intervalo de 10X50 

                            if (switc_outinver == true)
                              {
                                switc_outinver = false;                            

                                string_dinamica(swit_outinv_0);
                              }
                              else string_dinamica(swit_outinv_1);                                               

                          
                            if (aux5_minut_off == 5)
                              {
                                string_dinamica(swit_outinv_2); 
                              }
                              else if (aux5_minut_off == 10)
                                  {
                                    string_dinamica(swit_outinv_3);
                                  }
                                  else 
                                    {
                                      string_dinamica(swit_outinv_4); 
                                    }                       

                            delay(2500);               
                  
                  Memory.begin("espaco_1", false); 
                  Memory.putChar("aux3_minut_off", aux3_minut_off);
                  Memory.putChar("aux4_minut_off", aux4_minut_off);
                  Memory.putChar("aux5_minut_off", aux5_minut_off);                  
                  Memory.putBool("switch_estacao", switch_estacao);                                           
                  Memory.putBool("atual_primavera", atual_primavera);                    
                  Memory.putBool("switc_outinver", switc_outinver);                    
                  Memory.putBool("led_verao", led_verao); 
                  Memory.putBool("led_inver", led_inver);                    
                  Memory.end();
              }
              }

          else if ((text.equals("/min_5")) || (text.equals("/min_10")) || (text.equals("/min_15"))) // Sub comando.
              {
                if (!blockInvald()) return;

                        if (selct_min_outinv == true)
                          {
                            if (text.equals("/min_5"))
                              {
                                block_cmd_0 = false; // Trava comandos.
                                counter_cmd_activ = 0;
                                
                                aux5_minut_off = minuto[10];  // Desliga irrigação no minuto 5.

                                Memory.begin("espaco_1", false); 
                                Memory.putChar("aux5_minut_off", aux5_minut_off); 
                                Memory.end();

                              string_dinamica(search_irri_0);
                              }
                              else if (text.equals("/min_10"))
                                  {
                                    block_cmd_0 = false; // Trava comandos.
                                    counter_cmd_activ = 0;
                                    
                                    aux5_minut_off = minuto[5];  // Desliga irrigação no minuto 10.

                                    Memory.begin("espaco_1", false); 
                                    Memory.putChar("aux5_minut_off", aux5_minut_off); 
                                    Memory.end();
                                    
                                    string_dinamica(search_irri_1);
                                  }
                                  else 
                                    {
                                      block_cmd_0 = false; // Trava comandos.
                                      counter_cmd_activ = 0;
                                      
                                      aux5_minut_off = minuto[1];  // Desliga irrigação no minuto 15.

                                      Memory.begin("espaco_1", false); 
                                      Memory.putChar("aux5_minut_off", aux5_minut_off); 
                                      Memory.end();

                                      string_dinamica(search_irri_2);
                                    }

                                    string_dinamica(search_irri_3);

                                    delay(2500);

                                    enviarMensagem("Pronto, configurado!");
                                    //bot->sendMessage(idBuffer, F("Pronto, configurado!"), "");

                          } enviarMensagem(mens_min_outinv); //bot->sendMessage(idBuffer, mens_min_outinv, "");
              }            

          else if (text.equals("/set_tempera"))   // comando ok 
              {
                if (!blockInvald()) return;
                                                       
                        block_cmd_0 = false;
                        counter_cmd_activ = 0; // Zera variavel para começar a contagem.
                        activeContLockCmd = true; // Ativa contador.                         
                        start_corr_temp = true; // Habilita comandos de correção e incrementação.                        

                        string_dinamica(core_temp);
              }

          //else if ((text.equals("/temp_ambi")) || (text.equals("/temp_nutr")) || (text.equals("/temp_diss")) || (text.equals("/t_maxi_ambi")) || (text.equals("/t_maxi_nutr")) ||
          //(text.equals("/t_mini_ambi")) || (text.equals("/t_mini_nutr"))) // Sub comando.

          else if (text.startsWith("/temp_") || text.startsWith("/t_maxi_") || text.startsWith("/t_mini_"))     
              {
                if (on_off_geral == true)
                  {
                        if (start_corr_temp == true)
                          {                              
                             sensor.requestTemperatures();
                             delay(300); // Tempo de conversão para resolução de 12 bits (máxima) - ajuste conforme sua resolução
                            
                            if (text.equals("/temp_ambi"))
                              {
                                cor_temp_ambi = true;
                                cor_temp_nutr = false;
                                cor_temp_diss = false;
                                maxi_temp_ambi = false;
                                maxi_temp_nutr = false;
                                mini_temp_ambi = false;
                                mini_temp_nutr = false; 

                                ambiC = sensor.getTempC(temp_ambi);
                                
                                String star_cor_temp_0 = "Voce decidiu ajustar a temperatura do ambiente. Valor atual : ";
                                star_cor_temp_0 += String(ambiC, 2); // Converte float para String com 2 casas decimais
                                star_cor_temp_0 += " ºC. Valor do  ajuste: ";
                                star_cor_temp_0 += String(savedAdjCorAmb, 2); // Converte float para String com 2 casas decimais
                                //star_cor_temp_0 += " ºC. Valor do ajuste:"                                
                                star_cor_temp_0 += " ºC.\n\n";
                                star_cor_temp_0 += "Ok! Entre com a correção.    /incr    /decr\n\nou para setar para o valor nominal      /back_temp_ambi";

                                string_dinamica(star_cor_temp_0.c_str());                                  
                              }

                            if (text.equals("/temp_nutr"))
                              {
                                cor_temp_ambi = false;
                                cor_temp_nutr = true;
                                cor_temp_diss = false;
                                maxi_temp_ambi = false;
                                maxi_temp_nutr = false;
                                mini_temp_ambi = false;
                                mini_temp_nutr = false;                                 

                                nutriC = sensor.getTempC(temp_nutri); 
                                
                                String star_cor_temp_1 = "Voce decidiu ajustar a temperatura da nutrição. Valor atual no sensor: ";
                                star_cor_temp_1 += String(nutriC, 2); // Converte float para String com 2 casas decimais
                                star_cor_temp_1 += " ºC. Total ajustado: ";
                                star_cor_temp_1 += String(savedAdjCorNut, 2); // Converte float para String com 2 casas decimais
                                star_cor_temp_1 += " ºC.\n\n";
                                star_cor_temp_1 += "Ok! Entre com a correção.    /incr    /decr\n\nou para setar para o valor nominal      /back_temp_nutr"; 

                                string_dinamica(star_cor_temp_1.c_str());
                              }

                            if (text.equals("/temp_diss"))  
                              {
                                cor_temp_ambi = false;
                                cor_temp_nutr = false;
                                cor_temp_diss = true;
                                maxi_temp_ambi = false;
                                maxi_temp_nutr = false;
                                mini_temp_ambi = false;
                                mini_temp_nutr = false;                                

                                dissC = sensor.getTempC(temp_diss);                                

                                // Opção 1: Usando String para construir a mensagem
                                String star_cor_temp_2 = "Voce decidiu ajustar a temperatura da nutrição. Valor atual no sensor: ";
                                star_cor_temp_2 += String(dissC, 2); // Converte float para String com 2 casas decimais
                                star_cor_temp_2 += " ºC. Total ajustado: ";
                                star_cor_temp_2 += String(savedAdjCorDis, 2); // Converte float para String com 2 casas decimais
                                star_cor_temp_2 += " ºC.\n\n";
                                star_cor_temp_2 += "Ok! Entre com a correção.    /incr    /decr\n\nou para setar para o valor nominal      /back_temp_diss"; 

                                string_dinamica(star_cor_temp_2.c_str());                          
                              }

                            if (text.equals("/t_maxi_ambi")) 
                              {
                                cor_temp_ambi = false;
                                cor_temp_nutr = false;
                                cor_temp_diss = false;
                                maxi_temp_ambi = true;
                                maxi_temp_nutr = false;
                                mini_temp_ambi = false;
                                mini_temp_nutr = false;

                                char star_cor_temp_3[235]; // Defina um tamanho suficiente para a string resultante

                                sprintf(star_cor_temp_3, "Voce decidiu ajustar a temperatura máxima do ambiente. Valor atual: %.2f. Total ajustado: %.2f ºC.\n\n"
                                                         "Ok! Entre com a correção.     /incr     /decr\n\n"
                                                         "ou   para setar para o valor nominal       /back_maxi_ambi", savedAmbMax, savedAdjAmbMax);

                                string_dinamica(star_cor_temp_3);
                              }                            

                            if (text.equals("/t_mini_ambi")) 
                              {
                                cor_temp_ambi = false;
                                cor_temp_nutr = false;
                                cor_temp_diss = false;
                                maxi_temp_ambi = false;
                                maxi_temp_nutr = false;
                                mini_temp_ambi = true;
                                mini_temp_nutr = false;

                                char star_cor_temp_4[235]; // Defina um tamanho suficiente para a string resultante

                                sprintf(star_cor_temp_4, "Voce decidiu ajustar a temperatura mínima do ambiente. Valor atual: %.2f. Total ajustado: %.2f ºC.\n\n"
                                                         "Ok! Entre com a correção.      /incr      /decr\n\n"
                                                         "ou  para setar para o valor nominal       /back_mini_ambi", savedAmbMin, savedAdjAmbMin);   

                                string_dinamica(star_cor_temp_4);
                              }

                            if (text.equals("/t_maxi_nutr"))  
                              {
                                cor_temp_ambi = false;
                                cor_temp_nutr = false;
                                cor_temp_diss = false;
                                maxi_temp_ambi = false;
                                maxi_temp_nutr = true;
                                mini_temp_ambi = false;
                                mini_temp_nutr = false; 

                                char star_cor_temp_5[235];
                                
                                sprintf(star_cor_temp_5, "Voce decidiu ajustar a tempertaura máxima da nutrição. Valor atual: %.2f. Total ajustado: %.2f ºC.\n\n"
                                                         "Ok! Entre com a correção.      /incr      /decr\n\n"
                                                         "ou  para setar para o valor nominal       /back_maxi_nutr", savedNutMax, savedAdjNutMax);

                                string_dinamica(star_cor_temp_5);                          
                              }

                            if (text.equals("/t_mini_nutr")) 
                              {
                                cor_temp_ambi = false;
                                cor_temp_nutr = false;
                                cor_temp_diss = false;
                                maxi_temp_ambi = false;
                                maxi_temp_nutr = false;
                                mini_temp_ambi = false;
                                mini_temp_nutr = true;

                                char star_cor_temp_6[235];
                                
                                sprintf(star_cor_temp_6, "Voce decidiu ajustar a tempertaura mínima da nutrição. Valor atual: %.2f. Total ajustado: %.2f ºC.\n\n"
                                                         "Ok! Entre com a correção.      /incr      /decr\n\n"
                                                         "ou  para setar para o valor nominal       /back_mini_nutr", savedNutMin, savedAdjNutMin);  

                                string_dinamica(star_cor_temp_6);
                              } 
                          }                             
                          else 
                            {
                              if (block_cmd_0 == false)
                                {
                                  enviarMensagem(cmd_invalid);                                  
                                  //bot->sendMessage(idBuffer, cmd_invalid, ""); 

                                } else string_dinamica(star_cor_temp_7); 
                            }

                      } else enviarMensagem(cmd_invalid); //bot->sendMessage(idBuffer, cmd_invalid, "");    
                  
              }    

          else if ((text.equals("/incr")) || (text.equals("/decr"))) // aqui não é para colcoar block_cmd_1
              { 
                if (on_off_geral == true)
                  {
                    if (block_cmd_1 == true)
                      { 
                        block_cmd_1 = true;

                        counter_cmd_activ = 0; // Nesse ponto tem a função de resetar a variavael para mais 45 segundos de bloqeuio.

                        if (!(on_off_geral && block_cmd_1 && start_corr_temp)) 
                          {
                            enviarMensagem("Comando cancelado! Inicie a correção das temperaturas com o comando         /set_tempera.");
                            //bot->sendMessage(idBuffer, "Comando cancelado! Inicie a correção das temperaturas com o comando         /set_tempera.", "");
                            return; // Sai do bloco se as condições não forem satisfeitas
                          }

                        if (cor_temp_ambi) 
                          {
                            counter_corr_temp = 0;
                            adjustTemperature(savedAdjCorAmb, sensor.getTempC(temp_ambi), "CorAmb", "A temperatura ", counter_corr_temp);                    
                          }

                        if (cor_temp_nutr) 
                          {
                            adjustTemperature(savedAdjCorNut, sensor.getTempC(temp_nutri), "CorNut", "A temperatura ", counter_corr_temp);
                          }

                        if (cor_temp_diss) 
                          {
                            adjustTemperature(savedAdjCorDis, sensor.getTempC(temp_diss), "CorDis", "A temperatura ", counter_corr_temp);
                          }

                        if (maxi_temp_ambi) 
                          {
                            adjustTemperature(savedAdjAmbMax, temp_ref_ambi, "AmbMax", "A temperatura máxima ", counter_corr_temp);
                          }

                        if (maxi_temp_nutr) 
                          {
                            adjustTemperature(savedAdjNutMax, temp_ref_nutri, "NutMax", "A temperatura máxima ", counter_corr_temp);
                          }

                        if (mini_temp_ambi) 
                          {
                            adjustTemperature(savedAdjAmbMin, t_minima_ambi, "AmbMin", "A temperatura mínima ", counter_corr_temp);
                          }

                        if (mini_temp_nutr) 
                          {
                            adjustTemperature(savedAdjNutMin, t_minima_nutr, "NutMin", "A temperatura mínima ", counter_corr_temp);
                          }

                      } else enviarMensagem(cmd_invalid);  //bot->sendMessage(idBuffer, cmd_invalid, "");

                  } else string_dinamica(system_disa);              
              } 

          //else if ((text.equals("/back_temp_ambi")) || (text.equals("/back_temp_nutr")) || (text.equals("/back_temp_diss")) || (text.equals("/back_maxi_ambi")) || 
          //(text.equals("/back_maxi_nutr")) || (text.equals("/back_mini_ambi")) || (text.equals("/back_mini_nutr")) || (text.equals("/back_all_temp" )))   // Sub comando  
          
          else if (text.startsWith("/back_"))
              {
                if (on_off_geral) 
                  {
                    if (block_cmd_1) 
                    {
                      if (start_corr_temp) 
                      {
                        counter_cmd_activ = 0;
                        activeContLockCmd = true;

                        sensor.requestTemperatures();
                        delay(300);

                        Memory.begin("espaco_1", false);  // <- Única chamada de begin

                        if (text.equals("/back_temp_ambi")) 
                        {
                          savedAdjCorAmb = 0.00;
                          Memory.putFloat("savedAdjCorAmb", savedAdjCorAmb);

                          aux_ambiC = sensor.getTempC(temp_ambi);
                          char msg[75];
                          sprintf(msg, "A temperatura ambiente está nominal %.2f ºC.", aux_ambiC);
                          enviarMensagem(msg);
                          Memory.end();
                          return;
                        }

                        if (text.equals("/back_temp_nutr")) 
                        {
                          savedAdjCorNut = 0.00;
                          Memory.putFloat("savedAdjCorNut", savedAdjCorNut);

                          aux_nutriC = sensor.getTempC(temp_nutri);
                          char msg[75];
                          sprintf(msg, "A temperatura da nutrição está nominal %.2f ºC.", aux_nutriC);
                          enviarMensagem(msg);
                          Memory.end();
                          return;
                        }

                        if (text.equals("/back_temp_diss")) 
                        {
                          savedAdjCorDis = 0.0;
                          Memory.putFloat("savedAdjCorDis", savedAdjCorDis);

                          aux_dissC = sensor.getTempC(temp_diss);
                          char msg[75];
                          sprintf(msg, "A temperatura do dissipador está nominal %.2f ºC.", aux_dissC);
                          enviarMensagem(msg);
                          Memory.end();
                          return;
                        }

                        if (text.equals("/back_maxi_ambi")) 
                        {
                          savedAdjAmbMax = 0.00;
                          savedAmbMax = 34.00;
                          Memory.putFloat("savedAdjAmbMax", savedAdjAmbMax);
                          Memory.putFloat("savedAmbMax", savedAmbMax);

                          char msg[75];
                          sprintf(msg, "A temperatura máxima do ambiente está nominal %.2f ºC.", savedAmbMax);
                          enviarMensagem(msg);
                          Memory.end();
                          return;
                        }

                        if (text.equals("/back_maxi_nutr")) 
                        {
                          savedAdjNutMax = 0.00;
                          savedNutMax = 34.00;
                          Memory.putFloat("savedAdjNutMax", savedAdjNutMax);
                          Memory.putFloat("savedNutMax", savedNutMax);

                          char msg[75];
                          sprintf(msg, "A temperatura máxima da nutrição está nominal %.2f ºC.", savedNutMax);
                          enviarMensagem(msg);
                          Memory.end();
                          return;
                        }

                        if (text.equals("/back_mini_ambi")) 
                        {
                          savedAdjAmbMin = 0.00;
                          savedAmbMin = 18.00;
                          Memory.putFloat("savedAdjAmbMin", savedAdjAmbMin);
                          Memory.putFloat("savedAmbMin", savedAmbMin);

                          char msg[75];
                          sprintf(msg, "A temperatura mínima do ambiente está nominal %.2f ºC.", savedAmbMin);
                          enviarMensagem(msg);
                          Memory.end();
                          return;
                        }

                        if (text.equals("/back_mini_nutr"))
                        {
                          savedAdjNutMin = 0.00;
                          savedNutMin = 18.00;
                          Memory.putFloat("savedAdjNutMin", savedAdjNutMin);
                          Memory.putFloat("savedNutMin", savedNutMin);

                          char msg[75];
                          sprintf(msg, "A temperatura mínima da nutrição está nominal %.2f ºC.", savedNutMin);
                          enviarMensagem(msg);
                          Memory.end();
                          return;
                        }

                        if (text.equals("/back_all_temp")) 
                        {
                          String msg = "Todos os valores estão nominais:\n\n";

                          aux_ambiC = sensor.getTempC(temp_ambi);
                          savedCorAmb = aux_ambiC;
                          savedAdjCorAmb = 0.00;
                          Memory.putFloat("savedCorAmb", savedCorAmb);
                          Memory.putFloat("savedAdjCorAmb", savedAdjCorAmb);
                          msg += "Temp. ambiente: " + String(aux_ambiC, 2) + " ºC\n";

                          aux_nutriC = sensor.getTempC(temp_nutri);
                          savedCorNut = aux_nutriC;
                          savedAdjCorNut = 0.00;
                          Memory.putFloat("savedCorNut", savedCorNut);
                          Memory.putFloat("savedAdjCorNut", savedAdjCorNut);
                          msg += "Temp. nutrição: " + String(aux_nutriC, 2) + " ºC\n";

                          aux_dissC = sensor.getTempC(temp_diss);
                          savedCorDis = aux_dissC;
                          savedAdjCorDis = 0.00;
                          Memory.putFloat("savedCorDis", savedCorDis);
                          Memory.putFloat("savedAdjCorDis", savedAdjCorDis);
                          msg += "Temp. dissipador: " + String(aux_dissC, 2) + " ºC\n";

                          savedAmbMax = 34.00; savedAdjAmbMax = 0.00;
                          Memory.putFloat("savedAmbMax", savedAmbMax);
                          Memory.putFloat("savedAdjAmbMax", savedAdjAmbMax);
                          msg += "Max. ambiente: " + String(savedAmbMax, 2) + " ºC\n";

                          savedAmbMin = 18.00; savedAdjAmbMin = 0.00;
                          Memory.putFloat("savedAmbMin", savedAmbMin);
                          Memory.putFloat("savedAdjAmbMin", savedAdjAmbMin);
                          msg += "Min. ambiente: " + String(savedAmbMin, 2) + " ºC\n";

                          savedNutMax = 34.00; savedAdjNutMax = 0.00;
                          Memory.putFloat("savedNutMax", savedNutMax);
                          Memory.putFloat("savedAdjNutMax", savedAdjNutMax);
                          msg += "Max. nutrição: " + String(savedNutMax, 2) + " ºC\n";

                          savedNutMin = 18.00; savedAdjNutMin = 0.00;
                          Memory.putFloat("savedNutMin", savedNutMin);
                          Memory.putFloat("savedAdjNutMin", savedAdjNutMin);
                          msg += "Min. nutrição: " + String(savedNutMin, 2) + " ºC\n";

                          enviarMensagem(msg.c_str());
                          Memory.end();
                          return;
                        }

                        // Salvamento final
                        Memory.putFloat("kCorAmb", v_corr_ambi);
                        Memory.putFloat("kCorNut", v_corr_nutr);
                        Memory.putFloat("kCorDis", v_corr_diss);
                        Memory.putFloat("temp_ref_ambi", temp_ref_ambi);
                        Memory.putFloat("temp_ref_nutri", temp_ref_nutri);
                        Memory.end(); // <- Fechamento único da memória

                      } else 
                      {
                        enviarMensagem("Comando cancelado! Inicie a correção com /set_tempera.");
                      }
                    } else 
                    {
                      enviarMensagem(cmd_invalid);
                    }
                  } else 
                  {
                    string_dinamica(system_disa);
                  }
              }

          else if (text.equals("/scan_i2c")) 
              {
                if (!blockInvald()) return;
                               
                scanI2CBus();
              }                  

          else if (text.equals("/status_cmd")) 
              {
                
                if (on_off_geral == true) 
                  {
                    if (block_cmd_0 == true) 
                      {  
                       // Aloca memória dinamicamente para a string stat_coman
                        char* stat_coman = (char*)malloc(4096);  // Tamanho máximo da mensagem
                        
                        if (!stat_coman) 
                          {
                            enviarMensagem("Erro: Falha ao alocar memória.");
                            //bot->sendMessage(idBuffer, "Erro: Falha ao alocar memória.", "");
                            return;
                          }

                        strcpy(stat_coman, "Segue abaixo status do sistema.\n\n");

                       //------------------------------ STATUS DO CONTROLE ----------------------------------
                        if (on_off == true) 
                          {
                            if (on_off_geral == false) 
                              {
                                strcat(stat_coman, "Sistema de controle: DESATIVADO!\n");

                              } else strcat(stat_coman, "Sistema de controle: ATIVADO!\n");                  
                          }
                      //------------------------------------- FIM ------------------------------------------

                      //-------------------------- STATUS DO MODO DE TRABALHO ------------------------------
                        if (modo_manual == true) 
                          {
                            strcat(stat_coman, "Sistema configurado no modo MANUAL!.\n");

                            if (flag_count_auto == true) 
                              {
                                char counter_autom_str[16];
                                sprintf(counter_autom_str, "%d", counter_autom);
                                strcat(stat_coman, "Faltam ");
                                strcat(stat_coman, counter_autom_str);
                                strcat(stat_coman, " minutos para retomada do modo automático.\n");
                              }

                          } else strcat(stat_coman, "Sistema configurado no modo AUTOMÁTICO!\n");                        
                      //------------------------------------- FIM ------------------------------------------

                       //--------------------------- STATUS ALERTA DA NUTRIÇÃO ------------------------------
                        if (on_off_bip_nutr == false) 
                          {
                            strcat(stat_coman, "Alerta sonoro da nutrição: DESATIVADO!\n");

                          } else strcat(stat_coman, "Alerta sonoro da nutrição: ATIVADO!\n");
                        
                       //------------------------------------- FIM ------------------------------------------

                       //----------------------- STATUS DO ALERTA SONORO DA IRRIGAÇÃO -----------------------
                        if (on_off_bip_irri == false) 
                          {
                            strcat(stat_coman, "Alerta sonoro da irrigação: DESATIVADO!\n");

                          } else strcat(stat_coman, "Alerta sonoro da irrigação: ATIVADO!\n");
                        
                       //------------------------------------- FIM ------------------------------------------

                       //----------------------------- STATUS BUZZER RESET ----------------------------------
                        if (buzzer_reset == true) 
                          {
                              strcat(stat_coman, "Alerta sonoro do reset programado: ATIVADO!\n");
                          } else strcat(stat_coman, "Alerta sonoro do reset programado: DESATIVADO!\n");
                        
                       //------------------------------------- FIM ------------------------------------------

                       //-------------------------- STATUS DE REPOSIÇÃO DE AGUA -----------------------------
                        if (flag_conta_hora == false) 
                          {
                            strcat(stat_coman, "Alerta de reposição de nutriente: DESATIVADO!\n");

                          } else strcat(stat_coman, "Alerta de reposição de nutriente: ATIVADO!\n");
                        
                       //------------------------------------- FIM ------------------------------------------
            
                       //----------------- STATUS DE REFERÊNCIA DOS ALERTAS DE TEMPERATURA ------------------  trabalhando aqui

                        if ((auxAmbCorr_maxi != 34.00) || (auxAmbCorr_mini != 18.00)) 
                          {
                            strcat(stat_coman, "Temperatura máxima e mínima de trabalho para ambiente.\n");  

                            if (auxAmbCorr_maxi != 34.00) 
                              {                         
                                strcat(stat_coman, "   ├ Referência máxima 30 ºC.\n"); 
                                strcat(stat_coman, "   └ Foi alterado pelo operador para ");

                                // Converte o valor float para string usando sprintf
                                char aux_val_cor_amb[16];

                                sprintf(aux_val_cor_amb, "%.2f", auxAmbCorr_maxi);  // Formata com 2 casas decimais.

                                // Concatena a mensagem    
                                strcat(stat_coman, aux_val_cor_amb);
                                strcat(stat_coman, "ºC.\n");
                              }                              

                            if (auxAmbCorr_mini != 18.0)
                              {                                                       
                                strcat(stat_coman, "   │\n");
                                strcat(stat_coman, "   ├ Referência mínima 18 ºC.\n"); 
                                strcat(stat_coman, "   └ Foi alterado pelo operador para ");

                                // Converte o valor float para string usando sprintf
                                char aux_val_cor_nut[16];

                                sprintf(aux_val_cor_nut, "%.2f", auxAmbCorr_mini);  // Formata com 2 casas decimais

                                // Concatena a mensagem    
                                strcat(stat_coman, aux_val_cor_nut);
                                strcat(stat_coman, "ºC.\n\n");
                              }
                          }   
                        
                        if (temp_ref_nutri != 34.00) 
                          {
                            strcat(stat_coman, "Temperatura de referência da nutrição para o alerta.\n");                
                            strcat(stat_coman, "   ├ Referência padrão 25 ºC.\n");
                            strcat(stat_coman, "   └ Foi alterado pelo operador para ");
                            
                            strcat(stat_coman, "ºC.\n");
                          } 

                       //------------------------------------- FIM ------------------------------------------ 

                       //-------------------------- STATUS DE REPOSIÇÃO DE AGUA -----------------------------
                        if (flag_repor == true) 
                          {
                            strcat(stat_coman, "Reposição da água está em andamento: SIM!\n");

                          } else strcat(stat_coman, "Reposição da água está em andamento: NÃO!\n");
                        
                       //------------------------------------- FIM ------------------------------------------

                       //---------------------------- STATUS RESET PROGRAMADO -------------------------------
                        if (reset_esp == true) 
                          {
                            char cont_reset_aux[16];
                            sprintf(cont_reset_aux, "%d", cont_reset);
                            strcat(stat_coman, "Reset programado para 15 dias: ATIVADO!\n");
                            strcat(stat_coman, "Faltam ");
                            strcat(stat_coman, cont_reset_aux);
                            strcat(stat_coman, " dia(s) para o reset programado.\n");

                          } else strcat(stat_coman, "Reset programado para 14 dias: DESATIVADO!\n");
                          
                       //------------------------------------- FIM ------------------------------------------

                       //------------------------------ STATUS DO DS18B20 -----------------------------------
                        if (testa_18B20 == false) 
                          {
                            strcat(stat_coman, "Descrição do sensor DS18B20: DESATIVADO!\n");

                          } else strcat(stat_coman, "Descrição do sensor DS18B20: ATIVADO!\n");
                        
                       //------------------------------------- FIM ------------------------------------------

                       //------------------------------ STATUS DA ESTAÇÃO -----------------------------------
                        if (switch_estacao == false) 
                          {
                            strcat(stat_coman, "A irrigação está no modo outono e inverno.\n");
                            strcat(stat_coman, "   ├ Ciclo de irrigação diurna atual é 10X20 minutos!\n");
                            strcat(stat_coman, "   └");
                            if (aux5_minut_off == 5) 
                              {
                                strcat(stat_coman, " Ciclo de irrigação noturna atual é 5X55 minutos!\n");
                              } 
                              else if (aux5_minut_off == 10) 
                                  {
                                    strcat(stat_coman, " Ciclo de irrigação noturna atual é 10X50 minutos!\n");

                                  } else strcat(stat_coman, " Ciclo de irrigação noturna atual é 15X15 minutos!\n");                
                          } 
                          else 
                            {
                              strcat(stat_coman, "A irrigação está no modo primavera e verão.\n");
                              strcat(stat_coman, "   ├ Ciclo de irrigação diurna atual é 15X15 minutos!\n");
                              strcat(stat_coman, "   └ Ciclo de irrigação noturna atual é 15X45 minutos!\n");
                            }
                       //------------------------------------- FIM ------------------------------------------

                       //------------------------------ STATUS DA IRRIGAÇÃO ---------------------------------
                        if (digitalRead(rele_irriga) == HIGH) 
                          {
                            strcat(stat_coman, "A irrigação do cultivo sendo executada nesse momento!\n");
                          } else strcat(stat_coman, "A irrigação do cultivo está pausado nesse momento!\n");
                      
                       //------------------------------------- FIM ------------------------------------------

                       //------------------- EXIBE O STATUS DA CORREÇÃO DAS TEMPERATURAS -------------------
                        if (v_corr_ambi != 0) 
                          {
                            char aux_tempC[16];
                            sprintf(aux_tempC, "%.2f", v_corr_ambi);  // Formata o float com 2 casas decimais
                            strcat(stat_coman, "Temperatura do ambiente foi setada em: ");
                            strcat(stat_coman, aux_tempC);
                            strcat(stat_coman, " °C.\n");
                          }

                        if (v_corr_nutr != 0) 
                          {
                            char aux_nutrC[16];
                            sprintf(aux_nutrC, "%.2f", v_corr_nutr);  // Formata o float com 2 casas decimais
                            strcat(stat_coman, "Temperatura da nutrição foi setada em: ");
                            strcat(stat_coman, aux_nutrC);
                            strcat(stat_coman, " °C.\n");
                          }

                        if (v_corr_diss != 0) 
                          {
                            char aux_dissC[16];
                            sprintf(aux_dissC, "%.2f", v_corr_diss);  // Formata o float com 2 casas decimais
                            strcat(stat_coman, "Temperatura do dissipador foi setada em: ");
                            strcat(stat_coman, aux_dissC);
                            strcat(stat_coman, " °C.\n");
                          }

                       //------------------------------------- FIM ------------------------------------------

                       //------------------------------- STATUS BACKLIGHT -----------------------------------
                        if (active_light == true) 
                          {
                            strcat(stat_coman, "O backlight está no modo: TEMPORÁRIO.\n");
                          } else strcat(stat_coman, "O backlight está no modo: FIXO.\n\n");
                        
                       //------------------------------------- FIM ------------------------------------------

                       //--------------------------- FORNECE A HORA NO STATUS -------------------------------
                        ntp.update();
                       
                        char hora_str[32];
                        sprintf(hora_str, "Consulta do status executada as %02d:%02d:%02d.\n\n", ntp.getHours(), ntp.getHours(), ntp.getSeconds());
                        strcat(stat_coman, hora_str);

                        strcat(stat_coman, "Fim do status.");

                        enviarMensagem(stat_coman);
                        //bot->sendMessage(idBuffer, stat_coman, "");
                        
                        free(stat_coman);
                       //------------------------------------- FIM ------------------------------------------

                      } enviarMensagem(cmd_invalid);  //bot->sendMessage(idBuffer, cmd_invalid, "");
                  }
                  else 
                    {
                      char aux_stat_cmd[128];

                      sprintf(aux_stat_cmd, "Sistema está pausado, sistema em pausa não retorna nenhum status.");

                      enviarMensagem(aux_stat_cmd);
                      //bot->sendMessage(idBuffer, aux_stat_cmd, "");
                    }
              }

          else if (text.equals("/update"))
              { 
                checkFirmware();                  
              }

          else if (text.equals("/sim_atualize"))
              { 
                forceBoot_3 = true;                   
              }

          else if (text.equals("/versao"))
              {
                enviarMensagem("Buscando versão do firmware do dispositivo! Aguarde...");                
                delay(1800);

                char buffer[64];
                snprintf(buffer, sizeof(buffer), "Versão encontrada: %s", CURRENT_VERSION);
                enviarMensagem(buffer);
                enviarMensagem("\n\nVerificar se há atualização?\n\nVerificar ->        /update");                
              }

          else if (text.equals("/cancelar"))
              {
                block_cmd_0 = true; // Reativa todos os comandos.
                block_cmd_1 = true; // Reativa todos os comandos.
                activeContLockCmd = false; // Desativa o contador.
                counter_cmd_activ = 0;
                start_corr_temp = false;

                enviarMensagem("Comando anterior cancelado!");
                //bot->sendMessage(idBuffer, "Comando anterior cancelado!", "");   
              }

          else enviarMensagem(cmd_invalid);  //bot->sendMessage(idBuffer, cmd_invalid, "");     
        } // fim do laço for.    
    }

void blinkIpTelegram()      // Funcão que pisca se o Telegram ficar inativo.
    {
      static unsigned long lastSwitchTime = 0;
      static bool mostrarAlerta = false;

      bool telegramInativo = (strlen(tokenBuffer) == 0) && (strlen(idBuffer) == 0);
      bool wifiOk = WiFi.status() == WL_CONNECTED;

      if (!telegramInativo || !wifiOk) 
        {
          //digitalWrite(led_stat_erro_func, LOW);  // LED desligado se tudo ok
          return;
        }

      if (millis() - lastSwitchTime >= 3000) 
        {
          lastSwitchTime = millis();
          mostrarAlerta = !mostrarAlerta;

          if (mostrarAlerta) 
            {
              lcd.clear();  // Limpa antes de escrever o aviso
              digitalWrite(led_stat_erro_func, HIGH);

              lcd.setCursor(2, 1);
              lcd.print("TELEGRAM INATIVO");
              lcd.setCursor(3, 2);
              lcd.print("REQUER ATENCAO");
            } else 
                {
                  digitalWrite(led_stat_erro_func, LOW);
                  // Não escreve nada — permite a função externa exibir a temperatura/IP
                }
        }
    }

void all_led_on_off(bool led_)      // Função que acende ou apaga todos os leds.
    {
      digitalWrite(led_ciclo_15X30, led_);
      digitalWrite(led_ciclo_15X45, led_);
      digitalWrite(led_stat_rele, led_);
      digitalWrite(led_stat_erro_func, led_); 
    } 

void wait_nutri()      // Função que verifica se o usuário adicionou os nutriente e aguarda o comando pelo botão Set.
    {
      while ((((digitalRead(boia_baixa) == LOW) && (digitalRead(boia_alta) == LOW)) && flag_decisao == true) || ((digitalRead(boia_alta) == HIGH) && flag_decisao == true) || (flag_pos_fill == true))
          { 
            if (lcd_wait_nutri == true) 
              {
                lcd_wait_nutri = false;

                lcd.clear();
              }

            delay(1000);

            lcd.setCursor(6, 0);
            lcd.print("ATENCAO!");

            lcd.setCursor(0, 2);
            lcd.print("RESERVATORIO CHEIO");

            lcd.setCursor(0, 3);
            lcd.print("AGUARDANDO NUTRIENTE");            

            trava_blink = false;  //  Desliga o led piscante indicanto nivel baixa da água. Parei aqui.

            digitalWrite(led_stat_rele, HIGH);
            digitalWrite(led_stat_erro_func, HIGH);          

            if (counter_flag_wait_0 == true) // Intervalo que fica avisando a reposição de nutrientes.
              {  
                counter_flag_wait_0 = false;

                char buffer[512];  // Garante espaço suficiente para a mensagem

                strcpy_P(buffer, nutr_wait);      // Copia da flash para a RAM
                string_dinamica(buffer);                

                if (on_off_bip_nutr == true) a_buzzer(2, 200, 200);
                                       
                flag_wait_1 = false;                   
              }
                active_aler_nutr = true; // Ativa o cantador. 

                mills_newmess(); // Função que faz leitura dos comandos Enviardos pelo Telegram.  

                aux_pronto = true;  // Habilita o comando "/pronto" e ação do botão SET.  

                        
            if ((digitalRead(bt_set) == LOW) || pronto_now == true) // verificar se o botão foi presiona ou enviado o camondo do Telegran.
              {
                aux_pronto = false;  // Desabilita o comando "/pronto" e ação do botão SET.
                
                digitalWrite(rele_reser_2, LOW);  //  Garante que a bomba secadora esta desligada.

                lcd.clear();

                lcd.setCursor(4, 0);
                lcd.print("TUDO CERTO!");

                lcd.setCursor(0, 2);
                lcd.print("INICIANDO O SISTEMA");

                lcd.setCursor(0, 3);
                lcd.print("AGUARDE");              

                for (pol = 7; pol < 13; pol ++) // Cria o efeito dos pontos (......).
                  {                   
                    lcd.setCursor(pol, 3);
                    lcd.print(".");
                    
                    delay(850);              
                  }

                //lcd.clear();

                trava_blink = false;

                to_change_15X30 = true;  // Reativa a opção para mudar de ciclo 15X30 após o reservatório cheio. 

                flag_decisao = false;
                flag_wait_0 = true;
                flag_wait_1 = false;
                flag_cert = false;
                flag_toma_dec = false;

                desat_seca = true; // Reativa seca reserva.

                modo_work = false; // Religa irrigação após dreno total do reservatótio.

                counter_flag_wait_0 = false;  // Pausa contagem de aviso de reposição de ingridientes.
                active_aler_nutr = true;    // Destiva o cantador.
                cont_0 = 0;
                
                //counter_flag_wait_1 = false;  // Pausa contagem de aviso de reposição de ingridientes.                          

                all_led_on_off(LOW);  // Apaga todos os 4 leds superiores.

                char buffer[512];  // Garante espaço suficiente para a mensagem

                strcpy_P(buffer, bom_work);
                string_dinamica(buffer);

                string_dinamica(bom_work);

                repor = false;  // Libera irrigação após o operador adicionar os ingredientes

                flag_pos_fill = false;  // Ativa a decisão de repor nutriente do enchimento parcial.

                Memory.begin("espaco_1", false);
                Memory.putBool("flag_pos_fill", flag_pos_fill);
                Memory.putBool("modo_work", modo_work);
                Memory.end();

                pronto_now = false;
                fill_reser = false;
                active_aler_nutr = false; // Ativa o cantador.
                cont_0 = 0;                
              }
              
              flag_toma_dec = true;  
              flag_trava = true;  
              dry = false;     
          }
    } 

void reseta_ESP()      // Função que reseta o Esp32 por programação.
    { 
      if (reset_esp == true)
        {
          if (((T_horario > "00:20:00") && (T_horario < "00:21:00")) && (aux_reset_esp == true))          
            {         
              aux_reset_esp = false;

              cont_reset --;

              cont_reset_aux = String(cont_reset);

              Memory.begin("espaco_1", false);
              Memory.putInt("cont_reset", cont_reset);
              Memory.putString("cont_reset_aux", cont_reset_aux);
              Memory.putBool("aux_reset_esp", aux_reset_esp);
              Memory.end();

              if (cont_reset == 0) 
                {
                  cont_reset = 1;  // Valor padrão 15. 

                  //reset_esp = false; // Desativa o reset programado após reiniciar.

                  enviarMensagem("Executando reset programado, sistema será reiniciado...");
                  //bot->sendMessage(idBuffer, "Executando reset programado, sistema será reiniciado...", "");   

                  Memory.begin("espaco_1", false);
                  Memory.putInt("cont_reset", cont_reset);
                  Memory.putString("cont_reset_aux", cont_reset_aux);
                  Memory.end();

                  if ((buzzer_reset == true) && (on_off_all_bip == true))  a_buzzer(1, 3500, 0);// Verifica se o aviso sonoro do reset programado está ativado ou desativado.
                    
                  set_time_cur = true;

                  all_led_on_off(HIGH);

                  delay(3500);

                  ESP.restart();
                }            
            }

          if ((T_horario > "00:22:00") && (T_horario < "00:23:00")) 
            {
              aux_reset_esp = true;   // Reativa a flag para próximo reset. 

              Memory.begin("espaco_1", false);               
              Memory.putBool("aux_reset_esp", aux_reset_esp);
              Memory.end();
            }      
        }         
    }

void seca_reserva()      // Função que simula o reservatório evaporar.
    {
      if (lock_seca_rese == true)
        {
          if (modo_work == false)
            {
              if (desat_seca == true)
                {                  
                  if ((T_horario >= "07:00:00") && (T_horario <= "23:32:00")) // Controla o intervalo de hora das 7 a 22.
                    { 
                      if ((T_minuto == minuto[4]) && (block_dry == true))
                        {                                                     
                          block_dry = false;   

                          flag_seca = true; // Aciona o contador de tempo em que a bomba seca fica ligada.

                          counter_dry ++;

                          String aux_seca = String(counter_dry);

                          Memory.begin("espaco_1", false);
                          Memory.putInt("counter_dry", counter_dry);
                          Memory.end();

                          digitalWrite(rele_reser_2, HIGH);
                          digitalWrite(led_stat_rele, HIGH);

                          logToWeb(" - Drenando o reservatório: " + T_horario + "      ====[ "+  aux_seca  +" ]====.\n");

                          if (bot != nullptr) // Proteção contra panico. 
                            {                              
                              bot->sendMessage(idBuffer, "Drenando o reservatório: " + T_horario + "      ====[ "+  aux_seca  +" ]====.", "");
                            }                                                                                          
                        }

                      if (T_minuto != minuto[4]) block_dry = true;                               
                    }
                }
            }
        }
    }

void cont_hora()      // Função que avisa de tempos em tempos que o reservatório está vazio.
    {
      if (!flag_conta_hora) return; // Sai da função se a flag não estiver ativada

      // Estrutura para armazenar os alertas e seus respectivos tempos
      struct Alert 
            {
              bool* flag;          // Ponteiro para a flag correspondente
              const char* message; // Mensagem a ser enviada
              bool hasSound;       // Indica se deve tocar o som
            };

      // Array de alertas com suas configurações
      Alert alerts[] = 
          {
            {&flag_3600, "Oiiii!. Não se esqueça que tem que repor água do reservatório.", true},
            {&flag_10800, "Eiii!. Esqueceu! Tem que repor água do reservatório.", true},
            {&flag_18000, "Não me ignore!. Não se esqueça que tem que repor água do reservatório.", true},
            {&flag_25200, "Poxa vida que chato isso! Você está ignorando meus alertas, você ainda não repôs a água do reservatório.", true},
            {&flag_32400, "Percebi que você ainda não repôs a água do reservatório, ok! A partir de agora os alertas serão encerrados, agora está por sua conta e risco!", true}
          };

      // Itera sobre os alertas
      for (size_t i = 0; i < sizeof(alerts) / sizeof(Alert); ++i) 
        {
          if (*(alerts[i].flag)) 
            { 
              enviarMensagem(alerts[i].message);
              //bot->sendMessage(idBuffer, alerts[i].message, ""); // Envia a mensagem

              if (on_off_all_bip && alerts[i].hasSound) 
                {
                  a_buzzer(3, 150, 150); // Toca o som, se necessário
                }

              *(alerts[i].flag) = false; // Desativa a flag após o envio

              // Caso especial para o último alerta
              if (i == sizeof(alerts) / sizeof(Alert) - 1)
                {
                  counter_hora = 0;
                  trava_cont_hora = false;  // Desativa contador da função "cont_hora()"
                  flag_conta_hora = false;  // Desativa função "cont_hora()"
                }
            }
        }
    } 

String formatCorrectionStatus(const char* sensorName, float correctionValue)      // Função responsável por enviar strings para bot telegram.
    {
      // Usa buffer de char e snprintf para formatação segura e eficiente
      char buffer[150]; // Buffer para a string formatada

      if (fabs(correctionValue) < 0.01) 
        { // Compara float com uma pequena tolerância
          snprintf(buffer, sizeof(buffer), "Temperatura %s não foi alterada.", sensorName);
        } 
        else 
          {
            // dtostrf é útil para formatar float em microcontroladores, mas snprintf também funciona
            // char valueStr[10];
            // dtostrf(fabs(correctionValue), 1, 2, valueStr); // Converte float para string (valor absoluto, min 1 digito, 2 decimais)

           /* // Usando snprintf para float (requer configuração correta no PlatformIO/Arduino Core)
            snprintf(buffer, sizeof(buffer),
                    "Temperatura %s foi alterada em %.2f ºC para %s.", // %.2f formata float
                    sensorName, 
                    fabs(correctionValue), // Usa valor absoluto para a mensagem
                    (correctionValue > 0 ? "mais" : "menos")); // Determina "mais" ou "menos"*/

            snprintf(buffer, sizeof(buffer),
         "Temperatura %s foi alterada em %+.2f ºC.", // %+.2f mostra o sinal (+ ou -)
         sensorName, 
         correctionValue); // Mantém o valor original (com sinal)
          }

          return String(buffer); // Converte o buffer final para String do Arduino
    }

void temperaturas()      // Função responsável por exebir status da temperatura no telegram.
    {
      static unsigned long ultimaLeituraTemp = 0;
      const unsigned long intervaloLeituraTemp = 10000; // 10 segundos
    
      unsigned long agora = millis();
      if (agora - ultimaLeituraTemp < intervaloLeituraTemp) 
        {
          return; // Sai da função se não tiver passado 10 segundos
        }
      ultimaLeituraTemp = agora;

      sensor.requestTemperatures();
      delay(190);  // necessário aguardar sensor

      float temp_ambi_C = sensor.getTempC(temp_ambi);
      float temp_nutri_C = sensor.getTempC(temp_nutri);
      float temp_diss_C = sensor.getTempC(temp_diss);

        // 4. Verifica se houve erro na leitura (opcional mas recomendado)
        if (temp_ambi_C == TEMP_READ_ERROR || temp_ambi_C == TEMP_INIT_ERROR) 
          {
            Serial.println("ERRO ao ler sensor de ambiente!");
            logToWeb(" - ERRO ao ler sensor de ambiente!");            
          }
        
        if (temp_nutri_C == TEMP_READ_ERROR || temp_nutri_C == TEMP_INIT_ERROR) 
          {
            Serial.println("ERRO ao ler sensor de nutrição!");
            logToWeb(" - ERRO ao ler sensor de nutrição!");
          }

        if (temp_diss_C == TEMP_READ_ERROR || temp_diss_C == TEMP_INIT_ERROR) 
          {            
            Serial.println("ERRO ao ler sensor do dissipador!");
            logToWeb(" - ERRO ao ler sensor do dissipador!");
          }

          // 5. Calcula as temperaturas corrigidas
          float corrected_ambi = temp_ambi_C + savedAdjCorAmb;
          float corrected_nutri = temp_nutri_C + savedAdjCorNut;
          float corrected_diss = temp_diss_C + savedAdjCorDis;

        // 6. Constrói a parte da mensagem sobre as correções (se houver)
        String temp_corr_msg = ""; // String local inicializada vazia

        if (fabs(savedAdjCorAmb) > 0.01 || fabs(savedAdjCorNut) > 0.01 || fabs(savedAdjCorDis) > 0.01) 
          { // Verifica se alguma correção é diferente de zero
            temp_corr_msg = "OBS.: Houve alteração manual nos valores!\n\n";
            temp_corr_msg += formatCorrectionStatus("do ambiente", savedAdjCorAmb) + "\n"; 
            temp_corr_msg += formatCorrectionStatus("da nutrição", savedAdjCorNut) + "\n";
            temp_corr_msg += formatCorrectionStatus("do dissipador", savedAdjCorDis) + "\n";
          }
        
        char temper_buffer[300]; // Buffer local

        snprintf(temper_buffer, sizeof(temper_buffer),
                "\nValores coletados nos sensores:\n\n"
                "Temperatura do ambiente: %.2f °C\n"   // Formata o float com 2 decimais
                "Temperatura da nutrição: %.2f °C\n"
                "Temperatura do dissipador: %.2f °C\n",
                corrected_ambi,
                corrected_nutri,
                corrected_diss);

        // 8. Combina as mensagens e envia para o Telegram
        String finalMessage = temp_corr_msg + String(temper_buffer); // Concatena a parte da correção (se houver) com as leituras    

        if (bot != nullptr) // Proteção contra panico.
          {
            bot->sendMessage(idBuffer, finalMessage, "");
          }
    }

void falha_sensor()      // Função que verifica se o sensor DS18B20 está atuando.
    {
      if ((!trava_alert_temp) || (bot == nullptr)) return;

      static unsigned long lastAlertTime[3] = {0, 0, 0};
      static bool estadoAnterior[3] = {false, false, false}; // false = normal, true = problema

      auto verificarSensor = [&](SensorInfo& info, DeviceAddress& addr, float& temp,
                        float tempMax, bool& flagLed, const String& nomeSensor,
                        const String& comandoOff, const String& tipoSensor, 
                        int pinoRele, UniversalTelegramBot* bot, String chatId,
                        DallasTemperature& sensor, bool& alertFlag, int sensorIndex) 
      {
     
      // --- Leitura estável com filtro (técnico) ---
      static float tempAnterior[3] = {0,0,0};

      sensor.requestTemperatures();
      delay(300);
      float novaTemp = sensor.getTempC(addr);

      if (abs(novaTemp - tempAnterior[sensorIndex]) < 2.0 || novaTemp == DEVICE_DISCONNECTED_C) 
        {
          temp = novaTemp;
        }
      tempAnterior[sensorIndex] = novaTemp;

      // --- Verificação (sem alterar strings) ---
      bool leituraValida = (temp != DEVICE_DISCONNECTED_C && temp > -10 && temp < 125);
      bool conectadoAtual = leituraValida;
      bool tempNormalAtual = leituraValida && (temp <= tempMax);

      // --- Controle Físico ---
      alertFlag = !tempNormalAtual;
      flagLed = !tempNormalAtual;
    
      if (pinoRele != -1) 
        {
          digitalWrite(pinoRele, (leituraValida && !tempNormalAtual) ? HIGH : LOW);
        }

      // --- Lógica de Mensagens (STRINGS ORIGINAIS) ---
      static unsigned long lastStateChange[3] = {0,0,0};
    
      if ((millis() - lastStateChange[sensorIndex]) > 2000) 
        {  
          if (!leituraValida && info.conectadoAnterior) 
            {
              // ✅ String original EXATA
              String msg = "❌ ERRO! Sensor " + tipoSensor + " " + nomeSensor + " desconectado. Requer atenção!\n\n";
              logToWeb(" - " + msg);
              msg += "Desativar alerta ->" + comandoOff;

              if (bot != nullptr)
                {
                  bot->sendMessage(chatId, msg, "");                  
                }  

              a_buzzer(3, 200, 100);

              lastStateChange[sensorIndex] = millis();
            }
            else if (leituraValida && !info.conectadoAnterior) 
                {
                  // ✅ String original EXATA
                  String msg = "✅ Maravilha! Sensor " + tipoSensor + " " + nomeSensor + " atuando. ";                  
                  msg += "Temperatura atual: " + String(temp, 2) + "°C\n";
                  logToWeb(" - " + msg);

                  if (bot != nullptr) // Proteção contra panico. 
                    {
                      bot->sendMessage(chatId, msg, "");
                    }

                  a_buzzer(2, 100, 100);

                  lastStateChange[sensorIndex] = millis();
                }
                else if (leituraValida && temp > tempMax && info.temperaturaNormalAnterior) 
                    {
                    // ✅ String original EXATA
                    String msg = "⚠️ ALERTA! Temperatura " + tipoSensor + " " + nomeSensor + " ultrapassou o limite de " + String(tempMax, 2) + "°C. ";
                    msg += "Valor atual: " + String(temp, 2) + "°C\n\n";
                    msg += "Desativar alerta ->" + comandoOff;
                    logToWeb(" - " + msg);

                    if (bot != nullptr) // Proteção contra panico. 
                      {
                        bot->sendMessage(chatId, msg, "");
                      }

                    a_buzzer(2, 150, 150);

                    lastStateChange[sensorIndex] = millis();
                  }
                  else if (leituraValida && temp <= tempMax && !info.temperaturaNormalAnterior) 
                      {
                        // ✅ String original EXATA
                        String msg = "✅ Temperatura " + tipoSensor + " " + nomeSensor + " normalizou. ";                        
                        msg += "Valor atual: " + String(temp, 2) + "°C\n";
                        logToWeb(" - " + msg);

                        if (bot != nullptr) // Proteção contra panico. 
                          {
                            bot->sendMessage(chatId, msg, "");
                          }

                        a_buzzer(1, 100, 100);

                        lastStateChange[sensorIndex] = millis();
                      }
        }

        // Atualiza estados
        info.conectadoAnterior = conectadoAtual;
        info.temperaturaNormalAnterior = tempNormalAtual;
        info.ultimaTemperatura = temp;

      };

        // Verificação dos sensores (mantendo suas strings originais)
        verificarSensor(ambienteInfo, temp_ambi, ambiC, temp_ref_ambi, flag_pisc_red_0, 
                      "ambiente", "          /off_sensor_ambi", "do", rele_reser_1, bot, idBuffer, 
                      sensor, counter_alert_ambi_0, 0);

        verificarSensor(nutricaoInfo, temp_nutri, nutriC, temp_ref_nutri, flag_pisc_red_1, 
                      "nutrição", "          /off_sensor_nutri", "da", rele_reser_2, bot, idBuffer, 
                      sensor, counter_alert_nutri_0, 1);

        verificarSensor(dissipadorInfo, temp_diss, dissC, temp_ref_diss, flag_pisc_red_2,
                      "dissipador", "          /off_sensor_diss", "do", -1, bot, idBuffer, 
                      sensor, counter_alert_diss_0, 2);

        if (!flag_pisca_wifi) digitalWrite(led_stat_erro_func, LOW);
    }

void desativarTodosAlertas()      // Função desativa todos os alertas. 
    {
      alerta_ambi_ativo = false;
      alerta_nutri_ativo = false;
      alerta_diss_ativo = false;
      salvarEstadoAlertas(); // Salva as alterações na memória

      enviarMensagem("🔕 Todos os alertas foram desativados.");
      //bot->sendMessage(idBuffer, "🔕 Todos os alertas foram desativados.", "");
    }

void salvarEstadoAlertas()      // Salva alertas na memoria. 
    {
      Memory.begin("alertas", false); // "alertas" = namespace, false = modo leitura/escrita
      Memory.putBool("ambi_ativo", alerta_ambi_ativo);
      Memory.putBool("nutri_ativo", alerta_nutri_ativo);
      Memory.putBool("diss_ativo", alerta_diss_ativo);
      Memory.end();
    }   

void salvarEstadoFlash()  // Salva informaçãoes na memeoria flash relacioa a função falha_sensor.
    {
      Memory.begin("espaco_1", false);
      Memory.putBool("flag_pisc_red_0", flag_pisc_red_0);
      Memory.putBool("flag_pisc_red_1", flag_pisc_red_1);
      Memory.putBool("flag_pisc_red_2", flag_pisc_red_2);
      Memory.end();
    } 

void exibirTemperaturaLCD(byte linha, const char* nome, float temperatura) // Função que exibe as temperaturas no LCD.
    {
      lcd.setCursor(0, linha);
      lcd.print(nome);
      if (temperatura < -120.0) 
        {
          lcd.setCursor(12, linha);
          lcd.print("ERRO!");
          lcd.setCursor(18, linha);
          lcd.print("   "); // Limpa a parte da unidade
        } 
        else if (temperatura <= 84) 
            {
              lcd.setCursor(12, linha);
              lcd.print(temperatura);
              lcd.setCursor(18, linha);
              lcd.print((char) 223); // Símbolo de grau Celsius (consistente)
              lcd.setCursor(19, linha);
              lcd.print('C');
            } 
            else 
              {
                lcd.setCursor(12, linha);
                lcd.print("ALTA!"); // Indica temperatura fora da faixa esperada
                lcd.setCursor(18, linha);
                lcd.print("   ");
              }
    }

void lcd_info()      // Função que envia dados para o LCD.
    {
      if (block_empty) 
        {
          if (block_lcd_info && block_full) 
            {
              if (flag_clear_lcd_info) 
                {
                  flag_clear_lcd_info = false;
                  flag_alert_wifi_lcd = true;
                  lcd.clear();
                }
            }

            sensor.requestTemperatures(); // Solicita as temperaturas uma vez
            delay(190); // Exemplo para resolução padrão (10 bits) 

            // Sensor Ambiente
            float temp_ambiente = savedAdjCorAmb + sensor.getTempC(temp_ambi);
            exibirTemperaturaLCD(0, "TEMP. AMBI:", temp_ambiente);

            // Sensor Nutrição
            float temp_nutricao = savedAdjCorNut + sensor.getTempC(temp_nutri);
            exibirTemperaturaLCD(1, "TEMP. NUTR:", temp_nutricao);

            // Sensor Dissipador
            float temp_dissipador = savedAdjCorDis + sensor.getTempC(temp_diss);
            exibirTemperaturaLCD(2, "TEMP. DISS:", temp_dissipador);
        }
    }

bool blockInvald()      // Função que bloqueia comandos e sistema total.
    {
      if (!on_off_geral) 
        {
          enviarMensagem(system_disa);
          //bot->sendMessage(idBuffer, system_disa, "");
          return false;
        }

      if (!block_cmd_0) 
        {
          enviarMensagem(cmd_invalid);
          //bot->sendMessage(idBuffer, cmd_invalid, "");
          return false;
        }

        return true;
    } 

void local_ender()
    {
      if (!flag_conf_sens) return;

      delay(2500);
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("VAMOS CONFIGURAR OS");
      lcd.setCursor(0, 1); lcd.print("SENSORES P/ CONTROLE");
      lcd.setCursor(0, 2); lcd.print("DAS TEMPERATURAS.");
      delay(3000);

      digitalWrite(led_ciclo_15X30, LOW);
      digitalWrite(led_stat_rele, LOW);
      digitalWrite(led_stat_erro_func, LOW);

      if (inc_pos_sensor == 1)
        configurarSensor(addr, "DE TEMP. AMBIENTE", "SaveAdressAmbi", auxil_tempC_addr, 1);

      if (inc_pos_sensor == 2)
        configurarSensor(addr1, "DA TEMP. NUTRICAO", "SaveAdressNutri", auxil_tempC_addr1, 2);

      if (inc_pos_sensor == 3) 
        {
          configurarSensor(addr2, "DA TEMP. DISSIPADOR", "SaveAdressDiss", auxil_tempC_addr2, 3);

          inc_pos_sensor = 0;
          flag_conf_sens = false;

          Sensores.begin("sensor_5", false);
          Sensores.putBool("flag_conf_sens", flag_conf_sens);
          Sensores.end();

          lcd.clear();
          lcd.setCursor(0, 0); lcd.print("SEU DISPOSITIVO VAI");
          lcd.setCursor(0, 1); lcd.print("SER REINICIADO PARA");
          lcd.setCursor(0, 2); lcd.print("CONCLUIR A CONFIGU- ");
          lcd.setCursor(0, 3); lcd.print("RACAO DOS SENSORES.");
          delay(2800);

          ESP.restart();
        }
    }

void configurarSensor(byte *endereco, const char* label, const char* nomeMemoria, float &tempAux, uint8_t posSensorEsperada) 
    {
      lcd.clear();
      
      while (endereco[0] != 40) 
           {
             lcd.setCursor(1, 1);
             lcd.print("CONECTE O SENSOR");
             lcd.setCursor(1, 2);
             lcd.print(label);

             oneWire.reset_search();

             if (!oneWire.search(endereco)) 
               {
                 Serial.printf("Não foi possível encontrar o endereço para %s\n", nomeMemoria);
                 logToWeb(String(" - Erro ao localizar: ") + nomeMemoria);
               }

              delay(1500);

             if (endereco[0] == 40) 
               {
                 lcd.clear();
                 lcd.setCursor(0, 0);
                 lcd.print("SENSOR ENCONTRADO");
                 lcd.setCursor(0, 2);
                 lcd.print("CONFIGURANDO");

                 barprog_senso();

                 delay(1200);

                 lcd.clear();
                 lcd.setCursor(1, 1);
                 lcd.print("SENSOR CONFIGURADO");
                 lcd.setCursor(4, 2);
                 lcd.print("COM SUCESSO!");

                 delay(3000);

                 lcd.clear();
 
                 // Aguarda o usuário desconectar para seguir

                 waitConnSensor++;

                 if (waitConnSensor < 3)
                   {
                     while (inc_pos_sensor == posSensorEsperada) 
                          {
                            lcd.setCursor(0, 1);
                            lcd.print("DESCONECTE O SENSOR!");
                            lcd.setCursor(0, 2);
                            lcd.print("CONFIGURE O PROXIMO");

                            tempAux = sensor.getTempC(endereco);

                            if (tempAux < -125) inc_pos_sensor++;
                          }
                   } 

                    // Salva o endereço na flash
                    Sensores.begin("sensor_5", false);
                    Sensores.putBytes(nomeMemoria, (byte*)(endereco), sizeof(DeviceAddress));
                    Sensores.end();

                    delay(2000);
               }
           }
    }

void descr_DS18B20()       // Função que descreve ossensores DS18B20.
    {
      if (testa_18B20 == true)  // Ativar ou desativa a descrição do teste.
        {
          string_dinamica(test_18B20_0); 
        }
        else 
          {
            string_dinamica(test_18B20_1);
          }
    }

void test_sensores()       // Função que analisa enores para avaliar se é original.
    {
      enviarMensagem("Buscando sensores... aguarde!");
      //bot->sendMessage(idBuffer, F("Buscando sensores... aguarde!"), "");

      logToWeb(" - Buscando sensores... aguarde!\n");      
      
      //String teste_senso = "Ok, sensores encontrados:\n\n";
      //logToWeb(" - Ok, sensores encontrados:\n");

      delay(1200);

      // Testa cada sensor
      testarSensor("ambiente", temp_ambi, teste_senso);
      testarSensor("nutrição", temp_nutri, teste_senso);
      testarSensor("dissipador", temp_diss, teste_senso);

      //String teste_senso = "Ok, sensores encontrados:\n\n";
      logToWeb(" - Ok, sensores encontrados:\n");

      // Mensagem final
      //teste_senso += "Fim do teste dos sensores.";
      //bot->sendMessage(idBuffer, teste_senso, "");

      //String horarioStr = "";

        /*if (rtcOk) 
          {
            DateTime now = rtc.now();
            char horario[20];
            sprintf(horario, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
            horarioStr = "[" + String(horario) + "]";
          }*/

          //String mensagemFinal = horarioStr;

      //logToWeb(mensagemFinal + teste_senso);  // envia tudo de uma vez só
      logToWeb(teste_senso);  // envia tudo de uma vez só

      if (bot != nullptr)
        {
          bot->sendMessage(idBuffer, teste_senso, "");
        }

        teste_senso = " "; // Limpa string.
    }

void testarSensor(const String& tipo, byte* temp_sensor, String& resultado)       // Função auxiliar para testar um sensor específico
    {
      String endereco = formatarEndereco(temp_sensor);
      float temperatura = sensor.getTempC(temp_sensor);

      resultado += " - Serial do sensor do " + tipo + ": ";        
        
      if (temperatura == DEVICE_DISCONNECTED_C) 
        {
          resultado += "0:0:0:0:0:0:0:0, não foi possível localizar o sensor do " + tipo + ". Por favor, verifique o problema.\n\n"; 
          return;
        }

      resultado += endereco + ".\n";        

      // Verifica se é original (40:0:0 no início e 0:0 no final)
      if (temp_sensor[0] == 40 && temp_sensor[5] == 0 && temp_sensor[6] == 0) 
        {
          resultado += "PARABÉNS! 👏 Seu sensor do " + tipo + " é original.\n"; 
              
          Serial.println();
          Serial.print("Family: ");            
          String family = ds18b20_name(&oneWire, 0);            
          Serial.println(family);            
              
          resultado += "Família: " + family + ".\n\n";                         

        } 
        else 
          {
            resultado += "LAMENTO! 😞 Seu sensor do " + tipo + " é um clone, não é original. Substitua-o imediatamente.\n\n";                
          }              
    }

String formatarEndereco(byte* temp_sensor)       // Função para formatar o endereço do sensor
    {
        String endereco;

        for (int i = 0; i < 8; i++) 
           {
            String varHexa = String(temp_sensor[i], HEX);
            if (varHexa.length() == 1) varHexa = "0" + varHexa; // Adiciona zero à esquerda
            endereco += varHexa;
            if (i < 7) endereco += ":";
           }
          
           endereco.toUpperCase();
           return endereco;
    }

void black_light_activ()       // Função que acende o backlight por tempo determinado.
    {
      if (active_light == false)
        {          
          lcd.setBacklight(HIGH);
        }

      if (active_light == true)
        {
          if (on_backlig == true)
            {               
              on_backlig = false;               
              flag_blklight = true; //  Ativa contador.
            }       
          
          if (flag_counter_blackluz == true)
            {  
              flag_counter_blackluz = false;               
              lcd.noBacklight();    // Apaga o backlight.
            }          
        }
    }

void blackout()       // Função que verifica se houve queda de energia.
    {
      if (flag_blackout == true)  // Está flag está localizada na flash, vai ser sempre true, até o proximo ciclo de desliga.
        {          
          flag_cont_blackout = true;  // Habilita o contador do blakcout.          
          
          if (flag_on_irrig == true)  // Habilita essa flag dentro do contador de pois de 30 segundos.
            {
              if ((T_horario >= "06:00:00") && (T_horario <= "17:59:59")) 
                {
                  // Verifica do minuto 0 a té o minuto 10.
                  if (((T_minuto >= minuto[0]) && (T_minuto < minuto[5])) && flag_trava_blackout0 == true)  // Verifica no intervalo de 10 minutos se houve queda de energia.
                    {
                      flag_trava_blackout0 = false;                      

                      digitalWrite(rele_irriga, HIGH);  // Liga bomba para irrigação.                      
                      digitalWrite(led_stat_rele, HIGH);

                      flag_cont_blackout = false;  // Desabilita o contador do blakcout.
                      flag_on_irrig = false;
                      counter_blackout = 0; // Zera contador do flag_cont_blackout.
                      flag_blackout = false; 

                      off_sound = true;  // Ativa aviso sonoro após desligamento.

                      if ((on_off_bip_irri == true) && (on_off_all_bip == true)) a_buzzer(1, 200, 200);                           
                    }

                  //  Verifica do minuto 30 a té o minuto 40.
                  if (((T_minuto >= minuto[2]) && (T_minuto < minuto[6])) && flag_trava_blackout1 == true)  // Verifica no intervalo de 10 minutos se houve queda de energia.
                    {
                      flag_cont_blackout = false;  // Desabilita o contador do blakcout. 
                      
                      digitalWrite(rele_irriga, HIGH);  // Liga bomba para irrigação.                      
                      digitalWrite(led_stat_rele, HIGH);
                      
                      flag_on_irrig = false;
                      counter_blackout = 0; // Zera contador do flag_cont_blackout.
                      flag_blackout = false; 

                      off_sound = true;  // ativa aviso sonoro após desligamento.

                      if ((on_off_bip_irri == true) && (on_off_all_bip == true)) a_buzzer(1, 200, 200);
                    }  
                }            

              if (((T_horario >= "18:00:00") && (T_horario <= "23:59:59")) || ((T_horario >= "00:00:00") && (T_horario <= "05:59:59")))
                {  
                      if (((T_minuto >= minuto[0]) && (T_minuto < minuto[5])) && flag_trava_blackout2 == true)  // Verifica no intervalo de 10 minutos se houve queda de energia.
                        {
                          flag_trava_blackout2 = false;                      

                          digitalWrite(rele_irriga, HIGH);  // Liga bomba para irrigação.  // Liga bomba para irrigação.
                          
                          digitalWrite(led_stat_rele, HIGH);

                          flag_cont_blackout = false;  // Desabilita o contador do blakcout.
                          flag_on_irrig = false;
                          counter_blackout = 0; // Zera contador do flag_cont_blackout.
                          flag_blackout = false; 

                          off_sound = true;  // Ativa aviso sonoro após desligamento.

                          if ((on_off_bip_irri == true) && (on_off_all_bip == true)) a_buzzer(1, 200, 200);
                        }
                
                      if (((T_minuto >= minuto[0]) && (T_minuto < minuto[5])) && flag_trava_blackout3 == true)  // Verifica no intervalo de 00 a 10 minutos se houve queda de energia.
                        {
                          flag_trava_blackout3 = false;                      

                          digitalWrite(rele_irriga, HIGH);  // Liga bomba para irrigação.  // Liga bomba para irrigação.                          
                          digitalWrite(led_stat_rele, HIGH);

                          flag_cont_blackout = false;  // Desabilita o contador do blakcout.
                          flag_on_irrig = false;
                          counter_blackout = 0; // Zera contador do flag_cont_blackout.
                          flag_blackout = false; 

                          off_sound = true;  // Ativa aviso sonoro após desligamento.

                          if ((on_off_bip_irri == true) && (on_off_all_bip == true)) a_buzzer(1, 200, 200);
                        }

                      if (((T_minuto >= minuto[3]) && (T_minuto < minuto[7])) && flag_trava_blackout4 == true)  // Verifica no intervalo de 45 a 55 minutos se houve queda de energia.
                        {
                          flag_trava_blackout4 = false;                      

                          digitalWrite(rele_irriga, HIGH);  // Liga bomba para irrigação.  // Liga bomba para irrigação.                          
                          digitalWrite(led_stat_rele, HIGH);

                          flag_cont_blackout = false;  // Desabilita o contador do blakcout.
                          flag_on_irrig = false;
                          counter_blackout = 0; // Zera contador do flag_cont_blackout.
                          flag_blackout = false; 

                          off_sound = true;  // Ativa aviso sonoro após desligamento.

                          if ((on_off_bip_irri == true) && (on_off_all_bip == true)) a_buzzer(1, 200, 200);
                        }

                      if (((T_minuto >= minuto[2]) && (T_minuto < minuto[6])) && flag_trava_blackout5 == true)  // Verifica no intervalo de 15 a 25 minutos se houve queda de energia.
                        {
                          flag_trava_blackout5 = false;                      

                          digitalWrite(rele_irriga, HIGH);  // Liga bomba para irrigação.  // Liga bomba para irrigação.                          
                          digitalWrite(led_stat_rele, HIGH);

                          flag_cont_blackout = false;  // Desabilita o contador do blakcout.
                          flag_on_irrig = false;
                          counter_blackout = 0; // Zera contador do flag_cont_blackout.
                          flag_blackout = false; 

                          off_sound = true;  // Ativa aviso sonoro após desligamento.

                          if ((on_off_bip_irri == true) && (on_off_all_bip == true)) a_buzzer(1, 200, 200);
                        }

                      if (((T_minuto >= minuto[1]) && (T_minuto < minuto[8])) && flag_trava_blackout6 == true)  // Verifica no intervalo de 15 a 25 minutos se houve queda de energia.
                        {
                          flag_trava_blackout6 = false;                      

                          digitalWrite(rele_irriga, HIGH);  // Liga bomba para irrigação.  // Liga bomba para irrigação.                          
                          digitalWrite(led_stat_rele, HIGH);

                          flag_cont_blackout = false;  // Desabilita o contador do blakcout.
                          flag_on_irrig = false;
                          counter_blackout = 0; // Zera contador do flag_cont_blackout.
                          flag_blackout = false; 

                          off_sound = true;  // Ativa aviso sonoro após desligamento.

                          if ((on_off_bip_irri == true) && (on_off_all_bip == true)) a_buzzer(1, 200, 200);
                        }                     
                }
            }
        }
    }

void barprog_senso()       // Função de animações do barra de progresso.
    {
      // Desenha as bordas da barra de progresso /// desfazer até    áqui
      lcd.createChar(0, bord_esq);
      lcd.setCursor(12, 2);
      lcd.write(0);
      
      for (bords = 13; bords < 19; bords ++)
         {
           lcd.createChar(1, bord);
           lcd.setCursor(bords, 2);
           lcd.write(1);
         }
         bords = 0;

         lcd.createChar(2, bord_dir);
         lcd.setCursor(19, 2);
         lcd.write(2);

         delay(1200);

         for (bar_prog = 13; bar_prog < 19; bar_prog++) 
            {
              lcd.createChar(3, barra_pro);
              lcd.setCursor(bar_prog, 2);
              lcd.write(3);

              delay(400);
            }

            bar_prog = 0;
    }

void on_flag_blackout()       // Função que armazena status ligado da irrigação.
    {
      flag_blackout = true;  // Flag que vai ativar a verificação de queda de energia.
      
      Memory.begin("espaco_1", false); // Abre a memória para gravação. // Abre a memória para gravação.
      Memory.putBool("flag_blackout", flag_blackout);
      Memory.end();
  
      flag_blackout = false;  // Flag que vai ativar a verificação de queda de energia.
    }  

void off_flag_blackout()       // Função que armazena status desligado da irrigação. 
    {
      flag_blackout = false;  // Flag que vai ativar a verificação de queda de energia.
      
      Memory.begin("espaco_1", false); // Abre a memória para gravação. // Abre a memória para gravação.
      Memory.putBool("flag_blackout", flag_blackout);
      Memory.end();
    }

void a_buzzer(byte num_vezes, int tempo_ativ, int tempo_inat)       // Função para buzzer (nvz, temp ati, tem inat)
    {
      for (byte a_buz  = 0; a_buz < num_vezes; a_buz ++) 
         {              
           // Liga o buzzer
           digitalWrite(buzzer, HIGH);
           delay(tempo_ativ); // Mantém o buzzer ligado por tempoAtivo milissegundos

           // Desliga o buzzer
           digitalWrite(buzzer, LOW);
           delay(tempo_inat); // Mantém o buzzer desligado por tempoInativo milissegundos             
         }
    }

void setForceBoot(int type)       // Função auxiliar que força a reinicializações de varios pontos do codigo.
    {
      pendingBoot = type;
    }    

void force_boot()       // Função que força a reinicializações de varios pontos do codigo.
    {
      if (pendingBoot != BOOT_NONE) 
        {
          if (pendingBoot == BOOT_GENERIC) 
            {
              Serial.println("Forçando reinicialização genérica...");
              delay(1500);
              ESP.restart();
            } 
            else if (pendingBoot == BOOT_RESET_WIFI) 
                {
                  Serial.println("Forçando reinicialização com reset das configurações WiFi...");                  
                  wifiManager.resetSettings();                  
                  
                  ESP.restart();
                } 
                else if (pendingBoot == BOOT_TELEGRAM) 
                    {
                      if (sendMessage != nullptr) 
                        {
                          sendMessage(idBuffer, "OK! Reiniciando agora, aguarde...", "");
                          delay(250);
                          sendMessage(idBuffer, "3", "");
                          delay(100);
                          sendMessage(idBuffer, "2", "");
                          delay(100);
                          sendMessage(idBuffer, "1", "");
                          delay(100);
                          sendMessage(idBuffer, "Reiniciando...", "");
                          delay(100);
                        }
                        
                        lcd.clear();
                        lcd.setCursor(0, 3);
                        lcd.print("REINICIANDO");
                        digitalWrite(led_stat_erro_func, LOW);
                        delay(1250);

                        for (byte points = 11; points < 16; points++) 
                           {
                             lcd.setCursor(points, 3);
                             lcd.print(".");                             
                           }
                  
                           ESP.restart(); // Segurança
                    }     
                    else if (pendingBoot == BOOT_RESTORE) 
                        {                          
                          restore();                          
                        }
                        else if (pendingBoot == BOOT_UPGRADE)
                            {                              
                              upDateFirmware();
                            }

                        pendingBoot = BOOT_NONE; // Reset
        }

        // Verifique as flags e defina o pendingBoot
        if (forceBoot_gene) 
          {
            setForceBoot(BOOT_GENERIC);
            forceBoot_gene = false;
          }

        if (forceBoot_0) 
          {
            setForceBoot(BOOT_RESET_WIFI);
            forceBoot_0 = false;
          }

        if (forceBoot_1) 
          {
            setForceBoot(BOOT_TELEGRAM);
            forceBoot_1 = false;
          }

        if (forceBoot_2) 
          {
            setForceBoot(BOOT_RESTORE);
            forceBoot_2 = false;
          }

        if (forceBoot_3) 
          {
            setForceBoot(BOOT_UPGRADE);
            forceBoot_3 = false;
          } 
    } 

void string_dinamica(const char *mensagem) 
    {
      if (bot == nullptr || idBuffer == nullptr || strlen(idBuffer) == 0) 
        {
          Serial.println("Bot ou ID não inicializado.");
          return;
        }

      size_t tamanho_mensagem = strlen(mensagem);
      char *mensagem_dinamica = (char *)malloc(tamanho_mensagem + 1);

      if (mensagem_dinamica == nullptr) 
        {
          Serial.println("Erro: falha ao alocar memória.");
          return;
        }

      strcpy(mensagem_dinamica, mensagem);
      Serial.println("Enviando mensagem via bot...");
      Serial.println(mensagem_dinamica);

      enviarMensagem(mensagem_dinamica);
      free(mensagem_dinamica);

      //bot->sendMessage(idBuffer, mensagem_dinamica, "");
    }

void scanI2CBus()       // Função que escaneia o barramento I2C.
    {
      byte error, address;
      int nDevices;
      String message = "Resultado do Scanner I2C:\n\n";
      bool devicesFound = false;

      Serial.println("Iniciando escaneamento I2C..."); // Mensagem de status
      logToWeb(" - Iniciando escaneamento I2C...\n");

      nDevices = 0;

      for (address = 1; address < 127; address++ ) 
         {
            Wire.beginTransmission(address);
            error = Wire.endTransmission();

            if (error == 0) 
              {
                String deviceAddress = String("0x");

                if (address < 16) 
                  {
                    deviceAddress += "0";
                  }
                  deviceAddress += String(address, HEX);
                  message += "Dispositivo encontrado no endereco: " + deviceAddress + "\n\n";
                  logToWeb(" - Dispositivo encontrado no endereco: " + deviceAddress + ".\n");

                  // Serial.println("Dispositivo I2C encontrado no endereco 0x" + String(address, HEX)); // Removido ou comentado para nao poluir o Serial Monitor
                  nDevices++;
                  devicesFound = true;

                  delay(250);
              } 
              else if (error == 4) 
                  {
                    String deviceAddress = String("0x");
                    if (address < 16) 
                      {
                        deviceAddress += "0";
                      }

                      deviceAddress += String(address, HEX);
                      message += "Erro desconhecido no endereco: " + deviceAddress + "\n\n";
                      // Serial.println("Erro desconhecido no endereco 0x" + String(address, HEX)); // Removido ou comentado
                  }
         }

         if (!devicesFound) 
           {
             message += "Nenhum dispositivo I2C encontrado.\n\n";


             // Serial.println("Nenhum dispositivo I2C encontrado."); // Removido ou comentado
           }

           Serial.println("Enviando mensagem para o Telegram..."); // Mensagem de status

           enviarMensagem(message.c_str());
           //bot->sendMessage(idBuffer, message);

           Serial.println("Mensagem enviada."); // Mensagem de status

           Serial.println("Escaneamento I2C finalizado.\n"); // Mensagem de status
    }

void loadValuesFromFlash()       // Função responsável em quandar parametros do Telegram ao ser enviados por web.
    {
      Telegram.begin("token_id", false); // Abre o namespace "telegram"
      String savedToken = Telegram.getString("newToken", "");
      String savedId = Telegram.getString("newId", "");
      
      strncpy(tokenBuffer, savedToken.c_str(), sizeof(tokenBuffer) - 1); // Copia os valores para os buffers
      strncpy(idBuffer, savedId.c_str(), sizeof(idBuffer) - 1);

      Telegram.end(); // Fecha o namespace
    }

char* montarMensagemConexao()       // Função responsavel por montar strings para enviar ao bot.
    {
        String local_ip_str = WiFi.localIP().toString();
        String gateway_str = WiFi.gatewayIP().toString();
        String subnet_str = WiFi.subnetMask().toString();
        String macaddr_str = WiFi.macAddress();
        String SSID_str = WiFi.SSID();
        String BSSIDstr_str = WiFi.BSSIDstr();
        String psk_senha_str = WiFi.psk();
        String DNS_0_str = WiFi.dnsIP().toString();

        char RSSI_i[10];
        sprintf(RSSI_i, "%d", WiFi.RSSI());

        char local_ip[20];
        strcpy(local_ip, local_ip_str.c_str());

        char gateway[20];
        strcpy(gateway, gateway_str.c_str());

        char subnet[20];
        strcpy(subnet, subnet_str.c_str());

        char macaddr[20];
        strcpy(macaddr, macaddr_str.c_str());

        char SSID[33];
        strcpy(SSID, SSID_str.c_str());

        char BSSIDstr[20];
        strcpy(BSSIDstr, BSSIDstr_str.c_str());

        char psk_senha[64];
        strcpy(psk_senha, psk_senha_str.c_str());

        char DNS_0[20];
        strcpy(DNS_0, DNS_0_str.c_str());

        // Calcula o tamanho total necessário para a string resultante
        int tamanhoTotal = strlen("Tudo certo com a conexão entre o dispositivo de controle e o WI-FI!\n\nSegue dados de conexão de rede:\n\n") +
                           strlen("Endereço IP: ") + strlen(local_ip) + strlen("\n") +
                           strlen("Gateway padrão: ") + strlen(gateway) + strlen("\n") +
                           strlen("Mascara de sub-rede: ") + strlen(subnet) + strlen("\n") +
                           strlen("Endereço DNS: ") + strlen(DNS_0) + strlen("\n") +
                           strlen("Mac address do dispositivo: ") + strlen(macaddr) + strlen("\n") +
                           strlen("Mac address do Wi-Fi: ") + strlen(BSSIDstr) + strlen("\n") +
                           strlen("SSID da rede: ") + strlen(SSID) + strlen("\n") +
                           strlen("Senha: ") + strlen(psk_senha) + strlen("\n") +
                           strlen("Intensidade do sinal: ") + strlen(RSSI_i) + strlen(" dBm\n\n") +
                           strlen("Indicador de Qualidade:\n\n") +
                           strlen("-30 dBm: Sinal excelente (muito forte)\n") +
                           strlen("-50 a -60 dBm: Sinal ótimo (muito bom)\n") +
                           strlen("-70 dBm: Sinal adequado (bom)\n") +
                           strlen("-80 dBm: Sinal fraco (ruim)\n") +
                           strlen("-90 dBm ou menor: Sinal muito ruim (fraca ou instável)") + 1; // +1 para o caractere nulo terminador

        // Aloca memória para a string resultante
        char* rede_conex = (char*)malloc(tamanhoTotal);

        if (rede_conex == nullptr) 
          {
            // Falha na alocação de memória
            return nullptr;
          }

        // Concatena as strings usando strcpy e strcat
        strcpy(rede_conex, "Tudo certo com a conexão entre o dispositivo de controle e o WI-FI!\n\nSegue dados de conexão de rede:\n\n");
        strcat(rede_conex, "Endereço IP: ");
        strcat(rede_conex, local_ip);
        strcat(rede_conex, "\n");
        strcat(rede_conex, "Gateway padrão: ");
        strcat(rede_conex, gateway);
        strcat(rede_conex, "\n");
        strcat(rede_conex, "Mascara de sub-rede: ");
        strcat(rede_conex, subnet);
        strcat(rede_conex, "\n");
        strcat(rede_conex, "Endereço DNS: ");
        strcat(rede_conex, DNS_0);
        strcat(rede_conex, "\n");
        strcat(rede_conex, "Mac address do dispositivo: ");
        strcat(rede_conex, macaddr);
        strcat(rede_conex, "\n");
        strcat(rede_conex, "Mac address do Wi-Fi: ");
        strcat(rede_conex, BSSIDstr);
        strcat(rede_conex, "\n");
        strcat(rede_conex, "SSID da rede: ");
        strcat(rede_conex, SSID);
        strcat(rede_conex, "\n");
        strcat(rede_conex, "Senha: ");
        strcat(rede_conex, psk_senha);
        strcat(rede_conex, "\n");
        strcat(rede_conex, "Intensidade do sinal: ");
        strcat(rede_conex, RSSI_i);
        strcat(rede_conex, " dBm\n\n");
        strcat(rede_conex, "Indicador de Qualidade:\n\n");
        strcat(rede_conex, "-30 dBm: Sinal excelente (muito forte)\n");
        strcat(rede_conex, "-50 a -60 dBm: Sinal ótimo (muito bom)\n");
        strcat(rede_conex, "-70 dBm: Sinal adequado (bom)\n");
        strcat(rede_conex, "-80 dBm: Sinal fraco (ruim)\n");
        strcat(rede_conex, "-90 dBm ou menor: Sinal muito ruim (fraca ou instável)");

        return rede_conex;
    }

void saveValuesToFlash(const String& newToken, const String& newId)       // Função responsável em salvar parametros do Telegram na flash.
    {
      Telegram.begin("token_id", false); // Abre o namespace "telegram"
      Telegram.putString("newToken", newToken);
      Telegram.putString("newId", newId);
      Telegram.end(); // Fecha o namespace
    }

void handleRoot(AsyncWebServerRequest *request)  // Função de rota de envio da página.
    {
      // Envia o HTML diretamente da memória PROGMEM
      // A função send_P é otimizada para isso.
      request->send_P(200, "text/html", PAGE_Telegram);

      // Não é necessário usar malloc(), free() ou generateHTML()
    }

void handlerStatusRede(AsyncWebServerRequest *request)  // Função que escaneia dados da rede e exibe no navegador.
    {
      String htmlContent = Stat_rede;     
      String ssid_val = "Não Conectado";
      String localIP_val = "N/A";
      String subnet_val = "N/A";
      String gateway_val = "N/A";
      String publicIP_val = "Obtendo..."; // Placeholder inicial
      String rssi_val = "N/A";
      String macaddr_str = "N/A";
      String DNS_0_str = "N/A";

      if (WiFi.status() == WL_CONNECTED) 
        {
          ssid_val = WiFi.SSID();
          localIP_val = WiFi.localIP().toString();
          subnet_val = WiFi.subnetMask().toString();
          gateway_val = WiFi.gatewayIP().toString();
          rssi_val = String(WiFi.RSSI());
          macaddr_str = WiFi.macAddress();
          DNS_0_str = WiFi.dnsIP().toString();

          publicIP_val = getPublicIP(); // Função que obtém IP público (deve ser síncrona ou retornar string)
        } 
        else 
          {
            publicIP_val = "WiFi Desconectado"; // Mensagem se não houver WiFi
          }

      htmlContent.replace("{{SSID}}", ssid_val);
      htmlContent.replace("{{LOCAL_IP}}", localIP_val);
      htmlContent.replace("{{SUBNET}}", subnet_val);
      htmlContent.replace("{{GATEWAY}}", gateway_val);
      htmlContent.replace("{{MACADDRESS}}", macaddr_str);
      htmlContent.replace("{{DNS}}", DNS_0_str);
      htmlContent.replace("{{PUBLIC_IP}}", publicIP_val);
      htmlContent.replace("{{RSSI}}", rssi_val);

      request->send(200, "text/html", htmlContent);
    } 

String getPublicIP()       // Função que busca o IP da internet.
    {
      if (WiFi.status() == WL_CONNECTED) 
        {
          HTTPClient http; // Crie uma instância do cliente HTTP
    
          const char* publicIpServiceUrl = "http://api.ipify.org/"; // URL do serviço que retorna o IP público
          http.begin(publicIpServiceUrl);    
          int httpCode = http.GET(); // Faz a requisição GET

          // Verifica o código de resposta HTTP
          if (httpCode == HTTP_CODE_OK) 
            { // HTTP_CODE_OK é 200
              String publicIP = http.getString(); // Obtém o corpo da resposta (que é o IP)
              http.end(); // Fecha a conexão
              return publicIP; // Retorna o IP obtido
            } else 
                {
                  // Houve um erro na requisição
                  String errorMsg = "Erro HTTP (" + String(httpCode) + ")";
                  Serial.println(errorMsg); // Opcional: imprimir erro no Serial
                  http.end(); // Fecha a conexão
                  return errorMsg; // Retorna uma mensagem de erro
                }
          } else    
              {
                // Não há conexão Wi-Fi
                return "WiFi Desconectado";
              }
    }

void handleSave(AsyncWebServerRequest *request)  // Função recebe dados da web e guarda na flash.
    {
      if (request->hasParam("token", true) && request->hasParam("id", true)) // true para buscar no corpo POST
        {
          String newToken = request->getParam("token", true)->value();
          String newId = request->getParam("id", true)->value();

          saveValuesToFlash(newToken, newId); // Salva os valores na memória flash

          strncpy(tokenBuffer, newToken.c_str(), sizeof(tokenBuffer) - 1); // Atualiza os buffers
          tokenBuffer[sizeof(tokenBuffer) - 1] = '\0'; // Garante terminação
          strncpy(idBuffer, newId.c_str(), sizeof(idBuffer) - 1);
          idBuffer[sizeof(idBuffer) - 1] = '\0';

          if (bot == nullptr) // Inicializa o objeto bot se ainda não estiver inicializado
            {
              bot = new UniversalTelegramBot(tokenBuffer, client);
              Serial.println("Bot inicializado com sucesso!");
              logToWeb(" - Bot inicializado com sucesso!");
            }

            request->send_P(200, "text/html", PAGE_Save_Success); // Envia página de sucesso
        }
        else
          {
            request->send_P(400, "text/html", PAGE_Save_Fail); // Envia página de erro
          }
    }

void handleUpload(AsyncWebServerRequest *request)  // Envia a página para upload do firmware
    {
      request->send(200, "text/html", uploadPage); // pronto
    }

void handleUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) 
    {
        static bool updating = false;

        if (index == 0) 
          {
            Serial.printf("Iniciando upload: %s\n", filename.c_str());

            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) 
              {
                Update.printError(Serial);
                request->send(500, "text/plain", "Erro ao iniciar a atualização.");
                updating = false;
                return;
              }

              updating = true;
          }

        if (!updating) 
          {
            // Se não iniciou update, ignorar
            return;
          }

        if (len > 0) 
          {
              if (Update.write(data, len) != len) 
                {
                  Update.printError(Serial);
                  request->send(500, "text/plain", "Erro ao gravar o firmware.");
                  updating = false;
                  return;
                }
          }

        if (final) 
          {
            if (Update.end(true)) 
              {
                Serial.printf("Upload concluído. Tamanho: %u bytes\n", index + len);
                request->send(200, "text/plain", "Atualização concluída com sucesso! Reiniciando...");
                delay(1000);
                ESP.restart();
            } 
            else 
              {
                Update.printError(Serial);
                request->send(500, "text/plain", "Erro ao finalizar a atualização.");
              }

              updating = false;
          }
    }

void handleMenu(AsyncWebServerRequest *request) // pronto
    {
      request->send(200, "text/html", menu);
    }

void string_inc_decr(const char* text, float v_value)       // Função que exibe os dados das strings no bot telegram.
    {
      // Exemplo de valor adicional (pode ser removido ou ajustado conforme necessário)
      float aux_val_cor_amb = v_value;

      // Calcula o tamanho máximo necessário para a mensagem final
      size_t len = strlen(text) + 32; // 32 bytes adicionais para o valor numérico e formatação
      char* stat_coman = (char*)malloc(len); // Aloca memória suficiente    

      // Formata a mensagem com o texto e o valor numérico
      snprintf(stat_coman, len, text, v_value);

      enviarMensagem(stat_coman);
      //bot->sendMessage(idBuffer, stat_coman, "");

      // Libera a memória alocada
      free(stat_coman);
    }

void adjustTemperature(float& correctionValue, float baseValue, const char* memoryKey, String description, int temporiza)       // Função processa dados o ajustes das temperaturas.
    {
      temporiza = 0;
      
      // Verifica se o comando é "/incr" ou "/decr"
      if (text.equals("/incr"))
        {
          correctionValue += 0.5; // Incrementa o ajuste acumulado
        }
        else if (text.equals("/decr"))
            {
              correctionValue -= 0.5; // Decrementa o ajuste acumulado
            }

            // Calcula o valor ajustado como referência + ajuste acumulado  namespace
            float adjustedValue = baseValue + correctionValue;

            //auxAmbCorr_maxi = adjustedValue;

            // Monta a mensagem
            String message = description;

      /*if (text.equals("/incr")
        {
          message += " incrementada 0.5 ºC. Total ajustado: " + String(correctionValue, 2) + " ºC.\n";
        }
        else if (text.equals("/decr")
            {
              message += " decrementada 0.5 ºC. Total ajustado: " + String(correctionValue, 2) + " ºC.\n";
            }*/

      String keyCheckStr = String(memoryKey);
      const char* keyCheckCStr = keyCheckStr.c_str();

      sensor.requestTemperatures();
      delay(300); // Exemplo para resolução padrão (10 bits) 
  
      if (strcmp(keyCheckCStr, "CorAmb") == 0)
        {
          ambiC = sensor.getTempC(temp_ambi);
            
          message += "atual no sensor: " + String(ambiC, 2) + " ºC.\n";
          message += "Valor do ajuste: " + formatWithSign(correctionValue) + "ºC.\n";          
        }
         if (strcmp(keyCheckCStr, "CorNut") == 0)
            {
              nutriC = sensor.getTempC(temp_nutri);
              
              message += "atual no sensor: " + String(nutriC, 2) + " ºC.\n";
              message += "Valor do ajuste: " + formatWithSign(correctionValue) + "ºC.\n";
            }
             if (strcmp(keyCheckCStr, "CorDis") == 0)
                {
                  dissC = sensor.getTempC(temp_diss);

                  message += "atual no sensor: " + String(dissC, 2) + " ºC.\n";
                  message += "Valor do ajuste: " + formatWithSign(correctionValue) + "ºC.\n";
                } 
                 if (strcmp(keyCheckCStr, "AmbMax") == 0)
                    {
                      message += "de referência: " + String(savedAmbMax, 2) + " ºC.\n";
                      message += "Valor do ajuste: " + formatWithSign(correctionValue) + "ºC.\n";                          
                    }
                     if (strcmp(keyCheckCStr, "NutMax") == 0)
                        {
                          message += "de referência: " + String(savedNutMax, 2) + " ºC.\n";  
                          message += "Valor do ajuste: " + formatWithSign(correctionValue) + "ºC.\n";    
                        }
                         if (strcmp(keyCheckCStr, "AmbMin") == 0)
                            {
                              message += "de referência: " + String(savedAmbMin, 2) + " ºC.\n";
                              message += "Valor do ajuste: " + formatWithSign(correctionValue) + "ºC.\n"; 
                            }
                            if (strcmp(keyCheckCStr, "NutMin") == 0)                            
                              {
                                message += "de referência: " + String(savedNutMin, 2) + " ºC.\n";
                                message += "Valor do ajuste: " + formatWithSign(correctionValue) + "ºC.\n"; 
                              }
                             
                              message += "Valor atual mais ajuste: " + formatWithSign(adjustedValue) + " ºC.\n";

      enviarMensagem(message.c_str());
      //bot->sendMessage(idBuffer, message.c_str(), "");

      // Salva na memória
      Memory.begin("espaco_1", false);
      Memory.putFloat(memoryKey, correctionValue); // Salva o ajuste acumuladoSeria.
      Memory.putFloat((String(memoryKey) + "K").c_str(), adjustedValue); // Salva o valor ajustado      

      String keyToCheckStr = String(memoryKey) + "K";
      const char* keyToCheckCStr = keyToCheckStr.c_str();

      if (strcmp(keyToCheckCStr, "CorAmbK") == 0)
        {
          savedCorAmb = adjustedValue;
          savedAdjCorAmb = correctionValue; 
          Memory.putFloat("savedCorAmb", savedCorAmb);
          Memory.putFloat("savedAdjCorAmb", savedAdjCorAmb);
        }
        else if (strcmp(keyToCheckCStr, "CorNutK") == 0)
            {
              savedCorNut = adjustedValue;
              savedAdjCorNut = correctionValue;
              Memory.putFloat("savedCorNut", savedCorNut);
              Memory.putFloat("savedAdjCorNut", savedAdjCorNut);
            }
            else if (strcmp(keyToCheckCStr, "CorDisK") == 0)
                {
                  savedCorDis = adjustedValue;
                  savedAdjCorDis = correctionValue;
                  Memory.putFloat("savedCorDis", savedCorDis);
                  Memory.putFloat("savedAdjCorDis", savedAdjCorDis);
                }
                else if (strcmp(keyToCheckCStr, "AmbMaxK") == 0)
                    {
                      savedAmbMax = adjustedValue;
                      savedAdjAmbMax = correctionValue;
                      Memory.putFloat("savedAmbMax", savedAmbMax);
                      Memory.putFloat("savedAdjAmbMax", savedAdjAmbMax);
                    }
                    else if (strcmp(keyToCheckCStr, "NutMaxK") == 0)
                        {
                          savedNutMax = adjustedValue;
                          savedAdjNutMax = correctionValue;
                          Memory.putFloat("savedNutMax", savedNutMax);
                          Memory.putFloat("savedAdjNutMax", savedAdjNutMax);
                        }
                        else if (strcmp(keyToCheckCStr, "AmbMinK") == 0)
                            {
                              savedAmbMin = adjustedValue;
                              savedAdjAmbMin = correctionValue;
                              Memory.putFloat("savedAmbMin", savedAmbMin);
                              Memory.putFloat("savedAdjAmbMin", savedAdjAmbMin);
                            }
                            else  
                              {
                                savedNutMin = adjustedValue; 
                                savedAdjNutMin = correctionValue;
                                Memory.putFloat("savedNutMin", savedNutMin);
                                Memory.putFloat("savedAdjNutMin", savedAdjNutMin);
                              }
                                                  
      Memory.end();
    }

String formatWithSign(float value, int decimals)       // Função que formata sinalizaão de positivo e negativo.
    {
        if (value == 0.0f) return String("0.00"); // Evita +0.00 ou -0.00
        return (value > 0 ? "+" : "") + String(value, decimals); // Adiciona "+" se positivo
    }

void restore()       // Função que limpa memória e restaura para configurações de fábrica.
    {
      if (rese_fabrica == true)
        {
          digitalWrite(rele_irriga, LOW);
          digitalWrite(rele_val, LOW);
          digitalWrite(rele_reser_1, LOW);
          digitalWrite(rele_reser_2, LOW);       

          digitalWrite(led_stat_func, HIGH);
          digitalWrite(led_stat_wifi, HIGH);
          digitalWrite(led_stat_erro_func, HIGH);
          digitalWrite(led_stat_rele, HIGH); 
          digitalWrite(led_ciclo_15X30, HIGH);
          digitalWrite(led_ciclo_15X45, HIGH); 

          lcd.clear();   
          
          lcd.setCursor(6,0);
          lcd.print("ATENCAO!"); 

          lcd.setCursor(2,2);
          lcd.print("DELETANDO DADOS");  

          lcd.createChar(0, bord_esq);
          lcd.setCursor(1, 3);
          lcd.write(0);              

          for (byte bords = 2; bords < 17; bords ++)
            {
              lcd.createChar(1, bord);
              lcd.setCursor(bords, 3);
              lcd.write(1);         
            } 
            bords = 0;

          lcd.createChar(2, bord_dir);
          lcd.setCursor(17, 3);
          lcd.write(2); 

          delay(1200);
          
          for (int bar_prog = 2; bar_prog < 17; bar_prog ++)
            {          
              lcd.createChar(3, barra_pro);
              lcd.setCursor(bar_prog, 3);
              lcd.write(3);

              delay(420);
            }
            bar_prog = 0;

          delay(1200);
          
          lcd.clear();

          string_dinamica(restour); 
          
          delay(1250);           

          lcd.setCursor(0,1);
          lcd.print("DADOS DELETADOS COM"); 

          lcd.setCursor(1,2);
          lcd.print("SUCESSO!");  

          lcd.clear();        

          lcd.setCursor(0,3);
          lcd.print("REINICIANDO");      

          digitalWrite(led_stat_erro_func, LOW);
          delay(1250);   

          for (byte pols = 11; pols < 16; pols ++) // Cria o efeito dos pontos (........).
            {                   
              lcd.setCursor(pols, 3);
              lcd.print(".");
              
              switch (pols)
                    {
                      case 12:
                      lcd.setCursor(pols, 3);
                      lcd.print(".");

                      digitalWrite(led_stat_rele, LOW);
                      delay(1250);

                      break;

                      case 13:
                      lcd.setCursor(pols, 3);
                      lcd.print(".");

                      digitalWrite(led_ciclo_15X30, LOW);
                      delay(1250);
                      break; 

                      case 14:
                      lcd.setCursor(pols, 3);
                      lcd.print(".");                                   

                      digitalWrite(led_ciclo_15X45, LOW);
                      delay(1250); 
                      break;                 

                      digitalWrite(led_stat_wifi, LOW);
                      delay(1250); 
                      break;
                    } 
                      
              delay(429);  // Isso equivale a  3003 mils segundos. 429 * 7.                 
            }

          lcd.clear();  

          Memory.begin("espaco_1", false); // Abre a memória para gravação.  
          Memory.clear(); // Para limpar todas as preferências no namespace aberto (não exclui o namespace). 
          Memory.end();  // Fecha nome space.
          Memory.begin("espaco_0", false);
          Memory.clear(); // Para limpar todas as preferências no namespace aberto (não exclui o namespace). 
          Memory.end();  // Fecha nome space. 

          //wifiManager.resetSettings();  //caso tiver salvo alguma rede para conectar automaticamente, ela é apagada.

          Memory.begin("espaco_1", false); // Abre a memória para gravação. 

          on_off = true;
          Memory.putBool("on_off", on_off); //   Vai força para [true]. 
          
          stat_bomba = false;
          Memory.putBool("stat_bomba", stat_bomba); //   Vai força para [false].

          aux_stat_bomba = false;
          Memory.putBool("aux_stat_bomba", aux_stat_bomba); //   Vai força para [false].

          modo_trab = true;
          Memory.putBool("modo_trab", modo_trab); //   Vai força para [true]. 

          modo_work = false;
          Memory.putBool("modo_work", modo_work); //   Vai força para [false].  

          flag_count_auto = false;
          Memory.putBool("flag_count_auto", flag_count_auto); //   Vai força para [false].    

          on_off_bip_irri = true;
          Memory.putBool("on_off_bip_irri", on_off_bip_irri); //   Vai força para [true].

          on_off_bip_nutr = true;
          Memory.putBool("on_off_bip_nutr", on_off_bip_nutr); //   Vai força para [true].

          reset_esp = false;
          Memory.putBool("reset_esp", reset_esp); //   Vai força para [false].

          aux_reset_esp = false;
          Memory.putBool("aux_reset_esp", aux_reset_esp); //   Vai força para [false].

          buzzer_reset = false;
          Memory.putBool("buzzer_reset", buzzer_reset); //   Vai força para [false].

          modo_auto = true;
          Memory.putBool("modo_auto", modo_auto); //   Vai força para [true].

          modo_manual = false;
          Memory.putBool("modo_manual", modo_manual); //   Vai força para [false].

          on_off_geral = true;
          Memory.putBool("on_off_geral", on_off_geral); //   Vai força para [true].      

          block_on_00  = false;
          Memory.putBool("block_on_00", block_on_00); //   Vai força para [false].

          block_off_00 = false;
          Memory.putBool("block_off_00", block_off_00); //   Vai força para [false].

          block_on_15  = false;
          Memory.putBool("block_on_15", block_on_15); //   Vai força para [false].

          block_off_15 = false;
          Memory.putBool("block_off_15", block_off_15); //   Vai força para [false].

          block_on_45  = false;
          Memory.putBool("block_on_45", block_on_45); //   Vai força para [false].

          block_off_45 = false;
          Memory.putBool("block_off_45", block_off_45); //   Vai força para [false].

          block_on_30  = false;
          Memory.putBool("block_on_30", block_on_30); //   Vai força para [false].

          block_off_30 = false;
          Memory.putBool("block_off_30", block_off_30); //   Vai força para [false].

          trava_15X15_on = true;
          Memory.putBool("trava_15X15_on", trava_15X15_on); //   Vai força para [true].
    
          trava_15X15_off = false;
          Memory.putBool("trava_15X15_off", trava_15X15_off); //   Vai força para [false].

          trava_15X45_on = true;
          Memory.putBool("trava_15X45_on", trava_15X45_on); //   Vai força para [true].

          trava_15X45_off = false;
          Memory.putBool("trava_15X45_off", trava_15X45_off); //   Vai força para [false]. 

          dry = false;
          Memory.putBool("dry", dry); //   Vai força para [false].

          flag_bac_pwr_1 = false;
          Memory.putBool("flag_bac_pwr_1", flag_bac_pwr_1); //   Vai força para [false].

          flag_sim_abra = false;
          Memory.putBool("flag_sim_abra", flag_sim_abra); //   Vai força para [false].

          flag_bac_pwr_2 = false;
          Memory.putBool("flag_bac_pwr_2", flag_bac_pwr_2); //   Vai força para [false].

          block_lcd_info = true;
          Memory.putBool("block_lcd_info", block_lcd_info); //   Vai força para

          block_full = true;
          Memory.putBool("block_full", block_full); //   Vai força para

          testa_18B20 = true;      
          Memory.putBool("testa_18B20", testa_18B20); //   Vai força para [true].   

          active_light = false;
          Memory.putBool("active_light", active_light); //   Vai força para [false].

          flag_blackout = false;
          Memory.putBool("flag_blackout", flag_blackout); //   Vai força para [false].

          blackout_15X45 = true;
          Memory.putBool("blackout_15X45", blackout_15X45); //   Vai força para [true].

          blackout_15X30 = false;
          Memory.putBool("blackout_15X30", blackout_15X30); //   Vai força para [false]. 
          
          lock_senso_ambi = true;
          Memory.putBool("lock_senso_ambi", lock_senso_ambi); //   Vai força para [true].

          lock_senso_nutr = true;
          Memory.putBool("lock_senso_nutr", lock_senso_nutr); //   Vai força para [true].

          lock_senso_diss = true;
          Memory.putBool("lock_senso_diss", lock_senso_diss); //   Vai força para [true].
          
          //bloc_of_sen_amb = true;
          //Memory.putBool("bloc_of_sen_amb", bloc_of_sen_amb); //   Vai força para [true]. 

          flashDevConAmb0 = true;
          Memory.putBool("flashDevConAmb0", flashDevConAmb0); //   Vai força para [true].

          flashDevConAmb1 = true;
          Memory.putBool("flashDevConAmb1", flashDevConAmb1); //   Vai força para [true].           

          flashDevConNut0 = true;    
          Memory.putBool("flashDevConNut0", flashDevConNut0); //   Vai força para [true].

          flashDevConNut1 = true;    
          Memory.putBool("flashDevConNut1", flashDevConNut1); //   Vai força para [true].

          flashDevConDis0 = true;
          Memory.putBool("flashDevConDis0", flashDevConDis0); //   Vai força para [true].

          flashDevConDis1 = true;
          Memory.putBool("flashDevConDis1", flashDevConDis0); //   Vai força para [true].

          grav_flash_diss = false;
          Memory.putBool("grav_flash_diss", grav_flash_diss); //   Vai força para [false].

          bloc_of_sen_nut = true;
          Memory.putBool("bloc_of_sen_nut", bloc_of_sen_nut); //   Vai força para [true]. 

          bloc_of_sen_dis = true;
          Memory.putBool("bloc_of_sen_dis", bloc_of_sen_dis); //   Vai força para [true]. 

          flag_pisc_red_0 = false;
          Memory.putBool("flag_pisc_red_0", flag_pisc_red_0); //   Vai força para [false].

          flag_pisc_red_1 = false;
          Memory.putBool("flag_pisc_red_1", flag_pisc_red_1); //   Vai força para [false].

          flag_pisc_red_2 = false;
          Memory.putBool("flag_pisc_red_2", flag_pisc_red_2); //   Vai força para [false]. 

          up_led_45 = true;
          Memory.putBool("up_led_45", up_led_45); //   Vai força para [true].

          up_led_30 = false;
          Memory.putBool("up_led_30", up_led_30); //   Vai força para [false].

          lock_seca_rese = true;
          Memory.putBool("lock_seca_rese", lock_seca_rese); //   Vai força para [true].   

          anomalia = false;
          Memory.putBool("anomalia", anomalia); //   Vai força para [false].

          bomb_on = false;
          Memory.putBool("bomb_on", bomb_on); //   Vai força para [false].

          counter_dry = 0;
          Memory.putInt("counter_dry", counter_dry);  // Vai forçar o valor para 0.

          v_corr_ambi = 0;
          Memory.putFloat("kCorAmb", v_corr_ambi); // Vai forçar o valor para 0.          

          v_corr_nutr = 0;
          Memory.putFloat("kCorNut", v_corr_nutr); // Vai forçar o valor para 0.

          v_corr_diss = 0;
          Memory.putFloat("kCorDis", v_corr_diss); // Vai forçar o valor para 0.

          sta_val_cor_amb = 0;
          Memory.putFloat("sta_val_cor_amb", sta_val_cor_amb); // Vai forçar o valor para 0.

          sta_val_cor_nut = 0;
          Memory.putFloat("sta_val_cor_nut", sta_val_cor_nut); // Vai forçar o valor para 0.

          temp_ref_ambi = 34.00;
          Memory.putFloat("temp_ref_ambi", temp_ref_ambi); // Vai forçar o valor para 34.00.

          temp_ref_nutri = 34.00;
          Memory.putFloat("temp_ref_nutri", temp_ref_nutri); // Vai forçar o valor para 34.00.

          t_minima_ambi = 18.00;
          Memory.putFloat("t_minima_ambi", t_minima_ambi); // Vai forçar o valor para 18.00.

          t_minima_nutr = 18.00;
          Memory.putFloat("t_minima_nutr", t_minima_nutr); // Vai forçar o valor para 18.00.

          auxAmbCorr_maxi = 0;
          Memory.putFloat("auxAmbCorr_maxi", auxAmbCorr_maxi); // Vai forçar o valor para 0.

          auxAmbCorr_mini = 0;
          Memory.putFloat("auxAmbCorr_mini", auxAmbCorr_mini); // Vai forçar o valor para 0.          

          val_aux_cor_amb;
          Memory.putString("val_aux_cor_amb", val_aux_cor_amb);  //    Limpa a String.

          val_aux_cor_nut;
          Memory.putString("val_aux_cor_nut", val_aux_cor_nut);  //    Limpa a String.

          val_aux_cor_dis;
          Memory.putString("val_aux_cor_dis", val_aux_cor_dis);  //    Limpa a String.

          /*aux_val_cor_nut;
          Memory.putString("aux_val_cor_nut", aux_val_cor_nut);  //    Limpa a String. */

          aux_val_cor_dis;
          Memory.putString("aux_val_cor_dis", aux_val_cor_dis);  //    Limpa a String. 

          /*aux_v_cor_amb;
          Memory.putString("aux_v_cor_amb", aux_v_cor_amb);     // Limpa a String.*/

          aux_v_cor_nutri;
          Memory.putString("aux_v_cor_nutri", aux_v_cor_nutri);  //    Limpa a String. 

          no_pronto = true;
          Memory.putBool("no_pronto", no_pronto); //   Vai força para [true].

          on_valvu = false;
          Memory.putBool("on_valvu", on_valvu); //   Vai força para [false].

          flag_back_pow_2 = false;
          Memory.putBool("flag_back_pow_2", flag_back_pow_2); //   Vai força para [false].

          strcpy (line_0, "SEU RESERV. NAO ESTA"); 
          Memory.putString("line_0", line_0);  //    Limpa a String.

          strcpy (line_1, "TOTALMENTE VAZIO MES-"); 
          Memory.putString("line_1", line_1);  //    Limpa a String. 

          strcpy (line_2, "MO ASSIM REPOR AGUA");
          Memory.putString("line_2", line_2);  //    Limpa a String.  

          strcpy (line_3, "OK! REPONDO...");
          Memory.putString("line_3", line_3);  //    Limpa a String.

          back_parci_lcd = false;
          Memory.putBool("back_parci_lcd", back_parci_lcd); //   Vai força para [false].

          fill_reser = false;
          Memory.putBool("fill_reser", fill_reser); //   Vai força para [false].

          block_empty = true;
          Memory.putBool("block_empty", block_empty); //   Vai força para [true].

          repor_aux_1 = false;
          Memory.putBool("repor_aux_1", repor_aux_1); //   Vai força para [false].

          repor_aux_2 = false;
          Memory.putBool("repor_aux_2", repor_aux_2); //   Vai força para [false].

          level_crit_boia = false;
          Memory.putBool("level_crit_boia", level_crit_boia); //   Vai força para [false].

          memo_lcd = true;
          Memory.putBool("memo_lcd", memo_lcd); //   Vai força para [true]. 

          flag_pos_fill = false;
          Memory.putBool("flag_pos_fill", flag_pos_fill); //   Vai força para [false].

          dry_lcd = false;
          Memory.putBool("dry_lcd", dry_lcd); //   Vai força para [false].

          flag_back_pow_1 = false;
          Memory.putBool("flag_back_pow_1", flag_back_pow_1); //   Vai força para [false].

          aux1_minut_on = 0;
          Memory.putChar("aux1_minut_on", aux1_minut_on); // Vai forçar o valor para 0.

          aux2_minut_on = 30;
          Memory.putChar("aux2_minut_on", aux2_minut_on); // Vai forçar o valor para 30.

          aux3_minut_off = 15;
          Memory.putChar("aux3_minut_off", aux3_minut_off); // Vai forçar o valor para 15.

          aux4_minut_off = 45;
          Memory.putChar("aux4_minut_off", aux4_minut_off); // Vai forçar o valor para 45.

          aux5_minut_off = 15;
          Memory.putChar("aux5_minut_off", aux5_minut_off); // Vai forçar o valor para 15.

          switc_outinver = true;
          Memory.putBool("switc_outinver", switc_outinver); //   Vai força para [true].

          atual_primavera = false;
          Memory.putBool("atual_primavera", atual_primavera); //   Vai força para [false].

          atual_outoinver = true;
          Memory.putBool("atual_outoinver", atual_outoinver); //   Vai força para [true].

          switch_estacao = true;
          Memory.putBool("switch_estacao", switch_estacao); //   Vai força para [true].

          led_verao = true;
          Memory.putBool("led_verao", led_verao); //   Vai força para [true].

          led_inver = false;
          Memory.putBool("led_inver", led_inver); //   Vai força para [false].

          cont_reset_aux;
          Memory.putString("cont_reset_aux", cont_reset_aux);    // zera variavel. 

          aspers = false;
          Memory.putBool("aspers", aspers); //   Vai força para [false].  // Pertence ao controle com aspersores.

          refrig = false;
          Memory.putBool("refrig ", refrig); //   Vai força para [true].  // Pertence ao controle com refigeração da nutrição.

          activ_aler_ambi = true;
          Memory.putBool("activ_aler_ambi", activ_aler_ambi); //   Vai força para [true].   

          on_off_all_bip = true;
          Memory.putBool("on_off_all_bip", on_off_all_bip); //   Vai força para [true].  

          savedCorAmb = 0.0; 
          Memory.putFloat("savedCorAmb", savedCorAmb); //   Vai força para 0.00.  

          savedCorNut = 0.0; 
          Memory.putFloat("savedCorNut", savedCorNut); //   Vai força para 0.00.  

          savedCorDis = 0.0; 
          Memory.putFloat("savedCorDis", savedCorDis); //   Vai força para 0.00.  

          savedAmbMax = 34.00; 
          Memory.putFloat("savedAmbMax", savedAmbMax); //   Vai força para 34.00.  

          savedNutMax = 34.00; 
          Memory.putFloat("savedNutMax", savedNutMax); //   Vai força para 34.00.

          savedAmbMin = 18.0; 
          Memory.putFloat("savedAmbMin", savedAmbMin); //   Vai força para 18.00.

          savedNutMin = 18.0; 
          Memory.putFloat("savedNutMin", savedNutMin); //   Vai força para 18.00.

          savedAdjCorAmb = 0;
          Memory.putFloat("savedAdjCorAmb", savedAdjCorAmb); //   Vai força para 0.00.

          savedAdjCorNut = 0;
          Memory.putFloat("savedAdjCorNut", savedAdjCorNut); //   Vai força para 0.00.

          savedAdjCorDis = 0;
          Memory.putFloat("savedAdjCorDis", savedAdjCorDis); //   Vai força para 0.00.

          savedAdjAmbMax = 0;
          Memory.putFloat("savedAdjAmbMax", savedAdjAmbMax); //   Vai força para 0.00.

          savedAdjNutMax = 0;
          Memory.putFloat("savedAdjNutMax", savedAdjNutMax); //   Vai força para 0.00.

          savedAdjAmbMin = 0;
          Memory.putFloat("savedAdjAmbMin", savedAdjAmbMin); //   Vai força para 0.00.

          savedAdjNutMin = 0;
          Memory.putFloat("savedAdjNutMin", savedAdjNutMin); //   Vai força para 0.00.

          alerta_ambi_ativo = true;
          Memory.putBool("ambi_ativo", alerta_ambi_ativo);          

          alerta_nutri_ativo = true;
          Memory.putBool("nutri_ativo", alerta_nutri_ativo);          

          alerta_diss_ativo = true;
          Memory.putBool("diss_ativo", alerta_diss_ativo);   

          Memory.end();  // Fecha nome space.
          
          lcd.clear();

          delay(1500);

          reset_force = true; // Força o reset após coletar dados da flash

          Memory.begin("espaco_0", false);
          Memory.putBool("reset_force", reset_force);   
          Memory.end();

          ESP.restart(); 
        }
    }                          
    
void read_flash()       // Função para ler dados alocado na flash.
    {
      Memory.begin("espaco_0", false);
      start_flags = Memory.getString("read_flags", "");   
      reset_force = Memory.getBool("reset_force", reset_force);   
      Memory.end();

      Serial.print("Imprime valor inicial da string: ");
      Serial.println(start_flags);
      Serial.println("");
            
      if (start_flags == "iniciar")  // Buscar valor do ultimo estado das flags.
        {
          Memory.begin("espaco_1", false); // Abre a memória para gravação.

          Serial.println("Imprimindo as flags salva na memoria:");
          Serial.println("");

          on_off = Memory.getBool("on_off", on_off);   
          /*Serial.print("Imprime valor do on_off: ");
          Serial.println(on_off);
          Serial.println("");*/

          stat_bomba = Memory.getBool("stat_bomba", stat_bomba);   
          /*Serial.print("Imprime valor do stat_bomba: ");
          Serial.println(stat_bomba);
          Serial.println("");*/

          aux_stat_bomba = Memory.getBool("aux_stat_bomba", aux_stat_bomba);   
          /*Serial.print("Imprime valor do aux_stat_bomba: ");
          Serial.println(aux_stat_bomba);
          Serial.println("");*/

          modo_work = Memory.getBool("modo_work", modo_work);   
          /*Serial.print("Imprime valor do modo_work: ");
          Serial.println(modo_work);
          Serial.println("");*/

          modo_trab = Memory.getBool("modo_trab", modo_trab);   
          /*Serial.print("Imprime valor do modo_trab: ");
          Serial.println(modo_trab);
          Serial.println("");*/

          flag_count_auto = Memory.getBool("flag_count_auto", flag_count_auto);   
          /*Serial.print("Imprime valor do flag_count_auto: ");
          Serial.println(flag_count_auto);
          Serial.println("");*/
               
          on_off_bip_irri = Memory.getBool("on_off_bip_irri", on_off_bip_irri);   
          /*Serial.print("Imprime valor do on_off_bip_irri: ");
          Serial.println(on_off_bip_irri);
          Serial.println("");*/
              
          on_off_bip_nutr = Memory.getBool("on_off_bip_nutr", on_off_bip_nutr);     
          /*Serial.print("Imprime valor do on_off_bip_nutr: ");             
          Serial.println(on_off_bip_nutr);
          Serial.println("");*/
             
          reset_esp = Memory.getBool("reset_esp", reset_esp);   
          /*Serial.print("Imprime valor do reset_esp: ");
          Serial.println(reset_esp);
          Serial.println("");*/

          aux_reset_esp = Memory.getBool("aux_reset_esp", aux_reset_esp);   
          /*Serial.print("Imprime valor do aux_reset_esp: ");
          Serial.println(aux_reset_esp);
          Serial.println("");*/
  
          buzzer_reset = Memory.getBool("buzzer_reset", buzzer_reset);   
          /*Serial.print("Imprime valor do buzzer_reset: ");
          Serial.println(buzzer_reset);
          Serial.println("");*/
  
          modo_auto = Memory.getBool("modo_auto", modo_auto);   
          /*Serial.print("Imprime valor do modo_auto: ");
          Serial.println(modo_auto);
          Serial.println("");*/
  
          modo_manual = Memory.getBool("modo_manual", modo_manual);   
          /*Serial.print("Imprime valor do modo_manual: ");
          Serial.println(modo_manual);
          Serial.println("");*/
  
          on_off_geral = Memory.getBool("on_off_geral", on_off_geral);   
          /*Serial.print("Imprime valor do on_off_geral: ");
          Serial.println(on_off_geral);
          Serial.println("");*/ 

          trava_15X15_on = Memory.getBool("trava_15X15_on", trava_15X15_on);   
          /*Serial.print("Imprime valor do trava_15X15_on: ");
          Serial.println(trava_15X15_on);
          Serial.println("");*/

          trava_15X15_off = Memory.getBool("trava_15X15_off", trava_15X15_off);   
          /*Serial.print("Imprime valor do trava_15X15_off: ");
          Serial.println(trava_15X15_off);
          Serial.println("");*/

          trava_15X45_on = Memory.getBool("trava_15X45_on", trava_15X45_on);   
          /*Serial.print("Imprime valor do trava_15X45_on: ");
          Serial.println(trava_15X45_on);
          Serial.println("");*/

          trava_15X45_off = Memory.getBool("trava_15X45_off", trava_15X45_off);   
          /*Serial.print("Imprime valor do trava_15X45_off: ");
          Serial.println(trava_15X45_off);
          Serial.println("");*/ 
          
          dry = Memory.getBool("dry", dry);   
          /*Serial.print("Imprime valor do dry: ");
          Serial.println(dry);
          Serial.println("");*/ 

          flag_bac_pwr_1 = Memory.getBool("flag_bac_pwr_1", flag_bac_pwr_1);   
          /*Serial.print("Imprime valor do flag_bac_pwr_1: ");
          Serial.println(flag_bac_pwr_1);
          Serial.println("");*/

          flag_sim_abra = Memory.getBool("flag_sim_abra", flag_sim_abra);   
          /*Serial.print("Imprime valor do flag_sim_abra: ");
          Serial.println(flag_sim_abra);
          Serial.println("");*/ 

          flag_bac_pwr_2 = Memory.getBool("flag_bac_pwr_2", flag_bac_pwr_2);   
          /*Serial.print("Imprime valor do flag_bac_pwr_2: ");
          Serial.println(flag_bac_pwr_2);
          Serial.println("");*/

          block_lcd_info = Memory.getBool("block_lcd_info", block_lcd_info);   
          /*Serial.print("Imprime valor do block_lcd_info: ");
          Serial.println(block_lcd_info);
          Serial.println("");*/

          block_full = Memory.getBool("block_full", block_full);   
          /*Serial.print("Imprime valor do block_full: ");
          Serial.println(block_full);
          Serial.println(""); */

          testa_18B20 = Memory.getBool("testa_18B20", testa_18B20);   
          /*Serial.print("Imprime valor do testa_18B20: ");
          Serial.println(testa_18B20);          
          Serial.println("");*/ 

          active_light = Memory.getBool("active_light", active_light);     
          /*Serial.print("Imprime valor do active_light: ");
          Serial.println(active_light );               
          Serial.println(""); */       

          set_time_cur = Memory.getBool("set_time_cur", set_time_cur);     
          /*Serial.print("Imprime valor do set_time_cur: ");
          Serial.println(set_time_cur);               
          Serial.println("");*/    

          flag_blackout = Memory.getBool("flag_blackout", flag_blackout);     
          /*Serial.print("Imprime valor do flag_blackout: ");
          Serial.println(flag_blackout);               
          Serial.println("");*/

          blackout_15X45 = Memory.getBool("blackout_15X45", blackout_15X45);     
          /*Serial.print("Imprime valor do blackout_15X45: ");
          Serial.println(blackout_15X45);               
          Serial.println("");*/

          blackout_15X30 = Memory.getBool("blackout_15X30", blackout_15X30);     
          /*Serial.print("Imprime valor do blackout_15X30: ");
          Serial.println(blackout_15X30);               
          Serial.println("");*/  

          flag_pisc_red_0 = Memory.getBool("flag_pisc_red_0", flag_pisc_red_0);   
          /*Serial.print("Imprime valor do flag_pisc_red_0: ");
          Serial.println(flag_pisc_red_0);               
          Serial.println("");*/

          flag_pisc_red_1 = Memory.getBool("flag_pisc_red_1", flag_pisc_red_1);   
          /*Serial.print("Imprime valor do flag_pisc_red_1: ");
          Serial.println(flag_pisc_red_1);               
          Serial.println("");*/

          flag_pisc_red_2 = Memory.getBool("flag_pisc_red_2", flag_pisc_red_2);   
          /*Serial.print("Imprime valor do flag_pisc_red_2: ");
          Serial.println(flag_pisc_red_2);               
          Serial.println("");*/

          lock_senso_ambi = Memory.getBool("lock_senso_ambi", lock_senso_ambi);   
          /*Serial.print("Imprime valor do lock_senso_ambi: ");
          Serial.println(lock_senso_ambi);               
          Serial.println("");*/

          lock_senso_nutr = Memory.getBool("lock_senso_nutr", lock_senso_nutr);   
          /*Serial.print("Imprime valor do lock_senso_nutr: ");
          Serial.println(lock_senso_nutr);               
          Serial.println("");*/

          lock_senso_diss = Memory.getBool("lock_senso_diss", lock_senso_diss);   
          /*Serial.print("Imprime valor do lock_senso_diss: ");
          Serial.println(lock_senso_diss);               
          Serial.println("");*/

          flashDevConAmb0 = Memory.getBool("flashDevConAmb0", flashDevConAmb0);   
          /*Serial.print("Imprime valor do flashDevConAmb0: ");
          Serial.println(flashDevConAmb0);               
          Serial.println("");*/

          flashDevConAmb1 = Memory.getBool("flashDevConAmb1", flashDevConAmb1);   
          /*Serial.print("Imprime valor do flashDevConAmb1: ");
          Serial.println(flashDevConAmb1);               
          Serial.println("");*/

          flashDevConNut0 = Memory.getBool("flashDevConNut0", flashDevConNut0);   
          /*Serial.print("Imprime valor do flashDevConNut0: ");
          Serial.println(flashDevConNut0);               
          Serial.println("");*/

          flashDevConNut1 = Memory.getBool("flashDevConNut1", flashDevConNut1);   
          /*Serial.print("Imprime valor do flashDevConNut1: ");
          Serial.println(flashDevConNut1);               
          Serial.println("");*/

          flashDevConDis0 = Memory.getBool("flashDevConDis0", flashDevConDis0);   
          /*Serial.print("Imprime valor do flashDevConDis0: ");
          Serial.println(flashDevConDis0);               
          Serial.println("");*/ 

          flashDevConDis1 = Memory.getBool("flashDevConDis1", flashDevConDis1);   
          /*Serial.print("Imprime valor do flashDevConDis1: ");
          Serial.println(flashDevConDis1);               
          Serial.println("");*/          

          bloc_of_sen_nut = Memory.getBool("bloc_of_sen_nut", bloc_of_sen_nut);   
          /*Serial.print("Imprime valor do bloc_of_sen_nut: ");
          Serial.println(bloc_of_sen_nut);               
          Serial.println("");*/ 

          bloc_of_sen_dis = Memory.getBool("bloc_of_sen_dis", bloc_of_sen_dis);   
          /*Serial.print("Imprime valor do bloc_of_sen_dis: ");
          Serial.println(bloc_of_sen_dis);               
          Serial.println("");*/

          bomb_on = Memory.getBool("bomb_on", bomb_on);
          /*Serial.print("Imprime valor do bomb_on: ");
          Serial.println(bomb_on);               
          Serial.println("");*/

          counter_dry = Memory.getInt("counter_dry", counter_dry);
          /*Serial.print("Imprime valor do counter_dry: ");
          Serial.println(counter_dry);               
          Serial.println("");*/

          v_corr_ambi = Memory.getFloat("kCorAmb", v_corr_ambi); // zera variavel.
          /*Serial.print("Imprime valor do v_corr_ambi: ");
          Serial.println(v_corr_ambi);               
          Serial.println("");*/
          
          v_corr_nutr = Memory.getFloat("kCorNut", v_corr_nutr); // zera variavel.
          /*Serial.print("Imprime valor do v_corr_nutr: ");
          Serial.println(v_corr_nutr);               
          Serial.println("");*/

          v_corr_diss = Memory.getFloat("kCorDis", v_corr_diss); // zera variavel.
          /*Serial.print("Imprime valor do v_corr_diss: ");
          Serial.println(v_corr_diss);               
          Serial.println("");*/

          sta_val_cor_amb = Memory.getFloat("sta_val_cor_amb", sta_val_cor_amb); // zera variavel.
          /*Serial.print("Imprime valor do sta_val_cor_amb: ");
          Serial.println(sta_val_cor_amb);               
          Serial.println("");*/

          sta_val_cor_nut = Memory.getFloat("sta_val_cor_nut", sta_val_cor_nut); // zera variavel.
          /*Serial.print("Imprime valor do sta_val_cor_nut: ");
          Serial.println(sta_val_cor_nut);               
          Serial.println("");*/

          temp_ref_ambi = Memory.getFloat("temp_ref_ambi", temp_ref_ambi); // zera variavel.
          /*Serial.print("Imprime valor do temp_ref_ambi: ");
          Serial.println(temp_ref_ambi);               
          Serial.println("");*/

          temp_ref_nutri = Memory.getFloat("temp_ref_nutri", temp_ref_nutri); // zera variavel.
          /*Serial.print("Imprime valor do temp_ref_nutri: ");
          Serial.println(temp_ref_nutri);               
          Serial.println("");*/

          t_minima_ambi = Memory.getFloat("t_minima_ambi", t_minima_ambi); // zera variavel.
          /*Serial.print("Imprime valor do t_minima_ambi: ");
          Serial.println(t_minima_ambi);               
          Serial.println("");*/

          auxAmbCorr_maxi = Memory.getFloat("auxAmbCorr_maxi", auxAmbCorr_maxi); // zera variavel.
          /*Serial.print("Imprime valor do auxAmbCorr_maxi: ");
          Serial.println(auxAmbCorr_maxi);               
          Serial.println("");*/

          auxAmbCorr_mini = Memory.getFloat("auxAmbCorr_mini", auxAmbCorr_mini); // zera variavel.
          /*Serial.print("Imprime valor do 0ux_v_corr_maxi: ");
          Serial.println(0ux_v_corr_maxi);               
          Serial.println("");*/          

          val_aux_cor_amb = Memory.getString("val_aux_cor_amb", val_aux_cor_amb); // zera variavel.
          /*Serial.print("Imprime valor do val_aux_cor_amb: ");
          Serial.println(val_aux_cor_amb);               
          Serial.println(""); */  

          val_aux_cor_nut = Memory.getString("val_aux_cor_nut", val_aux_cor_nut); // zera variavel.
          /*Serial.print("Imprime valor do val_aux_cor_nut: ");
          Serial.println(val_aux_cor_nut);               
          Serial.println(""); */ 

          val_aux_cor_dis = Memory.getString("val_aux_cor_dis", val_aux_cor_dis); // zera variavel.
          /*Serial.print("Imprime valor do val_aux_cor_dis: ");
          Serial.println(val_aux_cor_dis);               
          Serial.println(""); */     
          
         /* aux_val_cor_nut = Memory.getString("aux_val_cor_nut", aux_val_cor_nut); // zera variavel.
          /*Serial.print("Imprime valor do aux_val_cor_nut: ");
          Serial.println(aux_val_cor_nut);               
          Serial.println("");*/

          aux_val_cor_dis = Memory.getString("aux_val_cor_dis", aux_val_cor_dis); // zera variavel.
          /*Serial.print("Imprime valor do aux_val_cor_dis: ");
          Serial.println(aux_val_cor_dis);               
          Serial.println("");*/          

          /*aux_v_cor_amb = Memory.getString("aux_v_cor_amb", aux_v_cor_amb); // zera variavel.
          Serial.print("Imprime valor do aux_v_cor_amb: ");
          Serial.println(aux_v_cor_amb);               
          Serial.println("");*/          
         
          aux_v_cor_nutri = Memory.getString("aux_v_cor_nutri", aux_v_cor_nutri); // zera variavel.
          /*Serial.print("Imprime valor do aux_v_cor_nutri: ");
          Serial.println(aux_v_cor_nutri);               
          Serial.println("");*/ 

          lock_seca_rese = Memory.getBool("lock_seca_rese", lock_seca_rese);
          /*Serial.print("Imprime valor do lock_seca_rese: ");
          Serial.println(lock_seca_rese);               
          Serial.println("");*/
          
          anomalia = Memory.getBool("anomalia", anomalia);
          /*Serial.print("Imprime valor da anomalia: ");
          Serial.println(anomalia);               
          Serial.println("");*/

          no_pronto = Memory.getBool("no_pronto", no_pronto); 
          /*Serial.print("Imprime valor do no_pronto: ");
          Serial.println(no_pronto);               
          Serial.println("");*/ 

          on_valvu = Memory.getBool("on_valvu", on_valvu); // zera variavel.
          /*Serial.print("Imprime valor do on_valvu: ");
          Serial.println(on_valvu);               
          Serial.println("");*/

          flag_back_pow_2 = Memory.getBool("flag_back_pow_2", flag_back_pow_2);   
          /*Serial.print("Imprime valor do flag_back_pow_2: ");
          Serial.println(flag_back_pow_2);
          Serial.println("");*/

          cont_reset_aux = Memory.getString("cont_reset_aux", cont_reset_aux); 
          /*Serial.print("Imprime valor do cont_reset: ");
          Serial.println(cont_reset);               
          Serial.println("");*/ 

          String aux_line_0 = Memory.getString("line_0", line_0);
          strncpy(line_0, aux_line_0.c_str(), sizeof(line_0));
          line_0[sizeof(line_0) -1] = '\0'; 
          /*Serial.print("Imprime valor do line_0: ");
          Serial.println(line_0);               
          Serial.println("");*/

          String aux_line_1 = Memory.getString("line_1", line_1);
          strncpy(line_1, aux_line_1.c_str(), sizeof(line_1));
          line_1[sizeof(line_1) -1] = '\0';          
          /*Serial.print("Imprime valor do line_1: ");
          Serial.println(line_1);               
          Serial.println(""); */

          String aux_line_2 = Memory.getString("line_2", line_2);
          strncpy(line_2, aux_line_2.c_str(), sizeof(line_2));
          line_2[sizeof(line_2) -1] = '\0';  
          /*Serial.print("Imprime valor do line_2: ");
          Serial.println(line_2);               
          Serial.println("");*/           

          String aux_line_3 = Memory.getString("line_3", line_3);
          strncpy(line_3, aux_line_3.c_str(), sizeof(line_3));
          line_3[sizeof(line_3) -1] = '\0';  
          /*Serial.print("Imprime valor do line_3: ");
          Serial.println(line_3);               
          Serial.println("");*/

          back_parci_lcd = Memory.getBool("back_parci_lcd", back_parci_lcd); 
          /*Serial.print("Imprime valor do back_parci_lcd: ");
          Serial.println(back_parci_lcd);               
          Serial.println("");*/

          fill_reser = Memory.getBool("fill_reser", fill_reser); 
          /*Serial.print("Imprime valor do fill_reser: ");
          Serial.println(fill_reser);
          Serial.println("");*/

          block_empty = Memory.getBool("block_empty", block_empty); 
          /*Serial.print("Imprime valor do block_empty: ");
          Serial.println(block_empty);               
          Serial.println("");*/

          //repor_aux_1 = Memory.getBool("repor_aux_1", repor_aux_1); 
          /*Serial.print("Imprime valor do repor_aux_1: ");
          Serial.println(repor_aux_1);               
          Serial.println(""); */ 

          repor_aux_2 = Memory.getBool("repor_aux_2", repor_aux_2); 
          /*Serial.print("Imprime valor do repor_aux_2: ");
          Serial.println(repor_aux_2);               
          Serial.println(""); */

          level_crit_boia = Memory.getBool("level_crit_boia", level_crit_boia); 
          /*Serial.print("Imprime valor do level_crit_boia: ");
          Serial.println(level_crit_boia);               
          Serial.println(""); */

          memo_lcd = Memory.getBool("memo_lcd", memo_lcd); 
          /*Serial.print("Imprime valor do memo_lcd: ");
          Serial.println(memo_lcd);               
          Serial.println(""); */ 

          flag_pos_fill = Memory.getBool("flag_pos_fill", flag_pos_fill); 
          /*Serial.print("Imprime valor do flag_pos_fill: ");
          Serial.println(flag_pos_fill);               
          Serial.println(""); */

          dry_lcd = Memory.getBool("dry_lcd", dry_lcd);   
          /*Serial.print("Imprime valor do dry_lcd: ");
          Serial.println(dry_lcd);
          Serial.println("");*/

          flag_back_pow_1 = Memory.getBool("flag_back_pow_1", flag_back_pow_1);   
          /*Serial.print("Imprime valor do flag_back_pow_1: ");
          Serial.println(flag_back_pow_1);
          Serial.println("");*/ 

          aux1_minut_on = Memory.getChar("aux1_minut_on", aux1_minut_on); 
          /*Serial.print("Imprime valor do aux1_minut_on: ");
          Serial.println(aux1_minut_on);
          Serial.println(""); */

          aux2_minut_on = Memory.getChar("aux2_minut_on", aux2_minut_on); 
          /*Serial.print("Imprime valor do aux2_minut_on: ");
          Serial.println(aux2_minut_on);
          Serial.println(""); */

          aux3_minut_off = Memory.getChar("aux3_minut_off", aux3_minut_off); 
          /*Serial.print("Imprime valor do aux3_minut_off: ");
          Serial.println(aux3_minut_off);
          Serial.println(""); */          

          aux4_minut_off = Memory.getChar("aux4_minut_off", aux4_minut_off); 
          /*Serial.print("Imprime valor do aux4_minut_off: ");
          Serial.println(aux4_minut_off);
          Serial.println(""); */

          aux5_minut_off = Memory.getChar("aux5_minut_off", aux5_minut_off); 
          /*Serial.print("Imprime valor do aux5_minut_off: ");
          Serial.println(aux5_minut_off);
          Serial.println(""); */

          switc_outinver = Memory.getBool("switc_outinver", switc_outinver); 
          /*Serial.print("Imprime valor do switc_outinver: ");
          Serial.println(switc_outinver);
          Serial.println(""); */

          atual_primavera = Memory.getBool("atual_primavera", atual_primavera); 
          /*Serial.print("Imprime valor do atual_primavera: ");
          Serial.println(atual_primavera);
          Serial.println(""); */   

          atual_outoinver = Memory.getBool("atual_outoinver", atual_outoinver); 
          /*Serial.print("Imprime valor do atual_outoinver: ");
          Serial.println(atual_outoinver);
          Serial.println(""); */       

          flag_10X20 = Memory.getBool("flag_10X20", flag_10X20); 
          /*Serial.print("Imprime valor do flag_10X20: ");
          Serial.println(flag_10X20);
          Serial.println(""); */

          flag_15X15 = Memory.getBool("flag_15X15", flag_15X15); 
          /*Serial.print("Imprime valor do flag_15X15: ");
          Serial.println(flag_15X15);
          Serial.println(""); */

          switch_estacao = Memory.getBool("switch_estacao", switch_estacao); 
          /*Serial.print("Imprime valor do switch_estacao: ");
          Serial.println(switch_estacao);
          Serial.println(""); */

          led_verao = Memory.getBool("led_verao", led_verao); 
          /*Serial.print("Imprime valor do led_verao: ");
          Serial.println(led_verao);
          Serial.println("");*/

          led_inver = Memory.getBool("led_inver", led_inver); 
          /*Serial.print("Imprime valor do led_inver: ");
          Serial.println(led_inver);
          Serial.println("");*/

          aspers = Memory.getBool("aspers", aspers); // Pertence ao controle com aspersores.
          /*Serial.print("Imprime valor do aspers: ");
          Serial.println(aspers);
          Serial.println("");*/

          refrig = Memory.getBool("refrig", refrig); // Pertence ao controle com refrigeração.
          /*Serial.print("Imprime valor do refrig: ");
          Serial.println(refrig);
          Serial.println("");*/

          activ_aler_ambi = Memory.getBool("activ_aler_ambi", activ_aler_ambi); 
          /*Serial.print("Imprime valor do activ_aler_ambi: ");
          Serial.println(activ_aler_ambi);
          Serial.println("");*/

          on_off_all_bip = Memory.getBool("on_off_all_bip", on_off_all_bip); 
          /*Serial.print("Imprime valor do on_off_all_bip: ");
          Serial.println(on_off_all_bip);
          Serial.println("");*/

          savedCorAmb = Memory.getFloat("savedCorAmb", savedCorAmb);
          /*Serial.print("Imprime valor do savedCorAmb: ");
          Serial.println(savedCorAmb);
          Serial.println("");*/

          savedCorNut = Memory.getFloat("savedCorNut", savedCorNut);
          /*Serial.print("Imprime valor do savedCorNut: ");
          Serial.println(savedCorNut);
          Serial.println("");*/

          savedCorDis = Memory.getFloat("savedCorDis", savedCorDis);
          /*Serial.print("Imprime valor do savedCorDis: ");
          Serial.println(savedCorDis);
          Serial.println("");*/

          savedAmbMax = Memory.getFloat("savedAmbMax", savedAmbMax);
          /*Serial.print("Imprime valor do savedAmbMax: ");
          Serial.println(savedAmbMax);
          Serial.println("");*/

          savedNutMax = Memory.getFloat("savedNutMax", savedNutMax);
          /*Serial.print("Imprime valor do savedNutMax: ");
          Serial.println(savedNutMax);
          Serial.println("");*/

          savedAmbMin = Memory.getFloat("savedAmbMin", savedAmbMin);
          /*Serial.print("Imprime valor do savedAmbMin: ");
          Serial.println(savedAmbMin);
          Serial.println("");*/

          savedNutMin = Memory.getFloat("savedNutMin", savedNutMin);
          /*Serial.print("Imprime valor do savedNutMin: ");
          Serial.println(savedNutMin);
          Serial.println("");*/ 

          savedAdjCorAmb = Memory.getFloat("savedAdjCorAmb", savedAdjCorAmb);
          /*Serial.print("Imprime valor do savedAdjCorAmb: ");
          Serial.println(savedAdjCorAmb);
          Serial.println("");*/ 

          savedAdjCorNut = Memory.getFloat("savedAdjCorNut", savedAdjCorNut);
          /*Serial.print("Imprime valor do savedAdjCorNut: ");
          Serial.println(savedAdjCorNut);
          Serial.println("");*/

          savedAdjCorDis = Memory.getFloat("savedAdjCorDis", savedAdjCorDis);
          /*Serial.print("Imprime valor do savedAdjCorDis: ");
          Serial.println(savedAdjCorDis);
          Serial.println("");*/

          savedAdjAmbMax = Memory.getFloat("savedAdjAmbMax", savedAdjAmbMax);
          /*Serial.print("Imprime valor do savedAdjAmbMax: ");
          Serial.println(savedAdjAmbMax);
          Serial.println("");*/

          savedAdjNutMax = Memory.getFloat("savedAdjNutMax", savedAdjNutMax);
          /*Serial.print("Imprime valor do savedAdjNutMax: ");
          Serial.println(savedAdjNutMax);
          Serial.println("");*/

          savedAdjAmbMin = Memory.getFloat("savedAdjAmbMin", savedAdjAmbMin);
          /*Serial.print("Imprime valor do savedAdjAmbMin: ");
          Serial.println(savedAdjAmbMin);
          Serial.println("");*/

          savedAdjNutMin = Memory.getFloat("savedAdjNutMin", savedAdjNutMin);
          /*Serial.print("Imprime valor do savedAdjNutMin: ");
          Serial.println(savedAdjNutMin);
          Serial.println("");*/

          alerta_ambi_ativo = Memory.getBool("ambi_ativo", alerta_ambi_ativo); // true = valor padrão se não existir
          /*Serial.print("Imprime valor do alerta_ambi_ativo: ");
          Serial.println(alerta_ambi_ativo);
          Serial.println("");*/

          alerta_nutri_ativo = Memory.getBool("nutri_ativo", alerta_nutri_ativo);
          /*Serial.print("Imprime valor do alerta_nutri_ativo: ");
          Serial.println(alerta_nutri_ativo);
          Serial.println("");*/

          alerta_diss_ativo = Memory.getBool("diss_ativo", alerta_diss_ativo);
          /*Serial.print("Imprime valor do alerta_diss_ativo: ");
          Serial.println(alerta_diss_ativo);
          Serial.println("");*/          

          // Recupera os valores para temperatura ambiente
          v_corr_ambi = Memory.getFloat("kCorAmb", 0.0); // Valor padrão 0.0
          aux_v_corr_ambi = sensor.getTempC(temp_ambi) + v_corr_ambi;

          // Recupera os valores para temperatura de nutrição
          v_corr_nutr = Memory.getFloat("kCorNut", 0.0); // Valor padrão 0.0
          aux_v_corr_nutr = sensor.getTempC(temp_nutri) + v_corr_nutr;

          // Recupera os valores para temperatura do dissipador
          v_corr_diss = Memory.getFloat("kCorDis", 0.0); // Valor padrão 0.0
          aux_v_corr_diss = sensor.getTempC(temp_diss) + v_corr_diss;

          // Recupera os valores para temperatura máxima do ambiente
          v_corr_amb_maxi = Memory.getFloat("AmbMax", 0.0); // Valor padrão 0.0
          auxAmbCorr_maxi = temp_ref_ambi + v_corr_amb_maxi;

          // Recupera os valores para temperatura máxima da nutrição
          v_corr_nut_maxi = Memory.getFloat("NutMax", 0.0); // Valor padrão 0.0
          auxNutCorr_maxi = temp_ref_nutri + v_corr_nut_maxi;

          //Recupera os valores para temperatura mínima do ambiente
          v_corr_amb_mini = Memory.getFloat("AmbMin", 0.0); // Valor padrão 0.0
          auxAmbCorr_mini = t_minima_ambi + v_corr_amb_mini;

          // Recupera os valores para temperatura mínima da nutrição
          t_minima_nutr = Memory.getFloat("NutMin", t_minima_nutr); // Valor padrão t_minima_nutr        

          Memory.end(); //  ######################  Fim da leitura no espaco_1.  ########################################## 
        }

          start_flags = "iniciar";

          Memory.begin("espaco_0", false);
          Memory.putString("read_flags", start_flags);
          Memory.end();

          if (reset_force == true)
            {
              reset_force = false;

              Memory.begin("espaco_0", false);
              Memory.putBool("reset_force", reset_force);
              Memory.end();
              
              digitalWrite(led_stat_func, LOW);
              digitalWrite(led_stat_wifi, LOW);
              digitalWrite(led_stat_erro_func, LOW);
              digitalWrite(led_stat_rele, LOW); 
              digitalWrite(led_ciclo_15X30, LOW);
              digitalWrite(led_ciclo_15X45, LOW); 

              lcd.clear();

              lcd.setCursor(0,0);
              lcd.print("O SISTEMA PRECISA DE "); 

              lcd.setCursor(0,1);
              lcd.print("ALGUNS PARAMETROS P/");

              lcd.setCursor(0,2);
              lcd.print("TRABALHAR!");

              lcd.setCursor(0,3);
              lcd.print("INICIANDO COLETA...");

              delay(5000);

              lcd.clear();
              
              lcd.setCursor(6,0);
              lcd.print("ATENCAO!"); 

              lcd.setCursor(2,2);
              lcd.print("COLETANDO DADOS");                           

              lcd.createChar(0, bord_esq);
              lcd.setCursor(1, 3);
              lcd.write(0);                           

              for (byte bords = 2; bords < 17; bords ++)
                {
                  lcd.createChar(1, bord);
                  lcd.setCursor(bords, 3);
                  lcd.write(1);         
                } 
                bords = 0;

              lcd.createChar(2, bord_dir);
              lcd.setCursor(17, 3);
              lcd.write(2); 

              delay(1200);
              
              for (int bar_prog = 2; bar_prog < 17; bar_prog ++)
                {          
                  lcd.createChar(3, barra_pro);
                  lcd.setCursor(bar_prog, 3);
                  lcd.write(3);

                  delay(420);
                }
                bar_prog = 0;

              lcd.init();          // Inicia o display LCD.

              delay(2500);
              
              //lcd.clear();                      

              lcd.setCursor(0,0);
              lcd.print("DADOS COLETADOS COM"); 

              lcd.setCursor(0,1);
              lcd.print("SUCESSO!");                       

              lcd.setCursor(0,3);
              lcd.print("REINICIANDO");

              delay(1500);  

              //lcd.clear(); 

              for (byte pols = 11; pols < 16; pols ++) // Cria o efeito dos pontos (........).
                {                   
                  lcd.setCursor(pols, 3);
                  lcd.print(".");
                  
                  switch (pols)
                        {
                          case 12:
                          lcd.setCursor(pols, 3);
                          lcd.print(".");

                          digitalWrite(led_ciclo_15X45, HIGH);
                          delay(1250);

                          break;

                          case 13:
                          lcd.setCursor(pols, 3);
                          lcd.print(".");

                          digitalWrite(led_ciclo_15X30, HIGH);
                          delay(1250);
                          break; 

                          case 14:
                          lcd.setCursor(pols, 3);
                          lcd.print(".");                                   

                          digitalWrite(led_stat_rele, HIGH);
                          delay(1250); 
                          break;

                          case 15:
                          lcd.setCursor(pols, 3);
                          lcd.print(".");                 

                          digitalWrite(led_stat_erro_func, HIGH);
                          delay(1250); 
                          break;
                        } 
                          
                          delay(400);               
                }

              ESP.restart();   
            }                 
    }

void read_flash_sensor()       // Função que reatribui valores as variáveis.
    {
      Sensores.begin("sensor_5", false);  //  Inicia o endereço na flash onde estão os endereços dos sensores.

          Sensores.getBytes("SaveAdressAmbi", &temp_ambi, sizeof(temp_ambi));

          Serial.println(temp_ambi[0]);
          Serial.println(temp_ambi[1]);
          Serial.println(temp_ambi[2]);
          Serial.println(temp_ambi[3]);
          Serial.println(temp_ambi[4]);
          Serial.println(temp_ambi[5]);
          Serial.println(temp_ambi[6]);
          Serial.println(temp_ambi[7]);

          Serial.println(" ");
          Serial.println("-----------------------------------------------------------------");
          Serial.println(" ");

          Sensores.getBytes("SaveAdressNutri", &temp_nutri, sizeof(temp_nutri));

          Serial.println(temp_nutri[0]);
          Serial.println(temp_nutri[1]);
          Serial.println(temp_nutri[2]);
          Serial.println(temp_nutri[3]);
          Serial.println(temp_nutri[4]);
          Serial.println(temp_nutri[5]);
          Serial.println(temp_nutri[6]);
          Serial.println(temp_nutri[7]);  
          Serial.println(""); 

          Serial.println(" ");
          Serial.println("-----------------------------------------------------------------");
          Serial.println(" ");

          Sensores.getBytes("SaveAdressDiss", &temp_diss, sizeof(temp_diss));

          Serial.println(temp_diss[0]);
          Serial.println(temp_diss[1]);
          Serial.println(temp_diss[2]);
          Serial.println(temp_diss[3]);
          Serial.println(temp_diss[4]);
          Serial.println(temp_diss[5]);
          Serial.println(temp_diss[6]);
          Serial.println(temp_diss[7]);  
          Serial.println(""); 

          flag_conf_sens = Sensores.getBool("flag_conf_sens", "");     
          /*Serial.print("Imprime valor do flag_conf_sens: ");
          Serial.println(flag_conf_sens);               
          Serial.println(""); */         
          
          Sensores.end();
    }

void pressButton()       // Função que aciona menu rapido.
    {
      exitPressButton = true;
      unsigned long currentMillis = millis();      
  
      if ((digitalRead(bt_set) == LOW) && (exitPressButton)) 
        {  
          if (currentMillis - previousMillis >= interval) 
            {
              block_empty = false;

              previousMillis = currentMillis;

              currentItem ++;

              if (currentItem >= maxItems) 
                {
                  currentItem = 0;

                  waitNonBlocking(1200);
                }
                
                switch (currentItem) // Atualiza display com item atual
                    {
                      case 1: // Ativa backlight
                          
                          lcd.clear();

                          Memory.begin("espaco_1", false);
                          active_light = Memory.getBool("active_light", active_light);
                          Memory.end();      
                
                          if (!active_light) 
                            {
                              lcd.setCursor(0, 0);
                              lcd.print("BACKLIGH: FIXO->TEMP");                                        
                            } 
                            else 
                              {        
                                lcd.setCursor(0, 0);
                                lcd.print("BACKLIGH: TEMP->FIXO");                                
                              }

                          Memory.begin("espaco_1", false);
                          Memory.putBool("active_light", active_light);
                          Memory.end();              
                        
                          lcd.setCursor(1, 2);
                          lcd.print("SOLTE O BOTAO PARA");

                          lcd.setCursor(6, 3);
                          lcd.print("EXECUTAR");

                          waitNonBlocking(1250);               

                        break;
                      case 2: // Delata as credencias de rede

                        lcd.clear();

                        lcd.setCursor(0, 0);
                        lcd.print("DELETAR CREDENCIAIS");

                        lcd.setCursor(1, 2);
                        lcd.print("SOLTE O BOTAO PARA");

                        lcd.setCursor(6, 3);
                        lcd.print("EXECUTAR");

                        waitNonBlocking(1250);                        

                        break;
                      case 3:

                        lcd.clear();

                        lcd.setCursor(2, 0);
                        lcd.print("DELETAR TELEGRAM");

                        lcd.setCursor(1, 2);
                        lcd.print("SOLTE O BOTAO PARA");

                        lcd.setCursor(6, 3);
                        lcd.print("EXECUTAR");

                        waitNonBlocking(1250);

                        break;
                      case 4: // Reseta dispositivo

                        lcd.clear();

                        lcd.setCursor(0, 0);
                        lcd.print("RESTAUR. DE FABRICA");

                        lcd.setCursor(1, 2);
                        lcd.print("SOLTE O BOTAO PARA");

                        lcd.setCursor(6, 3);
                        lcd.print("EXECUTAR");

                        waitNonBlocking(1250);                                      

                        break;
                      case 5: // Reseta dispositivo

                        lcd.clear();

                        lcd.setCursor(2, 0);
                        lcd.print("RESET DO SISTEMA");

                        lcd.setCursor(1, 2);
                        lcd.print("SOLTE O BOTAO PARA");

                        lcd.setCursor(6, 3);
                        lcd.print("EXECUTAR");

                        waitNonBlocking(1250);                                     

                        break; 

                      case 6: // Reseta dispositivo

                        lcd.clear();

                        lcd.setCursor(4, 0);
                        lcd.print("SAIR DO MENU");

                        lcd.setCursor(1, 2);
                        lcd.print("SOLTE O BOTAO PARA");

                        lcd.setCursor(8, 3);
                        lcd.print("SAIR");

                        waitNonBlocking(1250);
                        
                        lcd.clear();              

                        break; 
                    }
            }
        }

        if (digitalRead(bt_set) == HIGH) 
          { 
            executeAction(currentItem);

            currentItem = 0;  // Reset para o primeiro item        
          }
    }

void executeAction(int item)       //  Função que executa item selecionado no menu rapido
    {
      switch (item) 
            {
              case 1: // Alterna o backlight on/off

                Memory.begin("espaco_1", false);
                active_light = Memory.getBool("active_light", active_light);
                Memory.end();

                active_light = !active_light;

                if (active_light) 
                  {
                    lcd.noBacklight();  // Desliga o backlight físico
                    if (bot != nullptr) // Proteção contra panico.
                      { 
                        enviarMensagem("💡 Backlight: Modo Temporário.\n\nObs.: Alterado pelo painel.");
                        //bot->sendMessage(idBuffer, F("💡 Backlight: Modo Temporário.\n\nObs.: Alterado pelo painel."), "");
                      }

                  } else 
                      {
                        lcd.backlight();  // Liga o backlight físico

                        if (bot != nullptr) // Proteção contra panico.
                          { 
                            enviarMensagem("💡 Backlight: Modo Fixo.\n\nObs.: Alterado pelo painel.");
                            //bot->sendMessage(idBuffer, F("💡 Backlight: Modo Fixo.\n\nObs.: Alterado pelo painel."), "");
                          }

                        counter_blackluz = 0; // Zera o contador do backlight.
                      }

                Memory.begin("espaco_1", false);
                Memory.putBool("active_light", active_light);
                Memory.end();

                lcd.clear();

                block_empty = true;

                currentItem = 0;

                waitNonBlocking(1250);

                break;

              case 2: // Deleta credenciais de rede.

                 if (bot != nullptr) // Proteção contra panico.
                  {
                    bot->sendMessage(idBuffer, String(cred_del_0), ""); 
                  }                  

                currentItem = 0; 

                waitNonBlocking(1250);                 

                forceBoot_0 = true;

                break;

              case 3: // Deleta dados do Telegram
                
                  enviarMensagem("Apagando Token e ID do Telegram. Aguarde...");
                   logToWeb(" - Apagando Token e ID do Telegram.");
                    
                  Telegram.begin("token_id", false); // Abre namespace
                  Telegram.clear();
                  Telegram.end(); // Fecha namespace

                  // Mensagem final antes de reiniciar
                  enviarMensagem(cl_data_2); // Mensagem customizada final (confirmar conteúdo)
                  logToWeb(" - " + String(cl_data_2));
                    
                  // Reinicia ESP32
                  ESP.restart();
                    
                  break;
                
              case 4: // Limpa memoria Flash.

                Serial.println(" "); 
                Serial.println("ENTREI NO CASE 4 ONDE RESTAURAÇÂO DE FABRICA.");
                Serial.println(" ");            

                rese_fabrica = true; 
                forceBoot_2 = true;

                currentItem = 0;

                waitNonBlocking(1250);               

                break;

              case 5: // Reinicia o dispositivo.

                if (bot != nullptr) // Proteção contra panico.
                  {                 
                    enviarMensagem("Resetado pelo painel.");
                    //bot->sendMessage(idBuffer, "Resetado pelo painel.", "");
                  } 

                waitNonBlocking(2000);

                currentItem = 0;

                ESP.restart();
                break;

              case 6: // Sai do menu.

                exitPressButton = false;

                lcd.clear();

                block_empty = true;

                currentItem = 0;

                break;    
            }
    }

void waitNonBlocking(unsigned long durationMs)       // Função que trava, mas deixa o esp32 trabalhar.
    {
      unsigned long startTime = millis();

     while (millis() - startTime < durationMs) 
          {           
            yield();            
          }  
    }

void mostrarMensagemTentandoConectar(int tentativa)       //Função mostra tentativas de conexão.
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CONEXAO C/ DISPOSI-");
      lcd.setCursor(0, 1);
      lcd.print("TIVO SEM FIO.");
      lcd.setCursor(4, 3);
      lcd.print("TENTATIVA: ");
      lcd.setCursor(15, 3);
      lcd.print(tentativa);
    }

void mostrarMensagemFalhaTemporaria()       // Função que mostra falha temporaria e reinicia.
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("VERIFICANDO CONEXAO");
      lcd.setCursor(0, 1);
      lcd.print("AGUARDE");

      for (int i = 7; i < 12; i++) 
        {
          lcd.setCursor(i, 1);
          lcd.print(".");
          delay(850);
        }
      
      lcd.setCursor(0, 2);
      lcd.print("FALHA NA CONEXAO!!!");
      delay(1500);

      if (aux_tentativa < 3)
        {
          lcd.setCursor(0, 3);
          lcd.print("INIC. NOVA TENTATIVA");
        }
        else 
          {
            lcd.setCursor(0, 3);
            lcd.print("IGNORANDO CONEXAO!");
          }
      

      delay(2500);

     /* for (int i = 11; i < 16; i++) 
        {
          lcd.setCursor(i, 3);
          lcd.print(".");
          delay(850);
        }*/
    }

void mostrarMensagemFalhaFinal()       // Função que mostra falha final da conexão.
    {
      Serial.println("Falha ao conectar...");

      if (on_off_all_bip) a_buzzer(6, 200, 200);

      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("ATENCAO!");
      lcd.setCursor(0, 1);
      lcd.print("NAO FOI POSSIVEL");
      lcd.setCursor(0, 2);
      lcd.print("ESTABELECER CONEXAO");
      lcd.setCursor(0, 3);
      lcd.print("COM SEU DISPOSITIVO!");

      delay(5000);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("O SISTEMA CONTINUARA");
      lcd.setCursor(0, 1);
      lcd.print("A MONITORAR O SEU");
      lcd.setCursor(0, 2);
      lcd.print("CULTIVO SEM CONEXAO");
      lcd.setCursor(0, 3);
      lcd.print("CARREG. SISTEMA");
      
      for (int i = 15; i < 20; i++) 
        {
          lcd.setCursor(i, 3);
          lcd.print(".");
          delay(429); 
        }      
    }

void mostrarMensagemConectado()       // Função que mostra conexão com sucesso.
    {
      lcd.clear();

      lcd.setCursor(2, 1);
      lcd.print("CONEXAO COM WI-FI");
      lcd.setCursor(4, 2);
      lcd.print("BEM SUCEDIDA");
      //lcd.setCursor(1, 3);
      //lcd.print("BOM CULTIVO !!!");

      digitalWrite(led_stat_wifi, HIGH); 

      delay(2500); 
    }

void logToWeb(const String &mensagem) 
    {
      if (!rtcOk) 
        {
          ws.textAll(mensagem);
          return;
        }

        DateTime now = rtc.now();
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "[%02d:%02d:%02d]", now.hour(), now.minute(), now.second());

        ws.textAll(String(buffer) + mensagem);
    }

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) 
    {
      if (type == WS_EVT_CONNECT) 
        {
          Serial.println("WebSocket conectado");
         } 
         else if (type == WS_EVT_DISCONNECT) 
             {
               Serial.println("WebSocket desconectado");
             } 
             else if (type == WS_EVT_DATA) 
                 {
                   String msg = "";

                   for (size_t i = 0; i < len; i++) msg += (char)data[i];

                   Serial.println("Mensagem WebSocket: " + msg);
                 }
    }

String getLogAsText()       // Função que monta String para ser enviada por void logToWeb(const String& msg).
    {
      String output = "";
      DateTime now = rtc.now();  // <--- Atualiza o horário agora
      String horarioAtual = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

      for (int i = 0; i < LOG_SIZE; i++) 
        {
          int index = (logIndex + i) % LOG_SIZE;
          if (logBuffer[index].length() > 0) 
            {
              output += horarioAtual + " - " + logBuffer[index] + "\n";
            }
        }
        return output;
    }

void configConexWifi()       // Função responsável por configurar conexão ao dispositivo fisico.
    {
      if (WiFi.status() == WL_CONNECTED) 
        {
          // Já conectado, apenas mostra mensagem e continua
          mostrarMensagemConectado();
        } 
        else 
          {          
            bool conectado = false;      

            for (char tentativa = 1; tentativa <= 3; tentativa++) 
               {
                 aux_tentativa = tentativa;
                 
                 mostrarMensagemTentandoConectar(tentativa);

                 wifiManager.setConnectTimeout(10);
                 wifiManager.setConfigPortalTimeout(30);

                 conectado = wifiManager.autoConnect("SCHidro_AP");

                 if (conectado) 
                   {
                     mostrarMensagemConectado();
                     return; // sucesso
                   } 
                   else 
                     {
                       mostrarMensagemFalhaTemporaria();
                       wifiManager.resetSettings(); // limpa dados antigos antes da nova tentativa
                       delay(3000);
                     }
                }

                // Após 3 tentativas sem sucesso
                mostrarMensagemFalhaFinal();
                delay(1000);                
                return;  // nunca chega aqui, mas por clareza
          }
    }

void estabilishConnec()       //  Função que aguarda a conexão estabilizar.
    {
      lcd.clear();
      lcd.setCursor(1, 1);
      lcd.print("INICIANDO SISTEMA!");

      lcd.setCursor(3, 3);
      lcd.print("AGUARDE");
      
      for (int i = 10; i < 15; i++) 
         {
           lcd.setCursor(i, 3);
           lcd.print(".");
           delay(1000);
         }
      
      lcd.clear();
      lcd.setCursor(6, 0); lcd.print("BEM VINDO");
      lcd.setCursor(0, 1); lcd.print("SISTEMA DE CONTROLE");
      lcd.setCursor(5, 2); lcd.print("HIDROPONICO");
      lcd.setCursor(10, 3); lcd.print(CURRENT_VERSION);


      delay(2500);
    }

void timesStart()       // Função que mostra quantas vezes o dispositivo foi iniciado.
    {
      Serial.println(" ");
      Serial.println(" "); 

      Flash.begin("start", true);
      counter_start = Flash.getInt("counter_start", 0);
      Flash.end();

      counter_start ++;

      Flash.begin("start", false);
      Flash.putInt("counter_start", counter_start);      
      Flash.end();

      Flash.begin("start", true);
      aux_start = Flash.getInt("counter_start", 0);
      Flash.end();

      Serial.println(" ");
      Serial.println(" "); 

      Serial.print(F("O Dispositivo foi reiniciado: "));      
      Serial.print(aux_start);
      Serial.println(" vezes."); 

      Serial.println(" ");
      Serial.println(" ");
    }

void enviarMensagem(const char* mensagem) 
    {
      if (bot != nullptr && strlen(idBuffer) > 0) // Proteção contra ponteiro vazio.
        {          
          bot->sendMessage(idBuffer, mensagem, "");
        }
    }

void printAddress(const char* nome, const uint8_t* address, int sensorIndex) 
    {
      Serial.print("Sensor ");
      Serial.print(nome);
      Serial.print(" endereço: ");

      // Imprime endereço em hexadecimal com 2 dígitos cada byte
      for (int i = 0; i < 8; i++) 
        {
          if (address[i] < 16) Serial.print("0");
          Serial.print(address[i], HEX);
        }

      Serial.println();

      // Configura resolução para o sensor específico (9 a 12 bits)
      sensor.setResolution(sensorEnderecos[sensorIndex], 10); // Aqui define a resolução desejada
      delay(350); // Pequena pausa para garantir

      int resol = sensor.getResolution(sensorEnderecos[sensorIndex]);

      Serial.print("Resolução do sensor ");
      Serial.print(nome);
      Serial.print(" configurada para ");
      Serial.print(resol);
      Serial.println(" bits.");
    } 

void logInitStatus() 
    {
      // WebSocket (não retorna, mas pode indicar se está ativo)
      //webSocket.begin();     apagar
      logToWeb(" - WebSocket iniciado.\n");
      
      // RTC
      bool rtcStatus = rtc.begin();

      if (rtcStatus) logToWeb(" - RTC DS3231 iniciado com sucesso.\n");
      else logToWeb(" - Falha ao iniciar RTC!\n");

      // Sensor DS18B20
      // Se sua lib retorna bool no begin(), use assim, senão ajuste

      sensor.begin();
      bool sensorStatus = true;

      if (sensorStatus) logToWeb(" - Sensor DS18B20 iniciado com sucesso.\n");
      else logToWeb(" - Falha ao iniciar sensor DS18B20!\n");  

      // Wi-Fi (verificação de conexão)
      if (WiFi.status() == WL_CONNECTED) 
        {
          String wifiInfo = " - Dispositivo conectado no Wi-Fi:\n\n";
          wifiInfo += "             IP: " + WiFi.localIP().toString() + "\n";
          wifiInfo += "             Máscara: " + WiFi.subnetMask().toString() + "\n";
          wifiInfo += "             Gateway: " + WiFi.gatewayIP().toString() + "\n";
          wifiInfo += "             SSID: " + WiFi.SSID() + "\n";

          logToWeb(wifiInfo);
        }
        else logToWeb(" - Dispositivo não está conectado ao Wi-Fi.");

      // NTP (não retorna, mas podemos tentar update e verificar horário)
      ntp.begin();

      delay(100);

      unsigned long epoch = ntp.getEpochTime();

      if (epoch > 0) logToWeb(" - NTP iniciado e horário obtido.\n");
      else logToWeb(" - Falha ao iniciar NTP ou obter horário.\n");

      // Se quiser pode continuar para outras libs/hardware que usar...
    }

void pushReset()
    {
      lcd.init();
      delay(150);

      lcd.backlight();
      delay(150);
      
      if (digitalRead(bt_set) == LOW)
        {
           lcd.clear();

            while (digitalRead(bt_set) == LOW)
                 {
                   lcd.setCursor(3, 1); lcd.print("SOLTE O BOTAO!");
                 }

               lcd.clear();
               
               lcd.setCursor(0, 0); lcd.print("LIMPANDO CREDENCIAIS");
               lcd.setCursor(0, 2); lcd.print("AGUARDE");

               for (char push = 7; push < 12; push++)
                  {
                    lcd.setCursor(push, 2);
                    lcd.print(".");

                    delay(1250);
                  }

               lcd.clear();
               lcd.setCursor(4, 2); lcd.print("REINICIANDO!"); 

               delay(1850);

               wifiManager.resetSettings();

               ESP.restart();             
        }
    }

void checkFirmware() 
    {
      Serial.println("[OTA] Verificando se há nova versão...");
      logToWeb(" - [OTA] Verificando se há nova versão...");
      enviarMensagem("Verificando se há nova versão...");

      delay(1500);      

      HTTPClient http;
      http.begin(versionURL);
      int httpCode = http.GET();

      if (httpCode != 200) 
        {
          Serial.println("[OTA] Erro ao obter versão.");
          logToWeb(" - [OTA] Erro ao obter versão.");
          enviarMensagem("Erro ao obter versão.");
          http.end();
          return;
        }

        remoteVersion = http.getString();
        remoteVersion.trim();
        http.end();

        Serial.println("[OTA] Versão remota: " + remoteVersion);
        logToWeb(" - [OTA] Versão remota: " + remoteVersion);
        Serial.println("[OTA] Versão atual: " + String(CURRENT_VERSION));  
        logToWeb(" - [OTA] Versão atual: " + String(CURRENT_VERSION));

      if (!remoteVersion.equals(CURRENT_VERSION)) 
        {
          String msg = "Nova versão disponível: " + remoteVersion + ".\n\nDeseja atualizar? ->       /sim_atualize";
          enviarMensagem(msg.c_str());
        } 
        else 
          {
            enviarMensagem("Firmware já está atualizado!");
          }
    }

void upDateFirmware()
    {
     // block_cmd_0();
      
      if (remoteVersion == "" || remoteVersion.equals(CURRENT_VERSION)) 
        {
          const char* msgTelegram0 = "Nenhuma nova versão para atualizar.";
          char remVer0[64];
          snprintf(remVer0, sizeof(remVer0), " - [OTA] %s", msgTelegram0);
          
          Serial.println(remVer0);
          logToWeb(remVer0);
          enviarMensagem(msgTelegram0);
          return;
        }

        char remVer1[64];
        const char* msgTelegram1 = "Iniciando atualização...";

        // Versão segura usando snprintf (evita buffer overflow)
        snprintf(remVer1, sizeof(remVer1), " - [OTA] %s", msgTelegram1);

        Serial.println(remVer1);      // Log formatado no Serial
        logToWeb(remVer1);            // Log formatado na web
        enviarMensagem(msgTelegram1); // Mensagem limpa para o Telegram

        HTTPClient http;
        http.begin(firmwareURL);
        int code = http.GET();

      if (code != 200) 
        {
          char remVer2[64];
          const char* msgTelegram2 = "Erro ao baixar firmware.";

          // Versão segura mantendo os nomes originais:
          snprintf(remVer2, sizeof(remVer2), " - [OTA] %s", msgTelegram2);  // Formata seguro

          Serial.println(remVer2);       // Log formatado (mesmo nome)
          logToWeb(remVer2);             // Log na web (mesmo nome)
          enviarMensagem(msgTelegram2);  // Mensagem original (mesmo nome)    

          http.end();
          return;
        }

        int len = http.getSize();
        WiFiClient* stream = http.getStreamPtr();

      if (!Update.begin(len)) 
        {
          char remVer3[64];
          const char* msgTelegram3 = "Erro ao iniciar Update.";

          // Versão segura usando snprintf (mantendo os nomes originais)
          snprintf(remVer3, sizeof(remVer3), " - [OTA] %s", msgTelegram3);

          Serial.println(remVer3);       // Log formatado no Serial
          logToWeb(remVer3);             // Log formatado na Web
          enviarMensagem(msgTelegram3);  // Mensagem original para Telegram 
           
          http.end();
          return;
        }

        size_t written = Update.writeStream(*stream);
      if (written != len) 
        {
          char remVer4[64];
          const char* msgTelegram4 = "Escrita incompleta.";

          // Versão segura mantendo os nomes originais
          snprintf(remVer4, sizeof(remVer4), " - [OTA] %s", msgTelegram4);

          Serial.println(remVer4);       // Saída no monitor serial
          logToWeb(remVer4);            // Registro no log web
          enviarMensagem(msgTelegram4); // Envio para Telegram 
            
          Update.end();
          http.end();
          return;
        }

      if (!Update.end() || !Update.isFinished()) 
        {
          char remVer5[64];
          const char* msgTelegram5 = "Erro ao finalizar Update.";

          // Formatação segura mantendo os nomes originais
          snprintf(remVer5, sizeof(remVer5), " - [OTA] %s", msgTelegram5);

          Serial.println(remVer5);        // 1. Saída no Serial
          logToWeb(remVer5);              // 2. Log na Web
          enviarMensagem(msgTelegram5);   // 3. Mensagem para Telegram
           
          http.end();
          return;
        }

          char remVer6[64];
          const char* msgTelegram6 = "Atualização concluída. Reiniciando...";

          // Formatação segura usando snprintf (protege contra overflow)
          snprintf(remVer6, sizeof(remVer6), " - [OTA] %s", msgTelegram6);

          // Saídas do sistema (mantendo a estrutura original)
          Serial.println(remVer6);       // 1. Log no monitor serial
          logToWeb(remVer6);             // 2. Registro no log web
          enviarMensagem(msgTelegram6);  // 3. Notificação via Telegram

          delay(2000);
          ESP.restart();
    }

void verificarAtualizacao() 
    {
      if ((T_horario >= "11:45:00") && (T_horario <= "11:46:00") && switchUpdata)
        {
          switchUpdata = false;
      
      String remoteVersion = obterVersaoRemota();

      if (remoteVersion != "" && !remoteVersion.equals(CURRENT_VERSION)) 
        { 
          char mensagemUpdate[256];  // Tamanho suficiente para sua mensagem

          snprintf(mensagemUpdate, sizeof(mensagemUpdate),
            "📢 *Nova atualização disponível!*\n\n"
            "Versão atual: `%s`\n"
            "Nova versão: `%s`\n\n"
            "Atualize ->          /update",
            CURRENT_VERSION, remoteVersion.c_str()
          );

          novaVersaoDisponivel = true;
          led_update = true; // Pisca o led vermelho indicando nova atualização

          Serial.println("=== Verificação de atualização ===");
          Serial.print("CURRENT_VERSION: [");
          Serial.print(CURRENT_VERSION);
          Serial.println("]");
          Serial.print("remoteVersion:   [");
          Serial.print(remoteVersion);
          Serial.println("]");

          // 👉 Envia mensagem pelo Telegram
          bot->sendMessage(idBuffer, mensagemUpdate, "Markdown");
          logToWeb(mensagemUpdate);         
        }
        }       
    }

String obterVersaoRemota() 
    {           
          const char* url = "https://raw.githubusercontent.com/robedua/OTA/refs/heads/main/version.txt";

          HTTPClient http;
          http.begin(url);
          int httpCode = http.GET();

          if (httpCode == 200) 
            {
              String payload = http.getString();
              payload.replace("\r", "");
              payload.replace("\n", "");
              payload.trim();
              http.end();
              return payload;
            } 
            else 
              {
                Serial.printf(" - [OTA] Erro HTTP: %d\n", httpCode);
                logToWeb(" - [OTA] Falha ao obter versão remota.");
                http.end();
                return ""; 
              }
    }


