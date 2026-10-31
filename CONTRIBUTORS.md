# Contributors

## 主要作者

| 名字 | GitHub | 说明 |
|---|---|---|
| LaoMoQAQ | [@LaoMoQAQ](https://github.com/LaoMoQAQ) | 项目作者，剧本、引擎、UI 全部代码 |

> git 历史里 `LaoMoQAQ` 和 `LaoMo` 是同一个人的两个署名（同一邮箱），
> 统计时按一人计算。

## 关于 GitHub 侧栏的 Contributors 列表

GitHub 仓库首页右侧的 Contributors 是**自动生成的**，规则是：
统计仓库里所有 commit 的作者和共同作者，按提交数排序。

**这个列表没法通过改仓库里的文件来增删**。要调整只能：

1. 改写 git 历史（`git filter-branch` / `git filter-repo`），
   把不想出现的作者身份合并或移除——会重写所有 commit hash，
   已经发出的 Release 和 tag 都会失效，**不推荐**
2. 或者在 `.mailmap` 里把多个署名映射成同一个人
   （只能合并，不能删除）
3. 或者接受现状

本仓库没有第三方 bot 的提交记录（已核对 `git log --format="%an <%ae>"`），
所以不需要额外处理。

## 致谢

- SDL2 / SDL2_image / SDL2_ttf / SDL2_mixer —— 图形与音频
- 思源黑体 / 站酷字体 —— 中文字体
- 各 AI 生图模型（SD1.5 / MeinaMix / Counterfeit / DreamShaper /
  Realistic Vision）—— 立绘探索阶段使用，最终未采用
