//
// Created by lzx on 2025/8/27.
//

#include "yolocontrol.hpp"
#include "ui_yoloControl.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>
#include <QDir>


yoloControl::yoloControl(QWidget *parent) :
                                            QMainWindow(parent),
                                            ui(new Ui::yoloControl),
                                            m_trainProcess(nullptr) ,// 初始化进程指针为 nullptr
                                            m_convertProcess(nullptr)
{
  ui->setupUi(this);
}

yoloControl::~yoloControl() {
  delete ui;
  // 确保在程序退出时，如果进程还在运行，则终止它
  if (m_trainProcess && m_trainProcess->state() != QProcess::NotRunning) {
    m_trainProcess->kill();
    m_trainProcess->waitForFinished();
  }
}

void yoloControl::on_trainProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
  // 检查进程是否成功结束
  if (exitStatus == QProcess::NormalExit && exitCode == 0) {
    QMessageBox::information(this, "训练结束", "训练进程已成功完成。");

    // 使用正则表达式来精确匹配路径信息
    QRegularExpression regex("Results saved to (.*)");
    QRegularExpressionMatch match = regex.match(m_allOutput);

    if (match.hasMatch()) {
      QString relativePath = match.captured(1);

      // 统一路径分隔符
      relativePath.replace("\\", "/");

      QDir yoloDir(ui->m_lineYoloPath->text());
      QString resultsPath = yoloDir.absoluteFilePath(relativePath);

      QMessageBox::information(this, "训练完成", "训练结果已保存到：\n" + resultsPath);
      m_lastSavedPath = resultsPath;
    } else {
      QMessageBox::warning(this, "训练完成", "训练完成，但未能找到结果保存路径。");
    }
  } else {
    // 训练失败
    QMessageBox::critical(this, "训练失败", "训练进程意外终止，请检查日志。");
  }

  // 清理进程对象
  if (m_trainProcess) {
    m_trainProcess->deleteLater();
    m_trainProcess = nullptr;
  }
}

void yoloControl::on_m_btnYoloOpen_clicked() {
  QString dirPath = QFileDialog::getExistingDirectory(
      this,
      "选择YOLO项目目录",
      "F:\\Code\\gitee"
  );
  if (!dirPath.isEmpty()) {
    ui->m_lineYoloPath->setText(dirPath);
  }
}

