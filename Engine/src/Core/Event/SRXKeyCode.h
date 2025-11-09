#pragma once

namespace srx
{
/* The unknown key */
#define SRX_KEY_UNKNOWN            -1

/* Printable keys */
#define SRX_KEY_SPACE              32
#define SRX_KEY_APOSTROPHE         39  /* ' */
#define SRX_KEY_COMMA              44  /* , */
#define SRX_KEY_MINUS              45  /* - */
#define SRX_KEY_PERIOD             46  /* . */
#define SRX_KEY_SLASH              47  /* / */
#define SRX_KEY_0                  48
#define SRX_KEY_1                  49
#define SRX_KEY_2                  50
#define SRX_KEY_3                  51
#define SRX_KEY_4                  52
#define SRX_KEY_5                  53
#define SRX_KEY_6                  54
#define SRX_KEY_7                  55
#define SRX_KEY_8                  56
#define SRX_KEY_9                  57
#define SRX_KEY_SEMICOLON          59  /* ; */
#define SRX_KEY_EQUAL              61  /* = */
#define SRX_KEY_A                  65
#define SRX_KEY_B                  66
#define SRX_KEY_C                  67
#define SRX_KEY_D                  68
#define SRX_KEY_E                  69
#define SRX_KEY_F                  70
#define SRX_KEY_G                  71
#define SRX_KEY_H                  72
#define SRX_KEY_I                  73
#define SRX_KEY_J                  74
#define SRX_KEY_K                  75
#define SRX_KEY_L                  76
#define SRX_KEY_M                  77
#define SRX_KEY_N                  78
#define SRX_KEY_O                  79
#define SRX_KEY_P                  80
#define SRX_KEY_Q                  81
#define SRX_KEY_R                  82
#define SRX_KEY_S                  83
#define SRX_KEY_T                  84
#define SRX_KEY_U                  85
#define SRX_KEY_V                  86
#define SRX_KEY_W                  87
#define SRX_KEY_X                  88
#define SRX_KEY_Y                  89
#define SRX_KEY_Z                  90
#define SRX_KEY_LEFT_BRACKET       91  /* [ */
#define SRX_KEY_BACKSLASH          92  /* \ */
#define SRX_KEY_RIGHT_BRACKET      93  /* ] */
#define SRX_KEY_GRAVE_ACCENT       96  /* ` */
#define SRX_KEY_WORLD_1            161 /* non-US #1 */
#define SRX_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define SRX_KEY_ESCAPE             256
#define SRX_KEY_ENTER              257
#define SRX_KEY_TAB                258
#define SRX_KEY_BACKSPACE          259
#define SRX_KEY_INSERT             260
#define SRX_KEY_DELETE             261
#define SRX_KEY_RIGHT              262
#define SRX_KEY_LEFT               263
#define SRX_KEY_DOWN               264
#define SRX_KEY_UP                 265
#define SRX_KEY_PAGE_UP            266
#define SRX_KEY_PAGE_DOWN          267
#define SRX_KEY_HOME               268
#define SRX_KEY_END                269
#define SRX_KEY_CAPS_LOCK          280
#define SRX_KEY_SCROLL_LOCK        281
#define SRX_KEY_NUM_LOCK           282
#define SRX_KEY_PRINT_SCREEN       283
#define SRX_KEY_PAUSE              284
#define SRX_KEY_F1                 290
#define SRX_KEY_F2                 291
#define SRX_KEY_F3                 292
#define SRX_KEY_F4                 293
#define SRX_KEY_F5                 294
#define SRX_KEY_F6                 295
#define SRX_KEY_F7                 296
#define SRX_KEY_F8                 297
#define SRX_KEY_F9                 298
#define SRX_KEY_F10                299
#define SRX_KEY_F11                300
#define SRX_KEY_F12                301
#define SRX_KEY_F13                302
#define SRX_KEY_F14                303
#define SRX_KEY_F15                304
#define SRX_KEY_F16                305
#define SRX_KEY_F17                306
#define SRX_KEY_F18                307
#define SRX_KEY_F19                308
#define SRX_KEY_F20                309
#define SRX_KEY_F21                310
#define SRX_KEY_F22                311
#define SRX_KEY_F23                312
#define SRX_KEY_F24                313
#define SRX_KEY_F25                314
#define SRX_KEY_KP_0               320
#define SRX_KEY_KP_1               321
#define SRX_KEY_KP_2               322
#define SRX_KEY_KP_3               323
#define SRX_KEY_KP_4               324
#define SRX_KEY_KP_5               325
#define SRX_KEY_KP_6               326
#define SRX_KEY_KP_7               327
#define SRX_KEY_KP_8               328
#define SRX_KEY_KP_9               329
#define SRX_KEY_KP_DECIMAL         330
#define SRX_KEY_KP_DIVIDE          331
#define SRX_KEY_KP_MULTIPLY        332
#define SRX_KEY_KP_SUBTRACT        333
#define SRX_KEY_KP_ADD             334
#define SRX_KEY_KP_ENTER           335
#define SRX_KEY_KP_EQUAL           336
#define SRX_KEY_LEFT_SHIFT         340
#define SRX_KEY_LEFT_CONTROL       341
#define SRX_KEY_LEFT_ALT           342
#define SRX_KEY_LEFT_SUPER         343
#define SRX_KEY_RIGHT_SHIFT        344
#define SRX_KEY_RIGHT_CONTROL      345
#define SRX_KEY_RIGHT_ALT          346
#define SRX_KEY_RIGHT_SUPER        347
#define SRX_KEY_MENU               348

#define SRX_KEY_LAST               SRX_KEY_MENU

#define SRX_RELEASE                0
#define SRX_PRESS                  1
#define SRX_REPEAT                 2
}