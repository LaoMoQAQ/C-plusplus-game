#ifndef TEXT_SYSTEM_H
#define TEXT_SYSTEM_H

#include <string>
#include <vector>


// ==========================================================
// TextSystem
// ==========================================================
//
// 打字机。逐字显示文本，支持标点停顿、等待时间、跳过。
//
// 数据流：
//   SetText(text)         拆分 UTF-8 字符，重置状态
//   Update()              每帧推进，把字符加进 displayText
//   GetCurrentText()      取当前已显示的文本
//   Finished()            是否显示完毕
//   Skip()                立即全部显示
//
// 额外处理：
//   - '…' 替换成 '.'（字体缺字形）
//   - 标点符号后停顿（见 GetPunctuationDelay）

class TextSystem
{

public:

    TextSystem();

    void Init();



    // 设置文本。wait 是开始逐字显示前的等待秒数。
    void SetText(
        const std::string& text,
        float wait = 0.0f
    );



    // 每帧调用，推进打字机。
    void Update();

    // 立即显示全部文本。
    void Skip();



    // 是否显示完毕。
    bool Finished();
    bool IsFinished();



    // 当前已显示的文本（打字过程中逐渐变长）。
    std::string GetCurrentText();
    std::string GetText();



    // 设置每秒显示多少字。
    // 与 Config 的 textSpeed 对应。
    void SetCharsPerSecond(int cps);



private:

    // 把 fullText 按 UTF-8 字符切分成 characters。
    void SplitUTF8();

    // 返回字符对应的额外停顿秒数（标点用）。
    float GetPunctuationDelay(
        const std::string& ch
    );



private:

    std::string fullText;      // 原始文本
    std::string displayText;   // 当前已显示

    std::vector<std::string> characters;  // 切分后的字符
    int currentIndex;                     // 已显示到第几个

    float timer;              // 每字计时
    float speed;              // 每字间隔（秒）

    float waitTimer;          // SetText 的 wait 计时
    float waitTarget;

    float punctuationTimer;   // 标点额外停顿计时
    float punctuationTarget;

    bool finished;            // 是否显示完毕

};

#endif