void yoloControl::on_m_btnYamlOpen_clicked() {
  QString filePath = QFileDialog::getOpenFileName(
      this,
      "选择data.yaml文件",
      "F:\\Code\\database",
      "Yaml File (*.yaml);;All Files (*)"
  );
  if (!filePath.isEmpty()) {
    ui->m_lineYamlPath->setText(filePath);
  }
}
/*
void yoloControl::on_m_btnTrainPt_clicked() {
  QString yoloVersion = ui->m_comVersion->currentText();
  QString yoloPath = ui->m_lineYoloPath->text();
  QString yamlPath = ui->m_lineYamlPath->text();
  QString envName = ui->m_comEvnName->currentText();
  int epochs = ui->m_spinEpochs->value();
  int batchSize = ui->m_spinBatchSize->value();
  int imageSize = ui->m_spinImageSize->value();
  QString device = ui->m_comboDevice->currentText();

  QString command;
  QStringList args;
  QString deviceNum = (device == "GPU") ? "0" : "cpu";

  // 检查 Conda 环境名和项目路径是否为空
  if (envName.isEmpty()) {
    QMessageBox::warning(this, "缺少Conda环境", "请指定您的Conda环境。");
    return;
  }
  if (yoloPath.isEmpty()) {
    QMessageBox::warning(this, "路径错误", "请选择YOLO项目目录。");
    return;
  }

  if (yoloVersion == "YOLOv5") {
    command = "python";
    args << "train.py"
         << "--data" << yamlPath
         << "--epochs" << QString::number(epochs)
         << "--batch-size" << QString::number(batchSize)
         << "--imgsz" << QString::number(imageSize)
         << "--device" << deviceNum;
  } else if (yoloVersion == "YOLOv8" || yoloVersion == "YOLOv11") {
    command = "yolo";
    args << "detect"
         << "train"
         << "data=" + yamlPath
         << "epochs=" + QString::number(epochs)
         << "batch=" + QString::number(batchSize)
         << "imgsz=" + QString::number(imageSize)
         << "device=" + deviceNum;
  } else {
    QMessageBox::warning(this, "版本错误", "不支持的 YOLO 版本。请选择 YOLOv5、YOLOv8 或 YOLOv11。");
    return;
  }

  // 终止之前的进程，以防重复点击
  if (m_trainProcess && m_trainProcess->state() != QProcess::NotRunning) {
    m_trainProcess->kill();
    m_trainProcess->waitForFinished();
  }

  // 创建新的 QProcess 实例
  m_trainProcess = new QProcess(this);
  m_allOutput.clear();

  //  QProcess 的输出编码为 UTF-8
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  // 设置 UTF-8 编码
  env.insert("PYTHONUTF8", "1");
  env.insert("PYTHONIOENCODING", "utf-8");

#ifdef Q_OS_WIN
  // Windows 下设置控制台编码
  env.insert("PYTHONLEGACYWINDOWSSTDIO", "0");
  // 确保使用 UTF-8 代码页
  env.insert("CONSOLE_ENCODING", "utf-8");
#endif

  m_trainProcess->setProcessEnvironment(env);

  // ===== 使用定时器批量更新UI，减少开销 =====
  QTimer *updateTimer = new QTimer(this);
  QString *outputBuffer = new QString();  // 缓冲区

  connect(m_trainProcess, &QProcess::readyReadStandardOutput, this, [=]() {
    QString output = QString::fromUtf8(m_trainProcess->readAllStandardOutput());
    QRegularExpression ansiEscape("\x1B\\[[0-9;]*[A-Za-z]");
    output.remove(ansiEscape);

    m_allOutput += output;
    *outputBuffer += output;  // 先存到缓冲区
  });

  connect(m_trainProcess, &QProcess::readyReadStandardError, this, [=]() {
    QString errorOutput = QString::fromUtf8(m_trainProcess->readAllStandardError());
    QRegularExpression ansiEscape("\x1B\\[[0-9;]*[A-Za-z]");
    errorOutput.remove(ansiEscape);
    *outputBuffer += errorOutput;
  });

  // 每500ms更新一次UI，而不是每次输出都更新
  connect(updateTimer, &QTimer::timeout, this, [=]() {
    if (!outputBuffer->isEmpty()) {
      ui->m_editLog->append(*outputBuffer);
      outputBuffer->clear();
    }
  });

  connect(m_trainProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
            updateTimer->stop();
            updateTimer->deleteLater();
            delete outputBuffer;

            // 最后一次刷新缓冲区
            if (!outputBuffer->isEmpty()) {
              ui->m_editLog->append(*outputBuffer);
            }

            on_trainProcessFinished(exitCode, exitStatus);
          });

  updateTimer->start(500);  // 每500ms更新一次
                           // ===== 修改结束 =====

  // 连接信号和槽
  connect(m_trainProcess, &QProcess::readyReadStandardOutput, this, [=]() {
    QString output = QString::fromUtf8(m_trainProcess->readAllStandardOutput());

    // 去掉 ANSI 转义序列
    QRegularExpression ansiEscape("\x1B\\[[0-9;]*[A-Za-z]");
    output.remove(ansiEscape);

    m_allOutput += output;
    ui->m_editLog->append(output);
  });

  connect(m_trainProcess, &QProcess::readyReadStandardError, this, [=]() {
    QString errorOutput = QString::fromUtf8(m_trainProcess->readAllStandardError());

    QRegularExpression ansiEscape("\x1B\\[[0-9;]*[A-Za-z]");
    errorOutput.remove(ansiEscape);

    ui->m_editLog->append(errorOutput);
  });

  connect(m_trainProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          this, &yoloControl::on_trainProcessFinished);

  // 构建 Conda 环境激活命令
#ifdef Q_OS_WIN
  QString fullCommand = "cmd.exe";
  QStringList fullArgs;
  fullArgs << "/C" << "conda activate " + envName + " && " + command + " " + args.join(" ");
#else
  QString fullCommand = "bash";
  QStringList fullArgs;
  fullArgs << "-c" << "conda activate " + envName + " && " + command + " " + args.join(" ");
#endif

  // 设置工作目录并启动进程
  m_trainProcess->setWorkingDirectory(yoloPath);
  m_trainProcess->start(fullCommand, fullArgs);


  // ===== 添加这段代码 =====
#ifdef Q_OS_WIN
// 等待进程启动
if (m_trainProcess->waitForStarted(3000)) {
  // 获取进程 PID
  qint64 pid = m_trainProcess->processId();

  // 使用 Windows API 提升优先级
  HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, pid);
  if (hProcess) {
    // 设置为高优先级（或者使用 ABOVE_NORMAL_PRIORITY_CLASS）
    SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS);
    CloseHandle(hProcess);
    ui->m_editLog->append("✅ 已提升训练进程优先级");
  } else {
    ui->m_editLog->append("⚠️ 无法提升进程优先级");
  }
}
#endif


  QMessageBox::information(
      this,
      "训练命令",
      "Executing command: " + fullCommand + fullArgs.join(" ")
  );
}

*/
void yoloControl::on_m_btnTrainPt_clicked() {
  // ========== 参数准备 ==========
  QString yoloVersion = ui->m_comVersion->currentText();
  QString yoloPath = ui->m_lineYoloPath->text();
  QString yamlPath = ui->m_lineYamlPath->text();
  QString envName = ui->m_comEvnName->currentText();
  int epochs = ui->m_spinEpochs->value();
  int batchSize = ui->m_spinBatchSize->value();
  int imageSize = ui->m_spinImageSize->value();
  QString device = ui->m_comboDevice->currentText();

  QString command;
  QStringList args;
  QString deviceNum = (device == "GPU") ? "0" : "cpu";

  // 检查 Conda 环境名和项目路径是否为空
  if (envName.isEmpty()) {
    QMessageBox::warning(this, "缺少Conda环境", "请指定您的Conda环境。");
    return;
  }
  if (yoloPath.isEmpty()) {
    QMessageBox::warning(this, "路径错误", "请选择YOLO项目目录。");
    return;
  }

  // 根据 YOLO 版本构建命令
  if (yoloVersion == "YOLOv5") {
    command = "python";
    args << "-u"  // 无缓冲输出
         << "train.py"
         << "--data" << yamlPath
         << "--epochs" << QString::number(epochs)
         << "--batch-size" << QString::number(batchSize)
         << "--imgsz" << QString::number(imageSize)
         << "--device" << deviceNum;
  } else if (yoloVersion == "YOLOv8" || yoloVersion == "YOLOv11") {
    command = "yolo";
    args << "detect"
         << "train"
         << "data=" + yamlPath
         << "epochs=" + QString::number(epochs)
         << "batch=" + QString::number(batchSize)
         << "imgsz=" + QString::number(imageSize)
         << "device=" + deviceNum;
  } else {
    QMessageBox::warning(this, "版本错误", "不支持的 YOLO 版本。请选择 YOLOv5、YOLOv8 或 YOLOv11。");
    return;
  }

  // 终止之前的进程，以防重复点击
  if (m_trainProcess && m_trainProcess->state() != QProcess::NotRunning) {
    m_trainProcess->kill();
    m_trainProcess->waitForFinished();
  }

  // ========== 创建独立控制台窗口训练（高性能模式） ==========
#ifdef Q_OS_WIN
  // 创建临时批处理文件
  QString tempBat = QDir::temp().absoluteFilePath("yolo_train_" +
                                                  QString::number(QDateTime::currentMSecsSinceEpoch()) + ".bat");
  QFile batFile(tempBat);

  if (!batFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, "错误", "无法创建临时批处理文件！");
    return;
  }

  // Qt6 方式：使用 QTextStream 并设置编码
  QTextStream out(&batFile);
  out.setEncoding(QStringConverter::Utf8);  // Qt6 新方式

  out << "@echo off\n";
  out << "chcp 65001 >nul\n";  // 设置 UTF-8 编码
  out << "title YOLO Training - " << yoloVersion << "\n";
  out << "color 0A\n";  // 设置终端颜色（绿色文字）
  out << "cd /d \"" << yoloPath << "\"\n";
  out << "echo ========================================\n";
  out << "echo   YOLO 训练启动 - " << yoloVersion << "\n";
  out << "echo ========================================\n";
  out << "echo 工作目录: " << yoloPath << "\n";
  out << "echo Conda环境: " << envName << "\n";
  out << "echo 设备: " << device << "\n";
  out << "echo ========================================\n";
  out << "echo.\n";
  out << "call conda activate " << envName << "\n";
  out << "if errorlevel 1 (\n";
  out << "    echo ❌ Conda环境激活失败！\n";
  out << "    pause\n";
  out << "    exit /b 1\n";
  out << ")\n";
  out << "echo ✅ Conda环境激活成功\n";
  out << "echo.\n";
  out << "echo 执行命令: " << command << " " << args.join(" ") << "\n";
  out << "echo.\n";
  out << command << " " << args.join(" ") << "\n";
  out << "set TRAIN_EXIT_CODE=%errorlevel%\n";
  out << "echo.\n";
  out << "echo ========================================\n";
  out << "if %TRAIN_EXIT_CODE% EQU 0 (\n";
  out << "    echo ✅ 训练完成！退出码: %TRAIN_EXIT_CODE%\n";
  out << ") else (\n";
  out << "    echo ❌ 训练失败！退出码: %TRAIN_EXIT_CODE%\n";
  out << ")\n";
  out << "echo ========================================\n";
  out << "echo.\n";
  out << "echo 关闭窗口...\n";
  out << "pause >nul\n";
  out << "exit /b %TRAIN_EXIT_CODE%\n";
  batFile.close();

  // 创建 QProcess 实例
  m_trainProcess = new QProcess(this);
  m_trainProcess->setWorkingDirectory(yoloPath);

  // 设置创建新控制台窗口
  m_trainProcess->setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args) {
    args->flags |= CREATE_NEW_CONSOLE;  // 创建新控制台
    args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;  // 不重定向 I/O
  });

  // 记录开始时间
  QDateTime startTime = QDateTime::currentDateTime();
  ui->m_editLog->append("========================================");
  ui->m_editLog->append("🚀 训练启动 (高性能模式)");
  ui->m_editLog->append("版本: " + yoloVersion);
  ui->m_editLog->append("设备: " + device);
  ui->m_editLog->append("开始时间: " + startTime.toString("yyyy-MM-dd hh:mm:ss"));
  ui->m_editLog->append("========================================");

  // 连接完成信号
  connect(m_trainProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
            // 记录结束时间
            QDateTime endTime = QDateTime::currentDateTime();
            qint64 elapsed = startTime.secsTo(endTime);
            int hours = elapsed / 3600;
            int minutes = (elapsed % 3600) / 60;
            int seconds = elapsed % 60;

            ui->m_editLog->append("========================================");
            ui->m_editLog->append("结束时间: " + endTime.toString("yyyy-MM-dd hh:mm:ss"));
            ui->m_editLog->append(QString("总耗时: %1小时 %2分钟 %3秒")
                                      .arg(hours).arg(minutes).arg(seconds));
            ui->m_editLog->append("========================================");

            // 清理临时文件
            QFile::remove(tempBat);

            // 调用原有的完成处理
            on_trainProcessFinished(exitCode, exitStatus);
          });

  // 启动进程
  m_trainProcess->start("cmd.exe", QStringList() << "/C" << tempBat);

  if (m_trainProcess->waitForStarted(3000)) {
    ui->m_editLog->append("✅ 训练已在独立控制台窗口启动");
    ui->m_editLog->append("💡 提示: 训练日志将显示在新打开的控制台窗口中");
    ui->m_editLog->append("");

    QMessageBox::information(this, "训练启动成功",
                             "训练进程已在独立控制台窗口启动\n\n"
                             "✨ 优势:\n"
                             "• GPU利用率可达100%\n"
                             "• 训练速度与终端运行相同\n"
                             "• 实时查看详细训练日志\n\n"
                             "💡 训练完成后控制台窗口会提示，按任意键关闭");
  } else {
    QMessageBox::critical(this, "启动失败", "无法启动训练进程！");
    QFile::remove(tempBat);
    if (m_trainProcess) {
      m_trainProcess->deleteLater();
      m_trainProcess = nullptr;
    }
  }

