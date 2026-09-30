#ifndef TEXT_SYSTEM_H
#define TEXT_SYSTEM_H

#include <string>
#include <vector>

class TextSystem
{
public:

    TextSystem();

    void Init();

    void SetText(
        const std::string& text,
        float wait = 0.0f
    );

    void Update();

    void Skip();

    bool Finished();

    bool IsFinished();

    std::string GetCurrentText();

    std::string GetText();

    // ==========================================================
    // [新增] 设置每秒显示多少字
    // ==========================================================
    //
    // 与 Config 的 textSpeed 对应。
    // 内部换算成每字间隔秒数 = 1 / cps。

    void SetCharsPerSecond(int cps);

    // ==========================================================

private:

    void SplitUTF8();

    float GetPunctuationDelay(
        const std::string& ch
    );

private:

    std::string fullText;

    std::string displayText;

    std::vector<std::string> characters;

    int currentIndex;

    float timer;

    float speed;

    float waitTimer;

    float waitTarget;

    float punctuationTimer;

    float punctuationTarget;

    bool finished;
};

#endif