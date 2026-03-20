#ifndef _BIT_ARRAY_H_
#define _BIT_ARRAY_H_

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// #define BIT_ARRAY_CONFIG_64

// 如果您的系统是 64 位，可以在这里更改为 uint64_t。
#ifdef BIT_ARRAY_CONFIG_64
typedef uint64_t bit_array_t;
#define BIT_ARRAY_BIT(n) (1ULL << (n))
#else
typedef uint32_t bit_array_t;
#define BIT_ARRAY_BIT(n) (1UL << (n))
#endif
typedef bit_array_t bit_array_val_t;

#define BIT_ARRAY_BITS (sizeof(bit_array_val_t) * 8)

#define BIT_ARRAY_BIT_WORD(bit)  ((bit) / BIT_ARRAY_BITS)
#define BIT_ARRAY_BIT_INDEX(bit) ((bit_array_val_t)(bit) & (BIT_ARRAY_BITS - 1U))

#define BIT_ARRAY_MASK(bit)       BIT_ARRAY_BIT(BIT_ARRAY_BIT_INDEX(bit))
#define BIT_ARRAY_ELEM(addr, bit) ((addr)[BIT_ARRAY_BIT_WORD(bit)])

// 全1的字(word)
#define BIT_ARRAY_WORD_MAX (~(bit_array_val_t)0)

#define BIT_ARRAY_SUB_MASK(nbits) ((nbits) ? BIT_ARRAY_WORD_MAX >> (BIT_ARRAY_BITS - (nbits)) : (bit_array_val_t)0)

// 一种可能更快地使用掩码组合两个字的方法
// #define bitmask_merge(a,b,abits) ((a & abits) | (b & ~abits))
#define bitmask_merge(a, b, abits) (b ^ ((a ^ b) & abits))

/**
 * @brief 此宏计算表示具有 @a num_bits 位的位图所需的位数组变量的数量。
 *
 * @param num_bits 位数。
 */
#define BIT_ARRAY_BITMAP_SIZE(num_bits) (1 + ((num_bits)-1) / BIT_ARRAY_BITS)

/**
 * @brief 定义一个位数组变量数组。
 *
 * 此宏定义一个至少包含 @a num_bits 位的位数组变量数组。
 *
 * @note
 * 如果在文件作用域使用，数组的位将被初始化为零；
 * 如果在函数内部使用，位将保持未初始化状态。
 *
 * @cond INTERNAL_HIDDEN
 * @note
 * 此宏应复制到文档 Doxyfile 的 PREDEFINED 字段中。
 * @endcond
 *
 * @param name 位数组变量数组的名称。
 * @param num_bits 所需的位数。
 */
#define BIT_ARRAY_DEFINE(name, num_bits) bit_array_t name[BIT_ARRAY_BITMAP_SIZE(num_bits)]

#if 1
// See http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
static inline bit_array_val_t _windows_popcount(bit_array_val_t w)
{
    w = w - ((w >> 1) & (bit_array_val_t) ~(bit_array_val_t)0 / 3);
    w = (w & (bit_array_val_t) ~(bit_array_val_t)0 / 15 * 3) + ((w >> 2) & (bit_array_val_t) ~(bit_array_val_t)0 / 15 * 3);
    w = (w + (w >> 4)) & (bit_array_val_t) ~(bit_array_val_t)0 / 255 * 15;
    return (bit_array_val_t)(w * ((bit_array_val_t) ~(bit_array_val_t)0 / 255)) >> (sizeof(bit_array_val_t) - 1) * 8;
}

#define POPCOUNT(x) _windows_popcount(x)
#else
#define POPCOUNT(x) (unsigned)__builtin_popcountll(x)
#endif

#define bits_in_top_word(nbits) ((nbits) ? BIT_ARRAY_BIT_INDEX((nbits)-1) + 1 : 0)

static inline void _bit_array_mask_top_word(bit_array_t *target, int num_bits)
{
    // 掩码最高字
    int num_of_words = BIT_ARRAY_BITMAP_SIZE(num_bits);
    int bits_active = bits_in_top_word(num_bits);
    target[num_of_words - 1] &= BIT_ARRAY_SUB_MASK(bits_active);
}

