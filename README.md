# DigiFilter
Audio Processor

The DigiFilter is an in-line audio DSP module which features a filter with adjustable stop bands (0-4kHz), an NLMS Denoiser, Autonotch and Binaural operation. It
is intended as an add on accessory for radio transceivers/receivers to improve reception. The DigiFilter hardware and software presented here is for personal, 
non-commercial use as described in the Creative Commons License. See the Licence folder for a copy of the licence.

PW Readers!

Several late changes have been made to the software which could not be included in the PW article. These are summarised below;

Denoiser. Press and release the encoder to toggle the Denoiser on and off. With the Denoiser enabled, rotating the encoder steps through 5 levels of denoising. 
To exit Denoiser adjustment whilst leaving the Denoiser active, press and hold the encoder until the Morse "D" Denoiser menu announcement is heard. Rotating the 
encoder now allows navigation at menu level. With the Denoiser toggled off, normal menu navigation is active.

Binaural. The Binaural mode no longer supports an adjustable delay, this was found to offer little benefit and caused confusion when trying to use the menu 
system. Binaural mode is now toggled on or off by the encoder press/release operation. With Binaural active, rotating the encoder adjusts left and right channel
balance. Accordingly, menu options "T" and "Q" are no longer used. To exit Binaural mode leaving it active, press and hold the encoder until the Morse "B" Binaural 
menu announcement is heard. With Binaural toggled off, turning the encoder resumes normal menu navigation.

Menu Navigation. With the exception of the Filter Upper and Lower corner frequency adjsutments, the new menu structure is flat hence the menu "M" announcement is
no longer in use. In any menu, irrespective of that menu item's operation, a press and hold will return to navigation from current menu option which will be 
announced in Morse.

New Hardware Options. During use it was found that in some function and indication of whether or not that function was active would have been useful. This was
particularly the case for the Autonotch, which when engaged does not colour the audio and with no indication that it is enabled, could present some confusion.
Three additional outputs are now available on spare processor pins whose state goes high for a function being engaged and low for disengaged. These pins may
be used to drive low current indicator LEDs via suitable resistors, (around 220R -330R). The pins are as follows;

Denoiser State    Pin 14 RB5

Autonotch State   Pin 11 RB4

Binaural State    Pin 7  RB3

If you use these be sure not to cause solder bridges with adjacent pins!

Morse Announcements. The morse announcements used to indicate menu position and certain actions are described below.

F (di di dah dit) Filter – Press/Rel to enter Upper Passband Mode, Press/Hold to return to menu navigation.
  U (di di dah) Upper Passband Mode (Press/Rel for lower)
  L (di dah di dit) Lower Passband Mode (Press/Rel for upper)

A (di dah) Autonotch – Press/Rel to toggle on/off.  No adjustment, turn encoder to change menu.

D (dah di dit) Denoiser – Press/Rel to toggle on/off.  Turn to change denoiser levels. Press/Hold to exit to menu leaving Denoiser enabled.

B (dah di di dit) Binaural – Press/Rel to toggle on/off, turn for balance. Press/Hold to exit to menu leaving Binaural enabled.

P (di dah dah dit) Save filter against selected preset. Upon save a confirmation "K" will be heard.

G (dah da dit) Tone Gain – Press/Rel to enter (Press/hold to save and exit)

E (dit)  Adjustment Step notification

S (di di dit) Limit reached, High pitch= upper, low pitch= lower

K (dah di dah) Preset storage success

Ron Taylor G4GXO








