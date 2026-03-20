#ifndef _EBTN_H
#define _EBTN_H

#include <stdint.h>
#include <string.h>

#include "bit_array.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// #define EBTN_CONFIG_TIMER_16

// here can change to uint16_t, if you want reduce RAM size.
#ifdef EBTN_CONFIG_TIMER_16
typedef uint16_t ebtn_time_t;
typedef int16_t ebtn_time_sign_t;
#else
typedef uint32_t ebtn_time_t;
typedef int32_t ebtn_time_sign_t;
#endif

/* Forward declarations */
struct ebtn_btn;
struct ebtn;

#define EBTN_MAX_KEYNUM (64)

/**
 * @brief           按钮事件列表
 *
 */
typedef enum
{
    EBTN_EVT_ONPRESS = 0x00, /*!< On press event - sent when valid press is detected */
    EBTN_EVT_ONRELEASE,      /*!< On release event - sent when valid release event is detected (from
                                active to inactive) */
    EBTN_EVT_ONCLICK,        /*!< On Click event - sent when valid sequence of on-press and on-release
                                events occurs */
    EBTN_EVT_KEEPALIVE,      /*!< Keep alive event - sent periodically when button is active */
} ebtn_evt_t;

#define EBTN_EVT_MASK_ONPRESS   (1 << EBTN_EVT_ONPRESS)
#define EBTN_EVT_MASK_ONRELEASE (1 << EBTN_EVT_ONRELEASE)
#define EBTN_EVT_MASK_ONCLICK   (1 << EBTN_EVT_ONCLICK)
#define EBTN_EVT_MASK_KEEPALIVE (1 << EBTN_EVT_KEEPALIVE)

#define EBTN_EVT_MASK_ALL (EBTN_EVT_MASK_ONPRESS | EBTN_EVT_MASK_ONRELEASE | EBTN_EVT_MASK_ONCLICK | EBTN_EVT_MASK_KEEPALIVE)

/**
 * @brief 返回两个绝对时间的差值: time1 - time2
 * @param time1 绝对时间
 * @param time2 绝对时间
 * @return      时间差值
 */
static inline ebtn_time_sign_t ebtn_timer_sub(ebtn_time_t time1, ebtn_time_t time2)
{
    return time1 - time2;
}

// test time overflow error
// #define ebtn_timer_sub(time1, time2) (time1 - time2)

/**
 * @brief 按钮事件回调函数类型
 * @param btn 触发事件的按钮实例指针
 * @param evt 事件类型
 */
typedef void (*ebtn_evt_fn)(struct ebtn_btn *btn, ebtn_evt_t evt);

/**
 * @brief 获取按钮状态回调函数类型
 * @param btn 按钮实例指针
 * @return    当按钮处于活动状态时返回1，否则返回0
 */
typedef uint8_t (*ebtn_get_state_fn)(struct ebtn_btn *btn);

/**
 * \brief           Button Params structure
 */