/**
 * @brief 位数组测试某一位。
 *
 * 此例程测试 @a target 的第 @a bit 位是否被设置。
 *
 * @param target 位数组变量或数组的地址。
 * @param bit 位数（从 0 开始）。
 *
 * @return 如果该位被设置则返回 true(1)，否则返回 false(0)。
 */
static inline int bit_array_get(const bit_array_t *target, int bit)
{
    bit_array_val_t val = BIT_ARRAY_ELEM(target, bit);

    return (1 & (val >> (bit & (BIT_ARRAY_BITS - 1)))) != 0;
}

/**
 * @brief 位数组清除某一位。
 *
 * 位数组清除 @a target 的第 @a bit 位。
 *
 * @param target 位数组变量或数组的地址。
 * @param bit 位数（从 0 开始）。
 */
static inline void bit_array_clear(bit_array_t *target, int bit)
{
    bit_array_val_t mask = BIT_ARRAY_MASK(bit);

    BIT_ARRAY_ELEM(target, bit) &= ~mask;
}

/**
 * @brief 位数组设置某一位。
 *
 * 位数组设置 @a target 的第 @a bit 位。
 *
 * @param target 位数组变量或数组的地址。
 * @param bit 位数（从 0 开始）。
 */
static inline void bit_array_set(bit_array_t *target, int bit)
{
    bit_array_val_t mask = BIT_ARRAY_MASK(bit);

    BIT_ARRAY_ELEM(target, bit) |= mask;
}

/**
 * @brief 位数组翻转某一位。
 *
 * 位数组翻转 @a target 的第 @a bit 位。
 *
 * @param target 位数组变量或数组的地址。
 * @param bit 位数（从 0 开始）。
 */
static inline void bit_array_toggle(bit_array_t *target, int bit)
{
    bit_array_val_t mask = BIT_ARRAY_MASK(bit);

    BIT_ARRAY_ELEM(target, bit) ^= mask;
}

/**
 * @brief 位数组将某一位设置为给定值。
 *
 * 位数组将 @a target 的第 @a bit 位设置为值 @a val。
 *
 * @param target 位数组变量或数组的地址。
 * @param bit 位数（从 0 开始）。
 * @param val true表示1，false表示0。
 */
static inline void bit_array_assign(bit_array_t *target, int bit, int val)
{
    bit_array_val_t mask = BIT_ARRAY_MASK(bit);

    if (val)
    {
        BIT_ARRAY_ELEM(target, bit) |= mask;
    }
    else
    {
        BIT_ARRAY_ELEM(target, bit) &= ~mask;
    }
}

static inline void bit_array_clear_all(bit_array_t *target, int num_bits)
{
    memset((void *)target, 0, BIT_ARRAY_BITMAP_SIZE(num_bits) * sizeof(bit_array_val_t));
}

static inline void bit_array_set_all(bit_array_t *target, int num_bits)
{
    memset((void *)target, 0xff, BIT_ARRAY_BITMAP_SIZE(num_bits) * sizeof(bit_array_val_t));
    _bit_array_mask_top_word(target, num_bits);
}

static inline void bit_array_toggle_all(bit_array_t *target, int num_bits)
{
    for (int i = 0; i < BIT_ARRAY_BITMAP_SIZE(num_bits); i++)
    {
        target[i] ^= BIT_ARRAY_WORD_MAX;
    }
    _bit_array_mask_top_word(target, num_bits);
}

//
// 字符串和打印
//

// 从具有指定开启和关闭字符的子字符串构造 BIT_ARRAY。

// 从字符串方法
static inline void bit_array_from_str(bit_array_t *bitarr, const char *str)
{
    int i, index;
    int space = 0;
    int len = strlen(str);

    for (i = 0; i < len; i++)
    {
        index = i - space;
        if (strchr("1", str[i]) != NULL)
        {
            bit_array_set(bitarr, index);
        }
        else if (strchr("0", str[i]) != NULL)
        {
            bit_array_clear(bitarr, index);
        }
        else
        {
            // error.
            space++;
        }
    }
}

