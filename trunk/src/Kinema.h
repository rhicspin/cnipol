// Header file of KINEMATIC FUNCTION
// Author   : Itaru Nakagawa
// Creation : 07/26/2006         

#ifndef KINEMA_H
#define KINEMA_H

// Class name  : 
// Method name : float ekin(float edep, float dthick)
//
// Description : Function to convert energy deposit (edep) to 12C kinetic 
//             : energy for given deadlayer thickness (dthick)
// Input       : float edep, float dthick
// Return      : Ekin = p0 + p1*Edep + p2*Edep^2 + p3*Edep^3 + p4*Edep^4
//
float ekin(float edep, float dthick);


#endif /* KINEMA_H */
