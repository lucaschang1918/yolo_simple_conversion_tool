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
    env.insert("PYTHONUTF8", "1");
    m_trainProcess->setProcessEnvironment(env);

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
    QMessageBox::information(
            this,
            "训练命令",
            "Executing command: " + fullCommand + fullArgs.join(" ")
    );
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

    QString command;
    QStringList args;

    if (yoloVersion == "YOLOv5") {
        command = "python";
        args << "export.py"
             << "--weights" << ptPath
             << "--imgsz" << QString::number(imageSize)
             << "--batch" << "1"
             << "--include" << "onnx";
    } else if (yoloVersion == "YOLOv8" || yoloVersion == "YOLOv11") {
        command = "yolo";
        args << "export"
             << "model=" + ptPath
             << "format=onnx"
             << "imgsz=" + QString::number(imageSize)
             << "batch=1";
    } else {
        QMessageBox::warning(this, "版本错误", "不支持的 YOLO 版本，请选择 YOLOv5/YOLOv8/YOLOv11。");
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
                    QMessageBox::information(this, "转换完成", "ONNX 转换成功！");
                } else {
                    QMessageBox::critical(this, "转换失败", "ONNX 转换失败，请检查日志。");
                }
                m_convertProcess->deleteLater();
                m_convertProcess = nullptr;
            });

    ui->m_editLog->append("执行命令: " + fullCommand + " " + fullArgs.join(" "));
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
  QFile resFile(":/yolo.md");
  if (!resFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(this, "错误", "无法读取内置文档！");
    return;
  }

  QString mdContent = resFile.readAll();
  resFile.close();

  QString savePath = QFileDialog::getSaveFileName(
      this,
      "保存说明文档",
      "yolo_help.md",
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






