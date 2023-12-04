/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fonts.h"
#include "ssd1306.h"
#include "stdio.h"
#include <stdbool.h>
#include <string.h>
#include "bitmap.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#define TRIG_PIN GPIO_PIN_9
#define TRIG_PORT GPIOC
#define ECHO_PIN GPIO_PIN_8
#define ECHO_PORT GPIOC
#define pin_led GPIO_PIN_1
#define pin_ledport GPIOA
uint32_t pMillis;
uint32_t Value1 = 0;
uint32_t Value2 = 0;
uint16_t Distance = 0;  // cm

const uint8_t numLecturas = 10;
uint8_t lecturas[10]; // Array para almacenar lecturas

// Distancia a los 100 ml y vacío
const uint16_t distancia100 = 2.15;
const uint16_t distanciaVacio = 11.41;

uint16_t distanciaLleno = 0;
uint16_t cantidadLiquido = 0;
int porcentaje = 0;
uint16_t lecturaActual = 0; // Lectura por la que vamos
uint16_t total = 0; // Total de las que llevamos
uint16_t media = 0; // Media de las medidas
uint32_t DETECTAR_LIM = 0;
bool primeraMedia = false; // Para saber que ya hemos calculado por lo menos una


char strCopy[15];

const uint8_t seven_seg_digits_decode_abcdefg[75] = {
/*  0     1     2     3     4     5     6     7     8     9     :     ;     */
0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B, 0x40, 0x20,
/*  <     =     >     ?     @     A     B     C     D     E     F     G     */
0x10, 0x00, 0x04, 0x02, 0x01, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47, 0x5E,
/*  H     I     J     K     L     M     N     O     P     Q     R     S     */
0x37, 0x06, 0x3C, 0x57, 0x0E, 0x55, 0x15, 0x7E, 0x67, 0x73, 0x05, 0x5B,
/*  T     U     V     W     X     Y     Z     [     \     ]     ^     _     */
0x0F, 0x3E, 0x1C, 0x5C, 0x13, 0x3B, 0x6D, 0x02, 0x04, 0x08, 0x10, 0x20,
/*  `     a     b     c     d     e     f     g     h     i     j     k     */
0x40, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47, 0x5E, 0x37, 0x06, 0x3C, 0x57,
/*  l     m     n     o     p     q     r     s     t     u     v     w     */
0x0E, 0x55, 0x15, 0x1D, 0x67, 0x73, 0x05, 0x5B, 0x0F, 0x3E, 0x1C, 0x5C,
/*  x     y     z     */
0x13, 0x3B, 0x6D };



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */
void Write_D4_FSM(uint32_t current_tick, uint32_t wait, uint8_t *Msg);
void Write_ASCII_D4(uint8_t letter, uint8_t Digit, const uint8_t *Data);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static uint32_t Confirmar_lectura = 0;
static uint32_t DETECTAR_CLICK = 0;
static uint32_t DETECTAR_CLICK2 = 0;
char KeyPad_Read(void) {

	static uint8_t Char;
	static uint32_t cont = 0;
	static uint32_t lastTick = 0;
	if (HAL_GetTick() - lastTick > 700) {
		HAL_GPIO_WritePin(F1_GPIO_Port, F1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(F2_GPIO_Port, F2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(F3_GPIO_Port, F3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(F4_GPIO_Port, F4_Pin, GPIO_PIN_SET);

		if (!(HAL_GPIO_ReadPin(C1_GPIO_Port, C1_Pin))) {
			if (!(HAL_GPIO_ReadPin(C1_GPIO_Port, C1_Pin)))
				;
			Confirmar_lectura = 1;
			Char = '1';
		} else {
			cont = cont + 1;
		}

		if (!(HAL_GPIO_ReadPin(C2_GPIO_Port, C2_Pin))) {
			if (!(HAL_GPIO_ReadPin(C2_GPIO_Port, C2_Pin)))
				;
			Confirmar_lectura = 1;
			Char = '2';
		} else {
			cont = cont + 1;
		}

		if (!(HAL_GPIO_ReadPin(C3_GPIO_Port, C3_Pin))) {
			if (!(HAL_GPIO_ReadPin(C3_GPIO_Port, C3_Pin)))
				;
			Confirmar_lectura = 1;
			Char = '3';
		} else {
			cont = cont + 1;
		}

		if (!(HAL_GPIO_ReadPin(C4_GPIO_Port, C4_Pin))) {
			if (!(HAL_GPIO_ReadPin(C4_GPIO_Port, C4_Pin)))
				;
			Confirmar_lectura = 1;
			Char = 'A';
		} else {
			cont = cont + 1;
		}

		//--------------------------------------------------------------
		HAL_GPIO_WritePin(F1_GPIO_Port, F1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(F2_GPIO_Port, F2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(F3_GPIO_Port, F3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(F4_GPIO_Port, F4_Pin, GPIO_PIN_SET);

		if (!(HAL_GPIO_ReadPin(C1_GPIO_Port, C1_Pin))) {
			if (!(HAL_GPIO_ReadPin(C1_GPIO_Port, C1_Pin)))
				;
			Confirmar_lectura = 1;
			Char = '4';
		} else {
			cont = cont + 1;
		}

		if (!(HAL_GPIO_ReadPin(C2_GPIO_Port, C2_Pin))) {
			if (!(HAL_GPIO_ReadPin(C2_GPIO_Port, C2_Pin)))
				;
			Confirmar_lectura = 1;
			Char = '5';
		} else {
			cont = cont + 1;
		}

		if (!(HAL_GPIO_ReadPin(C3_GPIO_Port, C3_Pin))) {
			if (!(HAL_GPIO_ReadPin(C3_GPIO_Port, C3_Pin)))
				;
			Char = '6';
		} else {
			cont = cont + 1;
		}

		if (!(HAL_GPIO_ReadPin(C4_GPIO_Port, C4_Pin))) {
			if (!(HAL_GPIO_ReadPin(C4_GPIO_Port, C4_Pin)))
				;
			Confirmar_lectura = 1;
			Char = 'B';
		} else {

			cont = cont + 1;
		}
		//--------------------------------------------------------------
		HAL_GPIO_WritePin(F1_GPIO_Port, F1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(F2_GPIO_Port, F2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(F3_GPIO_Port, F3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(F4_GPIO_Port, F4_Pin, GPIO_PIN_SET);

		if (!(HAL_GPIO_ReadPin(C1_GPIO_Port, C1_Pin))) {
			if (!(HAL_GPIO_ReadPin(C1_GPIO_Port, C1_Pin)))
				;
			Confirmar_lectura = 1;
			Char = '7';
		} else {
			cont = cont + 1;
		}

		if (!(HAL_GPIO_ReadPin(C2_GPIO_Port, C2_Pin))) {
			if (!(HAL_GPIO_ReadPin(C2_GPIO_Port, C2_Pin)))
				;
			Confirmar_lectura = 1;
			Char = '8';
		} else {
			cont = cont + 1;
		}

		if (!(HAL_GPIO_ReadPin(C3_GPIO_Port, C3_Pin))) {
			if (!(HAL_GPIO_ReadPin(C3_GPIO_Port, C3_Pin)))
				;
			Confirmar_lectura = 1;
			Char = '9';
		} else {
			cont = cont + 1;
		}

		if (!(HAL_GPIO_ReadPin(C4_GPIO_Port, C4_Pin))) {
			if (!(HAL_GPIO_ReadPin(C4_GPIO_Port, C4_Pin)))
				;
			Confirmar_lectura = 1;
			Char = 'C';
		} else {

			cont = cont + 1;
		}

		//--------------------------------------------------------------
		HAL_GPIO_WritePin(F1_GPIO_Port, F1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(F2_GPIO_Port, F2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(F3_GPIO_Port, F3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(F4_GPIO_Port, F4_Pin, GPIO_PIN_RESET);

		if (!(HAL_GPIO_ReadPin(C1_GPIO_Port, C1_Pin))) {
			if (!(HAL_GPIO_ReadPin(C1_GPIO_Port, C1_Pin)))
				;
			Confirmar_lectura = 1;
			Char = 'E';
		} else {
			cont = cont + 1;
		}

		if (!(HAL_GPIO_ReadPin(C2_GPIO_Port, C2_Pin))) {
			if (!(HAL_GPIO_ReadPin(C2_GPIO_Port, C2_Pin)))
				;
			Confirmar_lectura = 1;
			Char = '0';
		} else {
			cont = cont + 1;
		}

		if (!(HAL_GPIO_ReadPin(C3_GPIO_Port, C3_Pin))) {
			if (!(HAL_GPIO_ReadPin(C3_GPIO_Port, C3_Pin)))
				;
			Confirmar_lectura = 1;
			Char = 'H';
		} else {
			cont = cont + 1;
		}

		if (!(HAL_GPIO_ReadPin(C4_GPIO_Port, C4_Pin))) {
			if (!(HAL_GPIO_ReadPin(C4_GPIO_Port, C4_Pin)))
				;
			Confirmar_lectura = 1;
			Char = 'D';
		} else {
			cont = cont + 1;
		}
		if (cont == 16) {
			cont = 0;
			if (Confirmar_lectura == 1) {
				Confirmar_lectura = 0;
				return Char;
			}
		} else {
			cont = 0;
		}
	}

	return 'G';
}
uint8_t msg[5] = "====";
uint8_t msg3[5] = "====";
uint8_t ContraCifrada[5] = "====";
uint8_t Contra[5] = "0000";
static uint32_t Estados = 2;
void LECTURA_BTN(void)

{

	static uint32_t VARIABLE_COMPROBAR_CLICK = 0; // VARIABLE PARA IMPLEMENTAR MAQUINA DE ESTADOS CON EL BOTON
	// LECTURA DEL PIN DEL BOTON LOGICA INVERSA
	uint32_t LECTURA_PUERTO_BOTON = HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin); // LEEMOS TODO EL PUERTO C QUE ES EL DEL BOTON

	// EVALUAMOS CONDICIONES
	if (LECTURA_PUERTO_BOTON != 0) {

		// EVALUO SI YA HABIA SIDO PRESIONADO
		if (VARIABLE_COMPROBAR_CLICK == 1) {
			// SI FUE PRESIONADO PONGO LA VARIABLE DE ESTADO EN CERO PARA INDICAR QUE YA SE EJECUTO LA ACCION DESEADA
			VARIABLE_COMPROBAR_CLICK = 0;
			DETECTAR_CLICK = 1; // REVISO EL TIEMPO ACTUAL PARA COMPARAR DESDE EL TIEMPO EN QUE SE PRESIONO

		}
	} else {
		// COMPROBAMOS EL PRIMER CLICK
		if (VARIABLE_COMPROBAR_CLICK == 0) {
			VARIABLE_COMPROBAR_CLICK = 1; // CAMBIO EL VALOR DE LA VARIABLE DE ESTADO A UNO PARA INDICAR QUE SE PRESIONO
		}
		//LL_GPIO_SetOutputPin(LED2_GPIO_PORT, LED2_PIN);

	}

}
void LECTURA_BTN2(void)

{

	static uint32_t VARIABLE_COMPROBAR_CLICK = 0; // VARIABLE PARA IMPLEMENTAR MAQUINA DE ESTADOS CON EL BOTON
	// LECTURA DEL PIN DEL BOTON LOGICA INVERSA
	uint32_t LECTURA_PUERTO_BOTON = HAL_GPIO_ReadPin(BTN2_GPIO_Port, BTN2_Pin); // LEEMOS TODO EL PUERTO C QUE ES EL DEL BOTON

	// EVALUAMOS CONDICIONES
	if (LECTURA_PUERTO_BOTON != 0) {

		// EVALUO SI YA HABIA SIDO PRESIONADO
		if (VARIABLE_COMPROBAR_CLICK == 1) {
			// SI FUE PRESIONADO PONGO LA VARIABLE DE ESTADO EN CERO PARA INDICAR QUE YA SE EJECUTO LA ACCION DESEADA
			VARIABLE_COMPROBAR_CLICK = 0;
			DETECTAR_CLICK2 = 1; // REVISO EL TIEMPO ACTUAL PARA COMPARAR DESDE EL TIEMPO EN QUE SE PRESIONO

		}
	} else {
		// COMPROBAMOS EL PRIMER CLICK
		if (VARIABLE_COMPROBAR_CLICK == 0) {
			VARIABLE_COMPROBAR_CLICK = 1; // CAMBIO EL VALOR DE LA VARIABLE DE ESTADO A UNO PARA INDICAR QUE SE PRESIONO
		}
		//LL_GPIO_SetOutputPin(LED2_GPIO_PORT, LED2_PIN);

	}

}
void Write_D4_FSM(uint32_t current_tick, uint32_t wait, uint8_t *Msg) {
	static uint32_t last_tick = 0;
	static uint32_t state = 0, next_state = 0;

	static uint8_t i = 0;

	if (current_tick - last_tick > wait) {
		if (state == 0) {
			Write_ASCII_D4((Msg[i] - 0x30), 4, seven_seg_digits_decode_abcdefg);
			next_state = state + 1;
		}
		if (state == 1) {
			Write_ASCII_D4((Msg[i + 1] - 0x30), 3,
					seven_seg_digits_decode_abcdefg);
			next_state = state + 1;
		}
		if (state == 2) {
			Write_ASCII_D4(Msg[i + 2] - 0x30, 2,
					seven_seg_digits_decode_abcdefg);
			next_state = state + 1;
		}
		if (state == 3) {
			Write_ASCII_D4(Msg[i + 3] - 0x30, 1,
					seven_seg_digits_decode_abcdefg);
			next_state = 0;
		}
		state = next_state;
		last_tick = current_tick;
	}
}

void Write_ASCII_D4(uint8_t letter, uint8_t Digit, const uint8_t *Data) {

	//	static uint32_t  last_tick = 0;
	//	static uint32_t state = 0, next_state = 4;
	//	static uint8_t letter = 0;

	//	if (  current_tick - last_tick > wait){

	//  LCLK (PB5) -
	HAL_GPIO_WritePin(LCLK_GPIO_Port, LCLK_Pin, GPIO_PIN_RESET);

	// SCLK (PA8) - 16 clock cycles
	HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_RESET);
	// Dot is set to turn it off
	HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_SET);
	HAL_GPIO_TogglePin(SCLK_GPIO_Port, SCLK_Pin);
	HAL_GPIO_TogglePin(SCLK_GPIO_Port, SCLK_Pin);
	for (int i = 0; i < 7; i++) {
		// SDI (PA9) - set
		HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, ~(Data[letter] >> i) & (1));
		HAL_GPIO_TogglePin(SCLK_GPIO_Port, SCLK_Pin);
		HAL_GPIO_TogglePin(SCLK_GPIO_Port, SCLK_Pin);
	}

	// All the displays are enable
	HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_RESET);
	for (int i = 0; i < 8; i++) {
		if (i == Digit + 3) {
			HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_RESET);
		}
		HAL_GPIO_TogglePin(SCLK_GPIO_Port, SCLK_Pin);
		HAL_GPIO_TogglePin(SCLK_GPIO_Port, SCLK_Pin);
	};
	//  LCLK (PB5) -
	HAL_GPIO_WritePin(LCLK_GPIO_Port, LCLK_Pin, GPIO_PIN_SET);
	//		letter++;
	//		if (letter > 74)
	//			letter = 0;
	//		last_tick = current_tick;
	//	}
}

void servo(uint8_t angul0)
{

uint16_t period = (1.0/90*angul0 + 0.5)*1000;

htim4.Init.Prescaler = 32;
HAL_TIM_Base_Init(&htim4);

for (int i = 0; i < 30; ++i) {

__HAL_TIM_SET_COUNTER(&htim4,0);
HAL_TIM_Base_Start(&htim4);
HAL_GPIO_WritePin(SM_GPIO_Port, SM_Pin, GPIO_PIN_SET); // Pin Motor

while(__HAL_TIM_GET_COUNTER(&htim4) < period);
HAL_GPIO_WritePin(SM_GPIO_Port, SM_Pin, GPIO_PIN_RESET); // Pin Motor

HAL_TIM_Base_Stop(&htim4);
__HAL_TIM_SET_COUNTER(&htim4,0);
HAL_TIM_Base_Start(&htim4);

while(__HAL_TIM_GET_COUNTER(&htim4) < 20000-period);
}

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	static uint8_t keychar;
	static uint32_t current_tick;
	static uint8_t i = 0;
	static uint32_t last_tick = 0;
	static uint32_t last_tick2 = 0;
	static uint8_t tick = 0;
	static uint32_t tiempo_espera = 10000;
	static uint32_t intentos = 1;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

	SSD1306_Clear();
	HAL_TIM_Base_Start(&htim1);
	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MB_GPIO_Port, MB_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET); // pull the TRIG pin low
	SSD1306_DrawBitmap(0, 0, logo, 128, 64, 1);
	SSD1306_UpdateScreen();
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_Delay(2000);
	SSD1306_Clear();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		HAL_GPIO_WritePin(pin_ledport, pin_led, SET);

		HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET); // pull the TRIG pin HIGH
		__HAL_TIM_SET_COUNTER(&htim1, 0);
		while (__HAL_TIM_GET_COUNTER (&htim1) < 10)
			;  // wait for 10 us
		HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET); // pull the TRIG pin low

		pMillis = HAL_GetTick(); // used this to avoid infinite while loop  (for timeout)
		// wait for the echo pin to go high
		while (!(HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN))
				&& pMillis + 10 > HAL_GetTick())
			;
		Value1 = __HAL_TIM_GET_COUNTER(&htim1);

		pMillis = HAL_GetTick(); // used this to avoid infinite while loop (for timeout)
		// wait for the echo pin to go low
		while ((HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN))
				&& pMillis + 50 > HAL_GetTick())
			;
		Value2 = __HAL_TIM_GET_COUNTER(&htim1);

		Distance = (Value2 - Value1) * 0.034 / 2;

		// Calculamos la media

//		    float distanciaLleno = distanciaVacio - Distance;
//		    float cantidadLiquido = distanciaLleno * 100 / distancia100;
//		    int porcentaje = (int) (distanciaLleno * 100 / distanciaVacio);

		SSD1306_GotoXY(0, 0);
		SSD1306_Puts("Dist: (Cm)", &Font_11x18, 1);
		sprintf(strCopy, "%d    ", Distance);
		SSD1306_GotoXY(0, 30);
		SSD1306_Puts(strCopy, &Font_16x26, 1);
		SSD1306_UpdateScreen();
		HAL_Delay(50);

		if (Distance > 10) {
				DETECTAR_LIM = 1;
			}
		if (Distance  <4) {
			DETECTAR_LIM = 0;
		}
		if (DETECTAR_LIM == 1) {

			HAL_GPIO_WritePin(MB_GPIO_Port, MB_Pin, GPIO_PIN_RESET);

		} else {
			HAL_GPIO_WritePin(MB_GPIO_Port, MB_Pin, GPIO_PIN_SET);
		}



//		LEYENDO LA CONTRASEÑA
		current_tick = HAL_GetTick();

			if (current_tick - last_tick >= 250) {
				last_tick = current_tick;
				HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			}

			current_tick = HAL_GetTick();	//LEEMOS EL TIEMPOR ACTUAL

			if (Estados == 2) {
				// ESTADO DONDE LEEMOS LA CONTRASEÑA
				keychar = KeyPad_Read(); /// LEEMOS LA SECUENCIA DE LOS DATOS
				if (keychar != 'G') {
					msg[i] = keychar;
					i++;
					Write_D4_FSM(current_tick, 1, msg);
					if (i > 3) {

						i = 0;

						if (!strncmp(msg, Contra, 4)) {
							HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, GPIO_PIN_SET);
							tick = 0;
							intentos = 1; // REINICIAMOS EL TIEMPO DE ESPERA AL MINIMO.
							Estados = 4; // VAMOS AL ESTADO DE ABIERTO, EN EL CUAL PROCEDEMOS A LEER EL BOTON 2 PARA CERRAR LA CAJA.
							servo(180);
						} else {
							tick++; // SUMA LOS INTENTOS ERRADOS
							HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_SET);
							if (tick == 3) {
								last_tick2 = last_tick;
								tick = 0;
								HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin,
										GPIO_PIN_RESET);
								HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin,
										GPIO_PIN_RESET);
								Estados = 3; // ESTADO DE BLOQUEADO

							}
						}
						strcpy(msg, msg3); //Lo que hay en apellido se copia a nombre;

					}

				}
				Write_D4_FSM(current_tick, 1, msg);

			}

			if (Estados == 1) {

				keychar = KeyPad_Read(); /// LEEMOS LA SECUENCIA DE LOS DATOS
				if (keychar != 'G') {
					Contra[i] = keychar;
					ContraCifrada[i] = ']';
					i++;
					Write_D4_FSM(current_tick, 1, ContraCifrada);
					if (i > 3) {

						strcpy(ContraCifrada, msg3);
						i = 0;
						Estados = 4; // LA PUERTA SIGUE ABIERTA
						HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, GPIO_PIN_SET);
					}

				}
				Write_D4_FSM(current_tick, 1, ContraCifrada);

			}
			// ESTADO PARA RETENER DEBIDO A INTENTOS FALLIDOS

			if (Estados == 3) {

				Write_D4_FSM(HAL_GetTick(), 1, "FAIL");

				if (HAL_GetTick() - last_tick2 == tiempo_espera * intentos) {

					if (intentos < 4) {
						intentos++; // EL TIEMPO MAXIMO DE ESPERA SERA DE 40 SEGUNDOS
					}
					Estados = 2; // VOLVER AL ESTADO INICIAL
					HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, GPIO_PIN_SET);
				}
			}
			if (Estados == 4) {

				Write_D4_FSM(HAL_GetTick(), 1, "OPEN");
				LECTURA_BTN2(); // LEEMOS EL BOTON1. PARA SABER SI QUEREMOS CAMBIAR CONTRASEÑA
				if (DETECTAR_CLICK2 == 1) {
					DETECTAR_CLICK2 = 0;
					Estados = 2; // CERRRAMOS LA PUERTA
					HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, GPIO_PIN_SET);
					servo(0);
				}
				LECTURA_BTN(); // LEEMOS EL BOTON1. PARA SABER SI QUEREMOS CAMBIAR CONTRASEÑA
				if (DETECTAR_CLICK == 1) {
					DETECTAR_CLICK = 0;
					Estados = 1; // NOS VAMOS A CAMBIAR CONTRASEÑA UNA VEZ SI PUDO ABRIR LA COMPUERTA.
					HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, GPIO_PIN_RESET);
				}

			}



	}

