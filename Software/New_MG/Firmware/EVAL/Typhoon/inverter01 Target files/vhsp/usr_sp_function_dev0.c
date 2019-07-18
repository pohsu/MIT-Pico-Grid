// generated using template: cop_main.template---------------------------------------------
/******************************************************************************************
**
**  Module Name: cop_main.c
**  NOTE: Automatically generated file. DO NOT MODIFY!
**  Description:
**            Main file
**
******************************************************************************************/
// generated using template: arm/custom_include.template-----------------------------------

// x86 libraries:
#include "../include/sp_functions_dev0.h"
// ----------------------------------------------------------------------------------------
// generated using template: VirtualHIL/custom_defines.template----------------------------

typedef unsigned char X_UnInt8;
typedef int X_Int32;
typedef unsigned int X_UnInt32;
typedef unsigned int uint;
typedef double real;

// ----------------------------------------------------------------------------------------
// generated using template: common_consts.template----------------------------------------200};

// arithmetic constants
#define C_SQRT_2                    1.4142135623730950488016887242097f
#define C_SQRT_3                    1.7320508075688772935274463415059f
#define C_PI                        3.1415926535897932384626433832795f
#define C_E                         2.7182818284590452353602874713527f
#define C_2PI                       6.283185307179586476925286766559f

//@cmp.def.start
//component defines
//@cmp.def.end

//-----------------------------------------------------------------------------------------
// generated using template: common_variables.template-------------------------------------
// true global variables

//@cmp.var.start
// variables
double _3ph_inverter_2_inc_const__out = 1.0;
double _3ph_inverter_2_inb_const__out = 1.0;
double _3ph_inverter_2_ina_const__out = 1.0;
double _3ph_inverter_2_en_const__out = 1.0;
double _3ph_inverter_1_inc_const__out = 1.0;
double _3ph_inverter_1_inb_const__out = 1.0;
double _3ph_inverter_1_ina_const__out = 1.0;
double _3ph_inverter_1_en_const__out = 1.0;
unsigned int _3ph_inverter_1_pwm_modulator__channels[3] = {0, 1, 2};
double _3ph_inverter_1_pwm_modulator__limited_in[3];
unsigned int _3ph_inverter_1_pwm_modulator__update_mask;

unsigned int _3ph_inverter_2_pwm_modulator__channels[3] = {3, 4, 5};
double _3ph_inverter_2_pwm_modulator__limited_in[3];
unsigned int _3ph_inverter_2_pwm_modulator__update_mask;

//@cmp.var.end

