#ifdef HAS_MIDI

// for finding memory leaks in debug mode with Visual Studio
#if defined _DEBUG && defined _MSC_VER
#include <crtdbg.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include "ft2_header.h"
#include "ft2_edit.h"
#include "ft2_config.h"
#include "ft2_gui.h"
#include "ft2_midi.h"
#include "ft2_audio.h"
#include "ft2_mouse.h"
#include "ft2_pattern_ed.h"
#include "ft2_structs.h"
#include "rtmidi/rtmidi_c.h"

#define MAX_DEV_STR_LEN 256

// hide POSIX warnings
#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

// This implements MIDI input only!

midi_t midi; // globalized

static volatile bool midiDeviceOpened;
static bool recMIDIValidChn = true;
static RtMidiPtr midiDev;

static inline void midiInSetChannel(uint8_t status)
{
	recMIDIValidChn = (config.recMIDIAllChn || (status & 0xF) == config.recMIDIChn-1);
}

static inline void midiInKeyAction(int8_t m, uint8_t mv)
{
	int16_t vol = (mv * 64 * config.recMIDIVolSens) / (127 * 100);
	if (vol > 64)
		vol = 64;

	// FT2 bugfix: If velocity>0, and sensitivity made vol=0, set vol to 1 (prevent key off)
	if (mv > 0 && vol == 0)
		vol = 1;

	if (mv > 0 && !config.recMIDIVelocity)
		vol = -1; // don't record volume (velocity)

	m -= 11;
	if (config.recMIDITransp)
		m += (int8_t)config.recMIDITranspVal;

	if ((mv == 0 || vol != 0) && m > 0 && m < 96 && recMIDIValidChn)
		recordNote(m, (int8_t)vol);
}

static inline void midiInControlChange(uint8_t data1, uint8_t data2)
{
	if (data1 != 1) // 1 = modulation wheel
		return;

	midi.currMIDIVibDepth = data2 << 6;

	if (recMIDIValidChn) // real FT2 forgot to check this here..
	{
		for (uint8_t i = 0; i < song.numChannels; i++)
		{
			if (channel[i].midiVibDepth != 0 || editor.keyOnTab[i] != 0)
				channel[i].midiVibDepth = midi.currMIDIVibDepth;
		}
	}

	const uint8_t vibDepth = (midi.currMIDIVibDepth >> 9) & 0x0F;
	if (vibDepth > 0 && recMIDIValidChn)
		recordMIDIEffect(0x04, 0xA0 | vibDepth);
}

static inline void midiInPitchBendChange(uint8_t data1, uint8_t data2)
{
	int16_t pitch = (int16_t)((data2 << 7) | data1) - 8192; // -8192..8191
	pitch >>= 6; // -128..127

	midi.currMIDIPitch = pitch;
	if (recMIDIValidChn)
	{
		channel_t *ch = channel;
		for (uint8_t i = 0; i < song.numChannels; i++, ch++)
		{
			if (ch->midiPitch != 0 || editor.keyOnTab[i] != 0)
				ch->midiPitch = midi.currMIDIPitch;
		}
	}
}

static void midiInCallback(double dTimeStamp, const unsigned char *message, size_t messageSize, void *userData)
{
	uint8_t byte[3];

	if (!midi.enable || messageSize < 2)
		return;

	byte[0] = message[0];
	if (byte[0] > 127 && byte[0] < 240)
	{
		byte[1] = message[1] & 0x7F;

		if (messageSize >= 3)
			byte[2] = message[2] & 0x7F;
		else
			byte[2] = 0;

		midiInSetChannel(byte[0]);

		     if (byte[0] >= 128 && byte[0] <= 128+15)       midiInKeyAction(byte[1], 0);
		else if (byte[0] >= 144 && byte[0] <= 144+15)       midiInKeyAction(byte[1], byte[2]);
		else if (byte[0] >= 176 && byte[0] <= 176+15)   midiInControlChange(byte[1], byte[2]);
		else if (byte[0] >= 224 && byte[0] <= 224+15) midiInPitchBendChange(byte[1], byte[2]);
	}

	(void)dTimeStamp;
	(void)userData;
}

static uint32_t getNumMidiInDevices(void)
{
	if (midiDev == NULL)
		return 0;

	return rtmidi_get_port_count(midiDev);
}

static char *getMidiInDeviceName(uint32_t deviceID)
{
	if (midiDev == NULL)
		return NULL;

	char *devStr = (char *)rtmidi_get_port_name(midiDev, deviceID);
	if (!midiDev->ok)
		return NULL;

	return devStr;
}

void closeMidiInDevice(void)
{
	while (!midi.initThreadDone);

	if (midiDeviceOpened)
	{
		if (midiDev != NULL)
		{
			rtmidi_in_cancel_callback(midiDev);
			rtmidi_close_port(midiDev);
		}

		midiDeviceOpened = false;
	}
}

