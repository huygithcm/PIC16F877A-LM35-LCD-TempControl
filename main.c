#include <16F877a.h>  // Include library
#device ADC=8         // Declare ADC 8-bit
#fuses HS,NOWDT,NOPUT,NOLVP
#use delay(clock=20000000)

// LCD pin definitions
#define LCD_RS_PIN       PIN_D0
#define LCD_RW_PIN       PIN_D1
#define LCD_ENABLE_PIN   PIN_D2

#define LCD_DATA4        PIN_D3
#define LCD_DATA5        PIN_D4
#define LCD_DATA6        PIN_D5
#define LCD_DATA7        PIN_D6 

#include <lcd.c>

// Define pins for components
#define LM35_SENSOR      PIN_A0
#define OUTPUT_DEVICE    PIN_C2

#define BUTTON_INC       PIN_C1
#define BUTTON_DEC       PIN_C0

float set_temperature = 0;
float current_temperature;
int8 lcd_line1[17];
int8 lcd_line2[17];

// Function to read button inputs
void read_buttons()
{
    if (input(BUTTON_INC) == 0) 
    {
        delay_ms(20);
        if (input(BUTTON_INC) == 0) { set_temperature = set_temperature + 1; }
    }
    if (input(BUTTON_DEC) == 0)
    {
        delay_ms(20);
        if (input(BUTTON_DEC) == 0) { set_temperature = set_temperature - 1; }
    }
    if (set_temperature < 0) { set_temperature = 0; }
}

// Function to initialize system settings
void initialize_system()
{
    setup_adc(ADC_CLOCK_DIV_8);
    setup_adc_ports(AN0);
    set_adc_channel(LM35_SENSOR); // Select ADC channel AN0
    lcd_init();
    lcd_gotoxy(1,1);
    printf(lcd_putc, " ");
    lcd_gotoxy(1,2);
    printf(lcd_putc, " ");
    
    delay_ms(1000);
}

// Function to read temperature from LM35 sensor
void read_temperature(int8 num_reads)
{
    int8 adc_read_count;
    float temp_sum;
    set_adc_channel(LM35_SENSOR); // Select ADC channel AN0
    temp_sum = 0;
    
    for (adc_read_count = 0; adc_read_count < num_reads; adc_read_count++)
    {
        temp_sum = temp_sum + ((read_adc() / 255.0) * 5000.0) / 10.0; 
        delay_ms(10);
    }
    current_temperature = temp_sum / num_reads;
}

// Main function
void main()
{
    initialize_system();
    lcd_putc("\f"); // Clear LCD

    while (TRUE)
    {    
        read_temperature(1);
        lcd_gotoxy(1,1);
        sprintf(lcd_line1, "TEMP :%.1f ", current_temperature);
        printf(lcd_putc, lcd_line1);
        lcd_putc(223); // Print degree symbol (°)
        lcd_putc("C ");
        
        lcd_gotoxy(1,2);
        sprintf(lcd_line2, "SET TEMP :%.1f ", set_temperature);
        printf(lcd_putc, lcd_line2);
        lcd_putc(223); // Print degree symbol (°)
        lcd_putc("C ");
        
        read_buttons();
        
        if (current_temperature > set_temperature)
        {
            output_high(OUTPUT_DEVICE);  
        }
        else 
        { 
            output_low(OUTPUT_DEVICE);
        }
    }
}