// 接收一个字符数组进行写入。 `str` 的长度必须是 bitarr->num_of_bits+1
// 以 '\0' 终止字符串
static inline char *bit_array_to_str(const bit_array_t *bitarr, int num_bits, char *str)
{
    int i;

    for (i = 0; i < num_bits; i++)
    {
        str[i] = bit_array_get(bitarr, i) ? '1' : '0';
    }

    str[num_bits] = '\0';

    return str;
}

// 接收一个字符数组进行写入。 `str` 的长度必须是 bitarr->num_of_bits+1
// 以 '\0' 终止字符串
static inline char *bit_array_to_str_8(const bit_array_t *bitarr, int num_bits, char *str)
{
    int i;
    int space = 0;

    for (i = 0; i < num_bits; i++)
    {
        str[i + space] = bit_array_get(bitarr, i) ? '1' : '0';

        if ((i + 1) % 8 == 0)
        {
            space++;
            str[i + space] = ' ';
        }
    }

    str[num_bits + space] = '\0';

    return str;
}

//
// 获取和设置字（仅供内部使用 -- 无边界检查）
//

static inline bit_array_val_t _bit_array_get_word(const bit_array_t *target, int num_bits, int start)
{
    int word_index = BIT_ARRAY_BIT_WORD(start);
    int word_offset = BIT_ARRAY_BIT_INDEX(start);

    bit_array_val_t result = target[word_index] >> word_offset;

    int bits_taken = BIT_ARRAY_BITS - word_offset;

    // word_offset 现在是我们需要的下一个字的位数
    // 检查下一个字是否至少有一些位
    if (word_offset > 0 && start + bits_taken < num_bits)
    {
        result |= target[word_index + 1] << (BIT_ARRAY_BITS - word_offset);
    }

    return result;
}

// 从特定起始位置设置 64 位
// 不扩展位数组
static inline void _bit_array_set_word(bit_array_t *target, int num_bits, int start, bit_array_val_t word)
{
    int word_index = BIT_ARRAY_BIT_WORD(start);
    int word_offset = BIT_ARRAY_BIT_INDEX(start);

    if (word_offset == 0)
    {
        target[word_index] = word;
    }
    else
    {
        target[word_index] = (word << word_offset) | (target[word_index] & BIT_ARRAY_SUB_MASK(word_offset));

        if (word_index + 1 < BIT_ARRAY_BITMAP_SIZE(num_bits))
        {
            target[word_index + 1] = (word >> (BIT_ARRAY_BITS - word_offset)) | (target[word_index + 1] & (BIT_ARRAY_WORD_MAX << word_offset));
        }
    }

    // 掩码最高字
    _bit_array_mask_top_word(target, num_bits);
}

//
// 填充区域（仅供内部使用）
//

// FillAction 是填充 0 或 1 或翻转
typedef enum
{
    ZERO_REGION,
    FILL_REGION,
    SWAP_REGION
} FillAction;

