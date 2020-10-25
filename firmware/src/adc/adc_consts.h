
#ifndef ADC_ADC_CONSTS_H_
#define ADC_ADC_CONSTS_H_

#ifdef CAFE_ARCH2
typedef enum {
#if defined(PAC5527)
    PWRCTL_PWRMON_VCORE0                = 0u << 3,                   // 0<<3 --> VCORE
    PWRCTL_PWRMON_VP                    = 1u << 3,                   // 1<<3 --> VP * 1/10
    PWRCTL_PWRMON_VCC33                 = 2u << 3,                   // 2<<3 --> VCC33 * 4/10
    PWRCTL_PWRMON_VCCIO                 = 3u << 3,                   // 3<<3 --> VCCIO * 4/10
    PWRCTL_PWRMON_VSYS                  = 4u << 3,                   // 4<<3 --> VSYS * 4/10
    PWRCTL_PWRMON_VREFDIV2              = 5u << 3,                   // 5<<3 --> VREF/2
    PWRCTL_PWRMON_VPTAT                 = 6u << 3,                   // 6<<3 --> VPTAT
    PWRCTL_PWRMON_VCP_MINUS_VM          = 7u << 3                    // 7<<3 --> (VCP-VM) * 5/10
#endif  // #if defined(PAC5527)
}ADC_PWRCTL_PWRMON_Type;

    // MODULE MISC CAFE REGISTER
    #define HIB_BIT_DEFAULT            0u								                                    //0 = Normal; 1 = Hibernate
    #define PBEN_BIT_DEFAULT           0u								                                    //0 = Push Button Disabled; 1 = Push Button Enabled (AIO6)
    #define VREFSET_BIT_DEFAULT        0u								                                    //0 = ADC VREF is 2.5V; 1 = ADC VREF is 3.0V
    #define CLKOUTEN_BIT_DEFAULT       0u								                                    //0 = Disabled; 1 = Enabled
    #define MCUALIVE_BIT_DEFAULT       1u								                                    //0 = Disabled; 1 = Enabled
    #define TPBD_BIT_DEFAULT           1u                                                                   //0 = Disabled; 1 = Enabled
    #define RFU_BIT_DEFAULT            0u                                                                   //0 = Disabled; 1 = Enabled
    #define ENSIG_BIT_DEFAULT          1u								                                    //0 = Disabled; 1 = Enabled
    #define MODULE_MISC_BITS_DEFAULT   ((HIB_BIT_DEFAULT << 7) + (PBEN_BIT_DEFAULT << 6) + (VREFSET_BIT_DEFAULT << 5) + (CLKOUTEN_BIT_DEFAULT << 4) + (MCUALIVE_BIT_DEFAULT << 3) + (TPBD_BIT_DEFAULT << 2) + (RFU_BIT_DEFAULT << 1) + (ENSIG_BIT_DEFAULT << 0))
#endif

// Resistor value of diffamp AIO RC filter
#define AIORES_VALUE                    100.0f

#define AIO0to5_DIFF_AMP_MODE           0x40u
#define AIO6789_IO_MODE                 0x00u
#define HP_DIS_LP_EN_PR1                0x20u

#define AIO_INPUT                       0x00u
#define AIO_OUTPUT                      0x20u

#define AIO_ACTIVE_H                    0x00u
#define AIO_ACTIVE_L                    0x08u

#define OFFSET_EN                       0x08u
#define OFFSET_DIS                      0x00u

#define CAL_OFFSET_EN                   0x04u
#define CAL_OFFSET_DIS                  0x00u

#define GAINx1                          (0x01u << 3)
#define GAINx2                          (0x03u << 3)
#define GAINx4                          (0x01u << 3)
#define GAINx8                          (0x04u << 3)
#define GAINx16                         (0x05u << 3)

#define LP_HP_EN_1us                    0x01u

#define LP54_EN                         0x04u
#define LP32_EN                         0x02u
#define LP10_EN                         0x01u

#define LP_PROT                         (LP54_EN | LP32_EN | LP10_EN)

#define LSPR1EN                         0x20u

// Digital_MUX_Setting (DMUX_DOUTx_Default)
#define DMUX_DOUTx_Default              0

#endif /* ADC_ADC_CONSTS_H_ */