//@cmp.svar.start
// state variables
//@cmp.svar.end
// generated using template: virtual_hil/custom_functions.template---------------------------------
void ReInit_user_sp_cpu_dev0() {

#if DEBUG_MODE
    printf("\n\rReInitTimer");
#endif

    //@cmp.init.block.start








    _3ph_inverter_1_pwm_modulator__update_mask = 7;
    HIL_OutInt32(HIL_PM_MAX_CNT + _3ph_inverter_1_pwm_modulator__channels[0], 2500);// divide by 2 is already implemented in hw
    HIL_OutInt32(HIL_PM_D_TIME + _3ph_inverter_1_pwm_modulator__channels[0], 0);
    HIL_OutInt32(HIL_PM_MAX_CNT + _3ph_inverter_1_pwm_modulator__channels[1], 2500);// divide by 2 is already implemented in hw
    HIL_OutInt32(HIL_PM_D_TIME + _3ph_inverter_1_pwm_modulator__channels[1], 0);
    HIL_OutInt32(HIL_PM_MAX_CNT + _3ph_inverter_1_pwm_modulator__channels[2], 2500);// divide by 2 is already implemented in hw
    HIL_OutInt32(HIL_PM_D_TIME + _3ph_inverter_1_pwm_modulator__channels[2], 0);
    HIL_OutInt32(HIL_PM_CARR_PHASE_OFF + _3ph_inverter_1_pwm_modulator__channels[0], 0);
    HIL_OutInt32(HIL_PM_CARR_SIG_DIR + _3ph_inverter_1_pwm_modulator__channels[0], 0);
    HIL_OutInt32(HIL_PM_CARR_PHASE_OFF + _3ph_inverter_1_pwm_modulator__channels[1], 0);
    HIL_OutInt32(HIL_PM_CARR_SIG_DIR + _3ph_inverter_1_pwm_modulator__channels[1], 0);
    HIL_OutInt32(HIL_PM_CARR_PHASE_OFF + _3ph_inverter_1_pwm_modulator__channels[2], 0);
    HIL_OutInt32(HIL_PM_CARR_SIG_DIR + _3ph_inverter_1_pwm_modulator__channels[2], 0);
    HIL_OutInt32(HIL_PM_USE_DI + _3ph_inverter_1_pwm_modulator__channels[0], 0);
    HIL_OutInt32(HIL_PM_USE_DI + _3ph_inverter_1_pwm_modulator__channels[0], 1);
    HIL_OutInt32(HIL_PM_DI_ADDR + _3ph_inverter_1_pwm_modulator__channels[0], 0);
    HIL_OutInt32(HIL_PM_USE_DI + _3ph_inverter_1_pwm_modulator__channels[1], 0);
    HIL_OutInt32(HIL_PM_USE_DI + _3ph_inverter_1_pwm_modulator__channels[1], 1);
    HIL_OutInt32(HIL_PM_DI_ADDR + _3ph_inverter_1_pwm_modulator__channels[1], 1);
    HIL_OutInt32(HIL_PM_USE_DI + _3ph_inverter_1_pwm_modulator__channels[2], 0);
    HIL_OutInt32(HIL_PM_USE_DI + _3ph_inverter_1_pwm_modulator__channels[2], 1);
    HIL_OutInt32(HIL_PM_DI_ADDR + _3ph_inverter_1_pwm_modulator__channels[2], 2);
    HIL_OutInt32(HIL_PM_MASK, _3ph_inverter_1_pwm_modulator__update_mask);


    _3ph_inverter_2_pwm_modulator__update_mask = 56;
    HIL_OutInt32(HIL_PM_MAX_CNT + _3ph_inverter_2_pwm_modulator__channels[0], 2500);// divide by 2 is already implemented in hw
    HIL_OutInt32(HIL_PM_D_TIME + _3ph_inverter_2_pwm_modulator__channels[0], 0);
    HIL_OutInt32(HIL_PM_MAX_CNT + _3ph_inverter_2_pwm_modulator__channels[1], 2500);// divide by 2 is already implemented in hw
    HIL_OutInt32(HIL_PM_D_TIME + _3ph_inverter_2_pwm_modulator__channels[1], 0);
    HIL_OutInt32(HIL_PM_MAX_CNT + _3ph_inverter_2_pwm_modulator__channels[2], 2500);// divide by 2 is already implemented in hw
    HIL_OutInt32(HIL_PM_D_TIME + _3ph_inverter_2_pwm_modulator__channels[2], 0);
    HIL_OutInt32(HIL_PM_CARR_PHASE_OFF + _3ph_inverter_2_pwm_modulator__channels[0], 0);
    HIL_OutInt32(HIL_PM_CARR_SIG_DIR + _3ph_inverter_2_pwm_modulator__channels[0], 0);
    HIL_OutInt32(HIL_PM_CARR_PHASE_OFF + _3ph_inverter_2_pwm_modulator__channels[1], 0);
    HIL_OutInt32(HIL_PM_CARR_SIG_DIR + _3ph_inverter_2_pwm_modulator__channels[1], 0);
    HIL_OutInt32(HIL_PM_CARR_PHASE_OFF + _3ph_inverter_2_pwm_modulator__channels[2], 0);
    HIL_OutInt32(HIL_PM_CARR_SIG_DIR + _3ph_inverter_2_pwm_modulator__channels[2], 0);
    HIL_OutInt32(HIL_PM_USE_DI + _3ph_inverter_2_pwm_modulator__channels[0], 0);
    HIL_OutInt32(HIL_PM_USE_DI + _3ph_inverter_2_pwm_modulator__channels[0], 1);
    HIL_OutInt32(HIL_PM_DI_ADDR + _3ph_inverter_2_pwm_modulator__channels[0], 3);
    HIL_OutInt32(HIL_PM_USE_DI + _3ph_inverter_2_pwm_modulator__channels[1], 0);
    HIL_OutInt32(HIL_PM_USE_DI + _3ph_inverter_2_pwm_modulator__channels[1], 1);
    HIL_OutInt32(HIL_PM_DI_ADDR + _3ph_inverter_2_pwm_modulator__channels[1], 4);
    HIL_OutInt32(HIL_PM_USE_DI + _3ph_inverter_2_pwm_modulator__channels[2], 0);
    HIL_OutInt32(HIL_PM_USE_DI + _3ph_inverter_2_pwm_modulator__channels[2], 1);
    HIL_OutInt32(HIL_PM_DI_ADDR + _3ph_inverter_2_pwm_modulator__channels[2], 5);
    HIL_OutInt32(HIL_PM_MASK, _3ph_inverter_2_pwm_modulator__update_mask);


    //@cmp.init.block.end
}
// generated using template: common_timer_counter_handler.template-------------------------