#else
  // ========== Linux/Mac 平台 ==========
  QString terminalCmd;
  QStringList terminalArgs;

  // 尝试不同的终端
  if (QFile::exists("/usr/bin/gnome-terminal")) {
    terminalCmd = "gnome-terminal";
    terminalArgs << "--" << "bash" << "-c";
  } else if (QFile::exists("/usr/bin/konsole")) {
    terminalCmd = "konsole";
    terminalArgs << "-e" << "bash" << "-c";
  } else if (QFile::exists("/usr/bin/xterm")) {
    terminalCmd = "xterm";
    terminalArgs << "-e" << "bash" << "-c";
  } else {
    QMessageBox::warning(this, "终端未找到", "未找到可用的终端模拟器！");
    return;
  }

  QString bashScript = QString(
                           "source ~/anaconda3/etc/profile.d/conda.sh && "
                           "conda activate %1 && "
                           "cd '%2' && "
                           "echo '======================================' && "
                           "echo '  YOLO 训练启动 - %3' && "
                           "echo '======================================' && "
                           "echo '工作目录: %2' && "
                           "echo 'Conda环境: %1' && "
                           "echo '======================================' && "
                           "echo '' && "
                           "%4 %5 && "
                           "echo '' && "
                           "echo '======================================' && "
                           "echo '✅ 训练完成！' && "
                           "echo '======================================' && "
                           "read -p '按Enter关闭...'"
                           ).arg(envName, yoloPath, yoloVersion, command, args.join(" "));

  terminalArgs << bashScript;

  QDateTime startTime = QDateTime::currentDateTime();
  ui->m_editLog->append("========================================");
  ui->m_editLog->append("🚀 训练启动 (高性能模式)");
  ui->m_editLog->append("版本: " + yoloVersion);
  ui->m_editLog->append("设备: " + device);
  ui->m_editLog->append("开始时间: " + startTime.toString("yyyy-MM-dd hh:mm:ss"));
  ui->m_editLog->append("========================================");

  if (QProcess::startDetached(terminalCmd, terminalArgs)) {
    ui->m_editLog->append("✅ 训练已在独立终端窗口启动");
    QMessageBox::information(this, "训练启动成功",
                             "训练进程已在独立终端窗口启动\n这样可以获得最佳性能！");
  } else {
    QMessageBox::critical(this, "启动失败", "无法启动终端窗口！");
  }