typedef struct ebtn_btn_param
{
    /**
     * \brief           Minimum debounce time for press event in units of milliseconds
     *
     * This is the time when the input shall have stable active level to detect valid *onpress*
     * event.
     *
     * When value is set to `> 0`, input must be in active state for at least
     * minimum milliseconds time, before valid *onpress* event is detected.
     *
     * \note            If value is set to `0`, debounce is not used and *press* event will be
     * triggered immediately when input states goes to *inactive* state.
     *
     *                  To be safe not using this feature, external logic must ensure stable
     * transition at input level.
     *
     */
    uint16_t time_debounce; /*!< Debounce time in milliseconds */

    /**
     * \brief           Minimum debounce time for release event in units of milliseconds
     *
     * This is the time when the input shall have minimum stable released level to detect valid
     * *onrelease* event.
     *
     * This setting can be useful if application wants to protect against
     * unwanted glitches on the line when input is considered "active".
     *
     * When value is set to `> 0`, input must be in inactive low for at least
     * minimum milliseconds time, before valid *onrelease* event is detected
     *
     * \note            If value is set to `0`, debounce is not used and *release* event will be
     * triggered immediately when input states goes to *inactive* state
     *
     */
    uint16_t time_debounce_release; /*!< Debounce time in milliseconds for release event  */

    /**
     * \brief           Minimum active input time for valid click event, in milliseconds
     *
     * Input shall be in active state (after debounce) at least this amount of time to even consider
     * the potential valid click event. Set the value to `0` to disable this feature
     *
     */
    uint16_t time_click_pressed_min; /*!< Minimum pressed time for valid click event */

    /**
     * \brief           Maximum active input time for valid click event, in milliseconds
     *
     * Input shall be pressed at most this amount of time to still trigger valid click.
     * Set to `-1` to allow any time triggering click event.
     *
     * When input is active for more than the configured time, click even is not detected and is
     * ignored.
     *
     */
    uint16_t time_click_pressed_max; /*!< Maximum pressed time for valid click event*/

    /**
     * \brief           Maximum allowed time between last on-release and next valid on-press,
     *                  to still allow multi-click events, in milliseconds
     *
     * This value is also used as a timeout length to send the *onclick* event to application from
     * previously detected valid click events.
     *
     * If application relies on multi consecutive clicks, this is the max time to allow user
     * to trigger potential new click, or structure will get reset (before sent to user if any
     * clicks have been detected so far)
     *
     */
    uint16_t time_click_multi_max; /*!< Maximum time between 2 clicks to be considered consecutive
                                      click */

    /**
     * \brief           Keep-alive event period, in milliseconds
     *
     * When input is active, keep alive events will be sent through this period of time.
     * First keep alive will be sent after input being considered
     * active.
     *
     */
    uint16_t time_keepalive_period; /*!< Time in ms for periodic keep alive event */

    /**
     * \brief           Maximum number of allowed consecutive click events,
     *                  before structure gets reset to default value.
     *
     * \note            When consecutive value is reached, application will get notification of
     * clicks. This can be executed immediately after last click has been detected, or after
     * standard timeout (unless next on-press has already been detected, then it is send to
     * application just before valid next press event).
     *
     */
    uint16_t max_consecutive; /*!< Max number of consecutive clicks */
} ebtn_btn_param_t;

#define EBTN_PARAMS_INIT(_time_debounce, _time_debounce_release, _time_click_pressed_min, _time_click_pressed_max, _time_click_multi_max,                      \
                         _time_keepalive_period, _max_consecutive)                                                                                             \
    {                                                                                                                                                          \
        .time_debounce = _time_debounce, .time_debounce_release = _time_debounce_release, .time_click_pressed_min = _time_click_pressed_min,                   \
        .time_click_pressed_max = _time_click_pressed_max, .time_click_multi_max = _time_click_multi_max, .time_keepalive_period = _time_keepalive_period,     \
        .max_consecutive = _max_consecutive                                                                                                                    \
    }

#define EBTN_BUTTON_INIT_RAW(_key_id, _param, _mask)                                                                                                           \
    {                                                                                                                                                          \
        .key_id = _key_id, .param = _param, .event_mask = _mask,                                                                                               \
    }

#define EBTN_BUTTON_INIT(_key_id, _param) EBTN_BUTTON_INIT_RAW(_key_id, _param, EBTN_EVT_MASK_ALL)

#define EBTN_BUTTON_DYN_INIT(_key_id, _param)                                                                                                                  \
    {                                                                                                                                                          \
        .next = NULL, .btn = EBTN_BUTTON_INIT(_key_id, _param),                                                                                                \
    }

#define EBTN_BUTTON_COMBO_INIT_RAW(_key_id, _param, _mask)                                                                                                     \
    {                                                                                                                                                          \
        .comb_key = {0}, .btn = EBTN_BUTTON_INIT_RAW(_key_id, _param, _mask),                                                                                  \
    }

#define EBTN_BUTTON_COMBO_INIT(_key_id, _param)                                                                                                                \
    {                                                                                                                                                          \
        .comb_key = {0}, .btn = EBTN_BUTTON_INIT(_key_id, _param),                                                                                             \
    }

#define EBTN_BUTTON_COMBO_DYN_INIT(_key_id, _param)                                                                                                            \
    {                                                                                                                                                          \
        .next = NULL, .btn = EBTN_BUTTON_COMBO_INIT(_key_id, _param),                                                                                          \
    }

#define EBTN_ARRAY_SIZE(_arr) sizeof(_arr) / sizeof((_arr)[0])

/**
 * @brief 按钮结构体
 */
