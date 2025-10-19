#pragma once

namespace ev
{
/* The unknown key */
#define EV_KEY_UNKNOWN            -1

/* Printable keys */
#define EV_KEY_SPACE              32
#define EV_KEY_APOSTROPHE         39  /* ' */
#define EV_KEY_COMMA              44  /* , */
#define EV_KEY_MINUS              45  /* - */
#define EV_KEY_PERIOD             46  /* . */
#define EV_KEY_SLASH              47  /* / */
#define EV_KEY_0                  48
#define EV_KEY_1                  49
#define EV_KEY_2                  50
#define EV_KEY_3                  51
#define EV_KEY_4                  52
#define EV_KEY_5                  53
#define EV_KEY_6                  54
#define EV_KEY_7                  55
#define EV_KEY_8                  56
#define EV_KEY_9                  57
#define EV_KEY_SEMICOLON          59  /* ; */
#define EV_KEY_EQUAL              61  /* = */
#define EV_KEY_A                  65
#define EV_KEY_B                  66
#define EV_KEY_C                  67
#define EV_KEY_D                  68
#define EV_KEY_E                  69
#define EV_KEY_F                  70
#define EV_KEY_G                  71
#define EV_KEY_H                  72
#define EV_KEY_I                  73
#define EV_KEY_J                  74
#define EV_KEY_K                  75
#define EV_KEY_L                  76
#define EV_KEY_M                  77
#define EV_KEY_N                  78
#define EV_KEY_O                  79
#define EV_KEY_P                  80
#define EV_KEY_Q                  81
#define EV_KEY_R                  82
#define EV_KEY_S                  83
#define EV_KEY_T                  84
#define EV_KEY_U                  85
#define EV_KEY_V                  86
#define EV_KEY_W                  87
#define EV_KEY_X                  88
#define EV_KEY_Y                  89
#define EV_KEY_Z                  90
#define EV_KEY_LEFT_BRACKET       91  /* [ */
#define EV_KEY_BACKSLASH          92  /* \ */
#define EV_KEY_RIGHT_BRACKET      93  /* ] */
#define EV_KEY_GRAVE_ACCENT       96  /* ` */
#define EV_KEY_WORLD_1            161 /* non-US #1 */
#define EV_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define EV_KEY_ESCAPE             256
#define EV_KEY_ENTER              257
#define EV_KEY_TAB                258
#define EV_KEY_BACKSPACE          259
#define EV_KEY_INSERT             260
#define EV_KEY_DELETE             261
#define EV_KEY_RIGHT              262
#define EV_KEY_LEFT               263
#define EV_KEY_DOWN               264
#define EV_KEY_UP                 265
#define EV_KEY_PAGE_UP            266
#define EV_KEY_PAGE_DOWN          267
#define EV_KEY_HOME               268
#define EV_KEY_END                269
#define EV_KEY_CAPS_LOCK          280
#define EV_KEY_SCROLL_LOCK        281
#define EV_KEY_NUM_LOCK           282
#define EV_KEY_PRINT_SCREEN       283
#define EV_KEY_PAUSE              284
#define EV_KEY_F1                 290
#define EV_KEY_F2                 291
#define EV_KEY_F3                 292
#define EV_KEY_F4                 293
#define EV_KEY_F5                 294
#define EV_KEY_F6                 295
#define EV_KEY_F7                 296
#define EV_KEY_F8                 297
#define EV_KEY_F9                 298
#define EV_KEY_F10                299
#define EV_KEY_F11                300
#define EV_KEY_F12                301
#define EV_KEY_F13                302
#define EV_KEY_F14                303
#define EV_KEY_F15                304
#define EV_KEY_F16                305
#define EV_KEY_F17                306
#define EV_KEY_F18                307
#define EV_KEY_F19                308
#define EV_KEY_F20                309
#define EV_KEY_F21                310
#define EV_KEY_F22                311
#define EV_KEY_F23                312
#define EV_KEY_F24                313
#define EV_KEY_F25                314
#define EV_KEY_KP_0               320
#define EV_KEY_KP_1               321
#define EV_KEY_KP_2               322
#define EV_KEY_KP_3               323
#define EV_KEY_KP_4               324
#define EV_KEY_KP_5               325
#define EV_KEY_KP_6               326
#define EV_KEY_KP_7               327
#define EV_KEY_KP_8               328
#define EV_KEY_KP_9               329
#define EV_KEY_KP_DECIMAL         330
#define EV_KEY_KP_DIVIDE          331
#define EV_KEY_KP_MULTIPLY        332
#define EV_KEY_KP_SUBTRACT        333
#define EV_KEY_KP_ADD             334
#define EV_KEY_KP_ENTER           335
#define EV_KEY_KP_EQUAL           336
#define EV_KEY_LEFT_SHIFT         340
#define EV_KEY_LEFT_CONTROL       341
#define EV_KEY_LEFT_ALT           342
#define EV_KEY_LEFT_SUPER         343
#define EV_KEY_RIGHT_SHIFT        344
#define EV_KEY_RIGHT_CONTROL      345
#define EV_KEY_RIGHT_ALT          346
#define EV_KEY_RIGHT_SUPER        347
#define EV_KEY_MENU               348

#define EV_KEY_LAST               EV_KEY_MENU

#define EV_RELEASE                0
#define EV_PRESS                  1
#define EV_REPEAT                 2
}