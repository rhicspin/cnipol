
#include "Kinema.h"

// Class name  : 
// Method name : float ekin(float edep, float dthick)
//
// Description : Function to convert energy deposit (edep) to 12C kinetic 
//             : energy for given deadlayer thickness (dthick)
// Input       : float edep, float dthick
// Return      : Ekin = p0 + p1*Edep + p2*Edep^2 + p3*Edep^3 + p4*Edep^4
//
float ekin(float edep, float dthick)
{
    const float cp0[4] = {-0.5174     , 0.4172      , 0.3610E-02  , -0.1286E-05};
    const float cp1[4] = {1.0000      , 0.8703E-02  , 0.1252E-04  , 0.6948E-07};
    const float cp2[4] = {0.2990E-05  , -0.7937E-05 , -0.2219E-07 , -0.2877E-09};
    const float cp3[4] = {-0.8258E-08 , 0.4031E-08  , 0.9673E-12  , 0.3661E-12};
    const float cp4[4] = {0.3652E-11  , -0.8652E-12 , 0.4059E-14  , -0.1294E-15};
    
    float d2 = dthick * dthick;
    float d3 = d2 * dthick;

    float pp[5];
    
    pp[0] = cp0[0] + cp0[1]*dthick + cp0[2]*d2 + cp0[3]*d3;
    pp[1] = cp1[0] + cp1[1]*dthick + cp1[2]*d2 + cp1[3]*d3;
    pp[2] = cp2[0] + cp2[1]*dthick + cp2[2]*d2 + cp2[3]*d3;
    pp[3] = cp3[0] + cp3[1]*dthick + cp3[2]*d2 + cp3[3]*d3;
    pp[4] = cp4[0] + cp4[1]*dthick + cp4[2]*d2 + cp4[3]*d3;
    
    float edep2 = edep * edep;
    float edep3 = edep2 * edep;
    float edep4 = edep3 * edep;

    return pp[0] + pp[1]*edep + pp[2]*edep2 + pp[3]*edep3 + pp[4]*edep4;
}
