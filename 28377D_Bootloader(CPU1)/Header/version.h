/*
 * version.h
 *
 *  Created on: 2021. 4. 7.
 *      Author: j.g.choe
 *
 *  Software Version Manager 2021. 4. 7, (c) G-Philos
 *
 */
#ifndef __VERSION_H
#define __VERSION_H

#define INDEX_SIGNITURE     (0)
#define INDEX_CORE_ID       (1)
#define INDEX_FIRMWARE_ID   (2)
#define INDEX_VERSION       (3)
#define INDEX_MODEL_ID      (4)
#define INDEX_CUSTOMER_ID   (5)
#define INDEX_CPU_ID        (6)
#define INDEX_BOARD_ID      (7)
#define INDEX_DATE_TIME_ID  (8)

#define VERSION_MAJOR       "1"
#define VERSION_MINOR       "0"

#define _CPU1 "C1"
#define _CPU2 "C2"

#define CORE_ID _CPU1

/*
 * Model ID
 *
 * 1KW   (1570)     28335
 * 1KW   (2545)     28335
 * 2.5KW (6741)     28335
 * 5KW   (42130)    28335
 * 6.5KW (11063)    28377D
 * 10KW  (67165)    28335
 */

#define __MODEL_ID_1570       "1570"
#define __MODEL_ID_2545       "2545"
#define __MODEL_ID_6741       "6741"
#define __MODEL_ID_42130      "42130"
#define __MODEL_ID_11063      "11063"
#define __MODEL_ID_67165      "67165"

#define MODEL_ID    __MODEL_ID_11063

/*
 * Customer ID
 * Reference: "4. 고객코드 ver.2.0 20201019.xlsx"
 *
 * Abbreviation     English Name            Korean Name
 * DF               DooSan Fuel Cell        (주)두산 퓨얼셀
 * AL               Acro LABS               아크로랩스 주식회사
 * BH               BumHan                  범한산업㈜
 * HP               H&Power                 H&Power
 * HS               Hyundai Steel           현대제철
 * KD               Kyung Dong Navien       ㈜경동나비엔
 * LC               LG Chem                 LG화학
 * MC               MICO                    (주)미코
 * PP               P&P ENG                 P&P ENG
 * PR               Pureun                  푸른기술에너지
 * SF               S-Fuel Cell             에스퓨얼셀(주)
 * ST               STX Heavy Industries    STX중공업
 * KE               Korea Institute         한국에너지기술연구원
 *                  of energy Research
 * KC               K Ceracell              케이세라셀
 * US               UPSTART                 UPSTART
 * HG               Hydro Genics            하이드로 제닉스
 * DA               Dong-A Fuel cell        동아퓨얼셀
 * EA               Energy                  에너지와 공조
 *                  Air Conditioning
 * CE               Cotek Energy            코텍에너지
 *
 */

#define CUSTOMER_ID     "DU"

/*
 * Firmware ID
 *
 * CONTROLLER
 * BOOTLOADER
 * BLACKBOX
 */

#define FIRMWARE_ID_CONTROLLER      "CONTROLLER"
#define FIRMWARE_ID_BOOTLOADER      "BOOTLOADER"
#define FIRMWARE_ID_BLACKBOX        "BLACKBOX"

#define FIRMWARE_ID        FIRMWARE_ID_BOOTLOADER

/*
 * CPU ID
 *
 * TMS320F28335
 * TMS320F28377D
 */

#define CPU_ID_28335                "TMS320F28335"
#define CPU_ID_28377D               "TMS320F28377D"

#define CPU_ID                      CPU_ID_28377D

/*
 * BOARD ID
 *
 * CURRENT_CONTROL
 * VOLTAGE_CONTROL
 */

#define BOARD_ID_0                  "DSP377D-SP-VER3.3"

#define BOARD_ID                    BOARD_ID_0



#endif // __VERSION_H