#endif
}



void yoloControl::on_m_btnStop_clicked() {
  if (m_trainProcess && m_trainProcess->state() != QProcess::NotRunning) {

    m_trainProcess->terminate();

    if (!m_trainProcess->waitForFinished(3000)) { // 等 3 秒
      // 如果还没结束就强制杀掉
      m_trainProcess->kill();
      m_trainProcess->waitForFinished();
    }

    QMessageBox::information(this, "训练停止", "训练进程已被停止。");

    // 清理进程对象
    m_trainProcess->deleteLater();
    m_trainProcess = nullptr;
  } else {
    QMessageBox::warning(this, "提示", "当前没有正在运行的训练进程。");
  }
}

void yoloControl::on_m_btn2Onnx_clicked() {
  if (m_lastSavedPath.isEmpty()) {
    QMessageBox::warning(this, "错误", "请先完成训练，再进行模型导出。");
    return;
  }

  QString yoloVersion = ui->m_comVersion->currentText();
  QString envName = ui->m_comEvnName->currentText();
  QString weightsPath = QDir(m_lastSavedPath).absoluteFilePath("weights/best.pt");
  int imageSize = ui->m_spinImageSize->value();

  QString command;
  QStringList args;

  if (yoloVersion == "YOLOv5") {
    command = "python";
    args << "export.py"
         << "--weights" << weightsPath
         << "--imgsz" << QString::number(imageSize)
         << "--batch" << "1"
         << "--opset" << "11"
         << "--include" << "onnx";
  } else if (yoloVersion == "YOLOv8" || yoloVersion == "YOLOv11") {
    command = "yolo";
    args << "export"
         << "model=" + weightsPath
         << "format=onnx"
         << "imgsz=" + QString::number(imageSize)
         << "batch=1";
  } else {
    QMessageBox::warning(this, "版本错误", "当前 YOLO 版本不支持导出 ONNX。");
    return;
  }



#ifdef Q_OS_WIN
  QString fullCommand = "cmd.exe";
  QStringList fullArgs;
  fullArgs << "/C" << "conda activate " + envName + " && " + command + " " + args.join(" ");
#else
  QString fullCommand = "bash";
  QStringList fullArgs;
  fullArgs << "-c" << "conda activate " + envName + " && " + command + " " + args.join(" ");
#endif

  // 创建进程对象
  QProcess *proc = new QProcess(this);
  proc->setWorkingDirectory(ui->m_lineYoloPath->text());
  proc->setProcessChannelMode(QProcess::MergedChannels);

  connect(proc, &QProcess::readyReadStandardOutput, this, [=]() {
    QString output = proc->readAllStandardOutput();
    ui->m_editLog->append(output.trimmed());
  });
  connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int exitCode, QProcess::ExitStatus status) {
    if (status == QProcess::NormalExit && exitCode == 0) {
      ui->m_editLog->append("✅ ONNX 导出完成！");
      QMessageBox::information(
          this,
          "ONNX转换",
          "转换成功"
      );
    } else {
      ui->m_editLog->append("❌ ONNX 导出失败！");
      QMessageBox::critical(
          this,
          "ONNX转换",
          "转换成功"
      );
    }
    proc->deleteLater();
  });

  ui->m_editLog->append(">>> 开始执行 ONNX 导出...");
  proc->start(fullCommand, fullArgs);
}

