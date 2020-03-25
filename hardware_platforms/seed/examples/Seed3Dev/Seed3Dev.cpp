// # Seed3Dev
// ## Description
// Dev Playground for Seed3
//
#include <stdio.h>
#include <string.h>
//#include "daisy_seed.h"
//#include "daisy_pod.h"
#include "daisy_petal.h"
#include "daisysp.h"
using namespace daisy;

// Globals
DaisyPetal          hw;
daisysp::Oscillator DSY_SDRAM_BSS osc;
uint8_t                           wave;

float amp, targetamp;

void AudioTest(float *in, float *out, size_t size)
{
    hw.UpdateAnalogControls();
    hw.DebounceControls();
    float sig, note;
    // One way to get value
    note = hw.GetKnobValue(DaisyPetal::KNOB_6) * 127.0f;
    osc.SetFreq(daisysp::mtof(note));

    // Handle Encoder for waveform switching test.
    int32_t inc;
    inc = hw.encoder.Increment();
    if(inc > 0)
    {
        wave = (wave + 1) % daisysp::Oscillator::WAVE_LAST;
        osc.SetWaveform(wave);
    }
    else if(inc < 0)
    {
        wave = (wave + daisysp::Oscillator::WAVE_LAST - 1)
               % daisysp::Oscillator::WAVE_LAST;
        osc.SetWaveform(wave);
    }

    // Turn amplitude on when pressing button.
    targetamp = (hw.encoder.Pressed() || hw.switches[DaisyPetal::SW_5].Pressed()) ? 1.0f : 0.0f;

    for(size_t i = 0; i < size; i += 2)

    {
        daisysp::fonepole(amp, targetamp, 0.005f);
        osc.SetAmp(amp);
        if(hw.switches[DaisyPetal::SW_6].Pressed())
        {
            sig    = osc.Process();
            out[i] = out[i + 1] = sig;
        }
        else
        {
            out[i] = in[i];
            out[i + 1] = in[i + 1];
        }
    }
}

int main(void)
{
    float samplerate;
    hw.Init();
    samplerate = hw.AudioSampleRate();
    // Init Synthesis
    osc.Init(samplerate);
    // Start
    hw.StartAdc();
    hw.StartAudio(AudioTest);
    hw.ClearLeds();
    char outbuff[64];
    size_t size;
	sprintf(outbuff, "Daisy USB Testing...\r\n");
	size = strlen(outbuff);
    for(;;)
    {
        hw.DelayMs(1000);
        hw.seed.usb_handle.TransmitExternal((uint8_t*)(outbuff), size);
    }
}
