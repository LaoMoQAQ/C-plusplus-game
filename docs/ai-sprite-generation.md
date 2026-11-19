# 用 AI 生成立绘

这是开发过程中的踩坑记录，不是使用本项目的必要阅读内容。

工具链在 `E:\fouryears workspace\imagegen\`，完整文档见该目录的 `README.md`。

## 结论先放前面

**能稳定出图，但产出达不到本项目要求。**

已做到：

- ComfyUI + DirectML 连续出图不 OOM（38~42 秒/张）
- ControlNet OpenPose 控制全身站姿
- 泛洪填充 / PS 魔棒抠图
- 多 seed 批量 + 自动评分筛选

做不到：

- 稳定出「白底 + 全身 + 无装饰 + 指定服装」
- 构图随机（半身 / 全身 / 侧身都可能）
- 服装上会印文字
- 分辨率上限 512x768

## 根因：DirectML 不支持 LoRA

问题不在显存，在 **DirectML 不支持 LoRA**。

PyTorch 被 `torch-directml` 锁在 2.4.1，而 `peft` 和 ComfyUI 的 LoRA 加载器都要求 2.5+。
要稳定出立绘，LoRA 几乎是必需的——没有它就无法把「白底全身、指定服装、无装饰」这些约束固定下来。

## 底模选择

试过的底模里只有 **Realistic Vision V5.1** 画风正确（写实照片风），
其余（SD 1.5 / MeinaMix / Counterfeit / DreamShaper）都会强加装饰。

已删除不用的底模（SD 1.5 11 GB、Counterfeit V2.5 2 GB、DreamShaper 4 3.7 GB、MeinaMix V10），
当前只保留 Realistic Vision V5.1。

## 硬件限制

AMD RX 6650 XT：标称 8 GB 显存，但 DirectML 实际可用分配空间只有约 2 GB。
DirectML 累计可到约 10 GB 但会碎片化。

512x768 分辨率必须用 fp16，配合 `--lowvram` 启动参数。

## 对游戏侧的要求

**游戏代码不关心立绘怎么来的**，只要满足：

- RGBA PNG
- 透明背景
- 高度约 700px
- 放在 `FourYears/resource/character/<角色>/<表情>.png`

角色清单：吴鸿涛（男）、李君浩（女）、张瀚宇（女）。

## 换用其他方案的建议

如果要把立绘补完，按投入产出排序：

1. **找美术画**：最可靠，能保证三个角色风格统一、表情齐全
2. **LoRA + CUDA 显卡**：需要换掉 DirectML（AMD 卡上可考虑 ZLUDA 或双系统）
3. **在线生图服务**：Midjourney / NovelAI 等，但角色一致性仍需 LoRA 或参考图
4. **继续磨 DirectML**：当前已验证到此，再往上空间有限

无论哪种方式，最后都需要按上面「对游戏侧的要求」处理成 RGBA PNG 并裁到统一高度。