static inline void _bit_array_set_region(bit_array_t *target, int start, int length, FillAction action)
{
    if (length == 0)
        return;

    int first_word = BIT_ARRAY_BIT_WORD(start);
    int last_word = BIT_ARRAY_BIT_WORD(start + length - 1);
    int foffset = BIT_ARRAY_BIT_INDEX(start);
    int loffset = BIT_ARRAY_BIT_INDEX(start + length - 1);

    if (first_word == last_word)
    {
        bit_array_val_t mask = BIT_ARRAY_SUB_MASK(length) << foffset;

        switch (action)
        {
        case ZERO_REGION:
            target[first_word] &= ~mask;
            break;
        case FILL_REGION:
            target[first_word] |= mask;
            break;
        case SWAP_REGION:
            target[first_word] ^= mask;
            break;
        }
    }
    else
    {
        // 设置第一个字
        switch (action)
        {
        case ZERO_REGION:
            target[first_word] &= BIT_ARRAY_SUB_MASK(foffset);
            break;
        case FILL_REGION:
            target[first_word] |= ~BIT_ARRAY_SUB_MASK(foffset);
            break;
        case SWAP_REGION:
            target[first_word] ^= ~BIT_ARRAY_SUB_MASK(foffset);
            break;
        }

        int i;

        // 设置完整字
        switch (action)
        {
        case ZERO_REGION:
            for (i = first_word + 1; i < last_word; i++)
                target[i] = (bit_array_val_t)0;
            break;
        case FILL_REGION:
            for (i = first_word + 1; i < last_word; i++)
                target[i] = BIT_ARRAY_WORD_MAX;
            break;
        case SWAP_REGION:
            for (i = first_word + 1; i < last_word; i++)
                target[i] ^= BIT_ARRAY_WORD_MAX;
            break;
        }

        // 设置最后一个字
        switch (action)
        {
        case ZERO_REGION:
            target[last_word] &= ~BIT_ARRAY_SUB_MASK(loffset + 1);
            break;
        case FILL_REGION:
            target[last_word] |= BIT_ARRAY_SUB_MASK(loffset + 1);
            break;
        case SWAP_REGION:
            target[last_word] ^= BIT_ARRAY_SUB_MASK(loffset + 1);
            break;
        }
    }
}

// 获取已设置的位数（汉明重量）
static inline int bit_array_num_bits_set(bit_array_t *target, int num_bits)
{
    int i;

    int num_of_bits_set = 0;

    for (i = 0; i < BIT_ARRAY_BITMAP_SIZE(num_bits); i++)
    {
        if (target[i] > 0)
        {
            num_of_bits_set += POPCOUNT(target[i]);
        }
    }

    return num_of_bits_set;
}

// 获取未设置的位数（1 - 汉明重量）
static inline int bit_array_num_bits_cleared(bit_array_t *target, int num_bits)
{
    return num_bits - bit_array_num_bits_set(target, num_bits);
}

// 将位从一个数组复制到另一个数组
// 注意：使用宏 bit_array_copy
// 目标和源可以是同一个位数组并且
// src/dst 区域可以重叠
static inline void bit_array_copy(bit_array_t *dst, int dstindx, const bit_array_t *src, int srcindx, int length, int src_num_bits, int dst_num_bits)
{
    // 要复制的完整字数
    int num_of_full_words = length / BIT_ARRAY_BITS;
    int i;

    int bits_in_last_word = bits_in_top_word(length);

    if (dst == src && srcindx > dstindx)
    {
        // 从左到右工作
        for (i = 0; i < num_of_full_words; i++)
        {
            bit_array_val_t word = _bit_array_get_word(src, src_num_bits, srcindx + i * BIT_ARRAY_BITS);
            _bit_array_set_word(dst, dst_num_bits, dstindx + i * BIT_ARRAY_BITS, word);
        }

        if (bits_in_last_word > 0)
        {
            bit_array_val_t src_word = _bit_array_get_word(src, src_num_bits, srcindx + i * BIT_ARRAY_BITS);
            bit_array_val_t dst_word = _bit_array_get_word(dst, dst_num_bits, dstindx + i * BIT_ARRAY_BITS);

            bit_array_val_t mask = BIT_ARRAY_SUB_MASK(bits_in_last_word);
            bit_array_val_t word = bitmask_merge(src_word, dst_word, mask);

            _bit_array_set_word(dst, dst_num_bits, dstindx + num_of_full_words * BIT_ARRAY_BITS, word);
        }
    }
    else
    {
        // 从右到左工作
        for (i = 0; i < num_of_full_words; i++)
        {
            bit_array_val_t word = _bit_array_get_word(src, src_num_bits, srcindx + length - (i + 1) * BIT_ARRAY_BITS);
            _bit_array_set_word(dst, dst_num_bits, dstindx + length - (i + 1) * BIT_ARRAY_BITS, word);
        }

        if (bits_in_last_word > 0)
        {
            bit_array_val_t src_word = _bit_array_get_word(src, src_num_bits, srcindx);
            bit_array_val_t dst_word = _bit_array_get_word(dst, dst_num_bits, dstindx);

            bit_array_val_t mask = BIT_ARRAY_SUB_MASK(bits_in_last_word);
            bit_array_val_t word = bitmask_merge(src_word, dst_word, mask);
            _bit_array_set_word(dst, dst_num_bits, dstindx, word);
        }
    }

    _bit_array_mask_top_word(dst, dst_num_bits);
}