typedef struct ebtn_btn
{
    uint16_t key_id;    /*!< User defined custom argument for callback function purpose */
    uint8_t flags;      /*!< Private button flags management */
    uint8_t event_mask; /*!< Private button event mask management */

    ebtn_time_t time_change;       /*!< Time in ms when button state got changed last time after valid
                                   debounce */
    ebtn_time_t time_state_change; /*!< Time in ms when button state got changed last time */

    ebtn_time_t keepalive_last_time; /*!< Time in ms of last send keep alive event */
    ebtn_time_t click_last_time;     /*!< Time in ms of last successfully detected (not sent!) click event
                                      */

    uint16_t keepalive_cnt; /*!< Number of keep alive events sent after successful on-press
                            detection. Value is reset after on-release */
    uint16_t click_cnt;     /*!< Number of consecutive clicks detected, respecting maximum timeout
                        between clicks */

    const ebtn_btn_param_t *param;
} ebtn_btn_t;

/**
 * @brief 组合按钮结构体
 */
typedef struct ebtn_btn_combo
{
    BIT_ARRAY_DEFINE(comb_key, EBTN_MAX_KEYNUM); /*!< select key index - `1` means active, `0` means inactive */

    ebtn_btn_t btn;
} ebtn_btn_combo_t;

/**
 * @brief 动态按钮结构体
 */
typedef struct ebtn_btn_dyn
{
    struct ebtn_btn_dyn *next; /*!< point to next button */

    ebtn_btn_t btn;
} ebtn_btn_dyn_t;

/**
 * @brief 动态组合按钮结构体
 */
typedef struct ebtn_btn_combo_dyn
{
    struct ebtn_btn_combo_dyn *next; /*!< point to next combo-button */

    ebtn_btn_combo_t btn;
} ebtn_btn_combo_dyn_t;

/**
 * @brief easy_button控制组结构体
 */
typedef struct ebtn
{
    ebtn_btn_t *btns;             /*!< Pointer to buttons array */
    uint16_t btns_cnt;            /*!< Number of buttons in array */
    ebtn_btn_combo_t *btns_combo; /*!< Pointer to comb-buttons array */
    uint16_t btns_combo_cnt;      /*!< Number of comb-buttons in array */

    ebtn_btn_dyn_t *btn_dyn_head;             /*!< Pointer to btn-dynamic list */
    ebtn_btn_combo_dyn_t *btn_combo_dyn_head; /*!< Pointer to btn-combo-dynamic list */

    ebtn_evt_fn evt_fn;             /*!< Pointer to event function */
    ebtn_get_state_fn get_state_fn; /*!< Pointer to get state function */

    BIT_ARRAY_DEFINE(old_state, EBTN_MAX_KEYNUM); /*!< Old button state - `1` means active, `0` means inactive */
} ebtn_t;

/**
 * @brief 按钮处理函数，读取输入并执行相应动作
 * @param mstime 当前毫秒系统时间
 */
void ebtn_process(ebtn_time_t mstime);

/**
 * @brief 带有所有按钮输入状态的按钮处理函数
 * @param curr_state 包含所有按钮当前状态的位数组
 * @param mstime     当前毫秒系统时间
 */
void ebtn_process_with_curr_state(bit_array_t *curr_state, ebtn_time_t mstime);

/**
 * @brief 检查按钮是否处于活动状态
 * @param btn 要检查的按钮句柄
 * @return    如果处于活动状态则返回1，否则返回0
 */
int ebtn_is_btn_active(const ebtn_btn_t *btn);

/**
 * @brief 检查按钮是否处于处理过程中
 * @param btn 要检查的按钮句柄
 * @return    如果处于处理过程中则返回1，否则返回0
 */
int ebtn_is_btn_in_process(const ebtn_btn_t *btn);

/**
 * @brief 检查是否有按钮处于处理过程中
 * @return 如果有处于处理过程中的按钮则返回1，否则返回0
 */
int ebtn_is_in_process(void);

/**
 * @brief 初始化按钮管理器
 * @param btns           要处理的按钮数组
 * @param btns_cnt       要处理的按钮数量
 * @param btns_combo     要处理的组合按钮数组
 * @param btns_combo_cnt 要处理的组合按钮数量
 * @param get_state_fn   提供按需读取按钮状态功能的函数指针
 * @param evt_fn         按钮事件回调函数
 *
 * @return               成功返回 `1`，否则返回 `0`
 */
int ebtn_init(ebtn_btn_t *btns, uint16_t btns_cnt, ebtn_btn_combo_t *btns_combo, uint16_t btns_combo_cnt, ebtn_get_state_fn get_state_fn, ebtn_evt_fn evt_fn);

/**
 * @brief 注册一个动态按钮
 *
 * @param button: 动态按钮结构体实例
 * @return        成功返回 `1`，否则返回 `0`
 */
