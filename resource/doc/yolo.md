
---

# 📘 YOLO 训练与 KModel 转换使用指南

---

## 🔧 1. 创建 Conda 环境

```bash
conda create -n yolo_to_kmodel python==3.10 -y
conda activate yolo_to_kmodel
```

---

## 📁 2. 克隆 YOLO 项目并配置环境

```bash
git clone https://github.com/ultralytics/yolov5.git
cd yolov5
pip install -r requirements.txt
```

---

## ⚡ 3. 安装 CUDA（用于加速训练）

> 使用 `nvidia-smi` 查看显卡驱动对应的 CUDA 版本。

```bash
pip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu121
```

---

## 🏋️ 4. 训练 YOLOv5 模型

> `--data` 对应你的数据集配置文件路径。

```bash
python train.py --weight yolov5n.pt --cfg models/yolov5n.yaml --data F:\\Code\\database\\2025_8_26\\data.yaml --epochs 300 --batch-size 8 --imgsz 320 --device 0
```

---

## 🔄 5. ONNX 转换环境配置（Windows）

### 📌 5.1 安装 dotnet-7（必须）

下载 dotnet SDK：

```
https://builds.dotnet.microsoft.com/dotnet/Sdk/7.0.317/dotnet-sdk-7.0.317-win-x64.exe
```

安装后需配置环境变量：

```
DOTNET_ROOT=C:\Program Files\dotnet
```

---

### 📌 5.2 安装 nncase 及相关依赖

> Windows 支持使用 pip 安装 nncase；
> **nncase-kpu 必须从 releases 手动下载**：
> [https://github.com/kendryte/nncase/releases](https://github.com/kendryte/nncase/releases)

在下载的目录安装：

```bash
pip install nncase_kpu-2.9.0-py2.py3-none-win_amd64.whl 
```

安装其他依赖：

```bash
pip install onnxruntime==1.21.1
pip install onnxsim==0.4.36
pip install onnx==1.17.0
```

如果冲突，可卸载重装：

```bash
pip uninstall onnxruntime
pip uninstall onnxsim
```

---

### 📌 5.3 下载官方转换工具

```bash
wget https://kendryte-download.canaan-creative.com/developer/k230/yolo_files/test_yolov5.zip
unzip test_yolov5.zip
```

---

## 📊 6. 模型导出与量化数据集准备

> 推荐使用训练数据集的 **20% 做量化**。

### 6.1 导出 ONNX

```bash
python export.py --weight runs/train/exp12/weights/best.pt --imgsz 320 --batch 1 --include onnx
```

### 6.2 进入转换工具目录

```bash
cd test_yolov5/detect
```

### 6.3 执行 YOLO → KModel 转换

```bash
python to_kmodel.py --target k230 --model runs/train/exp12/weights/best.onnx --dataset F:\\Code\\database\\2025_8_26\\images\\val --input_width 320 --input_height 320 --ptq_option 0
```

---

## 🧰 7. 手动输入命令示例（使用自动化后无需）

```bash
python train.py --weight yolov5n.pt --cfg models/yolov8.yaml --data D:/code/tree/num_yolo.yaml --epochs 1 --batch-size 8 --imgsz 320 --device 0

python export.py --weight F:/Code/gitee/yolo_test/yolov5/runs/train/exp10/weights/best.pt --imgsz 320 --batch 1 --include onnx

python to_kmodel.py --target k230 --model F:/Code/gitee/yolo_test/yolov5/runs/train/exp10/weights/best.onnx --dataset F:/Code/database/2025_8_6/images/  --input_width 320 --input_height 320 --ptq_option 0

set DOTNET_ROOT=C:\Program Files\dotnet

python to_kmodel.py --target k230 --model runs/train/exp/weights/best.onnx --dataset F:\\Code\\database\\2025_5_26\\test_val  --input_width 320 --input_height 320 --ptq_option 0
```

---

## 🐂 8. YOLOv8 训练与导出

### 8.1 训练 YOLOv8

```bash
yolo detect train model=./yolov8n.pt data=F:\\Code\\database\\2025_8_26\\data.yaml epochs=250 batch=8 imgsz=320 device=0

yolo detect train model=./yolov8n.pt data=D:/code/tree/num_yolo.yaml epochs=200 batch=8 imgsz=320 device=0
yolo detect train model=./yolov8n.pt data=F:/Code/database/tree/num_yolo.yaml epochs=200 batch=8 imgsz=320 device=0
```

### 8.2 导出 YOLOv8 ONNX

```bash
yolo export model=F:/Code/gitee/ultralytics/runs/detect/train11/weights/best.pt format=onnx imgsz=320 batch=1
```

### 8.3 YOLOv8 → KModel

```bash
python to_kmodel.py --target k230 --model F:/Code/gitee/ultralytics/runs/detect/train11/weights/best.onnx --dataset F:\\Code\\database\\2025_8_26\\images\\val  --input_width 320 --input_height 320 --ptq_option 0
```

若模型输入为 **224×224**：

```
--input_width 224 --input_height 224
```

---

## 🔬 9. 模型测试

### 9.1 生成输入 bin

```bash
python save_bin.py --image ./2025_7_8/000.jpg --input_width 320 --input_height 320
```

### 9.2 测试 ONNX 模型

```bash
python simulate.py --model ./2025_7_8/best.onnx --model_input onnx_input_float32.bin --kmodel ./2025_7_8/best.kmodel --kmodel_input kmodel_input_uint8.bin --input_width 320 --input_height 320
```