// 将 src 的所有内容复制到 dst。dst 调整大小以匹配 src。
static inline void bit_array_copy_all(bit_array_t *dst, const bit_array_t *src, int num_bits)
{
    for (int i = 0; i < BIT_ARRAY_BITMAP_SIZE(num_bits); i++)
    {
        dst[i] = src[i];
    }
}

//
// 逻辑运算符
//

// 目标可以是与源之一或两个源相同的数组
static inline void bit_array_and(bit_array_t *dest, const bit_array_t *src1, const bit_array_t *src2, int num_bits)
{
    for (int i = 0; i < BIT_ARRAY_BITMAP_SIZE(num_bits); i++)
    {
        dest[i] = src1[i] & src2[i];
    }
}

static inline void bit_array_or(bit_array_t *dest, const bit_array_t *src1, const bit_array_t *src2, int num_bits)
{
    for (int i = 0; i < BIT_ARRAY_BITMAP_SIZE(num_bits); i++)
    {
        dest[i] = src1[i] | src2[i];
    }
}

static inline void bit_array_xor(bit_array_t *dest, const bit_array_t *src1, const bit_array_t *src2, int num_bits)
{
    for (int i = 0; i < BIT_ARRAY_BITMAP_SIZE(num_bits); i++)
    {
        dest[i] = src1[i] ^ src2[i];
    }
}

static inline void bit_array_not(bit_array_t *dest, const bit_array_t *src, int num_bits)
{
    for (int i = 0; i < BIT_ARRAY_BITMAP_SIZE(num_bits); i++)
    {
        dest[i] = ~src[i];
    }
}

//
// 向左/向右移动数组。 如果 fill 为零，则填充 0，否则填充 1
//

// 移向 LSB（最低有效位） / 较低索引
static inline void bit_array_shift_right(bit_array_t *target, int num_bits, int shift_dist, int fill)
{
    if (shift_dist >= num_bits)
    {
        fill ? bit_array_set_all(target, num_bits) : bit_array_clear_all(target, num_bits);
        return;
    }
    else if (shift_dist == 0)
    {
        return;
    }

    FillAction action = fill ? FILL_REGION : ZERO_REGION;

    int cpy_length = num_bits - shift_dist;
    bit_array_copy(target, 0, target, shift_dist, cpy_length, num_bits, num_bits);

    _bit_array_set_region(target, cpy_length, shift_dist, action);
}

// 移向 MSB（最高有效位） / 较高索引
static inline void bit_array_shift_left(bit_array_t *target, int num_bits, int shift_dist, int fill)
{
    if (shift_dist >= num_bits)
    {
        fill ? bit_array_set_all(target, num_bits) : bit_array_clear_all(target, num_bits);
        return;
    }
    else if (shift_dist == 0)
    {
        return;
    }

    FillAction action = fill ? FILL_REGION : ZERO_REGION;

    int cpy_length = num_bits - shift_dist;
    bit_array_copy(target, shift_dist, target, 0, cpy_length, num_bits, num_bits);
    _bit_array_set_region(target, 0, shift_dist, action);
}

//
// 比较
//

// 比较两个位数组存储的值，索引 0 为最低有效位 (LSB)。
// 数组必须具有相同的长度。
// 返回值：
//  >0 当且仅当 bitarr1 > bitarr2
//   0 当且仅当 bitarr1 == bitarr2
//  <0 当且仅当 bitarr1 < bitarr2
static inline int bit_array_cmp(const bit_array_t *bitarr1, const bit_array_t *bitarr2, int num_bits)
{
    return memcmp(bitarr1, bitarr2, BIT_ARRAY_BITMAP_SIZE(num_bits) * sizeof(bit_array_val_t));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BIT_ARRAY_H_ */
