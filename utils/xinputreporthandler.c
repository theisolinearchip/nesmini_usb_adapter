#ifndef XinputReportHandler_c
#define XinputReportHandler_c

// REPORT INDEX
// ------------

#define XINPUT_REPORT_SIZE 						20

#define XINPUT_REPORT_INDEX_BUTTONS				2
#define XINPUT_REPORT_INDEX_TRIGGER_LEFT		4
#define XINPUT_REPORT_INDEX_TRIGGER_RIGHT		5
#define XINPUT_REPORT_INDEX_JOYSTICK_LEFT		6
#define XINPUT_REPORT_INDEX_JOYSTICK_RIGHT		10

// BUTTONS 
// -------

#define XINPUT_BUTTON_Y 					0x8000		// b 1000 0000 - report[3]
#define XINPUT_BUTTON_X 					0x4000		// b 0100 0000 - report[3]
#define XINPUT_BUTTON_B 					0x2000		// b 0010 0000 - report[3]
#define XINPUT_BUTTON_A 					0x1000		// b 0001 0000 - report[3]

// 0x0800 unused
#define XINPUT_BUTTON_LOGO					0x0400		// b 0000 0100 - report[3]
#define XINPUT_BUTTON_RIGHT					0x0200		// b 0000 0010 - report[3]
#define XINPUT_BUTTON_LEFT  				0x0100		// b 0000 0001 - report[3]			

#define XINPUT_BUTTON_R3 					0x0080		// b 1000 0000 - report[2]
#define XINPUT_BUTTON_L3 					0x0040		// b 0100 0000 - report[2]
#define XINPUT_BUTTON_BACK 					0x0020		// b 0010 0000 - report[2]
#define XINPUT_BUTTON_START 				0x0010		// b 0001 0000 - report[2]

#define XINPUT_BUTTON_DPAD_RIGHT	 		0x0008		// b 0000 1000 - report[2]
#define XINPUT_BUTTON_DPAD_LEFT				0x0004		// b 0000 0100 - report[2]
#define XINPUT_BUTTON_DPAD_DOWN				0x0002		// b 0000 0010 - report[2]
#define XINPUT_BUTTON_DPAD_UP 				0x0001		// b 0000 0001 - report[2]


void XinputReportInit(char *destinationReport) {
	for (int i = 0; i < XINPUT_REPORT_SIZE; i++) {
		destinationReport[i] = 0x00;
	}
	destinationReport[1] = 0x14; // byte length (always 20)
}

// Buttons
void XinputReportSetButtons(char *destinationReport, int buttons) {
	if (sizeof(buttons) != 2) return;

	destinationReport[XINPUT_REPORT_INDEX_BUTTONS] = buttons;
	destinationReport[XINPUT_REPORT_INDEX_BUTTONS + 1] = buttons >> 8;
}

// Triggers
void XinputReportSetTrigger(char *destinationReport, char trigger, uchar triggerValue) {
	destinationReport[(int) trigger] = triggerValue;
}

void XinputReportSetTriggerLeft(char *destinationReport, uchar triggerValue) {
	XinputReportSetTrigger(destinationReport, XINPUT_REPORT_INDEX_TRIGGER_LEFT, triggerValue);
}

void XinputReportSetTriggerRight(char *destinationReport, uchar triggerValue) {
	XinputReportSetTrigger(destinationReport, XINPUT_REPORT_INDEX_TRIGGER_RIGHT, triggerValue);
}

// Joystick
void XinputReportSetJoystick(char *destinationReport, char joystick, signed int xAxis, signed int yAxis) {
	// signed 16 bits number per axis
	// (-32768 to 32767)

	// "left" = neg / "right" = pos
	destinationReport[(int) joystick] = xAxis;
	destinationReport[(int) joystick + 1] = xAxis >> 8;

	// "up" = pos / "down" = neg
	destinationReport[(int) joystick + 2] = yAxis;
	destinationReport[(int) joystick + 3] = yAxis >> 8;
}

void XinputReportSetJoystickLeft(char *destinationReport, signed int xAxis, signed int yAxis) {
	XinputReportSetJoystick(destinationReport, XINPUT_REPORT_INDEX_JOYSTICK_LEFT, xAxis, yAxis);
}

void XinputReportSetJoystickRight(char *destinationReport, signed int xAxis, signed int yAxis) {
	XinputReportSetJoystick(destinationReport, XINPUT_REPORT_INDEX_JOYSTICK_RIGHT, xAxis, yAxis);
}

// Aux

char XinputReportBuffersEqual(char *reportOne, char *reportTwo) {
	for (int i = 0; i < XINPUT_REPORT_SIZE; i++) {
		if (reportOne[i] != reportTwo[i]) {
			return 0;
		}
	}
	return 1;
}

signed int XinputAxisPercent(char val) {
	if (val < 0) {
		return (val < -100) ? -32768 : ( (float) val / 100 * 32768);
	}
	return (val > 100) ? 32767 : ( (float) val / 100 * 32767);
}

signed int XinputTriggerPercent(char val) {
	if (val < 0) {
		return 0;
	}
	return (val > 100) ? 255 : ( (float) val / 100 * 255);
}

#endif