int ebtn_register(ebtn_btn_dyn_t *button);

/**
 * @brief 注册一个动态组合按钮
 * @param button: 动态组合按钮结构体实例
 *
 * @return        成功返回 `1`，否则返回 `0`
 */
int ebtn_combo_register(ebtn_btn_combo_dyn_t *button);

/**
 * @brief 获取当前按钮的总数
 *
 * @return 按钮的数量
 */
int ebtn_get_total_btn_cnt(void);

/**
 * @brief 通过 key_id 获取内部 key_idx
 * @param key_id key_id
 *
 * @return 错误返回 '-1'，否则返回 key_idx
 */
int ebtn_get_btn_index_by_key_id(uint16_t key_id);

/**
 * @brief 通过 key_id 获取内部 btn 实例，这里是按钮实例，动态注册的也是获取其按钮实例
 *
 * @param key_id key_id
 *
 * @return 错误返回 'NULL'，否则返回按钮实例
 */
ebtn_btn_t *ebtn_get_btn_by_key_id(uint16_t key_id);

/**
 * @brief 获取按钮的内部 key_idx
 * @param btn 按钮
 *
 * @return 错误返回 '-1'，否则返回 key_idx
 */
int ebtn_get_btn_index_by_btn(ebtn_btn_t *btn);

/**
 * @brief 获取动态按钮的内部 key_idx
 * @param btn 按钮
 *
 * @return 错误返回 '-1'，否则返回 key_idx
 */
int ebtn_get_btn_index_by_btn_dyn(ebtn_btn_dyn_t *btn);

/**
 * @brief 将组合按钮按键与 key_idx 绑定
 * @param btn 组合按钮
 * @param idx key_idx
 *
 */
void ebtn_combo_btn_add_btn_by_idx(ebtn_btn_combo_t *btn, int idx);

/**
 * @brief 移除组合按钮按键与 key_idx 的绑定
 * @param btn 组合按钮
 * @param idx key_idx
 *
 */
void ebtn_combo_btn_remove_btn_by_idx(ebtn_btn_combo_t *btn, int idx);

/**
 * @brief 将组合按钮按键与 key_id 绑定，确保 key_id(按钮) 已经注册
 * @param btn 组合按钮
 * @param key_id key_id
 *
 */
void ebtn_combo_btn_add_btn(ebtn_btn_combo_t *btn, uint16_t key_id);

/**
 * @brief 移除组合按钮按键与 key_id 的绑定，确保 key_id(按钮) 已经注册
 * @param btn 组合按钮
 * @param key_id key_id
 *
 */
void ebtn_combo_btn_remove_btn(ebtn_btn_combo_t *btn, uint16_t key_id);

/**
 * @brief 获取特定按钮的长按保持周期
 * @param btn 要获取长按保持周期的按钮实例
 * @return    长按保持周期，单位为 `ms`
 */
#define ebtn_keepalive_get_period(btn) ((btn)->time_keepalive_period)

/**
 * @brief 获取自上次按下事件以来的实际长按保持计数。
 *        如果按钮未按下，则设置为 `0`
 * @param btn 要获取长按保持周期的按钮实例
 * @return    自按下事件以来的长按保持事件数量
 */
#define ebtn_keepalive_get_count(btn) ((btn)->keepalive_cnt)

/**
 * @brief 获取特定所需时间(以毫秒为单位)的长按保持计数。
 *        它将计算在达到请求时间之前，特定按钮应产生的长按保持(keepalive)滴答数。
 *
 * 该函数的结果可与 \ref ebtn_keepalive_get_count 一起使用，后者返回
 * 自按钮最后一次按下事件以来的实际长按保持计数。
 *
 * \note            该值始终为整数对齐，粒度为一个长按保持时间周期
 * \note            实现为宏，以便在使用静态长按保持时可由编译器优化
 *
 * @param btn       用于检查的按钮
 * @param ms_time   计算长按保持计数的时间(ms)
 * @return          长按保持计数的数量
 */
#define ebtn_keepalive_get_count_for_time(btn, ms_time) ((ms_time) / ebtn_keepalive_get_period(btn))

/**
 * @brief 获取按钮上连续点击事件的数量
 * @param btn 要获取点击数量的按钮实例
 * @return    按钮上连续点击的数量
 */
#define ebtn_click_get_count(btn) ((btn)->click_cnt)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _EBTN_H */
