#include <18f452.h>
#device adc=10
#fuses HS, XT, NOWDT, NOPROTECT, NOBROWNOUT, NOLVP, NOPUT, NOWRT, NODEBUG, NOCPD 
#use delay(clock=20Mhz)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, parity=N, STREAM=iletisim_1, stop=1)

#define DHT11_LED PIN_D1
#define DS18B20_LED PIN_D2
#define HCSR04_LED PIN_D3
#define WATER_PUMP_LED PIN_D4
#define SPRAYING_MACHINE_LED PIN_D5
#define LDR_LED PIN_D6

#define FLAME_SENSOR PIN_E0
#define FLAME_BUZZER PIN_D0
#define START_LED PIN_E1
#define STOP_LED PIN_E2
#define MOTOR_IN1 PIN_A4
#define MOTOR_IN2 PIN_A5
#define MOTOR_IN3 PIN_C0
#define MOTOR_IN4 PIN_C1

#include <dht_11.c>
#include <stdbool.h>
#include <string.h>
#include <ds18b20.c>

// Global deðiþkenler
unsigned int16 distance,water_level;
unsigned int8 ldr_value;
unsigned int8 isi, nem;
int work_stat_flag, ldr_flag;
int flame_status,motor_control_stat;
// Önceki deðerleri saklama
int last_work_stat_flag = 0;
int last_flame = 0;
int last_ldr_flag = 0;
int last_isi = 0;
int last_nem = 0;
int last_spraying_machine_stat=0;
int last_motor_control_stat = 0;

unsigned long last_water_level = 0;

int ldr_sensor_test_flag = 0,ultrasonic_sensor_test_flag = 0,dht_sensor_test_flag = 0,spraying_machine_control_stat=0;
bool system_work, system_work_uart_control,flame_alarm_uart_control,water_pump_uart_control,ultrasonic_sensor_uart_control,dht11_sensor_uart_control,ldr_sensor_uart_control,spraying_machine_uart_control=0,ds18b20_sensor_test_flag= 0,ds18b20_uart_control = 0;

float last_temp;

#int_rda
void terminal_flame_alarm_control() {
    char c;
    if (kbhit()) {
        c = getchar();  // tampon boþsa okumaz, bloklamaz
        switch (c) {
            case 'q':
                flame_alarm_uart_control = 1;

                break;
            case 'w':
                flame_alarm_uart_control = 0;

                break;
            case 'd':
                system_work_uart_control = 1;

                break;
            case 'f':
                system_work_uart_control = 0;

                break;
            case 'e':
                water_pump_uart_control = 1;

                break;
            case 'r':
                water_pump_uart_control = 0;

                break;
            case 't':
                ultrasonic_sensor_uart_control = 1;

                break;
            case 'y':
                ultrasonic_sensor_uart_control = 0;

                break;
            case 'u':
                dht11_sensor_uart_control = 1;

                break;
            case 'i':  // ASCII uyumlu
                dht11_sensor_uart_control = 0;

                break;
            case 'o':
                ldr_sensor_uart_control = 1;

                break;
            case 'p':
                ldr_sensor_uart_control = 0;

                break;
            case 'a':
                spraying_machine_uart_control = 1;

                break;
            case 's':
                spraying_machine_uart_control = 0;

                break;
            case 'h':
                ds18b20_uart_control = 1;

                break;
            case 'g':
                ds18b20_uart_control = 0;

                break;
            default:
                // tanýmsýz karakter
                break;
        }
    }
}


void spraying_machine_control(){
   if(spraying_machine_uart_control == 1 && system_work_uart_control == 0){
      output_high(SPRAYING_MACHINE_LED);
      output_high(PIN_C0);
      output_low(PIN_C1);
      spraying_machine_control_stat = 1;
   }
   else{
   spraying_machine_control_stat= 0;
   output_low(SPRAYING_MACHINE_LED);
   output_low(PIN_C0);
   output_low(PIN_C1);
   }
}

void motor_control(){
if(water_pump_uart_control == 0 &&system_work_uart_control == 0 ){
   if(water_level <=60){output_low(PIN_A5); output_high(PIN_A4);motor_control_stat =1;output_high(WATER_PUMP_LED); }
   if(water_level >60 && water_level<70  ){motor_control_stat =0;output_low(PIN_A4); output_low(PIN_A5);output_low(WATER_PUMP_LED);}
   if(water_level > 70){output_low(PIN_A4); output_high(PIN_A5);motor_control_stat = -1;output_high(WATER_PUMP_LED); }
}
else{ motor_control_stat= 0;output_low(PIN_A4); output_low(PIN_A5);}
}

