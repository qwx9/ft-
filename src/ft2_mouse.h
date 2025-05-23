#ifndef _ft2_mouse_h_
#define _ft2_mouse_h_

#include <stdint.h>
#include <stdbool.h>

enum
{
	MOUSE_MODE_NORMAL = 0,
	MOUSE_MODE_DELETE = 1,
	MOUSE_MODE_RENAME = 2,

	MOUSE_WHEEL_DOWN = 0,
	MOUSE_WHEEL_UP = 1
};

typedef struct mouse_t
{
	volatile bool setPosFlag;
	bool leftButtonPressed, rightButtonPressed, leftButtonReleased, rightButtonReleased;
	bool firstTimePressingButton, mouseOverTextBox;
	int8_t buttonCounter, mode;
	int16_t lastUsedObjectID, lastUsedObjectType, lastEditBox;
	int32_t absX, absY, rawX, rawY, x, y, lastX, lastY, xBias, yBias, setPosX, setPosY;
	uint32_t buttonState;
} mouse_t;

extern mouse_t mouse; // ft2_mouse.c

// do not change these!
#define MOUSE_CURSOR_W 26
#define MOUSE_CURSOR_H 23
#define MOUSE_GLASS_ANI_FRAMES 22
#define MOUSE_CLOCK_ANI_FRAMES 5

#if defined __APPLE__ && defined __aarch64__
void armMacGhostMouseCursorFix(void);
#endif

void freeMouseCursors(void);
bool createMouseCursors(void);
void setMousePosToCenter(void);
void setMouseShape(int16_t shape);
void setMouseMode(uint8_t mode);
void mouseWheelHandler(bool directionUp);
void mouseButtonUpHandler(uint8_t mouseButton);
void mouseButtonDownHandler(uint8_t mouseButton);
void updateMouseScaling(void);
void setMouseBusy(bool busy); // can be called from other threads
void mouseAnimOn(void);
void mouseAnimOff(void);
void animateBusyMouse(void);
void handleLastGUIObjectDown(void);
void readMouseXY(void);
void resetMouseBusyAnimation(void);

#endif