void yoloControl::on_m_btn2Kmodel_clicked() {
  if (m_lastSavedPath.isEmpty()) {
    QMessageBox::warning(this, "错误", "请先完成训练，再进行模型导出。");
    return;
  }

  QString envName = ui->m_comEvnName->currentText();
  int imageSize = ui->m_spinImageSize->value();
  QString onnxPath = QDir(m_lastSavedPath).absoluteFilePath("weights/best.onnx");

  // 从 yaml 提取 val 路径
  QString yamlPath = ui->m_lineYamlPath->text();
  QString datasetPath;
  QFile yamlFile(yamlPath);
  if (yamlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString content = yamlFile.readAll();
    yamlFile.close();
    QRegularExpression re("val:\\s*(.*)");
    QRegularExpressionMatch m = re.match(content);
    if (m.hasMatch()) {
      datasetPath = m.captured(1).trimmed();
      datasetPath.replace("\\", "/");
    }
  }
  if (datasetPath.isEmpty()) {
    QMessageBox::warning(this, "错误", "未能在 data.yaml 中找到 val 数据集路径。");
    return;
  }

  QString command = "python";
  QStringList args;
  args << "to_kmodel.py"
       << "--target" << "k230"
       << "--model" << onnxPath
       << "--dataset" << datasetPath
       << "--input_width" << QString::number(imageSize)
       << "--input_height" << QString::number(imageSize)
       << "--ptq_option" << "0";

#ifdef Q_OS_WIN
  QString fullCommand = "cmd.exe";
  QStringList fullArgs;
  fullArgs << "/C" << "conda activate " + envName + " && " + command + " " + args.join(" ");
#else
  QString fullCommand = "bash";
  QStringList fullArgs;
  fullArgs << "-c" << "conda activate " + envName + " && " + command + " " + args.join(" ");
#endif

  QProcess *proc = new QProcess(this);
  proc->setWorkingDirectory(ui->m_lineYoloPath->text());
  proc->setProcessChannelMode(QProcess::MergedChannels);

  connect(proc, &QProcess::readyReadStandardOutput, this, [=]() {
    QString output = proc->readAllStandardOutput();
    ui->m_editLog->append(output.trimmed());
  });
  connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int exitCode, QProcess::ExitStatus status) {
    if (status == QProcess::NormalExit && exitCode == 0) {
      ui->m_editLog->append("✅ KModel 转换完成！");
      QMessageBox::information(
          this,
          "KModel转换",
          "转换成功"
      );

    } else {
      ui->m_editLog->append("❌ KModel 转换失败！");
      QMessageBox::critical(
          this,
          "KModel转换",
          "转换失败"
      );
    }
    proc->deleteLater();
  });

  ui->m_editLog->append(">>> 开始执行 KModel 转换...");
  proc->start(fullCommand, fullArgs);
}