// Su tanký seviyesi hesaplama
void water_tank_level_calc() {
    water_level = distance * 100 / 18; // Tank seviyesi yüzdesi
}
// Mesafe hesaplama fonksiyonu
void distance_calc() {
   if(ultrasonic_sensor_uart_control == 0 && system_work_uart_control == 0){
    unsigned int16 pulse_duration = 0;
    output_high(PIN_B1);
    delay_us(20);
    output_low(PIN_B1);
    set_timer1(0);
    output_high(HCSR04_LED);
    while (!input(PIN_B2) && get_timer1() < 1200);
    while (input(PIN_B2) && pulse_duration < 1800)
    pulse_duration = get_timer1();
    distance = pulse_duration / 58; // cm cinsinden mesafe
   }
   else{ distance = 0;output_low(HCSR04_LED);}
}


// Alev sensörü kontrolü
void flame_sensor_control() {

    if ((input(FLAME_SENSOR) != 0) || (flame_alarm_uart_control == 1)) {
        while ((input(FLAME_SENSOR) != 0) || (flame_alarm_uart_control == 1)) {
            flame_status = 1;
            printf("\n\rFlame Alarm On ");
            delay_ms(1000);
            output_high(FLAME_BUZZER);

            // Eðer UART kontrolü tetiklendiyse ve sensör devre dýþýysa
            if (flame_alarm_uart_control == 1 && input(FLAME_SENSOR) == 0) {
                printf("  [UART]");

                break; // UART alarm durumunda döngüden çýk
            }
        }
    } else {
        flame_status = 0;
        output_low(FLAME_BUZZER);
    } 
}


// DHT11 sensör kontrolü
void dht11_control() {
   if(dht11_sensor_uart_control == 0 && system_work_uart_control == 0){
   output_high(DHT11_LED);
    read_dht();
    isi = dht_dat[2]; // Sýcaklýk
    nem = dht_dat[0]; // Nem
   }else{
      output_low(DHT11_LED);
      isi = 0;
      nem = 0;
      }
}

// LDR ýþýk kontrolü
void led_check_system() {
if(ldr_sensor_uart_control ==0 && system_work_uart_control == 0 ){
    set_adc_channel(0);
    delay_us(20);
    ldr_value = read_adc();
    if (ldr_value <= 100){
        ldr_flag = 1; // Iþýk açýk
         delay_us(50);
        output_high(PIN_B5);
        output_high(PIN_B6);
        output_high(PIN_B7);
        output_high(LDR_LED);
}else{
         ldr_flag = 0; // Iþýk kapalý
         delay_us(50);
         output_low(PIN_B5);
         output_low(PIN_B6);
         output_low(PIN_B7);
         output_low(LDR_LED);         
}
}else{
         ldr_flag = 0; // Iþýk kapalý
         delay_us(50);
         output_low(PIN_B5);
         output_low(PIN_B6);
         output_low(PIN_B7);
         output_low(LDR_LED);
   }

}

// LDR sensör kontrolü
void ldr_sensor_check() {
    if (ldr_value >= 1 && ldr_value < 1023) { // ADC'nin tam aralýðýný kontrol edin
        ldr_sensor_test_flag = 1; // Sensör çalýþýyor
        printf("\n\rLDR Sensor  Working");
    } else {
        ldr_sensor_test_flag = 0; // Sensör hatalý
    }
}

// Ultrasonik sensör kontrolü
void ultrasonic_sensor_check() {
    if (water_level >= 1 && water_level <= 100) { // Su seviyesi %1-%100 arasýnda mý?
        ultrasonic_sensor_test_flag = 1; // Sensör çalýþýyor
           printf("\n\rUltrasonic Sensor Working");
    } else {
        ultrasonic_sensor_test_flag = 0; // Sensör hatalý
    }
}

