#ifndef TEXT_SYSTEM_H
#define TEXT_SYSTEM_H

#include <string>
#include <vector>

class TextSystem
{
public:

    TextSystem();

    void Init();

    // 设置文本
    void SetText(
        const std::string& text,
        float wait = 0.0f
    );

    // 更新
    void Update();

    // 快进
    void Skip();

    // 是否结束
    bool Finished();

    bool IsFinished();

    // 当前显示文字
    std::string GetCurrentText();

    std::string GetText();

private:

    // UTF-8 切分
    void SplitUTF8();

    // 标点额外停顿
    float GetPunctuationDelay(
        const std::string& ch
    );

private:

    // 原始文本
    std::string fullText;

    // 当前显示
    std::string displayText;

    // UTF-8 字符数组
    std::vector<std::string> characters;

    // 当前字符
    int currentIndex;

    // 每字计时
    float timer;

    // 每字速度
    float speed;

    // 剧本等待
    float waitTimer;

    float waitTarget;

    // 标点等待
    float punctuationTimer;

    float punctuationTarget;

    // 是否结束
    bool finished;
};

#endif