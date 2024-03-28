/* ******************************************************************************************************************
 * DIRM_0, DIRM_1 (GPIO) Register                                                                                   *
 * Register Name:	DIRM_0,       DIRM_1                                                                            *
 * Relative Address	0x0000000204, 0x0000000244                                                                      *
 * Absolute Address	0x00FF0A0204, 0x00FF0A0244 (GPIO)                                                               *
 * Width	32                                                                                                      *
 * Type	mixed                                                                                                       *
 * Reset Value	0x00000000                                                                                          *
 * Description	Direction mode (GPIO Bank0, MIO)                                                                    *
 * This register controls whether the IO pin is acting as an input or an output.                                    *
 * Since the input logic is always enabled, this effectively enables/disables the output driver.                    *
 * Each bit of the bank is independently controlled. This register controls bank0, which corresponds to MIO[25:0].  *
 *                                                                                                                  *
 * Direction mode                                                                                                   *
 * 0: input                                                                                                         *
 * 1: output                                                                                                        *
 * Each bit configures the corresponding pin within the 26-bit bank                                                 *
 *                                                                                                                  *
 * OEN_0, OEN_1 (GPIO) Register Description:                                                                        *  
 * Register Name:	OEN_0,        OEN_1                                                                             *
 * Relative Address	0x0000000208, 0x0000000248                                                                      *
 * Absolute Address	0x00FF0A0208, 0x00FF0A0208 (GPIO)                                                               *
 * Width	32                                                                                                      *
 * Type	mixed                                                                                                       *
 * Reset Value	0x00000000                                                                                          *
 * Description	Output enable (GPIO Bank0, MIO)                                                                     *
 * When the IO is configured as an output, this controls whether the output is enabled or not. When the output is   *
 * disabled, the pin is tri-stated. NOTE: The MIO driver setting MIO_MST_TRIx (IOU_SLCR) must be disabled           *
 * (i.e. set to 0) for this field to be operational. When the MIO tri-state is enabled, the driver is disabled      *
 * regardless of this GPIO setting. This register controls bank0, which corresponds to MIO[25:0].                   *  
 *                                                                                                                  *
 * Output enables                                                                                                   *
 * 0: disabled                                                                                                      *
 * 1: enabled                                                                                                       *
 * Each bit configures the corresponding pin within the 26-bit bank                                                 *
 *                                                                                                                  *
 * DATA_0, DATA_1 (GPIO) Register Description:                                                                      *
 * Register Name	DATA_0,       DATA_1                                                                            *
 * Relative Address	0x0000000040, 0x0000000044                                                                      *
 * Absolute Address	0x00FF0A0040, 0x00FF0A0044 (GPIO)                                                               *
 * Width	32                                                                                                      *
 * Type	mixed                                                                                                       *
 * Reset Value	0x00000000                                                                                          *
 * Description	Output Data (GPIO Bank0, MIO)                                                                       *
 * This register controls the value being output when the GPIO signal is configured as an output. All 32bits        *
 * of this register are written at one time. Reading from this register returns the previous value written to       *
 * either DATA or MASK_DATA_{LSW,MSW}; it does not return the value on the device pin. NOTE: This register does     *
 * not affect the enabling of the output driver. See the DIRM_0 and OEN_0 registers. This register controls the     *
 * output values for bank0, which corresponds to MIO[25:0].                                                         *  
 * ******************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <byteswap.h>

#include "mmio.h"

#define GPIO_BASE 0xFF0A0000
#define GPIO_SIZE 0x00010000 // 64K bytes

#define DIRM_0 0x00000204
#define OEN_0  0x00000208
#define DIRM_1 0x00000244
#define OEN_1  0x00000248
#define DATA_0 0x00000040
#define DATA_1 0x00000044

/* GPIO Initialization */
int gpio_init(mmio_t *mmio, uint verbos) {
    uint32_t dirm0_data = 0x03FFFFFF;
    uint32_t oen0_data  = 0x03FFFFFF;
    uint32_t dirm1_data = 0x03FFFFFF;
    uint32_t oen1_data  = 0x03FFFFFF;
    uint32_t data0_data = 0x00010000;
    uint32_t data1_data = 0x000018CC;

    /* Open MMIO */
    if (mmio_open(mmio, GPIO_BASE, GPIO_SIZE) < 0) {
        fprintf(stderr, "mmio_open(): %s\n", mmio_errmsg(mmio));
        exit(1);
    }
    
    // WRITE ======================================================
    if(verbos == 2) printf("\nGPIO Initialization...\n");

    /* Write DIRM_0 */
    if (mmio_write32(mmio, DIRM_0, dirm0_data) < 0) {
        fprintf(stderr, "DIRM_0 mmio_write32(): %s\n", mmio_errmsg(mmio));
        exit(1);
    }    
    /* Write OEN_0 */
    if (mmio_write32(mmio, OEN_0, oen0_data) < 0) {
        fprintf(stderr, "OEN_0 mmio_write32(): %s\n", mmio_errmsg(mmio));
        exit(1);
    }    
    /* Write DIRM_1 */
    if (mmio_write32(mmio, DIRM_1, dirm1_data) < 0) {
        fprintf(stderr, "DIRM_1 mmio_write32(): %s\n", mmio_errmsg(mmio));
        exit(1);
    }    
    /* Write OEN_1 */
    if (mmio_write32(mmio, OEN_1, oen1_data) < 0) {
        fprintf(stderr, "OEN_1 mmio_write32(): %s\n", mmio_errmsg(mmio));
        exit(1);
    }    
    /* Write DATA_0 */
    if (mmio_write32(mmio, DATA_0, data0_data) < 0) {
        fprintf(stderr, "DATA_0 mmio_write32(): %s\n", mmio_errmsg(mmio));
        exit(1);
    }    
    /* Write DATA_1 */
    if (mmio_write32(mmio, DATA_1, data1_data) < 0) {
        fprintf(stderr, "DATA_1 mmio_write32(): %s\n", mmio_errmsg(mmio));
        exit(1);
    }    
    // ============================================================

    // READ =======================================================    
    /* Read DIRM_0 */
    if (mmio_read32(mmio, DIRM_0, &dirm0_data) < 0) {
        fprintf(stderr, "DIRM_0 mmio_read32(): %s\n", mmio_errmsg(mmio));
        exit(1);
    }   
    /* Read OEN_0 */
    if (mmio_read32(mmio, OEN_0, &oen0_data) < 0) {
        fprintf(stderr, "OEN_0 mmio_read32(): %s\n", mmio_errmsg(mmio));
        exit(1);
    }
    /* Read DIRM_1 */
    if (mmio_read32(mmio, DIRM_1, &dirm1_data) < 0) {
        fprintf(stderr, "DIRM_1 mmio_read32(): %s\n", mmio_errmsg(mmio));
        exit(1);
    }
    /* Read OEN_1 */
    if (mmio_read32(mmio, OEN_1, &oen1_data) < 0) {
        fprintf(stderr, "OEN_0 mmio_read32(): %s\n", mmio_errmsg(mmio));
        exit(1);
    }
    /* Read DATA_0 */
    if (mmio_read32(mmio, DATA_0, &data0_data) < 0) {
        fprintf(stderr, "DATA_0 mmio_read32(): %s\n", mmio_errmsg(mmio));
        exit(1);
    }
    /* Read DATA_1 */
    if (mmio_read32(mmio, DATA_1, &data1_data) < 0) {
        fprintf(stderr, "DATA_1 mmio_read32(): %s\n", mmio_errmsg(mmio));
        exit(1);
    }
    /* Print Read DATA */
    if(verbos == 2) {
        printf("\n NAME:  ADDRESS:    DATA:\n");
        printf("DIRM_0  0x%08X  0x%08X\n", GPIO_BASE+DIRM_0, dirm0_data);
        printf(" OEN_0  0x%08X  0x%08X\n", GPIO_BASE+OEN_0, oen0_data);
        printf("DIRM_1  0x%08X  0x%08X\n", GPIO_BASE+DIRM_1, dirm1_data);
        printf(" OEN_1  0x%08X  0x%08X\n", GPIO_BASE+OEN_1, oen1_data);
        printf("DATA_0  0x%08X  0x%08X\n", GPIO_BASE+DATA_0, data0_data);
        printf("DATA_1  0x%08X  0x%08X\n\n", GPIO_BASE+DATA_1, data1_data);
    }
    else if (verbos == 1) {
        printf("\nGPIO Initialized\n\n");
    }     
    // ============================================================

    return 0;
}