// DHT11 sensör kontrolü
void dht11_sensor_check() {
    if (isi >= 0 && isi <= 100 && nem >= 0 && nem <= 100) { // DHT11'in tipik aralýklarý
        dht_sensor_test_flag = 1; // Sensör çalýþýyor
        printf("\n\rDHT11 Sensor Working");
    } else {
        dht_sensor_test_flag = 0; // Sensör hatalý
    }
}
void ds18b20_sensor_check(){
   if(temp >= -10.00 && temp <= 80.00){
   ds18b20_sensor_test_flag = 1;
   printf("\n\rDS18B20 Sensor Working");
   }else {
      ds18b20_sensor_test_flag= 0;
   }

}
// Tüm sensörleri kontrol et
void all_sensors_check() {
    ldr_sensor_check();        // LDR sensör testi
    ultrasonic_sensor_check(); // Ultrasonik sensör testi
    dht11_sensor_check();      // DHT11 sensör testi
   ds18b20_sensor_check();
    if (ldr_sensor_test_flag == 1 && ultrasonic_sensor_test_flag == 1 && dht_sensor_test_flag == 1,ds18b20_sensor_test_flag == 1) {
        printf("\n\r Sensor Test is Completed Succesfully "); // Tüm sensörler çalýþýyor
         system_work = 1;
    } else {
         system_work = 0;
        // Sensörlerin durumlarýný rapor et
        if (ldr_sensor_test_flag != 1) {
            printf("\n\rLDR Sensor Not Working");
        }
        if (ultrasonic_sensor_test_flag != 1) {
            printf("\n\rUltrasonic Sensor Not Working");
        }
        if (dht_sensor_test_flag != 1) {
            printf("\n\rDHT11 Sensor Not Working");
        }
        if(ds18b20_sensor_test_flag != 1){
            printf("\n\rDS18B20 Sensor Not Working");
        
        }
    }
}

void water_temp_function(){
if(ds18b20_uart_control == 0 && system_work_uart_control == 0){
 if(ds18b20_read(&raw_temp)) {temp = (float)raw_temp / 16;output_high(DS18B20_LED);}
}else{ temp = 0;output_low(DS18B20_LED);}

}

void timer_count_func(){
   if(system_work == 1 && system_work_uart_control == 0 ){
     output_low(STOP_LED);
     output_high(START_LED);
     work_stat_flag = 1;
   }
else {
         output_low(START_LED);
         output_high(STOP_LED);
         work_stat_flag = 0;
}
  
}
void initial_display_flag() {
    output_low(FLAME_BUZZER);
    printf("Vertical Farming SYS");
    printf("\n\rLoading ...");
    distance_calc();
    flame_sensor_control();
    led_check_system();
    dht11_control();
    water_tank_level_calc();
    all_sensors_check();
}
void main() {
    set_tris_b(0x0F); 
    setup_timer_0(RTCC_INTERNAL | RTCC_DIV_256);
    setup_timer_1(T1_INTERNAL | T1_DIV_BY_4);
    setup_adc_ports(AN0);
    setup_adc(ADC_CLOCK_DIV_32);
    set_timer1(0);
    setup_psp(PSP_DISABLED);
    setup_timer_2(T2_DISABLED, 0, 1);
    setup_adc_ports(AN0);
    setup_adc(ADC_CLOCK_DIV_32);
    setup_CCP1(CCP_OFF);
    setup_CCP2(CCP_OFF);
    enable_interrupts(GLOBAL);
    //enable_interrupts(INT_timer0);
    enable_interrupts(INT_RDA);
    ext_int_edge(L_TO_H);
    initial_display_flag();
    delay_ms(500);

    while (TRUE) {
    //Water level ultrasonic sensor func    
        distance_calc();
    //Flame Sensor Control    
        flame_sensor_control();
    // Led Control with LDR    
        led_check_system();
    // Air condition control    
        dht11_control();
    // Main tank level control    
        water_tank_level_calc();
    // Timer    
        timer_count_func();
    // Water Pump control    
        motor_control();
    // Water temp control   
       water_temp_function();
    // Spreying Machine control   
        spraying_machine_control();
        if (last_temp !=temp||work_stat_flag != last_work_stat_flag || spraying_machine_control_stat != last_spraying_machine_stat || ldr_flag != last_ldr_flag ||
            isi != last_isi || nem != last_nem || water_level != last_water_level|| motor_control_stat != last_motor_control_stat ) {
            // UART Log Mesajý
            printf("\n\r [LOG] Status: %d | Light: %d | Temp: %02dC | Humidity: %02d%% | Water Level: %3Lu%% | Spraying Machine: %d |  Motor Stat: %d | Water Temp : %f\n",
                   work_stat_flag,
                   ldr_flag ,
                   isi, nem, water_level,
                   spraying_machine_control_stat,
                   motor_control_stat,temp);
            // Deðerleri güncelle
            last_temp = temp;
            last_work_stat_flag = work_stat_flag;
            last_flame = flame_status;
            last_ldr_flag = ldr_flag;
            last_isi = isi;
            last_nem = nem;
            last_water_level = water_level;
            last_motor_control_stat = motor_control_stat;
            last_spraying_machine_stat= spraying_machine_control_stat;
        }
        delay_ms(1000); 
    }
}
