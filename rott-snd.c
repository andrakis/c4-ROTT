// rott-snd.c -- the game's sound interfaces, Apogee's FX_ (effects) and
// MUSIC_ (MIDI), in place of icculus's fx_man.c and dukemusc.c, which put
// them on SDL_mixer. For now there is no card: FX_Init and MUSIC_Init say
// so, which the game takes as the DOS original took a machine without one,
// and the rest answer as an idle card would. libjs's sound card is next:
// the effects on its sample channels, the music through Apogee's own AdLib
// MIDI driver (audiolib's al_midi.c) on its OPL.

#include "rt_def.h"
#include "fx_man.h"
#include "music.h"

static char *nocard = "no sound card";

// ---- effects ----
char *FX_ErrorString(int ErrorNumber) { (void) ErrorNumber; return nocard; }
int  FX_SetupCard(int SoundCard, fx_device *device) { (void) SoundCard; (void) device; return FX_Error; }
int  FX_Init(int SoundCard, int numvoices, int numchannels, int samplebits, unsigned mixrate) {
	(void) SoundCard; (void) numvoices; (void) numchannels; (void) samplebits; (void) mixrate;
	return FX_Error;
}
int  FX_Shutdown(void) { return FX_Ok; }
int  FX_SetCallBack(void (*function)(unsigned long)) { (void) function; return FX_Ok; }
void FX_SetVolume(int volume) { (void) volume; }
int  FX_GetVolume(void) { return 0; }
void FX_SetReverseStereo(int setting) { (void) setting; }
int  FX_GetReverseStereo(void) { return 0; }
void FX_SetReverb(int reverb) { (void) reverb; }
void FX_SetFastReverb(int reverb) { (void) reverb; }
int  FX_GetMaxReverbDelay(void) { return 0; }
int  FX_GetReverbDelay(void) { return 0; }
void FX_SetReverbDelay(int delay) { (void) delay; }
int  FX_VoiceAvailable(int priority) { (void) priority; return 0; }
int  FX_EndLooping(int handle) { (void) handle; return FX_Ok; }
int  FX_SetPan(int handle, int vol, int left, int right) { (void) handle; (void) vol; (void) left; (void) right; return FX_Ok; }
int  FX_SetPitch(int handle, int pitchoffset) { (void) handle; (void) pitchoffset; return FX_Ok; }
int  FX_SetFrequency(int handle, int frequency) { (void) handle; (void) frequency; return FX_Ok; }
int  FX_PlayVOC(char *ptr, int pitchoffset, int vol, int left, int right, int priority, unsigned long callbackval) {
	(void) ptr; (void) pitchoffset; (void) vol; (void) left; (void) right; (void) priority; (void) callbackval;
	return FX_Warning;
}
int  FX_PlayLoopedVOC(char *ptr, long loopstart, long loopend, int pitchoffset, int vol, int left, int right,
                      int priority, unsigned long callbackval) {
	(void) ptr; (void) loopstart; (void) loopend; (void) pitchoffset; (void) vol; (void) left; (void) right;
	(void) priority; (void) callbackval;
	return FX_Warning;
}
int  FX_PlayWAV(char *ptr, int pitchoffset, int vol, int left, int right, int priority, unsigned long callbackval) {
	return FX_PlayVOC(ptr, pitchoffset, vol, left, right, priority, callbackval);
}
int  FX_PlayLoopedWAV(char *ptr, long loopstart, long loopend, int pitchoffset, int vol, int left, int right,
                      int priority, unsigned long callbackval) {
	return FX_PlayLoopedVOC(ptr, loopstart, loopend, pitchoffset, vol, left, right, priority, callbackval);
}
int  FX_PlayVOC3D(char *ptr, int pitchoffset, int angle, int distance, int priority, unsigned long callbackval) {
	(void) ptr; (void) pitchoffset; (void) angle; (void) distance; (void) priority; (void) callbackval;
	return FX_Warning;
}
int  FX_PlayWAV3D(char *ptr, int pitchoffset, int angle, int distance, int priority, unsigned long callbackval) {
	return FX_PlayVOC3D(ptr, pitchoffset, angle, distance, priority, callbackval);
}
int  FX_PlayVOC3D_ROTT(char *ptr, int size, int pitchoffset, int angle, int distance, int priority, unsigned long callbackval) {
	(void) size;
	return FX_PlayVOC3D(ptr, pitchoffset, angle, distance, priority, callbackval);
}
int  FX_PlayWAV3D_ROTT(char *ptr, int size, int pitchoffset, int angle, int distance, int priority, unsigned long callbackval) {
	(void) size;
	return FX_PlayVOC3D(ptr, pitchoffset, angle, distance, priority, callbackval);
}
int  FX_PlayRaw(char *ptr, unsigned long length, unsigned rate, int pitchoffset, int vol, int left, int right,
                int priority, unsigned long callbackval) {
	(void) ptr; (void) length; (void) rate; (void) pitchoffset; (void) vol; (void) left; (void) right;
	(void) priority; (void) callbackval;
	return FX_Warning;
}
int  FX_PlayLoopedRaw(char *ptr, unsigned long length, char *loopstart, char *loopend, unsigned rate,
                      int pitchoffset, int vol, int left, int right, int priority, unsigned long callbackval) {
	(void) loopstart; (void) loopend;
	return FX_PlayRaw(ptr, length, rate, pitchoffset, vol, left, right, priority, callbackval);
}
int  FX_Pan3D(int handle, int angle, int distance) { (void) handle; (void) angle; (void) distance; return FX_Ok; }
int  FX_SoundActive(int handle) { (void) handle; return 0; }
int  FX_SoundsPlaying(void) { return 0; }
int  FX_StopSound(int handle) { (void) handle; return FX_Ok; }
int  FX_StopAllSounds(void) { return FX_Ok; }
int  FX_StartDemandFeedPlayback(void (*function)(char **ptr, unsigned long *length), int rate, int pitchoffset,
                                int vol, int left, int right, int priority, unsigned long callbackval) {
	(void) function; (void) rate; (void) pitchoffset; (void) vol; (void) left; (void) right;
	(void) priority; (void) callbackval;
	return FX_Error;
}
int  FX_StartRecording(int MixRate, void (*function)(char *ptr, int length)) { (void) MixRate; (void) function; return FX_Error; }
void FX_StopRecord(void) {}