/*****************************************************************************************/
/**
* This function is the handler which performs processing for the timer counter.
* It is called from an interrupt context such that the amount of processing
* performed should be minimized.  It is called when the timer counter expires
* if interrupts are enabled.
*
*
* @param    None
*
* @return   None
*
* @note     None
*
*****************************************************************************************/

void TimerCounterHandler_0_user_sp_cpu_dev0() {

#if DEBUG_MODE
    printf("\n\rTimerCounterHandler_0");
#endif

    //////////////////////////////////////////////////////////////////////////
    // Output block
    //////////////////////////////////////////////////////////////////////////
    //@cmp.out.block.start
    // Generated from the component: 3ph_inverter_2.InC_const

    // Generated from the component: 3ph_inverter_2.InB_const

    // Generated from the component: 3ph_inverter_2.InA_const

    // Generated from the component: 3ph_inverter_2.En_const

    // Generated from the component: 3ph_inverter_1.InC_const

    // Generated from the component: 3ph_inverter_1.InB_const

    // Generated from the component: 3ph_inverter_1.InA_const

    // Generated from the component: 3ph_inverter_1.En_const

    // Generated from the component: 3ph_inverter_1.PWM_Modulator
    if (_3ph_inverter_1_en_const__out == 0x0) {
        HIL_OutInt32(HIL_PM_EN + _3ph_inverter_1_pwm_modulator__channels[0], 0x0);
    }
    else {
        HIL_OutInt32(HIL_PM_EN + _3ph_inverter_1_pwm_modulator__channels[0], 0x1);
    }
    if (_3ph_inverter_1_en_const__out == 0x0) {
        HIL_OutInt32(HIL_PM_EN + _3ph_inverter_1_pwm_modulator__channels[1], 0x0);
    }
    else {
        HIL_OutInt32(HIL_PM_EN + _3ph_inverter_1_pwm_modulator__channels[1], 0x1);
    }
    if (_3ph_inverter_1_en_const__out == 0x0) {
        HIL_OutInt32(HIL_PM_EN + _3ph_inverter_1_pwm_modulator__channels[2], 0x0);
    }
    else {
        HIL_OutInt32(HIL_PM_EN + _3ph_inverter_1_pwm_modulator__channels[2], 0x1);
    }

    HIL_OutInt32(HIL_PM_EN_UPDATE, 0);

    if (_3ph_inverter_1_ina_const__out < -1.0)
        _3ph_inverter_1_pwm_modulator__limited_in[0] = -1.0;
    else if (_3ph_inverter_1_ina_const__out > 1.0)
        _3ph_inverter_1_pwm_modulator__limited_in[0] = 1.0;
    else
        _3ph_inverter_1_pwm_modulator__limited_in[0] = _3ph_inverter_1_ina_const__out;
    HIL_OutInt32(HIL_PM_REF_SIG + _3ph_inverter_1_pwm_modulator__channels[0], ((unsigned)((_3ph_inverter_1_pwm_modulator__limited_in[0] - (-1.0)) * 1250.0)));
    if (_3ph_inverter_1_inb_const__out < -1.0)
        _3ph_inverter_1_pwm_modulator__limited_in[1] = -1.0;
    else if (_3ph_inverter_1_inb_const__out > 1.0)
        _3ph_inverter_1_pwm_modulator__limited_in[1] = 1.0;
    else
        _3ph_inverter_1_pwm_modulator__limited_in[1] = _3ph_inverter_1_inb_const__out;
    HIL_OutInt32(HIL_PM_REF_SIG + _3ph_inverter_1_pwm_modulator__channels[1], ((unsigned)((_3ph_inverter_1_pwm_modulator__limited_in[1] - (-1.0)) * 1250.0)));
    if (_3ph_inverter_1_inc_const__out < -1.0)
        _3ph_inverter_1_pwm_modulator__limited_in[2] = -1.0;
    else if (_3ph_inverter_1_inc_const__out > 1.0)
        _3ph_inverter_1_pwm_modulator__limited_in[2] = 1.0;
    else
        _3ph_inverter_1_pwm_modulator__limited_in[2] = _3ph_inverter_1_inc_const__out;
    HIL_OutInt32(HIL_PM_REF_SIG + _3ph_inverter_1_pwm_modulator__channels[2], ((unsigned)((_3ph_inverter_1_pwm_modulator__limited_in[2] - (-1.0)) * 1250.0)));
    HIL_OutInt32(HIL_PM_MASK, _3ph_inverter_1_pwm_modulator__update_mask);

    // Generated from the component: 3ph_inverter_2.PWM_Modulator
    if (_3ph_inverter_2_en_const__out == 0x0) {
        HIL_OutInt32(HIL_PM_EN + _3ph_inverter_2_pwm_modulator__channels[0], 0x0);
    }
    else {
        HIL_OutInt32(HIL_PM_EN + _3ph_inverter_2_pwm_modulator__channels[0], 0x1);
    }
    if (_3ph_inverter_2_en_const__out == 0x0) {
        HIL_OutInt32(HIL_PM_EN + _3ph_inverter_2_pwm_modulator__channels[1], 0x0);
    }
    else {
        HIL_OutInt32(HIL_PM_EN + _3ph_inverter_2_pwm_modulator__channels[1], 0x1);
    }
    if (_3ph_inverter_2_en_const__out == 0x0) {
        HIL_OutInt32(HIL_PM_EN + _3ph_inverter_2_pwm_modulator__channels[2], 0x0);
    }
    else {
        HIL_OutInt32(HIL_PM_EN + _3ph_inverter_2_pwm_modulator__channels[2], 0x1);
    }

    HIL_OutInt32(HIL_PM_EN_UPDATE, 0);

    if (_3ph_inverter_2_ina_const__out < -1.0)
        _3ph_inverter_2_pwm_modulator__limited_in[0] = -1.0;
    else if (_3ph_inverter_2_ina_const__out > 1.0)
        _3ph_inverter_2_pwm_modulator__limited_in[0] = 1.0;
    else
        _3ph_inverter_2_pwm_modulator__limited_in[0] = _3ph_inverter_2_ina_const__out;
    HIL_OutInt32(HIL_PM_REF_SIG + _3ph_inverter_2_pwm_modulator__channels[0], ((unsigned)((_3ph_inverter_2_pwm_modulator__limited_in[0] - (-1.0)) * 1250.0)));
    if (_3ph_inverter_2_inb_const__out < -1.0)
        _3ph_inverter_2_pwm_modulator__limited_in[1] = -1.0;
    else if (_3ph_inverter_2_inb_const__out > 1.0)
        _3ph_inverter_2_pwm_modulator__limited_in[1] = 1.0;
    else
        _3ph_inverter_2_pwm_modulator__limited_in[1] = _3ph_inverter_2_inb_const__out;
    HIL_OutInt32(HIL_PM_REF_SIG + _3ph_inverter_2_pwm_modulator__channels[1], ((unsigned)((_3ph_inverter_2_pwm_modulator__limited_in[1] - (-1.0)) * 1250.0)));
    if (_3ph_inverter_2_inc_const__out < -1.0)
        _3ph_inverter_2_pwm_modulator__limited_in[2] = -1.0;
    else if (_3ph_inverter_2_inc_const__out > 1.0)
        _3ph_inverter_2_pwm_modulator__limited_in[2] = 1.0;
    else
        _3ph_inverter_2_pwm_modulator__limited_in[2] = _3ph_inverter_2_inc_const__out;
    HIL_OutInt32(HIL_PM_REF_SIG + _3ph_inverter_2_pwm_modulator__channels[2], ((unsigned)((_3ph_inverter_2_pwm_modulator__limited_in[2] - (-1.0)) * 1250.0)));
    HIL_OutInt32(HIL_PM_MASK, _3ph_inverter_2_pwm_modulator__update_mask);

    //@cmp.out.block.end


    //////////////////////////////////////////////////////////////////////////
    // Update block
    //////////////////////////////////////////////////////////////////////////
    //@cmp.update.block.start
    //@cmp.update.block.end
}
// ----------------------------------------------------------------------------------------  //-----------------------------------------------------------------------------------------