void yoloControl::on_m_btnPtOpen_clicked() {
  QString filePath = QFileDialog::getOpenFileName(
      this,
      "选择权重文件 (*.pt)",
      "F:\\Code\\gitee",
      "PyTorch Weights (*.pt);;All Files (*)"
  );
  if (!filePath.isEmpty()) {
    ui->m_linePtPath->setText(filePath);
    QMessageBox::information(this, "选择成功", "已选择权重文件：\n" + filePath);
  }
}

void yoloControl::on_m_btnValOpen_clicked() {
  QString dirPath = QFileDialog::getExistingDirectory(
      this,
      "选择验证集目录 (val)",
      "F:\\Code\\database"
  );
  if (!dirPath.isEmpty()) {
    ui->m_lineValPath->setText(dirPath);
    QMessageBox::information(this, "选择成功", "已选择验证集目录：\n" + dirPath);
  }
}

void yoloControl::on_m_btnDir2Onnx_clicked() {
  QString yoloVersion = ui->m_comVersion->currentText();
  QString yoloPath    = ui->m_lineYoloPath->text();
  QString ptPath      = ui->m_linePtPath->text();
  QString envName     = ui->m_comEvnName->currentText();
  int imageSize       = ui->m_spinImageSize->value();

  if (ptPath.isEmpty()) {
    QMessageBox::warning(this, "缺少参数", "请先选择 .pt 权重文件。");
    return;
  }
  // ---- 生成 ONNX 输出路径（和 .pt 同目录）----
  QString onnxPath = QFileInfo(ptPath).absolutePath() + "/best.onnx";


  QString command;
  QStringList args;

  // -------------------------------------------
  //        YOLOv5 导出：强制 opset=11
  // -------------------------------------------
  if (yoloVersion == "YOLOv5") {
    command = "python";
    args << "export.py"
         << "--weights" << ptPath
         << "--imgsz" << QString::number(imageSize)
         << "--batch" << "1"
         << "--opset" << "11"           // ⭐ 防止 OpenCV 加载失败
         << "--include" << "onnx";
  }

  // -------------------------------------------
  //   YOLOv8/YOLOv11 导出：同样加入 opset=11
  // -------------------------------------------
  else if (yoloVersion == "YOLOv8" || yoloVersion == "YOLOv11") {
    command = "yolo";
    args << "export"
         << "model=" + ptPath
         << "format=onnx"
         << "opset=11"                  // ⭐ 必须加
         << "imgsz=" + QString::number(imageSize)
         << "batch=1";
  }

  else {
    QMessageBox::warning(this, "版本错误", "不支持的 YOLO 版本，请选择 YOLOv5/YOLOv8/YOLOv11。");
    return;
  }

  // --------------------------------------------------------
  //       ⭐ 第二步：自动执行 onnxsim 将 best.onnx 简化
  // --------------------------------------------------------
  // 添加 onnxsim 步骤（使用绝对路径）
  QString simCommand = QString("python -m onnxsim %1 %1").arg(onnxPath);


#ifdef Q_OS_WIN
  QString fullCommand = "cmd.exe";
  QString fullScript =
      QString("conda activate %1 && %2 %3 && %4")
          .arg(envName)
          .arg(command)
          .arg(args.join(" "))
          .arg(simCommand);

  QStringList fullArgs = { "/C", fullScript };
#else
  QString fullCommand = "bash";
  QString fullScript =
      QString("source ~/anaconda3/etc/profile.d/conda.sh && "
              "conda activate %1 && %2 %3 && %4")
          .arg(envName)
          .arg(command)
          .arg(args.join(" "))
          .arg(simCmd);

  QStringList fullArgs = { "-c", fullScript };
#endif

  // 启动进程
  m_convertProcess = new QProcess(this);
  m_convertProcess->setWorkingDirectory(yoloPath);
  m_convertProcess->setProcessChannelMode(QProcess::MergedChannels);

  connect(m_convertProcess, &QProcess::readyReadStandardOutput, this, [=]() {
    ui->m_editLog->append(QString::fromUtf8(m_convertProcess->readAllStandardOutput()));
  });

  connect(m_convertProcess, &QProcess::readyReadStandardError, this, [=]() {
    ui->m_editLog->append(QString::fromUtf8(m_convertProcess->readAllStandardError()));
  });

  connect(m_convertProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          this, [=](int exitCode, QProcess::ExitStatus exitStatus) {

            if (exitStatus == QProcess::NormalExit && exitCode == 0) {

              QString finalOnnx = QDir(ui->m_lineYoloPath->text())
                                      .filePath("weights/best.onnx");

              ui->m_editLog->append("\n🎉 简化 ONNX 完成： " + finalOnnx);

              QMessageBox::information(this, "转换完成",
                                       "ONNX 导出 + 简化成功！\n最终文件：best.onnx");
            }
            else {
              QMessageBox::critical(this, "转换失败", "ONNX 导出失败，请查看日志。");
            }

            m_convertProcess->deleteLater();
            m_convertProcess = nullptr;
          });

  ui->m_editLog->append("执行命令: " + fullCommand + " " + fullScript);
  m_convertProcess->start(fullCommand, fullArgs);
}