//		if (Distance < 10)
//			HAL_GPIO_WritePin(pin_ledport, pin_led, RESET);
//		else
//			HAL_GPIO_WritePin(pin_ledport, pin_led, SET);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 9;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00702681;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 127;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 625;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SM_Pin|MB_Pin|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|D2_Pin|D3_Pin|SCLK_Pin
                          |SDI_Pin|F2_Pin|F1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, F4_Pin|F3_Pin|LCLK_Pin|D4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SM_Pin MB_Pin PC9 */
  GPIO_InitStruct.Pin = SM_Pin|MB_Pin|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN2_Pin */
  GPIO_InitStruct.Pin = BTN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin D2_Pin D3_Pin SCLK_Pin
                           SDI_Pin F2_Pin F1_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|D2_Pin|D3_Pin|SCLK_Pin
                          |SDI_Pin|F2_Pin|F1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN1_Pin PC8 */
  GPIO_InitStruct.Pin = BTN1_Pin|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : C2_Pin C1_Pin C4_Pin C3_Pin */
  GPIO_InitStruct.Pin = C2_Pin|C1_Pin|C4_Pin|C3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : F4_Pin F3_Pin LCLK_Pin D4_Pin */
  GPIO_InitStruct.Pin = F4_Pin|F3_Pin|LCLK_Pin|D4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
			/* User can add his own implementation to report the HAL error return state */
			__disable_irq();
			while (1) {
			}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