void freeMidiIn(void)
{
	while (!midi.initThreadDone);

	if (midiDev != NULL)
	{
		rtmidi_in_free(midiDev);
		midiDev = NULL;
	}
}

bool initMidiIn(void)
{
	if (midiDev != NULL)
		return false; // already initialized

	midiDev = rtmidi_in_create_default();
	if (!midiDev->ok)
	{
		midiDev = NULL;
		return false;
	}

	midiDeviceOpened = false;
	return true;
}

bool openMidiInDevice(uint32_t deviceID)
{
	if (midiDev == NULL)
		return false;

	const uint32_t numDevices = getNumMidiInDevices();
	if (numDevices == 0 || numDevices != midi.numInputDevices){
		midi.rescanDevicesFlag = true;
		return false;
	}

	rtmidi_open_port(midiDev, deviceID, "FT2 Clone MIDI Port");
	if (!midiDev->ok)
		return false;

	rtmidi_in_set_callback(midiDev, midiInCallback, NULL);
	if (!midiDev->ok)
	{
		rtmidi_close_port(midiDev);
		return false;
	}

	rtmidi_in_ignore_types(midiDev, true, true, true);

	midiDeviceOpened = true;
	return true;
}

void recordMIDIEffect(uint8_t efx, uint8_t efxData)
{
	// only handle this in record mode
	if (!midi.enable || (playMode != PLAYMODE_RECSONG && playMode != PLAYMODE_RECPATT))
		return;

	if (config.multiRec)
	{
		note_t *p = &pattern[editor.editPattern][editor.row * MAX_CHANNELS];
		for (int32_t i = 0; i < song.numChannels; i++, p++)
		{
			if (config.multiRecChn[i] && editor.chnMode[i])
			{
				if (!allocatePattern(editor.editPattern))
					return;

				if (p->efx == 0)
				{
					p->efx = efx;
					p->efxData = efxData;
					setSongModifiedFlag();
				}
			}
		}
	}
	else
	{
		if (!allocatePattern(editor.editPattern))
			return;

		note_t *p = &pattern[editor.editPattern][(editor.row * MAX_CHANNELS) + cursor.ch];
		if (p->efx != efx || p->efxData != efxData)
			setSongModifiedFlag();

		p->efx = efx;
		p->efxData = efxData;
	}
}

bool saveMidiInputDeviceToConfig(void)
{
	if (!midi.initThreadDone || midiDev == NULL || !midiDeviceOpened)
		return false;

	const uint32_t numDevices = getNumMidiInDevices();
	if (numDevices == 0)
		return false;

	char *midiInStr = getMidiInDeviceName(midi.inputDevice);
	if (midiInStr == NULL)
		return false;

	FILE *f = UNICHAR_FOPEN(editor.midiConfigFileLocationU, "w");
	if (f == NULL)
	{
		free(midiInStr);
		return false;
	}

	fputs(midiInStr, f);
	free(midiInStr);

	fclose(f);
	return true;
}

bool setMidiInputDeviceFromConfig(void)
{
	uint32_t i;

	if (midi.inputDeviceName != NULL)
	{
		free(midi.inputDeviceName);
		midi.inputDeviceName = NULL;
	}

	const uint32_t numDevices = getNumMidiInDevices();
	if (numDevices == 0)
		goto setDefMidiInputDev;

	FILE *f = UNICHAR_FOPEN(editor.midiConfigFileLocationU, "r");
	if (f == NULL)
		goto setDefMidiInputDev;

	char *devString = (char *)malloc((MAX_DEV_STR_LEN+4) * sizeof (char));
	if (devString == NULL)
	{
		fclose(f);
		goto setDefMidiInputDev;
	}

	devString[0] = '\0';

	if (fgets(devString, MAX_DEV_STR_LEN, f) == NULL)
	{
		fclose(f);
		free(devString);
		goto setDefMidiInputDev;
	}

	fclose(f);

	// scan for device in list
	char *midiInStr = NULL;
	for (i = 0; i < numDevices; i++)
	{
		midiInStr = getMidiInDeviceName(i);
		if (midiInStr == NULL)
			continue;

		if (!_stricmp(devString, midiInStr))
			break; // device matched

		free(midiInStr);
		midiInStr = NULL;
	}

	free(devString);

	// device not found in list, set default
	if (i == numDevices)
		goto setDefMidiInputDev;

	midi.inputDevice = i;
	midi.inputDeviceName = midiInStr;
	midi.numInputDevices = numDevices;

	return true;

	// couldn't load device, set default
setDefMidiInputDev:
	midi.inputDevice = 0;
	midi.inputDeviceName = strdup("RtMidi");
	midi.numInputDevices = numDevices;

	return false;
}

void freeMidiInputDeviceList(void)
{
	for (int32_t i = 0; i < MAX_MIDI_DEVICES; i++)
	{
		if (midi.inputDeviceNames[i] != NULL)
		{
			free(midi.inputDeviceNames[i]);
			midi.inputDeviceNames[i] = NULL;
		}
	}

	midi.numInputDevices = 0;
}