void yoloControl::on_m_Dir2Kmodel_clicked() {
  QString yoloVersion = ui->m_comVersion->currentText();
  QString yoloPath    = ui->m_lineYoloPath->text();
  QString valPath     = ui->m_lineValPath->text();
  QString envName     = ui->m_comEvnName->currentText();
  int imageSize       = ui->m_spinImageSize->value();

  // ONNX 路径自动推算
  QString onnxPath = QFileInfo(ui->m_linePtPath->text()).absolutePath() + "/best.onnx";

  if (valPath.isEmpty()) {
    QMessageBox::warning(this, "缺少参数", "请先选择验证集目录。");
    return;
  }

  QString command = "python";
  QStringList args;
  args << "to_kmodel.py"
       << "--target" << "k230"
       << "--model" << onnxPath
       << "--dataset" << valPath
       << "--input_width" << QString::number(imageSize)
       << "--input_height" << QString::number(imageSize)
       << "--ptq_option" << "0";

#ifdef Q_OS_WIN
  QString fullCommand = "cmd.exe";
  QStringList fullArgs;
  fullArgs << "/C" << "conda activate " + envName + " && " + command + " " + args.join(" ");
#else
  QString fullCommand = "bash";
  QStringList fullArgs;
  fullArgs << "-c" << "conda activate " + envName + " && " + command + " " + args.join(" ");
#endif

  m_convertProcess = new QProcess(this);
  m_convertProcess->setWorkingDirectory(yoloPath);

  connect(m_convertProcess, &QProcess::readyReadStandardOutput, this, [=]() {
    QString output = QString::fromUtf8(m_convertProcess->readAllStandardOutput());
    ui->m_editLog->append(output);
  });
  connect(m_convertProcess, &QProcess::readyReadStandardError, this, [=]() {
    QString output = QString::fromUtf8(m_convertProcess->readAllStandardError());
    ui->m_editLog->append(output);
  });
  connect(m_convertProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitStatus == QProcess::NormalExit && exitCode == 0) {
              QMessageBox::information(this, "转换完成", "Kmodel 转换成功！");
            } else {
              QMessageBox::critical(this, "转换失败", "Kmodel 转换失败，请检查日志。");
            }
            m_convertProcess->deleteLater();
            m_convertProcess = nullptr;
          });

  ui->m_editLog->append("执行命令: " + fullCommand + " " + fullArgs.join(" "));
  m_convertProcess->start(fullCommand, fullArgs);
}


void yoloControl::on_m_btnDownload_clicked()
{
  QFile resFile(":/doc/yolo.md");
  if (!resFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "错误", "无法读取内置文档！");
    return;
  }

  QString mdContent = resFile.readAll();
  resFile.close();

  QString savePath = QFileDialog::getSaveFileName(
      this,
      "保存说明文档",
      "yolo.md",
      "Markdown 文件 (*.md)"
  );

  if (savePath.isEmpty()) return;

  QFile out(savePath);
  if (!out.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "错误", "无法写入文件！");
    return;
  }

  out.write(mdContent.toUtf8());
  out.close();

  QMessageBox::information(this, "完成", "文档已成功保存！");
}
/**
 * 把资源文件中的脚本释放到 yoloPath 下
 */
QString yoloControl::extractScript(const QString& yoloPath, const QString& resourceName)
{
  QString fileName = QFileInfo(resourceName).fileName();     // 提取出 test_pt.py
  QString dst = QDir(yoloPath).absoluteFilePath(fileName);   // 写到 yoloPath 下

  QFile::remove(dst);  // 覆盖旧版本

  QFile res(resourceName);       // 打开 qrc 资源
  if (!res.open(QIODevice::ReadOnly)) {
    qDebug() << "Res not found:" << resourceName;
    return "";
  }

  QFile out(dst);
  if (!out.open(QIODevice::WriteOnly)) {
    qDebug() << "Cannot write to:" << dst;
    return "";
  }

  out.write(res.readAll());
  out.close();
  return dst;
}

