#ifndef BT_UUIDS_H
#define BT_UUIDS_H

/*
 W e wil*l need to stream a service with characteristics for each
 type of user input which should include:
 - X-axs from the thumbstick
 - Y-axis from the thumstick
 - Button press from the thumbstick
 - Button press (Yellow)
 - Button press (Red)
 - Button press (Green)
 - Button press (Blue)
 */
static constexpr const char *CENTRAL_NAME = "DUCKS_Central";
static constexpr const char *CENTRAL_ADDRESS = "a8:61:0a:43:64:38";

static constexpr const char *CONTROLLER_UUID = "547b5676-0377-480f-b6f8-2a94873c07ec";
static constexpr const char *LEFT_THUMB_STICK_X_AXIS_UUID = "5b03b0ef-c8db-4ef0-adf6-09e23d41a68d";
static constexpr const char *LEFT_THUMB_STICK_Y_AXIS_UUID = "b1169c28-5e12-4213-a4ff-0aa316f233cc";
static constexpr const char *LEFT_THUMB_STICK_BUTTON_UUID = "c2dd566c-65bb-4d28-8708-227c923433cf";
static constexpr const char *RIGHT_THUMB_STICK_X_AXIS_UUID = "37aa7d87-3161-4b8a-8f84-afddaf53ced3";
static constexpr const char *RIGHT_THUMB_STICK_Y_AXIS_UUID = "88d80b83-dcf1-47cf-b7dd-6b8c6d01b072";
static constexpr const char *RIGHT_THUMB_STICK_BUTTON_UUID = "c817eaef-8d21-4df8-a7d7-a4e27c63b390";
static constexpr const char *YELLOW_BUTTON_UUID = "b273ac1b-e05f-4e47-a508-6c87d89e46eb";
static constexpr const char *RED_BUTTON_UUID = "6db0dbd7-8830-4e3d-b885-baf0e4c75d93";
static constexpr const char *GREEN_BUTTON_UUID = "ec339d10-06c3-4ad0-80dc-0066f0fea2b9";
static constexpr const char *BLUE_BUTTON_UUID = "473abf6f-2591-427d-9ae9-9546d01e2287";

#endif