void rescanMidiInputDevices(void)
{
	freeMidiInputDeviceList();

	midi.numInputDevices = getNumMidiInDevices();
	if (midi.numInputDevices > MAX_MIDI_DEVICES)
		midi.numInputDevices = MAX_MIDI_DEVICES;

	for (int32_t i = 0; i < midi.numInputDevices; i++)
	{
		char *deviceName = getMidiInDeviceName(i);
		if (deviceName == NULL)
		{
			if (midi.numInputDevices > 0)
				midi.numInputDevices--; // hide device

			continue;
		}

		midi.inputDeviceNames[i] = deviceName;
	}

	setScrollBarEnd(SB_MIDI_INPUT_SCROLL, midi.numInputDevices);
	setScrollBarPos(SB_MIDI_INPUT_SCROLL, 0, false);
}

void drawMidiInputList(void)
{
	clearRect(114, 4, 365, 165);

	if (!midi.initThreadDone || midiDev == NULL || midi.numInputDevices == 0)
	{
		textOut(114, 4 + (0 * 11), PAL_FORGRND, "No MIDI input devices found!");
		textOut(114, 4 + (1 * 11), PAL_FORGRND, "Either wait a few seconds for MIDI to initialize, or restart the");
		textOut(114, 4 + (2 * 11), PAL_FORGRND, "tracker if you recently plugged in a MIDI device.");
		return;
	}

	for (uint16_t i = 0; i < 15; i++)
	{
		const int32_t deviceEntry = getScrollBarPos(SB_MIDI_INPUT_SCROLL) + i;
		if (deviceEntry < midi.numInputDevices)
		{
			if (midi.inputDeviceNames[deviceEntry] == NULL)
				continue;

			const uint16_t y = 4 + (i * 11);

			if (midi.inputDeviceName != NULL)
			{
				if (_stricmp(midi.inputDeviceName, midi.inputDeviceNames[deviceEntry]) == 0)
					fillRect(114, y, 365, 10, PAL_BOXSLCT); // selection background color
			}

			char *tmpString = utf8ToCp437(midi.inputDeviceNames[deviceEntry], true);
			if (tmpString != NULL)
			{
				textOutClipX(114, y, PAL_FORGRND, tmpString, 479);
				free(tmpString);
			}
		}
	}
}

void scrollMidiInputDevListUp(void)
{
	scrollBarScrollUp(SB_MIDI_INPUT_SCROLL, 1);
}

void scrollMidiInputDevListDown(void)
{
	scrollBarScrollDown(SB_MIDI_INPUT_SCROLL, 1);
}

void sbMidiInputSetPos(uint32_t pos)
{
	if (ui.configScreenShown && editor.currConfigScreen == CONFIG_SCREEN_MIDI_INPUT)
		drawMidiInputList();

	(void)pos;
}

bool testMidiInputDeviceListMouseDown(void)
{
	if (!ui.configScreenShown || editor.currConfigScreen != CONFIG_SCREEN_MIDI_INPUT)
		return false; // we didn't click the area

	if (!midi.initThreadDone)
		return true;

	const int32_t mx = mouse.x;
	const int32_t my = mouse.y;

	if (my < 4 || my > 166 || mx < 114 || mx > 479)
		return false; // we didn't click the area

	const int32_t deviceNum = (int32_t)scrollBars[SB_MIDI_INPUT_SCROLL].pos + ((my - 4) / 11);
	if (midi.numInputDevices <= 0 || deviceNum >= midi.numInputDevices)
		return true;

	if (midi.inputDeviceName != NULL)
	{
		if (!_stricmp(midi.inputDeviceName, midi.inputDeviceNames[deviceNum]))
			return true; // we clicked the currently selected device, do nothing

		free(midi.inputDeviceName);
	}

	midi.inputDeviceName = strdup(midi.inputDeviceNames[deviceNum]);
	midi.inputDevice = deviceNum;

	closeMidiInDevice();
	freeMidiIn();
	initMidiIn();
	if(!openMidiInDevice(midi.inputDevice)){
		free(midi.inputDeviceName);
		midi.inputDeviceName = NULL;
		midi.inputDevice = -1;
		return false;
	}

	drawMidiInputList();
	return true;
}

int32_t SDLCALL initMidiFunc(void *ptr)
{
	midi.closeMidiOnExit = true;

	midi.initThreadDone = false;
	initMidiIn();
	setMidiInputDeviceFromConfig();
	openMidiInDevice(midi.inputDevice);
	midi.initThreadDone = true;

	midi.rescanDevicesFlag = true;

	return true;

	(void)ptr;
}

#else
typedef int make_iso_compilers_happy; // kludge: prevent warning about empty .c file if HAS_MIDI is not defined
#endif
