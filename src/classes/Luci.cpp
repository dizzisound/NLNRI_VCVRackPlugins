#pragma once
#include "dsp/digital.hpp"
namespace rack {

//  Base class for Luci

struct luciCell {

// Luci cell has its own frequency, externally adjustable.
// When signal reaches +1, cell bangs her trigger out simmultaneously on her four out ports.
// She gets 4 incoming triggers from neighbouring cells. 
// The incoming triggers modify the ramp by influence amount.
	
	// luci variable
	float frequency = .5f;	// in HZ
	float f0 = .1277468f; // 1volt/octave C-6 = 0, transposed -6 octaves...
	
	float influence = 0.f;
	// integration
	float delta = frequency / engineGetSampleRate();
	float phase = 0.f;
	// this is for the standard vcv trigger
	PulseGenerator trigOut;
	float trigOutValue = 0.f;
	float triggerLength = 10e-3;
	// this is for the intrinsic one sample Luci trigger (to feed other cells)
	bool luciTrigger = false;
	
	void setFrequency( float pitchVoltage ){
		// from CV to Hz
		frequency = f0 * powf(2.f, pitchVoltage);
	}
	
	void setInfluence( float _influence){
		// numerical ranges as stated on widget
		influence =  clampf( _influence, 1.f, 3.50124f);
	}
	
	void setTrigInN( bool _trig1){
		// advance phase if trigger is present
		if( _trig1){ phase *= influence; } 
	}
	
	void setTrigInE( bool _trig2){
		// advance phase if trigger is present
		if( _trig2){ phase *= influence; }
	}
	
	void setTrigInS( bool _trig3){
		// advance phase if trigger is present
		if( _trig3){ phase *= influence; }
	}
	
	void setTrigInW( bool _trig4){
		// advance phase if trigger is present
		if( _trig4){ phase *= influence; }
	}
	
	void randomize( bool _rnd_trig){
		if( _rnd_trig ){
			phase = (rand() % 100) / 100.f;
			}
	}
	
	void resetLuci (){
		// shoot triggers out
		trigOut.trigger(triggerLength);
		luciTrigger = true;
		// reset phase 
		phase = 0.f;
	}
	
	float process(){
		// find increment per sample
		delta = frequency / engineGetSampleRate();
		// prepare trigOut signal
		trigOutValue = 10.f * (float) trigOut.process(delta);
		// restart cycle when reaching 1.f
		if( phase >= 1.f){
			this->resetLuci();
		} else {
			luciTrigger = false;
		}
		// integrate
		phase += delta;
		// process function returns an audio signal
		return( ( 10.f * phase ) - 5.f );
	}
	
	// extract trigger values
	float getTrigger (){
		return( trigOutValue );
	}
	
	bool getLuciTrigger (){
		return (luciTrigger);
	}
};

}; // namespace rack