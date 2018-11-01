
#ifndef _IO_EVENT_H
#define _IO_EVENT_H


<<<<<<< HEAD
typedef enum
{
    IO_EVT_NONE = 0,
    IO_EVT_RIGHT,
    IO_EVT_DOWN,
    IO_EVT_LEFT,
    IO_EVT_UP,


    IO_EVT_CNT,
}_ioEvt;

/* key active type */
enum
{
    IO_KEY_NONE = 0,
    IO_KEY_DOWN,
    IO_KEY_UP_BEFORE_HOLD,
    IO_KEY_HOLD,
    IO_KEY_REPEAT,
    IO_KEY_UP_AFTER_HOLD,
};

/* key track ticks */
#define KEY_DOWN_DEFAULT_DELAY      (5)
#define KEY_HOLD_DEFAULT_DELAY      (100)
#define KEY_REPEAT_DEFAULT_DELAY    (20)

=======


>>>>>>> e96eff9db448919ea7b4b15e25bb05bf6f4dfc56
#endif

