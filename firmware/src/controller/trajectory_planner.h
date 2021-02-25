
#include "src/common.h"

typedef struct
{
    // NOTE: The members of this struct are redundant,
    // i.e. not all are necessary to fully define a
    // trajectory. However this definition reduces the
    // computation during trajectory evaluation.
    float p_target;
    float deltat_acc;
    float t_acc_cruise;
    float deltat_cruise;
    float t_cruise_dec;
    float deltat_dec;
    float t_end;
    float v_0;
    float v_cruise;
    float v_target;
    float acc;
    float dec;
    float p_acc_cruise;
    float p_cruise_dec;
} MotionPlan;

bool planner_move_to(float p_target, float deltat_tot, float deltat_acc, float deltat_dec);
bool planner_prepare_plan(float p_target, float deltat_tot, float deltat_acc, float deltat_dec, MotionPlan *plan);
bool planner_evaluate(float t, MotionPlan *plan, float *pos, float *vel);
