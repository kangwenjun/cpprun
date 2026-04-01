void bin2hex(const char *src, int len, char *output)
{
    // 输入校验：输入或输出指针为空，或长度非正数，则输出空字符串并返回
    if (src == nullptr || output == nullptr || len <= 0)
    {
        if (output != nullptr)
            output[0] = '\0';
        return;
    }

    /*
     * 功能：
     *   将按字符表示的位流（每个字符为 '0' 或 '1'）转换为十六进制字符串。
     *
     * 行为约定：
     *   - 输入被视为位流（bit stream），按从左到右的顺序读取，src[0] 为先发位（高位优先）。
     *   - 每 4 个连续位（nibble）组合成 1 个十六进制字符，组合顺序为高位在前。
     *   - 若总位数不是 4 的倍数，**在输入头部补 0**（高位补零）后再按 4 位一组转换。
     *     例如："101" -> 头部补为 "0101" -> 0x5；"11" -> 头部补为 "0011" -> 0x3。
     *
     * 实现说明：
     *   - 计算需要在头部补的零数 pad = (4 - (len % 4)) % 4，然后把原始位流视作从索引 -pad 开始，
     *     对每个目标 nibble 的 4 个位置分别判断是否落在原始输入范围内，超出范围视为 '0'。
     *   - 非 '1' 字符视为 0（仅判断 (src[idx] == '1')）。
     *
     * 输出与安全：
     *   - 输出缓冲区 output 必须由调用方准备且大小至少为 ((len + 3) / 4 + 1) 字节，
     *     以容纳转换结果并追加终止符 '\0'。
     */

    static const char hex_digits[] = "0123456789abcdef";

    int pad = (4 - (len % 4)) % 4; // 需要在头部补的零数（0..3）
    int total_bits = len + pad;
    int nibbles = total_bits / 4;

    for (int i = 0; i < nibbles; ++i)
    {
        int x = 0;
        // 对每个 nibble 的 4 位（高位在前）进行采样：
        for (int k = 0; k < 4; ++k)
        {
            // 源索引相对于原始 src 的位置：
            // 目标位的全局索引为 i*4 + k，相应的原始索引为 (i*4 + k - pad)
            int src_idx = i * 4 + k - pad;
            int bit = 0;
            if (src_idx >= 0 && src_idx < len)
                bit = (src[src_idx] == '1') ? 1 : 0;
            x = (x << 1) | bit;
        }
        output[i] = hex_digits[x & 0x0F];
    }

    // 终止符
    output[nibbles] = '\0';
}