// ---- music ----
char *MUSIC_ErrorString(int ErrorNumber) { (void) ErrorNumber; return nocard; }
int  MUSIC_Init(int SoundCard, int Address) { (void) SoundCard; (void) Address; return MUSIC_Error; }
int  MUSIC_Shutdown(void) { return MUSIC_Ok; }
void MUSIC_SetMaxFMMidiChannel(int channel) { (void) channel; }
void MUSIC_SetVolume(int volume) { (void) volume; }
void MUSIC_SetMidiChannelVolume(int channel, int volume) { (void) channel; (void) volume; }
void MUSIC_ResetMidiChannelVolumes(void) {}
int  MUSIC_GetVolume(void) { return 0; }
void MUSIC_SetLoopFlag(int loopflag) { (void) loopflag; }
int  MUSIC_SongPlaying(void) { return 0; }
void MUSIC_Continue(void) {}
void MUSIC_Pause(void) {}
int  MUSIC_StopSong(void) { return MUSIC_Ok; }
int  MUSIC_PlaySong(unsigned char *song, int loopflag) { (void) song; (void) loopflag; return MUSIC_Error; }
int  MUSIC_PlaySongROTT(unsigned char *song, int size, int loopflag) { (void) song; (void) size; (void) loopflag; return MUSIC_Error; }
void MUSIC_SetContext(int context) { (void) context; }
int  MUSIC_GetContext(void) { return 0; }
void MUSIC_SetSongTick(unsigned long PositionInTicks) { (void) PositionInTicks; }
void MUSIC_SetSongTime(unsigned long milliseconds) { (void) milliseconds; }
void MUSIC_SetSongPosition(int measure, int beat, int tick) { (void) measure; (void) beat; (void) tick; }
void MUSIC_GetSongPosition(songposition *pos) { memset(pos, 0, sizeof *pos); }
void MUSIC_GetSongLength(songposition *pos) { memset(pos, 0, sizeof *pos); }
int  MUSIC_FadeVolume(int tovolume, int milliseconds) { (void) tovolume; (void) milliseconds; return MUSIC_Ok; }
int  MUSIC_FadeActive(void) { return 0; }
void MUSIC_StopFade(void) {}
void MUSIC_RerouteMidiChannel(int channel, int cdecl (*function)(int event, int c1, int c2)) { (void) channel; (void) function; }
void MUSIC_RegisterTimbreBank(unsigned char *timbres) { (void) timbres; }
