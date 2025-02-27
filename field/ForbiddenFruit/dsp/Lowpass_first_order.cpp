#include "Lowpass_first_order.h"

#define twopi 6.28318530718f

void lowpass_first_order_init(lowpass_first_order *filt, float fc, float fs){

    lowpass_first_order_set_samplerate(filt, fs);
    lowpass_first_order_set_cutoff(filt, fc);

    //Reset output during initialisation
    filt->out = 0.0f;

}

//Compute and store filter coefficients -> this is important to save resources performing the differential equation calculations
void store_coefficients(lowpass_first_order *filt, float fc){
    float alpha = twopi * fc / filt->fs;  //alpha = 2 * pi * fc / fs

    filt->coeff[0] = alpha / (1.0f + alpha);    //alpha / (1 + alpha)
    filt->coeff[1] = 1.0f / (1.0f + alpha);     //1 / (1 + alpha)
}

void lowpass_first_order_set_samplerate(lowpass_first_order *filt, float fs){
    if(fs > 50){
        filt->fs = fs;
    }
    else{
        fs = 50;
        filt->fs = fs;
    }

}

void lowpass_first_order_set_cutoff(lowpass_first_order *filt, float fc){
    //Clamp cutoff freq to stay in bounds (below nyquist limit / positive)
    if(fc > (0.5 * filt->fs))
    {
        fc = (0.5 * filt->fs);
    }
    if(fc < 0.0f){
        fc = 0.0f;
    }
}

float lowpass_first_order_update(lowpass_first_order *filt, float inp){
    //IIR filter update to get new output sample
    //Vout[n] = alpha / (1 + alpha) * Vin[n] + 1 / (1 + alpha) * Vout[n - 1]
    filt->out = filt->coeff[0] * inp + filt->coeff[1] * filt->out;

    //clamp output in case
    if(filt->out > 1.0f){
        filt->out = 1.0f;
    }else if(filt->out < -1.0f){
        filt->out = -1.0f;
    }

    //return filter outout
    return filt->out;
}