void yoloControl::on_m_btnTestPt_clicked()
{
  QString envName = ui->m_comEvnName->currentText();
  QString yoloPath = ui->m_lineYoloPath->text();

  // 释放 test_pt.py
  QString scriptPath = extractScript(yoloPath, ":/scripts/test_pt.py");
  if (scriptPath.isEmpty()) {
    ui->m_editLog->append("❌ 无法释放 test_pt.py！");
    return;
  }

  // 刚训练的模型
  QString ptPath = QDir(m_lastSavedPath).absoluteFilePath("weights/best.pt");

  //用户传入的模型
  QString ptUserPath      = ui->m_linePtPath->text();

  if (!QFile::exists(ptPath)  and !QFile::exists(ptUserPath) ) {
    QString version = ui->m_comVersion->currentText();
    QString weights;

    if (version == "YOLOv5")      weights = "yolov5s.pt";
    else if (version == "YOLOv8") weights = "yolov8n.pt";
    else                          weights = "yolo11n.pt";

    //如果两个都没有就是官方模型
    ptPath = QDir(yoloPath).absoluteFilePath(weights);
  }

  //用户传入模型就用用户的
  if(QFile::exists(ptUserPath)){
    ptPath = ptUserPath;
  }

#ifdef Q_OS_WIN
  QString fullCommand = "cmd.exe";

  // --- 你想要的写法 ---
  QString version = ui->m_comVersion->currentText(); // YOLOv5 / YOLOv8 / YOLOv11

  QString script = QString(
                       "conda activate %1 && python %2 %3 %4"
                       ).arg(envName, scriptPath, ptPath, version);

  QStringList args = { "/C", script };
#else
  QString fullCommand = "bash";
  QString script = QString("source ~/anaconda3/etc/profile.d/conda.sh && "
                           "conda activate \"%1\" && python \"%2\" \"%3\"")
                       .arg(envName, scriptPath, ptPath);

  QStringList args = { "-c", script };
#endif

  QProcess* proc = new QProcess(this);
  proc->setWorkingDirectory(yoloPath);
  proc->setProcessChannelMode(QProcess::MergedChannels);

  connect(proc, &QProcess::readyReadStandardOutput, this, [=]() {
    ui->m_editLog->append(proc->readAllStandardOutput());
  });
  connect(proc, &QProcess::readyReadStandardError, this, [=]() {
    ui->m_editLog->append(proc->readAllStandardError());
  });

  proc->start(fullCommand, args);
  ui->m_editLog->append(">>> 🚀 开始测试 PT 模型 ...");
}

void yoloControl::on_m_btnTestOnnx_clicked()
{
  QString envName = ui->m_comEvnName->currentText();
  QString yoloPath = ui->m_lineYoloPath->text();

  // 释放 test_onnx.py
  QString scriptPath = extractScript(yoloPath, ":/scripts/test_onnx.py");
  if (scriptPath.isEmpty()) {
    ui->m_editLog->append("❌ 无法释放 test_onnx.py！");
    return;
  }

  //默认也可以传一个onnx文件
  QString onnxPath = ui->m_linePtPath->text();
  // ONNX 路径自动推算
  QFileInfo info(onnxPath);

  QString onnxUserPath = info.absolutePath() + "/" + info.completeBaseName() + ".onnx";

  if (!QFile::exists(onnxPath) and !QFile::exists(onnxUserPath)) {
    QMessageBox::warning(this, "缺少 ONNX", "未找到 best.onnx，请先导出 ONNX。");
    return;
  }
  if(QFile::exists(onnxUserPath)){
    onnxPath = onnxUserPath;
  }

#ifdef Q_OS_WIN
  QString fullCommand = "cmd.exe";

  // --- 你想要的写法 ---
  QString script = QString(
                       "conda activate %1 && python %2 %3"
                       ).arg(envName, scriptPath, onnxPath);

  QStringList args = { "/C", script };
#else
  QString fullCommand = "bash";
  QString script = QString("source ~/anaconda3/etc/profile.d/conda.sh && "
                           "conda activate \"%1\" && python \"%2\" \"%3\"")
                       .arg(envName, scriptPath, onnxPath);

  QStringList args = { "-c", script };
#endif

  QProcess* proc = new QProcess(this);
  proc->setWorkingDirectory(yoloPath);
  proc->setProcessChannelMode(QProcess::MergedChannels);

  connect(proc, &QProcess::readyReadStandardOutput, this, [=]() {
    ui->m_editLog->append(proc->readAllStandardOutput());
  });
  connect(proc, &QProcess::readyReadStandardError, this, [=]() {
    ui->m_editLog->append(proc->readAllStandardError());
  });

  proc->start(fullCommand, args);
  ui->m_editLog->append(">>> 🚀 开始测试 ONNX 模型 ...");
}




