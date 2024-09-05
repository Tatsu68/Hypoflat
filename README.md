Give a boost to your sound with this spectral shaper!
The algorithm's quite simple: Sample the average logarithm loudness on the freq domain, and apply a gaussian filter to smooth the curve. Then the original signal is interpolated to the dest signal in the freq domain.

# Usage
* Strength: The main parameter.
* FFT Order: Set the fft size.
* Pink: Add some tilt to the frequcency domain. This makes the sound more suitable for bus mixing.
* AGC: Compensate the loudness to match the original RMS of the window. Set to full by default.
* Soften: Make the sound softer. This is useful when processing something acoustic.

# TODO
* I've built this on my M2 mac, but there are some issues loading it on x86 machines. So win-only right now. You can try building it on your mac yourself.

# Licnese
This plugin applies to the GNU Affero General Public License (AGPL) v3.0.
## JUCE
https://github.com/juce-framework/JUCE
## kiss-fft
https://github.com/mborgerding/kissfft